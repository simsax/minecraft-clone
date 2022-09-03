#pragma once

#include "Entity.h"
#include "glm/gtc/matrix_transform.hpp"

class QuadEntity : public Entity {
public:
    QuadEntity(std::string name, std::string texturePath, const glm::vec3 &position,
           const glm::vec3 &scale);
    void InitBuffers() override;
    void InitShaders(const std::string &vertShaderPath, const std::string &fragShaderPath) override;
    void Render(Renderer& renderer) override;

protected:
    static Shader s_Shader;
    static VertexBuffer s_VBO;
    static VertexBufferLayout s_VertexLayout;
    static VertexArray s_VAO;
};
