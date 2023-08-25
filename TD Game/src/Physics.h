#pragma once

#include <ext/vector_float3.hpp>
#include <Ray.h>
#include <Bounds.h>

class Physics
{
	public:
	Physics();
	~Physics();

	static bool RayCollisionAABB(Ray ray, Bounds bounds, float & distance);
};