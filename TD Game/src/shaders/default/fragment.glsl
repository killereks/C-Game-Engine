#version 330 core

out vec4 FragColor;

in vec3 fragPos;

in vec2 TexCoord;
in vec4 fragPosLight;
in vec3 Normal;

uniform vec3 lightDir;

in mat3 TBN;

uniform sampler2D shadowMap;

uniform sampler2D u_Diffuse;
//uniform sampler2D u_Normal;

uniform vec3 camFwd;

uniform int selected;

float num(vec4 seed){
	float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float shadowCalc(float dotLightNormal, vec2 offset){
	float bias = max(0.0005 * (1.0 - dotLightNormal), 0.0005);
	vec3 pos = fragPosLight.xyz / fragPosLight.w;
	pos = pos * 0.5 + 0.5;

	if (pos.z > 1.0) return 0.0;

	float rand = num(vec4(pos.xy, pos.y, pos.x)) * 0.001;
	float depth = texture(shadowMap, pos.xy + offset + rand).r;

	if (depth + bias < pos.z) return 0.0;
	return 1.0;
}

float softShadows(float dotLightNormal){
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	float shadow = 0.0;

	int size = 2;
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
	/*vec3 Normal = normalize(texture(u_Normal, TexCoord).rgb * 2.0 - 1.0);
	Normal = normalize(TBN * Normal) * 0.5 + 0.5;*/

	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	int x = int(round(fragPos.x)) % 2;
	int y = int(round(fragPos.z)) % 2;

	if(x + y == 1){
		color = vec4(0.7, 0.7, 0.7, 1.0);
	}
	else {
		color = vec4(0.4, 0.4, 0.4, 1.0);
	}

	color = texture(u_Diffuse, TexCoord);

	float shadow = softShadows(dot(lightDir, Normal));

	float ambient = 0.4;
	float diffuse = saturate(dot(lightDir, Normal));

	vec3 viewDir = normalize(camFwd - fragPos);
	vec3 reflectDir = reflect(-lightDir, Normal);

	float specular = pow(saturate(dot(viewDir, reflectDir)), 32.0);

	vec3 result = (ambient + diffuse + specular) * color.rgb * shadow;
	
	if(selected == 1){
		FragColor = max(0.5, dot(camFwd, Normal)) * color;
	} else {
		FragColor = vec4(result, 1.0);
	}
}