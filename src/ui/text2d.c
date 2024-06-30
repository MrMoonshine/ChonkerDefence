#include <ui/text2d.h>

static const char* TAG = "Text2D";

static void text2d_create_letter(Text2D *text,char letter, float xorigin, float yorigin, float* vertexbuffer, float* uvbuffer){
    //float xorigin = text->size*index;
    // Reset values
    for(int j = 0; j < 18; j++){
        *(vertexbuffer+j) = 0.0f;
    }
    // Vertex
            // Vertex #1
                // Vertex #1 P1
    *(vertexbuffer + 0) = xorigin;
    *(vertexbuffer + 1) = yorigin;
                // Vertex #1 P2
    *(vertexbuffer + 3) = xorigin + text->size;
    *(vertexbuffer + 4) = yorigin + text->size;
                // Vertex #1 P3
    *(vertexbuffer + 6) = xorigin;
    *(vertexbuffer + 7) = yorigin + text->size;
        // Vertex #2
                // Vertex #2 P1
    *(vertexbuffer + 9) = xorigin;
    *(vertexbuffer + 10) = yorigin;
                // Vertex #2 P2
    *(vertexbuffer + 12) = xorigin + text->size;
    *(vertexbuffer + 13) = yorigin;
                // Vertex #2 P3
    *(vertexbuffer + 15) = xorigin + text->size;
    *(vertexbuffer + 16) = yorigin + text->size;

    // UV
    float column = (int)letter % (int)UI_TEXT2D_FONT_TILES_X;
    float row = 16.0f - ((int)letter - column) / UI_TEXT2D_FONT_TILES_X;

    //printf("Character '%c' has coordinates (%.2f|%.2f)\n", letter, column, row);

    *(uvbuffer + 0) = (column)/UI_TEXT2D_FONT_TILES_X;
    *(uvbuffer + 1) = (row - 1)/UI_TEXT2D_FONT_TILES_Y;
    *(uvbuffer + 2) = (column + 1)/UI_TEXT2D_FONT_TILES_X;
    *(uvbuffer + 3) = (row + 0)/UI_TEXT2D_FONT_TILES_X;
    *(uvbuffer + 4) = (column)/UI_TEXT2D_FONT_TILES_Y;
    *(uvbuffer + 5) = (row + 0)/UI_TEXT2D_FONT_TILES_Y;

    *(uvbuffer + 6) = (column)/UI_TEXT2D_FONT_TILES_X;
    *(uvbuffer + 7) = (row - 1)/UI_TEXT2D_FONT_TILES_Y;
    *(uvbuffer + 8) = (column + 1)/UI_TEXT2D_FONT_TILES_X;
    *(uvbuffer + 9) = (row -1)/UI_TEXT2D_FONT_TILES_X;
    *(uvbuffer + 10) = (column + 1)/UI_TEXT2D_FONT_TILES_Y;
    *(uvbuffer + 11) = (row + 0)/UI_TEXT2D_FONT_TILES_Y;
}

int ui_text2d_create(Text2D* text, UI *ui, const char* message, float size){
    text->size = size;
    text->x = 0.0f;
    text->y = 0.0f;
    text->ui = ui;
    text->width = 0.0f;
    text->height = size;
    // Set no color blending as initial
    for(uint8_t i = 0; i < 4; i++)
        text->color[i] = 0.0f;

    float *vertexBufferData, *uvBufferData;
    size_t vertexBufferSize, uvBufferSize;

    text->length = strlen(message);
    if(text->length < 1){
        LOGW(TAG, "Aborting creation of Vertices fore <empty string>");
        return -1;
    }

    size_t pointcount = 3 * 2 * text->length; // number of coordinates
    vertexBufferSize = 3 * pointcount * sizeof(float);
    uvBufferSize = 2 * pointcount * sizeof(float);

    vertexBufferData = malloc(vertexBufferSize);
    uvBufferData = malloc(uvBufferSize);
    if(vertexBufferData == NULL || uvBufferData == NULL){
        LOGE(TAG, "Failed malloc for buffer data");
        return -2;
    }

    float xorigin = 0.0f, yorigin = 0.0f;
    for(size_t i = 0; i < text->length; i++){
        switch(message[i]){
            case '\r':
                xorigin = 0.0f;
                break;
            case '\n':
                yorigin -= text->size;
                text->height += text->size;
                break;
            default:
                text2d_create_letter(text, message[i], xorigin, yorigin, vertexBufferData + 18*i, uvBufferData + 12*i);
                xorigin += text->size;
                if(xorigin >= text->width)
                    text->width = xorigin;
                break;
        }
    }

    glBindVertexArray(ui->vao);

    glGenBuffers(1, &text->vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, text->vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexBufferData, GL_STATIC_DRAW);

    glGenBuffers(1, &text->uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, text->uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvBufferSize, uvBufferData, GL_STATIC_DRAW);

    free(vertexBufferData);
    free(uvBufferData);
    return 0;
}

void ui_text2d_draw(Text2D* text){
    glBindTexture( GL_TEXTURE_2D, text->ui->font.bufferID);
    mat4 model;
    glm_mat4_identity(model);

    vec3 transformation = {text->x, text->y, 0};
    glm_translate(model, transformation);

    glUniformMatrix4fv(text->ui->model, 1, GL_FALSE, *model);
    glUniform4fv(text->ui->colorize, 1, text->color);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, text->vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, text->length * 6); // start at 0; 6 because 6 points for 2 vertices
    //LOGGLERR(TAG);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, text->uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, text->length * 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);
}

void ui_text2d_destroy(Text2D* text){
    glDeleteBuffers(1, &text->vertexbuffer);
    glDeleteBuffers(1, &text->uvbuffer);
}

void ui_text2d_set_color(Text2D* text, const vec4 color){
    for(uint8_t i = 0; i < 4; i++){
        text->color[i] = color[i];
    }
}
