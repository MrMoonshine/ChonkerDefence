#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>
#include <texture.h>

#include <ui/ui.h>
#include <ui/text2d.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

typedef struct LevelSelection{
    UI *ui;
    GLuint bgvbo, bguvvbo;
    Texture texture;
    GLuint textureUniform;

    Text2D title;
}LevelSelection;

int levelselection_create(LevelSelection *selection, UI* ui);
int levelselection_draw(LevelSelection *selection);
int levelselection_destroy(LevelSelection *selection);
