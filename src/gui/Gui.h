#pragma once
#include "GL/glew.h"
#include <memory>
#include "../graphics/Texture.h"
#include "glm/gtc/matrix_transform.hpp"

class Gui {
public:
    Gui(std::string name, const std::string& texturePath, const glm::vec2& scale, const glm::vec2& position);
    Texture & GetTexture();
    glm::vec2 GetScale() const;
    glm::vec2 GetPosition() const;
    void SetPosition(const glm::vec2& position);
    std::string GetName() const;

private:
    std::string m_Name;
    Texture m_Texture;
    glm::vec2 m_Scale;
    glm::vec2 m_Position;
};
