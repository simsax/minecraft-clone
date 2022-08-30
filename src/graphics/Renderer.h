#pragma once
#include "GL/glew.h"
#include <memory>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"

class Renderer {
public:
    Renderer();
    void Init(int width, int height);
    void Draw(const VertexArray& vao, const IndexBuffer& ibo, GLenum type,
        const glm::vec3& chunkPos, uint32_t offset);
    void RenderOutline(const VertexArray& vao, const IndexBuffer& ibo, GLenum type,
        const glm::vec3& chunkPos, int i, int j, int k);
    void Clear() const;
    void SetViewMatrix(const glm::mat4& mv);
    void SetSkyColor(const glm::vec3& skyColor);
    void SetDeltaTime(float deltaTime);
    void Resize(int width, int height);

private:
    Shader m_Shader;
    Shader m_OutlineShader;
    Texture m_Texture;
    glm::mat4 m_View;
    glm::vec3 m_SkyColor;
    glm::mat4 m_Proj;
    float m_Visibility;
    float m_Increment;
    float m_DeltaTime;
};
