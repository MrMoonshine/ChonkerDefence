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

#define VBO_INDEX_VERTEX 0
#define VBO_INDEX_UV 1
#define VBO_INDEX_NORMAL 2

typedef struct VBO{
    GLuint vertices, uv, normals;
    size_t faceCount;
    //uint8_t initflags;
}VBO;

void vbo_init(VBO *vbo, size_t faceCount);
int vbo_create(VBO *vbo, float* vertices, float* uv, float* normals, size_t faceCount);
int vbo_create_vertices(VBO *vbo, float* buffer);
int vbo_create_uv(VBO *vbo, float* buffer);
int vbo_create_normals(VBO *vbo, float* buffer);
void vbo_draw(VBO* vbo, GLuint textureID);
void vbo_destroy(VBO* vbo);
