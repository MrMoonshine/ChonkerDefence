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
/*
    @brief Get a list of all available levels for the server
    @client client data (pre-allocated)
    @param data data pointer. (must be free-ed manually)
    @param len length of data in bytes
    @returns 0 if ok
*/
int clilevel_show(Client *client, unsigned char* data, size_t *len);

uint8_t clilevel_count(unsigned char* data, size_t len);
