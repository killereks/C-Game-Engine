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

Ray Camera::ScreenPointToRay(glm::vec2 point, glm::vec2 screenRes){
	point.y = screenRes.y - point.y;

    glm::vec2 normalizedPoint = glm::vec2(point.x / screenRes.x, point.y / screenRes.y);

	glm::vec2 clipSpace = glm::vec2(normalizedPoint.x * 2.0f - 1.0f, normalizedPoint.y * 2.0f - 1.0f);
	glm::vec4 eyeSpace = glm::vec4(clipSpace.x, clipSpace.y, -1.0f, 1.0f);
	glm::vec4 worldSpace = glm::inverse(GetProjectionMatrix()) * eyeSpace;

	worldSpace = glm::vec4(worldSpace.x, worldSpace.y, -1.0f, 0.0f);

	glm::vec3 direction = glm::normalize(glm::vec3(m_Transform.GetModelMatrix() * worldSpace));

	return Ray(m_Transform.m_Position, direction);
}