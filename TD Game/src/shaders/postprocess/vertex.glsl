#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

//uniform mat4 lightModel;
//uniform mat4 lightSpaceMatrix;

out vec2 TexCoord;

//out vec4 FragPosLightSpace;

void main(){
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

	TexCoord = aTexCoord;

	//FragPosLightSpace = lightSpaceMatrix * lightModel * vec4(aPos.x, aPos.y, 0.0, 1.0);
}