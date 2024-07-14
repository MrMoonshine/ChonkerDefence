#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include <texture.h>
#include <logging.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fast_obj.h>

typedef struct Decoration{
    GLuint vertexbuffer, uvbuffer, normalbuffer;
    size_t vertexCount;
    Texture texture;
}Decoration;

int decoration_create(Decoration* decoration);
void decoration_draw(Decoration* decoration);
void decoration_destroy(Decoration* decoration);
