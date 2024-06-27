#include <ui/levelselection.h>

static const char* TAG = "GUI: Levelselection:";

int levelselection_create(LevelSelection *selection, UI* ui){
    static const GLfloat g_vertex_buffer_data[] = {
        1.0f, 98.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        34.0f, 98.0f, 0.0f,
        34.0f, 98.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        34.0f, 1.0f, 0.0f,
    };

    static const GLfloat g_uv_buffer_data[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    //glGenVertexArrays(1, &ui->vao);
    glBindVertexArray(ui->vao);

    glGenBuffers(1, &selection->bgvbo);
    glBindBuffer(GL_ARRAY_BUFFER, selection->bgvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &selection->bguvvbo);
    glBindBuffer(GL_ARRAY_BUFFER, selection->bguvvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
    texture_create(&selection->texture, "../build/test2.png");

    ui_text2d_create(&selection->title, ui, "the quick brown fox jumps\r\nover the lazy dog!", 32.0f);
    selection->title.x = 64;
    selection->title.y = 128;
    return 0;
}

int levelselection_draw(LevelSelection *selection){
    glBindTexture( GL_TEXTURE_2D, selection->texture.bufferID);
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
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, selection->bguvvbo);
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

    ui_text2d_draw(&selection->title);
    return 0;
}

int levelselection_destroy(LevelSelection *selection){
    ui_text2d_destroy(&selection->title);
    texture_destroy(&selection->texture);
    glDeleteBuffers(1, &selection->bgvbo);
    glDeleteBuffers(1, &selection->bguvvbo);
    return 0;
}
