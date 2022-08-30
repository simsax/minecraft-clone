#pragma once
#include "GL/glew.h"
#include <memory>
#include "../graphics/VertexArray.h"
#include "../graphics/IndexBuffer.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

class GuiRenderer {
public:
    void Init(int width, int height);
    void Render(const Texture& texture, const glm::vec2& position,
        const glm::vec2& scale);
    void Resize(int width, int height);

private:
    Shader m_Shader;
    VertexBuffer m_VBO;
    VertexBufferLayout m_VertexLayout;
    VertexArray m_VAO;
    glm::mat4 m_MV;
    glm::mat4 m_Proj;
};
