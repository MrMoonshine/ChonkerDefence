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
typedef struct Image2D{
    GLuint vertexBuffer, uvBuffer;
    GLuint textureUniform;
    float x, y;
    UI* ui;
    Texture* texture;
    bool createdViaPNG;
}Image2D;
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
int ui_image2d_create(Image2D* image, UI* ui, Texture* texture);
int ui_image2d_create_from_png(Image2D* image, UI* ui, const char* pngfile);
void ui_image2d_draw(Image2D* image);
int ui_image2d_destroy(Image2D* image);
