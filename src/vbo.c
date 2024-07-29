#include <vbo.h>

void vbo_init(VBO *vbo, size_t faceCount){
    vbo->vertices = 0;
    vbo->uv = 0;
    vbo->normals = 0;
    vbo->faceCount = faceCount;
}

static void vbo_init_buffer(GLuint* id, float* buffer, size_t size){
    glGenBuffers(1, id);
    glBindBuffer(GL_ARRAY_BUFFER, *id);
    glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
}

int vbo_create(VBO *vbo, float* vertices, float* uv, float* normals, size_t faceCount){
    vbo_init(vbo, faceCount);

    vbo_create_vertices(vbo, vertices);
    vbo_create_uv(vbo, uv);
    vbo_create_normals(vbo, normals);
    return 0;
}

int vbo_create_vertices(VBO *vbo, float* buffer){
    vbo_init_buffer(&vbo->vertices, buffer, buffer ? vbo->faceCount * VERTEX_SIZE : 0);
    return 0;
}

int vbo_create_uv(VBO *vbo, float* buffer){
    vbo_init_buffer(&vbo->uv, buffer, buffer ? vbo->faceCount * UV_SIZE : 0);
    return 0;
}

int vbo_create_normals(VBO *vbo, float* buffer){
    vbo_init_buffer(&vbo->normals, buffer, buffer ? vbo->faceCount * NORMALS_SIZE : 0);
    return 0;
}

void vbo_draw(VBO* vbo, GLuint textureID){
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);       // Face culling has to be inverted for this voxel mode

    glBindTexture( GL_TEXTURE_2D, textureID);
    // VERTICES
    glEnableVertexAttribArray(VBO_INDEX_VERTEX);
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
    glEnableVertexAttribArray(VBO_INDEX_UV);
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
    glEnableVertexAttribArray(VBO_INDEX_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->normals);
    glVertexAttribPointer(
        2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
        3,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, vbo->faceCount * 3);

    glDisableVertexAttribArray(VBO_INDEX_VERTEX);
    glDisableVertexAttribArray(VBO_INDEX_UV);
    glDisableVertexAttribArray(VBO_INDEX_NORMAL);

    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);

    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);
}

void vbo_destroy(VBO* vbo){
    glDeleteBuffers(1, &vbo->vertices);
    glDeleteBuffers(1, &vbo->uv);
    glDeleteBuffers(1, &vbo->vertices);
}
