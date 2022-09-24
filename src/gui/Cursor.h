#pragma once

#include "glm/vec2.hpp"
#include "../graphics/GuiRenderer.h"
#include "../utils/Entity.hpp"

class Cursor : public Entity<glm::vec2, GuiRenderer> {
public:
    Cursor(std::string name, std::string texturePath, const glm::vec2 &scale,
           const glm::vec2 &position);

    void Render(GuiRenderer &renderer) override;
};
