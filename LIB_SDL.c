
#include <SDL/SDL.h>

#define LIB_END_MACRO

#ifndef ABS
#define ABS(x) ((x)<0?-(x):(x))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

int LIB_Init() {
   SDL_Init(SDL_INIT_TIMER);
   return 1;
}

int LIB_Done() {
   SDL_Quit();
}
