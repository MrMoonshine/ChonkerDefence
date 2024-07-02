#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <glshader.h>
#include <logging.h>
#include <texture.h>
#include <protocol.h>
#include <client.h>
#include <clilevel.h>

#include <ui/ui.h>
#include <ui/text2d.h>
#include <ui/button.h>
#include <ui/image2d.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define UI_LEVELMENU_BACK INT_MAX

typedef struct LevelMenu{
    UI *ui;
    Client* client;

    Image2D background;
    Button btnBack;
    Text2D title;
    Button* buttons;
    uint8_t buttonCount;
}LevelMenu;

int ui_levelmenu_create(LevelMenu *menu, UI* ui, Client* client);
int ui_levelmenu_draw(LevelMenu *menu);
int ui_levelmenu_destroy(LevelMenu *menu);

