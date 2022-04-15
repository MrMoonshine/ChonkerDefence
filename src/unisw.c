#include "unisw.h"
static const char* TAG = "[UNISW]";

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

static int electGoal(LevelServer *ls){
    for(uint8_t a = 0; a < ls->height; a++){
        if(isPath(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, ls->width - 1, a))){
            ls->goal.x = ls->width - 1;
            ls->goal.y = a;
            return 0;
        }
    }
    return -1;
}
// Loop through all Paths until goal node is found
static Path * getGoalNode(LevelServer *ls){
    Path *elem = ls->paths;
    while(elem){
        if(
            elem->x == ls->goal.x &&
            elem->y == ls->goal.y
        ){
            return elem;
        }
        elem = elem->next;
    }
    return NULL;
}

int uniswCreate(UNISW *unisw, LevelServer *ls){
    // Elect Goal. Exit on failure
    if(electGoal(ls) < 0){
        SDL_LogError(0, "%s\tNo goal found in Level", TAG);
        return -1;
    }
    // Create List with all paths
    ls->paths = NULL;
    for(int y = 0; y < ls->height; y++){
        for(int x = 0; x < ls->width; x++){
            if(isPath(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, x, y))){
                insertPath(&ls->paths, x, y);
            }
        }
    }
    // Debug
    //printPath(ls->paths);
    //Get Goal Node
    Path * goal = getGoalNode(ls);
    if(!goal)
        return -1;
    // Last node has no other nodes to follow
    goal->follow = NULL;
    
    return 0; 
}

void uniswDestroy(UNISW *unisw){
    
}
