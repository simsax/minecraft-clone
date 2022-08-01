#include "Renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Config.h"

#define ZNEAR 0.1f
#define ZFAR 1000.0f
#define FOV 45.0f
#define HEIGHT 1080.0f
#define WIDTH 1920.0f

void Renderer::Clear() const { glClear(GL_COLOR_BUFFER_BIT); }

void Renderer::SetMV(const glm::mat4& mv) { m_MV = mv; }

void Renderer::SetSkyColor(const glm::vec3& skyColor) { m_SkyColor = skyColor; }

void Renderer::Init()
{
    m_Proj = glm::perspective(glm::radians(FOV), WIDTH / HEIGHT, ZNEAR, ZFAR);
    m_Shader = std::make_unique<Shader>(std::string(SOURCE_DIR) + "/res/shaders/shader.vert",
        std::string(SOURCE_DIR) + "/res/shaders/shader.frag");
    m_Shader->Bind();
    m_Shader->SetUniform1i("u_Texture", 0);

    m_Texture = std::make_unique<Texture>(std::string(SOURCE_DIR) + "/res/textures/terrain.png");
}

void Renderer::Draw(
    const VertexArray& vao, const IndexBuffer& ibo, GLenum type, const glm::vec3& chunkPos) const
{
    glm::mat4 mvp = m_Proj * m_MV;
    m_Shader->SetUniformMat4f("u_MVP", mvp);
    m_Shader->SetUniformMat4f("u_MV", m_MV);
    m_Shader->SetUniform3fv("u_ChunkPos", chunkPos);
    m_Shader->SetUniform3fv("u_SkyColor", m_SkyColor);
    m_Texture->Bind(0);
    m_Shader->Bind();
    vao.Bind();
    ibo.Bind();
    glDrawElements(GL_TRIANGLES, ibo.GetCount(), type, nullptr);
}

void Renderer::DrawInstanced(
    const VertexArray& vao, const IndexBuffer& ibo, GLenum type, GLsizei num_instances) const
{
    glm::mat4 mvp = m_Proj * m_MV;
    m_Shader->SetUniformMat4f("u_MVP", mvp);
    m_Shader->Bind();
    vao.Bind();
    ibo.Bind();
    glDrawElementsInstanced(GL_TRIANGLES, ibo.GetCount(), type, nullptr, num_instances);
}
