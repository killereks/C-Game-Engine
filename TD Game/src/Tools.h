//
// Created by killereks on 21/08/2023.
//

#ifndef CPP_GAME_ENGINE_TOOLS_H
#define CPP_GAME_ENGINE_TOOLS_H


#include <string>
#include "glm.hpp"

class Tools {
    public:
        static std::string OpenFile(std::string path);
        static void GLErrorCheck(std::string label);
        static void PrintMatrix(glm::mat4 matrix);
        static void PrintVector(glm::vec3 vector);
};


#endif //CPP_GAME_ENGINE_TOOLS_H
