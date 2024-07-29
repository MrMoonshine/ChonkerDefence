#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 cameraPos;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;
out vec3 viewPos;

void main(){
    gl_Position =  projection * view * model * vec4(vertexPosition_modelspace,1);
    texCoord = vertexUV;
    //Pass normalvectors on to the Fragment shader
    fragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0));
    normal = vertexNormals;
    viewPos = cameraPos;

    /*mat3 normalMatrix = mat3(projection * view * model);
    normalMatrix = inverse(normalMatrix);
    normalMatrix = transpose(normalMatrix);
    normal = normalize(vertexNormals * normalMatrix);*/
}
