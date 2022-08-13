#include "Chunk.h"

#include <cstdlib>
#include "glm/gtx/norm.hpp"
#include <iostream>
#include <map>

#include "../camera/Camera.h"

bool operator==(const ChunkCoord& l, const ChunkCoord& r) { return l.x == r.x && l.z == r.z; }

bool operator!=(const ChunkCoord& l, const ChunkCoord& r) { return l.x != r.x || l.z != r.z; }

int operator-(const ChunkCoord& l, const ChunkCoord& r)
{
    return static_cast<int>(round(sqrt(pow(l.x - r.x, 2) + pow(l.z - r.z, 2))));
}

std::size_t hash_fn::operator()(const ChunkCoord& coord) const
{
    std::size_t h1 = std::hash<int>()(coord.x);
    std::size_t h2 = std::hash<int>()(coord.z);
    return h1 ^ h2;
}

static std::array<uint32_t, 4> GenerateTextCoords(const glm::uvec2& textureCoords)
{
    uint32_t t0 = (textureCoords[0] << 4 | textureCoords[1]) << 2;
    uint32_t t1 = t0 | 1;
    uint32_t t2 = t0 | 2;
    uint32_t t3 = t0 | 3;
    return { t0, t1, t2, t3 };
}

static void CreateQuad(std::vector<uint32_t>& target, const glm::uvec3& position,
    const glm::uvec2& textureCoords, const glm::uvec4& offsetx, const glm::uvec4& offsety,
    const glm::uvec4& offsetz)
{
    std::array<uint32_t, 4> t = GenerateTextCoords({ textureCoords[0], textureCoords[1] });

    uint32_t v0 = (position[0] + offsetx[0]) << 23 | (position[1] + offsety[0]) << 15
        | (position[2] + offsetz[0]) << 11 | t[0];
    uint32_t v1 = (position[0] + offsetx[1]) << 23 | (position[1] + offsety[1]) << 15
        | (position[2] + offsetz[1]) << 11 | t[1];
    uint32_t v2 = (position[0] + offsetx[2]) << 23 | (position[1] + offsety[2]) << 15
        | (position[2] + offsetz[2]) << 11 | t[2];
    uint32_t v3 = (position[0] + offsetx[3]) << 23 | (position[1] + offsety[3]) << 15
        | (position[2] + offsetz[3]) << 11 | t[3];

    target.emplace_back(v0);
    target.emplace_back(v1);
    target.emplace_back(v2);
    target.emplace_back(v3);
}

// the chunk has a border so that I know what faces to cull between chunks
// (I only generate the mesh of the part inside the border)
Chunk::Chunk(const glm::vec3& position, uint32_t maxVertexCount,
    const std::vector<uint32_t>& indices, const VertexBufferLayout& layout, int bindingIndex)
    : m_HeightMap({})
    , m_ChunkPosition(position)
    , m_Chunk(Matrix3D<Block, XSIZE, YSIZE, ZSIZE>())
    , m_MaxVertexCount(maxVertexCount)
    , m_MinHeight(YSIZE)
    , m_MaxHeight(0)
    , m_IBOCount(0)
    , m_TIBOCount(0)
{
    m_VBO.Init(layout.GetStride(), bindingIndex);
    m_VBO.CreateDynamic(sizeof(uint32_t) * maxVertexCount);

    m_Mesh.reserve(m_MaxVertexCount);
    m_TransparentMesh.reserve(maxVertexCount / 4);
    CreateHeightMap();
    FastFill();
    CreateSurfaceLayer();
}

float Chunk::Continentalness(int x, int y)
{
    float scale = 256.0f;
    float height = 0;

    float noise = m_Noise.OctaveNoise(x, y, 8, 0.01f);

    if (noise < 0.3) {
        // height = (noise + 2.3f) / 0.026f;
        height = (200.0f * noise + 980) / 13.0f;
    } else if (0.3 <= noise && noise < 0.8) {
        // height = 40 * noise + 88;
        height = 40 * noise + 68;
    } else if (noise >= 0.8) {
        // height = 120;
        height = 100;
    }

    return height;
}

void Chunk::CreateHeightMap()
{
    int elevation = 50; // increase for less water
    int index = 0;

    for (int i = 0; i < XSIZE; i++) {
        for (int k = 0; k < ZSIZE; k++) {
            float a
                = (m_Noise.OctaveNoise(i + m_ChunkPosition.x, k + m_ChunkPosition.z, 6, 0.002f) + 1)
                / 2;
            float m = (m_Noise.OctaveNoise(
                           i + m_ChunkPosition.x + 123, k + m_ChunkPosition.z + 456, 8, 0.01f)
                          + 1)
                / 2;
            a *= a;
            float terrain_height;
            if (a < 0.5)
                terrain_height = m * a * a * 2;
            else
                terrain_height = m * (1 - (1 - a) * (1 - a) * 2);
            terrain_height *= 200.0f;
            terrain_height = static_cast<int>(terrain_height + elevation);
            if (terrain_height < 40)
                terrain_height = 40;
            int height = static_cast<int>(
                m_Noise.OctaveNoise(i + m_ChunkPosition.x, k + m_ChunkPosition.z, 8) * 5.0f);

            int final_height = height + terrain_height;
            if (final_height > YSIZE)
                final_height = YSIZE;
            m_MinHeight = std::min(m_MinHeight, final_height);
            m_MaxHeight = std::max(m_MaxHeight, final_height);
            m_HeightMap[index++] = final_height;
        }
    }
}

void Chunk::FastFill()
{
    int level_size = XSIZE * ZSIZE;
    memset(m_Chunk.GetRawPtr(), (int)Block::BEDROCK, level_size);
    memset(m_Chunk.GetRawPtr() + level_size, (int)Block::STONE, level_size * (m_MinHeight - 1));
    memset(m_Chunk.GetRawPtr() + level_size * (m_MaxHeight + 1), (int)Block::EMPTY,
        level_size * (YSIZE - m_MaxHeight - 1));
}

void Chunk::CreateSurfaceLayer()
{
    int water_level = 63;
    int snow_level = 120;
    if (m_MaxHeight < water_level)
        m_MaxHeight = water_level;
    for (int j = m_MinHeight; j <= m_MaxHeight; j++) {
        int index = 0;
        for (int i = 0; i < XSIZE; i++) {
            for (int k = 0; k < ZSIZE; k++) {
                int height = m_HeightMap[index++];
                if (j < height) {
                    int dirtThickness
                        = static_cast<int>((m_Noise.OctaveNoise(i + m_ChunkPosition.x + 111,
                                                k + m_ChunkPosition.z + 111, 8)
                                               + 1)
                              / 2 * 10)
                        - j;
                    if (dirtThickness > 0)
                        m_Chunk(i, j, k) = Block::DIRT;
                    else
                        m_Chunk(i, j, k) = Block::STONE;
                } else if (j == height) {
                    float noise_chance
                        = m_Noise.OctaveNoise(i + m_ChunkPosition.x, k + m_ChunkPosition.z, 8);
//                    std::cout << noise_chance << "\n";
                    if (j == water_level && noise_chance >= 0)
                        m_Chunk(i, j, k) = Block::SAND;
                    else if (j < water_level) {
                        if (noise_chance >= 0.2)
                            m_Chunk(i, j, k) = Block::GRAVEL;
                        else
                            m_Chunk(i, j, k) = Block::SAND;
                    } else if (j >= snow_level)
                        m_Chunk(i, j, k) = Block::SNOW;
                    else if (j >= snow_level - 10 && noise_chance >= -0.2)
                        m_Chunk(i, j, k) = Block::SNOWY_GRASS;
                    else
                        m_Chunk(i, j, k) = Block::GRASS;
                } else {
                    if (j < water_level)
                        m_Chunk(i, j, k) = Block::WATER;
                    else
                        m_Chunk(i, j, k) = Block::EMPTY;
                }
            }
        }
    }
}

void Chunk::GenSolidCube(
    int i, int j, int k, std::vector<uint32_t>& target, const std::array<uint8_t, 6>& textureCoords)
{
    if (j > 0 && (m_Chunk(i, j - 1, k) == Block::EMPTY || m_Chunk(i, j - 1, k) == Block::WATER)) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[4], textureCoords[5]), // D
            glm::uvec4(0, 0, 1, 1), glm::uvec4(0), glm::uvec4(1, 0, 0, 1));
    }
    if (j < YSIZE - 1
        && (m_Chunk(i, j + 1, k) == Block::EMPTY || m_Chunk(i, j + 1, k) == Block::WATER)) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[0], textureCoords[1]), // U
            glm::uvec4(0, 1, 1, 0), glm::uvec4(1), glm::uvec4(1, 1, 0, 0));
    }
    if (m_Chunk(i, j, k - 1) == Block::EMPTY || m_Chunk(i, j, k - 1) == Block::WATER) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[2], textureCoords[3]), // B
            glm::uvec4(1, 0, 0, 1), glm::uvec4(0, 0, 1, 1), glm::uvec4(0));
    }
    if (m_Chunk(i, j, k + 1) == Block::EMPTY || m_Chunk(i, j, k + 1) == Block::WATER) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[2], textureCoords[3]), // F
            glm::uvec4(0, 1, 1, 0), glm::uvec4(0, 0, 1, 1), glm::uvec4(1));
    }
    if (m_Chunk(i - 1, j, k) == Block::EMPTY || m_Chunk(i - 1, j, k) == Block::WATER) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[2], textureCoords[3]), // L
            glm::uvec4(0), glm::uvec4(0, 0, 1, 1), glm::uvec4(0, 1, 1, 0));
    }
    if (m_Chunk(i + 1, j, k) == Block::EMPTY || m_Chunk(i + 1, j, k) == Block::WATER) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[2], textureCoords[3]), // R
            glm::uvec4(1), glm::uvec4(0, 0, 1, 1), glm::uvec4(1, 0, 0, 1));
    }
}

void Chunk::GenWaterCube(
    int i, int j, int k, std::vector<uint32_t>& target, const std::array<uint8_t, 6>& textureCoords)
{
    if (j > 0 && (m_Chunk(i, j - 1, k) == Block::EMPTY)) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[4], textureCoords[5]), // D
            glm::uvec4(0, 0, 1, 1), glm::uvec4(0), glm::uvec4(1, 0, 0, 1));
    }
    if (j < YSIZE - 1 && (m_Chunk(i, j + 1, k) == Block::EMPTY)) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[0], textureCoords[1]), // U
            glm::uvec4(0, 1, 1, 0), glm::uvec4(1), glm::uvec4(1, 1, 0, 0));
    }
    if (m_Chunk(i, j, k - 1) == Block::EMPTY) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[2], textureCoords[3]), // B
            glm::uvec4(1, 0, 0, 1), glm::uvec4(0, 0, 1, 1), glm::uvec4(0));
    }
    if (m_Chunk(i, j, k + 1) == Block::EMPTY) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[2], textureCoords[3]), // F
            glm::uvec4(0, 1, 1, 0), glm::uvec4(0, 0, 1, 1), glm::uvec4(1));
    }
    if (m_Chunk(i - 1, j, k) == Block::EMPTY) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[2], textureCoords[3]), // L
            glm::uvec4(0), glm::uvec4(0, 0, 1, 1), glm::uvec4(0, 1, 1, 0));
    }
    if (m_Chunk(i + 1, j, k) == Block::EMPTY) {
        CreateQuad(target, glm::vec3(i, j, k), glm::uvec2(textureCoords[2], textureCoords[3]), // R
            glm::uvec4(1), glm::uvec4(0, 0, 1, 1), glm::uvec4(1, 0, 0, 1));
    }
}

void Chunk::GenerateMesh()
{
    if (m_Mesh.empty() && m_TransparentMesh.empty()) {
        if (m_MinHeight < 1)
            m_MinHeight = 1;
        for (int j = m_MinHeight - 1; j <= m_MaxHeight; j++) {
            for (int i = 1; i < XSIZE - 1; i++) {
                for (int k = 1; k < ZSIZE - 1; k++) {
                    if (m_Chunk(i, j, k) != Block::EMPTY) {
                        std::array<uint8_t, 6> textureCoords = s_TextureMap.at(m_Chunk(i, j, k));
                        if (m_Chunk(i, j, k) != Block::WATER) {
                            GenSolidCube(i, j, k, m_Mesh, textureCoords);
                        } else {
                            GenWaterCube(i, j, k, m_TransparentMesh, textureCoords);
                        }
                    }
                }
            }
        }
        // solid mesh
        size_t indexCount = m_Mesh.size() / 4 * 6; // num faces * 6
        m_IBOCount = indexCount;
        uint32_t solidSize = m_Mesh.size() * sizeof(uint32_t);
        m_VBO.SendData(solidSize, m_Mesh.data(), 0);

        if (!m_TransparentMesh.empty()) {
            indexCount = m_TransparentMesh.size() / 4 * 6;
            m_TIBOCount = indexCount;
            m_VBO.SendData(
                m_TransparentMesh.size() * sizeof(uint32_t), m_TransparentMesh.data(), solidSize);
        }
    }
}

void Chunk::Render(Renderer& renderer, const VertexArray& vao, IndexBuffer& ibo)
{
    ibo.SetCount(m_IBOCount);
    m_VBO.Bind(vao.GetId());
    renderer.Draw(vao, ibo, GL_UNSIGNED_INT, m_ChunkPosition, 0);
    if (!m_TransparentMesh.empty()) {
        ibo.SetCount(m_TIBOCount);
        renderer.Draw(vao, ibo, GL_UNSIGNED_INT, m_ChunkPosition, m_Mesh.size());
    }
}

void Chunk::RenderOutline(Renderer& renderer, const VertexArray& vao, VertexBuffer& vbo,
    IndexBuffer& ibo, const glm::vec3& target)
{
    int i = target.x;
    int j = target.y;
    int k = target.z;
    if (m_Chunk(i, j, k) != Block::EMPTY && m_Chunk(i, j, k) != Block::WATER) {
        std::vector<uint32_t> outlineMesh;
        std::array<uint8_t, 6> textureCoords = s_TextureMap.at(m_Chunk(i, j, k));
        GenSolidCube(i, j, k, outlineMesh, textureCoords);
        size_t indexCount = outlineMesh.size() / 4 * 6;
        vbo.SendData(outlineMesh.size() * sizeof(uint32_t), outlineMesh.data(), 0);
        ibo.SetCount(indexCount);
        renderer.RenderOutline(vao, ibo, GL_UNSIGNED_INT, m_ChunkPosition, i, j, k);
    }
}

Block Chunk::GetBlock(uint32_t x, uint32_t y, uint32_t z) const { return m_Chunk(x, y, z); }

void Chunk::SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block)
{
    m_Chunk(x, y, z) = block;
    if (y < m_MinHeight)
        m_MinHeight = y;
    else if (y > m_MaxHeight)
        m_MaxHeight = y;
    m_Mesh.clear();
    m_TransparentMesh.clear();
}

glm::vec3 Chunk::GetPosition() const { return m_ChunkPosition; }

glm::vec3 Chunk::GetCenterPosition() const
{
    return glm::vec3(m_ChunkPosition.x + XSIZE / 2, 0, m_ChunkPosition.z + ZSIZE / 2);
}

const std::unordered_map<Block, std::array<uint8_t, 6>> Chunk::s_TextureMap = { // top, side, bottom
    { Block::GRASS, { 12, 3, 3, 15, 2, 15 } }, { Block::DIRT, { 2, 15, 2, 15, 2, 15 } },
    { Block::STONE, { 1, 15, 1, 15, 1, 15 } }, { Block::DIAMOND, { 2, 12, 2, 12, 2, 12 } },
    { Block::GOLD, { 0, 13, 0, 13, 0, 13 } }, { Block::COAL, { 2, 13, 2, 13, 2, 13 } },
    { Block::IRON, { 1, 13, 1, 13, 1, 13 } }, { Block::LEAVES, { 11, 1, 11, 1, 11, 1 } },
    { Block::WOOD, { 5, 14, 4, 14, 5, 14 } }, { Block::SNOW, { 2, 11, 2, 11, 2, 11 } },
    { Block::SNOWY_GRASS, { 2, 11, 4, 11, 2, 15 } }, { Block::WATER, { 15, 3, 15, 3, 15, 3 } },
    { Block::SAND, { 2, 14, 2, 14, 2, 14 } }, { Block::GRAVEL, { 3, 14, 3, 14, 3, 14 } },
    { Block::BEDROCK, { 1, 14, 1, 14, 1, 14 } }
};
