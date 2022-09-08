#include "Moon.h"

#include <utility>
#include "../utils/Logger.h"

Moon::Moon(std::string name, std::string texturePath, const glm::vec3 &position,
           const glm::vec3 &scale, const glm::vec3 &color) :
        Sun(std::move(name), std::move(texturePath), position, scale, color) {}

void Moon::Render(Renderer &renderer) {
    static constexpr float timeSpeed = 0.01f;
    static constexpr float moonHeight = 1000.0f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    model = glm::rotate(model, glm::pi<float>() + m_Time * timeSpeed, {1, 0, 0});
    model = glm::translate(model, {-m_Scale.x / 2.0f, moonHeight, -m_Scale.z / 2.0f});
    model = glm::scale(model, m_Scale);
    renderer.RenderQuad(s_VAO, s_Shader, m_Texture, model, false);
}
