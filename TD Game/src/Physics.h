#pragma once

#include <ext/vector_float3.hpp>
#include <Ray.h>
#include <Bounds.h>

class Physics
{
	public:
	Physics();
	~Physics();

	static bool RayCollisionAABB(Ray ray, Bounds bounds);
};

struct RaycastHit
{
	glm::vec3 m_Point;
	glm::vec3 m_Normal;
	float m_Distance;
};