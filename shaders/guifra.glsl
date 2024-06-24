#version 330 core

in vec2 texCoord;
// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	// Output color = red
	//color = vec3(0,1,1);
	color = texture(myTextureSampler, texCoord).rgba;
}
