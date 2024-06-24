#include <ui/ui.h>

static mat4 projection;

static mat4 view;
static vec3 eye = {4, 3, 3};
static vec3 center = {0, 0, 0};
static vec3 up = {0, 1, 0};
static mat4 model;

static const char* TAG = "UI";

int ui_create(UI *ui){
    glGenVertexArrays(1, &ui->vao);
    glBindVertexArray(ui->vao);
    ui->shader = glshader_load("../shaders/guiver.glsl", "../shaders/guifra.glsl");

    glm_ortho(0.0f, 100.0f, 0.0f, 100.0f, -1.0f, 1.0f, projection);
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);
    vec3 transformation = {0, 1, 0};
    glm_translate(model, transformation);

    ui->projection = glGetUniformLocation(ui->shader, "projection");
    ui->view = glGetUniformLocation(ui->shader, "view");
    ui->model = glGetUniformLocation(ui->shader, "model");
    // Font
    if(0 != texture_create(&ui->font, "../build/ascii.png")){
        LOGE(TAG, "Error while loading fonts. No text shown in GUI!");
        return -1;
    }
    return 0;
}

int ui_destroy(UI *ui){
    texture_destroy(&ui->font);
    glDeleteProgram(ui->shader);
	glDeleteVertexArrays(1, &ui->vao);
    return 0;
}

void ui_resize(UI *ui, int width, int height){
    //float aspectX = (float)APP_WIDTH / (float)width;
    float aspectY = (float)APP_HEIGHT / (float)height;
    //printf("Aspects are (%.2f|%.2f)\n", aspectX, aspectY);

    glBindVertexArray(ui->vao);
    glm_ortho(0.0f, 100.0f * aspectY, 0.0f, 100.0f * aspectY, -1.0f, 1.0f, projection);
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);
    vec3 transformation = {0, 1, 0};
    glm_translate(model, transformation);
}

void ui_enable_vao(UI *ui){
    glUseProgram(ui->shader);
    glBindVertexArray(ui->vao);

    glUniformMatrix4fv(ui->projection, 1, GL_FALSE, *projection);
    glUniformMatrix4fv(ui->view, 1, GL_FALSE, *view);
    glUniformMatrix4fv(ui->model, 1, GL_FALSE, *model);
}


