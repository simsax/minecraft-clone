#include "Renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Config.h"
#include "../camera/Constants.h"

Renderer::Renderer() :
        m_View(glm::mat4()),
        m_PersProj(glm::mat4()),
        m_OrthoProj(glm::mat4()),
        m_Visibility(0.5f),
        m_Increment(0.5f),
        m_DeltaTime(0.0f) {}

void Renderer::SetViewMatrix(const glm::mat4 &view) { m_View = view; }

void Renderer::Init(int width, int height) {
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    m_PersProj = glm::perspective(glm::radians(FOV),
                                  static_cast<float>(width) / static_cast<float>(height), ZNEAR,
                                  ZFAR);
    m_OrthoProj = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height),
                             -1.0f,
                             1.0f);
}

void Renderer::RenderChunk(
        const VertexArray &vao, const IndexBuffer &ibo, Shader& shader, const Texture& texture,
        GLenum type,
        const glm::vec3 &chunkPos, uint32_t offset, const glm::vec3& skyColor, const glm::vec3& sunColor) {
    glm::mat4 mvp = m_PersProj * m_View;
    shader.Bind();
    shader.SetUniformMat4f("u_MVP", mvp);
    shader.SetUniformMat4f("u_MV", m_View);
    shader.SetUniform3fv("u_ChunkPos", chunkPos);
    shader.SetUniform3fv("u_SkyColor", skyColor);
    shader.SetUniform3fv("u_SunColor", sunColor);
    texture.Bind(0);
    vao.Bind();
    ibo.Bind(vao.GetId());
    glDrawElementsBaseVertex(GL_TRIANGLES, ibo.GetCount(), type, nullptr, offset);
}

void Renderer::RenderQuad(const VertexArray &vao, Shader &shader, const Texture &texture,
                          const glm::mat4& model, bool ortho) {
    glm::mat4 mvp;
    if (ortho)
        mvp = m_OrthoProj * model;
    else
        mvp = m_PersProj * m_View * model;
    shader.Bind();
    shader.SetUniformMat4f("u_MVP", mvp);
    texture.Bind(0);
    vao.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Renderer::RenderOutline(
        const VertexArray &vao, const IndexBuffer &ibo, Shader& shader, GLenum type, const glm::vec3 &chunkPos,
        int i, int j, int k) {
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_STENCIL_TEST);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//    glStencilFunc(GL_ALWAYS, 1, 0xFF);
//    glStencilMask(0xFF);
    float scale = 1.002f;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.5 + i + chunkPos[0], 0.5 + j, 0.5 + k + chunkPos[2]));
    model = glm::scale(model, glm::vec3(scale));
    model = glm::translate(model,
                           glm::vec3(-0.5 - i - chunkPos[0], -0.5 - j, -0.5 - k - chunkPos[2]));
    glm::mat4 mvp = m_PersProj * m_View * model;
    if (m_Visibility < 0.5f || m_Visibility > 1.0f)
        m_Increment *= -1.0f;

    m_Visibility += m_Increment * m_DeltaTime;
//    glm::mat4 mvp = m_PersProj * m_View;

    shader.Bind();
    shader.SetUniformMat4f("u_MVP", mvp);
    shader.SetUniform3fv("u_ChunkPos", chunkPos);
    shader.SetUniform1f("u_Visibility", m_Visibility);
//    shader->SetUniform1i("u_Outline", false);
//    texture.Bind(0);
    vao.Bind();
    ibo.Bind(vao.GetId());
    glDrawElements(GL_TRIANGLES, ibo.GetCount(), type, nullptr);
//
//    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//    glStencilMask(0x00);
//    glDisable(GL_DEPTH_TEST);
//    scale = 1.1f;
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.5 + i + chunkPos[0], 0.5 + j, 0.5 + k + chunkPos[2]));
//    model = glm::scale(model, glm::vec3(scale));
//    model = glm::translate(model,
//                           glm::vec3(-0.5 - i - chunkPos[0], -0.5 - j, -0.5 - k - chunkPos[2]));
//
//    mvp = m_PersProj * m_View * model;
//    m_OutlineShader->Bind();
//    m_OutlineShader->SetUniformMat4f("u_MVP", mvp);
//    m_OutlineShader->SetUniform3fv("u_ChunkPos", chunkPos);
//    m_OutlineShader->SetUniform1i("u_Outline", true);
//    m_Texture->Bind(0);
//    vao.Bind();
//    ibo.Bind();
//    glDrawElements(GL_TRIANGLES, ibo.GetCount(), type, nullptr);
//    glStencilMask(0x00);
//    glStencilFunc(GL_ALWAYS, 0, 0xFF);
//    glDisable(GL_STENCIL_TEST);
//    glEnable(GL_DEPTH_TEST);
}

void Renderer::SetDeltaTime(float deltaTime) {
    m_DeltaTime = deltaTime;
}

void Renderer::Resize(int width, int height) {
    m_PersProj = glm::perspective(glm::radians(FOV),
                                  static_cast<float>(width) / static_cast<float>(height), ZNEAR,
                                  ZFAR);
    m_OrthoProj = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height),
                             -1.0f,
                             1.0f);
}

void Renderer::Clear(const glm::vec3 &skyColor) {
    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
