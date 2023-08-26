#version 330 core

out vec4 FragColor;

in vec3 fragPos;

in vec3 Normal;
in vec2 TexCoord;
in vec4 fragPosLight;

uniform sampler2D shadowMap;
uniform vec3 lightDir;

uniform vec3 camFwd;

uniform int selected;

float shadowCalc(float dotLightNormal, vec2 offset){
	float bias = max(0.0005 * (1.0 - dotLightNormal), 0.0005);
	vec3 pos = fragPosLight.xyz / fragPosLight.w;
	pos = pos * 0.5 + 0.5;

	if (pos.z > 1.0) return 0.0;
	float depth = texture(shadowMap, pos.xy + offset).r;

	if (depth + bias < pos.z) return 0.0;
	return 1.0;
}

float softShadows(float dotLightNormal){
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	float shadow = 0.0;

	int size = 3;
	for(int x = -size; x <= size; ++x){
		for(int y = -size; y <= size; ++y){
			vec2 sampleOffset = vec2(x,y) * texelSize;
			shadow += shadowCalc(dotLightNormal, sampleOffset);
		}
	}

	shadow /= (2 * size + 1) * (2 * size + 1);
	return shadow;
}

float saturate(float val){
	return clamp(val, 0.0, 1.0);
}


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

	float shadow = softShadows(dot(lightDir, Normal));

	
	if(selected == 1){
		color = dot(Normal, camFwd) * vec4(1.0, 0.0, 0.0, 1.0);
	}


	FragColor = color * shadow;
}