#include <vbo.h>

static const uint8_t FLAGS_VERTEX_BIT = 0;
static const uint8_t FLAGS_UV_BIT = 1;
static const uint8_t FLAGS_NORMALS_BIT = 2;

static void vbo_init_buffer(GLuint* id, float* buffer, size_t size){
    /*printf(" --- BUFFER ---\n");
    for(size_t i = 0; i < size/sizeof(float); i++){
        printf("%.2f, ", buffer[i]);
        if(i % 3 == 2)
            printf("\n");
    }*/
    glGenBuffers(1, id);
    glBindBuffer(GL_ARRAY_BUFFER, *id);
    glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
}

int vbo_create(VBO *vbo, float* vertices, float* uv, float* normals, size_t faceCount){
    //vbo->initflags = 0;
    vbo->vertices = 0;
    vbo->uv = 0;
    vbo->normals = 0;
    vbo->faceCount = faceCount;

    vbo_init_buffer(&vbo->vertices, vertices, vertices ? faceCount * VERTEX_SIZE : 0);
    vbo_init_buffer(&vbo->uv, uv, uv ? faceCount * UV_SIZE : 0);
    vbo_init_buffer(&vbo->normals, normals, normals ? faceCount * NORMALS_SIZE : 0);
    return 0;
}

void vbo_draw(VBO* vbo, GLuint textureID){
    glBindTexture( GL_TEXTURE_2D, textureID);
    // VERTICES
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->vertices);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, vbo->faceCount * 3);
    // UV
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->uv);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, vbo->faceCount * 3);
    // NORMALS
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->normals);
    glVertexAttribPointer(
        2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
        3,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, vbo->faceCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);
}

void vbo_destroy(VBO* vbo){
    glDeleteBuffers(1, &vbo->vertices);
    glDeleteBuffers(1, &vbo->uv);
    glDeleteBuffers(1, &vbo->vertices);
}
