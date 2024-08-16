#include <terrainwater.h>

static const char* TAG = "TerrainWater";
static const char* TERRAINWATER_TEXTURE = "../assets/textures/world/water_default.png";
static const float TERRAIN_WATER_FREQUENCY = 10.0f;

static void terrainwater_square_xy(float* buffer, uint8_t x, uint8_t y, uint8_t width, uint8_t height){
    size_t posVertex = 0;
    short xOrigin = x - width/2;
    short yOrigin = y - height/2;

    // Vertex 1 Point 2
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + 1;
    // Vertex 1 Point 1
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + 0;
    // Vertex 1 Point 3
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + 1;


    // Vertex 2 Point 3
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + 1;
    // Vertex 2 Point 2
    buffer[posVertex++] = xOrigin + 1;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + 0;
    // Vertex 2 Point 1
    buffer[posVertex++] = xOrigin + 0;
    buffer[posVertex++] = 0.0f;
    buffer[posVertex++] = yOrigin + 0;
    return;
}

int terrainwater_create(TerrainWater* water, uint8_t* buffer, size_t bufferSize, uint8_t width, uint8_t height){
    water->animationFrame = 0;
    water->lastAnimation = glfwGetTime();

    tilemap_create(&water->tilemap, 1, TERRAINWATER_FRAMECOUNT, TERRAINWATER_TEXTURE);
    //vbo_create()
    size_t triangleCount = 0;
    for(uint8_t y = 0; y < height; y++){
        for(uint8_t x = 0; x < width; x++){
            uint8_t nodedata = common_map_get_node_at(buffer, bufferSize, x, y, width, height);//terrain_get_node_at(terrain, x, y, buffer, len);
            uint8_t nnibble = nodedata & 0b0111;
            if(LEVEL_IS_WATER(nnibble))
                triangleCount += 2;
        }
    }

    float* terrainVertexBuffer = (float*)malloc(triangleCount * VERTEX_SIZE);
    float* terrainNormalsBuffer = (float*)malloc(triangleCount * NORMALS_SIZE);
    if(terrainVertexBuffer == NULL || terrainNormalsBuffer == NULL){
        LOGERRNO(TAG, "Malloc TerrainWater Buffer");
        return -1;
    }

    memset(terrainVertexBuffer, 0.0f, triangleCount * VERTEX_SIZE);
    memset(terrainNormalsBuffer, 0.0f, triangleCount * NORMALS_SIZE);

    size_t posVertex = 0;
    for(uint8_t y = 0; y < height; y++){
        for(uint8_t x = 0; x < width; x++){
            uint8_t nodedata = common_map_get_node_at(buffer, bufferSize, x, y, width, height);//terrain_get_node_at(terrain, x, y, buffer, len);
            uint8_t nnibble = nodedata & 0b0111;
            if(!LEVEL_IS_WATER(nnibble))
                continue;

            terrainwater_square_xy(terrainVertexBuffer + posVertex, x, y, width, height);
            posVertex += 2*VERTEX_SIZE/sizeof(float);
        }
    }

    /*
        Normals
    */
    size_t posNormal = 0;
    posVertex = 0;
    for(size_t i = 0; i < triangleCount; i++){
        common_get_normal(terrainVertexBuffer + posVertex, terrainNormalsBuffer + posNormal);
        memcpy(terrainNormalsBuffer + posNormal + 3, terrainNormalsBuffer + posNormal, sizeof(vec3));
        memcpy(terrainNormalsBuffer + posNormal + 6, terrainNormalsBuffer + posNormal, sizeof(vec3));
        posVertex += VERTEX_SIZE/sizeof(float);
        posNormal += 3*sizeof(vec3)/sizeof(float);
    }

    //vbo_create(&water->vbo, terrainVertexBuffer, terrainUVBuffer, terrainNormalsBuffer, triangleCount);
    // glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
    glGenBuffers(1, &water->vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, water->vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, triangleCount * VERTEX_SIZE, terrainVertexBuffer, GL_STATIC_DRAW);
    free(terrainVertexBuffer);

    glGenBuffers(1, &water->normalsbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, water->normalsbuffer);
    glBufferData(GL_ARRAY_BUFFER, triangleCount * NORMALS_SIZE, terrainNormalsBuffer, GL_STATIC_DRAW);
    free(terrainNormalsBuffer);

    glGenBuffers(TERRAINWATER_FRAMECOUNT, water->uvbuffers);
    float* terrainUVBuffer = (float*)malloc(triangleCount * UV_SIZE);
    if(!terrainUVBuffer){
        LOGE(TAG, "MALLOC UV BUFFER");
        return -1;
    }
    for(uint8_t i = 0; i < TERRAINWATER_FRAMECOUNT; i++){
        //clear
        memset(terrainUVBuffer, 0.0f, triangleCount * UV_SIZE);
        size_t posUV = 0;
        // get frame
        for(uint8_t j = 0; j < triangleCount/2; j++){
            tilemap_get_block_UV(&water->tilemap, terrainUVBuffer + posUV, 2,  0, i);
            posUV += 2*UV_SIZE / sizeof(float);
        }
        //common_print_uvs(frameCoordinates, 2*UV_SIZE);
        glBindBuffer(GL_ARRAY_BUFFER, water->uvbuffers[i]);
        glBufferData(GL_ARRAY_BUFFER, triangleCount*UV_SIZE, terrainUVBuffer, GL_STATIC_DRAW);
        //printf("Buffer %d: %d\n", i, water->uvbuffers);
    }
    free(terrainUVBuffer);

    water->triangleCount = triangleCount;
    LOGS(TAG, "Built water successfully!");
    return 0;
}

void terrainwater_draw(TerrainWater* water, GLuint uniformModel, GLuint uniformNormalMatrix, mat4 model){
    double thisTime = glfwGetTime();
    if((thisTime - water->lastAnimation) > (1.0f/TERRAIN_WATER_FREQUENCY)){
        water->animationFrame++;
        water->animationFrame %= TERRAINWATER_FRAMECOUNT;
        water->lastAnimation = thisTime;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);       // Face culling has to be inverted for this voxel mode

    glBindTexture( GL_TEXTURE_2D, water->tilemap.texture.bufferID);
    // VERTICES
    glEnableVertexAttribArray(VBO_INDEX_VERTEX);
    glBindBuffer(GL_ARRAY_BUFFER, water->vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, water->triangleCount * 3);
    // UV
    glEnableVertexAttribArray(VBO_INDEX_UV);
    glBindBuffer(GL_ARRAY_BUFFER, water->uvbuffers[water->animationFrame]);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0  // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, water->triangleCount * 3);
    // NORMALS
    glEnableVertexAttribArray(VBO_INDEX_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, water->normalsbuffer);
    glVertexAttribPointer(
        2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
        3,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, water->triangleCount * 3);

    glDisableVertexAttribArray(VBO_INDEX_VERTEX);
    glDisableVertexAttribArray(VBO_INDEX_UV);
    glDisableVertexAttribArray(VBO_INDEX_NORMAL);

    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);

    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);
}

void terrainwater_destroy(TerrainWater* water){
    glDeleteBuffers(1, &water->vertexbuffer);
    glDeleteBuffers(1, &water->normalsbuffer);
    glDeleteBuffers(TERRAINWATER_FRAMECOUNT, water->uvbuffers);
    tilemap_destroy(&water->tilemap);
}
