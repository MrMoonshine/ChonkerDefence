#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>
#include <texture.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define UI_SHADER_LAYOUT_VERTEX 1
#define UI_SHADER_LAYOUT_FRAGMENT 2

typedef struct UI{
    GLFWwindow* window;
    GLuint vao, shader;
    // Projection Uniforms
    GLint model, view, projection;
    // Uniforms for colorizing
    GLint colorize;
    //float intensity;

    bool anyButtonHover;

    int windowWidth, windowHeight;
    Texture font, widgets;
}UI;

/*
    @brief Create UI object
*/
int ui_create(UI *ui, GLFWwindow* window);
/*
    @brief Cleanup
*/
int ui_destroy(UI *ui);
/*
    @brief resize handler
*/
void ui_resize(UI *ui, int width, int height);

void ui_no_colorize(UI *ui);
/*
    @brief Enable the UI's VAO to before drawing VBOs
*/
void ui_enable_vao(UI *ui);
