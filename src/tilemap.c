#include <tilemap.h>

static const char* TAG = "Tilemap";

int tilemap_create(Tilemap *tilemap, uint8_t width, uint8_t height, const char* filepath){
    tilemap->width = width;
    tilemap->height = height;
    if(0 != texture_create(&tilemap->texture, filepath)){
        LOGE(TAG, "Unable to create texture");
        return -1;
    }

    return 0;
}

void tilemap_destroy(Tilemap *tilemap){
    texture_destroy(&tilemap->texture);
}
