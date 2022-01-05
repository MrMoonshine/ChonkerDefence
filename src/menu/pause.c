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

void pauseMenuEnable(PauseMenu *menu){
    menuButtonAlterText(&menu->toggle, ">");
    menu->enable = true;
}

void pauseMenuDisable(PauseMenu *menu){
    menuButtonAlterText(&menu->toggle, "||");
    menu->enable = false;
}

int pauseMenuHandle(PauseMenu *menu, bool gameIsRunning, float winscale){
    if(!(menu->enable || gameIsRunning))
        return 0;
    
    uint8_t ret = 0;
    //If the menu is enabled, this is the Resume Button, else it's the button to open the menu
    ret = menuButtonHandle(
        &menu->toggle,
        APP_WIDTH - menu->toggle.hitbox.w,
        10,
        winscale
    ) << (menu->enable ? MENU_PAUSE_BUTTON_RESUME : MENU_PAUSE_BUTTON_ENABLE);
    // Don't render any other menu components
    if(!menu->enable)
        return ret;
    
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
