#pragma once

#include "GL/glew.h"
#include <memory>
#include "../graphics/VertexArray.h"
#include "../graphics/IndexBuffer.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

namespace SkyColors {
    static constexpr glm::vec4 tc1 = {83.0f / 255.0f, 210.0f / 255.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 tc2 = {11.0f / 255.0f, 26.0f / 255.0f, 51.0f / 255.0f, 1.0f};
    static constexpr glm::vec4 bc1 = {211.0f / 255.0f, 232.0f/ 255.0f, 232 / 255.0f, 1.0f};
    static constexpr glm::vec4 bc2 = {242.0f / 255.0f , 110.0f / 255.0f, 34.0f / 255.0f, 1.0f};
};

class SkyRenderer {
public:
    void Init(glm::mat4* proj);
    void Render();
    void Resize(int width, int height);
    void SetSkyColor(const glm::vec4& skyColor);
    void SetFogColor(const glm::vec4& fogColor);
    void SetLowerLimit(float lowerLimit);

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
};
