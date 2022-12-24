#include "Moon.h"
#include "Config.h"

Moon::Moon(std::string name, std::string texturePath, const glm::vec3& position,
	const glm::vec3& scale, uint32_t texMagFilter, float timeSpeed) :
	Entity(std::move(name), std::move(std::string(SOURCE_DIR) + "/res/textures/" + texturePath),
		scale, position, texMagFilter),
	m_Model(glm::mat4(1.0f)),
	m_Time(0.0f),
	m_TimeSpeed(timeSpeed),
	m_PlayerPosition(glm::vec3(0))
{

}

void Moon::Render(QuadRenderer& renderer) {
	renderer.Render(m_Texture, m_Model);
}

void Moon::Update(float deltaTime, const glm::vec3& position) {
	m_Time += deltaTime;
	static constexpr float moonHeight = 1000.0f;
	static constexpr glm::vec3 rotAxis = glm::vec3{ 1, 0, 0 };

	m_Model = glm::mat4(1.0f);

	m_Model = glm::translate(m_Model, position);
	float angle = m_Time * m_TimeSpeed;
	if (angle >= 2 * glm::pi<float>())
		m_Time = 0;
	m_Model = glm::rotate(m_Model, angle + glm::pi<float>(), rotAxis);
	m_Model = glm::translate(m_Model, { -m_Scale.x / 2.0f, moonHeight, -m_Scale.z / 2.0f });
	m_Model = glm::scale(m_Model, m_Scale);
	m_Position = m_Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
