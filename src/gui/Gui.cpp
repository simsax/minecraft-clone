#include "Gui.h"

#include <utility>

Gui::Gui(std::string name, const std::string& texturePath, const glm::vec2 &scale,
         const glm::vec2 &position)
        : m_Name(std::move(name)),
          m_Texture(texturePath),
          m_Scale(scale), m_Position(position) {
    m_Texture.Init();
}

Texture& Gui::GetTexture() { return m_Texture; }

glm::vec2 Gui::GetScale() const { return m_Scale; }

glm::vec2 Gui::GetPosition() const { return m_Position; }

void Gui::SetPosition(const glm::vec2 &position) { m_Position = position; }

std::string Gui::GetName() const { return m_Name; }
