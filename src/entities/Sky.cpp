#include "Sky.h"
#include "Config.h"
#include <mutex>

Shader Sky::s_Shader = {};
static std::once_flag fShaders;

Sky::Sky(std::string name, std::string texturePath, const glm::vec3 &position,
         const glm::vec3 &scale, const glm::vec3 &color) : QuadEntity(
        std::move(name), std::move(texturePath), position, scale), m_Color(color) {}

void Sky::InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath) {
    std::call_once(fShaders, [&]() {
        s_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/" + vertShaderPath,
                      std::string(SOURCE_DIR) + "/res/shaders/" + fragShaderPath);
        s_Shader.Bind();
    });
}

void Sky::Render(Renderer &renderer) {
    static constexpr float skyHeight = 10000.0f;

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, {m_Position.x - m_Scale.x / 2.0f, m_Position.y + skyHeight,
                                   m_Position.z - m_Scale.z / 2.0f});
    model = glm::scale(model, m_Scale);

    renderer.RenderQuad(s_VAO, s_Shader, m_Color, model, false);
}

glm::vec3 Sky::GetColor() const {
    return m_Color;
}

void Sky::SetColor(const glm::vec3 &color) {
    m_Color = color;
}
