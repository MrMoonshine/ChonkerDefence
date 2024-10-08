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
#include <glshader.h>
#include <tilemap.h>
#include <common.h>
#include <decoration.h>
#include <modellib.h>
#include <terrainwater.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

//#define DEBUG_MAP 1

typedef struct Terrain{
    VBO vbo;
    Tilemap tilemap;
    uint8_t width, height;
    TerrainWater water;

    ModelLib modellib;
    Decoration* decorations;
    unsigned int decorationCount;
}Terrain;

/*
    @brief creates Terrain VBO
    @param terrain Terrain
    @param buffer total level bufffer
    @param bufferSize, size of buffer
    @returns 0 if ok
*/
int terrain_create(Terrain* terrain, uint8_t* buffer, size_t bufferSize);
void terrain_draw(Terrain* terrain, GLuint uniformModel, GLuint uniformNormalMatrix, mat4 model);
void terrain_destroy(Terrain* terrain);
