#include "Gui.h"

Gui::Gui(std::string name, std::string texturePath, glm::vec2 scale, glm::vec2 position)
    : m_Name(std::move(name))
    , m_Texture(std::make_shared<Texture>(std::move(texturePath)))
    , m_Scale(std::move(scale))
    , m_Position(std::move(position))
{
}

std::shared_ptr<Texture> Gui::GetTexture() const { return m_Texture; }

glm::vec2 Gui::GetScale() const { return m_Scale; }

glm::vec2 Gui::GetPosition() const { return m_Position; }

void Gui::SetPosition(const glm::vec2& position) { m_Position = position; }

std::string Gui::GetName() const { return m_Name; }