#include <tilemap.h>

//static const char* TAG = "Tilemap";

int tilemap_create(Tilemap *tilemap, uint8_t width, uint8_t height, const char* filepath){
    tilemap->width = width;
    tilemap->height = height;
    if(0 != texture_create(&tilemap->texture, filepath)){
        //LOGE(TAG, "Unable to create texture");
        return -1;
    }

    return 0;
}

void tilemap_destroy(Tilemap *tilemap){
    texture_destroy(&tilemap->texture);
}

void tilemap_get_block_UV(Tilemap *tilemap, float* buffer, uint8_t vertexCount, uint8_t x, uint8_t y){
    return tilemap_get_block_UV_rotate(tilemap, buffer, vertexCount, x, y, 0);
}

void tilemap_get_block_UV_rotate(Tilemap *tilemap, float* buffer, uint8_t vertexCount, uint8_t x, uint8_t y, uint8_t rotation){
    if(x >= tilemap->width || y >= tilemap->height){
        memset(buffer, 0.0f, 2*UV_SIZE / sizeof(float));
        return;
    }

    float unitX = 1.0f / (float)tilemap->width;
    float unitY = 1.0f / (float)tilemap->height;

    size_t pos = 0;
    switch(rotation % 4){
        case 3:
            // Vertex1 Point 1
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 0);
            // Vertex1 Point 2
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 1);
            // Vertex1 Point 3
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 0);
            if(vertexCount <= 1)
                return;
            // Vertex2 Point 1
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 0);
            // Vertex2 Point 2
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 1);
            // Vertex2 Point 3
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 1);
            break;
        case 2:
            // Vertex1 Point 1
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 1);
            // Vertex1 Point 2
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 0);
            // Vertex1 Point 3
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 0);
            if(vertexCount <= 1)
                return;
            // Vertex2 Point 1
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 1);
            // Vertex2 Point 2
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 1);
            // Vertex2 Point 3
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 0);
            break;
        case 1:
            // Vertex1 Point 1
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 1);
            // Vertex1 Point 2
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 0);
            // Vertex1 Point 3
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 1);
            if(vertexCount <= 1)
                return;
            // Vertex2 Point 1
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 1);
            // Vertex2 Point 2
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 0);
            // Vertex2 Point 3
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 0);
            break;
        default:
            // Vertex1 Point 1
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 0);
            // Vertex1 Point 2
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 1);
            // Vertex1 Point 3
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 1);
            if(vertexCount <= 1)
                return;
            // Vertex2 Point 1
            buffer[pos++] = unitX * (x + 0);
            buffer[pos++] = unitY * (y + 0);
            // Vertex2 Point 2
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 0);
            // Vertex2 Point 3
            buffer[pos++] = unitX * (x + 1);
            buffer[pos++] = unitY * (y + 1);
            break;
    }
    /*size_t pos = 0;
    // Vertex1 Point 1
    buffer[pos++] = unitX * (x + 0);
    buffer[pos++] = unitY * (y + 0);
    // Vertex1 Point 2
    buffer[pos++] = unitX * (x + 1);
    buffer[pos++] = unitY * (y + 1);
    // Vertex1 Point 3
    buffer[pos++] = unitX * (x + 0);
    buffer[pos++] = unitY * (y + 1);
    if(vertexCount <= 1)
        return;
    // Vertex2 Point 1
    buffer[pos++] = unitX * (x + 0);
    buffer[pos++] = unitY * (y + 0);
    // Vertex2 Point 2
    buffer[pos++] = unitX * (x + 1);
    buffer[pos++] = unitY * (y + 0);
    // Vertex2 Point 3
    buffer[pos++] = unitX * (x + 1);
    buffer[pos++] = unitY * (y + 1);*/
    /*size_t pos = 0;
    uint8_t vertexNum = 0, pointNum = 0;

    for(uint8_t i = 0; i < 3; i++){
        buffer[pos++] = unitX * (x + tilemap_rotate_UV_helper(vertexNum, pointNum, 0, rotation));
        buffer[pos++] = unitY * (y + tilemap_rotate_UV_helper(vertexNum, pointNum, 1, rotation));
        pointNum++;
    }

    if(vertexCount <= 1)
        return;

    vertexNum = 1;
    pointNum = 0;
    for(uint8_t i = 0; i < 3; i++){
        buffer[pos++] = unitX * (x + tilemap_rotate_UV_helper(vertexNum, pointNum, 0, rotation));
        buffer[pos++] = unitY * (y + tilemap_rotate_UV_helper(vertexNum, pointNum, 1, rotation));
        pointNum++;
    }*/
}
