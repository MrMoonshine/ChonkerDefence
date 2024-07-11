#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <texture.h>
#include <common.h>

typedef struct Tilemap{
    Texture texture;
    uint8_t width, height;
}Tilemap;

/*
    @brief creates a tilemap from a texture

*/
int tilemap_create(Tilemap *tilemap, uint8_t width, uint8_t height, const char* filepath);
void tilemap_destroy(Tilemap *tilemap);

void tilemap_get_block_UV_rotate(Tilemap *tilemap, float* buffer, uint8_t vertexCount, uint8_t x, uint8_t y, uint8_t rotation);
void tilemap_get_block_UV(Tilemap *tilemap, float* buffer, uint8_t vertexCount, uint8_t x, uint8_t y);
