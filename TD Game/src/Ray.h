#pragma once

#include "vec3.hpp"

class Ray
{
	public:
	Ray(glm::vec3 origin, glm::vec3 direction);
	~Ray();

	glm::vec3 m_Origin;
	glm::vec3 m_Direction;

	glm::vec3 GetPoint(float distance);
};

