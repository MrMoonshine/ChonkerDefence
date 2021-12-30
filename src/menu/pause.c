#include "menu.h"

static const SDL_Colour MENU_COLOR_TITLE = {
    .r = 72,
    .g = 98,
    .b = 135
};

void pauseMenuCreate(PauseMenu *menu, MenuCore *mc){
    menu->enable = false;
    menu->core = mc;
    
    fontCreate(&menu->title, FONT_DEFAULT, 56, MENU_COLOR_TITLE, mc->ui.renderer);
    fontRender(&menu->title, MENU_PAUSE_TITLE);
    menuButtonCreate(
        &menu->resume,
        MENU_PAUSE_TEXT_BUTTON_RESUME,
        NULL,
        &menu->core->ui,
        BUTTON_GREEN,
        false
    );
    
    menuButtonCreate(
        &menu->stop,
        MENU_PAUSE_TEXT_BUTTON_STOP,
        NULL,
        &menu->core->ui,
        BUTTON_RED,
        false
    );
    
    menuButtonCreate(
        &menu->toggle,
        MENU_STANDALONE_BUTTON_TEXT_PAUSE,
        NULL,
        &menu->core->ui,
        BUTTON_BLUE,
        BT_VERY_SHORT
    );
}

void pauseMenuEnable(PauseMenu *menu, ClientSock *csock){
    menu->enable = true;
}

void pauseMenuDisable(PauseMenu *menu){
    menu->enable = false;
}

int pauseMenuHandle(PauseMenu *menu, float winscale){
    int a = menuButtonHandle(
        &menu->toggle,
        APP_WIDTH - menu->toggle.hitbox.w,
        10,
        winscale
    ) << MENU_PAUSE_BUTTON_ENABLE;
    
    if(!menu->enable)
        return 0;
    
    uint8_t ret = 0;
    menuCoreDrawBackground(menu->core);
    fontDraw(
        &menu->title,
        menu->core->area.x + 20,
        menu->core->area.y + 20        
    );
    unsigned short bh = 192;
    //if the user wants to resume the game, it'll hust disable the menu and leave it there
    ret |= menuButtonHandle(
        &menu->resume,
        16,
        bh,
        winscale
    ) << MENU_PAUSE_BUTTON_RESUME;
    bh += BUTTON_HEIGHT;
    ret |= menuButtonHandle(
        &menu->stop,
        16,
        bh,
        winscale
    ) << MENU_PAUSE_BUTTON_STOP;
    
    return ret;
}

void pauseMenuDestroy(PauseMenu *menu){
    fontDestroy(&menu->title);
    menuButtonDestroy(&menu->resume);
    menuButtonDestroy(&menu->stop);
    menuButtonDestroy(&menu->toggle);
}
