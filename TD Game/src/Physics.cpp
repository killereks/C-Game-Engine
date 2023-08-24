#include "Physics.h"
#include <common.hpp>


Physics::Physics() {

}

Physics::~Physics() {

}

bool Physics::RayCollisionAABB(Ray ray, Bounds bounds)
{
	glm::vec3 dirfrac;

	dirfrac.x = 1.0f / ray.m_Direction.x;
	dirfrac.y = 1.0f / ray.m_Direction.y;
	dirfrac.z = 1.0f / ray.m_Direction.z;

	float t1 = (bounds.GetMin().x - ray.m_Origin.x)*dirfrac.x;
	float t2 = (bounds.GetMax().x - ray.m_Origin.x)*dirfrac.x;
	float t3 = (bounds.GetMin().y - ray.m_Origin.y)*dirfrac.y;
	float t4 = (bounds.GetMax().y - ray.m_Origin.y)*dirfrac.y;
	float t5 = (bounds.GetMin().z - ray.m_Origin.z)*dirfrac.z;
	float t6 = (bounds.GetMax().z - ray.m_Origin.z)*dirfrac.z;

	float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
	float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

	if (tmax < 0)
	{
		return false;
	}

	if (tmin > tmax)
	{
		return false;
	}

	return true;
}

