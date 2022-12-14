#include "SkyRenderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Config.h"
#include <array>
#include <cstdint>
#include "../utils/Logger.h"

static constexpr std::array<float, 8> positions = {
    0.0f, 1.0f, // bottom left
    0.0f, 0.0f, // top left
    1.0f, 1.0f, // bottom right
    1.0f, 0.0f // top right
};

void SkyRenderer::Init(glm::mat4* proj)
{
    m_Proj = proj;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
        model, glm::vec3(m_Position.x - m_Scale.x / 2.0f, m_Position.y - m_Scale.y / 2.0f, 0.0f));
    m_Model = glm::scale(model, glm::vec3(m_Scale.x, m_Scale.y, 1.0f));
    m_MVP = *m_Proj * m_Model;
    m_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_sky.vert",
        std::string(SOURCE_DIR) + "/res/shaders/shader_sky.frag");
    m_Shader.Bind();

    m_VertexLayout.Push<float>(2);
    m_VBO.Init(m_VertexLayout.GetStride(), 0);
    m_VBO.CreateStatic(positions.size() * sizeof(float), positions.data());
    m_VAO.Init();
    m_VBO.Bind(m_VAO.GetId());
    m_VAO.AddLayout(m_VertexLayout, 0);
}

void SkyRenderer::InitBox(glm::mat4* proj)
{
    m_BoxProj = proj;
    m_Proj = proj;
    static constexpr float scale = 1000.0f;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
        model, glm::vec3(-0.5f - scale / 2.0f, -0.5f - scale / 2.0f, -0.5f - scale / 2.0f));
    model = glm::scale(model, glm::vec3(scale));
    m_BoxMVP = *m_BoxProj * model;
    // hack
    m_BoxShader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_box.vert",
        std::string(SOURCE_DIR) + "/res/shaders/shader_box.frag");
    m_BoxShader.Bind();

    std::vector<uint32_t> indices;
    uint32_t offset = 0;

    for (size_t i = 0; i < 36; i += 6) {
        indices.push_back(0 + offset);
        indices.push_back(1 + offset);
        indices.push_back(2 + offset);

        indices.push_back(2 + offset);
        indices.push_back(3 + offset);
        indices.push_back(0 + offset);

        offset += 4;
    }
    m_BoxIbo.Init(indices.size() * sizeof(uint32_t), indices.data());
    m_BoxIbo.SetCount(36);
}

void SkyRenderer::Render()
{
    glDepthMask(0);
    m_Shader.Bind();
    m_Shader.SetUniform4fv("u_Color", m_SkyColor);
    m_Shader.SetUniform4fv("u_FogColor", m_FogColor);
    m_Shader.SetUniformMat4f("u_MVP", m_MVP);
    m_Shader.SetUniformMat4f("u_Model", m_Model);
    m_Shader.SetUniform1f("u_LowerLimit", m_LowerLimit);
    m_VAO.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDepthMask(1);
}

void SkyRenderer::RenderSkyBox(VertexArray& vao)
{
    // remember to resize just in case
    glDepthMask(0);
    m_BoxShader.Bind();
    m_BoxShader.SetUniformMat4f("u_MVP", m_BoxMVP);
    vao.Bind();
    m_BoxIbo.Bind(vao.GetId());
    glDrawElementsBaseVertex(GL_TRIANGLES, m_BoxIbo.GetCount(), GL_UNSIGNED_INT, nullptr, 0);
    glDepthMask(1);
}

void SkyRenderer::Resize(int width, int height)
{
    m_Position = { static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f };
    m_Scale = { static_cast<float>(width), static_cast<float>(height) };
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
        model, glm::vec3(m_Position.x - m_Scale.x / 2.0f, m_Position.y - m_Scale.y / 2.0f, 0.0f));
    m_Model = glm::scale(model, glm::vec3(m_Scale.x, m_Scale.y, 1.0f));
    m_MVP = *m_Proj * m_Model;
}

void SkyRenderer::SetSkyColor(const glm::vec4& skyColor) { m_SkyColor = skyColor; }

void SkyRenderer::SetFogColor(const glm::vec4& fogColor) { m_FogColor = fogColor; }

void SkyRenderer::SetLowerLimit(float lowerLimit) { m_LowerLimit = lowerLimit; }
