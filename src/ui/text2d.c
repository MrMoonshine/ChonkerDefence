#include <ui/text2d.h>

static const char* TAG = "Text2D";

int ui_text2d_create(Text2D* text, UI *ui, const char* message, float size){
    text->size = size;
    text->x = 0.0f;
    text->y = 0.0f;
    text->ui = ui;

    float *vertexBufferData, *uvBufferData;
    size_t vertexBufferSize, uvBufferSize;

    const GLfloat g_uv_buffer_data[] = {
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };

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

    memset(vertexBufferData, 0.0f, vertexBufferSize/sizeof(float));

    const uint8_t oordinateCount = 18;
    for(size_t i = 0; i < text->length; i++){
        float xorigin = text->size*i;
        // Vertex
            // Vertex #1
                // Vertex #1 P1
        vertexBufferData[oordinateCount*i +  0] = xorigin;
        vertexBufferData[oordinateCount*i +  1] = 0.0f;
                // Vertex #1 P2
        vertexBufferData[oordinateCount*i +  3] = xorigin + text->size;
        vertexBufferData[oordinateCount*i +  4] = text->size;
                // Vertex #1 P3
        vertexBufferData[oordinateCount*i +  6] = xorigin;
        vertexBufferData[oordinateCount*i +  7] = text->size;
        // Vertex #2
                // Vertex #2 P1
        vertexBufferData[oordinateCount*i +  9] = xorigin;
        vertexBufferData[oordinateCount*i + 10] = 0.0f;
                // Vertex #2 P2
        vertexBufferData[oordinateCount*i + 12] = xorigin + text->size;
        vertexBufferData[oordinateCount*i + 13] = 0.0f;
                // Vertex #2 P3
        vertexBufferData[oordinateCount*i + 15] = xorigin + text->size;
        vertexBufferData[oordinateCount*i + 16] = text->size;
        // UV
        memcpy(uvBufferData + 12*i, g_uv_buffer_data, sizeof(g_uv_buffer_data));
        //printf("%.2f, ", *(uvBufferData+12*i+1));
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
