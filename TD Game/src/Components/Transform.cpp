//
// Created by killereks on 21/08/2023.
//

#include <iostream>
#include "Transform.h"
#include "gtx/quaternion.hpp"

glm::mat4 Transform::GetModelMatrix() {
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, m_Position) * glm::toMat4(m_Rotation) * glm::scale(modelMatrix, m_Scale);

    return modelMatrix;
}

glm::vec3 Transform::Forward() {
    return m_Rotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 Transform::Right() {
    return m_Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Transform::Up() {
    return m_Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

void Transform::LookAt(glm::vec3 position)
{
    float dx = position.x - m_Position.x;
    float dz = position.z - m_Position.z;

    if (dx == 0.0f && dz == 0.0f) {
		return;
	}

    glm::mat4 lookMat4 = glm::lookAt(m_Position, position, glm::vec3(0.0f, 1.0f, 0.0f));

    m_Rotation = glm::quat_cast(lookMat4);
    //m_Rotation = glm::inverse(m_Rotation);
}

void Transform::Translate(glm::vec3 translation) {
    m_Position += translation;
}

void Transform::Save(std::ostream& os)
{
    os << m_Position.x << " " << m_Position.y << " " << m_Position.z << std::endl;
    os << m_Rotation.x << " " << m_Rotation.y << " " << m_Rotation.z << " " << m_Rotation.w << std::endl;
    os << m_Scale.x << " " << m_Scale.y << " " << m_Scale.z << std::endl;
}

void Transform::Load(std::istream& is)
{
    is >> m_Position.x >> m_Position.y >> m_Position.z;
	is >> m_Rotation.x >> m_Rotation.y >> m_Rotation.z >> m_Rotation.w;
	is >> m_Scale.x >> m_Scale.y >> m_Scale.z;
}

void Transform::SetRotationEuler(glm::vec3 euler) {
    euler = euler * (3.141592653589793238463f / 180.0f);

    m_Rotation = glm::quat(euler);
}

glm::vec3 Transform::GetEuler() {
    glm::vec3 euler = glm::eulerAngles(m_Rotation) * (180.0f / 3.141592653589793238463f);

    if (euler.x < 0.0f) {
        euler.x += 360.0f;
    }

    if (euler.y < 0.0f) {
        euler.y += 360.0f;
    }

    if (euler.z < 0.0f) {
        euler.z += 360.0f;
    }

    return euler;
}