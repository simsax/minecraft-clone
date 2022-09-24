#include "SkyEntities.h"

namespace SkyColors {
    static constexpr glm::vec4 tc1 = {83.0f / 255.0f, 210.0f / 255.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 tc2 = {11.0f / 255.0f, 26.0f / 255.0f, 51.0f / 255.0f, 1.0f};
    static constexpr glm::vec4 bc1 = {211.0f / 255.0f, 232.0f/ 255.0f, 232 / 255.0f, 1.0f};
    static constexpr glm::vec4 bc2 = {242.0f / 255.0f , 110.0f / 255.0f, 34.0f / 255.0f, 1.0f};
};

SkyEntities::SkyEntities(SkyRenderer* skyRenderer, int& screenHeight) :
    m_AmbientStrength(0.5f),
    m_SkyRenderer(skyRenderer),
    m_ScreenHeight(screenHeight),
    m_Sun("sun", "sun.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f)),
    m_Moon("moon", "moon.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f)),
    m_SunDir(0,0,0),
    m_SkyColor(0, 0, 0, 1)
{ }

void SkyEntities::Update(float deltaTime, const glm::vec3 &position, const glm::vec3& frontVector) {
    m_Sun.IncrTime(deltaTime);
    m_Sun.SetPosition(position);
    m_Moon.IncrTime(deltaTime);
    m_Moon.SetPosition(position);
    m_SunDir = glm::normalize(m_Sun.GetPosition() - position);

    // update sky color
    static constexpr glm::vec3 upVector = {0, 1, 0};
    const float d = glm::dot(upVector, m_SunDir);
    const float x = glm::dot(upVector, frontVector);

    if (d >= 0) {
        glm::vec4 color = d * SkyColors::tc1 + (1 - d) * SkyColors::tc2;
        m_SkyColor = d * SkyColors::bc1 + (1 - d) * SkyColors::bc2;
        m_SkyRenderer->SetSkyColor(color);
        m_SkyRenderer->SetFogColor(m_SkyColor);
    } else {
        m_SkyColor = -d * SkyColors::tc2 + (1 + d) * SkyColors::bc2;
        m_SkyRenderer->SetSkyColor(SkyColors::tc2);
        m_SkyRenderer->SetFogColor(m_SkyColor);
    }

    float lowerLimit = m_ScreenHeight * (-glm::asin(x) * 0.5f / glm::radians(35.0f) + 0.5f);
    m_SkyRenderer->SetLowerLimit(lowerLimit);

    // update ambient strength
    float dot = glm::dot(upVector, m_SunDir);

    if (dot >= 0) {
        m_AmbientStrength = dot * 0.5f + (1 - dot) * 0.1f;
    } else {
        m_AmbientStrength = -dot * 0.05f + (1 + dot) * 0.1f;
    }
}

void SkyEntities::Render(QuadRenderer &renderer) {
    m_Sun.Render(renderer);
    m_Moon.Render(renderer);
}

float SkyEntities::GetAmbientStrength() const {
    return m_AmbientStrength;
}

glm::vec4 SkyEntities::GetSkyColor() const {
    return m_SkyColor;
}

glm::vec3 SkyEntities::GetSunDir() const {
   if (IsDay())
       return -m_SunDir;
   else
       return m_SunDir; // moon dir
}

glm::vec3 SkyEntities::GetSunColor() const {
   if (IsDay())
       return m_Sun.GetColor();
   else
       return m_Moon.GetColor();
}

bool SkyEntities::IsDay() const {
    if (m_Sun.GetPosition().y > 40)
        return true;
    else
        return false;
}
