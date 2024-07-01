#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <unistd.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <logging.h>
#include <protocol.h>

typedef struct Client{
    int socket;
}Client;

int client_init(Client *client, const char* address_str, unsigned short portnum);
int client_disconnect(Client *client);
/*
    @brief shut down server
    @param client client
    @param key admin key
    @param tid thread ID to cleanly shut down
    @return -1 on error
*/
int client_server_kill(Client *client, uint32_t key, pthread_t tid);
