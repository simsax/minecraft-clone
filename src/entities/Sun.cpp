#include "Sun.h"
#include "Config.h"
#include <mutex>

Shader Sun::s_Shader = {};
static std::once_flag fShaders;

Sun::Sun(std::string name, std::string texturePath, const glm::vec3 &position,
         const glm::vec3 &scale, const glm::vec3 &color) : QuadEntity(
        std::move(name), std::move(texturePath), position, scale), m_Time(0.0f), m_Color(color),
        m_Day(true) {}

void Sun::InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath) {
    std::call_once(fShaders, [&](){
        s_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/" + vertShaderPath,
                      std::string(SOURCE_DIR) + "/res/shaders/" + fragShaderPath);
        s_Shader.Bind();
        s_Shader.SetUniform1i("u_Texture", 0);
    });
}

void Sun::Render(Renderer &renderer) {
    m_Day = true;
    static constexpr float timeSpeed = 0.01f;
    static constexpr float sunHeight = 1000.0f;
    static constexpr int dayLevel = 50;

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, m_Position);
    model = glm::rotate(model, m_Time * timeSpeed - glm::half_pi<float>(), {1, 0, 0});
    model = glm::translate(model, {-m_Scale.x/2.0f, sunHeight, -m_Scale.z/2.0f});
    model = glm::scale(model, m_Scale);
    glm::vec4 newPos = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if (newPos.y < dayLevel) {
        m_Day = false;
    }
    SetPosition(newPos);
    renderer.RenderQuad(s_VAO, s_Shader, m_Texture, model, false);
}

void Sun::IncrTime(float deltaTime) {
    m_Time += deltaTime;
}

glm::vec3 Sun::GetColor() const {
    return m_Color;
}

void Sun::SetColor(const glm::vec3 &lightColor) {
    m_Color = lightColor;
}

bool Sun::IsDay() const {
    return m_Day;
}
