#pragma once

#include "GuiEntity.h"

class CursorEntity : public GuiEntity {
public:
    CursorEntity(std::string name, std::string texturePath, const glm::vec3 &position,
    const glm::vec3 &scale);

    void Render(Renderer &renderer) override;
};
