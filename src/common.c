#include <common.h>

void common_get_normal(float* vertexdata, float* normal){
    vec3 v1, v2, v3;
    vec3 tmp1, tmp2;
    //vec3 vOut;

    memcpy(v1, vertexdata + 0, sizeof(vec3));
    memcpy(v2, vertexdata + 3, sizeof(vec3));
    memcpy(v3, vertexdata + 6, sizeof(vec3));

    glm_vec3_sub(v1, v2, tmp1);
    glm_vec3_sub(v2, v3, tmp2);

    glm_vec3_cross(tmp1, tmp2, normal);
    glm_normalize(normal);
}
