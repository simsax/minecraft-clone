#pragma once
#include "GL/glew.h"
#include <memory>
#include "../graphics/VertexArray.h"
#include "../graphics/IndexBuffer.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

class QuadRenderer {
public:
    void Init(glm::mat4* proj, glm::mat4* view);
    void Render(const Texture& texture, const glm::vec2& position,
                const glm::vec2& scale);
    void Render(const Texture& texture, const glm::mat4& model);

private:
    Shader m_Shader;
    VertexBuffer m_VBO;
    VertexBufferLayout m_VertexLayout;
    VertexArray m_VAO;
    glm::mat4* m_Proj;
    glm::mat4* m_View;
};
