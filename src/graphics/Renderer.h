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

    void RenderChunk(const VertexArray &vao, const IndexBuffer &ibo, Shader &shader,
                     const Texture &texture, GLenum type, const glm::vec3 &chunkPos,
                     uint32_t offset, const glm::vec3 &skyColor, const glm::vec3 &sunColor,
                     const glm::vec3 &viewPos, const glm::vec3& lightPos, bool isDay,
                     float ambientStrength);

    void RenderQuad(const VertexArray &vao, Shader &shader, const Texture &texture,
                    const glm::mat4 &model, bool ortho);
    void RenderQuad(const VertexArray &vao, Shader &shader, const std::array<glm::vec4, 4>& color,
                    const glm::mat4 &model, bool ortho);
    void RenderSky(const VertexArray &vao, Shader &shader, const glm::vec4& color, const glm::vec4& fogColor,
                    const glm::mat4 &model, bool ortho, float lowerLimit);

    void RenderOutline(const VertexArray &vao, const IndexBuffer &ibo, Shader &shader, GLenum type,
                       const glm::vec3 &chunkPos, int i, int j, int k);

    void SetViewMatrix(const glm::mat4 &mv);

    void SetSkyColor(const glm::vec3 &skyColor);

    void SetDeltaTime(float deltaTime);

    void Resize(int width, int height);

    static void Clear(const glm::vec4 &skyColor);

private:
    glm::mat4 m_View;
    glm::mat4 m_PersProj;
    glm::mat4 m_OrthoProj;
    float m_Visibility;
    float m_Increment;
    float m_DeltaTime;
};
