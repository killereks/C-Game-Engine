#pragma once

#include <string>

#include <glm/glm.hpp>

#include <unordered_map>

class Shader {
public:
	Shader();
	Shader(std::string vertex, std::string fragment);
	~Shader();

	void Bind();
	void Unbind();

	void LoadFromFiles(std::string vertex, std::string fragment);

	void SetFloat(std::string name, float value);
	void SetInt(std::string name, int value);
	void SetVec3(std::string name, float x, float y, float z);
	void SetVec3(std::string name, glm::vec3 value);
	void SetMat4(std::string name, glm::mat4 value);

    unsigned int GetID() const { return m_RendererID; }

	private:
		unsigned int m_RendererID;

		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		std::string ReadFile(const std::string& filepath);

		int GetUniformLocation(const std::string& name);

		// cache for uniforms
		std::unordered_map<std::string, int> m_UniformLocationCache;
};