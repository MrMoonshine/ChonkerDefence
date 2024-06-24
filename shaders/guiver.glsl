#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

//out vec3 texColor;
out vec2 texCoord;

void main(){
    gl_Position =  projection * model * vec4(vertexPosition_modelspace,1);
    texCoord = vertexUV;
}
