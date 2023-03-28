#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;

out vec3 fragPos;

void main(){
	fragPos = aPos;

	gl_Position = vec4(0.2, 0.5, 0.0, 1.0);
}