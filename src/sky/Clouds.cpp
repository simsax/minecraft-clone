#include "Clouds.h"
#include "Config.h"

Clouds::Clouds(std::string name, std::string texturePath, const glm::vec3& position,
	const glm::vec3& scale, uint32_t texMagFilter, float timeSpeed) :
	Entity(std::move(name), std::move(std::string(SOURCE_DIR) + "/res/textures/" + texturePath),
		scale, position, texMagFilter),
	m_Model(glm::mat4(1.0f)),
	m_Time(0.0f),
	m_TimeSpeed(timeSpeed),
	m_PlayerPosition(glm::vec3(0))
{
	static constexpr float cloudsHeight = 600.0f;

	m_Model = glm::translate(m_Model, { position.x - m_Scale.x / 2.0f, cloudsHeight,
		position.z - m_Scale.z / 2.0f });
	m_Model = glm::scale(m_Model, m_Scale);
}

void Clouds::Render(QuadRenderer& renderer, float alpha) {
	renderer.RenderClouds(m_Texture, m_Model, m_Time, alpha);
}

void Clouds::Update(float deltaTime, const glm::vec3& position) {
	m_Time += deltaTime * m_TimeSpeed * 0.5f;
	if (m_Time >= 1)
		m_Time = 0;
}
