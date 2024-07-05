#include <clilevel.h>

static const char* TAG = "Level (Client-Side)";
//static const size_t RECV_BUFFER_SIZE = 1024;
//static const char* RECV_BUFFER_OVERFLOW_MSG = "Client receiving data overflow!";

uint8_t clilevel_list_levels(Client *client, size_t* len){
    // Send command to server
    uint8_t cmd[] = {PROTO_SCOPE_LEVELS, PROTO_CMD_SHOW};
    send(client->socket, cmd, sizeof(cmd), 0);

    uint8_t status = 0;
    recv(client->socket, &status, 1, 0);
    if(status != CD_NET_CODE_OK){
        fprintf(stderr,"\e[0;31m[ERROR] %s: Error in Reply: %x\e[0m\n", TAG, status);
        return status;
    }
    recv(client->socket, len, sizeof(size_t), 0);

    //printf("[INFO] %s: Size is %d\n", TAG, (int)*len);
    return 0;
}

uint8_t clilevel_get_level(Client *client, size_t *len, uint8_t levelID, ClientLevel *level){
    client_flush(client);
    uint8_t cmd[] = {PROTO_SCOPE_LEVELS, PROTO_CMD_SHOW, levelID};
    send(client->socket, cmd, sizeof(cmd), 0);

    /*uint8_t header[PROTO_REPLY_BASE_LENGTH];
    recv(client->socket, header, PROTO_REPLY_BASE_LENGTH, 0);
    if(header[0] != CD_NET_CODE_OK){
        fprintf(stderr,"\e[0;31m[ERROR] %s: Error in Reply: %x\e[0m\n", TAG, header[0]);
        return header[0];
    }

    printf("CLILevel\n");
    for(uint8_t i = 0; i < 9; i++)
        printf("%x, ", header[i]);

    printf("\n");
    *len = 0;
    memcpy(len, header + 1, PROTO_REPLY_BASE_LENGTH - 1);
    printf("Level data size is %lx\n", *len);*/
    //uint8_t data[RECV_BUFFER_SIZE];
    uint8_t status = 0;
    recv(client->socket, &status, 1, 0);
    if(status != CD_NET_CODE_OK){
        fprintf(stderr,"\e[0;31m[ERROR] %s: Error in Reply: %x\e[0m\n", TAG, status);
        return status;
    }

    recv(client->socket, len, sizeof(size_t), 0);
    printf("Level data size is %lx\n", *len);

    uint8_t* buffer = (uint8_t*)malloc(*len);
    if(buffer == NULL){
        LOGERRNO(TAG, "Malloc");
        return 1;
    }
    recv(client->socket, buffer, *len, 0);
    for(size_t i = 0; i < *len; i++){
        printf("%.2x, ", buffer[i]);
        if(i%8 == 7)
            printf("\n");
    }

    printf("[INFO] %s: Size is %lu\n", TAG, *len);
    size_t pos = 0;
    memcpy(level->name, buffer + pos, LEVEL_NAME_LENGTH);
    pos += LEVEL_NAME_LENGTH;
    memcpy(level->style, buffer + pos, LEVEL_STYLE_LENGTH);
    pos += LEVEL_STYLE_LENGTH;
    level->width = buffer[pos++];
    level->height = buffer[pos++];
    // int counter;
    // ioctl(client->socket, FIONREAD, &counter);
    // printf("IOCTL Here1: %d\n", counter);
    free(buffer);
    return 0;
}

void clilevel_destroy(ClientLevel *level){

}

/*uint8_t clilevel_count(unsigned char* data, size_t len){
    size_t pos = 0;//1 + sizeof(size_t);
    if(len < pos + 1){
        LOGE(TAG, "Either invalid data or no levels available on server!");
        return 0;
    }

    uint8_t counter = 0;
    memcpy(&counter,data + pos, 1);
    return counter;
}*/
