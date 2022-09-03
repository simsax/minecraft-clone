#include "GuiEntity.h"

GuiEntity::GuiEntity(std::string name, std::string texturePath, const glm::vec3 &position,
         const glm::vec3 &scale) : QuadEntity(
        std::move(name), "gui/" + std::move(texturePath), position, scale) {}

void GuiEntity::Render(Renderer &renderer) {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    QuadEntity::Render(renderer);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
}
