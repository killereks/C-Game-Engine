//
// Created by killereks on 21/08/2023.
//


#pragma once

//#include "Entity.h"

class Entity;

class Component {
public:
	virtual void Update(float m_DeltaTime) = 0;
	virtual void Init() = 0;

	Entity* m_Owner;
};
