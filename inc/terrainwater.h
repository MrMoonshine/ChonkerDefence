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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define TERRAINWATER_FRAMECOUNT 16

typedef struct TerrainWater{
    GLuint vertexbuffer, normalsbuffer;
    Tilemap tilemap;
    GLuint uvbuffers[TERRAINWATER_FRAMECOUNT]; // create an UV buffer for each frame

    unsigned int triangleCount;
    uint8_t animationFrame;
    double lastAnimation;
}TerrainWater;

int terrainwater_create(TerrainWater* water, uint8_t* buffer, size_t bufferSize, uint8_t width, uint8_t height);
void terrainwater_draw(TerrainWater* water, GLuint uniformModel, GLuint uniformNormalMatrix, mat4 model);
void terrainwater_destroy(TerrainWater* water);
