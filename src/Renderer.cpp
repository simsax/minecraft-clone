#include "Renderer.h"
#include "Config.h"

void Renderer::Clear() const { glClear(GL_COLOR_BUFFER_BIT); }

void Renderer::SetMVP(const glm::mat4& mvp) { m_MVP = mvp; }

void Renderer::SetMV(const glm::mat4& mv) { m_MV = mv; }

void Renderer::SetSkyColor(const glm::vec3& skyColor) { m_SkyColor = skyColor; }

void Renderer::Init()
{
    m_Shader = std::make_unique<Shader>(std::string(SOURCE_DIR) + "/res/shaders/");
    m_Shader->Bind();

    m_Texture = std::make_unique<Texture>(std::string(SOURCE_DIR) + "/res/textures/terrain.png");
    m_Texture->Bind(0);
    m_Shader->SetUniform1i("u_Texture", 0);
}

void Renderer::Draw(
    const VertexArray& vao, const IndexBuffer& ibo, GLenum type, const glm::vec3& chunkPos) const
{
    m_Shader->SetUniformMat4f("u_MVP", m_MVP);
    m_Shader->SetUniformMat4f("u_MV", m_MV);
    m_Shader->SetUniform3fv("u_ChunkPos", chunkPos);
    m_Shader->SetUniform3fv("u_SkyColor", m_SkyColor);
    m_Shader->Bind();
    vao.Bind();
    ibo.Bind();
    glDrawElements(GL_TRIANGLES, ibo.GetCount(), type, nullptr);
}

void Renderer::DrawInstanced(
    const VertexArray& vao, const IndexBuffer& ibo, GLenum type, GLsizei num_instances) const
{
    m_Shader->SetUniformMat4f("u_MVP", m_MVP);
    m_Shader->Bind();
    vao.Bind();
    ibo.Bind();
    glDrawElementsInstanced(GL_TRIANGLES, ibo.GetCount(), type, nullptr, num_instances);
}
