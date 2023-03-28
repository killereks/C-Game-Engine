#include "entity.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Entity::Entity() {
	m_Position = glm::vec2(0.0f, 0.0f);
	m_Rotation = 0.0f;
	m_Scale = glm::vec2(1.0f, 1.0f);

	float vertices[] = {
		-0.2f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	glGenBuffers(1, &m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

Entity::~Entity() {
}

void Entity::SetPosition(glm::vec2 position) {
	m_Position = position;
}

void Entity::SetPosition(float x, float y) {
	m_Position = glm::vec2(x, y);
}

void Entity::SetRotation(float rotation) {
	m_Rotation = rotation;
}

void Entity::SetScale(glm::vec2 scale) {
	m_Scale = scale;
}

void Entity::SetScale(float x, float y) {
	m_Scale = glm::vec2(x, y);
}

glm::vec2 Entity::GetPosition() {
	return m_Position;
}

float Entity::GetRotation() {
	return m_Rotation;
}

glm::vec2 Entity::GetScale() {
	return m_Scale;
}

void Entity::Render(Shader shader) {
	shader.Bind();

	// model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(m_Position, 0.0f));
	model = glm::rotate(model, m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(m_Scale, 0.1f));

	// set model matrix
	shader.SetMat4("model", model);

	// drawing
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.Unbind();
}