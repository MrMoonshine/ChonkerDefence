#include "level.h"

static const char *TEST_WATER = "/home/david/Programmieren/C/ChonkerDefenceAssets/assets/textures/world/wasser.png";
static const char *TEST_GRASS = "/home/david/Programmieren/C/ChonkerDefenceAssets/assets/textures/world/grastest.png";
static const char *TEST_SHORES = "/home/david/Programmieren/C/ChonkerDefenceAssets/assets/textures/world/default_shore.png";
static const char *TEST_CAT_PLACE = "/home/david/Programmieren/C/ChonkerDefenceAssets/assets/textures/world/cat_placement_indicator.png";
    
static const char *TAG = "[LEVEL]";

static int ltsCreate(LTS *lts, const char* texture_set_name, SDL_Renderer *renderer){
    lts->variationGrass.frameNumber = 0;
    lts->variationGrass.frameCount = 1;
    lts->variationGrass.period = 0;
    lts->variationWater.frameNumber = 0;
    lts->variationWater.frameCount = 1;
    lts->variationWater.period = 0;
    
    int ret = 0;
    
    char texturepath[128];
    strcpy(texturepath, ASSET_PATH);
    strcat(texturepath, "textures/world/");
    
    SDL_Log("%s\tLoading Texture Set: %s\n", TAG, texture_set_name);
    
    ret |= textureLoad(&lts->grass, renderer, TEST_GRASS);
    SDL_Point varsize = textureGetSize(&lts->grass);
    lts->variationGrass.frameCount = varsize.y / varsize.x;
    
    ret |= textureLoad(&lts->shore, renderer, TEST_SHORES);
    varsize = textureGetSize(&lts->shore);
    lts->variationShores.frameCount = varsize.y / varsize.x;
    
    //printf("Grass has %d different textures\n", lts->variationGrass.frameCount);
    ret |= textureLoad(&lts->water, renderer, TEST_WATER);
    varsize = textureGetSize(&lts->water);
    lts->variationWater.frameCount = varsize.y / varsize.x;
    
    ret |= textureLoad(&lts->catPlace, renderer, TEST_CAT_PLACE);
    varsize = textureGetSize(&lts->catPlace);
    lts->variationCatPlacePlayer.frameCount = varsize.y / varsize.x;
    
    char buffer[192];
    sprintf(buffer, "%s%s_path_straight.png", texturepath, texture_set_name);
    ret |= textureLoad(&lts->pathStraight, renderer, buffer);
    sprintf(buffer, "%s%s_path_intersection.png", texturepath, texture_set_name);
    ret |= textureLoad(&lts->pathIntersection, renderer, buffer);
    sprintf(buffer, "%s%s_path_curve.png", texturepath, texture_set_name);
    ret |= textureLoad(&lts->pathCurve, renderer, buffer);
    sprintf(buffer, "%s%s_bridge.png", texturepath, texture_set_name);
    ret |= textureLoad(&lts->bridge, renderer, buffer);
    return ret;
}

static void ltsDestroy(LTS *lts){
    textureDestroy(&lts->grass);
    textureDestroy(&lts->shore);
    textureDestroy(&lts->water);
    textureDestroy(&lts->bridge);
    textureDestroy(&lts->pathStraight);
    textureDestroy(&lts->pathCurve);
    textureDestroy(&lts->pathIntersection);
    textureDestroy(&lts->catPlace);
}

static void levelResetValues(Level *level){
  level->width = 0;
  level->height = 0;
  strcpy(level->name, "");
  if(level->loaded)
      ltsDestroy(&level->lts);
}

//Works like array[x][y]
int8_t levelNodeInfoAt(uint8_t *data, size_t size, uint8_t width, uint8_t x, uint8_t y){
    unsigned short index = 0;
    index = (y * width + x)/2;
    if(x > width || index >= size){
        //SDL_Log("Out of range: i%d s%d w%d", index, size, width);
        return -1;
    }
    
    uint8_t ret = data[index];
    ret = (y * width + x) % 2 ? ret & 0x0F : ((ret & 0xF0) >> 4);
    return ret;
}

void levelSetNodeInfoAt(uint8_t *data, size_t size, uint8_t width, uint8_t x, uint8_t y, uint8_t node){
    unsigned short index = 0;
    index = (y * width + x)/2;
    if(x > width || index >= size)
        return;
    
    uint8_t shift = (y * width + x) % 2;
    data[index] &= shift == 0 ? 0xF0 : 0x0F;
    data[index] |= (node & 0x0f) << (4*shift);
}

int levelCreateFromServer(Level *level, ClientSock *csock, SDL_Renderer *renderer){
    if(level->loaded)
        levelDestroy(level);
    //Reset Cat Nodes
    level->catSpotCount = 0;
    
    const size_t len = 2;
    uint8_t *buffer;
    buffer = (uint8_t*)malloc(len);
    if(!buffer){
        perror(TAG);
        return -1;
    }
    buffer[0] = CD_NET_CODE_GET_VAR;
    
    //Get name
    buffer[1] = CD_NET_VAR_TITLE;
    send(csock->sock, buffer, len, 0);
    read(csock->sock, level->name, LEVEL_TITLE_LENGTH);
    //Get Style
    buffer[1] = CD_NET_VAR_STYLE;
    char* recvbuff;
    recvbuff = (char*)malloc(LEVEL_STYLE_LENGTH);
    if(!recvbuff){
        perror(TAG);
        return -1;
    }
    memset(recvbuff, 0, LEVEL_STYLE_LENGTH);
    send(csock->sock, buffer, len, 0);
    read(csock->sock, recvbuff, LEVEL_STYLE_LENGTH);
    if(ltsCreate(&level->lts, recvbuff, renderer) < 0){
        SDL_LogWarn(0,"%s\tMissing Textures for Style: %s. Hence: loading default\n", TAG, recvbuff);
        strcpy(recvbuff, "default");
        ltsCreate(&level->lts, recvbuff, renderer);
    }
    free(recvbuff);
    recvbuff = NULL;
    
    //get width
    buffer[1] = CD_NET_VAR_WIDTH;
    send(csock->sock, buffer, len, 0);
    read(csock->sock, &level->width, 1);
    if(level->width == 0)
        level->width = LEVEL_WIDTH;
    //get height
    buffer[1] = CD_NET_VAR_HEIGHT;
    send(csock->sock, buffer, len, 0);
    read(csock->sock, &level->height, 1);
    if(level->height == 0)
        level->height = LEVEL_HEIGHT;
    
    uint8_t *ldat8;
    size_t datCount8 = (level->width * level->height) / 2 + (level->width * level->height % 2);
    ldat8 = (unsigned char*)malloc(datCount8);
    memset(ldat8, LEVEL_NODE_CODE_GRASS | LEVEL_NODE_CODE_GRASS << 4, datCount8);
    //Request Map data
    buffer[0] = CD_NET_CODE_RECV_MAP_RAW;
    buffer[1] = 0;
    send(csock->sock, buffer, 1, 0);
    read(csock->sock, ldat8, datCount8);    
    SDL_Log("%s\tLoaded Map Data!\n", TAG);
    
    uint8_t playerCount = 0;
    buffer[0] = CD_NET_CODE_GET_VAR;
    buffer[1] = CD_NET_VAR_PLAYER_COUNT;
    send(csock->sock, buffer, 2, 0);
    read(csock->sock, &playerCount, 1);
    SDL_Log("%s\tServer has %d players\n", TAG, playerCount);
    
    /*--------------------------------------*/
    /*            Level Surface             */
    /*--------------------------------------*/
    level->background.renderer = renderer;
    level->background.texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_TARGET,
        level->width * NODE_SIZE,
        level->height * NODE_SIZE
    );
    if(level->background.texture)
        level->background.valid = true;
    //Assign Renderer to Backgroung
    SDL_SetRenderTarget(renderer, level->background.texture);
    SDL_RendererFlip randflip = SDL_FLIP_NONE;

    for(int y = 0; y < level->height; y++){
        for(int x = 0; x < level->width; x++){
            uint8_t cont = levelNodeInfoAt(ldat8, datCount8, level->width, x, y);            
            switch(rand()%3){
                case 1: randflip = SDL_FLIP_HORIZONTAL; break;
                //case 2: randflip = SDL_FLIP_VERTICAL; break;
                default: randflip = SDL_FLIP_NONE; break;
            }
            
            if(cont == LEVEL_NODE_CODE_WATER || cont == LEVEL_NODE_CODE_BRIDGE){
                level->lts.variationWater.frameNumber = rand() % level->lts.variationWater.frameCount;
                textureDraw(
                    &level->lts.water,
                    x * NODE_SIZE,
                    y * NODE_SIZE,
                    randflip,
                    &level->lts.variationWater
                );
                // Shores
                int8_t above = levelNodeInfoAt(ldat8, datCount8, level->width, x, y-1);
                if(
                    above == LEVEL_NODE_CODE_GRASS ||
                    above == LEVEL_NODE_CODE_PATH ||
                    above == LEVEL_NODE_CODE_CAT0 ||
                    above == LEVEL_NODE_CODE_CAT1
                ){
                    level->lts.variationShores.frameNumber = rand() % level->lts.variationShores.frameCount;
                    textureDraw(
                        &level->lts.shore,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        randflip,
                        &level->lts.variationShores
                    );
                }else if(cont == LEVEL_NODE_CODE_BRIDGE){
                    textureDraw(
                        &level->lts.bridge,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        randflip,
                        NULL
                    );
                }
            }else if(cont == LEVEL_NODE_CODE_GRASS || cont == LEVEL_NODE_CODE_CAT0 || cont == LEVEL_NODE_CODE_CAT1){
                level->lts.variationGrass.frameNumber = rand() % level->lts.variationGrass.frameCount;
                textureDraw(
                    &level->lts.grass,
                    x * NODE_SIZE,
                    y * NODE_SIZE,
                    randflip,
                    &level->lts.variationGrass
                );

                if(cont != LEVEL_NODE_CODE_GRASS){
                    level->catSpotCount++;
                }
            }else if(cont == LEVEL_NODE_CODE_PATH || cont == LEVEL_NODE_CODE_PATH_FWD){
                //Draw Grass to merge the path with the map
                textureDraw(
                    &level->lts.grass,
                    x * NODE_SIZE,
                    y * NODE_SIZE,
                    randflip,
                    NULL
                );
                uint8_t neighbors = 0;

                uint8_t ninfo = levelNodeInfoAt(ldat8, datCount8, level->width, x, y + 1);
                if(ninfo == LEVEL_NODE_CODE_PATH || ninfo == LEVEL_NODE_CODE_PATH_FWD)
                    neighbors |= 1 << 0;
                ninfo = levelNodeInfoAt(ldat8, datCount8, level->width, x + 1, y);
                if(ninfo == LEVEL_NODE_CODE_PATH || ninfo == LEVEL_NODE_CODE_PATH_FWD)
                    neighbors |= 1 << 1;
                ninfo = levelNodeInfoAt(ldat8, datCount8, level->width, x, y - 1);
                if(ninfo == LEVEL_NODE_CODE_PATH || ninfo == LEVEL_NODE_CODE_PATH_FWD)
                    neighbors |= 1 << 2;
                ninfo = levelNodeInfoAt(ldat8, datCount8, level->width, x - 1, y);
                if(ninfo == LEVEL_NODE_CODE_PATH || ninfo == LEVEL_NODE_CODE_PATH_FWD)
                    neighbors |= 1 << 3;
                
                switch(neighbors){
                    case LEVEL_NODE_AJACENCY_STRAIGHT_NS: textureDrawRotated(
                        &level->lts.pathStraight,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_NONE,
                        NULL,
                        90
                    ); break;
                    case LEVEL_NODE_AJACENCY_CURVE_NE: textureDrawRotated(
                        &level->lts.pathCurve,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_NONE,
                        NULL,
                        0
                    ); break;
                    case LEVEL_NODE_AJACENCY_CURVE_ES: textureDrawRotated(
                        &level->lts.pathCurve,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_VERTICAL,
                        NULL,
                        0
                    ); break;
                    case LEVEL_NODE_AJACENCY_CURVE_WN: textureDrawRotated(
                        &level->lts.pathCurve,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_HORIZONTAL,
                        NULL,
                        0
                    ); break;
                    case LEVEL_NODE_AJACENCY_CURVE_WS: textureDrawRotated(
                        &level->lts.pathCurve,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_NONE,
                        NULL,
                        180
                    ); break;
                    case LEVEL_NODE_AJACENCY_INTERSECTION_NES: textureDrawRotated(
                        &level->lts.pathIntersection,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_NONE,
                        NULL,
                        270
                    ); break;
                    case LEVEL_NODE_AJACENCY_INTERSECTION_NEW: textureDrawRotated(
                        &level->lts.pathIntersection,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_NONE,
                        NULL,
                        0
                    ); break;
                    case LEVEL_NODE_AJACENCY_INTERSECTION_NSW: textureDrawRotated(
                        &level->lts.pathIntersection,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_NONE,
                        NULL,
                        90
                    ); break;
                    case LEVEL_NODE_AJACENCY_INTERSECTION_ESW: textureDrawRotated(
                        &level->lts.pathIntersection,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_NONE,
                        NULL,
                        180
                    ); break;
                    default: textureDrawRotated(
                        &level->lts.pathStraight,
                        x * NODE_SIZE,
                        y * NODE_SIZE,
                        SDL_FLIP_NONE,
                        NULL,
                        0
                    ); break;
                }
            }
        }
    }    
    //Handle Cat Nodes:
    level->catSpots = (uint8_t*)malloc(level->catSpotCount * 3);
    buffer[0] = CD_NET_CODE_GET_VAR;
    buffer[1] = CD_NET_VAR_PLAYER_NODES;
    send(csock->sock, buffer, 2, 0);
    //Transfer Done
    free(buffer);
    buffer = NULL;
    //Receive data
    read(csock->sock, level->catSpots, level->catSpotCount * 3);
    //Darw player Nodes
    for(int a = 0; a < level->catSpotCount; a++){
        switch(rand()%3){
            case 1: randflip = SDL_FLIP_HORIZONTAL; break;
            case 2: randflip = SDL_FLIP_VERTICAL; break;
            default: randflip = SDL_FLIP_NONE; break;
        }
        
        uint8_t pid =  level->catSpots[3*a + 0];
        uint8_t x =  level->catSpots[3*a + 1];
        uint8_t y =  level->catSpots[3*a + 2];
        level->lts.variationCatPlacePlayer.frameNumber = pid;
        //SDL_Log("%s\tDrawing node at (%d|%d) for Player %d.\n",TAG,x,y,pid);
        
        textureDraw(
            &level->lts.catPlace,
            x * NODE_SIZE,
            y * NODE_SIZE,
            randflip,
            &level->lts.variationCatPlacePlayer
        );
    }
    
    //Assign Renderer to default (Window)
    SDL_SetRenderTarget(renderer, NULL);
    //Free the tentative Data
    free(ldat8);
    //Querry the Background Texture
    SDL_QueryTexture(
        level->background.texture,
        NULL,
        NULL,
        &level->background.destination.w,
        &level->background.destination.h
    );
    
    SDL_SetRenderTarget(renderer, level->background.texture);
    SDL_Surface* surface = SDL_CreateRGBSurface(0, level->background.destination.w, level->background.destination.h, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, NULL);
    
    SDL_Log("%s\tLevel Loaded Sucessfully!", TAG);
    level->loaded = true;
    return 0;
}

int levelLoadFromFile(ClientSock *csock, const char* filepath){
    uint8_t *buffer;
    size_t len = 1 + strlen(filepath);
    int ret = 0;
    buffer = (uint8_t*)malloc(len);
    if(!buffer){
        perror(TAG);
        return -1;
    }
    memset(buffer, 0, len);
    buffer[0] = CD_NET_CODE_LOAD_LEVEL;
    strcpy((char*)buffer + 1, filepath);
    send(csock->sock, buffer, len, 0);
    free(buffer);
    buffer = NULL;
    
    uint8_t ok = 0;
    read(csock->sock, &ok, sizeof(uint8_t));
    if(ok != CD_NET_CODE_OK)
        ret = -1;
    
    return ret;
}

int levelHandle(Level *level, ClientSock *csock){
    size_t count = 0;
    ioctl(csock->sock, FIONREAD, &count);
    
    if(count < 1)
        return 0;
    
    /*char* buffer;
    buffer = (char*)malloc(queuelen);
    if(!buffer)
        return -1;
    strcpy(buffer, "");
    read(csock->sock, buffer, 7);
    SDL_Log("%s\tIoctl test: %s\n", TAG, buffer);
    
    free(buffer);*/
    return 0;
}

void levelCreate(Level *level ,int stage, const char* texture_set_name, SDL_Renderer *renderer){
    ltsCreate(&level->lts, texture_set_name, renderer);
}

void levelCreateEmpty(Level *level){
    level->loaded = false;
    levelResetValues(level);
}

void levelDraw(Level *level){
    if(!level->loaded)
        return;
    textureDraw(&level->background, 0, 0, SDL_FLIP_NONE, NULL);
}

void levelDestroy(Level *level){
    if(level->loaded){
        textureDestroy(&level->background);
        ltsDestroy(&level->lts);
        
        free(level->catSpots);
        level->catSpots = NULL;
    }
}
