#include <decoration.h>

static const char* TAG = "Decoration";

int decoration_create(Decoration* decoration){
    fastObjMesh* mesh = fast_obj_read("../assets/models/maus.obj");
    decoration->vertexCount = mesh->face_count;
    printf("Texture count is %d\n", mesh->texture_count);
    printf("Vertex count is %lu\tPoscount is %u\n", decoration->vertexCount, mesh->position_count);

    for(uint8_t i = 0; i < mesh->texture_count; i++){
        //if(!mesh->textures->name)
            //continue;

        printf("Texture name is %s\tpath is %s\n", mesh->textures->name, mesh->textures->path);
    }
    for(uint8_t i = 0; i < mesh->material_count; i++){
        printf("Material name is %s\tTexture is %d\n", mesh->materials->name, mesh->materials->map_Kd);
    }
    for(uint8_t i = 0; i < mesh->object_count; i++){
        printf("Object name is %s\n", mesh->objects->name);
    }
    for(uint8_t i = 0; i < mesh->group_count; i++){
        printf("Group name is %s\n", mesh->groups->name);
    }
    for(size_t i = 0; i < decoration->vertexCount * 3; i++){
        printf("%.2f, ", mesh->positions[i]);
        if(i % 3 == 2)
            printf("\n");
        if(i % 9 == 2)
            printf("---VERTEX---\n");
    }

    if(0 > texture_create(&decoration->texture, "../assets/models/maus.albedo.png"))
        texture_create(&decoration->texture, TEXTURE_UNKNOWN);

    glGenBuffers(1, &decoration->vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, decoration->vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, decoration->vertexCount * 3, mesh->positions, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &decoration->uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, decoration->uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh->texcoord_count, mesh->texcoords, GL_STATIC_DRAW);

    fast_obj_destroy(mesh);
    return 0;
}

void decoration_draw(Decoration* decoration){
glBindTexture( GL_TEXTURE_2D, decoration->texture.bufferID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, decoration->vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, decoration->vertexCount * 3); // start at 0; 6 because 6 points for 2 vertices
    //LOGGLERR(TAG);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, decoration->uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, decoration->vertexCount * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);
}

void decoration_destroy(Decoration* decoration){
    glDeleteBuffers(1, &decoration->uvbuffer);
    glDeleteBuffers(1, &decoration->vertexbuffer);
    texture_destroy(&decoration->texture);
}
