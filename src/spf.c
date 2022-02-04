#include "spf.h"
static const char* TAG = "[SPF]";

static bool isPath(uint8_t a){
    return a == LEVEL_NODE_CODE_PATH || a == LEVEL_NODE_CODE_BRIDGE; 
}

//Print a 32-bit unsigned integer in an IPv4 like fashion
static void dottedDecimal(char* buff, uint32_t num){
    sprintf(
        buff,
        "%d.%d.%d.%d",
        (num & 0xff000000) >> 24,
        (num & 0x00ff0000) >> 16,
        (num & 0x0000ff00) >> 8,
        (num & 0x000000ff) >> 0
    );
}
// Instead of having a bunch of 2-Way states, just discover each Node from the provided map data
// This function also assigns the goal
static int spfSolicitateNodes(LevelServer *ls, SPFRouter *nodes, uint8_t* nodeCount){
    uint8_t iCounter = 0;
    uint32_t routerID = 0;
    for(uint8_t y = 0; y < ls->height; y++){
        for(uint8_t x = 0; x < ls->width; x++){
            if(isPath(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, x, y))){
                uint8_t tx, ty, counter = 0;
                tx = x - 1;
                ty = y;
                if(tx >= 0)
                    if(isPath(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, tx, ty)))
                        counter++;
                tx = x;
                ty = y - 1;
                if(ty >= 0)
                    if(isPath(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, tx, ty)))
                        counter++;
                tx = x + 1;
                ty = y;
                if(tx < ls->width)
                    if(isPath(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, tx, ty)))
                        counter++;
                tx = x;
                ty = y + 1;
                if(ty < ls->height)
                    if(isPath(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, tx, ty)))
                        counter++;
                        
                routerID = 0 | (iCounter << 16) | (x << 8) | y;
                if(counter > 2){
                    routerID |= SPF_TYPE_INTERSECTION << 24;
                    if(nodes){
                        //nodes[iCounter].x = x;
                        //nodes[iCounter].y = y;
                        nodes[iCounter].routerID = routerID;
                    }
                    iCounter++;
                }else if(counter == 1){
                    // Is it located at the right or left edge of the screen
                    if(x == 0 || x == ls->width - 1){
                        routerID |= SPF_TYPE_START << 24;
                        if(nodes){
                            //nodes[iCounter].x = x;
                            //nodes[iCounter].y = y;
                            nodes[iCounter].routerID = routerID;
                        }
                        iCounter++;
                        
                    }
                }
            }
        }
    }
    if(!nodes)
        *nodeCount = iCounter;
    else{
        //Solicitate goal
        bool goalFound = false;
        uint8_t lastStartIndex = 0;
        char buff[16] = "";
        for(uint8_t a = 0; a < iCounter; a++){
            uint32_t rid = nodes[a].routerID;
            if(((rid & 0xff000000) >> 24) == SPF_TYPE_START){
                lastStartIndex = a;
                if(((rid >> 8) & 0xff) == ls->width - 1){
                    nodes[a].routerID &= 0x00ffffff;
                    nodes[a].routerID |= SPF_TYPE_GOAL << 24;
                    dottedDecimal(buff, nodes[a].routerID);
                    goalFound = true;
                    break;
                }
            }
        }
        //Fallback action if no goal has been found on the right
        if(!goalFound){
            nodes[lastStartIndex].routerID &= 0x00ffffff;
            nodes[lastStartIndex].routerID |= SPF_TYPE_GOAL << 24;
            dottedDecimal(buff, nodes[lastStartIndex].routerID);
        }
        SDL_Log("%s\tGoal node is: %s\n",TAG, buff);
    }
    return 0;
}

int spfCreate(SPF *spf, LevelServer *ls){
    spf->links = NULL;
    spf->routers = NULL;
    uint8_t nodeCount = 0;
    spfSolicitateNodes(ls,NULL,&nodeCount);
    spf->routers = (SPFRouter*)malloc(sizeof(SPFRouter)*nodeCount);
    if(!spf->routers)
        return -1;
    spfSolicitateNodes(ls, spf->routers, &nodeCount);
    char buff[16] = "";
    //Count Start Nodes
    ls->startCount = 0;
    for(int a = 0; a < nodeCount; a++){
        dottedDecimal(buff, spf->routers[a].routerID);
        SDL_Log("%s\tRouter ID is: %s\n",TAG, buff);
        if(spf->routers[a].routerID >> 24 == SPF_TYPE_START)
            ls->startCount++;
    }
    //Allocate space for them
    (ls->starts) = (Path**)malloc(sizeof(Path*) * ls->startCount);
    if(ls->starts == NULL)
        return -1;
    /*
        ***
        * 
        *   CONTINUE HERE...
        * 
        ***
     */
    return 0; 
}

void spfDestroy(SPF *spf){
    if(spf->routers){
        free(spf->routers);
        spf->routers = NULL;
    }
    
    if(spf->links){
        free(spf->links);
        spf->links = NULL;
    }
}
