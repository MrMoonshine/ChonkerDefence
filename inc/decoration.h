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

typedef struct Decoration{
    Obj* object;
    vec3 position;
    float rotation, scale;
}Decoration;

int decoration_create(Decoration* decoration, Obj* object);
void decoration_placement(Decoration* decoration, vec3 position, float rotation, float scale);
void decoration_draw(Decoration* decoration, GLuint uniformModel, GLuint uniformNormalMatrix, mat4 model);
void decoration_destroy(Decoration* decoration);
