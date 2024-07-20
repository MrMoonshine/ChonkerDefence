#include <decoration.h>

static const char* TAG = "Decoration";
//static const char* FILENAME = "/home/david/Dokumente/Minecraft/Mods/Railroading/2016/deploy/oebb2016.obj";
static const char* FILENAME = "/home/david/Dokumente/Minecraft/Mods/Railroading/1144/deploy/deploymc/oebb1144.obj";
//static const char* FILENAME = "../build/oida1.obj";
//static const char* FILENAME = "../assets/models/maus.obj";
//static const char* FILENAME = "../assets/models/cube.obj";

int decoration_create(Decoration* decoration){
  return obj_create(&decoration->object, FILENAME);
}

void decoration_draw(Decoration* decoration){
  //vbo_draw(&decoration->vbo, decoration->texture.bufferID);
  obj_draw(&decoration->object);
}

void decoration_destroy(Decoration* decoration){
    obj_destroy(&decoration->object);
}
