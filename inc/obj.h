
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
#include <vbo.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <tinyobj_loader_c.h>

typedef struct Obj{
    VBO vbo;
    GLuint vertexbuffer, uvbuffer, normalbuffer;
    size_t faceCount;
    Texture texture;
}Obj;

int obj_create(Obj* obj, const char* filename);
void obj_draw(Obj* obj);
void obj_destroy(Obj* obj);
