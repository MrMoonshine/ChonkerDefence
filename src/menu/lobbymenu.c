#include "menu.h"
static const char* TAG = "[LOBBY]";
static const char* LOBBYBG = "textures/ui/background-lobby.png";
static const unsigned int MENU_LOBBY_ENTRY_WIDTH = 128;
static const unsigned int MENU_LOBBY_WIDTH = MENU_LOBBY_ENTRY_WIDTH*GAME_PLAYERS_MAX;
static const unsigned int TEXT_SIZE = 20;
static const SDL_Colour NAME_COLOUR = {
    .r = 255,
    .g = 255,
    .b = 255
};

static void clearNames(LobbyMenu *menu){
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        fontRender(menu->names + a, " ");
    }
}

//Lobby
void lobbyMenuCreate(LobbyMenu *menu, MenuCore *mc){
    menu->enable = false;
    menu->core = mc;
    char *buff;
    buff = (char*)malloc(strlen(ASSET_PATH) + strlen(LOBBYBG));
    memset(buff, 0, strlen(ASSET_PATH) + strlen(LOBBYBG));
    strcpy(buff, ASSET_PATH);
    strcat(buff, LOBBYBG);
    textureLoad(&menu->background, menu->core->ui.renderer, buff);
    free(buff);
    
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        fontCreate(menu->names + a, FONT_DEFAULT, TEXT_SIZE, NAME_COLOUR, mc->ui.renderer);
        char buffer[11];
        sprintf(buffer, "netPlayer%d", a);
        fontRender(menu->names + a, buffer);
    }
}

void lobbyMenuEnable(LobbyMenu *menu, ClientSock *csock){
    SDL_Log("%s\tEnabling Lobby Menu...\n", TAG);
    menu->enable = true;
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        fontRender(menu->names + a, strlen(csock->pinfo[a].name) ? csock->pinfo[a].name : " ");
    }
}

void lobbyMenuDisable(LobbyMenu *menu){
    menu->enable = false;
    clearNames(menu);
}

int lobbyMenuHandle(LobbyMenu *menu){
    if(!menu->enable)
        return 0;
    
    size_t menux = (APP_WIDTH / 2) - (MENU_LOBBY_WIDTH/2);
    textureDraw(&menu->background, menux, 0, SDL_FLIP_NONE, NULL);
    
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        fontDraw(menu->names + a, menux + 12 + MENU_LOBBY_ENTRY_WIDTH * a, 62);
    }
    
    return 0;
}

void lobbyMenuDestroy(LobbyMenu *menu){
    textureDestroy(&menu->background);
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        fontDestroy(menu->names + a);
    }
}
