
#include <SDL/SDL.h>

typedef SDL_Surface GFX_Image;

SDL_Surface* framebuffer;

void GFX_Done() {
}

void GFX_Free(GFX_Image* image) {
   SDL_FreeSurface(image);
}

GFX_Image* GFX_Load(char* filename) {
   GFX_Image* image;
   image = SDL_LoadBMP(filename);
   SDL_SetColorKey(image, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0xff00ff);
   return image;
}

GFX_Image* GFX_CreateBlank(int w, int h) {
   GFX_Image* image;
   image = SDL_CreateRGBSurface(0, w, h, 16, 0, 0, 0, 0);
   return image;
}

void GFX_Clear(GFX_Image* image) {
   SDL_FillRect(image, NULL, 0x00000000);
}

void GFX_Draw(GFX_Image* dest, GFX_Image* src, int destX, int destY) {
   SDL_Rect rect;
   rect.x = destX;
   rect.y = destY;
   SDL_BlitSurface(src, NULL, dest, &rect);
}

void GFX_Blit(GFX_Image* src, GFX_Image* dest, int srcX, int srcY, int destX, int destY, int srcW, int srcH) {
   SDL_Rect srcRect;
   SDL_Rect destRect;
   srcRect.x = srcX;
   srcRect.y = srcY;
   srcRect.w = srcW;
   srcRect.h = srcH;
   destRect.x = destX;
   destRect.y = destY;
   SDL_BlitSurface(src, &srcRect, dest, &destRect);
}

void GFX_FillRectangle(GFX_Image* dest, int x1, int y1, int x2, int y2, int color) {
   SDL_Rect rect;
   rect.x = x1;
   rect.y = y1;
   rect.w = x2-x1+1;
   rect.h = y2-y1+1;
   SDL_FillRect(dest, &rect, color);
}

int GFX_Init(int resX, int resY, int depth) {
   SDL_Init(SDL_INIT_VIDEO);
   framebuffer = SDL_SetVideoMode(resX, resY, depth, SDL_HWSURFACE|SDL_DOUBLEBUF);
   return 1;
}

GFX_Image* GFX_Screen() {
   return SDL_GetVideoSurface();
}

void GFX_DisplayBuffer(GFX_Image* image) {
   SDL_BlitSurface(image, NULL, framebuffer, NULL);
   SDL_Flip(framebuffer);
}

void GFX_UpdateScreen() {
   SDL_Flip(framebuffer);
}

void GFX_ToggleFullScreen() {
   // 2013-10-08 This got me in a video mode that I could not get out of!
   // SDL_WM_ToggleFullScreen(framebuffer);
}
