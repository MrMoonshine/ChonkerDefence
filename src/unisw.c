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
    printPath(ls->paths);
    //Get Goal Node
    Path * goal = getGoalNode(ls);
    if(!goal)
        return -1;
    Path * currentNode = goal;
    // Last node has no other nodes to follow
    goal->follow = NULL;
    // A buffer to store intersections that haven't been fully processed
    size_t intersectionBufferSize = 2;
    Path** intersectionBuffer = (Path**)malloc((sizeof(Path*) * intersectionBufferSize));
    if(intersectionBuffer == NULL)
        return -1;
    // Ladies and Gentlemen, the time has come... to finally trace the path for the mice
    Path * neighbours[4];
    uint8_t neighbourCount = 0;
    while(neighbourCount < 30 || currentNode == goal){
        //Reset
        //neighbourCount = 0;
        memset(neighbours, 0, 4);
        // Iterating
        Path *elem = ls->paths;
        while(elem){
            if(
                abs(elem->x - currentNode->x) <= 1 &&
                abs(elem->y - currentNode->y) <= 1
            ){
                // Don't allow diagonal Nodes!
                if(
                    ((abs(elem->x - currentNode->x) == 1) !=
                    (abs(elem->y - currentNode->y) == 1))
                ){
                    //(elem->x != ls->goal.x && elem->y != ls->goal.y)
                    SDL_Log("%s\tNeighbouring node (%d|%d)\n",TAG,elem->x,elem->y);
                    if(elem->follow == NULL && elem != goal){
                        elem->follow = currentNode;
                        currentNode = elem;
                        neighbourCount++;
                        break;
                    }
                }                
            }
            elem = elem->next;
        }
        //break;
    }
    free(intersectionBuffer);
    return 0; 
}

void uniswDestroy(UNISW *unisw){
    
}
