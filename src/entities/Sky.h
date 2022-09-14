#pragma once

#include "QuadEntity.h"

namespace SkyColors {
    static constexpr glm::vec4 tc1 = {83.0f / 255.0f, 210.0f / 255.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 tc2 = {11.0f / 255.0f, 26.0f / 255.0f, 51.0f / 255.0f, 1.0f};
    static constexpr glm::vec4 bc1 = {211.0f / 255.0f, 232.0f/ 255.0f, 232 / 255.0f, 1.0f};
    static constexpr glm::vec4 bc2 = {242.0f / 255.0f , 110.0f / 255.0f, 34.0f / 255.0f, 1.0f};
};

class Sky : public QuadEntity {
public:
    Sky(std::string name, std::string texturePath, const glm::vec3 &position,
        const glm::vec3 &scale);
    static void InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath);
    void SetColor(const glm::vec4 &color, const glm::vec4 &fogColor);
    void SetLowerLimit(float lowerLimit);
    void Render(Renderer &renderer) override;
    glm::vec4 GetColor() const;


private:
    static Shader s_Shader;
    glm::vec4 m_Color;
    glm::vec4 m_FogColor;
    float m_LowerLimit;
};
