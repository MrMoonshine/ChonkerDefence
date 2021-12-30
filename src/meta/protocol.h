#pragma once
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "game.h"

/* Protocol:

    +------------+---------------+-----------------+
    | CMD 1 Byte | Length 2 Byte | Payload n Bytes |
    +------------+---------------+-----------------+
*/

#define CD_NET_CODE_OK 0x60
#define CD_NET_CODE_FAIL 0x61
#define CD_NET_CODE_GLOB_SRV_OP 0x62
#define CD_NET_CODE_SET_VAR 0x63
#define CD_NET_CODE_GET_VAR 0x64
#define CD_NET_CODE_LOAD_LEVEL 0x65
#define CD_NET_CODE_RECV_MAP_RAW 0x66
#define CD_NET_CODE_PLAYER_NAME 0x67
#define CD_NET_CODE_CHAT_MSG 0x68
#define CD_NET_CODE_EVENT 0x69

#define CD_NET_EVENT_PLAYER_UPDATE 0xa0

#define CD_NET_GLOB_SRV_OP_QUIT 0x80
#define CD_NET_GLOB_SRV_OP_KILL 0x81
#define CD_NET_GLOB_SRV_OP_SINGLEPLAYER 0x82
#define CD_NET_GLOB_SRV_OP_NO_SINGLEPLAYER 0x83

#define CD_NET_VAR_TITLE 0x01
#define CD_NET_VAR_STYLE 0x02
#define CD_NET_VAR_HEIGHT 0x03
#define CD_NET_VAR_WIDTH 0x04
#define CD_NET_VAR_PLAYERS 0x05
#define CD_NET_VAR_PLAYER_COUNT 0x06
#define CD_NET_VAR_PLAYER_NODES 0x07

#define CD_NET_CLIENT_TIMEOUT 1.5f

#define CLIENT_SOCK_SERVER_NO_UP 0
#define CLIENT_SOCK_SERVER_UP_SINGLEPLAYER 1
#define CLIENT_SOCK_SERVER_UP_MULTIPLAYER 2

#define CLIENT_SOCK_FLAG_KILL (1 << 0)
#define CLIENT_SOCK_FLAG_UNINIT (1 << 1)

#define CLEINT_SOCK_BUFFER_SIZE_PLAYER_DISCOVERY (1 + GAME_PLAYERS_MAX*(GAME_PLAYER_NAME_LENGTH+1))

#define CLEINT_SOCK_EVENT_QUEUE_POLL_PERIOD 500

typedef struct{
    struct sockaddr_in6 saddr;
    socklen_t slen;
    int sock;
    uint8_t serverstatus;
    unsigned int level;
    unsigned int flags;
    //Game Gata
    PlayerInfo pinfo[GAME_PLAYERS_MAX];
} ClientSock;

int netInitClientMeta(ClientSock *csock);
int netLobbyDiscovery(ClientSock *csock);
int netSockSetup(ClientSock *csock);
int netKillServer(ClientSock *csock);
int netLoadLevel(ClientSock *csock, const char* filepath);
