#pragma once

#include "QuadEntity.h"

class GuiEntity : public QuadEntity {
public:
    GuiEntity(std::string name, std::string texturePath, const glm::vec3 &position,
        const glm::vec3 &scale);

    void Render(Renderer &renderer) override;
};
