#pragma once
#include "image.h"
#include <SDL.h>

//In pixels
#define NODE_SIZE 48

typedef SDL_Point Nodepos;
typedef enum{
    REAR,
    RIGHT,
    FRONT,
    LEFT
} Rotation;
