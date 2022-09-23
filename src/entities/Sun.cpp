#include "Sun.h"
#include "Config.h"

Sun::Sun(std::string name, std::string texturePath, const glm::vec3 &position,
         const glm::vec3 &scale, const glm::vec3 &color, float timeSpeed) : Entity(
        std::move(name), std::move(std::string(SOURCE_DIR) + "/res/textures/" + texturePath), scale, position),
        m_Day(true), m_TimeSpeed(timeSpeed), m_Time(0.0f), m_Color(color),
        m_Model(glm::mat4(1.0f))
        {}

void Sun::Render(QuadRenderer &renderer) {
    renderer.Render(m_Texture, m_Model);
}

void Sun::IncrTime(float deltaTime) {
    m_Time += deltaTime;
}

glm::vec3 Sun::GetColor() const {
    return m_Color;
}

void Sun::SetColor(const glm::vec3 &lightColor) {
    m_Color = lightColor;
}

bool Sun::IsDay() const {
    return m_Day;
}

void Sun::SetPosition(const glm::vec3 &position) {
    m_Day = true;
    static constexpr float sunHeight = 1000.0f;
    static constexpr int dayLevel = 40;
//    static constexpr glm::vec3 rotAxis = glm::vec3{1, 0, 0.7};
    static constexpr glm::vec3 rotAxis = glm::vec3{1, 0, 0};

    m_Model = glm::mat4(1.0f);

    m_Model = glm::translate(m_Model, position);
    m_Model = glm::rotate(m_Model, m_Time * m_TimeSpeed - glm::half_pi<float>(), glm::normalize(rotAxis));
    m_Model = glm::translate(m_Model, {-m_Scale.x/2.0f, sunHeight, -m_Scale.z/2.0f});
    m_Model = glm::scale(m_Model, m_Scale);
    m_Position = m_Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if (m_Position.y < dayLevel)
        m_Day = false;
}
