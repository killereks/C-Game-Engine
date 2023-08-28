//
// Created by killereks on 21/08/2023.
//

#ifndef CPP_GAME_ENGINE_MESH_H
#define CPP_GAME_ENGINE_MESH_H


#include <string>
#include <vector>
#include "Component.h"
#include "vec3.hpp"
#include "vec2.hpp"
#include "../Camera.h"
#include "../Shader.h"
#include "../Bounds.h"

class Mesh : public Component {
    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec2> m_UVs;
    std::vector<glm::vec3> m_Normals;
    std::vector<unsigned int> m_Indices;

    std::vector<glm::vec3> m_Tangents;
    std::vector<glm::vec3> m_Bitangents;

    unsigned int m_VAO; // Vertex Array Object
    unsigned int m_VBO; // Vertex Buffer Object
    unsigned int m_UBO; // UV Buffer Object
    unsigned int m_IBO; // Index Buffer Object
    unsigned int m_NBO; // Normal Buffer Object

    unsigned int m_TBO; // Tangent Buffer Object
    unsigned int m_BBO; // Bitangent Buffer Object

    Bounds* m_Bounds;

    public:

    bool m_CastShadows = true;

    std::string filePath;

    std::string defaultObject;

    Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices);
    Mesh();
    ~Mesh();

    void Draw();

    Bounds GetBounds();

    void CreateCube(glm::vec3 size);
    void CreateSphere(float radius, int rings, int sectors);
    void CreatePlane(glm::vec2 size);
    void LoadFromFile(std::string path);

    void Init() override;
    void Update(float dt) override;

    std::string GetName() override;
    void DrawInspector() override;
    void DrawGizmos() override;

    void SetupRender(Camera* camera, Shader* shader);

    void RecalculateNormals();
    void UpdateBuffers();
    void CalculateTBN();
};


#endif //CPP_GAME_ENGINE_MESH_H
