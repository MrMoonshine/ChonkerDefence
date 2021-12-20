#pragma once
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>

typedef struct{
    SDL_Renderer* renderer;
    SDL_Rect destination;
    SDL_Texture* texture;
    bool valid;
} Image;

typedef struct{
    uint32_t period;        //In Milliseconds
    uint32_t lastSample;
    unsigned short frameNumber;
    unsigned short frameCount;
}AnimationInfo;

//Load an Image
int textureLoad(Image *image, SDL_Renderer* renderer, const char* filename);
//Destroy an Image
int textureDestroy(Image *image);
//Draw an Image
void textureDraw(Image *image, int x, int y, SDL_RendererFlip flip, AnimationInfo *animation);
void textureDrawRotated(Image *image, int x, int y, SDL_RendererFlip flip, AnimationInfo *animation, const double angle);
//Mata Info Functions
SDL_Point textureGetSize(Image *image);
