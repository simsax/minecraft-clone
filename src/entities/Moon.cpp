#include "Moon.h"

#include <utility>
#include "../utils/Logger.h"

Moon::Moon(std::string name, std::string texturePath, const glm::vec3 &position,
           const glm::vec3 &scale, const glm::vec3 &color) :
        Sun(std::move(name), std::move(texturePath), position, scale, color) {}

void Moon::Render(Renderer &renderer) {
    renderer.RenderQuad(s_VAO, s_Shader, m_Texture, m_Model, false);
}

void Moon::SetPosition(const glm::vec3 &position) {
    static constexpr float moonHeight = 1000.0f;
    static constexpr glm::vec3 rotAxis = glm::vec3{1, 0, 0};

    m_Model = glm::mat4(1.0f);
    m_Model = glm::translate(m_Model, position);
    m_Model = glm::rotate(m_Model, m_Time * s_TimeSpeed + glm::half_pi<float>(), glm::normalize(rotAxis));
    m_Model = glm::translate(m_Model, {-m_Scale.x / 2.0f, moonHeight, -m_Scale.z / 2.0f});
    m_Model = glm::scale(m_Model, m_Scale);
    m_Position = m_Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
