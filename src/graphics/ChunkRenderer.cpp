#include "ChunkRenderer.h"
#include "TextureAtlas.h"
#include "glm/gtc/matrix_transform.hpp"

#define MAX_INDEX_COUNT 18432 // each cube has 6 faces, each face has 6 indexes

struct Vertex {
    uint32_t base;
    uint16_t light;
};

ChunkRenderer::ChunkRenderer()
    : m_BindingIndex(0)
    , m_Visibility(0.5f)
    , m_Increment(0.5f)
    , m_DeltaTime(0)
{
    m_VertexLayout.Push<uint32_t>(1); // position + texture coords + normals
    m_VertexLayout.Push<uint32_t>(1); // lighting

    m_Indices.reserve(MAX_INDEX_COUNT);
    uint32_t offset = 0;

    for (size_t i = 0; i < MAX_INDEX_COUNT * 2; i += 6) {
        m_Indices.push_back(0 + offset);
        m_Indices.push_back(1 + offset);
        m_Indices.push_back(2 + offset);

        m_Indices.push_back(2 + offset);
        m_Indices.push_back(3 + offset);
        m_Indices.push_back(0 + offset);

        offset += 4;
    }
}

void ChunkRenderer::Init(glm::mat4* proj, glm::mat4* view)
{
    m_Proj = proj;
    m_View = view;
    TextureAtlas::GetTexture().Init();

    m_ChunkShader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_chunk.vert",
        std::string(SOURCE_DIR) + "/res/shaders/shader_chunk.frag");
    m_ChunkShader.Bind();
    m_ChunkShader.SetUniform1i("u_Texture", 0);

    m_OutlineShader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_outline.vert",
        std::string(SOURCE_DIR) + "/res/shaders/shader_outline.frag");

    //    m_OutlineShader.Bind();
    //    m_OutlineShader.SetUniform1i("u_Texture", 0);
    m_IBO.Init(m_Indices.size() * sizeof(uint32_t), m_Indices.data());
    m_VAO.Init();
    m_VAO.AddLayout(m_VertexLayout, m_BindingIndex);
    m_OutlineVBO.Init(m_VertexLayout.GetStride(), m_BindingIndex);
    m_OutlineVBO.CreateDynamic(sizeof(Vertex) * 24);
}

void ChunkRenderer::Render(const glm::vec3& chunkPosition, uint32_t offset)
{
    glm::mat4 mvp = *m_Proj * *m_View;
    m_ChunkShader.Bind();
    m_ChunkShader.SetUniformMat4f("u_MVP", mvp);
    m_ChunkShader.SetUniformMat4f("u_MV", *m_View);
    m_ChunkShader.SetUniform3fv("u_ChunkPos", chunkPosition);
    m_ChunkShader.SetUniform3fv("u_SkyColor", m_SkyColor);
    // m_ChunkShader.SetUniform3fv("u_SunColor", m_SunColor);
    // m_ChunkShader.SetUniform3fv("u_ViewPos", m_ViewPos);
    // m_ChunkShader.SetUniform3fv("u_LightDir", m_SunDir);
    // m_ChunkShader.SetUniform1i("u_IsDay", m_IsDay);
    // m_ChunkShader.SetUniform1f("u_AmbientStrength", m_AmbientStrength);
    TextureAtlas::GetTexture().Bind(0);
    m_VAO.Bind();
    m_IBO.Bind(m_VAO.GetId());
    glDrawElementsBaseVertex(GL_TRIANGLES, m_IBO.GetCount(), GL_UNSIGNED_INT, nullptr, offset);
}

void ChunkRenderer::RenderOutline(const glm::vec3& chunkPos, const glm::uvec3& voxel)
{
    m_OutlineVBO.Bind(m_VAO.GetId());
    //    glDisable(GL_DEPTH_TEST);
    //    glEnable(GL_STENCIL_TEST);
    //    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    //    glStencilMask(0xFF);
    uint32_t i = voxel.x;
    uint32_t j = voxel.y;
    uint32_t k = voxel.z;
    float scale = 1.002f;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.5 + i + chunkPos[0], 0.5 + j, 0.5 + k + chunkPos[2]));
    model = glm::scale(model, glm::vec3(scale));
    model = glm::translate(
        model, glm::vec3(-0.5 - i - chunkPos[0], -0.5 - j, -0.5 - k - chunkPos[2]));
    glm::mat4 mvp = *m_Proj * *m_View * model;
    if (m_Visibility < 0.5f || m_Visibility > 1.0f)
        m_Increment *= -1.0f;

    m_Visibility += m_Increment * m_DeltaTime;
    //    glm::mat4 mvp = m_PersProj * m_View;

    m_OutlineShader.Bind();
    m_OutlineShader.SetUniformMat4f("u_MVP", mvp);
    m_OutlineShader.SetUniform3fv("u_ChunkPos", chunkPos);
    m_OutlineShader.SetUniform1f("u_Visibility", m_Visibility);
    //    shader->SetUniform1i("u_Outline", false);
    //    texture.Bind(0);
    m_VAO.Bind();
    m_IBO.Bind(m_VAO.GetId());
    glDrawElements(GL_TRIANGLES, m_IBO.GetCount(), GL_UNSIGNED_INT, nullptr);
    //
    //    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //    glStencilMask(0x00);
    //    glDisable(GL_DEPTH_TEST);
    //    scale = 1.1f;
    //    model = glm::mat4(1.0f);
    //    model = glm::translate(model, glm::vec3(0.5 + i + chunkPos[0], 0.5 + j, 0.5 + k +
    //    chunkPos[2])); model = glm::scale(model, glm::vec3(scale)); model = glm::translate(model,
    //                           glm::vec3(-0.5 - i - chunkPos[0], -0.5 - j, -0.5 - k -
    //                           chunkPos[2]));
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

void ChunkRenderer::SetSkyColor(const glm::vec3& skyColor) { m_SkyColor = skyColor; }

void ChunkRenderer::SetSunColor(const glm::vec3& sunColor) { m_SunColor = sunColor; }

void ChunkRenderer::SetLightPos(const glm::vec3& lightPos) { m_LightPos = lightPos; }

void ChunkRenderer::SetViewPos(const glm::vec3& viewPos) { m_ViewPos = viewPos; }

void ChunkRenderer::SetAmbientStrength(float ambientStrength)
{
    m_AmbientStrength = ambientStrength;
}

void ChunkRenderer::SetSunDir(const glm::vec3& sunDir) { m_SunDir = sunDir; }

void ChunkRenderer::SetIsDay(bool isDay) { m_IsDay = isDay; }

void ChunkRenderer::SetIboCount(size_t count) { m_IBO.SetCount(count); }

uint32_t ChunkRenderer::GetVaoId() const { return m_VAO.GetId(); }

uint32_t ChunkRenderer::GetStride() const { return m_VertexLayout.GetStride(); }

void ChunkRenderer::SendOutlineData(GLsizeiptr size, const void* data, uint32_t offset)
{
    m_OutlineVBO.SendData(size, data, offset);
}

void ChunkRenderer::SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }
