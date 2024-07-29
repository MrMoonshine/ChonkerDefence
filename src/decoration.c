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

void decoration_draw(Decoration* decoration, GLuint uniformModel, mat4 model){
  mat4 decomodel;
  glm_mat4_copy(model, decomodel);
  glm_translate(decomodel, decoration->position);

  vec3 axis = {0, 0, 1}; // Z Axis
  glm_rotate(decomodel, decoration->rotation, axis);
  glm_scale_uni(decomodel, decoration->scale);

  glUniformMatrix4fv(uniformModel, 1, GL_FALSE, *decomodel);
  // Draw
  if(decoration->object)
    obj_draw(decoration->object);
}

void decoration_destroy(Decoration* decoration){

}
