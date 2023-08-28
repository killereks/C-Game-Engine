#include "Material.h"

Material::Material()
{
	
}

Material::~Material()
{
	if (m_Diffuse != nullptr) {
		delete m_Diffuse;
	}
}

void Material::Bind(Shader* shader)
{
	if (m_Diffuse != nullptr){
		m_Diffuse->Bind(3);
		shader->SetInt("u_Diffuse", 3);
	}
}

void Material::SetDiffusePath(std::string path)
{
	if (m_Diffuse == nullptr) {
		m_Diffuse = new Texture();
	}

	m_Diffuse->LoadFromFile(path);
}

ImTextureID Material::GetDiffuseID()
{
	if (m_Diffuse == nullptr) return 0;

	return (ImTextureID)m_Diffuse->GetID();
}

