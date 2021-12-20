#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include "../image.h"
#include "../text.h"
#include "../server.h"
#include "../meta/level.h"
#include "../meta/protocol.h"
#include "../meta/game.h"
#include "button.h"

#define MENU_MAIN_TEXT_BUTTON_PLAY "Play"
#define MENU_MAIN_TEXT_BUTTON_LOBBY "Start Lobby"
#define MENU_MAIN_TEXT_BUTTON_NO_LOBBY "Stop Lobby"
#define MENU_MAIN_TEXT_BUTTON_JOIN "Join Game"
#define MENU_MAIN_TEXT_BUTTON_OPTIONS "Options"
#define MENU_MAIN_TEXT_BUTTON_QUIT "Quit"

#define MENU_TEXT_TITLE_START "Level Selection:"
#define MENU_TEXT_BUTTON_PLAY "Play"
#define MENU_TEXT_BUTTON_BACK "Back"
#define MENU_TEXT_BUTTON_RANDOM "Random"

#define MENU_TEXT_BUTTON_START_SINGLEPLAYER "Play Alone"
#define MENU_TEXT_BUTTON_START_LAN_PARTY "Play Together"

#define MENU_MOUSE_COOLDOWN 2500
#define MENU_LEVEL_PATH_LENGTH 256
#define MENU_LEVEL_PER_PAGE 6

#define MENU_MAIN_BUTTON_END 0
#define MENU_MAIN_BUTTON_PLAY 1
#define MENU_MAIN_BUTTON_LOBBY 2
#define MENU_MAIN_BUTTON_JOIN 3
#define MENU_MAIN_BUTTON_OPTIONS 4

#define MENU_START_BUTTON_END 0
#define MENU_START_BUTTON_RANDOM 1
#define MENU_START_BUTTON_LEVELS_OFFSET 2

#define MENU_START_BUTTON_STRLEN 24

#define MENU_ACTION_START_GAME 1
#define MENU_ACTION_START_SOCKET 2
/*--------------------------------------*/
/*            Core Class                */
/*--------------------------------------*/
typedef struct{
    SDL_Rect area;
    UIElements ui;
    SDL_Window* window;
    unsigned short mouse_cooldown;
} MenuCore;

void menuCoreInit(MenuCore *mc, SDL_Window* window, SDL_Renderer *renderer);
void menuCoreDestroy(MenuCore *mc);
/*--------------------------------------*/
/*           Menu Class                 */
/*--------------------------------------*/
typedef struct{
    Button *buttons;
    size_t buttonCount;
}MenuMask;

typedef struct{
    MenuCore *core;
    Font title;
    Button play;
    Button lobby;
    Button nolobby;
    Button join;
    Button options;
    Button bexit;
    bool enable;
}MainMenu;

typedef struct{
    MenuCore *core;
    Font title;
    Button *levels;
    size_t levelCount;
    char **levelList;
    Button prev, next;
    Button random;
    Button back;
    uint8_t currentPage;
    bool enable;
}StartMenu;

//An overview of joined players
typedef struct{
    MenuCore *core;
    Image background;
    Font names[GAME_PLAYERS_MAX];
    bool enable;
}LobbyMenu;


int menuHandleAll(ClientSock *client, MainMenu* main, StartMenu* start, LobbyMenu *lobby, float winscale);
//Core
void menuCoreDrawBackground(MenuCore *mc);
//Main Menu
void mainMenuCreate(MainMenu *menu, MenuCore *mc);
uint8_t mainMenuHandle(MainMenu *menu, LobbyMenu *lobby, float winscale);
void menuCoreMouseCD(MenuCore *mc);
void mainMenuDestroy(MainMenu *menu);
//Start Menu
void startMenuCreate(StartMenu *menu, MenuCore *mc);
void startMenuEnable(StartMenu *menu);
void startMenuDisable(StartMenu *menu);
unsigned int startMenuHandle(StartMenu *menu, float winscale);
void startMenuDestroy(StartMenu *menu);
//Lobby
void lobbyMenuCreate(LobbyMenu *menu, MenuCore *mc);
void lobbyMenuEnable(LobbyMenu *menu, ClientSock *csock);
void lobbyMenuDisable(LobbyMenu *menu);
int lobbyMenuHandle(LobbyMenu *menu);
void lobbyMenuDestroy(LobbyMenu *menu);
