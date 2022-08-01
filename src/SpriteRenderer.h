#pragma once
#include <GL/glew.h>
#include <memory>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"

class SpriteRenderer {
public:
    void Init();
    void Draw() const;
    void Clear() const;
    void SetMV(const glm::mat4& mv);
    void SetSkyColor(const glm::vec3& skyColor);

private:
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Texture> m_Texture;
    std::unique_ptr<VertexBuffer> m_VBO;
    VertexBufferLayout m_VertexLayout;
    std::unique_ptr<VertexArray> m_VAO;
    glm::mat4 m_MV;
    glm::vec3 m_SkyColor;
    glm::mat4 m_Proj;
};