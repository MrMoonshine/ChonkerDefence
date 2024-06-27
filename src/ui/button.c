
#include <ui/button.h>

static const char* TAG = "Button";

static const size_t VERTEX_COUNT = 6;
static const size_t VERTEX_POINT_COUNT = VERTEX_COUNT*3*3;
static const size_t UV_POINT_COUNT = VERTEX_COUNT*3*2;

static const float WIDTH_SIDE_FACTOR = 0.1;
static const float WIDTH_CENTER_FACTOR = 0.8;

static float button_vertices(float* vertexbuffer, float* uvbuffer, float xorigin, float width, float height, int index){
    vertexbuffer[0] = xorigin;
    vertexbuffer[1] = 0.0f;

    vertexbuffer[3] = xorigin + width;
    vertexbuffer[4] = height;

    vertexbuffer[6] = xorigin;
    vertexbuffer[7] = height;

    vertexbuffer[9] = xorigin;
    vertexbuffer[10] = 0.0f;

    vertexbuffer[12] = xorigin + width;
    vertexbuffer[13] = 0.0f;

    vertexbuffer[15] = xorigin + width;
    vertexbuffer[16] = height;
    for(int a = 0; a < 18; a++){
        if(a % 3 == 2)
            vertexbuffer[a] = 0.0f;
    }

    // UVs - Suited to user minecraft widget tilemat
    float x1tex = 0.0f;
    float y1tex = 600.0f/1024.0f;
    float x2tex = 800.0f/1024.0f;
    float y2tex = 680.0f/1024.0f;

    switch(index){
        case 0:
            x2tex *= WIDTH_SIDE_FACTOR;
            break;
        case 1:
            x1tex += x2tex*WIDTH_SIDE_FACTOR;
            x2tex = x2tex*(1 - WIDTH_SIDE_FACTOR);
            break;
        case 2:
            x1tex = x2tex*(1 - WIDTH_SIDE_FACTOR);
            break;
        default:
            break;
    }

    uvbuffer[ 0] = x1tex;
    uvbuffer[ 1] = y1tex;
    uvbuffer[ 2] = x2tex;
    uvbuffer[ 3] = y2tex;
    uvbuffer[ 4] = x1tex;
    uvbuffer[ 5] = y2tex;
    uvbuffer[ 6] = x1tex;
    uvbuffer[ 7] = y1tex;
    uvbuffer[ 8] = x2tex;
    uvbuffer[ 9] = y1tex;
    uvbuffer[10] = x2tex;
    uvbuffer[11] = y2tex;

    return xorigin + width;
}

int ui_button_create(Button *button, UI *ui, float width, float height, const char* text){
    button->ui = ui;
    button->x = 0;
    button->y = 0;
    button->height = height;
    button->width = width;

    if(strstr(text, "\n") != NULL || strstr(text, "\r") != NULL)
        LOGW(TAG, "Button with multiline text might result in broken menu!");

    ui_text2d_create(&button->text, button->ui, text, height/2.0f);
    button->text.color[0] = 0.9f;
    button->text.color[1] = 0.9f;
    button->text.color[2] = 0.1f;
    // Init arrays
    float vertexBufferData[VERTEX_POINT_COUNT];
    for(unsigned int i = 0; i < VERTEX_POINT_COUNT; i++)
        vertexBufferData[i] = 0.0f;

    float uvBufferData[UV_POINT_COUNT];
    for(unsigned int i = 0; i < UV_POINT_COUNT; i++)
        uvBufferData[i] = 0.0f;

    float xorigin = 0.0f;
    size_t vertexpos = 0, uvpos = 0;
    int pindex = 0;
    xorigin = button_vertices(vertexBufferData + 0, uvBufferData + vertexpos, xorigin, width*WIDTH_SIDE_FACTOR, height, pindex++);
    vertexpos += VERTEX_POINT_COUNT/3;
    uvpos += UV_POINT_COUNT/3;
    xorigin = button_vertices(vertexBufferData + vertexpos, uvBufferData + uvpos, xorigin, width*WIDTH_CENTER_FACTOR, height, pindex++);
    vertexpos += VERTEX_POINT_COUNT/3;
    uvpos += UV_POINT_COUNT/3;
    xorigin = button_vertices(vertexBufferData + vertexpos, uvBufferData + uvpos, xorigin, width*WIDTH_SIDE_FACTOR, height, pindex++);

    glBindVertexArray(ui->vao);

    glGenBuffers(1, &button->vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, button->vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_POINT_COUNT * sizeof(float), vertexBufferData, GL_STATIC_DRAW);

    glGenBuffers(1, &button->uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, button->uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, UV_POINT_COUNT * sizeof(float), uvBufferData, GL_STATIC_DRAW);
//    printf("My Buffers are %d and %d\n", button->vertexbuffer, button->uvbuffer);

    return 0;
}

void ui_button_draw(Button *button){
    glBindTexture( GL_TEXTURE_2D, button->ui->widgets.bufferID);
    mat4 model;
    glm_mat4_identity(model);
    vec3 transformation = {button->x, button->y, 0};
    glm_translate(model, transformation);

    glUniformMatrix4fv(button->ui->model, 1, GL_FALSE, *model);

    ui_no_colorize(button->ui);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, button->vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT*3); // start at 0; 6 because 6 points for 2 vertices
    //LOGGLERR(TAG);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, button->uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT*3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);

    //button->x = button->ui->windowWidth/2 - button->width/2; // Center to window

    button->text.x = button->x + button->width/2 - button->text.width/2;
    button->text.y = button->y + button->height/2- button->text.height/2;
    ui_text2d_draw(&button->text);

    if(ui_button_mouse_click(button)){
        printf("Clicked Button!\n");
    }
}

void ui_button_destroy(Button *button){
    ui_text2d_destroy(&button->text);
    glDeleteBuffers(1, &button->vertexbuffer);
    glDeleteBuffers(1, &button->uvbuffer);
}

bool ui_button_mouse_click(Button* button){
    // No coloring of button text
    button->text.color[3] = 0.0f;

    double xpos, ypos;
    glfwGetCursorPos(button->ui->window, &xpos, &ypos);
    ypos = button->ui->windowHeight - ypos;
    xpos -= button->x;
    ypos -= button->y;

    //printf("Oida Pos is (%.2f|%.2f)\n", xpos, ypos);

    if(xpos < 0 || ypos < 0)
        return false;

    if(!(xpos <= button->width && ypos <= button->height)){
        return false;
    }

    button->text.color[3] = 1.0f;

    button->ui->anyButtonHover |= true;
    bool ret = GLFW_PRESS == glfwGetMouseButton(button->ui->window, GLFW_MOUSE_BUTTON_LEFT);
    return ret;
}
