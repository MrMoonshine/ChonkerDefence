#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>
#include <texture.h>

#include <ui/ui.h>
#include <ui/container.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

typedef struct MainMenu{

}MainMenu;

int ui_mainmenu_create(MainMenu* menu, UI* ui);
void ui_mainmenu_draw(MainMenu* menu);
void ui_mainmenu_destroy(MainMenu* menu);
