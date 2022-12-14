#include "SkyBox.h"

static void CreateQuad(std::vector<Vertex>& target, const glm::uvec3& position,
    const glm::uvec4& offsetx, const glm::uvec4& offsety, const glm::uvec4& offsetz,
    const glm::vec3& color)
{
    static constexpr int vertices = 4;

    for (int i = 0; i < vertices; i++) {
        Vertex v = { glm::vec3(position[0] + offsetx[i], position[1] + offsety[i],
                         position[2] + offsetz[i]),
            color };
        target.emplace_back(v);
    }
}

static void GenCube(int i, int j, int k, std::vector<Vertex>& target)
{
    glm::vec3 color = { 0.0, 0.0, 0.0 }; // value for each vertex

    CreateQuad(target, { i, j, k }, // D
        { 0, 0, 1, 1 }, glm::uvec4(0), { 1, 0, 0, 1 }, color);
    CreateQuad(target, { i, j, k }, // U
        { 1, 1, 0, 0 }, glm::uvec4(1), { 1, 0, 0, 1 }, color);
    CreateQuad(target, { i, j, k }, // F
        { 0, 0, 1, 1 }, { 1, 0, 0, 1 }, glm::uvec4(1), color);
    CreateQuad(target, { i, j, k }, // B
        { 1, 1, 0, 0 }, { 1, 0, 0, 1 }, glm::uvec4(0), color);
    CreateQuad(target, { i, j, k }, // L
        glm::uvec4(0), { 1, 0, 0, 1 }, { 0, 0, 1, 1 }, color);
    CreateQuad(target, { i, j, k }, // R
        glm::uvec4(1), { 1, 0, 0, 1 }, { 1, 1, 0, 0 }, color);
}

SkyBox::SkyBox()
{
    static constexpr uint8_t numVertices = 24;
    m_Layout.Push<float>(3);
    m_Layout.Push<float>(3);
    m_VBO.Init(m_Layout.GetStride(), 0);
    m_VBO.CreateDynamic(sizeof(Vertex) * numVertices);
    m_Mesh.reserve(numVertices);
    m_VAO.Init();
    m_VAO.AddLayout(m_Layout, 0);
    m_VBO.Bind(m_VAO.GetId());

    GenCube(0, 0, 0, m_Mesh);
    m_VBO.SendData(m_Mesh.size() * sizeof(Vertex), m_Mesh.data(), 0);
}

void SkyBox::Render(SkyRenderer& renderer) { renderer.RenderSkyBox(m_VAO); }