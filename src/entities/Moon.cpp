#include "Moon.h"

#include <utility>
#include "../utils/Logger.h"

Moon::Moon(std::string name, std::string texturePath, const glm::vec3 &position,
           const glm::vec3 &scale, const glm::vec3 &color) :
        Sun(std::move(name), std::move(texturePath), position, scale, color) {}

void Moon::Render(Renderer &renderer) {
    static constexpr float moonHeight = 1000.0f;
    static constexpr glm::vec3 rotAxis = glm::vec3{1, 0, 0.7};

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    model = glm::rotate(model, m_Time * s_TimeSpeed + glm::half_pi<float>(), glm::normalize(rotAxis));
    model = glm::translate(model, {-m_Scale.x / 2.0f, moonHeight, -m_Scale.z / 2.0f});
    model = glm::scale(model, m_Scale);
    glm::vec4 newPos = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    SetPosition(newPos);
    renderer.RenderQuad(s_VAO, s_Shader, m_Texture, model, false);
}
