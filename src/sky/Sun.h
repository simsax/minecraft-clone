#pragma once

#include "../utils/Entity.hpp"
#include "../graphics/QuadRenderer.h"

class Sun : public Entity<glm::vec3, QuadRenderer> {
public:
    Sun(std::string name, std::string texturePath, const glm::vec3 &position,
        const glm::vec3 &scale, const glm::vec3& color = glm::vec3(1.0f), float timeSpeed = 0.1f);
    virtual void IncrTime(float deltaTime);
    glm::vec3 GetColor() const;
    void SetColor(const glm::vec3& lightColor);
    void Render(QuadRenderer &renderer) override;
    bool IsDay() const;
    void SetPosition(const glm::vec3 &position) override;

private:
    bool m_Day;
    float m_TimeSpeed;
    glm::vec3 m_Color;
    float m_Time;
    glm::mat4 m_Model;
};
