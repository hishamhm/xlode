
#define SDLCALL
#include <SDL/SDL_mixer.h>

typedef Mix_Chunk SFX_Sample;

void SFX_Init() {
   Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 4096);
   Mix_AllocateChannels(32);
   Mix_ReserveChannels(4);
}

void SFX_Quit() {
}

void SFX_Free(SFX_Sample* sample) {
   Mix_FreeChunk(sample);
}

SFX_Sample* SFX_Load(char* filename) {
   return Mix_LoadWAV(filename);
}

void SFX_Play(SFX_Sample* sample, int volume, int pan, int channel, int loop) {
   SDL_LockAudio();
   Mix_SetPanning(channel, 255-pan, pan);
   Mix_Volume(channel, volume);
   Mix_PlayChannel(channel, sample, loop);
   SDL_UnlockAudio();
}

void SFX_Stop(int channel) {
   SDL_LockAudio();
   Mix_HaltChannel(channel);
   SDL_UnlockAudio();
}
