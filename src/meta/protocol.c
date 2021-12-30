#include "protocol.h"

static const char* TAG = "[NET]";

int netInitClientMeta(ClientSock *csock){
    csock->serverstatus = CLIENT_SOCK_SERVER_NO_UP;
    csock->flags = CLIENT_SOCK_FLAG_UNINIT;
    
    for(int a = 0; a < GAME_PLAYERS_MAX; a++){
        strcpy(csock->pinfo[a].name," ");
        csock->pinfo[a].profileImage = 0;
    }
    return 0;
}

int netLobbyDiscovery(ClientSock *csock){
    if(csock->serverstatus == CLIENT_SOCK_SERVER_UP_SINGLEPLAYER){
        memset(csock->pinfo, 0, GAME_PLAYERS_MAX * sizeof(PlayerInfo));
        strcpy(csock->pinfo->name, "Player");
        csock->pinfo->profileImage = 1;
    }else{
        uint8_t *buffer;
        buffer = (uint8_t *)malloc(2);
        if(!buffer)
            return -1;
        buffer[0] = CD_NET_CODE_GET_VAR;
        buffer[1] = CD_NET_VAR_PLAYERS;
        send(csock->sock, buffer, 2, 0);
        free(buffer);
        buffer = NULL;
        buffer = (uint8_t*)malloc(CLEINT_SOCK_BUFFER_SIZE_PLAYER_DISCOVERY);
        if(!buffer)
            return -1;
        if(read(csock->sock, buffer, CLEINT_SOCK_BUFFER_SIZE_PLAYER_DISCOVERY) < 0){
            SDL_LogError(0, "%s\tFailed to querry Lobby!\n", TAG);
            free(buffer);
            return -1;
        }
        
        uint8_t *nbuff;
        nbuff = buffer + 1;
        for(int a = 0; a < GAME_PLAYERS_MAX; a++){
            strncpy(csock->pinfo[a].name, (char*)nbuff, GAME_PLAYER_NAME_LENGTH);
            nbuff += GAME_PLAYER_NAME_LENGTH;
            csock->pinfo->profileImage = *(nbuff++);
            if(strlen(csock->pinfo[a].name))
                SDL_Log("%s\tDiscovered: %s with cat type: %d\n", TAG, csock->pinfo[a].name, csock->pinfo[a].profileImage);
        }
        free(buffer);
    }
    return 0;
}

int netSockSetup(ClientSock *csock){
    csock->flags = 0;
    csock->level = 0;
	csock->sock = -1;
	int ret;
 
	/* Create socket for communication with server */
	csock->sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (csock->sock == -1) {
		perror("socket()");
		return EXIT_FAILURE;
	}
	
	struct timeval tv;
	tv.tv_sec = CD_NET_CLIENT_TIMEOUT;
    tv.tv_usec = 0;
	setsockopt(csock->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
 
	/* Connect to server running on localhost */
	csock->saddr.sin6_family = AF_INET6;
	inet_pton(AF_INET6, "::1", &csock->saddr.sin6_addr);
	csock->saddr.sin6_port = htons(CDNET_PORT);
 
	/* Try to do TCP handshake with server */
	ret = connect(csock->sock, (struct sockaddr*)&csock->saddr, sizeof(csock->saddr));
	if (ret == -1) {
		perror("connect()");
		close(csock->sock);
		return EXIT_FAILURE;
	}

	uint8_t buff1[1 + 10 + 1];
    memset(buff1, 0, 11);
    //strcpy((char*)buff1 + 1, "David");
    strncpy((char*)buff1 + 1, getlogin(), 11);
    buff1[0] = CD_NET_CODE_PLAYER_NAME;
    buff1[11] = 1;
	write(csock->sock, buff1, 12);
	
    return 0;
}

int netKillServer(ClientSock *csock){
    if(csock->serverstatus == CLIENT_SOCK_SERVER_NO_UP)
        return 0;
    SDL_Log("%s\tKilling Server...\n", TAG);
    uint8_t kill[2];
    kill[0] = CD_NET_CODE_GLOB_SRV_OP;
    kill[1] = CD_NET_GLOB_SRV_OP_KILL;
    write(csock->sock, kill, 2);
    close(csock->sock);
    csock->serverstatus = CLIENT_SOCK_SERVER_NO_UP;
    csock->flags = CLIENT_SOCK_FLAG_KILL;
    return 0;
}

int netLoadLevel(ClientSock *csock, const char* filepath){
    uint8_t *buffer;
    size_t len = 1 + strlen(filepath);
    int ret = 0;
    buffer = (uint8_t*)malloc(len);
    if(!buffer){
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
