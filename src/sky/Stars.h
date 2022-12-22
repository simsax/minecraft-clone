#pragma once

#include "../utils/Entity.hpp"
#include "../graphics/QuadRenderer.h"

class Stars : public Entity<glm::vec3, QuadRenderer> {
public:
	Stars(std::string name, std::string texturePath, const glm::vec3& position,
		const glm::vec3& scale, uint32_t texMagFilter = GL_LINEAR, float timeSpeed = 0.005f);
	void Render(QuadRenderer& renderer, float alpha);
	void Update(float deltaTime, const glm::vec3& playerPosition);

private:
	struct Vertex {
		Vertex(const glm::vec3& pos, const glm::vec2& size)
			: billboardPos(pos), billboardSize(size)
		{
		}
		glm::vec3 billboardPos;
		glm::vec2 billboardSize;
	};

	glm::mat4 m_Model;
	float m_Time;
	float m_TimeSpeed;
	VertexBuffer m_VBO;
	VertexBuffer m_InstancedVBO;
	VertexArray m_VAO;
	std::vector<Vertex> m_InstancedVertices;
};
