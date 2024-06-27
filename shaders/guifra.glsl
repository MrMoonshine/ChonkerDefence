#version 330 core

in vec2 texCoord;
// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	// Output color = red
	vec4 mycolor = vec4(0,0.8,0.5, 0.5);
	color = texture(myTextureSampler, texCoord).rgba;
	//color = texture(myTextureSampler, texCoord).rgba;
	//vec4 texturecolor = texture(myTextureSampler, texCoord).rgba;
	//vec3 coloropaque = mix(texturecolor.rgb, mycolor.rgb, mycolor.a);
}
