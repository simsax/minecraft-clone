#pragma once

#include "QuadEntity.h"

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
