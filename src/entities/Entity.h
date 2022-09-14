#pragma once

#include "GL/glew.h"
#include "../graphics/Shader.h"
#include "../graphics/VertexBuffer.h"
#include "../graphics/VertexBufferLayout.hpp"
#include "../graphics/VertexArray.h"
#include "../graphics/Texture.h"
#include "../graphics/Renderer.h"

class Entity {
public:
    Entity(std::string name, std::string texturePath, const glm::vec3 &position,
           const glm::vec3 &scale);

    void InitTexture();
    std::string GetName() const;
    glm::vec3 GetScale() const;
    glm::vec3 GetPosition() const;
    void SetScale(const glm::vec3 &scale);
    virtual void SetPosition(const glm::vec3 &position);

    virtual void Render(Renderer &renderer) = 0;

protected:
    std::string m_Name;
    Texture m_Texture;
    glm::vec3 m_Position;
    glm::vec3 m_Scale;
};
