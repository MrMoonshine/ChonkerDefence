#pragma once
#include <GL/glew.h>
#include <GL/glext.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logging.h>

#define GL_WRAPPER_GLSL_MAX_ROW_LENGTH 256

GLuint glshader_load(const char* vtxshdrf, const char* frgmntshdrf);
