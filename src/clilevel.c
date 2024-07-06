#include <clilevel.h>

static const char* TAG = "Level (Client-Side)";

/*static const vec3 X_AXIS = {1, 0, 0};
static const vec3 Y_AXIS = {0, 1, 0};
static const vec3 Z_AXIS = {0, 0, 1};

static const float ISOMETRIC_ROTATION_VERTICAL = 45.0f;
static const float ISOMETRIC_ROTATION_HORIZONTAL = 35.264f;*/

static const float CLILEVEL_ORTHO_DEPTH = 1000.0f;
static vec3 eye = {sqrt(1 / 3.0), sqrt(1 / 3.0), sqrt(1 / 3.0)};
static vec3 center = {0, 0, 0};
static vec3 up = {0, 1, 0};

static mat4 model;
static mat4 view;
static mat4 projection;

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

static const uint8_t clilevel_get_node_at(ClientLevel *level, uint8_t x, uint8_t y, uint8_t *buffer, size_t len){
    size_t totalPos = (y * level->width) + x;
    size_t pos = totalPos / 2;

    if(pos >= len)
        return 0xFF;

    return totalPos % 2 ? (buffer[pos] & 0x0f) : ((buffer[pos] & 0xf0) >> 4);
}

static int clilevel_build(ClientLevel *level, uint8_t *buffer, size_t len){
    level->windowScale = 1;
    level->modelScale = 1;
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

        }
        printf("\n");
    }

    tilemap_create(&level->tilemap, LEVEL_TILEMAP_TERRAIN_WIDTH, LEVEL_TILEMAP_TERRAIN_HEIGHT, LEVEL_TILEMAP_TERRAIN_FILE);

    glGenVertexArrays(1, &level->vao);
    glBindVertexArray(level->vao);
    level->shader = glshader_load("../shaders/gamever.glsl", "../shaders/gamefra.glsl");

    glm_ortho(0.0f, (float)APP_WIDTH, 0.0f, (float)APP_HEIGHT, -1000.0f, 1000.0f, projection);

    // glm_vec3_rotate(up, glm_rad(ISOMETRIC_ROTATION_HORIZONTAL), (float*)X_AXIS);
    // glm_vec3_rotate(up, glm_rad(ISOMETRIC_ROTATION_VERTICAL), (float*)Z_AXIS);
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);

    level->projection = glGetUniformLocation(level->shader, "projection");
    level->view = glGetUniformLocation(level->shader, "view");
    level->model = glGetUniformLocation(level->shader, "model");

    glGenBuffers(1, &level->terrainVertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, level->terrainVertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &level->terrainUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, level->terrainUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
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

int clilevel_draw(ClientLevel *level){
    //float angle = glm_rad(45.0f);

    //glm_ortho(0.0f, 10.0f, 0.0f, 10.0f, -100.0f, 100.0f, projection);
    //glm_ortho(0.0f, APP_WIDTH, 0.0f, APP_HEIGHT, -1000.0f, 1000.0f, projection);

    //eye[0] += 0.05;
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);
    glm_scale_uni(model, 150.0f * level->windowScale);

    vec3 transformation = {3, 3, -1};
    glm_translate(model, transformation);
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
    glDrawArrays(GL_TRIANGLES, 0, 6 * 6); // start at 0; 6 because 6 points for 2 vertices
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
    glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);
    return 0;
}
