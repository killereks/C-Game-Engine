//
// Created by killereks on 21/08/2023.
//

#include "Camera.h"

glm::mat4 Camera::GetViewMatrix() {
    return glm::inverse(m_Transform.GetModelMatrix());
}

Camera::Camera(float fov, float near, float far) {
	m_FOV = fov;
	m_Near = near;
	m_Far = far;

	m_IsOrtho = false;
}

Camera::Camera(float left, float right, float bottom, float top, float near, float far) {
	m_Left = left;
	m_Right = right;
	m_Bottom = bottom;
	m_Top = top;
	m_OrthoNear = near;
	m_OrthoFar = far;
	m_IsOrtho = true;
}

glm::mat4 Camera::GetProjectionMatrix() {
	if (m_IsOrtho) {
		return glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_OrthoNear, m_OrthoFar);
	}
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
