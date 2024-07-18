#include <decoration.h>
#include "../lib/tinyobjloader-c/tinyobj_loader_c.h"

static const char* TAG = "Decoration";
//static const char* FILENAME = "/home/david/Dokumente/Minecraft/Mods/Railroading/2016/deploy/oebb2016.obj";
static const char* FILENAME = "../assets/models/maus.obj";
//static const char* FILENAME = "../assets/models/cube.obj";

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

static void get_file_data(void* ctx, const char* filename, const int is_mtl, const char* obj_filename, char** data, size_t* len) {
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
}

int decoration_create(Decoration* decoration){
    tinyobj_attrib_t attrib;
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
      //LOGS(TAG, "Buffer freeed");
        free(*context);
        *context = NULL;
    }else{
        //LOGW(TAG, "Buffer was NULL all the time");
    }

    // set face count
    decoration->faceCount = attrib.num_face_num_verts;

    printf("[INFO] %s: # of Shapes: %lu | # of Materials: %lu\n", TAG, shapeCount, materialCount);
    printf("[INFO] %s: # of Vertices: %u | # of Faces: %d | # of normals: %d\n", TAG, attrib.num_vertices, attrib.num_face_num_verts, attrib.num_normals);
    //printf("Context %p | Context[0] %p\n", context, *context);
    /*for (int i = 0; i < shapeCount; i++) {
        printf("shape[%d] name = %s\n", i, shapes[i].name);
    }*/

    size_t vertexBufferSize = decoration->faceCount * VERTEX_SIZE;
    size_t uvBufferSize = decoration->faceCount * UV_SIZE;
    size_t normalsBufferSize = decoration->faceCount * NORMALS_SIZE;

    printf("[INFO] %s: Buffer sizes are (%lu|%lu|%lu)\n", TAG, vertexBufferSize, uvBufferSize, normalsBufferSize);
    size_t sizes[3] = {vertexBufferSize, uvBufferSize, normalsBufferSize};
    vbo_init(&decoration->vbo, decoration->faceCount);
    for(size_t i = 0; i < 3; i++){
        float* buffer = NULL;
        buffer = (float*)malloc(sizes[i]);
        if(!buffer){
          LOGERRNO(TAG, "Malloc buffers decoration_create");
          tinyobj_shapes_free(shapes, shapeCount);
          tinyobj_materials_free(materials, materialCount);
          tinyobj_attrib_free(&attrib);
          return -1;
        }

        for(size_t j = 0; j < sizes[i]/sizeof(float); j++){
          buffer[j] = 0.0f;
        }

        size_t pos = 0;
        switch(i){
          case VBO_INDEX_VERTEX: {
            //memcpy(buffer, g_vertex_buffer_data, sizes[i]);
            printf("Size is %lu\t# of floats = %lu\n", sizes[i], sizes[i] / sizeof(float));
            //size_t faceOffset = 0;
            for(size_t a = 0; a < attrib.num_face_num_verts * 3; a++){
              //printf("Faces (v|vt|vn) (%d|%d|%d)\n", attrib.faces[a].v_idx, attrib.faces[a].vt_idx, attrib.faces[a].vn_idx);
              for(uint8_t b = 0; b < 3; b++){
                if(pos >= sizes[i] / sizeof(float))
                  continue;
                buffer[pos++] = attrib.vertices[attrib.faces[a].v_idx * 3 + b];
              }
              //printf("Oida %lu\n", (size_t)attrib.face_num_verts[a]);
              /*for(size_t b = 0; b < (size_t)attrib.face_num_verts[a]; b++){
                printf("\tOida %lu => %.2f\n", faceOffset + b, 0.0f);
                size_t pos1 = 3 * faceOffset + b;
                buffer[pos1 + 0] = 1.0f;
                buffer[pos1 + 1] = 2.0f;
                buffer[pos1 + 2] = 3.0f;
              }
              faceOffset += (size_t)attrib.face_num_verts[a];*/
            }

            /*size_t triangleCount = 1;
            for(size_t a = 0; a < sizes[i] / (sizeof(float)); a++){
              if(a % 9 == 0)
                printf(" --- TRIANGLE %lu --- \n", triangleCount++);
              printf("%.2f, ", buffer[a]);
              if(a % 3 == 2)
                printf("\n");
            }
            printf("\n");*/

            /*for(size_t a = 0; a < attrib.num_vertices * 3; a++){
              //printf("Vertices (v|vt|vn) (%d|%d|%d)\n", attrib.faces[a].v_idx, attrib.faces[a].vt_idx, attrib.faces[a].vn_idx);
              printf("%.2f, ", attrib.vertices[a]);
              if(a % 3 == 2)
                printf("\n");
            }*/
            vbo_create_vertices(&decoration->vbo, buffer);
          } break;
          case VBO_INDEX_UV:{
            //memcpy(buffer, g_uv_buffer_data, sizes[i]);
            for(size_t a = 0; a < attrib.num_face_num_verts * 3; a++){
              //printf("Faces (v|vt|vn) (%d|%d|%d)\n", attrib.faces[a].v_idx, attrib.faces[a].vt_idx, attrib.faces[a].vn_idx);
              for(uint8_t b = 0; b < 2; b++){
                if(pos >= sizes[i] / sizeof(float))
                  continue;
                buffer[pos++] = attrib.texcoords[attrib.faces[a].vt_idx * 2 + b];
              }
            }

            /*size_t triangleCount = 1;
            for(size_t a = 0; a < sizes[i] / (sizeof(float)); a++){
              if(a % 6 == 0)
                printf(" --- UV TRIANGLE %lu --- \n", triangleCount++);
              printf("%.2f, ", buffer[a]);
              if(a % 2 == 1)
                printf("\n");
            }
            printf("\n");*/

            vbo_create_uv(&decoration->vbo, buffer);
          } break;
          case VBO_INDEX_NORMAL: {
            for(size_t a = 0; a < attrib.num_face_num_verts; a++){
              //printf("Faces (v|vt|vn) (%d|%d|%d)\n", attrib.faces[a].v_idx, attrib.faces[a].vt_idx, attrib.faces[a].vn_idx);
              for(uint8_t b = 0; b < 3; b++){
                if(pos >= sizes[i] / sizeof(float))
                  continue;
                buffer[pos++] = attrib.texcoords[attrib.faces[a * 3].vn_idx + b];
              }
            }

            /*size_t triangleCount = 1;
            for(size_t a = 0; a < sizes[i] / (sizeof(float)); a++){
              if(a % 3 == 0)
                printf(" --- Normal TRIANGLE %lu --- \n", triangleCount++);
              printf("%.2f, ", buffer[a]);
              if(a % 3 == 2)
                printf("\n");
            }
            printf("\n");*/
            vbo_create_normals(&decoration->vbo, buffer);
          } break;
          default: break;
        }

        free(buffer);
    }


    //texture_create(&decoration->texture, TEXTURE_UNKNOWN);
    texture_create(&decoration->texture, "../assets/models/maus.albedo.png");
    //vbo_create(&decoration->vbo, (float*)g_vertex_buffer_data, (float*)g_uv_buffer_data, NULL, 12);
    printf("[INFO] %s: VBO Buffers are (%d|%d|%d)\n", TAG, decoration->vbo.vertices, decoration->vbo.uv, decoration->vbo.normals);
    return 0;

    tinyobj_shapes_free(shapes, shapeCount);
    tinyobj_materials_free(materials, materialCount);
    tinyobj_attrib_free(&attrib);
}

void decoration_draw(Decoration* decoration){
  vbo_draw(&decoration->vbo, decoration->texture.bufferID);
}

void decoration_destroy(Decoration* decoration){
    // glDeleteBuffers(1, &decoration->uvbuffer);
    // glDeleteBuffers(1, &decoration->vertexbuffer);
    vbo_destroy(&decoration->vbo);
    texture_destroy(&decoration->texture);
}
