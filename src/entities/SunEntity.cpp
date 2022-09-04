#include "SunEntity.h"
#include "Config.h"
#include <mutex>

Shader SunEntity::s_Shader = {};
static std::once_flag fShaders;

SunEntity::SunEntity(std::string name, std::string texturePath, const glm::vec3 &position,
                     const glm::vec3 &scale) : QuadEntity(
        std::move(name), std::move(texturePath), position, scale), m_Time(0.0f) {}

void SunEntity::InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath) {
    std::call_once(fShaders, [&](){
        s_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/" + vertShaderPath,
                      std::string(SOURCE_DIR) + "/res/shaders/" + fragShaderPath);
        s_Shader.Bind();
        s_Shader.SetUniform1i("u_Texture", 0);
    });
}

void SunEntity::Render(Renderer &renderer) {
    static constexpr float timeSpeed = 1.0f;
    static constexpr float sunHeight = 1000.0f;

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, m_Position);
    model = glm::rotate(model, m_Time * timeSpeed, {1, 0, 0});
    model = glm::translate(model, {-m_Scale.x/2.0f, sunHeight, -m_Scale.z/2.0f});
    model = glm::scale(model, m_Scale);
    renderer.RenderQuad(s_VAO, s_Shader, m_Texture, model, false);
}

void SunEntity::IncrTime(float deltaTime) {
    m_Time += deltaTime;
}
