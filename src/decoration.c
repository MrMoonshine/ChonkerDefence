#include <decoration.h>

static const char* TAG = "Decoration";
//static const char* FILENAME = "/home/david/Dokumente/Minecraft/Mods/Railroading/2016/deploy/oebb2016.obj";
//static const char* FILENAME = "/home/david/Dokumente/Minecraft/Mods/Railroading/1144/deploy/deploymc/oebb1144.obj";
//static const char* FILENAME = "../build/model/oida.obj";
//static const char* FILENAME = "../assets/models/maus.obj";
static const char* FILENAME = "../assets/models/default_tree_2.obj";

int decoration_create(Decoration* decoration){
  // Reset position
  for(uint8_t i = 0; i < 3; i++)
    decoration->position[i] = 0.0f;

  decoration->rotation = 0.0f;

  return obj_create(&decoration->object, FILENAME);
}

void decoration_placement(Decoration* decoration, vec3 position, float rotation, float scale){
  memcpy(decoration->position, position, sizeof(vec3));
  decoration->rotation = rotation;
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
  obj_draw(&decoration->object);
}

void decoration_destroy(Decoration* decoration){
    obj_destroy(&decoration->object);
}
