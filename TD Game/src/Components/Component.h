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

	Entity* m_Owner;
};
