#pragma once
#include <stdint.h>
#include <SDL.h>
#include "level.h"
#define GAME_PLAYERS_MAX 3
#define GAME_PLAYER_NAME_LENGTH 10

//Speeds are for the Mice in the game. Not for your computer mouse
//Speed in pixels/s
#define GAME_MOUSE_SPEED 50
#define GAME_MOUSE_SPEED_DEVIATION 5


typedef struct{
    char name[GAME_PLAYER_NAME_LENGTH];
    uint8_t profileImage;
}PlayerInfo;

typedef struct Path{
    uint8_t x, y;
    // Next Path in the actual game
    struct Path *follow;
    // Next is the next Path in the data model
    struct Path *next;
} Path;

typedef struct {
    char name[LEVEL_TITLE_LENGTH];   //Name
    uint8_t width;
    uint8_t height;
    char style[LEVEL_STYLE_LENGTH];
    uint8_t *nodes;
    size_t nodesSize;
    //Reserved vars
    //char reserved[xxx];
    //Raw Cat node data:
    // 3 bytes per node: player, x, y
    uint8_t *catNodes;
    uint8_t catNodesCount;
    //Information regarding paths
    // All Paths in a List
    Path *paths;
    // Goal
    SDL_Point goal;
    // Starts
    Path **starts;
    uint8_t startCount;
} LevelServer;

Path * insertPath(Path **head, uint8_t x, uint8_t y);
//Delete a given element
void deletePath(Path **head, Path *elem);
//Purge the entire list
void deleteAllPath(Path **head);
//Dump it
void printPath(Path *head);
// Length
size_t lengthPath(Path *head);
