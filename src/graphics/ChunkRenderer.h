#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "IndexBuffer.h"

class ChunkRenderer {
public:
    ChunkRenderer();
    void Init(glm::mat4* proj, glm::mat4* view);
    void Render(const glm::vec3& chunkPosition, uint32_t offset);
    void RenderOutline(const glm::vec3& chunkPosition, const glm::uvec3& voxel);
    void SetSkyColor(const glm::vec3& skyColor);
    void SetSunColor(const glm::vec3& sunColor);
    void SetSunDir(const glm::vec3& sunDir);
    void SetIsDay(bool isDay);
    void SetLightPos(const glm::vec3& lightPos);
    void SetViewPos(const glm::vec3& viewPos);
    void SetAmbientStrength(float ambientStrength);
    void SetIboCount(size_t count);
    uint32_t GetVaoId() const;
    uint32_t GetStride() const;
    void SendOutlineData(GLsizeiptr size, const void *data, uint32_t offset);
    void SetDeltaTime(float deltaTime);

private:
    Shader m_ChunkShader;
    Shader m_OutlineShader;
    VertexBufferLayout m_VertexLayout;
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    VertexBuffer m_OutlineVBO;
    std::vector<uint32_t> m_Indices;
    int m_BindingIndex;

    glm::mat4* m_Proj;
    glm::mat4* m_View;
    glm::vec3 m_SkyColor{};
    glm::vec3 m_SunColor{};
    glm::vec3 m_SunDir{};
    glm::vec3 m_ViewPos{};
    glm::vec3 m_LightPos{};
    bool m_IsDay{};
    float m_AmbientStrength{};
    float m_Visibility;
    float m_Increment;
    float m_DeltaTime;
};
