#include <ui/levelselection.h>

static const char* TAG = "GUI: Levelselection:";

    static mat4 projection;
    static float angle = 45.0f;

    static mat4 view;
    static vec3 eye = {4, 3, 3};
    static vec3 center = {0, 0, 0};
    static vec3 up = {0, 1, 0};
    //dumpMat4(view, "View");

    static mat4 model;

int levelselection_create(LevelSelection *selection){
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
    };

    glGenVertexArrays(1, &selection->vao);
    glBindVertexArray(selection->vao);

    glGenBuffers(1, &selection->bgvbo);
    glBindBuffer(GL_ARRAY_BUFFER, selection->bgvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    //selection->shader = glshader_load("../shaders/guiver.glsl", "../shaders/guifra.glsl");
    selection->shader = glshader_load("../shaders/guiver.glsl", "../shaders/guifra.glsl");

    selection->sp = glGetUniformLocation(selection->shader, "projection");
    selection->sv = glGetUniformLocation(selection->shader, "view");
    selection->sm = glGetUniformLocation(selection->shader, "model");

    glm_make_rad(&angle);
    //glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, projection);
    //glm_ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, -1.0f, projection);
    glm_ortho(-1.0f, 1.0f, -10.0f, 10.0f, -1.0f, 1.0f, projection);
    glm_lookat(eye, center, up, view);
    glm_mat4_identity(model);
    vec3 transformation = {0, 1, 0};
    glm_translate(model, transformation);

    return 0;
}

int levelselection_draw(LevelSelection *selection){
    glUseProgram(selection->shader);
    glBindVertexArray(selection->vao);

    glUniformMatrix4fv(selection->sp, 1, GL_FALSE, *projection);
    glUniformMatrix4fv(selection->sv, 1, GL_FALSE, *view);
    glUniformMatrix4fv(selection->sm, 1, GL_FALSE, *model);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, selection->bgvbo);
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
    glDisableVertexAttribArray(0);
    return 0;
}

int levelselection_destroy(LevelSelection *selection){
    glDeleteBuffers(1, &selection->bgvbo);
	glDeleteProgram(selection->shader);
	glDeleteVertexArrays(1, &selection->vao);
    return 0;
}
