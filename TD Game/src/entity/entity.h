
// render entity, everything that is needed to
// render the entity

#include "../Shader.h"
#include <glm/glm.hpp>

class Entity {
public:
	Entity();
	~Entity();

	void SetPosition(glm::vec2 position);
	void SetPosition(float x, float y);
	void SetRotation(float rotation);
	void SetScale(glm::vec2 scale);
	void SetScale(float x, float y);

	glm::vec2 GetPosition();
	float GetRotation();
	glm::vec2 GetScale();

	void Render(Shader shader);

private:
	glm::vec2 m_Position;
	float m_Rotation;
	glm::vec2 m_Scale;

	// opengl buffers, we just need indices
	// and vertex positions
	unsigned int m_VBO;
};