//
// Created by killereks on 21/08/2023.
//

#pragma once

#include <GL/glew.h>
#include "Mesh.h"
#include "geometric.hpp"
#include <Camera.h>
#include <Shader.h>
#include <iostream>
#include "Entity.h"
#include "imgui.h"
#include <fstream>

#include <OpenFBX/ofbx.h>

#include "Icons/IconsFontAwesome5.h"

void Mesh::Update(float dt) {

}

void Mesh::Init() {

}

Mesh::Mesh() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);
    glGenBuffers(1, &m_NBO);
    glGenBuffers(1, &m_UBO);

    m_Bounds = new Bounds();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);
    glDeleteBuffers(1, &m_NBO);
    glDeleteBuffers(1, &m_UBO);

    delete m_Bounds;
}

Bounds Mesh::GetBounds()
{
    // returns bounds in world space
    glm::vec3 pos = m_Owner->m_Transform.m_Position;
    glm::vec3 size = m_Bounds->GetSize() / 2.0f * m_Owner->m_Transform.m_Scale;

    return Bounds(pos, size);
}

void Mesh::LoadFromFile(std::string path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    auto flags = ofbx::LoadFlags::TRIANGULATE | ofbx::LoadFlags::IGNORE_CAMERAS;

    ofbx::IScene* scene = ofbx::load((ofbx::u8*)buffer.data(), size, (ofbx::u64) flags);

    if (scene == nullptr) {
        std::cout << "Failed to load FBX file at " << path << std::endl;
        return;
    }

    std::vector<glm::vec3> verts;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> norms;
    std::vector<unsigned int> inds;

    unsigned int indexOffset = 0;

    int meshCount = scene->getMeshCount();
    for (int i = 0; i < meshCount; i++) {
        const ofbx::Mesh* mesh = scene->getMesh(i);

        const ofbx::Geometry* geom = mesh->getGeometry();
        const ofbx::Vec3* vertices = geom->getVertices();
        const ofbx::Vec2* uvs = geom->getUVs();
        const ofbx::Vec3* normals = geom->getNormals();

        int vertexCount = geom->getVertexCount();
        const int* indices = geom->getFaceIndices();

        for (int j = 0; j < vertexCount; j++) {
            verts.push_back(glm::vec3(vertices[j].x, vertices[j].y, vertices[j].z));
            uv.push_back(glm::vec2(uvs[j].x, uvs[j].y));
            norms.push_back(glm::vec3(normals[j].x, normals[j].y, normals[j].z));
        }

        if (geom->getIndexCount() % 3 != 0) {
			std::cout << "Invalid index count" << std::endl;
			return;
		}

        for (int j = 0; j < geom->getIndexCount(); j++) {
            inds.push_back(indices[j] + indexOffset);
        }

        indexOffset += vertexCount;

        m_Vertices = verts;
        m_UVs = uv;
        m_Normals = norms;
        m_Indices = inds;

        UpdateBuffers();
    }
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

    RecalculateNormals();
    UpdateBuffers();
}

void Mesh::CreateSphere(float radius, int rings, int sectors) {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    float const R = 1.0f / static_cast<float>(rings - 1);
    float const S = 1.0f / static_cast<float>(sectors - 1);

    const float M_PI = 3.14159265359f;
    const float M_PI_2 = 1.57079632679f;

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < sectors; ++s) {
            float const y = sin(-M_PI_2 + M_PI * r * R);
            float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
            float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

            verts.push_back(glm::vec3(x, y, z) * radius);
            uvs.push_back(glm::vec2(s * S, r * R));
        }
    }

    for (int r = 0; r < rings - 1; ++r) {
        for (int s = 0; s < sectors - 1; ++s) {
            indices.push_back(r * sectors + (s + 1));
            indices.push_back(r * sectors + s);
            indices.push_back((r + 1) * sectors + (s + 1));

            indices.push_back((r + 1) * sectors + (s + 1));
            indices.push_back(r * sectors + s);
            indices.push_back((r + 1) * sectors + s);
        }
    }

    this->m_Vertices = verts;
    this->m_UVs = uvs;
    this->m_Indices = indices;

    RecalculateNormals();
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

    RecalculateNormals();
    UpdateBuffers();
}

// binds buffers, and updates the information
void Mesh::UpdateBuffers() {
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

    m_Bounds->Clear();

    for (auto& v : m_Vertices) {
        m_Bounds->InsertPoint(v);
    }
}

void Mesh::DrawGizmos() {

}

void Mesh::SetupRender(Camera* camera, Shader* shader) {
    shader->Bind();

    shader->SetMat4("model", m_Owner->m_Transform.GetModelMatrix());
    shader->SetMat4("view", camera->GetViewMatrix());
    shader->SetMat4("projection", camera->GetProjectionMatrix());
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

std::string Mesh::GetName() {
	return ICON_FA_CUBE" Mesh";
}

void Mesh::DrawInspector() {
    ImGui::Text("Vertices: %d", m_Vertices.size());
    ImGui::Text("UVs: %d", m_UVs.size());
    ImGui::Text("Indices: %d", m_Indices.size());

    ImGui::Text("Normals: %d", m_Normals.size());

    Bounds bounds = GetBounds();

    ImGui::Text("Bounds");
    ImGui::Text("Min: %f, %f, %f", bounds.GetMin().x, bounds.GetMin().y, bounds.GetMin().z);
    ImGui::Text("Max: %f, %f, %f", bounds.GetMax().x, bounds.GetMax().y, bounds.GetMax().z);
    ImGui::Text("Center: %f, %f, %f", bounds.GetCenter().x, bounds.GetCenter().y, bounds.GetCenter().z);
    ImGui::Text("Size: %f, %f, %f", bounds.GetSize().x, bounds.GetSize().y, bounds.GetSize().z);

    if (ImGui::Button("Create Cube")) {
		CreateCube(glm::vec3(1, 1, 1));
	}
    if (ImGui::Button("Create Sphere")) {
		CreateSphere(1, 32, 32);
	}

    if (ImGui::Button("Create Plane")) {
		CreatePlane(glm::vec2(1, 1));
	}
}