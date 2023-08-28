#pragma once
#include <vector>
#include <Shader.h>
class PostProcess
{
	public:
	PostProcess();
	~PostProcess();

	unsigned int m_IBO;
	unsigned int m_VBO;

	unsigned int m_VAO;

	unsigned int m_FBO;
	unsigned int m_RBO;

	unsigned int m_Texture;

	int m_Width;
	int m_Height;

	void Setup();

	std::vector<float> m_Vertices;

	std::vector<unsigned int> m_Indices;

	void BindFBO(int width, int height);
	void Unbind();

	void Draw(Shader* shader);
};

