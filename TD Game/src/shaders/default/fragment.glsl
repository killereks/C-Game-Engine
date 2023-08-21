#version 330 core

out vec4 FragColor;

in vec3 fragPos;

void main(){
	vec3 lightPos = vec3(0.0f, 3.0f, 0.0f);
	vec4 color = vec4(1.0, 0.0, 0.2, 1.0);

	float dist = length(lightPos - fragPos);
	float att = 1000.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);

	color *= att;

	FragColor = color;
}