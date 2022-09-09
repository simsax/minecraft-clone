#include "Sky.h"
#include "Config.h"
#include <mutex>

Shader Sky::s_Shader = {};
static std::once_flag fShaders;

Sky::Sky(std::string name, std::string texturePath, const glm::vec3 &position,
         const glm::vec3 &scale) : QuadEntity(
        std::move(name), std::move(texturePath), position, scale) {
    glm::vec4 topColor = {173.0f / 255.0f, 223.0f / 255.0f, 230.0f / 255.0f, 1.0f};
    glm::vec4 bottomColor = glm::vec4(1.0f);
    SetColor(topColor, bottomColor);
}

void Sky::InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath) {
    std::call_once(fShaders, [&]() {
        s_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/" + vertShaderPath,
                      std::string(SOURCE_DIR) + "/res/shaders/" + fragShaderPath);
        s_Shader.Bind();
    });
}

void Sky::Render(Renderer &renderer) {
    glDepthMask(0);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,
                           {m_Position.x - m_Scale.x / 2.0f, m_Position.z - m_Scale.z / 2.0f, 0.0f});
    model = glm::scale(model, {m_Scale.x, m_Scale.z, 0.0f});
    renderer.RenderQuad(s_VAO, s_Shader, m_Color, model, true);

    glDepthMask(1);
}

std::array<glm::vec4, 4> Sky::GetColor() const {
    return m_Color;
}

void Sky::SetColor(const glm::vec4 &topColor, const glm::vec4& bottomColor) {
    m_Color = {topColor, bottomColor, topColor, bottomColor};
}
