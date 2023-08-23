//
// Created by killereks on 21/08/2023.
//

#pragma once

#include "Component.h"
#include "vec3.hpp"
#include "ext/quaternion_float.hpp"

class Transform {
public:
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);

    glm::mat4 GetModelMatrix();

    glm::vec3 Forward();
    glm::vec3 Right();
    glm::vec3 Up();

    void Translate(glm::vec3 translation);

    std::string GetName();
    void DrawInspector();

    void SetRotationEuler(glm::vec3 euler);
    glm::vec3 GetEuler();
};
