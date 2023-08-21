#version 330 core

out vec4 FragColor;

in vec3 fragPos;

void main(){
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	// create a grid pattern
	int x = int(round(fragPos.x)) % 2;
	int y = int(round(fragPos.z)) % 2;

	if(x + y == 1){
		color = vec4(0.7f, 0.7f, 0.7f, 1.0f);
	}
	else {
		color = vec4(0.4f, 0.4f, 0.4f, 1.0f);
	}

	FragColor = color;
}