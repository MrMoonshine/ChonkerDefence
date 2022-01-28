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
    MOUSE_SMALL=0,
    MOUSE_NORMAL=1,
    MOUSE_FAT=2,
    MOUSE_CHEF=3,             //He will build a field camp, hence: spawning more mice
    MOUSE_BIGGIE_CHEESE=4     //Mor Info at:  r/biggiecheese  :3
} MouseType;
//Mouse List Item
typedef struct MouseLI{
    MouseType type;
    //The Position of the current Node
    SDL_Point node;
    //Speed of this Particular Mouse
    float speed;
    char word[16];
    struct MouseLI *next;
}MouseLI;

//----------------------------------
//  List Functions
//----------------------------------
MouseLI * insertMouseLI(MouseLI **head, const char *data);
//Delete a given element
void deleteMouseLI(MouseLI *head, MouseLI *elem);
//Purge the entire list
void deleteAllMouseLI(MouseLI **head);
//Dump it
void printMouseLI(MouseLI *head);
//----------------------------------
//  Graphical Functions
//----------------------------------
