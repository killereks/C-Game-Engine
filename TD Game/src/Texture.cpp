#define STB_IMAGE_IMPLEMENTATION
#include "images/stb_image.h"

#include "Texture.h"
#include <GL/glew.h>

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::LoadFromFile(const std::string path)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int numChannels;
	unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &numChannels, 3);
	
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "Loaded texture: " << path << std::endl;
	}
	else {
		std::cerr << "Failed to load texture: " << path << std::endl;
	}

	stbi_image_free(data);
}

void Texture::Bind(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}