#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "meta/game.h"

/*
    This basically resembles OSPFv3 without areas and just L3 P2P links
    (ordered on wish :3 )
    Link Local Address:     fe80:<link-id>:<intersection-id>::<interface-id>/16
    NET-ID                  2001:<link-id>::/64
    Router ID:              <type>.<intersection-id>.x.y
*/

#define SPF_TYPE_GOAL 0
#define SPF_TYPE_START 1
#define SPF_TYPE_INTERSECTION 2

typedef struct SPFRouter{
    uint8_t x, y;
    uint32_t routerID;
    struct SPFRouter* neighbours[4];
    uint8_t neighbourCount;
}SPFRouter;

typedef struct SPFLink{
    
}SPFLink;

typedef struct SPF{
    SPFRouter *routers;
    uint8_t routerCount;
    SPFLink *links;
    uint8_t linkCount;
}SPF;

int spfCreate(SPF *spf, LevelServer *ls);
void spfDestroy(SPF *spf);
