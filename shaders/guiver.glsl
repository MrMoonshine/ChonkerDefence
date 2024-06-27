#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

//Color blending. Mainly used for text
uniform vec4 colorize;

out vec2 texCoord;
out vec4 colorize_fra;

void main(){
    gl_Position =  projection * model * vec4(vertexPosition_modelspace,1);
    texCoord = vertexUV;
    colorize_fra = colorize;
}
