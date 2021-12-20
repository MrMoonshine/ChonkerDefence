#include "text.h"

void fontCreate(Font* font, const char* file, unsigned short size, SDL_Colour colour ,SDL_Renderer *renderer){
    if(font == NULL)
        return;
    
    font->font = TTF_OpenFont(TEST_FONT, size);
    font->renderer = renderer;
    font->colour = colour;
    
    font->texture = NULL;
}

void fontRender(Font* font, const char* msg){
    fontDestroy(font);
    SDL_Surface *surface;
    surface = TTF_RenderText_Solid(font->font, msg, font->colour);
    font->texture = SDL_CreateTextureFromSurface(font->renderer, surface);
    //IMG_SavePNG(surface, "/home/david/oida.png");
    font->rect.x = 0;
    font->rect.y = 0;
    font->rect.w = surface->w;
    font->rect.h = surface->h;
    SDL_FreeSurface(surface);
}

void fontDraw(Font* font, int x, int y){
    font->rect.y = y;
    font->rect.x = x;
    SDL_RenderCopy(font->renderer, font->texture, NULL, &font->rect);
}

void fontDestroy(Font* font){
    if(font->texture)
        SDL_DestroyTexture(font->texture);
}
