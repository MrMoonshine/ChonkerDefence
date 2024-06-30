#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>
#include <texture.h>
#include <ui/ui.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define UI_TEXT2D_FONT_TILES_X 16.0f
#define UI_TEXT2D_FONT_TILES_Y 16.0f

typedef struct Text2D{
    GLuint vertexbuffer, uvbuffer;
    size_t length;
    float size, x, y;
    vec4 color;
    float width, height;    // Dimensions of text block
    UI* ui;
}Text2D;

/*
    @brief generates text for 3d space
    @param text struct
    @param ui UI
    @param message message
    @param size Font Size
    @return 0 if ok
*/
int ui_text2d_create(Text2D* text, UI *ui, const char* message, float size);
void ui_text2d_draw(Text2D* text);
void ui_text2d_destroy(Text2D* text);
void ui_text2d_set_color(Text2D* text, const vec4 color);
