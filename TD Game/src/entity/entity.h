
// render entity, everything that is needed to
// render the entity

#include "../Shader.h"
#include "../Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include <typeinfo>

class Entity {
public:
	Entity(std::string name);
	~Entity();

    Transform m_Transform;

	void Render(Camera* camera, Shader* shader);

    std::string Name() { return m_Name; }

    std::vector<Component> m_Components;

    template<class T>
    void AddComponent() {
        m_Components.push_back(T());
    }

    template<class T>
    T* GetComponent() {
        for (T &component: m_Components) {
            if (typeid(component) == typeid(T)) {
                return (T *) &component;
            }
        }
    }

    template<class T>
    bool TryGetComponent(T* component) {
        for (T &comp: m_Components) {
            if (typeid(comp) == typeid(component)) {
                component = &comp;
                return true;
            }
        }

        return false;
    }

    template<class T>
    void RemoveComponent(T component) {
        for (int i = 0; i < m_Components.size(); i++) {
            if (typeid(m_Components[i]) == typeid(component)) {
                m_Components.erase(m_Components.begin() + i);
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