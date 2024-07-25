#include <modellib.h>
#define MODELLIB_STYLE_SAFE(stlye) (style == NULL ? "default" : style)

static const char* TAG = "ModelLib";
/*
    Retunrs the number of avilable obj files
*/
static uint8_t modellib_count_obj(const char* pattern, const char* style){
    uint8_t retval = 0;
    size_t pathlen = strlen(pattern) + strlen(MODELLIB_STYLE_SAFE(stlye)) + 3; // 3 because uint8_t max is 3 digits
    char* pathBuffer = (char*)malloc(pathlen);
    if(pathBuffer == NULL){
        LOGERRNO(TAG, "Malloc path buffer");
        return -1;
    }
    memset(pathBuffer, '\0', pathlen);
    do{
        sprintf(pathBuffer, pattern, MODELLIB_STYLE_SAFE(stlye), ++retval);
        //printf("Searching for %s\t...\n", pathBuffer);
    }while(FILE_EXISTS(pathBuffer));
    // In the loop it's always 1 ahead
    retval--;
    free(pathBuffer);
    return retval;
}
/*
    Loads OBJ and returns # of valid ones
*/
static uint8_t modellib_build_obj(Obj* obj, const char* pattern, uint8_t count, const char* style){
    uint8_t retval = 0;
    size_t pathlen = strlen(pattern) + strlen(MODELLIB_STYLE_SAFE(stlye)) + 3; // 3 because uint8_t max is 3 digits
    char* pathBuffer = (char*)malloc(pathlen);
    if(pathBuffer == NULL){
        LOGERRNO(TAG, "Malloc path buffer");
        return -1;
    }
    for(uint8_t i = 0; i < count; i++){
        memset(pathBuffer, '\0', pathlen);
        sprintf(pathBuffer, pattern, MODELLIB_STYLE_SAFE(stlye), i + 1);
        if(0 == obj_create(obj + retval, pathBuffer))
            retval++;
    }
    /*do{
        sprintf(pathBuffer, pattern, MODELLIB_STYLE_SAFE(stlye), ++retval);
        //printf("Searching for %s\t...\n", pathBuffer);
    }while(FILE_EXISTS(pathBuffer));*/
    // In the loop it's always 1 ahead
    //retval--;
    free(pathBuffer);
    return retval;
}

int modellib_create(ModelLib* lib, const char* style){
    bool treeStyleFallback = false;
    lib->models = NULL;
    /*
        Trees
    */
    lib->treeCount = modellib_count_obj(MODELLIB_FILE_PATTERN_TREE, style);
    if(lib->treeCount == 0 && strcmp("default", MODELLIB_STYLE_SAFE(style)) != 0){
        printf("\e[0;33m[WARNING] %s: Failed to load Trees for style %s! Loading default set instead\n\e[0m", TAG, MODELLIB_STYLE_SAFE(style));
        lib->treeCount = modellib_count_obj(MODELLIB_FILE_PATTERN_TREE, NULL);
        treeStyleFallback = true;
    }

    if(lib->treeCount == 0){
        LOGE(TAG, "Default set contains no trees!");
        return -1;
    }
    printf("[INFO] %s: Found %d %s trees\n", TAG,  lib->treeCount, treeStyleFallback ? "default" : MODELLIB_STYLE_SAFE(style));
    lib->models = (Obj*)malloc((lib->treeCount) * sizeof(Obj));
    if(lib->models == NULL){
        LOGERRNO(TAG, "Malloc models");
        return -1;
    }

    lib->treeCount = modellib_build_obj(
        lib->models + 0,
        MODELLIB_FILE_PATTERN_TREE,
        lib->treeCount,
        treeStyleFallback ? NULL : style
    );

    return 0;
}

void modellib_destroy(ModelLib* lib){
    if(lib->models)
        free(lib->models);
}

Obj* modellib_get_tree(ModelLib* lib, uint8_t variant){
    return lib->models + (variant % lib->treeCount);
}
