#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
	public:
	Material();
	~Material();

	void Bind();

	Shader* m_Shader;

	std::string m_DiffusePath;
	std::string m_SpecularPath;
	std::string m_NormalPath;
	std::string m_HeightPath;
	std::string m_OcclusionPath;
};

