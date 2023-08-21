//
// Created by killereks on 21/08/2023.
//

#include <GL/glew.h>
#include "Mesh.h"
#include "geometric.hpp"
//#include "ofbx.h"

Mesh Mesh::LoadFromFile(std::string path) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

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
}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices) {
    m_Vertices = vertices;
    m_UVs = uvs;
    m_Indices = indices;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);
    glGenBuffers(1, &m_NBO);
}

Mesh Mesh::CreateCube(glm::vec3 size) {

}

Mesh Mesh::CreateSphere(float radius, int rings, int sectors) {

}

Mesh Mesh::CreatePlane(glm::vec2 size) {

}

// binds buffers, and updates the information
void Mesh::Update() {
    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(glm::vec3), &m_Vertices[0], GL_STATIC_DRAW);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    // UVs
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_UVs.size() * sizeof(glm::vec2), &m_UVs[0], GL_STATIC_DRAW);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(glm::vec3), &m_Normals[0], GL_STATIC_DRAW);
}
