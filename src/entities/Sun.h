#pragma once

#include "QuadEntity.h"

class Sun : public QuadEntity {
public:
    Sun(std::string name, std::string texturePath, const glm::vec3 &position,
        const glm::vec3 &scale, const glm::vec3& color = glm::vec3(1.0f));
    static void InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath);
    virtual void IncrTime(float deltaTime);
    glm::vec3 GetColor() const;
    void SetColor(const glm::vec3& lightColor);
    void Render(Renderer &renderer) override;
    bool IsDay() const;

protected:
    static Shader s_Shader;
    static bool s_Day;
    static constexpr float s_TimeSpeed = 0.001f;
    glm::vec3 m_Color;
    float m_Time;
};
