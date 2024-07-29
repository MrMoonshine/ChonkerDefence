#include <decoration.h>

static const char* TAG = "Decoration";

int decoration_create(Decoration* decoration, Obj* object){
  // Reset position
  for(uint8_t i = 0; i < 3; i++)
    decoration->position[i] = 0.0f;

  decoration->rotation = 0.0f;
  decoration->object = object;
  return 0;
}

void decoration_placement(Decoration* decoration, vec3 position, float rotation, float scale){
  memcpy(decoration->position, position, sizeof(vec3));
  decoration->rotation = glm_rad(rotation);
  decoration->scale = scale;
}

void decoration_draw(Decoration* decoration, GLuint uniformModel, GLuint uniformNormalMatrix, mat4 model){
  mat4 decomodel;
  glm_mat4_copy(model, decomodel);
  glm_translate(decomodel, decoration->position);

  vec3 axis = {0, 1, 0};
  glm_rotate(decomodel, decoration->rotation, axis);
  glm_scale_uni(decomodel, decoration->scale);

  mat3 normalMatrix;
  common_mat4_to_mat3(decomodel, normalMatrix);
  glm_mat3_inv(normalMatrix, normalMatrix);
  glm_mat3_transpose(normalMatrix);

  //printf("Rotation \n");
  /*for(uint8_t i = 0; i < 3; i++){
    for(uint8_t j = 0; j < 3; j++){
        printf("%.2f, ", normalMatrix[i][j]);
    }
    printf("\n");
  }*/

  glUniformMatrix4fv(uniformModel, 1, GL_FALSE, *decomodel);
  glUniformMatrix3fv(uniformNormalMatrix, 1, GL_FALSE, *normalMatrix);
  // Draw
  if(decoration->object)
    obj_draw(decoration->object);
}

void decoration_destroy(Decoration* decoration){

}
