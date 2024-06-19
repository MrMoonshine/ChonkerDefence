#include <clilevel.h>

static const char* TAG = "Level (Client-Side)";
//static const size_t RECV_BUFFER_SIZE = 1024;
static const char* RECV_BUFFER_OVERFLOW_MSG = "Client receiving data overflow!";

int clilevel_show(Client *client, unsigned char* data, size_t *len){
    size_t maxSize = *len;
    uint8_t cmd[] = {PROTO_SCOPE_LEVELS, PROTO_CMD_SHOW};
    send(client->socket, cmd, sizeof(cmd), 0);
    //uint8_t data[RECV_BUFFER_SIZE];

    recv(client->socket, data, *len, 0);
    uint8_t status = data[0];
    if(status != CD_NET_CODE_OK){
        fprintf(stderr,"\e[0;31m[ERROR] %s: Error in Reply: %x\e[0m\n", TAG, status);
        return -1;
    }
    memcpy(len, data + 1, sizeof(size_t));
    if(*len >= maxSize){
        LOGE(TAG, RECV_BUFFER_OVERFLOW_MSG);
        return -1;
    }
    printf("[INFO] %s: Size is %d\n", TAG, (int)*len);
    return 0;
}

uint8_t clilevel_count(unsigned char* data, size_t len){
    size_t pos = 1 + sizeof(size_t);
    if(len < pos + 1){
        LOGE(TAG, "Either invalid data or no levels available on server!");
        return 0;
    }

    uint8_t counter = 0;
    memcpy(&counter,data + pos, 1);
    return counter;
}
