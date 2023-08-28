#pragma once

#include "Shader.h"
#include "Texture.h"
#include <imgui.h>

class Material
{

	public:
	Material();
	~Material();

	void Bind(Shader* shader);

	void SetDiffusePath(std::string path);

	ImTextureID GetDiffuseID();

	Texture* m_Diffuse;
};

