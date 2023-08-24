#pragma once

#include <ext/vector_float3.hpp>
#include <vector>

class Bounds
{
	public:
	Bounds();
	Bounds(glm::vec3 pos, glm::vec3 halfSize);
	~Bounds();

	void Clear();

	void InsertPoint(glm::vec3 point);

	glm::vec3 GetCenter();
	glm::vec3 GetSize();

	glm::vec3 GetMin();
	glm::vec3 GetMax();

	glm::vec3 ClosestPoint(glm::vec3 point);
	bool Intersects(Bounds bounds);
	bool Contains(glm::vec3 point);

private:
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;
	glm::vec3 size;

	std::vector<glm::vec3> m_Points;
};

