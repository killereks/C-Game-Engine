#version 330 core

out vec4 FragColor;

in vec3 fragPos;

in vec3 Normal;
in vec2 TexCoord;

void main(){
	vec3 lightDir = normalize(vec3(0.3, 0.2, 0.5));

	float diff = max(dot(Normal, lightDir), 0.0);
	FragColor = vec4(Normal, 1.0);

	//FragColor = vec4(diff, diff, diff, 1.0);

	/*vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	// create a grid pattern
	int x = int(round(fragPos.x)) % 2;
	int y = int(round(fragPos.z)) % 2;

	if(x + y == 1){
		color = vec4(0.7f, 0.7f, 0.7f, 1.0f);
	}
	else {
		color = vec4(0.4f, 0.4f, 0.4f, 1.0f);
	}

	FragColor = color;*/

	// just display the UVs
	//FragColor = vec4(Normal, 1.0);
}