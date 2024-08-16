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
void common_mat4_to_mat3(mat4 mi, mat3 dest);
/*
    @brief Get the node data at (x|y)
    @param buffer[in] buffer of map data
    @param len[in] buffer length
    @param x[in] x of map
    @param y[in] y of map
    @param width[in] width of map
    @param height[in] height of map
*/
uint8_t common_map_get_node_at(uint8_t *buffer, size_t len, uint8_t x, uint8_t y, uint8_t width, uint8_t height);

void common_print_vertices(float* buffer, size_t size);
void common_print_uvs(float* buffer, size_t size);
void common_print_normals(float* buffer, size_t size);
