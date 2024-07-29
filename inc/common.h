#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define VERTEX_SIZE (3*3*sizeof(float))
#define UV_SIZE (2*3*sizeof(float))
#define NORMALS_SIZE (3*3*sizeof(float))

#define FILESYSTEM_SLASH '/'
#define FILE_EXISTS(filename) (access(filename, F_OK) == 0)

/*
    @brief gets the normal vector
    @param [in]vertexdata vertex data. 3 vectors, 9 components
    @patam [out]normal normal vector
*/
void common_get_normal(float* vertexdata, float* normal);

void common_print_vertices(float* buffer, size_t size);
void common_print_uvs(float* buffer, size_t size);
void common_print_normals(float* buffer, size_t size);
