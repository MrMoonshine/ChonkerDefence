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

typedef struct LevelMenu{
    UI *ui;
    GLuint bgvbo, bguvvbo;
    Texture texture;
    GLuint textureUniform;

    Text2D title;
}LevelMenu;

int levelselection_create(LevelMenu *selection, UI* ui);
int levelselection_draw(LevelMenu *selection);
int levelselection_destroy(LevelMenu *selection);
