#include "Renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Config.h"

#define ZNEAR 0.2f
#define ZFAR 1000.0f
#define FOV 45.0f
#define HEIGHT 1080.0f
#define WIDTH 1920.0f

Renderer::Renderer() :
        m_Texture(std::string(SOURCE_DIR) + "/res/textures/terrain.png"),
        m_View(glm::mat4()),
        m_SkyColor(glm::vec3()),
        m_Proj(glm::mat4()),
        m_Visibility(0.5f),
        m_Increment(0.5f),
        m_DeltaTime(0.0f)
{}

void Renderer::Clear() const { glClear(GL_COLOR_BUFFER_BIT); }

void Renderer::SetViewMatrix(const glm::mat4 &view) { m_View = view; }

void Renderer::SetSkyColor(const glm::vec3 &skyColor) { m_SkyColor = skyColor; }

void Renderer::Init() {
    m_Texture.Init();
    m_Proj = glm::perspective(glm::radians(FOV), WIDTH / HEIGHT, ZNEAR, ZFAR);
    m_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader.vert",
                                        std::string(SOURCE_DIR) + "/res/shaders/shader.frag");
    m_Shader.Bind();
    m_Shader.SetUniform1i("u_Texture", 0);

    m_OutlineShader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_outline.vert",
                                       std::string(SOURCE_DIR) +
                                       "/res/shaders/shader_outline.frag");

    m_OutlineShader.Bind();
    m_OutlineShader.SetUniform1i("u_Texture", 0);
}

void Renderer::Draw(
        const VertexArray &vao, const IndexBuffer &ibo, GLenum type,
        const glm::vec3 &chunkPos, uint32_t offset) {
    glm::mat4 mvp = m_Proj * m_View;
    m_Shader.Bind();
    m_Shader.SetUniformMat4f("u_MVP", mvp);
    m_Shader.SetUniformMat4f("u_MV", m_View);
    m_Shader.SetUniform3fv("u_ChunkPos", chunkPos);
    m_Shader.SetUniform3fv("u_SkyColor", m_SkyColor);
    m_Texture.Bind(0);
    vao.Bind();
    ibo.Bind(vao.GetId());
    glDrawElementsBaseVertex(GL_TRIANGLES, ibo.GetCount(), type, nullptr, offset);
}

void Renderer::RenderOutline(
        const VertexArray &vao, const IndexBuffer &ibo, GLenum type, const glm::vec3 &chunkPos,
        int i, int j, int k) {
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_STENCIL_TEST);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//    glStencilFunc(GL_ALWAYS, 1, 0xFF);
//    glStencilMask(0xFF);
//    float scale = 1.001f;
//    glm::mat4 model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.5 + i + chunkPos[0], 0.5 + j, 0.5 + k + chunkPos[2]));
//    model = glm::scale(model, glm::vec3(scale));
//    model = glm::translate(model,
//                           glm::vec3(-0.5 - i - chunkPos[0], -0.5 - j, -0.5 - k - chunkPos[2]));
//    glm::mat4 mvp = m_Proj * m_View * model;
    if (m_Visibility < 0.5f || m_Visibility > 1.0f)
        m_Increment *= -1.0f;

    m_Visibility += m_Increment * m_DeltaTime;
    glm::mat4 mvp = m_Proj * m_View;

    m_OutlineShader.Bind();
    m_OutlineShader.SetUniformMat4f("u_MVP", mvp);
    m_OutlineShader.SetUniform3fv("u_ChunkPos", chunkPos);
    m_OutlineShader.SetUniform1f("u_Visibility", m_Visibility);
//    m_OutlineShader->SetUniform1i("u_Outline", false);
    m_Texture.Bind(0);
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
//    mvp = m_Proj * m_View * model;
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
