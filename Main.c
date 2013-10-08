
#include "SDL.h"

void GFX_Init() {
   int status;
   status = SDL_Init(SDL_INIT_VIDEO);
   if (status != 0) {
      Die("Couldn't init SDL.")
   }
   SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUFFER);
}

void GFX_Quit() {
   SDL_Quit();
}

int main(int argc, char** argv) {
   GFX_Init();
   GFX_Quit();
}

