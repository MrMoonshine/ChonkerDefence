#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <texture.h>

typedef struct Tilemap{
    Texture texture;
    uint8_t width, height;
}Tilemap;

/*
    @brief creates a tilemap from a texture

*/
int tilemap_create(Tilemap *tilemap, uint8_t width, uint8_t height, const char* filepath);
void tilemap_destroy(Tilemap *tilemap);
