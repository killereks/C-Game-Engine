//
// Created by killereks on 21/08/2023.
//


#pragma once

//#include "Entity.h"

#include <iostream>

enum class ComponentType {
	None,
	Mesh,
	Light
};

class Entity;

class Component {
public:
	virtual void Update(float m_DeltaTime) = 0;
	virtual void Init() = 0;

	virtual std::string GetName() = 0;
	virtual void DrawInspector() = 0;
	virtual void DrawGizmos() = 0;

	virtual void Save(std::ostream& os) = 0;
	virtual void Load(std::istream& is) = 0;

	virtual ComponentType GetType() = 0;

	static Component* CreateComponent(ComponentType type);

	Entity* m_Owner;
};
