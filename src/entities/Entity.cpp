#include "Entity.h"
#include "Config.h"

Entity::Entity(std::string name, std::string texturePath, const glm::vec3 &position,
               const glm::vec3 &scale) :
        m_Name(std::move(name)),
        m_Texture(std::string(SOURCE_DIR) + "/res/textures/" + std::move(texturePath)),
        m_Position(position),
        m_Scale(scale) {
}


glm::vec3 Entity::GetScale() const {
    return m_Scale;
}

glm::vec3 Entity::GetPosition() const {
    return m_Position;
}

std::string Entity::GetName() const {
    return m_Name;
}

void Entity::SetScale(const glm::vec3 &scale) {
    m_Scale = scale;
}

void Entity::SetPosition(const glm::vec3 &position) {
    m_Position = position;
}

void Entity::InitTexture() {
    m_Texture.Init();
}
