#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <signal.h>

#include "image.h"
#include "text.h"
#include "cat.h"
#include "level.h"
#include "server.h"

#include "menu/menu.h"

bool windowHandleEvents(SDL_Window* window, SDL_Renderer* renderer, float *scale){
    float scalefactor = 1.0f;
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT: return false; break;
            case SDL_WINDOWEVENT:{
                if(event.window.windowID == SDL_GetWindowID(window)){
                    switch (event.window.event){
                        case SDL_WINDOWEVENT_SIZE_CHANGED:{
                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            scalefactor = (float)h/(float)APP_HEIGHT;
                            //printf("Scale Factor altered to: %d\n", scalefactor);
                            SDL_RenderSetScale(renderer,scalefactor,scalefactor);
                            *scale = scalefactor;
                        }break;
                        default: break;
                    }
                }
                break;
            }
            default: break;
        }
    }
    return true;    
}

int main(int argc, char *argv[]){    
    //The window we'll be rendering to
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    
    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) < 0 )
    {
        fprintf(stderr ,"SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    window = SDL_CreateWindow(
        APP_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        APP_WIDTH,
        APP_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    
    if(window == NULL){
        fprintf(stderr ,"SDL could not initialize Window! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    //Initialize other stuff
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    
    //Setup random
    srand(time(NULL));

    //Setup Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    Image wallpaper;
    textureLoad(&wallpaper, renderer, "/home/david/Programmieren/C/ChonkerDefenceAssets/assets/textures/ui/wallpaper.png");
    
    CTS orangeCheckered;
    ctsCreate(&orangeCheckered, renderer, "orange_checkered");
    
    Cat cat1;
    Cat cat2;
    Cat cat3;
    Cat cat4;
    catCreate(&cat1, &orangeCheckered, OH_LAWD_HE_COMIN);
    catCreate(&cat2, &orangeCheckered, A_HECKIN_CHONKER);
    catCreate(&cat3, &orangeCheckered, HE_CHOMNK);
    catCreate(&cat4, &orangeCheckered, OH_LAWD_HE_COMIN);
    
    cat2.position.y *= 2;
    cat2.rotation = LEFT;
    cat3.position.y *= 3;
    cat3.rotation = RIGHT;
    cat4.position.y *= 4;
    cat4.rotation = REAR;
    
    Font font;
    SDL_Colour yellow = {255,64,16,0};
    fontCreate(&font, TEST_FONT, 20, yellow, renderer);
    fontRender(&font, "Oh Lawd He Commin'!");

    ClientSock client;
    netInitClientMeta(&client);
    
    Level lvl1;
    levelCreateEmpty(&lvl1);
    //levelCreateFromFile(&lvl1, TEST_LEVEL, renderer);
    
    /*----------------------------------------------------------*/
    /*               Building Menus                             */
    /*----------------------------------------------------------*/
    MenuCore menuCore;
    menuCoreInit(&menuCore,window, renderer);
    MainMenu mainMenu;
    mainMenuCreate(&mainMenu, &menuCore);
    StartMenu startMenu;
    startMenuCreate(&startMenu, &menuCore);
    LobbyMenu lobbyMenu;
    lobbyMenuCreate(&lobbyMenu, &menuCore);
    float winscale = 1.0f;
    
    //levelSockSetup(&client);
    unsigned int lastTime = 0, currentTime = 0;
    while(windowHandleEvents(window, renderer, &winscale)){
        SDL_RenderClear(renderer);      
        
        levelDraw(&lvl1);
        if(!lvl1.loaded)
            textureDraw(&wallpaper, 0, 0, SDL_FLIP_NONE, NULL);
            

        catDraw(&cat1);
        catDraw(&cat2);
        catDraw(&cat3);
        catDraw(&cat4);
        
        fontDraw(&font, 160, 16);
        
        if(menuCore.mouse_cooldown)
            menuCore.mouse_cooldown--;
        
        int mres = menuHandleAll(&client, &mainMenu, &startMenu, &lobbyMenu, winscale);
        if(mres < 0){
            break;
        }else if(mres == MENU_ACTION_START_GAME){
            levelCreateFromServer(&lvl1, &client, renderer);
            netLobbyDiscovery(&client);
            lobbyMenuEnable(&lobbyMenu, &client);
        }
        
        currentTime = SDL_GetTicks();
        if(currentTime > lastTime + CLEINT_SOCK_EVENT_QUEUE_POLL_PERIOD){
            if(client.serverstatus != CLIENT_SOCK_SERVER_NO_UP)
                levelHandle(&lvl1, &client);
            lastTime = currentTime;
        }
    
        SDL_RenderPresent(renderer);
    }
    
    
    levelDestroy(&lvl1);
    fontDestroy(&font);
    ctsDestroy(&orangeCheckered);
    catDestroy(&cat1);
    catDestroy(&cat2);
    catDestroy(&cat3);
    catDestroy(&cat4);
    
    mainMenuDestroy(&mainMenu);
    startMenuDestroy(&startMenu);
    lobbyMenuDestroy(&lobbyMenu);
    menuCoreDestroy(&menuCore);
    
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    SDL_Log("Goodbye :3\n");
    return 0;
}
