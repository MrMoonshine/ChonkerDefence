
#include <terrain.h>

static const char* TAG = "Terrain";

static const uint8_t NEIGHBOUR_BIT_N  = 0;
static const uint8_t NEIGHBOUR_BIT_E  = 1;
static const uint8_t NEIGHBOUR_BIT_S  = 2;
static const uint8_t NEIGHBOUR_BIT_W  = 3;

static const uint16_t NEIGHBOUR_MASK_NORMAL  = 0x00f;
static const uint16_t NEIGHBOUR_MASK_DIGONAL = 0x0f0;
static const uint16_t NEIGHBOUR_MASK_STEEP   = 0xf00;

static const uint8_t terrain_get_node_at(Terrain* terrain, uint8_t x, uint8_t y, uint8_t *buffer, size_t len){
    size_t pos = (y * terrain->width) + x;

    if(pos >= len || x > terrain->width - 1 || y > terrain->height - 1)
        return 0xF0 | LEVEL_BLOCK_WATER;

    return buffer[pos];
}

static const uint16_t terrain_get_elevation_neighbors(Terrain* terrain, uint8_t x, uint8_t y, uint8_t *buffer, size_t len){
    //uint8_t matchpattern = 0b0111;
    uint8_t matchelevation = (0b01110000 & terrain_get_node_at(terrain, x, y, buffer, len)) >> 4;
    matchelevation++;
    matchelevation &= 0b0111;
    //matchpattern |= matchelevation << 4;

    uint16_t neipattern = 0;
    uint8_t node = 0;
    for(uint8_t i = 0; i < 12; i++){
        // Use last 4 bit to store info about steep neighbours. (only diagonal diff 2 blocks)
        bool steep = i > 7;
        uint8_t myi = steep ? i - 4: i;
        uint8_t nx = x, ny = y;
        switch(myi){
            // Normal neighbours
            case 0:
                ny++;
                break;
            case 1:
                nx++;
                break;
            case 2:
                ny--;
                break;
            case 3:
                nx--;
                break;
            // Diagonal leighbours
            case 4:
                nx--;
                ny++;
                break;
            case 5:
                nx++;
                ny++;
                break;
            case 6:
                nx++;
                ny--;
                break;
            case 7:
                nx--;
                ny--;
                break;
            default:
                break;
        }
        node = terrain_get_node_at(terrain, nx, ny, buffer, len);
        neipattern |= ((0b0111 & (node >> 4)) == (steep ? matchelevation + 1: matchelevation) && LEVEL_IS_SKIRT_REQURED((node & 0b0111)) ? 1 : 0) << i;
    }
    return neipattern;
}

static uint8_t terrain_count_neighbours_from_pattern(uint16_t pattern){
    uint8_t neiCounter = 0;
    for(uint8_t i = 0; i < 16; i++){
        if(pattern & (1 << i))
            neiCounter++;
    }
    return neiCounter;
}

static void terrain_square_xy(Terrain* terrain, float* buffer, uint8_t x, uint8_t y, uint8_t height){
    size_t posVertex = 0;
    short xOrigin = x - terrain->width/2;
    short yOrigin = y - terrain->height/2;

    // Vertex 1 Point 2
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = (float)height;
    buffer[posVertex++] = yOrigin + 1;
    // Vertex 1 Point 1
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = (float)height;
    buffer[posVertex++] = yOrigin + 0;
    // Vertex 1 Point 3
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = (float)height;
    buffer[posVertex++] = yOrigin + 1;


    // Vertex 2 Point 3
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = (float)height;
    buffer[posVertex++] = yOrigin + 1;
    // Vertex 2 Point 2
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = (float)height;
    buffer[posVertex++] = yOrigin + 0;
    // Vertex 2 Point 1
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = (float)height;
    buffer[posVertex++] = yOrigin + 0;
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

    // Vertex 1 Point 3
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + deltaY * 0;
    // Vertex 1 Point 1
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = 1.0f;
    buffer[posVertex++] = yOrigin + deltaY * 1;
    // Vertex 1 Point 2
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = 1.0f;
    buffer[posVertex++] = yOrigin + deltaY * 0;

    // Vertex 2 Point 1
    buffer[posVertex++] = xOrigin + deltaX * 0;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + deltaY * 0;
    // Vertex 2 Point 2
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + deltaY * 1;
    // Vertex 2 Point 3
    buffer[posVertex++] = xOrigin + deltaX * 1;
    buffer[posVertex++] = 1.0f;
    buffer[posVertex++] = yOrigin + deltaY * 1;

    if(neiCounter == 2){
        // Lid for inner corners
        // Vertex 1 Point 1
        buffer[posVertex++] = xOrigin + deltaX * 0;
        buffer[posVertex++] = 1.0f;
        buffer[posVertex++] = yOrigin + deltaY * 0;
        // Vertex 1 Point 2
        buffer[posVertex++] = xOrigin + deltaX * 1;
        buffer[posVertex++] = 1.0f;
        buffer[posVertex++] = yOrigin + deltaY * 1;
        // Vertex 1 Point 3
        if(sonw){
            buffer[posVertex++] = x - terrain->width/2 + (neighbourPattern == 0b0110 ? 1 : 0);
            buffer[posVertex++] = 1.0f;
            buffer[posVertex++] = y - terrain->height/2 + (neighbourPattern == 0b0110 ? 1 : 0);

        }else{
            buffer[posVertex++] = xOrigin + deltaX * 1;
            buffer[posVertex++] = 1.0f;
            buffer[posVertex++] = yOrigin + deltaY * 0;
        }
    }

    return retval;
}

static size_t terrain_square_xy_elevation_skirt(Terrain* terrain, float* buffer, uint8_t x, uint8_t y, uint8_t elevation, uint16_t neighbourPattern){
    size_t retval = 0;
    uint8_t neiCounterNorm = terrain_count_neighbours_from_pattern(neighbourPattern & NEIGHBOUR_MASK_NORMAL);
    uint8_t neiCounterDiag = terrain_count_neighbours_from_pattern(neighbourPattern & NEIGHBOUR_MASK_DIGONAL);
    uint8_t neiCounterSteep = terrain_count_neighbours_from_pattern(neighbourPattern & NEIGHBOUR_MASK_STEEP);

    uint8_t neiPatternNorm = neighbourPattern & NEIGHBOUR_MASK_NORMAL;
    uint8_t neiPatternDiag = 0x0f & ((neighbourPattern & NEIGHBOUR_MASK_DIGONAL) >> 4);
    uint8_t neiPatternSteep = 0x0f & ((neighbourPattern & NEIGHBOUR_MASK_STEEP) >> 8);

    short xOrigin = x - terrain->width/2;
    short yOrigin = y - terrain->height/2;
    short zOrigin = elevation;
    int8_t deltaX = 1;
    int8_t deltaY = 1;
    int8_t deltaZ = 1;
    bool sonw = false;

    vec3 rotationaxis = {0, 1, 0};
    float angle = 0;

    size_t posVertex = 0;

    vec3 points[6] = {
        // Vertex 1
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        // Vertex 2
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };

    if(neiCounterNorm == 1){
        /*
            USUAL SLOPE
        */
        retval = 2;
        // Triangle 1 Point 1
        points[0][0] = 0;
        points[0][1] = 0;
        points[0][2] = 0;
        // Triangle 1 Point 2
        points[1][0] = 1;
        points[1][1] = 1;
        points[1][2] = 1;
        // Triangle 1 Point 3
        points[2][0] = 1;
        points[2][1] = 1;
        points[2][2] = 0;
        // Triangle 1 Point 1
        points[3][0] = 0;
        points[3][1] = 0;
        points[3][2] = 0;
        // Triangle 1 Point 2
        points[4][0] = 0;
        points[4][1] = 0;
        points[4][2] = 1;
        // Triangle 1 Point 3
        points[5][0] = 1;
        points[5][1] = 1;
        points[5][2] = 1;

        switch(neighbourPattern & NEIGHBOUR_MASK_NORMAL){
            case 0b0001:
                // North
                angle = 270;
                xOrigin += 1;
                break;
            case 0b0010:
                // East
                angle = 0;
                break;
            case 0b0100:
                // South
                angle = 90;
                yOrigin += 1;
                break;
            case 0b1000:
                // West
                angle = 180;
                xOrigin += 1;
                yOrigin += 1;
                break;
            default:
                break;
        }
    }else if(neiCounterSteep == 1 && neiCounterNorm == 2){
        /*
            STEEP OUTER SLOPE
        */
        retval = 2;
        // Triangle 1 Point 1
        points[0][0] = 0;
        points[0][1] = 1;
        points[0][2] = 1;
        // Triangle 1 Point 2
        points[1][0] = 1;
        points[1][1] = 1;
        points[1][2] = 0;
        // Triangle 1 Point 3
        points[2][0] = 0;
        points[2][1] = 2;
        points[2][2] = 0;
        // Triangle 1 Point 1
        points[3][0] = 0;
        points[3][1] = 1;
        points[3][2] = 1;
        // Triangle 1 Point 2
        points[4][0] = 1;
        points[4][1] = 0;
        points[4][2] = 1;
        // Triangle 1 Point 3
        points[5][0] = 1;
        points[5][1] = 1;
        points[5][2] = 0;

        switch(neiPatternSteep){
            case 0b0001:
                // North-West
                angle = 90;
                yOrigin += 1;
                break;
            case 0b0010:
                // North-East
                angle = 180;
                xOrigin += 1;
                yOrigin += 1;
                break;
            case 0b0100:
                // South-East
                angle = 270;
                xOrigin += 1;
                break;
            case 0b1000:
                // South-West
                break;
            default:
                break;
        }
    }else if(neiCounterDiag == 1 && neiCounterNorm == 0){
        /*
            OUTER DIAGONAL SLOPE
        */
        retval = 2;
        // Triangle 1 Point 1
        points[0][0] = 0;
        points[0][1] = 0;
        points[0][2] = 1;
        // Triangle 1 Point 2
        points[1][0] = 1;
        points[1][1] = 0;
        points[1][2] = 0;
        // Triangle 1 Point 3
        points[2][0] = 0;
        points[2][1] = 0;
        points[2][2] = 0;
        // Triangle 1 Point 1
        points[3][0] = 0;
        points[3][1] = 0;
        points[3][2] = 1;
        // Triangle 1 Point 2
        points[4][0] = 1;
        points[4][1] = 1;
        points[4][2] = 1;
        // Triangle 1 Point 3
        points[5][0] = 1;
        points[5][1] = 0;
        points[5][2] = 0;

        switch(neiPatternDiag){
            case 0b0001:
                // North-West
                angle = 270;
                xOrigin += 1;
                break;
            case 0b0010:
                // North-East
                break;
            case 0b0100:
                // South-East
                angle = 90;
                yOrigin += 1;
                break;
            case 0b1000:
                // South-West
                angle = 180;
                xOrigin += 1;
                yOrigin += 1;
                break;
            default:
                break;
        }
    }else if(neiCounterDiag > 0 && neiCounterNorm == 2){
        /*
            INNER DIAGONAL SLOPE
        */
        retval = 2;
        // Triangle 1 Point 1
        points[0][0] = 0;
        points[0][1] = 1;
        points[0][2] = 1;
        // Triangle 1 Point 2
        points[1][0] = 1;
        points[1][1] = 1;
        points[1][2] = 0;
        // Triangle 1 Point 3
        points[2][0] = 0;
        points[2][1] = 1;
        points[2][2] = 0;
        // Triangle 1 Point 1
        points[3][0] = 0;
        points[3][1] = 1;
        points[3][2] = 1;
        // Triangle 1 Point 2
        points[4][0] = 1;
        points[4][1] = 0;
        points[4][2] = 1;
        // Triangle 1 Point 3
        points[5][0] = 1;
        points[5][1] = 1;
        points[5][2] = 0;

        switch(neiPatternNorm){
            case 0b0011:
                // North-East
                angle = 180;
                yOrigin += 1;
                xOrigin += 1;
                break;
            case 0b0110:
                // South-East
                angle = 270;
                xOrigin += 1;
                break;
            case 0b1100:
                // South-West
                break;
            case 0b1001:
                // North-West
                angle = 90;
                yOrigin += 1;
                break;
            default:
                break;
        }

        /*switch(neiPatternDiag){
            case 0b0001:
                // North-West
                angle = 90;
                yOrigin += 1;
                break;
            case 0b0010:
                // North-East
                angle = 180;
                yOrigin += 1;
                xOrigin += 1;
                break;
            case 0b0100:
                // South-East
                angle = 270;
                xOrigin += 1;
                break;
            case 0b1000:
                // South-West
                break;
            default:
                break;
        }*/
    }

    for(uint8_t i = 0; i < 6; i++){
        glm_vec3_rotate(points[i], glm_rad(angle), rotationaxis);
        points[i][0] += xOrigin;
        points[i][1] += zOrigin;
        points[i][2] += yOrigin;
        // Round results
        for(uint8_t j = 0; j < 3; j++)
            points[i][j] = round(points[i][j]);

        memcpy(buffer + posVertex, points[i], sizeof(vec3));
        posVertex += 3;
    }
/*
    if(neiCounter == 2){
        // Lid for inner corners
        // Vertex 1 Point 1
        buffer[posVertex++] = xOrigin + deltaX * 0;
        buffer[posVertex++] = 1.0f;
        buffer[posVertex++] = yOrigin + deltaY * 0;
        // Vertex 1 Point 2
        buffer[posVertex++] = xOrigin + deltaX * 1;
        buffer[posVertex++] = 1.0f;
        buffer[posVertex++] = yOrigin + deltaY * 1;
        // Vertex 1 Point 3
        if(sonw){
            buffer[posVertex++] = x - terrain->width/2 + (neighbourPattern == 0b0110 ? 1 : 0);
            buffer[posVertex++] = 1.0f;
            buffer[posVertex++] = y - terrain->height/2 + (neighbourPattern == 0b0110 ? 1 : 0);

        }else{
            buffer[posVertex++] = xOrigin + deltaX * 1;
            buffer[posVertex++] = 1.0f;
            buffer[posVertex++] = yOrigin + deltaY * 0;
        }
    }*/

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
        if(i%(terrain->width) == (terrain->width) - 1)
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
            uint8_t nodedata = terrain_get_node_at(terrain, x, y, buffer, len);
            uint8_t nnibble = nodedata & 0b0111;
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
                //uint8_t elevationNei = terrain_get_elevation_neighbors(terrain, x, y, buffer, len);
                //uint8_t elevationNeiCounterNorm = terrain_count_neighbours_from_pattern(elevationNei & NEIGHBOUR_MASK_NORMAL);
                //uint8_t elevationNeiCounterDiag = terrain_count_neighbours_from_pattern(elevationNei & NEIGHBOUR_MASK_DIGONAL);
                vertexCount += 2;
                //if(elevationNei > 0)
                    //printf("(%d|%d) => Neighbours %.2x\n", x,y,elevationNei);

            }else if(LEVEL_IS_WATER(nnibble)){
                uint8_t neiCounter = 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED((0b0111 & terrain_get_node_at(terrain, x + 1, y + 0, buffer, len))) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED((0b0111 & terrain_get_node_at(terrain, x + 0, y + 1, buffer, len))) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED((0b0111 & terrain_get_node_at(terrain, x - 1, y + 0, buffer, len))) ? 1 : 0;
                neiCounter += LEVEL_IS_SKIRT_REQURED((0b0111 & terrain_get_node_at(terrain, x + 0, y - 1, buffer, len))) ? 1 : 0;

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
            uint8_t nodedata = terrain_get_node_at(terrain, x, y, buffer, len);
            uint8_t nnibble = nodedata & 0b0111;
            if(LEVEL_IS_WATER(nnibble)){
                uint8_t neiPattern = 0;
                neiPattern |= LEVEL_IS_SKIRT_REQURED((0b0111 & terrain_get_node_at(terrain, x + 0, y - 1, buffer, len))) ? 1 << NEIGHBOUR_BIT_N : 0;
                neiPattern |= LEVEL_IS_SKIRT_REQURED((0b0111 & terrain_get_node_at(terrain, x + 1, y + 0, buffer, len))) ? 1 << NEIGHBOUR_BIT_E : 0;
                neiPattern |= LEVEL_IS_SKIRT_REQURED((0b0111 & terrain_get_node_at(terrain, x + 0, y + 1, buffer, len))) ? 1 << NEIGHBOUR_BIT_S : 0;
                neiPattern |= LEVEL_IS_SKIRT_REQURED((0b0111 & terrain_get_node_at(terrain, x - 1, y + 0, buffer, len))) ? 1 << NEIGHBOUR_BIT_W : 0;

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
            uint8_t nodedata = terrain_get_node_at(terrain, x, y, buffer, len);
            uint8_t elevation = 1 + ((nodedata >> 4) & 0b0111);
            uint8_t nnibble = nodedata & 0b0111;
            uint16_t elevationNei = terrain_get_elevation_neighbors(terrain, x, y, buffer, len);
            if(LEVEL_IS_LAND(nnibble)){
                //uint8_t elevationNeiCounterNorm = terrain_count_neighbours_from_pattern(elevationNei & NEIGHBOUR_MASK_NORMAL);
                //uint8_t elevationNeiCounterDiag = terrain_count_neighbours_from_pattern(elevationNei & NEIGHBOUR_MASK_DIGONAL);
                //printf("NodeData is %.2x\tElevation is %d\teNei Patten %.2x\n",nodedata, elevation, elevationNei);
                if(elevationNei == 0){
                    terrain_square_xy(terrain, terrainVertexBuffer + posVertex, x, y, elevation);
                    posVertex += 2*VERTEX_SIZE/sizeof(float);
                    tilemap_get_block_UV(&terrain->tilemap, terrainUVBuffer + posUV, 2,  rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_GRASS);
                    posUV += 2*UV_SIZE / sizeof(float);
                }else{
                    terrain_square_xy_elevation_skirt(terrain, terrainVertexBuffer + posVertex, x, y, elevation, elevationNei);
                    posVertex += 2*VERTEX_SIZE/sizeof(float);
                    tilemap_get_block_UV(&terrain->tilemap, terrainUVBuffer + posUV, 2,  rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_GRASS);
                    posUV += 2*UV_SIZE / sizeof(float);
                }
            }else if(nnibble == LEVEL_BLOCK_PATH){
                uint8_t neiPattern = 0;
                neiPattern |= LEVEL_IS_PATH((0b0111 & terrain_get_node_at(terrain, x + 0, y - 1, buffer, len))) ? 1 << NEIGHBOUR_BIT_N : 0;
                neiPattern |= LEVEL_IS_PATH((0b0111 & terrain_get_node_at(terrain, x + 1, y + 0, buffer, len))) ? 1 << NEIGHBOUR_BIT_E : 0;
                neiPattern |= LEVEL_IS_PATH((0b0111 & terrain_get_node_at(terrain, x + 0, y + 1, buffer, len))) ? 1 << NEIGHBOUR_BIT_S : 0;
                neiPattern |= LEVEL_IS_PATH((0b0111 & terrain_get_node_at(terrain, x - 1, y + 0, buffer, len))) ? 1 << NEIGHBOUR_BIT_W : 0;

                uint8_t piece = 0, rotation = 0;
                terrain_path_piece(neiPattern, &piece, &rotation);

                if(elevationNei == 0){
                    terrain_square_xy(terrain, terrainVertexBuffer + posVertex, x, y, elevation);
                    posVertex += 2*VERTEX_SIZE/sizeof(float);
                    tilemap_get_block_UV_rotate(&terrain->tilemap, terrainUVBuffer + posUV, 2,  piece, LEVEL_TILEMAP_TERRAIN_PATH, rotation);
                    posUV += 2*UV_SIZE / sizeof(float);
                }else{
                    // Build vertices
                    terrain_square_xy_elevation_skirt(terrain, terrainVertexBuffer + posVertex, x, y, elevation, elevationNei);
                    posVertex += 2*VERTEX_SIZE/sizeof(float);
                    // Get UV Coordinates
                    tilemap_get_block_UV_rotate(&terrain->tilemap, terrainUVBuffer + posUV, 2,  piece, LEVEL_TILEMAP_TERRAIN_PATH, 1);
                    posUV += 2*UV_SIZE / sizeof(float);
                }
            }

            if(nnibble == LEVEL_BLOCK_DECORATION_LAND /*|| nnibble == LEVEL_BLOCK_DECORATION_WATER*/){
                decoration_create(
                    terrain->decorations + decorationCounter,
                    modellib_get_tree(&terrain->modellib, rand() % 256)
                    //terrain->modellib.models
                );
                vec3 decopos = {
                    x - terrain->width/2,
                    elevation,
                    y - terrain->height/2
                };
                float rotation = rand() % 360;
                decoration_placement(
                    terrain->decorations + decorationCounter,
                    decopos,
                    rotation,
                    (float)(70 + (rand() % 30))/100.0f
                );
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

void terrain_draw(Terrain* terrain, GLuint uniformModel, GLuint uniformNormalMatrix, mat4 model){
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    vbo_draw(&terrain->vbo, terrain->tilemap.texture.bufferID);
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);

    for(size_t i = 0; i < terrain->decorationCount; i++){
        decoration_draw(terrain->decorations + i, uniformModel, uniformNormalMatrix, model);
        /*decoration_placement(
            terrain->decorations + i, (terrain->decorations + i)->position,
            (int)(glfwGetTime() * 64.0f) % 360,
            (terrain->decorations + i)->scale
        );*/
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

