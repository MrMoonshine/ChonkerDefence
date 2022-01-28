#pragma once
#include "meta/protocol.h"
#include "meta/level.h"
#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>

#include "nodes.h"
#include "meta/game.h"
#include "level.h"

#define SERVER_FLAG_SINGLEPLAYER (1 << 0)

#define SERVER_CONNECTIONS_MAX GAME_PLAYERS_MAX
#define SERVER_RECEIVE_BUFFER_SIZE 64
#define SERVER_CLI_LINE_LEN 192

#define SERVER_HEADER_LENGTH (sizeof(uint8_t) + sizeof(unsigned short))

/*--------------------------------------*/
/*            Server                    */
/*--------------------------------------*/
typedef struct{
   bool useIPv6;
   struct sockaddr_in6 saddr6;
   int serverFD, maxSD;

   fd_set fds;
   int csocks[SERVER_CONNECTIONS_MAX];
   PlayerInfo pinfo[SERVER_CONNECTIONS_MAX];
   bool singleplayer;
} LevelSock;

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
    SDL_Point goal;
    Path **starts;
    uint8_t startCount;
} LevelServer;

int serverStart(unsigned int flags);
int serverLoadLevel(LevelServer *ls, char* filename);
void serverDestroy(LevelServer *ls);
