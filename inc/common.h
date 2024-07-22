#pragma once

#define VERTEX_SIZE (3*3*sizeof(float))
#define UV_SIZE (2*3*sizeof(float))
#define NORMALS_SIZE (3*sizeof(float))

#define FILESYSTEM_SLASH '/'
#define FILE_EXISTS(filename) (access(filename, F_OK) == 0)
