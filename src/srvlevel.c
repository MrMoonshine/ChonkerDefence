#include <srvlevel.h>

static const char* TAG = "Level (Server-Side)";
static const char* LEVEL_FILE_EXTENSION = ".chonkmap";
static const char* LEVEL_DIR = SERVER_LEVEL_DIR;


static bool srvlevel_is_chonkmap(struct dirent* ep){
    return ep->d_type == DT_DIR || strstr(ep->d_name, LEVEL_FILE_EXTENSION) == NULL;
}

static int srvlevel_show_list(unsigned char* data, size_t* len, uint8_t* filecount){
    DIR* dp1;
    struct dirent* ep;
    dp1 = opendir(LEVEL_DIR);
    if(dp1 == NULL){
        LOGE(TAG, "Failed to open Level DIR");
        return -1;
    }

    uint8_t status = CD_NET_CODE_OK;
    size_t position = 0;

    if(data == NULL){
        printf("[INFO] %s: Scanning directory: %s\n", TAG, LEVEL_DIR);
        *len = 1 + sizeof(size_t) + 1;
    }else{
        memset(data, 0x00, *len);
        //memcpy(data + position++, &status, 1);
        data[position++] = status;
        size_t levellen = (*len) - PROTO_REPLY_BASE_LENGTH;
        memcpy(data + position, &levellen, sizeof(size_t));
        position += sizeof(size_t);
        data[position++] = *filecount;
        //printf("Filecount in data is %x hex or %d dec\tpos is %lu\n", data[9], data[9], position);
    }

    while((ep = readdir(dp1))){
        if(srvlevel_is_chonkmap(ep))
            continue;               // Skip directories
        /*printf("%s: %d\t", TAG, ep->d_type);
        puts(ep->d_name);*/
        if(data == NULL){
            (*filecount)++;
            *len += 1 + strlen(ep->d_name) - strlen(LEVEL_FILE_EXTENSION) + 1;
        }else{


            uint8_t filelen = strlen(ep->d_name) - strlen(LEVEL_FILE_EXTENSION);
            memcpy(data + position++, &filelen,1);
            memcpy(data + position, (unsigned char*)ep->d_name,filelen);
            position += filelen;
            data[position++] = '\0';
        }
    };
    closedir((dp1));
    return 0;
}

int srvlevel_show(struct pollfd* clientFDs, uint8_t fdCount, uint8_t* levelID){
    int retval = 0;
    if(levelID == NULL){
        size_t dataSize = 0;
        uint8_t filecount = 0;
        srvlevel_show_list(NULL, &dataSize, &filecount);
        unsigned char* data = (unsigned char*)malloc(dataSize);
        if(data != NULL){
            srvlevel_show_list(data, &dataSize, &filecount);

        size_t headerlen = 0;
        memcpy(&headerlen, data + 1, sizeof(size_t));
        printf("[INFO] %s: Size is %d for %d files\tHeader size is %lu\n", TAG, (int)dataSize, filecount, headerlen);

        if(filecount > 1){
            send(clientFDs->fd, data, dataSize, 0);
        }else{
            uint8_t status = CD_NET_CODE_FAIL;
            send(clientFDs->fd, &status, CD_NET_CODE_FAIL, 0);
        }

        free(data);
        }else{
            LOGERRNO(TAG, "level show: Malloc");
        }
    }else{
        retval = 0;
        //printf("Load Level %d\n", *levelID);
        DIR* dp1;
        struct dirent* ep;
        dp1 = opendir(LEVEL_DIR);
        if(dp1 == NULL){
            LOGE(TAG, "Failed to open Level DIR");
            return -1;
        }

        uint8_t counter = 0;
        while((ep = readdir(dp1))){
            if(srvlevel_is_chonkmap(ep))
                continue;

            if(counter++ != *levelID)
                continue;

            //printf("Loading level %s -> %lu\n", ep->d_name, strlen(ep->d_name));
            size_t filepathLen = strlen(LEVEL_DIR) + strlen(ep->d_name) + 1;
            char* filepath = (char*)malloc(filepathLen);
            if(!filepath){
                LOGERRNO(TAG, "Malloc");
                return -1;
            }
            memset(filepath, '\0', filepathLen);
            strcpy(filepath, LEVEL_DIR);
            strcat(filepath, ep->d_name);
            FILE *fp;
            fp = fopen(filepath, "rb");
            if(fp == NULL){
                //fprintf(stderr,"\e[0;31m[ERROR] Failed to open file %s: %s\e[0m\n", TAG, filepath);
                /*for(size_t i = 0; i < filepathLen; i++){
                    printf("%lu\t%c\t%.2x\n", i, filepath[i], (unsigned char)filepath[i]);
                }
                printf("d_name is %s\n", ep->d_name);*/
                LOGE_S(TAG, "Failed to open file ", filepath);
                retval = -1;
                free(filepath);
                break;
            }
            free(filepath);

            size_t fileSize = 0;
            while(EOF != getc(fp)){
                fileSize++;
            }
            rewind(fp);

            uint8_t status = 0;
            uint8_t* buffer = (uint8_t*)malloc(PROTO_REPLY_BASE_LENGTH + fileSize);
            memcpy(buffer + 0, &status, 1);
            memcpy(buffer + 1, &fileSize, sizeof(size_t));
            fread(buffer + PROTO_REPLY_BASE_LENGTH, fileSize, 1, fp);

            fclose(fp);

            // Send data to each socket
            LOGS(TAG, "Sending data to client");
            for(uint8_t i = 0; i < fdCount; i++){
                send(clientFDs[i].fd, buffer, PROTO_REPLY_BASE_LENGTH + fileSize, 0);
            }

            free(buffer);
            retval = 0;
            break;
        };

        closedir(dp1);
    }
    if(retval != 0){
        uint8_t status = CD_NET_CODE_FAIL;
        for(uint8_t i = 0; i < fdCount; i++){
            send(clientFDs[i].fd, &status, 1, 0);
        }
    }
    return retval;
}
