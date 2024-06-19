#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

typedef struct LevelSelection{
    GLuint vao, bgvbo, shader;
    GLuint sp, sv, sm;
}LevelSelection;

int levelselection_create(LevelSelection *selection);
int levelselection_draw(LevelSelection *selection);
int levelselection_destroy(LevelSelection *selection);
