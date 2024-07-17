#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <texture.h>
#include <logging.h>
#include <common.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct VBO{
    GLuint vertices, uv, normals;
    size_t faceCount;
    //uint8_t initflags;
}VBO;

int vbo_create(VBO *vbo, float* vertices, float* uv, float* normals, size_t faceCount);
void vbo_draw(VBO* vbo, GLuint textureID);
void vbo_destroy(VBO* vbo);
