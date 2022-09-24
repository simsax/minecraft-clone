#include "Cursor.h"

Cursor::Cursor(std::string name, std::string texturePath, const glm::vec2 &scale,
               const glm::vec2 &position) :
        Entity(std::move(name), std::move(texturePath), scale, position) {
}

void Cursor::Render(GuiRenderer &renderer) {
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    Entity::Render(renderer);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
