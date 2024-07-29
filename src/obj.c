
#include <obj.h>
#include "../lib/tinyobjloader-c/tinyobj_loader_c.h"

static const char* TAG = "Obj";
static const uint8_t MTL_FILENAME_BUFFER_LENGTH = 128;

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
#ifdef DEBUG_OBJ
  printf("[INFO] %s: File %s is %lu Bytes Long\n", TAG, filename, (*len));
#endif /*DEBUG_OBJ*/
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

int obj_create(Obj* obj, const char* filename){
    obj->faceCount = 0;
    obj->shapeCount = 0;
    obj->textureCount = 0;

    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t shapeCount = 0;
    tinyobj_material_t* materials = NULL;
    size_t materialCount = 0;
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;

    //Buffer for filedata
    char* context[1] = {NULL};

    int retval = tinyobj_parse_obj(&attrib, &shapes, &shapeCount, &materials, &materialCount, filename, get_file_data, context, flags);
    if (retval != TINYOBJ_SUCCESS) {
        LOGE_S(TAG, "Failed to load ", filename);
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
    obj->faceCount = attrib.num_face_num_verts;
#ifdef DEBUG_OBJ
    printf("[INFO] %s: # of Shapes: %lu | # of Materials: %lu\n", TAG, shapeCount, materialCount);
    printf("[INFO] %s: # of Vertices: %u | # of Faces: %d | # of normals: %d\n", TAG, attrib.num_vertices, attrib.num_face_num_verts, attrib.num_normals);
#endif /*DEBUG_OBJ*/
    // Process shapes
    // attrib.materials_id is for each face

    for(size_t i = 0; i < shapeCount; i++){
      size_t matindex = attrib.material_ids[shapes[i].face_offset];
#ifdef DEBUG_OBJ
      printf("[INFO] %s: Shape %s:\n", TAG, shapes[i].name);
      printf("\tface_offset %d\n",shapes[i].face_offset);
      printf("\tlength %d\n",shapes[i].length);
      printf("\tMaterial %s %s\n", materials[matindex].name, materials[matindex].diffuse_texname);
#endif /*DEBUG_OBJ*/
      if(strlen(materials[matindex].diffuse_texname) > 0)
        obj->textureCount++;
    }

    obj->shapeCount = shapeCount;
    obj->shapes = (struct Shape*)malloc(sizeof(struct Shape) * obj->shapeCount);
    if(!obj->shapes){
      LOGERRNO(TAG, "Malloc Shapes");
      return -1;
    }

    obj->textures = (Texture*)malloc(sizeof(Texture) * obj->textureCount);
    if(!obj->textures){
      LOGERRNO(TAG, "Malloc Textures");
      return -1;
    }
    /*
      Build shapes and textures
    */
    unsigned int textureCounter = 0;
    for(size_t i = 0; i < shapeCount; i++){
      obj->shapes[i].faceOffset = shapes[i].face_offset;
      obj->shapes[i].length = shapes[i].length;

      size_t matindex = attrib.material_ids[shapes[i].face_offset];
      size_t textureFileLength = strlen(materials[matindex].diffuse_texname);
      if(textureFileLength > 0){
        int ret =0;
        if(materials[matindex].diffuse_texname[0] == FILESYSTEM_SLASH){
          ret = texture_create(obj->textures + textureCounter, materials[matindex].diffuse_texname);
        }else{
          unsigned int lastslashpos = 0;
          for(unsigned int j = 0; j < strlen(filename); j++){
            if(filename[j] == FILESYSTEM_SLASH)
              lastslashpos = j;
          }
          //printf("Last / pos is %u\n", lastslashpos);
          char texturefilename[MTL_FILENAME_BUFFER_LENGTH];
          memset(texturefilename, '\0', MTL_FILENAME_BUFFER_LENGTH);
          if(textureFileLength + lastslashpos > MTL_FILENAME_BUFFER_LENGTH){
            ret = texture_create(obj->textures + textureCounter, TEXTURE_UNKNOWN);
          }else{
            strcpy(texturefilename, "");
            strncat(texturefilename, filename, lastslashpos + 1);
            strcat(texturefilename, materials[matindex].diffuse_texname);
            texture_create(obj->textures + textureCounter, texturefilename);
          }
        }
        if(ret < 0){
          LOGE_S(TAG, "Failed to load ", materials[matindex].diffuse_texname);
          texture_create(obj->textures + textureCounter, TEXTURE_UNKNOWN);
        }
        obj->shapes[i].textureIndex = textureCounter;
        textureCounter++;
      }else{
        obj->shapes[i].textureIndex = UINT_MAX;
      }
    }

    /*for(size_t i = 0; i < materialCount; i++){
      printf("[INFO] %s: Material %s:\n", TAG, materials[i].name);
      printf("\tmap_Kd %s\n", materials[i].diffuse_texname);
    }*/

    //printf("Context %p | Context[0] %p\n", context, *context);
    /*for (int i = 0; i < shapeCount; i++) {
        printf("shape[%d] name = %s\n", i, shapes[i].name);
    }*/

    size_t vertexBufferSize = obj->faceCount * VERTEX_SIZE;
    size_t uvBufferSize = obj->faceCount * UV_SIZE;
    size_t normalsBufferSize = obj->faceCount * NORMALS_SIZE;
#ifdef DEBUG_OBJ
    printf("[INFO] %s: Buffer sizes are (%lu|%lu|%lu)\n", TAG, vertexBufferSize, uvBufferSize, normalsBufferSize);
#endif
    size_t sizes[3] = {vertexBufferSize, uvBufferSize, normalsBufferSize};
    vbo_init(&obj->vbo, obj->faceCount);
    for(size_t i = 0; i < 3; i++){
        float* buffer = NULL;
        buffer = (float*)malloc(sizes[i]);
        if(!buffer){
          LOGERRNO(TAG, "Malloc buffers obj_create");
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
            #ifdef DEBUG_OBJ
              LOGI(TAG, "Building Veritces");
            #endif

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
            vbo_create_vertices(&obj->vbo, buffer);
          } break;
          case VBO_INDEX_UV:{
            #ifdef DEBUG_OBJ
              LOGI(TAG, "Building UVs");
            #endif
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

            vbo_create_uv(&obj->vbo, buffer);
          } break;
          case VBO_INDEX_NORMAL: {
            #ifdef DEBUG_OBJ
              LOGI(TAG, "Building Normals");
            #endif
            for(size_t a = 0; a < attrib.num_face_num_verts * 3 && attrib.num_normals > 0; a++){
              // printf("Faces (v|vt|vn) (%d|%d|%d)\n", attrib.faces[a].v_idx, attrib.faces[a].vt_idx, attrib.faces[a].vn_idx);
              // printf("Normal %u | ", attrib.faces[a].vn_idx);
              // size_t mypos = attrib.faces[a].vn_idx;
              // printf("(%.2f|%.2f|%.2f)\n", attrib.normals[mypos * 3 + 1], attrib.normals[mypos * 3 + 1], attrib.normals[mypos * 3 + 2]);
              for(uint8_t b = 0; b < 3; b++){
                if(pos >= sizes[i] / sizeof(float))
                  continue;
                //printf("Index %u of %u\n", attrib.faces[a].vn_idx, attrib.num_normals);
                buffer[pos++] = attrib.normals[attrib.faces[a].vn_idx * 3 + b];
              }
            }
            //common_print_normals(buffer, attrib.num_face_num_verts * NORMALS_SIZE);
            vbo_create_normals(&obj->vbo, buffer);
          } break;
          default: break;
        }
        free(buffer);
    }


    //vbo_create(&obj->vbo, (float*)g_vertex_buffer_data, (float*)g_uv_buffer_data, NULL, 12);
#ifdef DEBUG_OBJ
    printf("[INFO] %s: VBO Buffers are (%d|%d|%d)\n", TAG, obj->vbo.vertices, obj->vbo.uv, obj->vbo.normals);
#endif
    return 0;

    tinyobj_shapes_free(shapes, shapeCount);
    tinyobj_materials_free(materials, materialCount);
    tinyobj_attrib_free(&attrib);
}

void obj_draw(Obj* obj){
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glCullFace(GL_FRONT);       // Face culling has to be inverted for this voxel mode

  for(unsigned int i = 0; i < obj->shapeCount; i++){
    //LOGW(TAG, "Here 1");
    if(obj->shapes[i].textureIndex < obj->textureCount)
    glBindTexture( GL_TEXTURE_2D, obj->textures[obj->shapes[i].textureIndex].bufferID);
    //LOGW(TAG, "Here VERTICES");
    // VERTICES
    glEnableVertexAttribArray(VBO_INDEX_VERTEX);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo.vertices);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)(obj->shapes[i].faceOffset * VERTEX_SIZE)            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, obj->shapes[i].length * 3);
    //LOGW(TAG, "Here UV");
    // UV
    glEnableVertexAttribArray(VBO_INDEX_UV);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo.uv);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)(obj->shapes[i].faceOffset * UV_SIZE)                          // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, obj->shapes[i].length * 3);
    //LOGW(TAG, "Here NORMALS");
    // NORMALS
    glEnableVertexAttribArray(VBO_INDEX_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo.normals);
    glVertexAttribPointer(
        2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
        3,                                // size : Normals => 3
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)(obj->shapes[i].faceOffset * NORMALS_SIZE)                          // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, obj->shapes[i].length * 3);

    glDisableVertexAttribArray(VBO_INDEX_VERTEX);
    glDisableVertexAttribArray(VBO_INDEX_UV);
    glDisableVertexAttribArray(VBO_INDEX_NORMAL);

    // unbind texture
    glBindTexture( GL_TEXTURE_2D, 0);
  }
  glCullFace(GL_BACK);
  glDisable(GL_DEPTH_TEST);
}

void obj_destroy(Obj* obj){
    vbo_destroy(&obj->vbo);
    free(obj->shapes);
    for(unsigned int i = 0; i < obj->textureCount; i++)
      texture_destroy(obj->textures + i);
}
