
// render entity, everything that is needed to
// render the entity

#pragma once

#include "Transform.h"
#include "../Shader.h"
#include "../Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include <typeinfo>
#include "Component.h"

class Component;

class Entity {
public:
	Entity(std::string name);
	~Entity();

    Transform m_Transform;

    std::string Name() { return m_Name; }

    std::vector<Component*> m_Components;

    template<typename T>
    T* AddComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        T* component = new T();
        component->m_Owner = this;
        m_Components.push_back(component);

        component->Init();

        return component;
    }

    template<typename T>
    T* GetComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        for (Component* component : m_Components) {
            if (T* result = dynamic_cast<T*>(component)) {
                return result;
            }
        }

        return nullptr;
    }

    template<typename T>
    bool TryGetComponent(T*& component) {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        for (Component* comp : m_Components) {
            if (T* result = dynamic_cast<T*>(comp)) {
                component = result;
                return true;
            }
        }

        return false;
    }

    template<typename T>
    void RemoveComponent(T component) {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        for (int i = 0; i < m_Components.size(); i++) {
            if (T* result = dynamic_cast<T*>(m_Components[i])) {
                m_Components.erase(m_Components.begin() + i);
                return;
            }
        }
    }

private:
	glm::vec2 m_Position;
	float m_Rotation;
	glm::vec2 m_Scale;

    std::string m_Name;

    // list of vertices
    std::vector<float> m_Vertices;

    std::vector<unsigned int> m_Indices;

	// opengl buffers, we just need indices
	// and vertex positions
	unsigned int m_VBO;

    // index buffer object
    unsigned int m_IBO;
};