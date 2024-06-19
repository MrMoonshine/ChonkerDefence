#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void LOGI(const char* TAG, const char* msg);
void LOGS(const char* TAG, const char* msg);
void LOGW(const char* TAG, const char* msg);
void LOGE(const char* TAG, const char* msg);

void LOGGLERR(const char* TAG);
