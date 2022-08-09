#include "GuiRenderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Config.h"

#define ZNEAR 0.1f
#define ZFAR 1000.0f
#define FOV 45.0f
#define HEIGHT 1080.0f
#define WIDTH 1920.0f

static const std::array<float, 8> positions = {
    0.0f, 1.0f, // bottom left
    0.0f, 0.0f, // top left
    1.0f, 1.0f, // bottom right
    1.0f, 0.0f // top right
};

void GuiRenderer::Init()
{
    m_Proj = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, -1.0f, 1.0f);
    m_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_sprite.vert",
        std::string(SOURCE_DIR) + "/res/shaders/shader_sprite.frag");
    m_Shader.Bind();
    m_Shader.SetUniform1i("u_Texture", 0);

    m_VertexLayout.Push<float>(2); // texture coordinates are the same as the position ones
    m_VBO.Init(m_VertexLayout.GetStride(), 0);
    m_VBO.CreateStatic(positions.size() * sizeof(float), positions.data());
    m_VAO.Init();
    m_VBO.Bind(m_VAO.GetId());
    m_VAO.AddLayout(m_VertexLayout, 0);
}

void GuiRenderer::Render(const Texture& texture, const glm::vec2& position,
    const glm::vec2& scale)
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
        model, glm::vec3(position.x - scale.x / 2.0f, position.y - scale.y / 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));
    glm::mat4 mvp = m_Proj * model;
    m_Shader.Bind();
    m_Shader.SetUniformMat4f("u_MVP", mvp);
    texture.Bind(0);
    m_VAO.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
}
