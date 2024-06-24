#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <png.h>

typedef struct Texture{
    GLuint bufferID;
    uint32_t width, height;
}Texture;

/*
 * @brief Load image from PNG file
 * @param filename PNG file
 * @param width returns width if not NULL
 * @param height returns height if not NULL
 * @return 0 if ok
*/
int texture_create(Texture* texture, const char* filename);
// Cleanup GPU buffer
void texture_destroy(Texture* texture);
