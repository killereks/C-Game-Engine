#pragma once

#include "Shader.h"

class LightMapper
{
	public:	
	LightMapper();
	LightMapper(int width, int height);
	~LightMapper();

	// opengl framebuffer stuff
	unsigned int shadowMapFBO;
	unsigned int shadowMapTexture;

	unsigned int mapWidth;
	unsigned int mapHeight;

	void Bind();
	void Unbind();

	void SaveDepthAsBMP(std::string path);

	void SetUniforms(Shader* shader);

	void SetSize(int width, int height);

private:
	void Update();
};

