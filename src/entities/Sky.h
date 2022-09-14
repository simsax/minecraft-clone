#pragma once

#include "QuadEntity.h"

namespace SkyColors {
    static constexpr glm::vec4 tc1 = {83.0f / 255.0f, 210.0f / 255.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 tc2 = {203.0f / 255.0f, 247.0f / 255.0f, 242.0f / 255.0f, 1.0f};
    static constexpr glm::vec4 tc3 = {1.0f / 255.0f, 22.0f / 255.0f, 56.0f / 255.0f, 1.0f};
    static constexpr glm::vec4 bc1 = {1.0f, 1.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 bc2 = {1.0f, 86.0f / 255.0f, 3.0f / 255.0f, 1.0f};
};

class Sky : public QuadEntity {
public:
    Sky(std::string name, std::string texturePath, const glm::vec3 &position,
        const glm::vec3 &scale);
    static void InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath);
    void SetColor(const glm::vec4 &topColor, const glm::vec4& bottomColor);
    void Render(Renderer &renderer) override;
    std::array<glm::vec4, 4> GetColor() const;


private:
    static Shader s_Shader;
    std::array<glm::vec4, 4> m_Color;
};
