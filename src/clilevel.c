#include <clilevel.h>

static const char* TAG = "Level (Client-Side)";

static const vec3 X_AXIS = {1, 0, 0};
static const vec3 Y_AXIS = {0, 1, 0};
static const vec3 Z_AXIS = {0, 0, 1};

/*static const float ISOMETRIC_ROTATION_VERTICAL = 45.0f;
static const float ISOMETRIC_ROTATION_HORIZONTAL = 35.264f;*/

static const float CLILEVEL_ORTHO_DEPTH = 1000.0f;
//static vec3 eye = {sqrt(1 / 3.0), sqrt(1 / 3.0), sqrt(1 / 3.0)};
static vec3 eye = {4, 3, 3};
static vec3 center = {0, 0, 0};
static vec3 up = {0, 1, 0};

static mat4 model;
static mat4 view;
static mat4 projection;

static const uint8_t NEIGHBOUR_BIT_N  = 0;
static const uint8_t NEIGHBOUR_BIT_E  = 1;
static const uint8_t NEIGHBOUR_BIT_S  = 2;
static const uint8_t NEIGHBOUR_BIT_W  = 3;

/*static const uint8_t NEIGHBOUR_POSITION_N  = 1 << NEIGHBOUR_BIT_N;
static const uint8_t NEIGHBOUR_POSITION_E  = 1 << NEIGHBOUR_BIT_E;
static const uint8_t NEIGHBOUR_POSITION_S  = 1 << NEIGHBOUR_BIT_S;
static const uint8_t NEIGHBOUR_POSITION_W  = 1 << NEIGHBOUR_BIT_W;;

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

static const GLfloat g_uv_buffer_data[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f
};
*/
static const uint8_t clilevel_get_node_at(ClientLevel *level, uint8_t x, uint8_t y, uint8_t *buffer, size_t len){
    size_t totalPos = (y * level->width) + x;
    size_t pos = totalPos / 2;

    if(pos >= len || x > level->width - 1 || y > level->height - 1)
        return 0xFF;

    return totalPos % 2 ? (buffer[pos] & 0x0f) : ((buffer[pos] & 0xf0) >> 4);
}

static void clilevel_square_xy(ClientLevel *level, float* buffer, uint8_t x, uint8_t y){
    size_t posVertex = 0;
    short xOrigin = x - level->width/2;
    short yOrigin = y - level->height/2;
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

static size_t clilevel_square_xy_skirt(ClientLevel *level, float* buffer, uint8_t x, uint8_t y, uint8_t neighbourPattern){
    uint8_t neiCounter = 0;
    for(uint8_t i = 0; i < 4; i++){
        if(neighbourPattern & (1 << i))
            neiCounter++;
    }

    if(neiCounter < 1){
        return 0;
    }

    printf("(%d|%d) has %d land neighbours: hex: %x\n", x, y, neiCounter, neighbourPattern);

    short xOrigin = x - level->width/2;
    short yOrigin = y - level->height/2;
    int8_t deltaX = 1;
    int8_t deltaY = 1;

    if(neiCounter == 1){
        switch(neighbourPattern){
            case 0b0001:
                xOrigin += 0;
                yOrigin += 1;
                deltaX = 1;
                deltaY = 0;
                break;
            case 0b0010:
                xOrigin += 1;
                yOrigin += 1;
                deltaX = 0;
                deltaY = -1;
                break;
            case 0b0100:
                xOrigin += 1;
                yOrigin += 1;
                deltaX = -0;
                deltaY = 0;
                break;
            case 0b1000:
                xOrigin += 0;
                yOrigin += 0;
                deltaX = 0;
                deltaY = 1;
                break;
            default:
                break;
        }

        size_t posVertex = 0;
        // Vertex 1 Point 1
        buffer[posVertex++] = xOrigin;
        buffer[posVertex++] = yOrigin;
        buffer[posVertex++] = 0.0f;
        // Vertex 1 Point 2
        buffer[posVertex++] = xOrigin + deltaX * 1;
        buffer[posVertex++] = yOrigin + deltaY * 1;
        buffer[posVertex++] = 0.0f;
        // Vertex 1 Point 3
        buffer[posVertex++] = xOrigin + deltaX * 0;
        buffer[posVertex++] = yOrigin + deltaY * 1;
        buffer[posVertex++] = 1.0f;
        // Vertex 2 Point 1
        buffer[posVertex++] = xOrigin;
        buffer[posVertex++] = yOrigin;
        buffer[posVertex++] = 0.0f;
        // Vertex 2 Point 2
        buffer[posVertex++] = xOrigin + deltaX;
        buffer[posVertex++] = yOrigin;
        buffer[posVertex++] = 1.0f;
        // Vertex 2 Point 3
        buffer[posVertex++] = xOrigin + deltaX;
        buffer[posVertex++] = yOrigin + deltaY;
        buffer[posVertex++] = 1.0f;

        /*for(uint8_t i = 0; i < 3*6; i++){
            printf("%.2f, ", buffer[i]);
            if(i % 3 == 2)
                printf("\n");
        }*/
        return 2;
    }

    return 0;
}

static int clilevel_build(ClientLevel *level, uint8_t *buffer, size_t len){
    level->windowScale = 1;
    level->modelScale = 1;

    level->terrainVertexCount = 0;

    /*for(size_t i = 0; i < len; i++){
        printf("%.2x", buffer[i]);
        if(i%(level->width/2) == (level->width/2) - 1)
            printf("\n");
    }
    printf("-------------------------\n");*/
    printf("   ");
    for(uint8_t x = 0; x < level->width; x++)
        printf("%.1d", x/10);
    printf("\n   ");
    for(uint8_t x = 0; x < level->width; x++)
        printf("%.1d", x % 10);
    printf("\n");
    for(uint8_t y = 0; y < level->height; y++){
        printf("%.2d ", y);
        for(uint8_t x = 0; x < level->width; x++){
            uint8_t nnibble = clilevel_get_node_at(level, x, y, buffer, len);
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

            if(LEVEL_IS_LAND(nnibble)){
                level->terrainVertexCount += 2;
            }else if(LEVEL_IS_WATER(nnibble)){
                uint8_t neiCounter = 0;
                neiCounter += LEVEL_IS_LAND(clilevel_get_node_at(level, x + 1, y + 0, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_LAND(clilevel_get_node_at(level, x + 0, y + 1, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_LAND(clilevel_get_node_at(level, x - 1, y + 0, buffer, len)) ? 1 : 0;
                neiCounter += LEVEL_IS_LAND(clilevel_get_node_at(level, x + 0, y - 1, buffer, len)) ? 1 : 0;

                switch(neiCounter){
                    case 1: level->terrainVertexCount += 2; break;
                    case 2: level->terrainVertexCount += 3; break;
                    case 3: level->terrainVertexCount += 3*2; break;
                    default: break;
                }
            }
        }
        printf("\n");
    }
    printf("[INFO] %s: Vertexcount terrain is %lu\n", TAG, level->terrainVertexCount);
    float* terrainVertexBuffer = (float*)malloc(level->terrainVertexCount * VERTEX_SIZE);
    float* terrainUVBuffer = (float*)malloc(level->terrainVertexCount * UV_SIZE);
    if(terrainUVBuffer == NULL || terrainVertexBuffer == NULL){
        LOGERRNO(TAG, "Malloc Terrain Buffer");
        return -1;
    }
    // Zero all buffers
    for(size_t a = 0; a < level->terrainVertexCount*VERTEX_SIZE/sizeof(float); a++)
        terrainVertexBuffer[a] = 0.0f;
    for(size_t a = 0; a < level->terrainVertexCount*UV_SIZE/sizeof(float); a++)
        terrainUVBuffer[a] = 0.0f;

    tilemap_create(&level->tilemap, LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_HEIGHT, LEVEL_TILEMAP_TERRAIN_FILE);

    // Build terrain
    size_t posVertex = 0;
    size_t posUV = 0;
    for(uint8_t y = 0; y < level->height; y++){
        for(uint8_t x = 0; x < level->width; x++){
            uint8_t nnibble = clilevel_get_node_at(level, x, y, buffer, len);
            if(LEVEL_IS_LAND(nnibble)){
                clilevel_square_xy(level, terrainVertexBuffer + posVertex, x, y);
                posVertex += 2*VERTEX_SIZE/sizeof(float);
                tilemap_get_UV_square(&level->tilemap, terrainUVBuffer + posUV, rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_HEIGHT - 1);
                posUV += 2*UV_SIZE / sizeof(float);
            }else if(LEVEL_IS_WATER(nnibble)){
                uint8_t neiPattern = 0;

                neiPattern |= LEVEL_IS_LAND(clilevel_get_node_at(level, x + 0, y + 1, buffer, len)) ? 1 << NEIGHBOUR_BIT_N : 0;
                neiPattern |= LEVEL_IS_LAND(clilevel_get_node_at(level, x + 1, y + 0, buffer, len)) ? 1 << NEIGHBOUR_BIT_E : 0;
                neiPattern |= LEVEL_IS_LAND(clilevel_get_node_at(level, x + 0, y - 1, buffer, len)) ? 1 << NEIGHBOUR_BIT_S : 0;
                neiPattern |= LEVEL_IS_LAND(clilevel_get_node_at(level, x - 1, y + 0, buffer, len)) ? 1 << NEIGHBOUR_BIT_W : 0;

                size_t terrainSkirtVertexCount = clilevel_square_xy_skirt(level, terrainVertexBuffer + posVertex, x, y, neiPattern);
                posVertex += terrainSkirtVertexCount * VERTEX_SIZE/sizeof(float);
                switch(terrainSkirtVertexCount){
                    case 2:
                        tilemap_get_UV_square(&level->tilemap, terrainUVBuffer + posUV, rand() % LEVEL_TILEMAP_TERRAIN_WIDTH, 1);
                        posUV += 2*UV_SIZE / sizeof(float);
                        break;
                    default: break;
                }
                printf("Oida total: %lu\tskirt: %lu\n", level->terrainVertexCount, posVertex / (VERTEX_SIZE/sizeof(float)));
                /*switch(neiCounter){
                    case 1: level->terrainVertexCount += 2; break;
                    case 2: level->terrainVertexCount += 3; break;
                    case 3: level->terrainVertexCount += 3*2; break;
                    default: break;
                }*/
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

    glGenVertexArrays(1, &level->vao);
    glBindVertexArray(level->vao);
    level->shader = glshader_load("../shaders/gamever.glsl", "../shaders/gamefra.glsl");

    glm_ortho(0.0f, (float)APP_WIDTH, 0.0f, (float)APP_HEIGHT, -1000.0f, 1000.0f, projection);

    //glm_vec3_rotate(up, glm_rad(ISOMETRIC_ROTATION_HORIZONTAL), (float*)X_AXIS);
    // glm_vec3_rotate(up, glm_rad(ISOMETRIC_ROTATION_VERTICAL), (float*)Z_AXIS);
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);

    level->projection = glGetUniformLocation(level->shader, "projection");
    level->view = glGetUniformLocation(level->shader, "view");
    level->model = glGetUniformLocation(level->shader, "model");

    glGenBuffers(1, &level->terrainVertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, level->terrainVertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, posVertex*sizeof(float), terrainVertexBuffer, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &level->terrainUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, level->terrainUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, posUV*sizeof(float), terrainUVBuffer, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
    // Buffer Cleanup
    free(terrainVertexBuffer);
    free(terrainUVBuffer);
    return 0;
}

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

uint8_t clilevel_get_level(ClientLevel *level, Client *client, GLFWwindow* window, size_t *len, uint8_t levelID){
    level->window = window;
    level->client = client;

    client_flush(client);
    uint8_t cmd[] = {PROTO_SCOPE_LEVELS, PROTO_CMD_SHOW, levelID};
    send(client->socket, cmd, sizeof(cmd), 0);

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
    /*for(size_t i = 0; i < *len; i++){
        printf("%.2x, ", buffer[i]);
        if(i%8 == 7)
            printf("\n");
    }*/

    printf("[INFO] %s: Size is %lu\n", TAG, *len);
    size_t pos = 0;
    memcpy(level->name, buffer + pos, LEVEL_NAME_LENGTH);
    pos += LEVEL_NAME_LENGTH;
    memcpy(level->style, buffer + pos, LEVEL_STYLE_LENGTH);
    pos += LEVEL_STYLE_LENGTH;
    level->width = buffer[pos++];
    level->height = buffer[pos++];
    pos += LEVEL_RESERVED_LENGTH;
    clilevel_build(level, buffer + pos, (*len) - pos);

    free(buffer);
    return 0;
}

void clilevel_destroy(ClientLevel *level){
    glDeleteBuffers(1, &level->terrainUVBuffer);
    glDeleteBuffers(1, &level->terrainVertexbuffer);
    tilemap_destroy(&level->tilemap);
    glDeleteProgram(level->shader);
	glDeleteVertexArrays(1, &level->vao);
}

void clilevel_dump(ClientLevel *level){
    printf("+--- %s ---+\r\n", level->name);
    printf(" - Style:\t%s\r\n", level->style);
    printf(" - Width:\t%d\r\n", level->width);
    printf(" - Height:\t%d\r\n", level->height);
}

void clilevel_resize(ClientLevel *level, int width, int height){
    level->windowScale = (float)height/(float)APP_HEIGHT;

    glBindVertexArray(level->vao);
    glm_ortho(0.0f, width, 0.0f, height, -CLILEVEL_ORTHO_DEPTH, CLILEVEL_ORTHO_DEPTH, projection);
    //glm_lookat(eye, center, up, view);
}

void clilevel_enable_vao(ClientLevel* level){
    glUseProgram(level->shader);
    glBindVertexArray(level->vao);

    glUniformMatrix4fv(level->projection, 1, GL_FALSE, *projection);
    glUniformMatrix4fv(level->view, 1, GL_FALSE, *view);
    //glUniformMatrix4fv(level->model, 1, GL_FALSE, *model);
}

static float moveX = 22.0f;
static float moveY = 0.0f;
static float angle = -90.0f;
int clilevel_draw(ClientLevel *level){
    moveX += glfwGetKey(level->window, GLFW_KEY_DOWN ) == GLFW_PRESS ? 0.2f : 0.0f;
    moveX += glfwGetKey(level->window, GLFW_KEY_UP ) == GLFW_PRESS ? -0.2f : 0.0f;
    moveY += glfwGetKey(level->window, GLFW_KEY_RIGHT ) == GLFW_PRESS ? 0.2f : 0.0f;
    moveY += glfwGetKey(level->window, GLFW_KEY_LEFT ) == GLFW_PRESS ? -0.2f : 0.0f;
    angle += glfwGetKey(level->window, GLFW_KEY_SPACE ) == GLFW_PRESS ? -0.2f : 0.0f;

    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);
    //angle -= 0.4;
    //glm_scale(model, mirrorX);
    glm_rotate(model, glm_rad(-90), (float*)X_AXIS);
    glm_rotate(model, glm_rad(angle), (float*)Z_AXIS);
    vec3 mirror = {-1, 1, 1};
    glm_scale(model, mirror);
    glm_scale_uni(model, 32.0f * level->windowScale);

    vec3 transformation = {moveX, moveY, 0};
    glm_translate(model, transformation);
    //printf("AngleZ: %.2f\tMoveX: %.2f\tMoveY: %.2f\n", angle, moveX, moveY);

    glUniformMatrix4fv(level->projection, 1, GL_FALSE, *projection);
    glUniformMatrix4fv(level->view, 1, GL_FALSE, *view);
    glUniformMatrix4fv(level->model, 1, GL_FALSE, *model);

    glBindTexture( GL_TEXTURE_2D, level->tilemap.texture.bufferID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, level->terrainVertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, level->terrainVertexCount * 3); // start at 0; 6 because 6 points for 2 vertices
    //LOGGLERR(TAG);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, level->terrainUVBuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, level->terrainVertexCount * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);
    return 0;
}
