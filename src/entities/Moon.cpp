#include "Moon.h"
#include "Config.h"

Moon::Moon(std::string name, std::string texturePath, const glm::vec3 &position,
           const glm::vec3 &scale, const glm::vec3 &color, float timeSpeed) : Entity(
        std::move(name), std::move(std::string(SOURCE_DIR) + "/res/textures/" + texturePath), scale, position),
m_TimeSpeed(timeSpeed), m_Time(0.0f), m_Color(color),m_Model(glm::mat4(1.0f)) {}

void Moon::Render(QuadRenderer &renderer) {
    renderer.Render(m_Texture, m_Model);
}

void Moon::IncrTime(float deltaTime) {
    m_Time += deltaTime;
}

glm::vec3 Moon::GetColor() const {
    return m_Color;
}

void Moon::SetColor(const glm::vec3 &lightColor) {
    m_Color = lightColor;
}

void Moon::SetPosition(const glm::vec3 &position) {
    static constexpr float moonHeight = 1000.0f;
    static constexpr glm::vec3 rotAxis = glm::vec3{1, 0, 0};

    m_Model = glm::mat4(1.0f);

    m_Model = glm::translate(m_Model, position);
    m_Model = glm::rotate(m_Model, m_Time * m_TimeSpeed + glm::half_pi<float>(),
                          glm::normalize(rotAxis));
    m_Model = glm::translate(m_Model, {-m_Scale.x / 2.0f, moonHeight, -m_Scale.z / 2.0f});
    m_Model = glm::scale(m_Model, m_Scale);
    m_Position = m_Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
