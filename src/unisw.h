#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "meta/game.h"

/*
    It basically works like an unmanaged switch,
    the major difference is, that each node only stores a
    single MAC Table entry containing information about 
    the goal node
    
    L2 Addr <type>.<intersection-id>.x.y
*/

#define UNISW_TYPE_GOAL 0
#define UNISW_TYPE_START 1
#define UNISW_TYPE_INTERSECTION 2

#define UNISW_FACING_NORTH 0
#define UNISW_FACING_EAST 1
#define UNISW_FACING_SOUTH 2
#define UNISW_FACING_WEST 3

//"MAC" address table entry
typedef struct UNISWMacTabE{
    
}UNISWMacTabE;

typedef struct UNISwitch{
    uint8_t x, y;
    uint32_t switchID;
    //Mac Address "Table"
    bool goalKnown;
    uint8_t fwdInt;
}UNISwitch;

typedef struct UNISW{
    UNISwitch *switches;
    uint8_t switchCount;
}UNISW;

int uniswCreate(UNISW *unisw, LevelServer *ls);
void uniswDestroy(UNISW *unisw);
