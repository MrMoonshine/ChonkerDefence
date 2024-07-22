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
#include <obj.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define MODELLIB_FILE_PATTERN_TREE "../assets/models/%s_tree_%d.obj"

typedef struct ModelLib{
    Obj *models;
    uint8_t treeCount;
}ModelLib;

int modellib_create(ModelLib* lib, const char* style);
void modellib_destroy(ModelLib* lib);

Obj* modellib_get_tree(ModelLib* lib, uint8_t variant);
