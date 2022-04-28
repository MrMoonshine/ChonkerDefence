#include "mousepath.h"
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

static int countStarts(LevelServer *ls, uint8_t x){
    int count = 0;
    for(int y = 0; y < ls->height; y++){
        Path * p = whereXYmatchPath(ls->paths, x, y);
        if(p == NULL)
            continue;
        count++;
    }
    return count;
}

static int mousepathSearchStarts(LevelServer *ls){
    // Count
    ls->startCount = countStarts(ls, 0);
    ls->startCount += countStarts(ls, ls->width - 1) - 1;
    //SDL_Log("Found %d start nodes\n", (int)ls->startCount);
    // Allocate
    ls->starts = (Path**)malloc(ls->startCount * sizeof(Path*));
    if(ls->starts == NULL)
        return -1;
    // Memset NULL all
    for(int a = 0; a < ls->startCount; a++){
        ls->starts[a] = NULL;
    }
    // Helper var
    bool ignoredGoal = false;
    uint8_t index = 0;
    // Add from left side
    for(int y = 0; y < ls->height; y++){
        Path * p = whereXYmatchPath(ls->paths, 0, y);
        if(p == NULL)
            continue;
        ls->starts[index++] = p;
    }
    // Add from right side
    for(int y = 0; y < ls->height; y++){
        Path * p = whereXYmatchPath(ls->paths, ls->width - 1, y);
        if(p == NULL)
            continue;
        // Skip it once
        if(ignoredGoal)
            ls->starts[index++] = p;
        else
            ignoredGoal = true;
    }
#ifdef DEBUG_PATH
    for(int a = 0; a < ls->startCount; a++){
        SDL_Log("Start node X = %d\tY = %d\n", ls->starts[a]->x, ls->starts[a]->y);
    }
#endif
    return ls->startCount;
}

int mousepathCreate(LevelServer *ls){
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
    //Get Goal Node
    Path * goal = getGoalNode(ls);
    if(!goal)
        return -1;
    SDL_Log("%s\tGoal is (%d|%d)\n",TAG, goal->x, goal->y);
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
        // Delete first
        deletePath(&openSet, openSet);
        if(currentNode == NULL){
            SDL_LogError(0, "%s\tFailed to parse node (%d|%d) from open set!\n", TAG, openSet->x, openSet-> y);
            //deletePath(&openSet, openSet);
            continue;
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
            //SDL_Log("%s\tNew Node in Open Set (%d|%d)\n",TAG, x, y);
            adjacent->follow = currentNode;
            // Finally valid node
            insertPath(&openSet, x, y);
        }
    };
    deleteAllPath(&openSet);
#ifdef DEBUG_PATH
    // Finally show all Nodes With Next
    Path *elem = ls->paths;
    while(elem){
        if(elem->follow)
            SDL_Log("Path (%d|%d) -> (%d | %d)", elem->x, elem->y, elem->follow->x, elem->follow->y);
        else
            SDL_Log("Path (%d|%d) -> NULL", elem->x, elem->y);
        elem = elem->next;
    }
    SDL_Log("\n");
#endif
    // Search for start nodes
    mousepathSearchStarts(ls);
    return 0; 
}
