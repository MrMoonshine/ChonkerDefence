#include <common.h>

void common_get_normal(float* vertexdata, float* normal){
    vec3 v1, v2, v3;
    vec3 tmp1, tmp2;
    vec3 vOut;

    memcpy(v1, vertexdata + 0, sizeof(vec3));
    memcpy(v2, vertexdata + 3, sizeof(vec3));
    memcpy(v3, vertexdata + 6, sizeof(vec3));

    glm_vec3_sub(v1, v2, tmp1);
    glm_vec3_sub(v2, v3, tmp2);

    glm_vec3_cross(tmp1, tmp2, vOut);
    glm_normalize(vOut);

    //printf("Generating Normal: (%.2f|%.2f|%.2f)\n", vOut[0], vOut[1], vOut[2]);
    for(uint8_t i = 0; i < 3; i++)
        normal[i] = vOut[i];
}

void common_print_vertices(float* buffer, size_t size){
    printf(" --- Vertex BUFFER ---\n");
    for(size_t i = 0; i < size/sizeof(float); i++){
        if(i % 9 == 0)
            printf("[");
        if(i % 3 == 0)
            printf("(");
        printf("%.2f", buffer[i]);
        if(i % 3 == 2)
            printf(")");
        else
            printf(", ");

        if(i % 9 == 8)
            printf("]\n");
        else if(i % 3 == 2)
            printf(", ");
    }
}

void common_print_uvs(float* buffer, size_t size){
    printf(" --- UV BUFFER ---\n");
    for(size_t i = 0; i < size/sizeof(float); i++){
        if(i % 6 == 0)
            printf("[");
        if(i % 2 == 0)
            printf("(");
        printf("%.2f", buffer[i]);
        if(i % 2 == 1)
            printf(")");
        else
            printf(", ");

        if(i % 6 == 5)
            printf("]");
        else if(i % 2 == 1)
            printf(", ");

        if(i % 18 == 17)
            printf("\n");
    }
}

void common_print_normals(float* buffer, size_t size){
    printf(" --- Normals BUFFER ---\n");
    for(size_t i = 0; i < size/sizeof(float); i++){
        if(i % 3 == 0)
            printf("(");
        printf("%.2f", buffer[i]);
        if(i % 3 == 2)
            printf("), ");
        else
            printf(", ");

        if(i % 18 == 17)
            printf("\n");
    }
}
