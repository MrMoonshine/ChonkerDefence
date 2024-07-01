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
    @param len length of data in bytes
    @returns CD_NET_CODE_OK if ok
*/
uint8_t clilevel_get_info(Client *client, size_t *len);

uint8_t clilevel_count(unsigned char* data, size_t len);
