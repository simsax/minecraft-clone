#pragma once

#include "Sun.h"
#include "Moon.h"
#include "Star.h"
#include "../graphics/SkyRenderer.h"

class SkyEntities {
public:
    SkyEntities(SkyRenderer* skyRenderer, int& screenHeight);
    void Update(float deltaTime, const glm::vec3& position, const glm::vec3& frontVector);
    void Render(QuadRenderer& renderer);
    [[nodiscard]] float GetAmbientStrength() const;
    [[nodiscard]] glm::vec4 GetSkyColor() const;
    [[nodiscard]] glm::vec3 GetSunDir() const;
    [[nodiscard]] glm::vec3 GetSunColor() const;
    [[nodiscard]] bool IsDay() const;

private:
    float m_AmbientStrength;
    SkyRenderer* m_SkyRenderer;
    int& m_ScreenHeight;
    Sun m_Sun;
    Moon m_Moon;
    std::vector<Star> m_Star;
    glm::vec3 m_SunDir;
    glm::vec4 m_SkyColor;
};
