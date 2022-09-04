#pragma once

#include "Entity.h"
#include "glm/gtc/matrix_transform.hpp"

class QuadEntity : public Entity {
public:
    QuadEntity(std::string name, std::string texturePath, const glm::vec3 &position,
           const glm::vec3 &scale);
    static void InitBuffers(); // all quads share the same vbo
    void Render(Renderer& renderer) override = 0;

protected:
    static VertexBuffer s_VBO;
    static VertexBufferLayout s_VertexLayout;
    static VertexArray s_VAO;
};
