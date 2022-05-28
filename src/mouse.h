#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>

//Helper functions for the server
//Mouse Texture Set
typedef struct{
   int a; 
} MTS;
//Mouse Types
typedef enum {
    MOUSE_SMALL=0,              // Small...but speeedyyyy!
    MOUSE_NORMAL=1,             // Normal
    MOUSE_FAT=2,                // THICC Mouse
    MOUSE_CHEF=3,               //He will build a field camp, hence: spawning more mice
    MOUSE_BIGGIE_CHEESE=4       //Mor Info at:  r/biggiecheese  :3
} MouseType;
//Mouse List Item
typedef struct Mouse{
    MouseType type;
    uint8_t speed;
    unsigned short id;
    struct Mouse *next;
}Mouse;

//----------------------------------
//  List Functions
//----------------------------------
