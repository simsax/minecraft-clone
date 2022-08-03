#pragma once
#include <GL/glew.h>
#include <memory>
#include "Texture.h"
#include "glm/gtc/matrix_transform.hpp"

class Gui {
public:
    Gui(std::string name, std::string texturePath, glm::vec2 scale, glm::vec2 position);
    std::shared_ptr<Texture> GetTexture() const;
    glm::vec2 GetScale() const;
    glm::vec2 GetPosition() const;
    void SetPosition(const glm::vec2& position);
    std::string GetName() const;

private:
    std::string m_Name;
    std::shared_ptr<Texture> m_Texture;
    glm::vec2 m_Scale;
    glm::vec2 m_Position;
};