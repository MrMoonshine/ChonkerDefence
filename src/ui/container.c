#include <ui/container.h>

int ui_container_create(UIContainer* container, UI* ui, int *width, int *height, int margin, float zindex, Texture* texture){
    container->createdViaPNG = false;
    glBindVertexArray(ui->vao);         // UI VAO

    const GLfloat vertex_buffer_data[] = {
        (*width) - 2*margin,    (*height) - 2*margin,   zindex,
        margin,                 (*height) - 2*margin,   zindex,
        margin,                 margin,                 zindex,
        (*width) - 2*margin,    margin,                 zindex,
        (*width) - 2*margin,    (*height) - 2*margin,   zindex,
        margin,                 margin,                 zindex,
    };

    const GLfloat uv_buffer_data[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
    };

    glGenBuffers(1, &container->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, container->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &container->uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, container->uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

    return 0;
}

int ui_container_create_from_png(UIContainer* container, UI* ui, int *width, int *height, int margin, float zindex, const char* pngfile){
    Texture texture;
    texture_create(&texture, pngfile);
    container->texture = (Texture*)malloc(sizeof(Texture));
    if(container->texture == NULL){
        return -1;
    }
    memcpy(container->texture, &texture, sizeof(Texture));
    ui_container_create(container, ui, width, height, margin, zindex, container->texture);
    container->createdViaPNG = true;
    return 0;
}

void ui_container_draw(UIContainer* container){
    glBindTexture( GL_TEXTURE_2D, container->texture->bufferID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, container->vertexBuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
    //LOGGLERR(TAG);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, container->uvBuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 2*3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // Unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);
}

int ui_container_destroy(UIContainer* container){
    if(container->createdViaPNG){
        texture_destroy(container->texture);
        free(container->texture);
        container->texture = NULL;
    }

    glDeleteBuffers(1, &container->vertexBuffer);
    glDeleteBuffers(1, &container->uvBuffer);

    return 0;
}
