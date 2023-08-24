#include "Ray.h"

Ray::Ray(glm::vec3 origin, glm::vec3 direction) {
	m_Origin = origin;
	m_Direction = direction;
}

Ray::~Ray() {

}

glm::vec3 Ray::GetPoint(float distance) {
	return m_Origin + m_Direction * distance;
}