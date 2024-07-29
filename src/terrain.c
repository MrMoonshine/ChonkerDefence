
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

static uint8_t terrain_count_neighbours_from_pattern(uint8_t pattern){
    uint8_t neiCounter = 0;
    for(uint8_t i = 0; i < 4; i++){
        if(pattern & (1 << i))
            neiCounter++;
    }
    return neiCounter;
}

static void terrain_square_xy(Terrain* terrain, float* buffer, uint8_t x, uint8_t y){
    size_t posVertex = 0;
    short xOrigin = x - terrain->width/2;
    short yOrigin = y - terrain->height/2;

    // Vertex 1 Point 1
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = yOrigin + 0;
    buffer[posVertex++] = 1.0f;
    // Vertex 1 Point 2
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = yOrigin + 1;
    buffer[posVertex++] = 1.0f;
    // Vertex 1 Point 3
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = yOrigin + 1;
    buffer[posVertex++] = 1.0f;

    // Vertex 2 Point 1
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = yOrigin + 0;
    buffer[posVertex++] = 1.0f;
    // Vertex 2 Point 2
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = yOrigin + 0;
    buffer[posVertex++] = 1.0f;
    // Vertex 2 Point 3
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = yOrigin + 1;
    buffer[posVertex++] = 1.0f;
    return;
}

static size_t terrain_square_xy_skirt(Terrain* terrain, float* buffer, uint8_t x, uint8_t y, uint8_t neighbourPattern){
    size_t retval = 0;
    uint8_t neiCounter = terrain_count_neighbours_from_pattern(neighbourPattern);
    if(neiCounter < 1){
        return 0;
    }

    //printf("(%d|%d) has %d land neighbours: hex: %x\n", x, y, neiCounter, neighbourPattern);

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
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = yOrigin + deltaY * 1;
    buffer[posVertex++] = 1.0f;
    // Vertex 1 Point 2
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = yOrigin + deltaY * 0;
    buffer[posVertex++] = 0.0f;
    // Vertex 1 Point 3
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = yOrigin + deltaY * 0;
    buffer[posVertex++] = 1.0f;

    // Vertex 2 Point 1
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = yOrigin + deltaY * 1;
    buffer[posVertex++] = 1.0f;
    // Vertex 2 Point 2
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = yOrigin + deltaY * 1;
    buffer[posVertex++] = 0.0f;
    // Vertex 2 Point 3
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = yOrigin + deltaY * 0;
    buffer[posVertex++] = 0.0f;

    if(neiCounter == 2){
        // Lid for inner corners
        // Vertex 1 Point 2
        buffer[posVertex++] = xOrigin + deltaX * 1;
        buffer[posVertex++] = yOrigin + deltaY * 1;
        buffer[posVertex++] = 1.0f;
        // Vertex 1 Point 1
        buffer[posVertex++] = xOrigin + deltaX * 0;
        buffer[posVertex++] = yOrigin + deltaY * 0;
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

static void terrain_path_piece(uint8_t pattern, uint8_t *piece, uint8_t *rotation){
    uint8_t neiCounter = terrain_count_neighbours_from_pattern(pattern);
    switch(neiCounter){
        case 1:
            *piece = 0;
            if(pattern & (1 << NEIGHBOUR_BIT_N) || pattern & (1 << NEIGHBOUR_BIT_S)){
                *rotation = 0;
            }else{
                *rotation = 1;
            }
            break;
        case 2:
            switch (pattern){
                case 0b0101:
                    // N - S
                    *piece = 0;
                    *rotation = 0;
                    break;
                case 0b1010:
                    // E - W
                    *piece = 0;
                    *rotation = 1;
                    break;
                case 0b0110:
                    // S - E
                    *piece = 1;
                    *rotation = 0;
                    break;
                case 0b1100:
                    // S - W
                    *piece = 1;
                    *rotation = 3;
                    break;
                case 0b0011:
                    // N - E
                    *piece = 1;
                    *rotation = 1;
                    break;
                case 0b1001:
                    // N - E
                    *piece = 1;
                    *rotation = 2;
                    break;
                default:
                    *piece = 1;
                    *rotation = 0;
                    printf("pattern is %x\n", pattern);
                    break;
            }
            break;
        case 3:{
                uint8_t counter = 0;
                while((pattern >> counter++) & 1 || counter > 3){}
                *piece = 2;
                *rotation = 4 - counter;
            } break;
        default:
            *piece = 3;
            *rotation = 0;
            break;
    }
}

int terrain_create(Terrain* terrain, uint8_t* buffer_i, size_t bufferSize){
    size_t vertexCount = 0;
    terrain->decorationCount = 0;
    terrain->decorations = NULL;
    char style[LEVEL_STYLE_LENGTH];

    LOGS(TAG, "Loaded ModelLibrary");

    LOGI(TAG, "Building terrain...");

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

    /*
        Decorations Model Lib
    */
    modellib_create(&terrain->modellib, style);

    size_t tilemapFileLength = strlen(LEVEL_TILEMAP_TERRAIN_FILE_PATTERN) + LEVEL_STYLE_LENGTH;
    char* tilemapFile = (char*)malloc(tilemapFileLength);
    memset(tilemapFile, 0, tilemapFileLength);
    sprintf(tilemapFile, LEVEL_TILEMAP_TERRAIN_FILE_PATTERN, style);
    if(0 > tilemap_create(&terrain->tilemap, LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_HEIGHT, tilemapFile)){
        //printf("\e[0;33m[WARNING] %s: Unable to open %s. Falling back to default style\n\e[0m", TAG, tilemapFile);
        LOGE_S(TAG, "Falling back to default tilemap. Original doesn't exist:  %s", tilemapFile);
        sprintf(tilemapFile, LEVEL_TILEMAP_TERRAIN_FILE_PATTERN, "default");
        if(0 > tilemap_create(&terrain->tilemap, LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_HEIGHT, tilemapFile)){
            LOGE(TAG, "Unable to fallback on default tilemap");
        }
    }
    free(tilemapFile);
    LOGS(TAG, "Created tilemap");

    /*for(size_t i = 0; i < len; i++){
        printf("%.2x", buffer[i]);
        if(i%(terrain->width/2) == (terrain->width/2) - 1)
            printf("\n");
    }
    printf("-------------------------\n");*/
#ifdef DEBUG_MAP
    printf("   ");
    for(uint8_t x = 0; x < terrain->width; x++)
        printf("%.1d", x/10);
    printf("\n   ");
    for(uint8_t x = 0; x < terrain->width; x++)
        printf("%.1d", x % 10);
    printf("\n");
#endif /* DEBUG_MAP */
    /*
        Counting Dry Run
    */
    for(uint8_t y = 0; y < terrain->height; y++){
#ifdef DEBUG_MAP
        printf("%.2d ", y);
#endif /* DEBUG_MAP */
        for(uint8_t x = 0; x < terrain->width; x++){
            uint8_t nnibble = terrain_get_node_at(terrain, x, y, buffer, len);
#ifdef DEBUG_MAP
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
#endif /* DEBUG_MAP */
            if(LEVEL_IS_LAND(nnibble) || nnibble == LEVEL_BLOCK_PATH){
                vertexCount += 2;
            }else if(LEVEL_IS_WATER(nnibble)){
                uint8_t neiCounter = 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 1, y + 0, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 0, y + 1, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x - 1, y + 0, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED(terrain_get_node_at(terrain, x + 0, y - 1, buffer, len)) ? 1 : 0;

                switch(neiCounter){
                    case 1: vertexCount += 2; break;
                    case 2: vertexCount += 3; break;
                    case 3: vertexCount += 3*2; break;
                    default: break;
                }
            }
            // Decoration
            if(nnibble == LEVEL_BLOCK_DECORATION_LAND /*|| nnibble == LEVEL_BLOCK_DECORATION_WATER*/){
                terrain->decorationCount++;
            }
        }
#ifdef DEBUG_MAP
        printf("\n");
#endif /* DEBUG_MAP */
    }
    printf("[INFO] %s: Vertexcount terrain is %lu\n", TAG, vertexCount);
    float* terrainVertexBuffer = (float*)malloc(vertexCount * VERTEX_SIZE);
    float* terrainUVBuffer = (float*)malloc(vertexCount * UV_SIZE);
    float* terrainNormalsBuffer = (float*)malloc(vertexCount * NORMALS_SIZE);
    if(terrainUVBuffer == NULL || terrainVertexBuffer == NULL || terrainNormalsBuffer == NULL){
        LOGERRNO(TAG, "Malloc Terrain Buffer");
        return -1;
    }
    memset(terrainVertexBuffer, 0.0f, vertexCount * VERTEX_SIZE);
    memset(terrainUVBuffer, 0.0f, vertexCount * UV_SIZE);
    memset(terrainNormalsBuffer, 0.0f, vertexCount * NORMALS_SIZE);

    terrain->decorations = (Decoration*)malloc(terrain->decorationCount * sizeof(Decoration));
    if(terrain->decorations == NULL){
        LOGERRNO(TAG, "Malloc Decorations");
        return -1;
    }
    // Zero all buffers
    for(size_t a = 0; a < vertexCount*VERTEX_SIZE/sizeof(float); a++)
        terrainVertexBuffer[a] = 0.0f;
    for(size_t a = 0; a < vertexCount*UV_SIZE/sizeof(float); a++)
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

    unsigned int decorationCounter = 0;
    for(uint8_t y = 0; y < terrain->height; y++){
        for(uint8_t x = 0; x < terrain->width; x++){
            uint8_t nnibble = terrain_get_node_at(terrain, x, y, buffer, len);
            if(LEVEL_IS_LAND(nnibble)){
                terrain_square_xy(terrain, terrainVertexBuffer + posVertex, x, y);
                posVertex += 2*VERTEX_SIZE/sizeof(float);
                tilemap_get_block_UV(&terrain->tilemap, terrainUVBuffer + posUV, 2,  rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_GRASS);
                posUV += 2*UV_SIZE / sizeof(float);
            }else if(nnibble == LEVEL_BLOCK_PATH){
                uint8_t neiPattern = 0;
                neiPattern |= LEVEL_IS_PATH(terrain_get_node_at(terrain, x + 0, y - 1, buffer, len)) ? 1 << NEIGHBOUR_BIT_N : 0;
                neiPattern |= LEVEL_IS_PATH(terrain_get_node_at(terrain, x + 1, y + 0, buffer, len)) ? 1 << NEIGHBOUR_BIT_E : 0;
                neiPattern |= LEVEL_IS_PATH(terrain_get_node_at(terrain, x + 0, y + 1, buffer, len)) ? 1 << NEIGHBOUR_BIT_S : 0;
                neiPattern |= LEVEL_IS_PATH(terrain_get_node_at(terrain, x - 1, y + 0, buffer, len)) ? 1 << NEIGHBOUR_BIT_W : 0;

                uint8_t piece = 0, rotation = 0;
                terrain_path_piece(neiPattern, &piece, &rotation);
                // Build vertices
                terrain_square_xy(terrain, terrainVertexBuffer + posVertex, x, y);
                posVertex += 2*VERTEX_SIZE/sizeof(float);
                // Get UV Coordinates
                tilemap_get_block_UV_rotate(&terrain->tilemap, terrainUVBuffer + posUV, 2,  piece, LEVEL_TILEMAP_TERRAIN_PATH, rotation);
                posUV += 2*UV_SIZE / sizeof(float);
            }

            if(nnibble == LEVEL_BLOCK_DECORATION_LAND /*|| nnibble == LEVEL_BLOCK_DECORATION_WATER*/){
                decoration_create(
                    terrain->decorations + decorationCounter,
                    modellib_get_tree(&terrain->modellib, rand() % 256)
                    //terrain->modellib.models
                );
                vec3 decopos = {x - terrain->width/2, y - terrain->height/2, 1};
                float rotation = 0;//(rand() % 4) * 90;//rand() % 360;
                decoration_placement(terrain->decorations + decorationCounter, decopos, rotation, (float)(70 + (rand() % 30))/100.0f);
                decorationCounter++;
            }
        }
    }
    /*
        Normals
    */
    size_t posNormal = 0;
    posVertex = 0;
    for(size_t i = 0; i < vertexCount; i++){
        common_get_normal(terrainVertexBuffer + posVertex, terrainNormalsBuffer + posNormal);
        memcpy(terrainNormalsBuffer + posNormal + 3, terrainNormalsBuffer + posNormal, sizeof(vec3));
        memcpy(terrainNormalsBuffer + posNormal + 6, terrainNormalsBuffer + posNormal, sizeof(vec3));
        posVertex += VERTEX_SIZE/sizeof(float);
        posNormal += 3*sizeof(vec3)/sizeof(float);
    }

    vbo_create(&terrain->vbo, terrainVertexBuffer, terrainUVBuffer, terrainNormalsBuffer, vertexCount);
    // Buffer Cleanup
    free(terrainVertexBuffer);
    free(terrainUVBuffer);
    free(terrainNormalsBuffer);

    LOGS(TAG, "Terrain Created successfully");
    return 0;
}

void terrain_draw(Terrain* terrain, GLuint uniformModel, mat4 model){
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //glBindTexture( GL_TEXTURE_2D, terrain->tilemap.texture.bufferID);
    vbo_draw(&terrain->vbo, terrain->tilemap.texture.bufferID);

    /*glEnableVertexAttribArray(0);
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
    glDrawArrays(GL_TRIANGLES, 0, vertexCount * 3); // start at 0; 6 because 6 points for 2 vertices
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
    glDrawArrays(GL_TRIANGLES, 0, vertexCount * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);*/
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);

    for(size_t i = 0; i < terrain->decorationCount; i++){
        decoration_draw(terrain->decorations + i, uniformModel, model);
    }
}

void terrain_destroy(Terrain* terrain){
    vbo_destroy(&terrain->vbo);
    tilemap_destroy(&terrain->tilemap);
    modellib_destroy(&terrain->modellib);

    for(size_t i = 0; i < terrain->decorationCount; i++){
        decoration_destroy(terrain->decorations + i);
    }
    free(terrain->decorations);
    terrain->decorations = NULL;
}

