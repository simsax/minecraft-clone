#include "QuadEntity.h"
#include "Config.h"
#include <mutex>

VertexBuffer QuadEntity::s_VBO = {};
VertexBufferLayout QuadEntity::s_VertexLayout = {};
VertexArray QuadEntity::s_VAO = {};

static std::once_flag fBuffers;

static constexpr std::array<float, 12> positions = {
        0.0f, 0.0f, 1.0f, // bottom left
        0.0f, 0.0f, 0.0f, // top left
        1.0f, 0.0f, 1.0f, // bottom right
        1.0f, 0.0f, 0.0f // top right
};

QuadEntity::QuadEntity(std::string name, std::string texturePath, const glm::vec3 &position,
                       const glm::vec3 &scale) : Entity(std::move(name), std::move(texturePath),
                                                        position, scale) {}

void QuadEntity::InitBuffers() {
    std::call_once(fBuffers, [](){
        s_VertexLayout.Push<float>(3);
        s_VBO.Init(s_VertexLayout.GetStride(), 0);
        s_VBO.CreateStatic(positions.size() * sizeof(float), positions.data());
        s_VAO.Init();
        s_VBO.Bind(s_VAO.GetId());
        s_VAO.AddLayout(s_VertexLayout, 0);
    });
}
