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

typedef struct Node{
    Image *texture;         //Texture
    AnimationInfo *aniinfo; //Animation Data for e.g Waterfalls
    struct Node* next;      //Next Node
} Node;
