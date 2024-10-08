#include <clilevel.h>

static const char* TAG = "Level (Client-Side)";

static const vec3 X_AXIS = {1, 0, 0};
static const vec3 Y_AXIS = {0, 1, 0};
static const vec3 Z_AXIS = {0, 0, 1};

#define ISOMETRIC_ROTATION_VERTICAL glm_rad(180 + 45.0f)
#define ISOMETRIC_ROTATION_HORIZONTAL glm_rad(35.264f)

static const float CLILEVEL_ORTHO_DEPTH = 2000.0f;
//static vec3 eye = {sqrt(1 / 3.0), sqrt(1 / 3.0), sqrt(1 / 3.0)};
static vec3 eye = {4, 3, 3};
static vec3 center = {0, 0, 0};
static vec3 up = {0, 1, 0};

static mat4 model;
static mat4 view;
static mat4 projection;

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
    level->windowScale = 1;
    level->modelScale = 10.0f;
    level->position[0] = 14.8f;
    level->position[1] = 5.2f;

    client_flush(client);
    uint8_t cmd[] = {PROTO_SCOPE_LEVELS, PROTO_CMD_SHOW, levelID};
    send(client->socket, cmd, sizeof(cmd), 0);

    uint8_t status = 0;
    recv(client->socket, &status, 1, 0);
    if(status != CD_NET_CODE_OK){
        fprintf(stderr,"\e[0;31m[ERROR] %s: Error in Reply: 0x%.2x\e[0m\n", TAG, status);
        return status;
    }

    recv(client->socket, len, sizeof(size_t), 0);
    printf("[INFO] %s: Level size is %luB\n", TAG, *len);

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

    //printf("[INFO] %s: Size is %lu\n", TAG, *len);
    size_t pos = 0;
    memcpy(level->name, buffer + pos, LEVEL_NAME_LENGTH);
    pos += LEVEL_NAME_LENGTH;
    memcpy(level->style, buffer + pos, LEVEL_STYLE_LENGTH);
    pos += LEVEL_STYLE_LENGTH;
    level->width = buffer[pos++];
    level->height = buffer[pos++];
    pos += LEVEL_RESERVED_LENGTH;

    int windowWidth = APP_WIDTH, windowHeigth = APP_HEIGHT;
    glfwGetWindowSize(level->window, &windowWidth, &windowHeigth);
    clilevel_resize(level, windowWidth, windowHeigth);
    printf("Window dimensions are (%d|%d)\n", windowWidth, windowHeigth);

    glGenVertexArrays(1, &level->vao);
    glBindVertexArray(level->vao);

    //glm_ortho(0.0f, (float)APP_WIDTH, 0.0f, (float)APP_HEIGHT, -1000.0f, 1000.0f, projection);

    eye[0] = 16.0f;
    eye[1] = 0.0f;
    eye[2] = 0.0f;
    glm_vec3_rotate(eye, ISOMETRIC_ROTATION_VERTICAL, (float*)Y_AXIS);
    printf("Eye is (%.2f|%.2f|%.2f)\n", eye[0],eye[1],eye[2]);
    vec3 isorotation;
    glm_vec3_copy(eye, isorotation);
    isorotation[2] *= (-1);
    //printf("EyeNormal is (%.2f|%.2f|%.2f)\n", eyenormal[0],eyenormal[1],eyenormal[2]);
    glm_vec3_rotate(eye, ISOMETRIC_ROTATION_HORIZONTAL, (float*)isorotation);

    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);

    level->projection = glGetUniformLocation(level->shader, "projection");
    level->view = glGetUniformLocation(level->shader, "view");
    level->model = glGetUniformLocation(level->shader, "model");
    level->cameraPosition = glGetUniformLocation(level->shader, "viewPos");
    level->normalMatrix = glGetUniformLocation(level->shader, "normalMatrix");

    //printf("[INFO] %s: Buffer len is %lu\n", TAG, *len);
    terrain_create(&level->terrain, buffer, *len);
    free(buffer);
    return 0;
}

void clilevel_destroy(ClientLevel *level){
    terrain_destroy(&level->terrain);
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
    level->modelScale = 2*APP_WIDTH + sqrt(3);
    level->modelScale /= 2*cos(glm_rad(30));
    level->modelScale *= 1.3;   // Correct it a bit
    level->modelScale /= level->width;
    printf("Model scale is %.2f => Wpx = %.2f\n", level->modelScale, level->modelScale * level->width);

    level->position[0] = -15.0f;
    level->position[1] = 4.5f;
}

void clilevel_enable_vao(ClientLevel* level){
    glUseProgram(level->shader);
    glBindVertexArray(level->vao);

    glUniformMatrix4fv(level->projection, 1, GL_FALSE, *projection);
    glUniformMatrix4fv(level->view, 1, GL_FALSE, *view);
    //glUniformMatrix4fv(level->model, 1, GL_FALSE, *model);
}

static float angle = 0.0f;
//static float eyemod = 1.0f;
int clilevel_draw(ClientLevel *level){
    level->position[0] += glfwGetKey(level->window, GLFW_KEY_DOWN ) == GLFW_PRESS ? 0.2f : 0.0f;
    level->position[0] += glfwGetKey(level->window, GLFW_KEY_UP ) == GLFW_PRESS ? -0.2f : 0.0f;
    level->position[1] += glfwGetKey(level->window, GLFW_KEY_RIGHT ) == GLFW_PRESS ? 0.2f : 0.0f;
    level->position[1] += glfwGetKey(level->window, GLFW_KEY_LEFT ) == GLFW_PRESS ? -0.2f : 0.0f;
    angle += glfwGetKey(level->window, GLFW_KEY_SPACE ) == GLFW_PRESS ? -1.0f : 0.0f;

    //eyemod += glfwGetKey(level->window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS ? 0.1f : 0.0f;
    //eye[1] = eyemod;
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);
    //angle -= 0.4;
    //glm_scale(model, mirrorX);
    //glm_rotate(model, glm_rad(-90), (float*)X_AXIS);
    glm_rotate(model, glm_rad(angle), (float*)Y_AXIS);
    vec3 mirror = {-1, 1, 1};
    glm_scale(model, mirror);
    glm_scale_uni(model, level->modelScale * level->windowScale);

    vec3 transformation = {level->position[0], 0, level->position[1]};
    glm_translate(model, transformation);
    //printf("AngleZ: %.2f\tMoveX: %.2f\tMoveY: %.2f\n", angle, level->position[0], level->position[1]);
    // Normals
    mat3 normalMatrix;
    common_mat4_to_mat3(model, normalMatrix);
    glm_mat3_inv(normalMatrix, normalMatrix);
    glm_mat3_transpose(normalMatrix);

    glUniformMatrix4fv(level->projection, 1, GL_FALSE, *projection);
    glUniformMatrix4fv(level->view, 1, GL_FALSE, *view);
    glUniformMatrix4fv(level->model, 1, GL_FALSE, *model);
    glUniform4fv(level->cameraPosition, 1, eye);
    glUniformMatrix3fv(level->normalMatrix, 1, GL_FALSE, *normalMatrix);

    terrain_draw(&level->terrain, level->model, level->normalMatrix, model);
    return 0;
}
