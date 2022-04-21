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

// Return a pointer to the path with the given coordinates. NULL on fail
static Path * whereXYmatchPath(Path *head, uint8_t x, uint8_t y){
    Path *elem = head;
    while(elem){
        if(elem->x == x && elem->y == y)
            return elem;
        elem = elem->next;
    }
    return NULL;
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
    Path * currentNode = goal;
    // Last node has no other nodes to follow
    goal->follow = NULL;
    // Nodes that require Processing NOTE: Do NOT use follow parameter on open set
    Path * openSet = NULL;
    insertPath(&openSet, goal->x, goal->y);
    // Ladies and Gentlemen, the time has come... to finally trace the path for the mice
    while(lengthPath(openSet)){
        // Match from original list
        currentNode = whereXYmatchPath(ls->paths, openSet->x, openSet-> y);
        if(currentNode == NULL){
            SDL_LogError(0, "%s\tFailed to parse node (%d|%d) from open set!\n", TAG, openSet->x, openSet-> y);
            deletePath(&openSet, openSet);
        }
        // Check all neighbours
        for(uint8_t a = 0; a < 4; a++){
            short x = currentNode->x, y = currentNode->y;
            switch(a){
                case 0: x++; break;
                case 1: x--; break;
                case 2: y++; break;
                case 3: y--; break;
                default: break;
            }
            //out of dimension
            if(y < 0 || x < 0 || y > ls->height || x > ls->width)
                continue;
            // Check if exists
            Path * adjacent = whereXYmatchPath(ls->paths, x, y);
            if(adjacent == NULL)
                continue;
            //SDL_Log("%s\tAdjacent node (%d|%d)\n",TAG, x, y);
            // If goal or already known node continue 
            if(adjacent == goal || adjacent->follow)
                continue;
            SDL_Log("%s\tNew Node in Open Set (%d|%d)\n",TAG, x, y);
            // Finally valid node
            insertPath(&openSet, x, y);
        }
        // Delete first
        deletePath(&openSet, openSet);
        printPath(openSet);
    };
    deleteAllPath(&openSet);
    return 0; 
}

void uniswDestroy(UNISW *unisw){
    
}
