#pragma once
#include "GL/glew.h"
#include <memory>
#include "../graphics/Texture.h"
#include "glm/gtc/matrix_transform.hpp"

template <typename T, typename R> class Entity {
public:
    Entity(std::string name, std::string texturePath, const T& scale, const T& position,
        uint32_t texMagFilter = GL_NEAREST);
    [[nodiscard]] std::string GetName() const;
    Texture& GetTexture();
    T GetScale() const;
    void SetScale(const T& scale);
    T GetPosition() const;

    virtual void SetPosition(const T& position);
    virtual void Render(R& renderer);

protected:
    std::string m_Name;
    Texture m_Texture;
    T m_Scale;
    T m_Position;
};

template <typename T, typename R>
Entity<T, R>::Entity(std::string name, std::string texturePath, const T& scale, const T& position,
    uint32_t texMagFilter)
    : m_Name(std::move(name))
    , m_Texture(std::move(texturePath))
    , m_Scale(scale)
    , m_Position(position)
{
    m_Texture.Init(texMagFilter);
}

template <typename T, typename R> Texture& Entity<T, R>::GetTexture() { return m_Texture; }

template <typename T, typename R> T Entity<T, R>::GetScale() const { return m_Scale; }

template <typename T, typename R> T Entity<T, R>::GetPosition() const { return m_Position; }

template <typename T, typename R> void Entity<T, R>::SetPosition(const T& position)
{
    m_Position = position;
}

template <typename T, typename R> std::string Entity<T, R>::GetName() const { return m_Name; }

template <typename T, typename R> void Entity<T, R>::SetScale(const T& scale) { m_Scale = scale; }

template <typename T, typename R> void Entity<T, R>::Render(R& renderer)
{
    renderer.Render(m_Texture, m_Position, m_Scale);
}
