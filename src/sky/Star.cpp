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
    static constexpr float radius = 1010.0f;
    auto alpha = static_cast<float>(glm::radians(Random::GetRand<double>(0, 360)));
    auto theta = static_cast<float>(glm::radians(Random::GetRand<double>(0, 360)));
    auto ranScale = static_cast<float>(Random::GetRand<double>(5, 20));
    // float y = radius * glm::sin(alpha);
    // float radius2 = radius * glm::cos(alpha);
    // float x = radius2 * glm::cos(theta);
    // float z = radius2 * glm::sin(theta);

    // m_Model = glm::mat4(1.0f);
    // m_Model = glm::translate(m_Model, { x, y, z });
    // m_Model = glm::scale(m_Model, m_Scale);

    m_Model = glm::translate(m_Model, position);
    m_Model = glm::rotate(m_Model, theta, glm::normalize(glm::vec3 { 0, 1, 0 }));
    m_Model = glm::rotate(
        m_Model, alpha - glm::half_pi<float>(), glm::normalize(glm::vec3 { 0, 0, 1 }));
    m_Model = glm::translate(m_Model, { -ranScale / 2.0f, radius, -ranScale / 2.0f });
    m_Model = glm::scale(m_Model, { ranScale, 1.0f, ranScale });
    m_Position = m_Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Star::Render(QuadRenderer& renderer) { renderer.Render(m_Texture, m_Model); }

void Star::IncrTime(float deltaTime) { m_Time += deltaTime; }

glm::vec3 Star::GetColor() const { return m_Color; }

void Star::SetColor(const glm::vec3& lightColor) { m_Color = lightColor; }

void Star::SetPosition(const glm::vec3& position)
{
    static constexpr float starHeight = 1010.0f;
    static constexpr glm::vec3 rotAxis = glm::vec3 { 1, 0, 0 };

    m_Model = glm::mat4(1.0f);

    m_Model = glm::translate(m_Model, position);
    // m_Model = glm::rotate(
    //     m_Model, m_Time * m_TimeSpeed + glm::half_pi<float>(), glm::normalize(rotAxis));
    m_Model = glm::translate(m_Model, { -m_Scale.x / 2.0f, starHeight, -m_Scale.z / 2.0f });
    m_Model = glm::scale(m_Model, m_Scale);
    m_Position = m_Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
