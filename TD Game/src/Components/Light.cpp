#include "Light.h"
#include <imgui.h>
#include "Icons/IconsFontAwesome5.h"

#include "Entity.h"
#include <GL/glew.h>

Light::Light() {

}

std::string Light::GetName() {
	return ICON_FA_LIGHTBULB" Light";
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

void Light::DrawGizmos() {

}
