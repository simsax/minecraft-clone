#include "Chunk.h"
#include <cstdlib>
#include <stdexcept>
#include "glm/gtx/norm.hpp"
#include <iostream>
#include <map>
#include "../camera/Camera.h"

bool operator==(const ChunkCoord &l, const ChunkCoord &r) { return l.x == r.x && l.z == r.z; }

bool operator!=(const ChunkCoord &l, const ChunkCoord &r) { return l.x != r.x || l.z != r.z; }

int operator-(const ChunkCoord &l, const ChunkCoord &r) {
    return static_cast<int>(round(sqrt(pow(l.x - r.x, 2) + pow(l.z - r.z, 2))));
}

std::size_t hash_fn::operator()(const ChunkCoord &coord) const {
    std::size_t h1 = std::hash<int>()(coord.x);
    std::size_t h2 = std::hash<int>()(coord.z);
    return h1 ^ h2;
}

static std::array<uint32_t, 4> GenerateTextCoords(const glm::uvec2 &textureCoords) {
    uint32_t t0 = (textureCoords[0] << 6) | (textureCoords[1] << 2);
    uint32_t t1 = t0 | 1;
    uint32_t t2 = t0 | 2;
    uint32_t t3 = t0 | 3;
    return {t0, t1, t2, t3};
}

static void CreateQuad(std::vector<uint32_t> &target, const glm::uvec3 &position,
                       const glm::uvec2 &textureCoords, const glm::uvec4 &offsetx,
                       const glm::uvec4 &offsety,
                       const glm::uvec4 &offsetz) {
    constexpr int vertices = 4;
    std::array<uint32_t, vertices> t = GenerateTextCoords({textureCoords[0], textureCoords[1]});

    for (int i = 0; i < vertices; i++) {
        uint32_t v = (position[0] + offsetx[i]) << 24 | (position[1] + offsety[i]) << 15
                     | (position[2] + offsetz[i]) << 10 | t[i];
        target.emplace_back(v);
    }
}

Chunk::Chunk(const ChunkCoord &coords, const glm::vec3 &position, uint32_t maxVertexCount,
             const std::vector<uint32_t> &indices, const VertexBufferLayout &layout,
             int bindingIndex, ChunkMap* chunkMap)
        : m_HeightMap({}), m_Coords(coords), m_ChunkPosition(position),
          m_Chunk(Matrix3D<Block, XSIZE, YSIZE, ZSIZE>()), m_MaxVertexCount(maxVertexCount),
          m_MinHeight(YSIZE), m_MaxHeight(0), m_IBOCount(0), m_TIBOCount(0), m_ChunkMap(chunkMap),
          m_CenterPosition({m_ChunkPosition.x + static_cast<float>(XSIZE) / 2, 0,
                            m_ChunkPosition.z + static_cast<float>(ZSIZE) / 2}) {
    m_VBO.Init(layout.GetStride(), bindingIndex);
    m_VBO.CreateDynamic(sizeof(uint32_t) * maxVertexCount);

    m_Mesh.reserve(m_MaxVertexCount);
    m_TransparentMesh.reserve(maxVertexCount / 4);
    CreateHeightMap();
    FastFill();
}

float Chunk::Continentalness(int x, int y) {
    constexpr float scale = 256.0f;
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

void Chunk::CreateHeightMap() {
    constexpr int elevation = 50; // increase for less water
    int index = 0;

    for (int i = 0; i < XSIZE; i++) {
        for (int k = 0; k < ZSIZE; k++) {
            float a
                    =
                    (m_Noise.OctaveNoise(i + m_ChunkPosition.x, k + m_ChunkPosition.z, 6, 0.002f) +
                     1)
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

void Chunk::FastFill() {
    constexpr int height = 63;
    constexpr int level_size = XSIZE * ZSIZE;
    auto begin = m_Chunk.GetRawPtr();
    std::fill(begin, begin + level_size, Block::BEDROCK);
    std::fill(begin + level_size, begin + level_size * m_MinHeight, Block::STONE);
    std::fill(begin + level_size * (m_MaxHeight + 1), begin + level_size * YSIZE, Block::EMPTY);
}

BlockVec Chunk::CreateSurfaceLayer(const BlockVec &blocksToSet) {
    BlockVec blockVec = {};
    constexpr int water_level = 63;
    constexpr int snow_level = 120;
    if (m_MaxHeight < water_level)
        m_MaxHeight = water_level;
    for (int j = m_MinHeight; j <= m_MaxHeight; j++) {
        int index = 0;
        for (int i = 0; i < XSIZE; i++) {
            for (int k = 0; k < ZSIZE; k++) {
                int height = m_HeightMap[index++];
                if (m_Chunk(i, j, k) != Block::WOOD && m_Chunk(i, j, k) != Block::LEAVES) {
                    if (j < height) {
                        int dirtThickness
                                = static_cast<int>((m_Noise.OctaveNoise(i + m_ChunkPosition.x + 111,
                                                                        k + m_ChunkPosition.z + 111,
                                                                        8)
                                                    + 1)
                                                   / 2 * 10)
                                  - j;
                        if (dirtThickness >= 0)
                            m_Chunk(i, j, k) = Block::DIRT;
                        else
                            m_Chunk(i, j, k) = Block::STONE;
                    } else if (j == height) {
                        float noise_chance
                                = m_Noise.OctaveNoise(i + m_ChunkPosition.x, k + m_ChunkPosition.z,
                                                      8);
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
//                    if (j < snow_level)
//                        CreateTrees(i, j, k, blockVec);
                }
            }
        }
    }
    SetBlocks(blocksToSet);
    return blockVec;
}

void Chunk::CreateTrees(int i, int j, int k, BlockVec &blockVec) {
    float noise_chance
            = m_Noise.OctaveNoise(i + m_ChunkPosition.x + 22.2f, k + m_ChunkPosition.z + 22.2f,
                                  4, 0.008f);
    const int treeHeight = 5 + j;
    if (j > 0 && noise_chance >= 0.1 &&
        i != 0 && k != 0 && i != XSIZE - 1 && k != ZSIZE - 1 &&
        m_Chunk(i, j - 1, k) == Block::GRASS) {
        if ((float) std::rand() / (float) RAND_MAX < 0.005f) {
            int height;
            for (height = j; height < treeHeight; height++) {
                if (height > YSIZE)
                    break;
                m_Chunk(i, height, k) = Block::WOOD;
            }
            int leaves_height;
            const int top = height + 1;
            for (leaves_height = height - 2; leaves_height <= top; leaves_height++) {
                int size = 2;
//                if (leaves_height == top)
//                    size = 1;
                for (int x = -size; x <= size; x++) {
                    for (int z = -size; z <= size; z++) {
                        const int leafx = i + x;
                        const int leafz = k + z;
                        if (leafx >= 0 && leafz >= 0 && leafx < XSIZE && leafz < ZSIZE
                            && m_Chunk(leafx, leaves_height, leafz) != Block::WOOD)
                            m_Chunk(leafx, leaves_height, leafz) = Block::LEAVES;
                        // farlo anche per il tronco
                        if (leafx <= 1 || leafz <= 1 || leafx >= XSIZE - 2 || leafz >= ZSIZE - 2) {
                            blockVec.emplace_back(Block::LEAVES,
                                                  glm::vec3(leafx, leaves_height, leafz)
                            );
                        }
                    }
                }
            }
            if (leaves_height > m_MaxHeight)
                m_MaxHeight = leaves_height;
        }
    }
}

template<typename... Args>
bool
Chunk::CheckNeighbor(const Chunk *const chunk, const glm::uvec3 &position, Args... voidBlocks) {
    if (((chunk->m_Chunk(position.x, position.y, position.z) == voidBlocks) || ...))
        return true;
    else
        return false;
}

template<typename... Args>
void Chunk::GenCube(int i, int j, int k, std::vector<uint32_t> &target,
                    const std::array<uint8_t, 6> &textureCoords,
                    const std::array<Chunk *, 4> &neighbors,
                    Args... voidBlocks) {
    constexpr int west = 0;
    constexpr int north = 1;
    constexpr int east = 2;
    constexpr int south = 3;

    if (j == 0 || (j > 0 && ((m_Chunk(i, j - 1, k) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[4], textureCoords[5]}, // D
                   {0, 0, 1, 1}, glm::uvec4(0), {1, 0, 0, 1});
    }
    if (j == YSIZE - 1 || (j < YSIZE - 1 && ((m_Chunk(i, j + 1, k) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[0], textureCoords[1]}, // U
                   {0, 1, 1, 0}, glm::uvec4(1), {1, 1, 0, 0});
    }
    if ((k == 0 && CheckNeighbor(neighbors[north], {i, j, ZSIZE - 1}, voidBlocks...))
        || (k > 0 && ((m_Chunk(i, j, k - 1) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[2], textureCoords[3]}, // B
                   {1, 0, 0, 1}, {0, 0, 1, 1}, glm::uvec4(0));
    }
    if ((k == ZSIZE - 1 && CheckNeighbor(neighbors[south], {i, j, 0}, voidBlocks...))
        || (k < ZSIZE - 1 && ((m_Chunk(i, j, k + 1) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[2], textureCoords[3]}, // F
                   {0, 1, 1, 0}, {0, 0, 1, 1}, glm::uvec4(1));
    }
    if ((i == 0 && CheckNeighbor(neighbors[west], {XSIZE - 1, j, k}, voidBlocks...))
        || (i > 0 && ((m_Chunk(i - 1, j, k) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[2], textureCoords[3]}, // L
                   glm::uvec4(0), {0, 0, 1, 1}, {0, 1, 1, 0});
    }
    if ((i == XSIZE - 1 && CheckNeighbor(neighbors[east], {0, j, k}, voidBlocks...))
        || (i < XSIZE - 1 && ((m_Chunk(i + 1, j, k) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[2], textureCoords[3]}, // R
                   glm::uvec4(1), {0, 0, 1, 1}, {1, 0, 0, 1});
    }
}

bool Chunk::FindNeighbors(std::array<Chunk *, 4> &neighbors) {
    int n = 0;
    for (int i = -1; i <= 1; i += 2) {
        if (const auto it = m_ChunkMap->find({m_Coords.x + i, m_Coords.z});
                it != m_ChunkMap->end()) {
            neighbors[n++] = &it->second;
        } else { // border chunk, don't generate the mesh
            return false;
        }

        if (const auto it = m_ChunkMap->find({m_Coords.x, m_Coords.z + i});
                it != m_ChunkMap->end()) {
            neighbors[n++] = &it->second;
        } else { // border chunk, don't generate the mesh
            return false;
        }
    }
    return true;
}

bool Chunk::GenerateMesh() {
    if (m_Mesh.empty() && m_TransparentMesh.empty()) {
        // retrieve the 4 pointers from the map and continue only if all of them exist
        std::array<Chunk *, 4> neighbors = {nullptr}; // west, north, east, south
        if (FindNeighbors(neighbors)) {
            if (m_MinHeight < 1)
                m_MinHeight = 1;
            for (int j = m_MinHeight - 1; j <= m_MaxHeight; j++) {
                for (int i = 0; i < XSIZE; i++) {
                    for (int k = 0; k < ZSIZE; k++) {
                        if (m_Chunk(i, j, k) != Block::EMPTY) {
                            std::array<uint8_t, 6> textureCoords = s_TextureMap.at(
                                    m_Chunk(i, j, k));
                            if (m_Chunk(i, j, k) != Block::WATER &&
                                m_Chunk(i, j, k) != Block::LEAVES) {
                                GenCube(i, j, k, m_Mesh, textureCoords, neighbors,
                                        Block::EMPTY, Block::LEAVES, Block::WATER);
                            } else {
                                GenCube(i, j, k, m_TransparentMesh, textureCoords, neighbors,
                                        Block::EMPTY, Block::LEAVES);
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
                        m_TransparentMesh.size() * sizeof(uint32_t), m_TransparentMesh.data(),
                        solidSize);
            }
            return true;
        } else
            return false;
    }
    return true;
}

void Chunk::Render(Renderer &renderer, const VertexArray &vao, IndexBuffer &ibo) {
    ibo.SetCount(m_IBOCount);
    m_VBO.Bind(vao.GetId());
    renderer.Draw(vao, ibo, GL_UNSIGNED_INT, m_ChunkPosition, 0);
    if (!m_TransparentMesh.empty()) {
        ibo.SetCount(m_TIBOCount);
        renderer.Draw(vao, ibo, GL_UNSIGNED_INT, m_ChunkPosition, m_Mesh.size());
    }
}

void Chunk::RenderOutline(Renderer &renderer, const VertexArray &vao, VertexBuffer &vbo,
                          IndexBuffer &ibo, const glm::vec3 &target) {
    std::array<Chunk *, 4> neighbors = {nullptr};
    if (FindNeighbors(neighbors)) {
        int i = target.x;
        int j = target.y;
        int k = target.z;
        if (m_Chunk(i, j, k) != Block::EMPTY && m_Chunk(i, j, k) != Block::WATER
            && m_Chunk(i, j, k) != Block::LEAVES) {
            std::vector<uint32_t> outlineMesh;
            std::array<uint8_t, 6> textureCoords = s_TextureMap.at(m_Chunk(i, j, k));
            GenCube(i, j, k, outlineMesh, textureCoords, neighbors, Block::EMPTY, Block::LEAVES,
                    Block::WATER);
            size_t indexCount = outlineMesh.size() / 4 * 6;
            vbo.SendData(outlineMesh.size() * sizeof(uint32_t), outlineMesh.data(), 0);
            ibo.SetCount(indexCount);
            renderer.RenderOutline(vao, ibo, GL_UNSIGNED_INT, m_ChunkPosition, i, j, k);
        }
    }
}

Block Chunk::GetBlock(uint32_t x, uint32_t y, uint32_t z) const { return m_Chunk(x, y, z); }

void Chunk::SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block) {
    if (x >= XSIZE || y >= YSIZE || z >= ZSIZE)
        throw std::logic_error("Chunk coordinates out of bound.");
    m_Chunk(x, y, z) = block;
    if (y < m_MinHeight)
        m_MinHeight = y;
    else if (y > m_MaxHeight)
        m_MaxHeight = y;
    m_Mesh.clear();
    m_TransparentMesh.clear();
}

glm::vec3 Chunk::GetCenterPosition() const {
    return m_CenterPosition;
}

void Chunk::SetBlocks(const BlockVec &blocksToSet) {
    for (const auto &[block, vec]: blocksToSet) {
        glm::uvec3 pos = vec;
        m_Chunk(pos.x, pos.y, pos.z) = block;
    }
}

const std::unordered_map<Block, std::array<uint8_t, 6>> Chunk::s_TextureMap = { // top, side, bottom
        {Block::GRASS,       {12, 3,  3,  15, 2,  15}},
        {Block::DIRT,        {2,  15, 2,  15, 2,  15}},
        {Block::STONE,       {1,  15, 1,  15, 1,  15}},
        {Block::DIAMOND,     {2,  12, 2,  12, 2,  12}},
        {Block::GOLD,        {0,  13, 0,  13, 0,  13}},
        {Block::COAL,        {2,  13, 2,  13, 2,  13}},
        {Block::IRON,        {1,  13, 1,  13, 1,  13}},
        {Block::LEAVES,      {11, 1,  11, 1,  11, 1}},
        {Block::WOOD,        {5,  14, 4,  14, 5,  14}},
        {Block::SNOW,        {2,  11, 2,  11, 2,  11}},
        {Block::SNOWY_GRASS, {2,  11, 4,  11, 2,  15}},
        {Block::WATER,       {15, 3,  15, 3,  15, 3}},
        {Block::SAND,        {2,  14, 2,  14, 2,  14}},
        {Block::GRAVEL,      {3,  14, 3,  14, 3,  14}},
        {Block::BEDROCK,     {1,  14, 1,  14, 1,  14}}
};

ChunkCoord Chunk::GetCoord() const {
    return m_Coords;
}

bool Chunk::NotVisible(ChunkCoord playerChunk, int radius) const {
    if (std::abs(playerChunk.x - m_Coords.x) > radius ||
        std::abs(playerChunk.z - m_Coords.z) > radius)
        return true;
    else
        return false;
}
