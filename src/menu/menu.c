#include "menu.h"
static const char* TAG = "[MENU]";

static const SDL_Colour MENU_COLOR_TITLE = {
    .r = 72,
    .g = 98,
    .b = 135
};

void menuCoreInit(MenuCore *mc, SDL_Window* window, SDL_Renderer *renderer){
    mc->mouse_cooldown = 0;
    mc->ui.renderer = renderer;
    mc->window = window;
    uiElementsCreate(&mc->ui, mc->ui.renderer);
    //Get height of image
    SDL_QueryTexture(mc->ui.background.texture, NULL, NULL, NULL, &mc->area.h);
    
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    mc->area.x = 0;
    mc->area.y = (h - mc->area.h)/2;
}

void menuCoreMouseCD(MenuCore *mc){
    mc->mouse_cooldown = MENU_MOUSE_COOLDOWN;
}

void menuCoreDrawBackground(MenuCore *mc){
    textureDraw(
        &mc->ui.background,
        mc->area.x,
        mc->area.y,
        SDL_FLIP_NONE,
        NULL
    );
}

void menuCoreDestroy(MenuCore *mc){
    uiElementsDestroy(&mc->ui);
}

static void startMenuScanLevels(char **list, size_t *levelCount){
    if(list == NULL)
        (*levelCount) = 0;
    
    char dirpath[256];
    strcpy(dirpath, ASSET_PATH);
    strcat(dirpath, "levels/");
    //SDL_Log("Searching Levels at:\t%s\n", dirpath);
    
    DIR *dp;
    struct dirent *ep;     
    dp = opendir (dirpath);
    uint8_t counter = 0;
    if (dp != NULL)
    {
        while ((ep = readdir (dp)) != NULL){
            //puts (ep->d_name);
            if(strstr(ep->d_name, ".chonkmap") == NULL)
                continue;
            if(list){
                //SDL_Log("%s\tfound: %s\n", TAG, ep->d_name);
                if(strlen(ep->d_name) < MENU_LEVEL_PATH_LENGTH)
                strcpy(list[counter++], ep->d_name);
                else
                strcpy(list[counter++], "");
            }else{
               (*levelCount)++;
            }
        }
        (void) closedir (dp);
    }
    else
        SDL_LogError(0,"Couldn't open the directory");
}

static void cleanString(char *str){
    size_t len = strlen(str);
    char buffer[64];
    strcpy(buffer, "");
    for(int a = 0; a < len; a++){
        char c = str[a];
        if(isprint(c)){
            strcat(buffer, &c);
        }
    }
    strcpy(str, "");
    strcpy(str, buffer);
}

static int startMenuGetLevelData(StartMenu *menu, char* name, uint8_t *priority){
    const size_t len = 192;
    char *buffer;
    buffer = (char*)malloc(len);
    if(!buffer)
        return -1;
    memset(buffer, 0, len);
    strcpy(buffer, ASSET_PATH);
    strcat(buffer, "levels/");
    strcat(buffer, name);
    
    FILE *fp = fopen(buffer,"r");
    if(!fp){
        perror(name);
        strcpy(name, "ERROR");
        return -1;
    }
    
    memset(buffer, 0, len);
    fread(buffer, 1, LEVEL_TITLE_LENGTH, fp);
    sprintf(name,"%s",buffer);
    //Shorten too long Level Names
    if(strlen(name) > MENU_START_BUTTON_STRLEN){
        char buff1[64];
        memset(buff1, 0, 64);
        strncpy(buff1, name, MENU_START_BUTTON_STRLEN - 3);
        strcat(buff1, "...");
        sprintf(name,"%s",buff1);
        //SDL_Log("%s wird verkürzt zu %s\n", buffer, name);
    }
    free(buffer);
    buffer = NULL;
    
    fclose(fp);
    return 0;
}

static void startMenuCreateButtons(StartMenu *menu){
    menu->levels = (Button*)malloc(sizeof(Button) * menu->levelCount);
    if(menu->levels == NULL){
        perror("malloc");
        return;
    }
    
    char buffer[64];
    uint8_t lvl;
    
    for(int a = 0; a < menu->levelCount; a++){
        memset(buffer, 0, 64);
        strcpy(buffer, menu->levelList[a]);
        startMenuGetLevelData(menu, buffer, &lvl);
        
        menuButtonCreate(
            menu->levels + a,
            buffer,
            NULL,
            &menu->core->ui,
            BUTTON_BLUE,
            false
        );
    }
}

static unsigned int startMenuHandleLevelButtons(StartMenu *menu, float winscale){
    if(!menu->levels)
        return 0;
    
    unsigned short bh = 140;
    unsigned short bw = 16;
    
    unsigned int ret = 0;
    for(int a = 0; a < menu->levelCount; a++){
        if(
            a < MENU_LEVEL_PER_PAGE * (menu->currentPage + 1) &&
            a >= MENU_LEVEL_PER_PAGE * (menu->currentPage)
        ){
            if(menuButtonHandle(
                menu->levels + a,
                bw,
                bh,
                winscale
            )){
                ret = a+1;
            }
            
            bh += BUTTON_HEIGHT;
        } 
    }
    return ret;
}

void startMenuCreate(StartMenu *menu, MenuCore *mc){
    menu->enable = false;
    menu->currentPage = 0;
    menu->core = mc;
    menu->levelCount = 0;
    
    fontCreate(&menu->title, FONT_DEFAULT, 56, MENU_COLOR_TITLE, mc->ui.renderer);
    fontRender(&menu->title, MENU_TEXT_TITLE_START);
    
    menuButtonCreate(
        &menu->prev,
        "<<",
        NULL,
        &menu->core->ui,
        BUTTON_YELLOW,
        true
    );
    
    menuButtonCreate(
        &menu->next,
        ">>",
        NULL,
        &menu->core->ui,
        BUTTON_YELLOW,
        true
    );
    
    menuButtonCreate(
        &menu->random,
        MENU_TEXT_BUTTON_RANDOM,
        NULL,
        &menu->core->ui,
        BUTTON_BLUE,
        true
    );
    
    menuButtonCreate(
        &menu->back,
        MENU_TEXT_BUTTON_BACK,
        NULL,
        &menu->core->ui,
        BUTTON_RED,
        true
    );
}

void startMenuEnable(StartMenu *menu){
    menu->enable = true;
    
    startMenuScanLevels(NULL, &menu->levelCount);
    //SDL_Log("Found %d Levels!\n", (int)menu->levelCount);
    if(menu->levelCount == 0)
        return;
    //Make Space for the Level Path names
    menu->levelList = (char**)malloc(menu->levelCount * sizeof(char**));
    for(int a = 0; a < menu->levelCount; a++){
        menu->levelList[a] = (char*)malloc(MENU_LEVEL_PATH_LENGTH);
        memset(menu->levelList[a], 0, MENU_LEVEL_PATH_LENGTH);
    }
    startMenuScanLevels(menu->levelList, &menu->levelCount);    
    startMenuCreateButtons(menu);
}

void startMenuDisable(StartMenu *menu){
    menu->enable = false;
    menu->currentPage = 0;
    if(menu->levelCount){
        for(int a = 0; a < menu->levelCount; a++){
            free(menu->levelList[a]);
            menu->levelList[a] = NULL;
            menuButtonDestroy(menu->levels + a);
        }
        free(menu->levelList);
        menu->levelList = NULL;
        free(menu->levels);
        menu->levels = NULL;
        menu->levelCount = 0;
    }
}

unsigned int startMenuHandle(StartMenu *menu, float winscale){
    if(!menu->enable)
        return 0;
   
    menuCoreDrawBackground(menu->core);
    fontDraw(
        &menu->title,
        menu->core->area.x + 20,
        menu->core->area.y + 20        
    );
    
    unsigned int ret = 0;
    unsigned short bh = 192 + BUTTON_HEIGHT * (MENU_LEVEL_PER_PAGE);
    unsigned short bw = 16;
    
    if(menuButtonHandle(
        &menu->prev,
        bw,
        bh,
        winscale
    )){
        if(menu->currentPage)
            menu->currentPage--;
        
        menuCoreMouseCD(menu->core);
    }
    if(menuButtonHandle(
        &menu->next,
        bw + BUTTON_WIDTH_SHORT,
        bh,
        winscale
    )){
        if((1 + menu->currentPage) * MENU_LEVEL_PER_PAGE < menu->levelCount){
            menu->currentPage++;
            menuCoreMouseCD(menu->core);
            //SDL_Log("Loading Page %d\n", menu->currentPage);
        }
    }
    bh += BUTTON_HEIGHT;
    ret |= menuButtonHandle(
        &menu->random,
        bw + BUTTON_WIDTH_SHORT,
        bh,
        winscale
    ) << MENU_START_BUTTON_RANDOM;
    ret |= menuButtonHandle(
        &menu->back,
        bw,
        bh,
        winscale
    ) << MENU_START_BUTTON_END;
    
    ret |= startMenuHandleLevelButtons(menu, winscale) << MENU_START_BUTTON_LEVELS_OFFSET;
    
    return ret; 
}

void startMenuDestroy(StartMenu *menu){
    menuButtonDestroy(&menu->prev);
    menuButtonDestroy(&menu->next);
    menuButtonDestroy(&menu->random);
    menuButtonDestroy(&menu->back);
}

int menuHandleAll(ClientSock *client, MainMenu* main, StartMenu* start, LobbyMenu *lobby, PauseMenu *pause, float winscale){
        unsigned int ret = 0;
        /*----------------------------------------------------------*/
        /*                Main Menu                                 */
        /*----------------------------------------------------------*/
        uint8_t mmres = mainMenuHandle(main, lobby, winscale);
        mmres = main->core->mouse_cooldown ? 0 : mmres;
        if(mmres & (1 << MENU_MAIN_BUTTON_END)){
            return -1;
        }else if(mmres & (1 << MENU_MAIN_BUTTON_PLAY)){
            //startMenuHandle(&startMenu, winscale);
            menuCoreMouseCD(main->core);
            startMenuEnable(start);
            main->enable = false;
            
            if(client->serverstatus == CLIENT_SOCK_SERVER_NO_UP){
                serverStart(lobby->enable ? 0 : 1);
                client->serverstatus = lobby->enable ? CLIENT_SOCK_SERVER_UP_MULTIPLAYER : CLIENT_SOCK_SERVER_UP_SINGLEPLAYER;
                SDL_Delay(100);
                netSockSetup(client);
            }
        }else if(mmres & (1 << MENU_MAIN_BUTTON_LOBBY)){
            menuCoreMouseCD(main->core);
            
            if(lobby->enable){
                lobbyMenuDisable(lobby);
            }else{
                lobbyMenuEnable(lobby, client);
            }
            
            //if(client->serverstatus != (lobby->enable ? CLIENT_SOCK_SERVER_UP_MULTIPLAYER : CLIENT_SOCK_SERVER_UP_SINGLEPLAYER)){
                netKillServer(client);
                serverStart(lobby->enable ? 0 : 1);
                client->serverstatus = lobby->enable ? CLIENT_SOCK_SERVER_UP_MULTIPLAYER : CLIENT_SOCK_SERVER_UP_SINGLEPLAYER;
            //}
            //Give the Server some time to start...
            SDL_Delay(100);
            netSockSetup(client);
            SDL_Delay(10);
            netLobbyDiscovery(client);   
            if(lobby->enable)
                lobbyMenuEnable(lobby, client);
        }else if(mmres & (1 << MENU_MAIN_BUTTON_JOIN)){
            menuCoreMouseCD(main->core);
            //serverStart(0);
        }else if(mmres & (1 << MENU_MAIN_BUTTON_OPTIONS)){
            menuCoreMouseCD(main->core);              
        }
        /*----------------------------------------------------------*/
        /*                    Start menu                            */
        /*----------------------------------------------------------*/
        mmres = startMenuHandle(start, winscale);
        mmres = start->core->mouse_cooldown ? 0 : mmres;
        if(mmres & (1 << MENU_START_BUTTON_END)){
            menuCoreMouseCD(start->core);
            main->enable = true; 
            startMenuDisable(start); 
        }else if(mmres & (1 << MENU_START_BUTTON_RANDOM)){
            menuCoreMouseCD(start->core);
            SDL_Log("Random Level!");
        }else if(mmres >> MENU_START_BUTTON_LEVELS_OFFSET){
            unsigned int level = mmres >> MENU_START_BUTTON_LEVELS_OFFSET;
            if(start->levelCount >= level){
                if(client->flags & CLIENT_SOCK_FLAG_KILL || client->flags & CLIENT_SOCK_FLAG_UNINIT)
                    netSockSetup(client);
                client->level = level;
                SDL_Log("%s\tMy Level is: %s\n", TAG, start->levelList[client->level - 1]);
                netLoadLevel(client, start->levelList[client->level - 1]);
                menuCoreMouseCD(start->core);
                startMenuDisable(start);
                return MENU_ACTION_START_GAME;
            }else{
                SDL_LogError(0,"%s\tInvalid Level\n", TAG);
            }
            menuCoreMouseCD(start->core);
        }
        /*----------------------------------------------------------*/
        /*                   Pause Menu                             */
        /*----------------------------------------------------------*/
        // Determine if game is running by testing if one of the main menus is enabled
        mmres = pauseMenuHandle(pause, winscale, !(main->enable || start->enable));
        mmres = start->core->mouse_cooldown ? 0 : mmres;
        if(mmres & (1 << MENU_PAUSE_BUTTON_ENABLE)){
            menuCoreMouseCD(pause->core);
            pauseMenuEnable(pause);
            SDL_Log("Eable pause!\n");
        }else if(mmres & (1 << MENU_PAUSE_BUTTON_RESUME)){
            menuCoreMouseCD(pause->core);
            pauseMenuDisable(pause);
            SDL_Log("Resume pause!\n");
        }else if(mmres & (1 << MENU_PAUSE_BUTTON_STOP)){
            //If the player clicks on stop Return to main menu
            menuCoreMouseCD(pause->core);
            pauseMenuDisable(pause);
            main->enable = true;
        }
        
        lobbyMenuHandle(lobby);
        
        //pauseMenuHandle(pause, winscale);
        
        return 0;       
}

