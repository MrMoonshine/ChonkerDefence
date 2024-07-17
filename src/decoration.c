#include <decoration.h>
//#include "../lib/tinyobjloader-c/tinyobj_loader_c.h"

static const char* TAG = "Decoration";
//static const char* FILENAME = "/home/david/Dokumente/Minecraft/Mods/Railroading/2016/deploy/oebb2016.obj";
//static const char* FILENAME = "../assets/models/maus.obj";
static const char* FILENAME = "../assets/models/cube.obj";

static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	// Two UV coordinatesfor each vertex. They were created with Blender.
static const GLfloat g_uv_buffer_data[] = {
		0.000059f, 1.0f-0.000004f,
		0.000103f, 1.0f-0.336048f,
		0.335973f, 1.0f-0.335903f,
		1.000023f, 1.0f-0.000013f,
		0.667979f, 1.0f-0.335851f,
		0.999958f, 1.0f-0.336064f,
		0.667979f, 1.0f-0.335851f,
		0.336024f, 1.0f-0.671877f,
		0.667969f, 1.0f-0.671889f,
		1.000023f, 1.0f-0.000013f,
		0.668104f, 1.0f-0.000013f,
		0.667979f, 1.0f-0.335851f,
		0.000059f, 1.0f-0.000004f,
		0.335973f, 1.0f-0.335903f,
		0.336098f, 1.0f-0.000071f,
		0.667979f, 1.0f-0.335851f,
		0.335973f, 1.0f-0.335903f,
		0.336024f, 1.0f-0.671877f,
		1.000004f, 1.0f-0.671847f,
		0.999958f, 1.0f-0.336064f,
		0.667979f, 1.0f-0.335851f,
		0.668104f, 1.0f-0.000013f,
		0.335973f, 1.0f-0.335903f,
		0.667979f, 1.0f-0.335851f,
		0.335973f, 1.0f-0.335903f,
		0.668104f, 1.0f-0.000013f,
		0.336098f, 1.0f-0.000071f,
		0.000103f, 1.0f-0.336048f,
		0.000004f, 1.0f-0.671870f,
		0.336024f, 1.0f-0.671877f,
		0.000103f, 1.0f-0.336048f,
		0.336024f, 1.0f-0.671877f,
		0.335973f, 1.0f-0.335903f,
		0.667969f, 1.0f-0.671889f,
		1.000004f, 1.0f-0.671847f,
		0.667979f, 1.0f-0.335851f
	};

/*static void get_file_data(void* ctx, const char* filename, const int is_mtl, const char* obj_filename, char** data, size_t* len) {
  // NOTE: If you allocate the buffer with malloc(),
  // You can define your own memory management struct and pass it through `ctx`
  // to store the pointer and free memories at clean up stage(when you quit an
  // app)
  // This example uses mmap(), so no free() required.
  //(void)ctx;

  if (!filename) {
    LOGE(TAG, "get_file_data - Filename is NULL");
    (*data) = NULL;
    (*len) = 0;
    return;
  }

  FILE* fp = fopen(filename, "r");
  if(!fp){
    LOGERRNO(TAG, filename);
    (*data) = NULL;
    (*len) = 0;
    return;
  }

  fseek(fp, 0L, SEEK_END);
  (*len) = ftell(fp);
  printf("[INFO] %s: File %s is %lu Bytes Long\n", TAG, filename, (*len));

  *data = (char*)malloc(*len);
  if(!data){
    LOGERRNO(TAG, "Malloc get_file_data");
    fclose(fp);
    return;
  }

  memcpy(ctx, data, sizeof(char*));
  // printf("Data %p | Data[0] %p\n", data, *data);
  // printf("ctx %p\n", ctx);
  rewind(fp);
  for(size_t i = 0; i < *len; i++){
    (*data)[i] = fgetc(fp);
  }

  fclose(fp);
}*/

int decoration_create(Decoration* decoration){
    /*tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t shapeCount = 0;
    tinyobj_material_t* materials = NULL;
    size_t materialCount = 0;
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;

    //Buffer for filedata
    char* context[1] = {NULL};

    int retval = tinyobj_parse_obj(&attrib, &shapes, &shapeCount, &materials, &materialCount, FILENAME, get_file_data, context, flags);
    if (retval != TINYOBJ_SUCCESS) {
        LOGE_S(TAG, "Failed to load ", FILENAME);
        return -1;
    }
    if(*context){
      LOGS(TAG, "Buffer freeed");
        free(*context);
        *context = NULL;
    }else{
        LOGW(TAG, "Buffer was NULL all the time");
    }

    printf("[INFO] %s: # of Shapes: %lu | # of Materials: %lu\n", TAG, shapeCount, materialCount);
    printf("[INFO] %s: # of Vertices: %u | # of Faces: %u\n", TAG, attrib.num_vertices, attrib.num_faces);*/
    //printf("Context %p | Context[0] %p\n", context, *context);
    /*for (int i = 0; i < shapeCount; i++) {
        printf("shape[%d] name = %s\n", i, shapes[i].name);
    }*/

    // size_t vertexBufferSize = attrib.num_faces * VERTEX_SIZE;
    // size_t uvBufferSize = attrib.num_faces * UV_SIZE;
    // size_t normalsBufferSize = attrib.num_faces * NORMALS_SIZE;
    /*size_t vertexBufferSize = 12 * VERTEX_SIZE;
    size_t uvBufferSize = 12 * UV_SIZE;
    size_t normalsBufferSize = 12 * NORMALS_SIZE;
    float *vertexBuffer, *uvBuffer, *normalsBuffer;

    printf("[INFO] %s: Buffer sizes are (%lu|%lu|%lu)\n", TAG, vertexBufferSize, uvBufferSize, normalsBufferSize);

    vertexBuffer = (float*)malloc(vertexBufferSize);
    uvBuffer = (float*)malloc(uvBufferSize);
    normalsBuffer = (float*)malloc(normalsBufferSize);
    if(vertexBuffer == NULL || uvBuffer == NULL || normalsBuffer == NULL){
      LOGERRNO(TAG, "Malloc buffers decoration_create");
      //tinyobj_shapes_free(shapes, shapeCount);
      //tinyobj_materials_free(materials, materialCount);
      //tinyobj_attrib_free(&attrib);
      return -1;
    }

   for(size_t i = 0; i < vertexBufferSize; i++)
      vertexBuffer[i] = 0.0f;
    for(size_t i = 0; i < uvBufferSize; i++)
      uvBuffer[i] = 0.0f;
    for(size_t i = 0; i < normalsBufferSize; i++)
      normalsBuffer[i] = 0.0f;
    */
    /*unsigned int triangleCount = 0;
    for(size_t i = 0; i < vertexBufferSize; i++){
      if(i % 9 == 0)
            printf("---Trianggle #%u---\n", ++triangleCount);
        printf("%.2f, ", vertexBuffer[i]);
        if(i % 3 == 2)
            printf("\n");
    }*/
    //LOGS(TAG, "Here 1");

    //if(0 > texture_create(&decoration->texture, "../assets/models/maus.albedo.png"))

    //printf("Num faces is %u\n", attrib.num_faces);
    //printf("Num vertices is %u\n", attrib.num_vertices);
    /*glGenBuffers(1, &decoration->vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, decoration->vertexbuffer);
    //glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexBuffer, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &decoration->uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, decoration->uvbuffer);
    //glBufferData(GL_ARRAY_BUFFER, uvBufferSize, uvBuffer, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);*/

    // free(vertexBuffer);
    // free(uvBuffer);
    // free(normalsBuffer);

    // tinyobj_shapes_free(shapes, shapeCount);
    // tinyobj_materials_free(materials, materialCount);
    // tinyobj_attrib_free(&attrib);
    texture_create(&decoration->texture, TEXTURE_UNKNOWN);
    vbo_create(&decoration->vbo, (float*)g_vertex_buffer_data, (float*)g_uv_buffer_data, NULL, 12);
    printf("[INFO] %s: VBO Buffers are (%d|%d|%d)\n", TAG, decoration->vbo.vertices, decoration->vbo.uv, decoration->vbo.normals);
    return 0;
}

void decoration_draw(Decoration* decoration){
  vbo_draw(&decoration->vbo, decoration->texture.bufferID);
  /*glBindTexture( GL_TEXTURE_2D, decoration->texture.bufferID);

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
    glDrawArrays(GL_TRIANGLES, 0, decoration->faceCount * 3); // start at 0; 6 because 6 points for 2 vertices
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
    glDrawArrays(GL_TRIANGLES, 0, decoration->faceCount * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);*/
}

void decoration_destroy(Decoration* decoration){
    // glDeleteBuffers(1, &decoration->uvbuffer);
    // glDeleteBuffers(1, &decoration->vertexbuffer);
    vbo_destroy(&decoration->vbo);
    texture_destroy(&decoration->texture);
}
