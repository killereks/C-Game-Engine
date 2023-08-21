#include "Shader.h"

#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "Tools.h"

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

void Shader::LoadFromFiles(std::string vertexPath, std::string fragmentPath) {
    std::cout << "Loading shader from files..." << std::endl;

    std::string vertex = Tools::OpenFile(vertexPath);
    std::string fragment = Tools::OpenFile(fragmentPath);

	m_RendererID = CreateShader(vertex, fragment);
}

Shader::~Shader() {
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() {
    if (glIsProgram(m_RendererID) == GL_FALSE) {
        std::cout << "Program is not valid!" << std::endl;
    }

	glUseProgram(m_RendererID);
    Tools::GLErrorCheck("Use Program");
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

		std::vector<GLchar> message(length);
		glGetShaderInfoLog(id, length, &length, &message[0]);

		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
		std::cout << &message[0] << std::endl;

		glDeleteShader(id);

		return 0;
	}

    std::cout << "Shader compiled successfully!" << std::endl;

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    std::cout << "Creating shader..." << std::endl;

    std::cout << "Vertex: " << vertexShader << std::endl;
    std::cout << "Fragment: " << fragmentShader << std::endl;

	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*) &isLinked);
    if (isLinked == GL_FALSE){
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);

        std::cout << "Shader failed to link!" << std::endl;
        std::cout << &infoLog[0] << std::endl;

        return 0;
    }

    glDetachShader(program, vs);
    glDetachShader(program, fs);

	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int Shader::GetUniformLocation(const std::string& name) {
	//if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		//return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	//m_UniformLocationCache[name] = location;
	return location;
}