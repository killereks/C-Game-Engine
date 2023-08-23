//
// Created by killereks on 21/08/2023.
//

#include "Camera.h"

glm::mat4 Camera::GetViewMatrix() {
    return glm::inverse(m_Transform.GetModelMatrix());
}

glm::mat4 Camera::GetProjectionMatrix() {
    return glm::perspective(glm::radians(m_FOV), 4.0f / 3.0f, m_Near, m_Far);
}