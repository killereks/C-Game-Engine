#include "LightMapper.h"

#include <GL/glew.h>
#include <cstddef>

LightMapper::LightMapper(int width, int height) {
	// create a framebuffer object
	glGenFramebuffers(1, &shadowMapFBO);

	SetSize(width, height);
}

LightMapper::~LightMapper()
{
	glDeleteFramebuffers(1, &shadowMapFBO);
	glDeleteTextures(1, &shadowMapTexture);
}

void LightMapper::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glViewport(0, 0, mapWidth, mapHeight);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void LightMapper::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#include <fstream>

#include <fstream>

void LightMapper::SaveDepthAsBMP(std::string path)
{
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    float* data = new float[mapWidth * mapHeight];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);

    unsigned char* data2 = new unsigned char[mapWidth * mapHeight];
    for (int i = 0; i < mapWidth * mapHeight; i++)
    {
        data2[i] = (unsigned char)(data[i] * 255.0f);
    }

    std::ofstream file(path, std::ios::binary);
    if (file.is_open())
    {
        // Write BMP header
        unsigned char header[54] = { 'B', 'M' };
        int fileSize = 54 + mapWidth * mapHeight;
        header[2] = (unsigned char)(fileSize & 0xff);
        header[3] = (unsigned char)((fileSize >> 8) & 0xff);
        header[4] = (unsigned char)((fileSize >> 16) & 0xff);
        header[5] = (unsigned char)((fileSize >> 24) & 0xff);
        header[10] = 54;
        header[14] = 40;
        header[18] = (unsigned char)(mapWidth & 0xff);
        header[19] = (unsigned char)((mapWidth >> 8) & 0xff);
        header[20] = (unsigned char)((mapWidth >> 16) & 0xff);
        header[21] = (unsigned char)((mapWidth >> 24) & 0xff);
        header[22] = (unsigned char)(mapHeight & 0xff);
        header[23] = (unsigned char)((mapHeight >> 8) & 0xff);
        header[24] = (unsigned char)((mapHeight >> 16) & 0xff);
        header[25] = (unsigned char)((mapHeight >> 24) & 0xff);
        header[26] = 1;
        header[28] = 8;
        file.write((char*)header, sizeof(header));

        // Write color table
        unsigned char colorTable[1024];
        for (int i = 0; i < 256; i++)
        {
            colorTable[i * 4 + 0] = i;
            colorTable[i * 4 + 1] = i;
            colorTable[i * 4 + 2] = i;
            colorTable[i * 4 + 3] = 0;
        }
        file.write((char*)colorTable, sizeof(colorTable));

        // Write pixel data
        int rowSize = ((mapWidth + 3) / 4) * 4;
        unsigned char* rowData = new unsigned char[rowSize];
        for (int y = mapHeight - 1; y >= 0; y--)
        {
            memcpy(rowData, &data2[y * mapWidth], mapWidth);
            file.write((char*)rowData, rowSize);
        }

        delete[] rowData;

        file.close();
    }

    delete[] data;
    delete[] data2;
}


void LightMapper::SetUniforms(Shader* shader)
{
	shader->SetInt("shadowMap", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
}

void LightMapper::SetSize(int width, int height)
{
	if (width == mapWidth && height == mapHeight)
		return;

	mapWidth = width;
	mapHeight = height;

	Update();
}

void LightMapper::Update()
{
	glGenTextures(1, &shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapWidth, mapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}