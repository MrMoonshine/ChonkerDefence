#include <ui/image2d.h>

int ui_image2d_create(Image2D* image, UI* ui, Texture* texture){
    image->createdViaPNG = false;
    image->ui = ui;
    image->x = 0;
    image->y = 0;
    glBindVertexArray(ui->vao);         // UI VAO

    uint32_t width = texture->width;
    uint32_t height = texture->height;

    const GLfloat vertex_buffer_data[] = {
        (width) - 2*0.0f,    (height) - 2*0.0f,   0.0f,
        0.0f,                 (height) - 2*0.0f,   0.0f,
        0.0f,                 0.0f,                 0.0f,
        (width) - 2*0.0f,    0.0f,                 0.0f,
        (width) - 2*0.0f,    (height) - 2*0.0f,   0.0f,
        0.0f,                 0.0f,                 0.0f,
    };

    const GLfloat uv_buffer_data[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
    };

    glGenBuffers(1, &image->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, image->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &image->uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, image->uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

    return 0;
}

int ui_image2d_create_from_png(Image2D* image, UI* ui, const char* pngfile){
    Texture texture;
    texture_create(&texture, pngfile);
    image->texture = (Texture*)malloc(sizeof(Texture));
    if(image->texture == NULL){
        return -1;
    }
    memcpy(image->texture, &texture, sizeof(Texture));
    ui_image2d_create(image, ui, image->texture);
    image->createdViaPNG = true;
    return 0;
}

void ui_image2d_draw(Image2D* image){
    glBindTexture( GL_TEXTURE_2D, image->texture->bufferID);

    float scale = (float)image->ui->windowHeight / (float)image->texture->height;
    if(scale*image->texture->width < image->ui->windowWidth){
        scale = (float)image->ui->windowWidth / (float)image->texture->width;
    }

    float imgwidth = scale*image->texture->width;
    float imgheight = scale*image->texture->height;

    image->x = image->ui->windowWidth/2.0f;
    image->x -= imgwidth/2.0f;
    image->y = image->ui->windowHeight/2.0f;
    image->y -= imgheight/2.0f;
    //printf("imgw = %.2f | x = %.2f\n", imgwidth, image->x);

    mat4 model;
    glm_mat4_identity(model);
    vec3 transformation = {image->x, image->y, 0};
    glm_scale_uni(model, scale);
    glm_translate(model, transformation);

    glUniformMatrix4fv(image->ui->model, 1, GL_FALSE, *model);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, image->vertexBuffer);
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

    glBindBuffer(GL_ARRAY_BUFFER, image->uvBuffer);
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

int ui_image2d_destroy(Image2D* image){
    if(image->createdViaPNG){
        texture_destroy(image->texture);
        free(image->texture);
        image->texture = NULL;
    }

    glDeleteBuffers(1, &image->vertexBuffer);
    glDeleteBuffers(1, &image->uvBuffer);

    return 0;
}
