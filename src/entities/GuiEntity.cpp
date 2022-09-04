#include "GuiEntity.h"
#include "Config.h"
#include <mutex>

Shader GuiEntity::s_Shader = {};
static std::once_flag fShaders;

GuiEntity::GuiEntity(std::string name, std::string texturePath, const glm::vec3 &position,
         const glm::vec3 &scale) : QuadEntity(
        std::move(name), "gui/" + std::move(texturePath), position, scale) {}

void GuiEntity::InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath) {
    std::call_once(fShaders, [&](){
        s_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/" + vertShaderPath,
                      std::string(SOURCE_DIR) + "/res/shaders/" + fragShaderPath);
        s_Shader.Bind();
        s_Shader.SetUniform1i("u_Texture", 0);
    });
}

void GuiEntity::Render(Renderer &renderer) {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,
                       {m_Position.x - m_Scale.x / 2.0f, m_Position.z - m_Scale.z / 2.0f, 0.0f});
    model = glm::scale(model, {m_Scale.x, m_Scale.z, 0.0f});
    renderer.RenderQuad(s_VAO, s_Shader, m_Texture, model, true);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
}
