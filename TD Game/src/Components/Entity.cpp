#include "Entity.h"
#include "Tools.h"

#include <gl/glew.h>
#include <iostream>
#include <Mesh.h>

Entity::Entity(std::string m_Name) {
    this->m_Name = m_Name;

    m_Transform.m_Position = glm::vec3(0.0f, -1.0f, 0.0f);
}

Entity::~Entity() {
    for (Component* component : m_Components) {
		delete component;
	}
}
