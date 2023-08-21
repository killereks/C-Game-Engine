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

class Mesh : public Component {
    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec2> m_UVs;
    std::vector<glm::vec3> m_Normals;
    std::vector<unsigned int> m_Indices;

    unsigned int m_VAO; // Vertex Array Object
    unsigned int m_VBO; // Vertex Buffer Object
    unsigned int m_IBO; // Index Buffer Object
    unsigned int m_NBO; // Normal Buffer Object

    public:

    Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices);

    static Mesh CreateCube(glm::vec3 size);
    static Mesh CreateSphere(float radius, int rings, int sectors);
    static Mesh CreatePlane(glm::vec2 size);
    static Mesh LoadFromFile(std::string path);

    void RecalculateNormals();
    void Update();
};


#endif //CPP_GAME_ENGINE_MESH_H
