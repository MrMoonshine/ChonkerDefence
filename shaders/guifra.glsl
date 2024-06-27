#version 330 core

in vec2 texCoord;
in vec4 colorize_fra;
// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	// Output color = red
	vec4 texturecolor = texture(myTextureSampler, texCoord).rgba;
	vec4 blendedcolor = mix(texturecolor, colorize_fra, colorize_fra.a);
	blendedcolor.a = texturecolor.a;
	color = blendedcolor;
}
