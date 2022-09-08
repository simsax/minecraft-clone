#pragma once

#include "QuadEntity.h"

class Sky : public QuadEntity {
public:
    Sky(std::string name, std::string texturePath, const glm::vec3 &position,
        const glm::vec3 &scale, const glm::vec3& color);
    static void InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath);
    glm::vec3 GetColor() const;
    void SetColor(const glm::vec3 &color);
    void Render(Renderer &renderer) override;

private:
    static Shader s_Shader;
    glm::vec3 m_Color;
};
