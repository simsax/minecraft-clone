#pragma once
#include <GL/glew.h>
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
    void DrawInstanced(
        const VertexArray& vao, const IndexBuffer& ibo, GLenum type, GLsizei num_instances) const;
    void Clear() const;
    void SetMVP(const glm::mat4& mvp);
    void SetMV(const glm::mat4& mv);
    void SetSkyColor(const glm::vec3& skyColor);

private:
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Texture> m_Texture;
    glm::mat4 m_MVP;
    glm::mat4 m_MV;
    glm::vec3 m_SkyColor;
};