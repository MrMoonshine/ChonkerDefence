#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define TEST_FONT "/home/david/Programmieren/C/ChonkerDefenceAssets/assets/textures/text.ttf"
#define FONT_DEFAULT TEST_FONT

typedef struct{
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Rect rect;
    SDL_Texture *texture;
    SDL_Color colour;
} Font;

void fontCreate(Font* font, const char* file, unsigned short size, SDL_Colour colour ,SDL_Renderer *renderer);
void fontRender(Font* font, const char* msg);
void fontDraw(Font* font, int x, int y);
void fontDestroy(Font* font); 
