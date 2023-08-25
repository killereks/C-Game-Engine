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

void Entity::Save(std::ostream& os)
{
    m_Transform.Save(os);

    // save the name
    os.write(m_Name.c_str(), m_Name.size() + 1);

    size_t numComponents = m_Components.size();
    os.write((char*)&numComponents, sizeof(size_t));

    for (Component* component : m_Components) {
        int type = (int)component->GetType();

		os.write((char*)&type, sizeof(int));

		component->Save(os);
	}
}

void Entity::Load(std::istream& is)
{
    m_Transform.Load(is);

    // load the name
    std::getline(is, m_Name, '\0');

    size_t numComponents;
    is.read((char*)&numComponents, sizeof(size_t));

    std::cout << "Attempting to load " << numComponents << " components on entity " << m_Name << "\n";

    if (numComponents > 1000) return;

    m_Components.clear();

    for (size_t i = 0; i < numComponents; i++) {
        int type;
        is.read((char*)&type, sizeof(int));

        Component* component = Component::CreateComponent((ComponentType)type);

        component->m_Owner = this;
        component->Load(is);

        m_Components.push_back(component);
    }
}
