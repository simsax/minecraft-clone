#include "CursorEntity.h"

CursorEntity::CursorEntity(std::string name, std::string texturePath, const glm::vec3 &position,
const glm::vec3 &scale) : GuiEntity(
        std::move(name), std::move(texturePath), position, scale) {}


void CursorEntity::Render(Renderer &renderer) {
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    GuiEntity::Render(renderer);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
