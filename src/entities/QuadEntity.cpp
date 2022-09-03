#include "QuadEntity.h"
#include "Config.h"
#include <mutex>

VertexBuffer QuadEntity::s_VBO = {};
VertexBufferLayout QuadEntity::s_VertexLayout = {};
VertexArray QuadEntity::s_VAO = {};
Shader QuadEntity::s_Shader = {};

static std::once_flag fBuffers, fShaders;

static constexpr std::array<float, 12> positions = {
        0.0f, 0.0f, 1.0f, // bottom left
        0.0f, 0.0f, 0.0f, // top left
        1.0f, 0.0f, 1.0f, // bottom right
        1.0f, 0.0f, 0.0f // top right
};

QuadEntity::QuadEntity(std::string name, std::string texturePath, const glm::vec3 &position,
                       const glm::vec3 &scale) : Entity(std::move(name), std::move(texturePath),
                                                        position, scale) {}

void QuadEntity::InitBuffers() {
    std::call_once(fBuffers, [](){
        s_VertexLayout.Push<float>(3);
        s_VBO.Init(s_VertexLayout.GetStride(), 0);
        s_VBO.CreateStatic(positions.size() * sizeof(float), positions.data());
        s_VAO.Init();
        s_VBO.Bind(s_VAO.GetId());
        s_VAO.AddLayout(s_VertexLayout, 0);
    });
}

void QuadEntity::InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath) {
    std::call_once(fShaders, [&](){
        s_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/" + vertShaderPath,
                      std::string(SOURCE_DIR) + "/res/shaders/" + fragShaderPath);
        s_Shader.Bind();
        s_Shader.SetUniform1i("u_Texture", 0);
    });
}

void QuadEntity::Render(Renderer &renderer) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
            model, glm::vec3(m_Position.x - m_Scale.x / 2.0f, m_Position.y - m_Scale.y / 2.0f,
                             m_Position.z - m_Scale.z / 2.0f));
    model = glm::scale(model, m_Scale);
    renderer.RenderQuad(s_VAO, s_Shader, m_Texture, model, true);
}
