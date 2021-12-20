#include "menu.h"

static const SDL_Colour MENU_COLOR_TITLE = {
    .r = 72,
    .g = 98,
    .b = 135
};

void mainMenuCreate(MainMenu *menu, MenuCore *mc){
    menu->enable = true;
    menu->core = mc;
    
    fontCreate(&menu->title, FONT_DEFAULT, 56, MENU_COLOR_TITLE, mc->ui.renderer);
    fontRender(&menu->title, APP_TITLE);
    menuButtonCreate(
        &menu->play,
        MENU_MAIN_TEXT_BUTTON_PLAY,
        NULL,
        &menu->core->ui,
        BUTTON_BLUE,
        false
    );
    menuButtonCreate(
        &menu->lobby,
        MENU_MAIN_TEXT_BUTTON_LOBBY,
        NULL,
        &menu->core->ui,
        BUTTON_BLUE,
        false
    );
    menuButtonCreate(
        &menu->nolobby,
        MENU_MAIN_TEXT_BUTTON_NO_LOBBY,
        NULL,
        &menu->core->ui,
        BUTTON_RED,
        false
    );
    menuButtonCreate(
        &menu->join,
        MENU_MAIN_TEXT_BUTTON_JOIN,
        NULL,
        &menu->core->ui,
        BUTTON_BLUE,
        false
    );
    menuButtonCreate(
        &menu->options,
        MENU_MAIN_TEXT_BUTTON_OPTIONS,
        NULL,
        &menu->core->ui,
        BUTTON_YELLOW,
        false
    );
    menuButtonCreate(
        &menu->bexit,
        MENU_MAIN_TEXT_BUTTON_QUIT,
        NULL,
        &menu->core->ui,
        BUTTON_RED,
        false
    );
}

uint8_t mainMenuHandle(MainMenu *menu, LobbyMenu *lobby, float winscale){
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
    ret |= menuButtonHandle(
        &menu->play,
        16,
        bh,
        winscale
    ) << MENU_MAIN_BUTTON_PLAY;
    bh += BUTTON_HEIGHT;
    ret |= menuButtonHandle(
        lobby->enable ? &menu->nolobby : &menu->lobby,
        16,
        bh,
        winscale
    ) << MENU_MAIN_BUTTON_LOBBY;
    bh += BUTTON_HEIGHT;
    ret |= menuButtonHandle(
        &menu->join,
        16,
        bh,
        winscale
    ) << MENU_MAIN_BUTTON_JOIN;
    bh += BUTTON_HEIGHT;
    ret |= menuButtonHandle(
        &menu->options,
        16,
        bh,
        winscale
    ) << MENU_MAIN_BUTTON_OPTIONS;
    bh += BUTTON_HEIGHT;
    ret |= menuButtonHandle(
        &menu->bexit,
        16,
        bh,
        winscale
    ) << MENU_MAIN_BUTTON_END;
    
    return ret;  
}

void mainMenuDestroy(MainMenu *menu){
    fontDestroy(&menu->title);
    menuButtonDestroy(&menu->play);
    menuButtonDestroy(&menu->lobby);
    menuButtonDestroy(&menu->nolobby);
    menuButtonDestroy(&menu->join);
    menuButtonDestroy(&menu->options);
    menuButtonDestroy(&menu->bexit);
}
