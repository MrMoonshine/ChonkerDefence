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
#include <terrain.h>
#include <common.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

typedef struct ClientLevel{
    GLFWwindow* window;
    char name[LEVEL_NAME_LENGTH];
    char style[LEVEL_STYLE_LENGTH];
    uint8_t width, height;
    Client* client;
    //Tilemap tilemap;

    float windowScale;
    float modelScale;
    GLuint vao, shader;
    // Projection Uniforms
    GLint model, view, projection;
    Terrain terrain;
    /*GLuint terrainVertexbuffer, terrainUVBuffer;
    size_t terrainVertexCount;*/
}ClientLevel;

/*
    @brief Requests a level list
    @param client Client
    @param len returns length of data in bytes
    @returns CD_NET_CODE_OK if ok
*/
uint8_t clilevel_list_levels(Client *client, size_t *len);

uint8_t clilevel_get_level(ClientLevel *level, Client *client, GLFWwindow* window, size_t *len, uint8_t levelID);
void clilevel_destroy(ClientLevel *level);

void clilevel_dump(ClientLevel *level);
void clilevel_resize(ClientLevel *level, int width, int height);
void clilevel_enable_vao(ClientLevel* level);
int clilevel_draw(ClientLevel *level);
