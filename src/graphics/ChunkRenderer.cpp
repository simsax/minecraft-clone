#include "ChunkRenderer.h"
#include "TextureAtlas.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../utils/Logger.h"

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

    m_IBO.Init(m_Indices.size() * sizeof(uint32_t), m_Indices.data());
    m_VAO.Init();
    m_VAO.AddLayout(m_VertexLayout, m_BindingIndex);

    VertexBufferLayout outlineLayout;
    outlineLayout.Push<uint32_t>(1);
    m_OutlineVBO.Init(outlineLayout.GetStride(), m_BindingIndex);
    m_OutlineVBO.CreateDynamic(sizeof(uint32_t) * 24);
    m_OutlineVAO.Init();
    m_OutlineVAO.AddLayout(outlineLayout, m_BindingIndex);
    m_OutlineVBO.Bind(m_OutlineVAO.GetId());

    glLineWidth(4);
    glEnable(GL_LINE_SMOOTH);
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

void ChunkRenderer::RenderOutline(
    const glm::vec3& chunkPos, const glm::uvec3& voxel, size_t indexCount)
{
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

    m_OutlineShader.Bind();
    m_OutlineShader.SetUniformMat4f("u_MVP", mvp);
    m_OutlineShader.SetUniform3fv("u_ChunkPos", chunkPos);
    m_OutlineVAO.Bind();
    glDrawArrays(GL_LINE_STRIP, 0, indexCount);
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
