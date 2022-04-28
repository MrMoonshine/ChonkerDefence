#include "server.h"
//Font: 3D-Ascii
// https://patorjk.com/software/taag/#p=display&f=3D-ASCII&t=Cat%20Spots
static const char *TAG = "[SERVER]";
static const char *TAG_GAME = "[SRV-GAME]";

static void shuffleArr(uint8_t *array, size_t n){
    if (n > 1){
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          uint8_t t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

static uint8_t srvCountPlayers(LevelSock *lso){
    uint8_t sp = 0;
    for(uint8_t a = 0; a < SERVER_CONNECTIONS_MAX; a++)
        sp += lso->csocks[a] > 0 ? 1 : 0;
    return sp;
}

//Is the Level "optimized" for a 2 player experience
static int srvLevelStrictCoop(LevelServer *ls){
    uint8_t ret = 0;
    for(int y = 0; y < ls->height; y++){
        for(int x = 0; x < ls->width; x++){
            switch(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, x, y)){
                case LEVEL_NODE_CODE_CAT0: {
                    ret |= 0b01;
                } break;
                case LEVEL_NODE_CODE_CAT1: {
                    ret |= 0b10;
                } break;
                default: break; 
            } 
        }
    }
    SDL_Log("%s\tLevel is optimized: %d\n", TAG, 0b11 ? ret : 0);
    // If both bits are high then it's a "optimized level"
    return ret == 0b11 ? ret : 0;
}

/*
 ________  ________  _________        ________  ________  ________  _________  ________      
|\   ____\|\   __  \|\___   ___\     |\   ____\|\   __  \|\   __  \|\___   ___\\   ____\     
\ \  \___|\ \  \|\  \|___ \  \_|     \ \  \___|\ \  \|\  \ \  \|\  \|___ \  \_\ \  \___|_    
 \ \  \    \ \   __  \   \ \  \       \ \_____  \ \   ____\ \  \\\  \   \ \  \ \ \_____  \   
  \ \  \____\ \  \ \  \   \ \  \       \|____|\  \ \  \___|\ \  \\\  \   \ \  \ \|____|\  \  
   \ \_______\ \__\ \__\   \ \__\        ____\_\  \ \__\    \ \_______\   \ \__\  ____\_\  \ 
    \|_______|\|__|\|__|    \|__|       |\_________\|__|     \|_______|    \|__| |\_________\
                                        \|_________|                             \|_________|
*/
static int srvLoadCatSpots(LevelServer *ls, LevelSock *lso){
    uint8_t playerCount = srvCountPlayers(lso);
    
    //Free data if exists...
    if(ls->catNodes){
        free(ls->catNodes);
        ls->catNodes = NULL;
    }
    ls->catNodes = (uint8_t*)malloc(ls->catNodesCount * 3);
    unsigned short counter = 0;
    uint8_t *tdata = ls->catNodes;
    if(playerCount == 2 && srvLevelStrictCoop(ls)){
        for(int y = 0; y < ls->height; y++){
            for(int x = 0; x < ls->width; x++){
                switch(levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, x, y)){
                    case LEVEL_NODE_CODE_CAT0: {
                        *(tdata++) = 0;
                        *(tdata++) = x;
                        *(tdata++) = y;
                    } break;
                    case LEVEL_NODE_CODE_CAT1: {
                        *(tdata++) = 1;
                        *(tdata++) = x;
                        *(tdata++) = y;
                    } break;
                    default: break;
                }
            }
        }
    }else{
        uint8_t id = 0;
        uint8_t *playerIDs;
        playerIDs = (uint8_t*)malloc(playerCount);
        if(!playerIDs)
            return -1;
        //Assign initial data for player election
        for(uint8_t a = 0; a < playerCount; a++)
            playerIDs[a] = a;
        
        for(int y = 0; y < ls->height; y++){
            for(int x = 0; x < ls->width; x++){
                uint8_t node = levelNodeInfoAt(ls->nodes, ls->nodesSize, ls->width, x, y);
                if(node == LEVEL_NODE_CODE_CAT0 || node == LEVEL_NODE_CODE_CAT1){
                    if(!(counter % playerCount)){
                        shuffleArr(playerIDs, playerCount);
                    }
                    *(tdata++) = playerIDs[counter % playerCount];
                    *(tdata++) = x;
                    *(tdata++) = y;
                    counter++;
                }
            }
        }
        free(playerIDs);
    }        
    return 0;
}
/*
 ___       _______   ___      ___ _______   ___               ___       ________  ________  ________  ___  ________   ________     
|\  \     |\  ___ \ |\  \    /  /|\  ___ \ |\  \             |\  \     |\   __  \|\   __  \|\   ___ \|\  \|\   ___  \|\   ____\    
\ \  \    \ \   __/|\ \  \  /  / | \   __/|\ \  \            \ \  \    \ \  \|\  \ \  \|\  \ \  \_|\ \ \  \ \  \\ \  \ \  \___|    
 \ \  \    \ \  \_|/_\ \  \/  / / \ \  \_|/_\ \  \            \ \  \    \ \  \\\  \ \   __  \ \  \ \\ \ \  \ \  \\ \  \ \  \  ___  
  \ \  \____\ \  \_|\ \ \    / /   \ \  \_|\ \ \  \____        \ \  \____\ \  \\\  \ \  \ \  \ \  \_\\ \ \  \ \  \\ \  \ \  \|\  \ 
   \ \_______\ \_______\ \__/ /     \ \_______\ \_______\       \ \_______\ \_______\ \__\ \__\ \_______\ \__\ \__\\ \__\ \_______\
    \|_______|\|_______|\|__|/       \|_______|\|_______|        \|_______|\|_______|\|__|\|__|\|_______|\|__|\|__| \|__|\|_______|
 */
static int serverPathDelete(LevelServer *ls){
    if(ls->starts){
        free(ls->starts);
    }
    ls->starts = NULL;
    // Delete saved Path nodes
    if(ls->paths){
        deleteAllPath(&ls->paths);
    }
    ls->paths = NULL;
    return 0;
}

int serverLoadLevel(LevelServer *ls, char* filename){
    ls->starts = NULL;
    ls->startCount = 0;    
    
    char *buffer;
    buffer = (char*)malloc(192);
    if(!buffer){
        perror(TAG);
        return -1;
    }
    memset(buffer, 0, 192);
    sprintf(buffer, "%slevels/%s", ASSET_PATH, filename);
    SDL_Log("%s\tOpening %s\n", TAG, buffer);
    FILE *fp = fopen(buffer, "r");
    free(buffer);
    buffer = NULL;
    if(!fp){
        perror("Error: ");
        return EIO;
    }
    
    /*--------------------------------------*/
    /*            Read Metadata             */
    /*--------------------------------------*/
    fread(ls->name, 1, LEVEL_TITLE_LENGTH, fp);
    fread(ls->style, 1, LEVEL_STYLE_LENGTH, fp);
    fread(&ls->width, sizeof(uint8_t), 1, fp);
    fread(&ls->height, sizeof(uint8_t), 1, fp);
    //Cat count
    fread(&ls->catNodesCount, sizeof(uint8_t), 1, fp);
    //Reserved bytes
    buffer = (char*)malloc(LEVEL_RESERVED_BYTE_COUNT);
    if(!buffer){
        perror(TAG);
        return -1;
    }
    fread(buffer, 1, LEVEL_RESERVED_BYTE_COUNT, fp);
    free(buffer);
    buffer = NULL;
    /*--------------------------------------*/
    /*            Read Map Data             */
    /*--------------------------------------*/
    //One Nibble per node
    ls->nodesSize = (ls->width*ls->height) / 2 + (ls->width*ls->height % 2);
    ls->nodes = (unsigned char*)malloc(ls->nodesSize);
    fread(ls->nodes, sizeof(uint8_t), ls->nodesSize, fp);
    fclose(fp);
    SDL_Log("%s\tLoaded %s with style %s\n", TAG, ls->name, ls->style);
    /*--------------------------------------*/
    /*   Mouse Path  Tracing                */
    /*--------------------------------------*/
    if(mousepathCreate(ls) < 0){
        SDL_LogError(0, "%s\tUnable to trace the path for the mice. Game is unplayable!\n", TAG);
        return -1;
    }
    return 0;
}

static int srvSendToAllClientsExcept(LevelSock *lso, LevelServer *lsr, uint8_t *data, size_t size, int sd){
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        if(lso->csocks[a]){
            if(lso->csocks[a] != sd)
                send(lso->csocks[a], data, size, 0);
        }
    }
    return 0;
}

static int srvSendToAllClients(LevelSock *lso, LevelServer *lsr, uint8_t *data, size_t size){
    return srvSendToAllClientsExcept(lso, lsr, data, size, 0);
}

/*
 ________   _______  _________  ___       __   ________  ________  ___  __    ___  ________   ________     
|\   ___  \|\  ___ \|\___   ___\\  \     |\  \|\   __  \|\   __  \|\  \|\  \ |\  \|\   ___  \|\   ____\    
\ \  \\ \  \ \   __/\|___ \  \_\ \  \    \ \  \ \  \|\  \ \  \|\  \ \  \/  /|\ \  \ \  \\ \  \ \  \___|    
 \ \  \\ \  \ \  \_|/__  \ \  \ \ \  \  __\ \  \ \  \\\  \ \   _  _\ \   ___  \ \  \ \  \\ \  \ \  \  ___  
  \ \  \\ \  \ \  \_|\ \  \ \  \ \ \  \|\__\_\  \ \  \\\  \ \  \\  \\ \  \\ \  \ \  \ \  \\ \  \ \  \|\  \ 
   \ \__\\ \__\ \_______\  \ \__\ \ \____________\ \_______\ \__\\ _\\ \__\\ \__\ \__\ \__\\ \__\ \_______\
    \|__| \|__|\|_______|   \|__|  \|____________|\|_______|\|__|\|__|\|__| \|__|\|__|\|__| \|__|\|_______|
 */
//Create an IPv6 Socket
static int socketCreate6(LevelSock *ls){
    int opt = 1;
    //Create Socket Descriptor
    if ((ls->serverFD = socket(AF_INET6, SOCK_STREAM, 0)) == 0){
        perror(TAG);
        return -1;
    }
    //Attach socket to port
    if (setsockopt(ls->serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror(TAG);
        return -1;
    }
    ls->saddr6.sin6_family = AF_INET6;
    ls->saddr6.sin6_addr = ls->singleplayer ? in6addr_loopback : in6addr_any;
    //Multicast server
    ls->saddr6.sin6_port = htons( CDNET_PORT );
    
    if (bind(ls->serverFD, (struct sockaddr *)&ls->saddr6, sizeof(ls->saddr6))<0){
        perror(TAG);
        return -1;
    }
    SDL_LogInfo(0,"%s\tListening on port %d\n", TAG, CDNET_PORT);
    
    if (listen(ls->serverFD, 3) < 0){
        perror(TAG);
        return -1;
    }

    //Reset all available Sockets
    for(int a = 0; a < SERVER_CONNECTIONS_MAX; a++)
        ls->csocks[a] = 0;
    return 0;
}

static void socketAddChildren(LevelSock *ls){
    int sd = 0;
    for(int a = 0; a < SERVER_CONNECTIONS_MAX; a++){
        sd = ls->csocks[a];
        if(sd > 0)
            FD_SET(sd, &ls->fds);
        
        if(sd > ls->maxSD)
            ls->maxSD = sd;
    }
}

static int srvSendLobby(LevelSock *lso, int index){
    uint8_t *buffer;
    buffer = (uint8_t*)malloc(CLEINT_SOCK_BUFFER_SIZE_PLAYER_DISCOVERY);
    if(!buffer)
        return -1;
    buffer[0] = GAME_PLAYERS_MAX;
    uint8_t *nbuff;
    nbuff = buffer + 1;
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        strncpy((char*)nbuff, lso->pinfo[a].name, GAME_PLAYER_NAME_LENGTH);
        nbuff += GAME_PLAYER_NAME_LENGTH;
        *nbuff++ = lso->pinfo[a].profileImage;
    }
    //SDL_Log("%s\tMy lobby string is %s\n", TAG, buffer);
    send(lso->csocks[index], buffer, CLEINT_SOCK_BUFFER_SIZE_PLAYER_DISCOVERY, 0);
    
    free(buffer);
    return 0;
}

static void srvAddClient(LevelSock *ls, int client){
    for(int a = 0; a < SERVER_CONNECTIONS_MAX; a++){
        if(ls->csocks[a] == 0){
            ls->csocks[a] = client;
            break;
        }
    }
}

static void srvKickAllClients(LevelSock *ls){
    for(int a = 0; a < SERVER_CONNECTIONS_MAX; a++){
        if(ls->csocks[a] != 0){
            close(ls->csocks[a]);
            ls->csocks[a] = 0;
        }
    }
}
/*
 ________  ___       ___  _______   ________   _________        ___  ___  ________  ________   ________  ___       ___  ________   ________     
|\   ____\|\  \     |\  \|\  ___ \ |\   ___  \|\___   ___\     |\  \|\  \|\   __  \|\   ___  \|\   ___ \|\  \     |\  \|\   ___  \|\   ____\    
\ \  \___|\ \  \    \ \  \ \   __/|\ \  \\ \  \|___ \  \_|     \ \  \\\  \ \  \|\  \ \  \\ \  \ \  \_|\ \ \  \    \ \  \ \  \\ \  \ \  \___|    
 \ \  \    \ \  \    \ \  \ \  \_|/_\ \  \\ \  \   \ \  \       \ \   __  \ \   __  \ \  \\ \  \ \  \ \\ \ \  \    \ \  \ \  \\ \  \ \  \  ___  
  \ \  \____\ \  \____\ \  \ \  \_|\ \ \  \\ \  \   \ \  \       \ \  \ \  \ \  \ \  \ \  \\ \  \ \  \_\\ \ \  \____\ \  \ \  \\ \  \ \  \|\  \ 
   \ \_______\ \_______\ \__\ \_______\ \__\\ \__\   \ \__\       \ \__\ \__\ \__\ \__\ \__\\ \__\ \_______\ \_______\ \__\ \__\\ \__\ \_______\
    \|_______|\|_______|\|__|\|_______|\|__| \|__|    \|__|        \|__|\|__|\|__|\|__|\|__| \|__|\|_______|\|_______|\|__|\|__| \|__|\|_______|
*/
static int srvHandleClient(LevelSock *lso, LevelServer *lsr, int sockindex, uint8_t* command){
    if(!command)
        return -1;
    
    int sd = lso->csocks[sockindex];
    
    size_t index = 0;
    switch(command[index++]){
        case CD_NET_CODE_OK: {
            
        } break;
        case CD_NET_CODE_FAIL: {
            
        } break;
        case CD_NET_CODE_PLAYER_NAME: {
            char buff[GAME_PLAYER_NAME_LENGTH];
            strncpy(buff, (char*)command + 1, GAME_PLAYER_NAME_LENGTH);
            uint8_t cattype = command[11];
            SDL_Log("%s\tSD %d has name: %s\tand type: %d\n", TAG, sd, buff, cattype);
            strncpy(lso->pinfo[sockindex].name, buff, GAME_PLAYER_NAME_LENGTH);
            lso->pinfo[sockindex].profileImage = cattype;
            
            uint8_t evt[2] = {CD_NET_CODE_EVENT, CD_NET_EVENT_PLAYER_UPDATE};
            srvSendToAllClientsExcept(lso, lsr, evt, 2, sd);
        } break;
        case CD_NET_CODE_GLOB_SRV_OP: {
            switch(command[index++]){
                case CD_NET_GLOB_SRV_OP_QUIT: {
                    close(sd);
                    return 1;
                }break;
                case CD_NET_GLOB_SRV_OP_KILL: {
                    SDL_Log("Received Kill...\n");
                    return -1;
                }break;
                default: break;
            }
        } break;
        case CD_NET_CODE_SET_VAR: {
            switch(command[index++]){
                case CD_NET_VAR_TITLE: {
                    memset(lsr->name, 0, LEVEL_TITLE_LENGTH);
                    strcpy(lsr->name, (char*)command + index);
                }break;
                case CD_NET_VAR_STYLE: {
                    memset(lsr->style, 0, LEVEL_STYLE_LENGTH);
                    strcpy(lsr->style, (char*)command + index);
                }break;
                case CD_NET_VAR_HEIGHT: {
                    lsr->height = LEVEL_HEIGHT;
                    lsr->height = command[index];
                }break;
                case CD_NET_VAR_WIDTH: {
                    lsr->width = LEVEL_WIDTH;
                    lsr->width = command[index];
                }break;
                default: break;
            }
        } break;
        case CD_NET_CODE_GET_VAR: {
            switch(command[index++]){
                case CD_NET_VAR_TITLE: {
                    send(sd, lsr->name, strlen(lsr->name) , 0);
                }break;
                case CD_NET_VAR_STYLE: {
                    send(sd, lsr->style, strlen(lsr->style) , 0);
                }break;
                case CD_NET_VAR_HEIGHT: {
                    send(sd, &lsr->height, 1 , 0);
                }break;
                case CD_NET_VAR_WIDTH: {
                    send(sd, &lsr->width, 1 , 0);
                }break;
                case CD_NET_VAR_PLAYERS: {
                    srvSendLobby(lso, sockindex);
                }break;
                case CD_NET_VAR_PLAYER_COUNT:{
                    uint8_t sp = 0;
                    for(uint8_t a = 0; a < SERVER_CONNECTIONS_MAX; a++)
                        sp += lso->csocks[a] > 0 ? 1 : 0;
                    send(sd, &sp, 1 , 0);
                } break;
                case CD_NET_VAR_PLAYER_NODES:{
                    if(lsr->catNodes)
                    send(sd, lsr->catNodes, lsr->catNodesCount * 3, 0);
                } break;
                default: break;
            }
        } break;
        case CD_NET_CODE_LOAD_LEVEL: {
            uint8_t msg = serverLoadLevel(lsr, (char*)command + index) == 0 ? CD_NET_CODE_OK : CD_NET_CODE_FAIL;
            srvLoadCatSpots(lsr, lso);
            send(sd, &msg, sizeof(uint8_t), 0);                
        } break;
        case CD_NET_CODE_RECV_MAP_RAW: {
            uint8_t fail = CD_NET_CODE_FAIL;
            if(lsr->nodesSize){
                send(sd, lsr->nodes, lsr->nodesSize , 0);
            }else{
                send(sd, &fail, sizeof(uint8_t), 0);
            }
        } break;
        case CD_NET_CODE_EVENT: {

        } break;
        default: break;
    }
    return 0;
}

static void srvInitGameServer(LevelServer *gameServer){
    strcpy(gameServer->name, "");
    strcpy(gameServer->style, "default");
    gameServer->width = LEVEL_WIDTH;
    gameServer->height = LEVEL_HEIGHT;
    gameServer->nodes = NULL;
    gameServer->nodesSize = 0;
    gameServer->paths = NULL;
    gameServer->catNodes = NULL;
    gameServer->catNodesCount = 0;;
}

static int threadServer(void *varg){
    unsigned int sarg = *(unsigned int*) varg;
    LevelServer gameServer;
    srvInitGameServer(&gameServer);
    
    if((sarg) & SERVER_FLAG_SINGLEPLAYER)
        SDL_LogInfo(0,"%s\tStaring in Singleplayer Mode\n", TAG);
    else
        SDL_LogInfo(0,"%s\tOpening Server to LAN\n", TAG);
    
    LevelSock lsock;
    lsock.singleplayer = sarg & SERVER_FLAG_SINGLEPLAYER;
    if(socketCreate6(&lsock))
        return -1;
    
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        memset(lsock.pinfo[a].name, 0, GAME_PLAYER_NAME_LENGTH);
        lsock.pinfo[a].profileImage = 0;
    }
    
    struct sockaddr_in6 clientAddress;
    int addrlen = sizeof(struct sockaddr_in6);
    int activity = 0;
    int newClient = 0;
    int valread = 0;
    char astring[INET6_ADDRSTRLEN];
    uint8_t buffer[SERVER_RECEIVE_BUFFER_SIZE + 1];
    
    bool serverOperative = true;
    while(serverOperative){
        FD_ZERO(&lsock.fds);
        FD_SET(lsock.serverFD, &lsock.fds);
        lsock.maxSD = lsock.serverFD;
        socketAddChildren(&lsock);
        
        activity = select( lsock.maxSD + 1 , &lsock.fds , NULL , NULL , NULL);
        if ((activity < 0) && (errno!=EINTR))
            perror(TAG);
        
        if(FD_ISSET(lsock.serverFD, &lsock.fds)){
            //SDL_LogInfo(0,"%s\tAccepting new Client\n", TAG);
            newClient = accept(lsock.serverFD, (struct sockaddr *)&clientAddress, (socklen_t*)&addrlen);
            if(newClient < 0)
                perror(TAG);
            //Print Info
            strcpy(astring, "");
            inet_ntop(AF_INET6, &(clientAddress), astring, INET6_ADDRSTRLEN);
            SDL_LogInfo(0,"%s\tClient's IP address is: %s\n", TAG, astring);
            //Add client to list
            srvAddClient(&lsock, newClient);
        }
        
        for(int index = 0; index < SERVER_CONNECTIONS_MAX; index++){
            int sd = lsock.csocks[index];
            if(FD_ISSET(sd, &lsock.fds)){
                memset(buffer, 0, SERVER_RECEIVE_BUFFER_SIZE);
                valread = read( sd , buffer, SERVER_RECEIVE_BUFFER_SIZE);
                if(valread == 0){
                    close(sd);
                    lsock.csocks[index] = 0;
                }else{
                    buffer[valread] = '\0';  
                    //send(sd , buffer , strlen(buffer) , 0 ); 
                    int ecode = srvHandleClient(&lsock, &gameServer, index, buffer);
                    switch(ecode){
                        case 1: lsock.csocks[index] = 0; break;
                        case -1: serverOperative = false; break;
                        default: break;
                    }
                }
            }
        }
    }
    
    SDL_Log("%s\tClosing Server...", TAG);
    srvKickAllClients(&lsock);
    serverDestroy(&gameServer);
    
    close(lsock.serverFD);
    return 0;
}

int serverStart(unsigned int flags){
    SDL_Thread *thread;
    int *tmp;
    tmp = malloc(sizeof(int));
    tmp[0] = flags;

    thread = SDL_CreateThread(threadServer, "Game Server", (void*)tmp);
    if(thread){
        SDL_DetachThread(thread);
        return 0;
    }else{
        fprintf(stderr, "%s\tCouldn't start Server!\n",TAG);
        return -1;
    }
    free(tmp);
}

void serverDestroy(LevelServer *ls){
    if(ls->nodes){
        free(ls->nodes);
        ls->nodes = NULL;
    }
    if(ls->catNodes){
        free(ls->catNodes);
        ls->catNodes = NULL;
    }  
    serverPathDelete(ls);
}
