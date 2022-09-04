#pragma once

#include "QuadEntity.h"

class GuiEntity : public QuadEntity {
public:
    GuiEntity(std::string name, std::string texturePath, const glm::vec3 &position,
        const glm::vec3 &scale);

    static void InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath);
    void Render(Renderer &renderer) override;
protected:
    static Shader s_Shader;
};
