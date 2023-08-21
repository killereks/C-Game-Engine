#include "entity.h"
#include "../Tools.h"

#include <gl/glew.h>
#include <iostream>

Entity::Entity() {
    // create quad
    m_Vertices.push_back(-0.5f);
    m_Vertices.push_back(0.0f);
    m_Vertices.push_back(-0.5f);

    m_Vertices.push_back(0.5f);
    m_Vertices.push_back(0.0f);
    m_Vertices.push_back(-0.5f);

    m_Vertices.push_back(0.5f);
    m_Vertices.push_back(0.0f);
    m_Vertices.push_back(0.5f);

    m_Vertices.push_back(-0.5f);
    m_Vertices.push_back(0.0f);
    m_Vertices.push_back(0.5f);

    m_Indices.push_back(2);
    m_Indices.push_back(1);
    m_Indices.push_back(0);

    m_Indices.push_back(0);
    m_Indices.push_back(3);
    m_Indices.push_back(2);

    m_Transform.m_Position = glm::vec3(0.0f, -1.0f, 0.0f);
    m_Transform.m_Scale = glm::vec3(50.0f);

	glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float), &m_Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
}

Entity::~Entity() {
}

void Entity::Render(Camera* camera, Shader* shader) {
	shader->Bind();

	// model matrix
	/*glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(m_Position, 0.0f));
	model = glm::rotate(model, m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(m_Scale, 0.1f));*/

	// set model matrix
	shader->SetMat4("model", m_Transform.GetModelMatrix());
    shader->SetMat4("view", camera->GetViewMatrix());
    shader->SetMat4("projection", camera->GetProjectionMatrix());

	// drawing

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader->Unbind();
}