//
// Created by killereks on 21/08/2023.
//

#include <fstream>
#include <GL/glew.h>
#include <iostream>
#include "Tools.h"

std::string Tools::OpenFile(std::string path) {
    // opens file and returns string contents
    std::ifstream file(path);

    std::string output;

    std::string line;

    while (std::getline(file, line)) {
        output += line + "\n";
    }

    file.close();

    return output;
}

void Tools::GLErrorCheck(std::string label){
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << label << " ";
        switch (err){
            case GL_INVALID_ENUM:
                std::cout << "GL_INVALID_ENUM" << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cout << "GL_INVALID_VALUE" << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cout << "GL_INVALID_OPERATION" << std::endl;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cout << "GL_OUT_OF_MEMORY" << std::endl;
                break;
            case GL_STACK_UNDERFLOW:
                std::cout << "GL_STACK_UNDERFLOW" << std::endl;
                break;
            case GL_STACK_OVERFLOW:
                std::cout << "GL_STACK_OVERFLOW" << std::endl;
                break;
            default:
                std::cout << "Unknown Error" << std::endl;
                break;
        }
    }
}

void Tools::PrintMatrix(glm::mat4 matrix) {
    for (int i = 0; i < 4; i++) {
        std::cout << "[";
        for (int j = 0; j < 4; j++) {
            std::cout << matrix[i][j] << ", ";
        }
        std::cout << "]" << std::endl;
    }
}

void Tools::PrintVector(glm::vec3 vector) {
    std::cout << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]" << std::endl;
}