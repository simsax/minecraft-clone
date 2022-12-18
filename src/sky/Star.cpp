#include "Star.h"
#include "Config.h"
#include "../utils/Random.hpp"

Star::Star(std::string name, std::string texturePath, const glm::vec3& position,
	const glm::vec3& scale, const glm::vec3& color, float timeSpeed, uint32_t texMagFilter)
	: Entity(std::move(name), std::move(std::string(SOURCE_DIR) + "/res/textures/" + texturePath),
		scale, position, texMagFilter)
	, m_TimeSpeed(timeSpeed)
	, m_Time(0.0f)
	, m_Color(color)
	, m_Model(glm::mat4(1.0f))
{
	static constexpr float radius = 3000.0f;
	float x = static_cast<float>(Random::Normal(0, 1));
	float y = static_cast<float>(Random::Normal(0, 1));
	float z = static_cast<float>(Random::Normal(0, 1));
	float ranScale = static_cast<float>(Random::Uniform<double>(5, 30));

	if (x != 0 && y != 0 && z != 0) {
		const float rad = glm::sqrt(x * x + y * y + z * z);
		x = x / rad * radius;
		y = y / rad * radius;
		z = z / rad * radius;
	}

	m_BillboardPos = { position.x + x, position.y + y, position.z + z };
	m_Position = { x, y, z };
	m_BillboardSize = { ranScale, ranScale };
}

void Star::Render(QuadRenderer& renderer) { renderer.RenderBillboard(m_Texture, m_BillboardPos, m_BillboardSize); }

void Star::IncrTime(float deltaTime) { m_Time += deltaTime; }

glm::vec3 Star::GetColor() const { return m_Color; }

void Star::SetColor(const glm::vec3& lightColor) { m_Color = lightColor; }

void Star::SetPosition(const glm::vec3& position)
{
	m_BillboardPos = m_Position + position;
}
