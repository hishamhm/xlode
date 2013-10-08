
#define SDLCALL
#include <SDL/SDL_mixer.h>

typedef Mix_Music MUS_Song;

void MUS_Init() {
}

MUS_Song* MUS_Load(char* filename) {
   return Mix_LoadMUS(filename);
}

void MUS_Free(MUS_Song* song) {
   Mix_FreeMusic(song);
}

void MUS_Play(MUS_Song* song, int doLoop) {
   Mix_PlayMusic(song, (doLoop == 1 ? -1 : 0));
}

void MUS_Stop(MUS_Song* song) {
   Mix_HaltMusic();
}

void MUS_SetVolume(int volume) {
   Mix_VolumeMusic(volume);
}
