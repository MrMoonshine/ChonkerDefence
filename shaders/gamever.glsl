#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec2 vertexNormals;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoord;
out vec4 colorize_fra;

void main(){
    gl_Position =  projection * view * model * vec4(vertexPosition_modelspace,1);
    texCoord = vertexUV;
}
