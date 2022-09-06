#pragma once

#include "QuadEntity.h"

class SunEntity : public QuadEntity {
public:
    SunEntity(std::string name, std::string texturePath, const glm::vec3 &position,
              const glm::vec3 &scale, const glm::vec3& color = glm::vec3(1.0f));
    static void InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath);
    glm::vec3 GetColor() const;
    void SetColor(const glm::vec3& lightColor);
    void IncrTime(float deltaTime);
    void Render(Renderer &renderer) override;
private:
    static Shader s_Shader;
    glm::vec3 m_Color;
    float m_Time;
};
