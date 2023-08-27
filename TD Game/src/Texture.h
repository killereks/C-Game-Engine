#pragma once

#include <iostream>

class Texture
{
	public:
	Texture();
	~Texture();

	void LoadFromFile(const std::string path);

	void Bind(unsigned int slot);

	unsigned int GetWidth() { return m_Width; }
	unsigned int GetHeight() { return m_Height; }

	private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
};

