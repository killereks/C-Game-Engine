#version 330 core

out vec4 FragColor;

in vec3 fragPos;

in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewDir;

struct Light {
	vec3 position;
	vec3 color;
	float intensity;
	float range;
};

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (1.0 + 0.09 * pow(distance, 2.0));

	vec3 ambient = light.color * light.intensity * 0.1;
	vec3 diffuse = light.color * diff * light.intensity;
	vec3 specular = light.color * spec * light.intensity;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

uniform Light lights[4];

void main(){
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	int x = int(round(fragPos.x)) % 2;
	int y = int(round(fragPos.z)) % 2;

	if(x + y == 1){
		color = vec4(0.7, 0.7, 0.7, 1.0);
	}
	else {
		color = vec4(0.4, 0.4, 0.4, 1.0);
	}

	vec3 intensity = vec3(0.0);

	for(int i = 0; i < 4; i++){
		intensity += CalcPointLight(lights[i], normalize(Normal), fragPos, viewDir);
	}

	FragColor = color * vec4(intensity, 1.0f);
}