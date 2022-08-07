#pragma once
#include "GL/glew.h"
#include <memory>
#include "../graphics/VertexArray.h"
#include "../graphics/IndexBuffer.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

class GuiRenderer {
public:
    void Init();
    void Render(const std::shared_ptr<Texture>& texture, const glm::vec2& position,
        const glm::vec2& scale) const;

private:
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<VertexBuffer> m_VBO;
    VertexBufferLayout m_VertexLayout;
    std::unique_ptr<VertexArray> m_VAO;
    glm::mat4 m_MV;
    glm::mat4 m_Proj;
};
