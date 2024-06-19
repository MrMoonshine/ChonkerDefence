#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <logging.h>
#include <protocol.h>
#include <srvlevel.h>

#define SERVER_PORT_DEFAULT 42069
#define SERVER_CLIENT_COUNT_MAX 3
#define SERVER_RCV_BUFF_SIZE 128

typedef struct server_initial_params{
    uint16_t port;          // Port Number
    uint32_t sessionkey;    // Admin key to e.g gracefully stop server
    pthread_t thrid;        // Thread ID
}server_initial_params;

/*
    @brief Start server.
    @returns thread ID
*/
int server_start(server_initial_params *params);
int server_stop();
