//
// Created by killereks on 21/08/2023.
//

#pragma once

#include "Components/Transform.h"
#include <Ray.h>
#include <vector>

class Camera {
public:
    // perspective
    Camera(float fov, float near, float far);
    // ortho
    Camera(float left, float right, float bottom, float top, float near, float far);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    Transform m_Transform;

    Ray ScreenPointToRay(glm::vec2 point, glm::vec2 screenRes);

    float m_FOV = 60.0f;
    float m_Near = 0.1f;
    float m_Far = 1000.0f;

    float m_Left = -1.0f;
    float m_Right = 1.0f;
    float m_Bottom = -1.0f;
    float m_Top = 1.0f;
    float m_OrthoNear = 0.1f;
    float m_OrthoFar = 1000.0f;

    bool m_IsOrtho = false;
};
