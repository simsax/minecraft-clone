#pragma once
#include "GL/glew.h"
#include <memory>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"

class Renderer {
public:
    void Init();
    void Draw(const VertexArray& vao, const IndexBuffer& ibo, GLenum type,
        const glm::vec3& chunkPos) const;
    void RenderOutline(const VertexArray& vao, const IndexBuffer& ibo, GLenum type,
        const glm::vec3& chunkPos, int i, int j, int k);
    void Clear() const;
    void SetViewMatrix(const glm::mat4& mv);
    void SetSkyColor(const glm::vec3& skyColor);
    void SetDeltaTime(float deltaTime);

private:
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Shader> m_OutlineShader;
    std::unique_ptr<Texture> m_Texture;
    glm::mat4 m_View;
    glm::vec3 m_SkyColor;
    glm::mat4 m_Proj;
    float m_Visibility;
    float m_Increment;
    float m_DeltaTime;
};
