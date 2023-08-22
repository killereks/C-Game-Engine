//
// Created by killereks on 21/08/2023.
//

#pragma once

#include "Components/Transform.h"

class Camera {
public:
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    Transform m_Transform;
};
