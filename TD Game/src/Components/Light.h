#pragma once

#include "Component.h"
#include "vec3.hpp"

class Light : public Component {
	public:
	glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_Intensity = 1.0f;
	float m_Range = 10.0f;

	Light();

	void Init() override;
	void Update(float dt) override;

	std::string GetName() override;
	void DrawInspector() override;
	void DrawGizmos() override;

	void Save(std::ostream& os) override;
	void Load(std::istream& is) override;

	ComponentType GetType() override;
};

