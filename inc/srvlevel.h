#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <protocol.h>
#include <logging.h>
/*
    @brief Get a list of all available levels for the server
    @client client data if NULL, just determine the size of the pointer in bytes
    @param data data pointer. (must be free-ed manually)
    @param len length of data in bytes
    @returns 0 if ok
*/
int srvlevel_show(unsigned char* data, size_t* len, uint8_t* filecount);
