
#include <SDL/SDL.h>

#define IO_KEY_ESC SDLK_ESCAPE
#define IO_KEY_UP SDLK_UP
#define IO_KEY_DOWN SDLK_DOWN
#define IO_KEY_LEFT SDLK_LEFT
#define IO_KEY_RIGHT SDLK_RIGHT
#define IO_KEY_I SDLK_i
#define IO_KEY_J SDLK_j
#define IO_KEY_K SDLK_k
#define IO_KEY_L SDLK_l
#define IO_KEY_O SDLK_o
#define IO_KEY_U SDLK_u
#define IO_KEY_C SDLK_c
#define IO_KEY_H SDLK_h
#define IO_KEY_T SDLK_t
#define IO_KEY_N SDLK_n
#define IO_KEY_G SDLK_g
#define IO_KEY_R SDLK_r
#define IO_KEY_F SDLK_f
#define IO_KEY_V SDLK_v
#define IO_KEY_S SDLK_s
#define IO_KEY_SPACE SDLK_SPACE
#define IO_KEY_ENTER SDLK_RETURN
#define IO_KEY_RSHIFT SDLK_RSHIFT
#define IO_KEY_END SDLK_END

typedef char IO_Key;

void IO_Init() {
}

void IO_Done() {
}

void IO_PollKeyboard() {
}

int IO_ReadKey() {
   SDL_Event event;
   SDL_KeyboardEvent* kevent;
   event.type = SDL_USEREVENT;
   while (event.type != SDL_KEYUP) {
      SDL_WaitEvent(&event);
   }
   kevent = (SDL_KeyboardEvent*) &event;
   return kevent->keysym.sym;
}

void IO_Wait(int time) {
   SDL_Delay(time);
}

void IO_ClearKeyboardBuffer() {
   SDL_Event event;
   while (SDL_PollEvent(&event)) {
   }
}

int IO_ReadExtKey() {
   SDL_Event event;
   SDL_KeyboardEvent* kevent;
   event.type = SDL_USEREVENT;
   while (event.type != SDL_KEYUP) {
      SDL_WaitEvent(&event);
   }
   kevent = (SDL_KeyboardEvent*) &event;
   return kevent->keysym.sym;
}

IO_Key* IO_GetKeyboardState() {
   int numkeys;
   SDL_PumpEvents();
   return SDL_GetKeyState(&numkeys);
}
