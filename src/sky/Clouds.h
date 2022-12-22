#pragma once

#include "../utils/Entity.hpp"
#include "../graphics/QuadRenderer.h"

class Clouds : public Entity<glm::vec3, QuadRenderer> {
public:
	Clouds(std::string name, std::string texturePath, const glm::vec3& position,
		const glm::vec3& scale, uint32_t texMagFilter = GL_NEAREST, float timeSpeed = 0.005f);
	void Render(QuadRenderer& renderer, float alpha);
	void Update(float deltaTime, const glm::vec3& playerPosition);

private:
	glm::mat4 m_Model;
	float m_Time;
	float m_TimeSpeed;
	glm::vec3 m_PlayerPosition;
};
