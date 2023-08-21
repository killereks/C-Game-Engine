
// render entity, everything that is needed to
// render the entity

#include "../Shader.h"
#include "../Camera.h"
#include <glm/glm.hpp>
#include <vector>

class Entity {
public:
	Entity();
	~Entity();

    Transform m_Transform;

	void Render(Camera* camera, Shader* shader);

private:
	glm::vec2 m_Position;
	float m_Rotation;
	glm::vec2 m_Scale;

    // list of vertices
    std::vector<float> m_Vertices;

    std::vector<unsigned int> m_Indices;

	// opengl buffers, we just need indices
	// and vertex positions
	unsigned int m_VBO;

    // index buffer object
    unsigned int m_IBO;
};