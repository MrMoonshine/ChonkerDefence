#version 330 core

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;
in vec3 viewPos;
// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	vec3 lightPos = vec3(0.0, 5.0, 16.0);
	vec3 lightColour = vec3(1,1,1);
	vec4 texture_color = texture(myTextureSampler, texCoord).rgba;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	//Using dot product to determine diffusal
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColour;

	//Using ambient variables to darken the model
	float ambientStrength = 0.7;
	vec3 ambient = ambientStrength * lightColour;

	// Specular stuff
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColour;

	color = vec4((ambient + diffuse), 1.0) * texture_color;
	//color = vec4(normalize(normal), 1.0);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
	//color = texture(myTextureSampler, texCoord).rgba;
}
