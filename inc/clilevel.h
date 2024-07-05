#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <client.h>
#include <protocol.h>
#include <level.h>

typedef struct ClientLevel{
    char name[LEVEL_NAME_LENGTH];
    char style[LEVEL_STYLE_LENGTH];
    uint8_t width, height;
    Client* client;
}ClientLevel;

/*
    @brief Requests a level list
    @param client Client
    @param len returns length of data in bytes
    @returns CD_NET_CODE_OK if ok
*/
uint8_t clilevel_list_levels(Client *client, size_t *len);

uint8_t clilevel_get_level(Client *client, size_t *len, uint8_t levelID, ClientLevel *level);
void clilevel_destroy(ClientLevel *level);

//uint8_t clilevel_count(unsigned char* data, size_t len);
