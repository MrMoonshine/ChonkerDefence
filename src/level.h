#pragma once
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <netdb.h>
#include <sys/ioctl.h>

#include <errno.h>
extern int errno;

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "nodes.h"
#include "image.h"
#include "meta/level.h"
#include "meta/protocol.h"

//Level Texture Set
typedef struct{
    Image pathStraight;
    Image pathIntersection;
    Image pathCurve;
    Image grass;
    Image shore;
    Image water;
    Image bridge;
    Image catPlace;
    AnimationInfo variationGrass;
    AnimationInfo variationShores;
    AnimationInfo variationWater;
    AnimationInfo variationCatPlacePlayer;
} LTS;

typedef struct {
    char name[LEVEL_TITLE_LENGTH];   //Name
    uint8_t width;
    uint8_t height;
    LTS lts;                        //Level Textures
    SDL_Surface *surface;
    Image background;
    uint8_t *catSpots;   // 3 Byte for each cat node
    uint8_t catSpotCount;
    bool loaded;
} Level;

int levelSockSetup(ClientSock *csock);
int levelCreateFromServer(Level *level, ClientSock *csock, SDL_Renderer *renderer);
//Load Level onto Server
int levelLoadFromFile(ClientSock *csock, const char* filepath);
int levelHandle(Level *level, ClientSock *csock);

void levelCreate(Level *level ,int stage, const char* texture_set_name, SDL_Renderer *renderer);
void levelCreateEmpty(Level *level);
void levelDraw(Level *level);
void levelDestroy(Level *level);
