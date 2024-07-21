
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

//#define DEBUG_OBJ 1

struct Shape{
    unsigned int faceOffset;
    unsigned int length;
    unsigned int textureIndex;
};

typedef struct Obj{
    VBO vbo;
    size_t faceCount;
    // Shapes
    struct Shape* shapes;
    unsigned int shapeCount;
    // Textures
    Texture* textures;
    unsigned int textureCount;
}Obj;

int obj_create(Obj* obj, const char* filename);
void obj_draw(Obj* obj);
void obj_destroy(Obj* obj);
