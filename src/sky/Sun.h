#pragma once

#include "../utils/Entity.hpp"
#include "../graphics/QuadRenderer.h"

class Sun : public Entity<glm::vec3, QuadRenderer> {
public:
	Sun(std::string name, std::string texturePath, const glm::vec3& position,
		const glm::vec3& scale, uint32_t texMagFilter = GL_NEAREST, float timeSpeed = 0.1f);
	void Render(QuadRenderer& renderer) override;
	void Update(float deltaTime, const glm::vec3& playerPositon);
	bool IsDay() const;

private:
	bool m_Day;
	glm::mat4 m_Model;
	float m_Time;
	float m_TimeSpeed;
	glm::vec3 m_PlayerPosition;
};
