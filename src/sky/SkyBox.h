#include "GL/glew.h"
#include "glm/glm.hpp"
#include "../graphics/SkyRenderer.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

class SkyBox {
public:
    SkyBox();
    void Render(SkyRenderer& renderer);

private:
    std::vector<Vertex> m_Mesh;
    VertexBufferLayout m_Layout;
    VertexBuffer m_VBO;
    VertexArray m_VAO;
};
