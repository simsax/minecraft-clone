#pragma once

#include "GL/glew.h"
#include <memory>
#include "../graphics/VertexArray.h"
#include "../graphics/IndexBuffer.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

class SkyRenderer {
public:
    void Init(glm::mat4* proj);
    void Render();
    void Resize(int width, int height);
    void SetSkyColor(const glm::vec4& skyColor);
    void SetFogColor(const glm::vec4& fogColor);
    void SetLowerLimit(float lowerLimit);

    // hack
    void RenderSkyBox(VertexArray& vao);
    void InitBox(glm::mat4* proj);

private:
    Shader m_Shader;
    VertexBuffer m_VBO;
    VertexBufferLayout m_VertexLayout;
    VertexArray m_VAO;
    glm::mat4* m_Proj;
    glm::mat4 m_MVP;
    glm::mat4 m_Model;
    float m_LowerLimit;
    glm::vec4 m_SkyColor;
    glm::vec4 m_FogColor;
    glm::vec2 m_Position;
    glm::vec2 m_Scale;

    // hack
    Shader m_BoxShader;
    glm::mat4 m_BoxMVP;
    glm::mat4* m_BoxProj;
    IndexBuffer m_BoxIbo;
};
