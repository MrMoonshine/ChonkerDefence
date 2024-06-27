#version 330 core

in vec2 texCoord;
in vec3 fontcolor;
// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	color = texture(myTextureSampler, texCoord).rgba;
}
