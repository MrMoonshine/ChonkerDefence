#include <logging.h>

void LOGI(const char* TAG, const char* msg){
    printf("[INFO] %s: %s\n", TAG, msg);
}

void LOGS(const char* TAG, const char* msg){
    printf("\e[0;32m[SUCCESS] %s: %s\e[0m\n", TAG, msg);
}

void LOGW(const char* TAG, const char* msg){
    printf("\e[0;33m[WARNING] %s: %s\n\e[0m", TAG, msg);
}

void LOGE(const char* TAG, const char* msg){
    fprintf(stderr,"\e[0;31m[ERROR] %s: %s\e[0m\n", TAG, msg);
}

void LOGGLERR(const char* TAG){
    GLenum err = glGetError();
    switch(err){
        case GL_NO_ERROR: LOGS(TAG, "No Error"); break;
        case GL_INVALID_ENUM: LOGE(TAG, "GL_INVALID_ENUM"); break;
        case GL_INVALID_VALUE: LOGE(TAG, "GL_INVALID_VALUE"); break;
        case GL_INVALID_OPERATION: LOGE(TAG, "GL_INVALID_OPERATION"); break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: LOGE(TAG, "GL_INVALID_FRAMEBUFFER_OPERATION"); break;
        case GL_OUT_OF_MEMORY: LOGE(TAG, "GL_OUT_OF_MEMORY"); break;
        case GL_STACK_UNDERFLOW: LOGE(TAG, "GL_STACK_UNDERFLOW"); break;
        case GL_STACK_OVERFLOW: LOGE(TAG, "GL_STACK_OVERFLOW"); break;
        default: LOGW(TAG, "Unknown Error"); break;
    }
}
