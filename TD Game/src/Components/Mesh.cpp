//
// Created by killereks on 21/08/2023.
//

#include <GL/glew.h>
#include "Mesh.h"
#include "geometric.hpp"
#include <Camera.h>
#include <Shader.h>
#include <iostream>
#include "Entity.h"
//#include "ofbx.h"

void Mesh::Update(float dt) {

}

void Mesh::Init() {

}

void Mesh::LoadFromFile(std::string path) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    std::cout << "Not implemented yet" << std::endl;
}

void Mesh::RecalculateNormals() {
    m_Normals.clear();

    for (int i = 0; i < m_Indices.size(); i += 3){
        unsigned int triangle0 = m_Indices[i];
        unsigned int triangle1 = m_Indices[i + 1];
        unsigned int triangle2 = m_Indices[i + 2];

        glm::vec3 v0 = m_Vertices[triangle0];
        glm::vec3 v1 = m_Vertices[triangle1];
        glm::vec3 v2 = m_Vertices[triangle2];

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        m_Normals.push_back(normal);
    }

    UpdateBuffers();
}

Mesh::Mesh() {

}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices) {
    m_Vertices = vertices;
    m_UVs = uvs;
    m_Indices = indices;

    UpdateBuffers();
}

void Mesh::CreateCube(glm::vec3 size) {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    verts.push_back(glm::vec3(-size.x, -size.y, -size.z));
    verts.push_back(glm::vec3(-size.x, -size.y, size.z));
    verts.push_back(glm::vec3(size.x, -size.y, size.z));
    verts.push_back(glm::vec3(size.x, -size.y, -size.z));
    verts.push_back(glm::vec3(-size.x, size.y, -size.z));
    verts.push_back(glm::vec3(-size.x, size.y, size.z));
    verts.push_back(glm::vec3(size.x, size.y, size.z));
    verts.push_back(glm::vec3(size.x, size.y, -size.z));

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(4);
    indices.push_back(6);
    indices.push_back(7);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(0);
    indices.push_back(5);
    indices.push_back(4);

    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(2);
    indices.push_back(7);
    indices.push_back(6);

    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(1);
    indices.push_back(6);
    indices.push_back(5);

    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(0);
    indices.push_back(7);
    indices.push_back(4);

    this->m_Vertices = verts;
    this->m_UVs = uvs;
    this->m_Indices = indices;

    UpdateBuffers();
}

void Mesh::CreateSphere(float radius, int rings, int sectors) {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    this->m_Vertices = verts;
    this->m_UVs = uvs;
    this->m_Indices = indices;

    UpdateBuffers();
}

void Mesh::CreatePlane(glm::vec2 size) {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    verts = {
			glm::vec3(-size.x, 0, -size.y),
			glm::vec3(-size.x, 0, size.y),
			glm::vec3(size.x, 0, size.y),
			glm::vec3(size.x, 0, -size.y)
	};

    uvs = {
        glm::vec2(0, 0),
        glm::vec2(0, 1),
        glm::vec2(1, 1),
        glm::vec2(1, 0)
    };

    indices = {
        0, 1, 2,
        0, 2, 3
    };

    this->m_Vertices = verts;
    this->m_UVs = uvs;
    this->m_Indices = indices;

    std::cout << "Lengths: " << verts.size() << ", " << uvs.size() << ", " << indices.size() << "\n";

    UpdateBuffers();
}

// binds buffers, and updates the information
void Mesh::UpdateBuffers() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);
    glGenBuffers(1, &m_NBO);
    glGenBuffers(1, &m_UBO);

    std::cout << "Vertices length: " << m_Vertices.size() << "\n";

    if (m_Vertices.size() > 0) {
        // Vertices
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(glm::vec3), &m_Vertices[0], GL_STATIC_DRAW);
    }

    std::cout << "Indices length: " << m_Indices.size() << "\n";

    if (m_Indices.size() > 0) {
        // Indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
    }

    std::cout << "UVs length: " << m_UVs.size() << "\n";

    if (m_UVs.size() > 0) {
        // UVs
        glBindBuffer(GL_ARRAY_BUFFER, m_UBO);
        glBufferData(GL_ARRAY_BUFFER, m_UVs.size() * sizeof(glm::vec2), &m_UVs[0], GL_STATIC_DRAW);
    }

    std::cout << "Normals length: " << m_Normals.size() << "\n";

    if (m_Normals.size() > 0) {
        // Normals
        glBindBuffer(GL_ARRAY_BUFFER, m_NBO);
        glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(glm::vec3), &m_Normals[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Render(Camera* camera, Shader* shader) {
    shader->Bind();

    shader->SetMat4("model", m_Owner->m_Transform.GetModelMatrix());
    shader->SetMat4("view", camera->GetViewMatrix());
    shader->SetMat4("projection", camera->GetProjectionMatrix());

    Draw();

    shader->Unbind();
}

void Mesh::Draw() {
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    if (m_UVs.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_UBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

    if (m_Normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_NBO);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }
    
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);
}