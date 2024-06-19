#include <srvlevel.h>

static const char* TAG = "Level (Server-Side)";
static const char* LEVEL_DIR = "levels/";
static const char* LEVEL_FILE_EXTENSION = ".chonkmap";

int srvlevel_show(unsigned char* data, size_t* len, uint8_t* filecount){
    size_t pwdlen = strlen(ASSET_PATH) + strlen(LEVEL_DIR);
    char* fullLevelDir = (char*)malloc(pwdlen);
    if(!fullLevelDir){
        return -1;
    }
    strcpy(fullLevelDir, ASSET_PATH);
    strcat(fullLevelDir, LEVEL_DIR);

    DIR* dp1;
    struct dirent* ep;
    dp1 = opendir(fullLevelDir);
    if(dp1 == NULL){
        free(fullLevelDir);
        return -1;
    }

    uint8_t status = CD_NET_CODE_OK;
    size_t position = 0;

    if(data == NULL){
        printf("[INFO] %s: Scanning directory: %s\n", TAG, fullLevelDir);
        *len = 1 + sizeof(size_t) + 1;
    }else{
        memset(data, 0x00, *len);
        //memcpy(data + position++, &status, 1);
        data[position++] = status;
        memcpy(data + position, len, sizeof(size_t));
        position += sizeof(size_t);
        data[position++] = *filecount;
        //printf("Filecount in data is %x hex or %d dec\tpos is %lu\n", data[9], data[9], position);
    }

    while((ep = readdir(dp1))){
        if(ep->d_type == DT_DIR || strstr(ep->d_name, LEVEL_FILE_EXTENSION) == NULL)
            continue;               // Skip directories
        /*printf("%s: %d\t", TAG, ep->d_type);
        puts(ep->d_name);*/
        if(data == NULL){
            (*filecount)++;
            *len += 1 + strlen(ep->d_name);
        }else{
            uint8_t filelen = strlen(ep->d_name);
            memcpy(data + position++, &filelen,1);
            memcpy(data + position, (unsigned char*)ep->d_name,filelen);
            position += filelen;
        }
    };
    closedir((dp1));
    free(fullLevelDir);
    return 0;
}
