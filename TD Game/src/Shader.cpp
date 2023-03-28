#include "Shader.h"

#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
		abort();
	}
}

#ifdef _DEBUG
#define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
#define GL_CHECK(stmt) stmt
#endif

Shader::Shader() {

}

Shader::Shader(std::string vertex, std::string fragment) {
	m_RendererID = CreateShader(vertex, fragment);
}

void Shader::LoadFromFiles(std::string vertex, std::string fragment) {
	m_RendererID = CreateShader(vertex, fragment);
}

Shader::~Shader() {
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() {
	glUseProgram(m_RendererID);
}

void Shader::Unbind() {
	glUseProgram(0);
}

void Shader::SetFloat(std::string name, float value) {
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetInt(std::string name, int value) {
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetVec3(std::string name, float x, float y, float z) {
	glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetVec3(std::string name, glm::vec3 value) {
	glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void Shader::SetMat4(std::string name, glm::mat4 value) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

std::string Shader::ReadFile(const std::string& filepath) {
	std::cout << "Reading file: " << filepath << std::endl;

	std::string output;
	std::string line;

	std::ifstream fileStream(filepath);

	while (std::getline(fileStream, line)) {
		output += line + "\n";
	}

	fileStream.close();

	return output;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);

		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	std::string vertex_src = ReadFile(vertexShader);
	std::string fragment_src = ReadFile(fragmentShader);

	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex_src);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment_src);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int Shader::GetUniformLocation(const std::string& name) {
	//if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		//return m_UniformLocationCache[name];

	std::cout << "attempting to fetch uniform location for " << name << std::endl;

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	//m_UniformLocationCache[name] = location;
	return location;
}