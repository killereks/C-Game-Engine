#include "Light.h"
#include <imgui.h>

Light::Light() {

}

std::string Light::GetName() {
	return "Light";
}

void Light::Init() {

}

void Light::Update(float dt) {

}

void Light::DrawInspector() {
	ImGui::ColorEdit3("Color", &m_Color[0]);
	ImGui::DragFloat("Intensity", &m_Intensity, 0.1f, 0.0f, 1000.0f);
	ImGui::DragFloat("Range", &m_Range, 0.1f, 0.0f, 1000.0f);
}
