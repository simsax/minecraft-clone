#define _USE_MATH_DEFINES
#include "Stars.h"
#include "Config.h"
#include "../utils/Random.hpp"
#include <array>
#include <cmath>


static constexpr std::array<float, 12> positions = {
		0.0f, 0.0f, 1.0f, // bottom left
		0.0f, 0.0f, 0.0f, // top left
		1.0f, 0.0f, 1.0f, // bottom right
		1.0f, 0.0f, 0.0f // top right
};

static constexpr float radius = 3000.0f;
static constexpr uint32_t starCount = 10000;

Stars::Stars(std::string name, std::string texturePath, const glm::vec3& position,
	const glm::vec3& scale, uint32_t texMagFilter, float timeSpeed) :
	Entity(std::move(name), std::move(std::string(SOURCE_DIR) + "/res/textures/" + texturePath),
		scale, position, texMagFilter),
	m_Model(glm::mat4(1.0f)),
	m_Time(0.0f),
	m_TimeSpeed(timeSpeed)
{
	m_InstancedVertices.reserve(starCount);

	for (int i = 0; i < starCount; i++) {
		float x = static_cast<float>(Random::Normal(0, 1));
		float y = static_cast<float>(Random::Normal(0, 1));
		float z = static_cast<float>(Random::Normal(0, 1));
		float ranScale = static_cast<float>(Random::Uniform<double>(5, 40));

		if (x != 0 && y != 0 && z != 0) {
			const float rad = glm::sqrt(x * x + y * y + z * z);
			x = x / rad * radius;
			y = y / rad * radius;
			z = z / rad * radius;

			m_InstancedVertices.emplace_back(glm::vec3{ x, y, z }, glm::vec2{ ranScale, ranScale });
		}
	}

	VertexBufferLayout layout;
	layout.Push<float>(3);
	m_VBO.Init(layout.GetStride(), 0);
	m_VBO.CreateStatic(positions.size() * sizeof(float), positions.data());
	m_VAO.Init();
	m_VBO.Bind(m_VAO.GetId());
	m_VAO.AddLayout(layout, 0);

	VertexBufferLayout instancedLayout;
	instancedLayout.Push<float>(3, true); // billboard position
	instancedLayout.Push<float>(2, true); // billboard size
	m_InstancedVBO.Init(instancedLayout.GetStride(), 1);
	m_InstancedVBO.CreateStatic(sizeof(Vertex) * starCount, m_InstancedVertices.data());
	m_InstancedVBO.Bind(m_VAO.GetId());
	m_VAO.AddLayout(instancedLayout, 1);
}

void Stars::Render(QuadRenderer& renderer, float alpha) {
	renderer.RenderStars(m_Texture, m_VAO, m_Model, starCount, alpha);
}

void Stars::Update(float deltaTime, const glm::vec3& position)
{
	static constexpr glm::vec3 rotAxis = { M_SQRT1_2, 0, M_SQRT1_2 };
	m_Model = glm::mat4(1.0f);
	m_Time += deltaTime;
	float angle = m_Time * m_TimeSpeed;
	if (angle >= 2 * glm::pi<float>())
		m_Time = 0;
	m_Model = glm::translate(m_Model, position);
	m_Model = glm::rotate(m_Model, angle - glm::half_pi<float>(), rotAxis);
}
