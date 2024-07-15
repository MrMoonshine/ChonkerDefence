#pragma once
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void LOGI(const char* TAG, const char* msg);
void LOGS(const char* TAG, const char* msg);
void LOGW(const char* TAG, const char* msg);
void LOGE(const char* TAG, const char* msg);

void LOGE_S(const char* TAG, const char* msg1, const char* msg2);

void LOGGLERR(const char* TAG);
void LOGERRNO(const char* TAG, const char* msg);
