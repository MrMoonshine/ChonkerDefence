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

typedef struct Button{
    UI *ui;
    GLuint vertexbuffer, uvbuffer;
    Text2D text;
    float width, height;
    float x, y;
}Button;

int ui_button_create(Button *button, UI *ui, float width, float height, const char* text);
void ui_button_draw(Button *button);
void ui_button_destroy(Button *button);
