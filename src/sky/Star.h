#pragma once

#include "../utils/Entity.hpp"
#include "../graphics/QuadRenderer.h"

class Star : public Entity<glm::vec3, QuadRenderer> {
public:
	Star(std::string name, std::string texturePath, const glm::vec3& position,
		const glm::vec3& scale, const glm::vec3& color, float timeSpeed,
		uint32_t texMagFilter = GL_LINEAR);
	virtual void IncrTime(float deltaTime);
	glm::vec3 GetColor() const;
	void SetColor(const glm::vec3& lightColor);
	void Render(QuadRenderer& renderer) override;
	void SetPosition(const glm::vec3& position) override;

private:
	float m_TimeSpeed;
	glm::vec3 m_Color;
	float m_Time;
	glm::mat4 m_Model;
	glm::vec3 m_BillboardPos;
	glm::vec2 m_BillboardSize;
};
