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

/*
    Just a rectangular box with an image.
*/
typedef struct UIContainer{
    GLuint vertexBuffer, uvBuffer;
    GLuint textureUniform;
    Texture* texture;
    bool createdViaPNG;
}UIContainer;
/*
    @brief Constructs Staic UI Container
    @param container Struct
    @param width NULL if auto
    @param height NULL if auto
    @param margin
    @param zindex how far should this be in the background
    @param texture or pngfile
    @returns 0 if ok
*/
int ui_container_create(UIContainer* container, UI* ui, int *width, int *height, int margin, float zindex, Texture* texture);
int ui_container_create_from_png(UIContainer* container, UI* ui, int *width, int *height, int margin, float zindex, const char* pngfile);
void ui_container_draw(UIContainer* container);
int ui_container_destroy(UIContainer* container);
