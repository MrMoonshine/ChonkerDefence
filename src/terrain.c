
#include <terrain.h>

static const char* TAG = "Terrain";

static const uint8_t NEIGHBOUR_BIT_N  = 0;
static const uint8_t NEIGHBOUR_BIT_E  = 1;
static const uint8_t NEIGHBOUR_BIT_S  = 2;
static const uint8_t NEIGHBOUR_BIT_W  = 3;

static const uint8_t terrain_get_node_at(Terrain* terrain, uint8_t x, uint8_t y, uint8_t *buffer, size_t len){
    size_t totalPos = (y * terrain->width) + x;
    size_t pos = totalPos / 2;

    if(pos >= len || x > terrain->width - 1 || y > terrain->height - 1)
        return 0xFF;

    return totalPos % 2 ? (buffer[pos] & 0x0f) : ((buffer[pos] & 0xf0) >> 4);
}

static void terrain_square_xy(Terrain* terrain, float* buffer, uint8_t x, uint8_t y){
    size_t posVertex = 0;
    short xOrigin = x - terrain->width/2;
    short yOrigin = y - terrain->height/2;

    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = yOrigin + 1;
    buffer[posVertex++] = 1.0f;
    // Vertex 1 Point 1
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = yOrigin + 0;
    buffer[posVertex++] = 1.0f;
    // Vertex 1 Point 2
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = yOrigin + 1;
    buffer[posVertex++] = 1.0f;
    // Vertex 1 Point 3

    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = yOrigin + 1;
    buffer[posVertex++] = 1.0f;

    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = yOrigin + 0;
    buffer[posVertex++] = 1.0f;
    // Vertex 2 Point 1
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = yOrigin + 0;
    buffer[posVertex++] = 1.0f;
    // Vertex 2 Point 2

    // Vertex 2 Point 3
    return;
}

static size_t terrain_square_xy_skirt(Terrain* terrain, float* buffer, uint8_t x, uint8_t y, uint8_t neighbourPattern){
    uint8_t neiCounter = 0;
    size_t retval = 0;
    for(uint8_t i = 0; i < 4; i++){
        if(neighbourPattern & (1 << i))
            neiCounter++;
    }

    if(neiCounter < 1){
        return 0;
    }

    printf("(%d|%d) has %d land neighbours: hex: %x\n", x, y, neiCounter, neighbourPattern);

    short xOrigin = x - terrain->width/2;
    short yOrigin = y - terrain->height/2;
    int8_t deltaX = 1;
    int8_t deltaY = 1;
    bool sonw = false;

    size_t posVertex = 0;

    if(neiCounter == 1){
        retval = 2;
        switch(neighbourPattern){
            case 0b0001:
                // North
                xOrigin += 0;
                yOrigin += 0;
                deltaX = 1;
                deltaY = 0;
                break;
            case 0b0010:
                // East
                xOrigin += 1;
                yOrigin += 0;
                deltaX = 0;
                deltaY = 1;
                break;
            case 0b0100:
                // South
                xOrigin += 1;
                yOrigin += 1;
                deltaX = -1;
                deltaY = 0;
                break;
            case 0b1000:
                // West
                xOrigin += 0;
                yOrigin += 1;
                deltaX = 0;
                deltaY = -1;
                break;
            default:
                break;
        }

    }else if(neiCounter == 2){
        retval = 3;
        switch(neighbourPattern){
            case 0b0011:
                // North-East
                xOrigin += 0;
                yOrigin += 0;
                deltaX = 1;
                deltaY = 1;
                break;
            case 0b0110:
                // South-East
                xOrigin += 1;
                yOrigin += 0;
                deltaX = -1;
                deltaY = 1;
                sonw = true;
                break;
            case 0b1100:
                // South-West
                xOrigin += 1;
                yOrigin += 1;
                deltaX = -1;
                deltaY = -1;
                break;
            case 0b1001:
                // North-West
                xOrigin += 0;
                yOrigin += 1;
                deltaX = 1;
                deltaY = -1;
                sonw = true;
                break;
            default:
                break;
        }
    }
    // Vertex 1 Point 1
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = yOrigin + deltaY * 0;
    buffer[posVertex++] = 0.0f;
    // Vertex 1 Point 2
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = yOrigin + deltaY * 1;
    buffer[posVertex++] = 1.0f;
    // Vertex 1 Point 3
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = yOrigin + deltaY * 0;
    buffer[posVertex++] = 1.0f;
    // Vertex 2 Point 1
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = yOrigin + deltaY * 0;
    buffer[posVertex++] = 0.0f;
    // Vertex 2 Point 2
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = yOrigin + deltaY * 1;
    buffer[posVertex++] = 0.0f;
    // Vertex 2 Point 3
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = yOrigin + deltaY * 1;
    buffer[posVertex++] = 1.0f;

    if(neiCounter == 2){
        // Lid for inner corners
        // Vertex 1 Point 1
        buffer[posVertex++] = xOrigin + deltaX * 0;
        buffer[posVertex++] = yOrigin + deltaY * 0;
        buffer[posVertex++] = 1.0f;
        // Vertex 1 Point 2
        buffer[posVertex++] = xOrigin + deltaX * 1;
        buffer[posVertex++] = yOrigin + deltaY * 1;
        buffer[posVertex++] = 1.0f;
        // Vertex 1 Point 3
        if(sonw){
            buffer[posVertex++] = x - terrain->width/2 + (neighbourPattern == 0b0110 ? 1 : 0);
            buffer[posVertex++] = y - terrain->height/2 + (neighbourPattern == 0b0110 ? 1 : 0);;

        }else{
            buffer[posVertex++] = xOrigin + deltaX * 1;
            buffer[posVertex++] = yOrigin + deltaY * 0;
        }
        buffer[posVertex++] = 1.0f;
    }

    return retval;
}

int terrain_create(Terrain* terrain, uint8_t* buffer_i, size_t bufferSize){
    terrain->vertexCount = 0;
    char style[LEVEL_STYLE_LENGTH];

    size_t pos = 0;
    pos += LEVEL_NAME_LENGTH;
    memcpy(style, buffer_i + pos, LEVEL_STYLE_LENGTH);
    pos += LEVEL_STYLE_LENGTH;
    terrain->width = buffer_i[pos++];
    terrain->height = buffer_i[pos++];
    pos += LEVEL_RESERVED_LENGTH;
    // Just deal wiht level data from here on
    uint8_t* buffer = buffer_i + pos;
    size_t len = bufferSize -pos;

    char* tilemapFile = (char*)malloc(strlen(LEVEL_TILEMAP_TERRAIN_FILE_PATTERN) + LEVEL_STYLE_LENGTH);
    sprintf(tilemapFile, LEVEL_TILEMAP_TERRAIN_FILE_PATTERN, style);
    if(0 > tilemap_create(&terrain->tilemap, LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_HEIGHT, tilemapFile)){
        printf("\e[0;33m[WARNING] %s: Unable to open %s. Falling back to default style\n\e[0m", TAG, tilemapFile);
        sprintf(tilemapFile, LEVEL_TILEMAP_TERRAIN_FILE_PATTERN, "default");
        if(0 > tilemap_create(&terrain->tilemap, LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_HEIGHT, tilemapFile)){
            LOGE(TAG, "Unable to fallback on default tilemap");
        }
    }
    free(tilemapFile);

    /*for(size_t i = 0; i < len; i++){
        printf("%.2x", buffer[i]);
        if(i%(terrain->width/2) == (terrain->width/2) - 1)
            printf("\n");
    }
    printf("-------------------------\n");*/
    printf("   ");
    for(uint8_t x = 0; x < terrain->width; x++)
        printf("%.1d", x/10);
    printf("\n   ");
    for(uint8_t x = 0; x < terrain->width; x++)
        printf("%.1d", x % 10);
    printf("\n");
    /*
        Counting Dry Run
    */
    for(uint8_t y = 0; y < terrain->height; y++){
        printf("%.2d ", y);
        for(uint8_t x = 0; x < terrain->width; x++){
            uint8_t nnibble = terrain_get_node_at(terrain, x, y, buffer, len);
            char node = ' ';
            switch(nnibble){
                case 0b0000: node = 'P'; break;
                case 0b0001: node = 'W'; break;
                case 0b0010: node = 'G'; break;
                case 0b0011: node = 'B'; break;
                case 0b0100: node = 'D'; break;
                case 0b0101: node = 'd'; break;
                case 0b0110: node = 'c'; break;
                case 0b0111: node = 'C'; break;
                default: break;
            }
            printf("%c", node);

            if(LEVEL_IS_LAND(nnibble) || nnibble == LEVEL_BLOCK_PATH){
                terrain->vertexCount += 2;
            }else if(LEVEL_IS_WATER(nnibble)){
                uint8_t neiCounter = 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 1, y + 0, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 0, y + 1, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x - 1, y + 0, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 0, y - 1, buffer, len)) ? 1 : 0;

                switch(neiCounter){
                    case 1: terrain->vertexCount += 2; break;
                    case 2: terrain->vertexCount += 3; break;
                    case 3: terrain->vertexCount += 3*2; break;
                    default: break;
                }
            }
        }
        printf("\n");
    }
    printf("[INFO] %s: Vertexcount terrain is %lu\n", TAG, terrain->vertexCount);
    float* terrainVertexBuffer = (float*)malloc(terrain->vertexCount * VERTEX_SIZE);
    float* terrainUVBuffer = (float*)malloc(terrain->vertexCount * UV_SIZE);
    if(terrainUVBuffer == NULL || terrainVertexBuffer == NULL){
        LOGERRNO(TAG, "Malloc Terrain Buffer");
        return -1;
    }
    // Zero all buffers
    for(size_t a = 0; a < terrain->vertexCount*VERTEX_SIZE/sizeof(float); a++)
        terrainVertexBuffer[a] = 0.0f;
    for(size_t a = 0; a < terrain->vertexCount*UV_SIZE/sizeof(float); a++)
        terrainUVBuffer[a] = 0.0f;

    /*
     * Build terrain
    */
    size_t posVertex = 0;
    size_t posUV = 0;
    for(uint8_t y = 0; y < terrain->height; y++){
        for(uint8_t x = 0; x < terrain->width; x++){
            uint8_t nnibble = terrain_get_node_at(terrain, x, y, buffer, len);
            if(LEVEL_IS_WATER(nnibble)){
                uint8_t neiPattern = 0;
                neiPattern |= LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 0, y - 1, buffer, len)) ? 1 << NEIGHBOUR_BIT_N : 0;
                neiPattern |= LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 1, y + 0, buffer, len)) ? 1 << NEIGHBOUR_BIT_E : 0;
                neiPattern |= LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 0, y + 1, buffer, len)) ? 1 << NEIGHBOUR_BIT_S : 0;
                neiPattern |= LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x - 1, y + 0, buffer, len)) ? 1 << NEIGHBOUR_BIT_W : 0;

                size_t terrainSkirtVertexCount = terrain_square_xy_skirt(terrain, terrainVertexBuffer + posVertex, x, y, neiPattern);
                posVertex += terrainSkirtVertexCount * VERTEX_SIZE/sizeof(float);
                switch(terrainSkirtVertexCount){
                    case 2:
                        tilemap_get_block_UV(&terrain->tilemap, terrainUVBuffer + posUV, 2, rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_SKIRT);
                        posUV += 2*UV_SIZE / sizeof(float);
                        break;
                    case 3:
                        tilemap_get_block_UV(&terrain->tilemap, terrainUVBuffer + posUV, 2, rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_SKIRT);
                        posUV += 2*UV_SIZE / sizeof(float);
                        tilemap_get_block_UV(&terrain->tilemap, terrainUVBuffer + posUV, 1, rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_GRASS);
                        posUV += 1*UV_SIZE / sizeof(float);
                        break;
                    default: break;
                }
            }
        }
    }

    uint8_t counter = 0;
    for(uint8_t y = 0; y < terrain->height; y++){
        for(uint8_t x = 0; x < terrain->width; x++){
            uint8_t nnibble = terrain_get_node_at(terrain, x, y, buffer, len);
            if(LEVEL_IS_LAND(nnibble)){
                terrain_square_xy(terrain, terrainVertexBuffer + posVertex, x, y);
                posVertex += 2*VERTEX_SIZE/sizeof(float);
                tilemap_get_block_UV(&terrain->tilemap, terrainUVBuffer + posUV, 2,  rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_GRASS);
                posUV += 2*UV_SIZE / sizeof(float);
            }else if(nnibble == LEVEL_BLOCK_PATH){
                terrain_square_xy(terrain, terrainVertexBuffer + posVertex, x, y);
                posVertex += 2*VERTEX_SIZE/sizeof(float);

                tilemap_get_block_UV_rotate(&terrain->tilemap, terrainUVBuffer + posUV, 2,  1, LEVEL_TILEMAP_TERRAIN_PATH, counter++%4);
                posUV += 2*UV_SIZE / sizeof(float);
            }
        }
    }

    /*for(int a = 0; a < posVertex/sizeof(float); a++){
        printf("%.2f, ", terrainVertexBuffer[a]);
        if( a % 3 == 2)
            printf("\n");
    }*/

    /*for(int a = 0; a < posUV/sizeof(float); a++){
        printf("%.2f, ", terrainUVBuffer[a] * 3);
        if( a % 2 == 1)
            printf("\n");
    }*/

    glGenBuffers(1, &terrain->vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, posVertex*sizeof(float), terrainVertexBuffer, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &terrain->uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, posUV*sizeof(float), terrainUVBuffer, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
    // Buffer Cleanup
    free(terrainVertexBuffer);
    free(terrainUVBuffer);
    return 0;
}

void terrain_draw(Terrain* terrain){
    glBindTexture( GL_TEXTURE_2D, terrain->tilemap.texture.bufferID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, terrain->vertexCount * 3); // start at 0; 6 because 6 points for 2 vertices
    //LOGGLERR(TAG);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, terrain->uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, terrain->vertexCount * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);
}

void terrain_destroy(Terrain* terrain){
    glDeleteBuffers(1, &terrain->uvbuffer);
    glDeleteBuffers(1, &terrain->vertexbuffer);
    tilemap_destroy(&terrain->tilemap);
}

