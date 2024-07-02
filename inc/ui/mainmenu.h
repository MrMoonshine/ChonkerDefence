#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <glshader.h>
#include <logging.h>
#include <texture.h>

#include <ui/ui.h>
#include <ui/image2d.h>
#include <ui/button.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define UI_MAINMENU_SINGLEPLAYER    1
#define UI_MAINMENU_JOIN            2
#define UI_MAINMENU_SERVER          3
#define UI_MAINMENU_ABOUT           4
#define UI_MAINMENU_QUIT            5

typedef struct MainMenu{
    UI *ui;

    Text2D title;
    Image2D background;

    Button btnSingleplayer;
    Button btnServer;
    Button btnJoin;
    Button btnAbout;
    Button btnQuit;
}MainMenu;

int ui_mainmenu_create(MainMenu* menu, UI* ui);
int ui_mainmenu_draw(MainMenu* menu);
void ui_mainmenu_destroy(MainMenu* menu);
