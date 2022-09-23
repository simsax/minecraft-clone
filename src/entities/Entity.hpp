#pragma once
#include "GL/glew.h"
#include <memory>
#include "../graphics/Texture.h"
#include "glm/gtc/matrix_transform.hpp"

template<typename T>
class Entity {
public:
    Entity(std::string name, std::string texturePath, const T& scale, const T& position);
    Texture& GetTexture();
    T GetScale() const;
    void SetScale(const T& scale);
    T GetPosition() const;
    virtual void SetPosition(const T& position);
    [[nodiscard]] std::string GetName() const;

protected:
    std::string m_Name;
    Texture m_Texture;
    T m_Scale;
    T m_Position;
};

template<typename T>
Entity<T>::Entity(std::string name, std::string texturePath, const T& scale,
               const T& position)
        : m_Name(std::move(name)),
          m_Texture(std::move(texturePath)),
          m_Scale(scale), m_Position(position) {
    m_Texture.Init();
}

template<typename T>
Texture& Entity<T>::GetTexture() { return m_Texture; }

template<typename T>
T Entity<T>::GetScale() const { return m_Scale; }

template<typename T>
T Entity<T>::GetPosition() const { return m_Position; }

template<typename T>
void Entity<T>::SetPosition(const T& position) { m_Position = position; }

template<typename T>
std::string Entity<T>::GetName() const { return m_Name; }

template<typename T>
void Entity<T>::SetScale(const T& scale) { m_Scale = scale; }
