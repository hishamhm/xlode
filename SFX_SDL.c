
#include <allegro.h>

typedef SDL_AudioSpec SFX_Sample;

SDL_AudioSpec* hardwareSpec;

void _SFX_AudioCallback(void* userdata, Uint8 *stream, int len) {
}

void SFX_Init() {
   SDL_AudioSpec *desired, *obtained;
   SDL_Init(SDL_INIT_AUDIO);
   desired = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));
   obtained = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));
   desired->freq = 22050;
   desired->format = AUDIO_S16LSB;
   desired->channels = 1;
   desired->samples = 8192;
   desired->callback = _SFX_AudioCallback;
   desired->userdata = NULL;
   if (SDL_OpenAudio(desired, obtained) < 0) {
      fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
      exit(-1);
   }
   free(desired);
   hardwareSpec = obtained;
   SDL_PauseAudio(0);

   reserve_voices(16, -1);
   install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
   set_volume(255, -1);
}

void SFX_Quit() {
   free(audioSpec);
}

void SFX_Free(SFX_Sample* sample) {
   destroy_sample(sample);
}

SFX_Sample* SFX_Load(char* filename) {
   return load_sample(filename);
}

void SFX_Play(SFX_Sample* sample, int volume, int pan, int something, int loop) {
   play_sample(sample, volume, pan, something, loop);
}

void SFX_Stop(SFX_Sample* sample) {
   stop_sample(sample);
}
