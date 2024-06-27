#include <ui/ui.h>

static mat4 projection;

static mat4 view;
static vec3 eye = {4, 3, 3};
static vec3 center = {0, 0, 0};
static vec3 up = {0, 1, 0};
static mat4 model;

static vec4 UI_NO_COLORIZE = {0.0f, 0.0f, 0.0f, 0.0f};

static const char* TAG = "UI";

int ui_create(UI *ui, GLFWwindow* window){
    ui->windowWidth = APP_WIDTH;
    ui->windowHeight = APP_HEIGHT;
    ui->window = window;
    ui->anyButtonHover = false;

    glGenVertexArrays(1, &ui->vao);
    glBindVertexArray(ui->vao);
    ui->shader = glshader_load("../shaders/guiver.glsl", "../shaders/guifra.glsl");

    glm_ortho(0.0f, APP_WIDTH, 0.0f, APP_HEIGHT, -1.0f, 1.0f, projection);
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);
    vec3 transformation = {0, 1, 0};
    glm_translate(model, transformation);

    ui->projection = glGetUniformLocation(ui->shader, "projection");
    ui->view = glGetUniformLocation(ui->shader, "view");
    ui->model = glGetUniformLocation(ui->shader, "model");
    ui->colorize = glGetUniformLocation(ui->shader, "colorize");

    //ui->intensity = glGetUniformLocation(ui->shader, "intensity");
    // Font
    if(0 != texture_create(&ui->font, "../build/ascii.png")){
        LOGE(TAG, "Error while loading fonts. No text shown in GUI!");
        return -1;
    }
    if(0 != texture_create(&ui->widgets, "../build/widgets1.png")){
        LOGE(TAG, "Error while GUI Texture!");
        return -1;
    }
    return 0;
}

int ui_destroy(UI *ui){
    texture_destroy(&ui->widgets);
    texture_destroy(&ui->font);
    glDeleteProgram(ui->shader);
	glDeleteVertexArrays(1, &ui->vao);
    return 0;
}

void ui_resize(UI *ui, int width, int height){
    ui->windowWidth = width;
    ui->windowHeight = height;
    float scaleY = (float)height/(float)APP_HEIGHT;

    glBindVertexArray(ui->vao);
    glm_ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f, projection);
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);
    vec3 transformation = {0, 1, 0};
    glm_translate(model, transformation);
    glm_scale_uni(model, scaleY);
}

void ui_no_colorize(UI *ui){
    glUniform4fv(ui->colorize, 1, UI_NO_COLORIZE);
}


void ui_enable_vao(UI *ui){
    ui->anyButtonHover = false;

    glUseProgram(ui->shader);
    glBindVertexArray(ui->vao);

    glUniformMatrix4fv(ui->projection, 1, GL_FALSE, *projection);
    glUniformMatrix4fv(ui->view, 1, GL_FALSE, *view);
    glUniformMatrix4fv(ui->model, 1, GL_FALSE, *model);
    ui_no_colorize(ui);
}


