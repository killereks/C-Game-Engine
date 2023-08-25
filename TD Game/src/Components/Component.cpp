#include "Component.h"

#include "Mesh.h"
#include "Light.h"

//
// Created by killereks on 21/08/2023.
//

Component* Component::CreateComponent(ComponentType type)
{
	switch (type)
	{
		case ComponentType::Mesh:
			return new Mesh();
		case ComponentType::Light:
			return new Light();

		default:
			std::cerr << "Component::CreateComponent: Unknown component type" << std::endl;
	}
}
