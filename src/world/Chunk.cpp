#include "Chunk.h"
#include "glm/gtx/norm.hpp"
#include <map>
#include "../camera/Camera.h"
#include "../utils/Random.hpp"
#include "../utils/Logger.h"

#define assertm(exp, msg) assert(((void)msg, exp))

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
                       const glm::uvec4 &offsetz,
                       uint8_t normalIndex) {
    static constexpr int vertices = 4;
    std::array<uint32_t, vertices> t = GenerateTextCoords({textureCoords[0], textureCoords[1]});

    for (int i = 0; i < vertices; i++) {
        uint32_t v = normalIndex << 29 | (position[0] + offsetx[i]) << 24 |
                     (position[1] + offsety[i]) << 15
                     | (position[2] + offsetz[i]) << 10 | t[i];
        target.emplace_back(v);
    }
}

Chunk::Chunk(const ChunkCoord &coords, const glm::vec3 &position, uint32_t maxVertexCount,
             const std::vector<uint32_t> &indices, const VertexBufferLayout &layout,
             int bindingIndex,
             ChunkMap *chunkMap)
        : m_HeightMap({}), m_Coords(coords), m_ChunkPosition(position),
          m_Chunk(Matrix3D<Block, XSIZE, YSIZE, ZSIZE>()), m_MaxVertexCount(maxVertexCount),
          m_MinHeight(YSIZE), m_MaxHeight(0), m_IBOCount(0), m_TIBOCount(0), m_SIBOCount(0),
          m_ChunkMap(chunkMap),
          m_CenterPosition({m_ChunkPosition.x + static_cast<float>(XSIZE) / 2, 0,
                            m_ChunkPosition.z + static_cast<float>(ZSIZE) / 2}) {
    m_VBO.Init(layout.GetStride(), bindingIndex);
    m_VBO.CreateDynamic(sizeof(uint32_t) * maxVertexCount);

    m_Mesh.reserve(m_MaxVertexCount);
    m_TransparentMesh.reserve(maxVertexCount / 4);
    m_SpriteMesh.reserve(2048); // 8 vertices * 16x16
    CreateHeightMap();
    FastFill();
}

float Chunk::Continentalness(int x, int y) {
    static constexpr float scale = 256.0f;
    float height = 0;

    float noise = m_Noise.OctaveNoiseSingle(y, x, 8, 0.01f);

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

#if 1

void Chunk::CreateHeightMap() {
    int index = 0;
    static constexpr float fudge_factor = 1.0f;
    static constexpr float exp = 3.0f;

    std::array<float, ZSIZE * XSIZE> noiseMapA = m_Noise.OctaveNoiseGrid<ZSIZE, XSIZE>(
            m_ChunkPosition.z, m_ChunkPosition.x, 6, 0.001f);

    int n = 0;
    for (int i = 0; i < XSIZE; i++) {
        for (int k = 0; k < ZSIZE; k++) {
            float a = (noiseMapA[n++] + 1) * 0.5f; // [0, 1]
//            a = std::pow(a * fudge_factor, exp);
            if (a >= 0.5)
                a = 6.0f * std::pow(a - 0.5f, exp) + 0.25f;
            else
                a *= 0.5f;
//            else
//                a = 2.0f * std::pow(a - 0.5f, exp) + 0.25f;
            int terrain_height = static_cast<int>(a * YSIZE); // [0, 255]

            m_MinHeight = std::min(m_MinHeight, terrain_height);
            m_MaxHeight = std::max(m_MaxHeight, terrain_height);
            m_HeightMap[index++] = terrain_height;
        }
    }
    m_MaxHeight += 1; // allow sprite generation
}

#else
void Chunk::CreateHeightMap() {
    static constexpr int elevation = 50; // increase for less water
    int index = 0;

    std::array<float, ZSIZE * XSIZE> noiseMapA = m_Noise.OctaveNoiseGrid<ZSIZE, XSIZE>(
            m_ChunkPosition.z, m_ChunkPosition.x, 6, 0.002f);
    std::array<float, XSIZE * ZSIZE> noiseMapM = m_Noise.OctaveNoiseGrid<XSIZE, ZSIZE>(
            m_ChunkPosition.z + 12.3f, m_ChunkPosition.x + 12.3f, 8, 0.01f);

    int n = 0;
    for (int i = 0; i < XSIZE; i++) {
        for (int k = 0; k < ZSIZE; k++) {
            float a = (noiseMapA[n] + 1) * 0.5f;
            float m = (noiseMapM[n] + 1) * 0.5f;
            n++;
            a *= a;
            if (a < 0.5)
                a = m * a * a * 2;
            else
                a = m * (1 - (1 - a) * (1 - a) * 2);
            int terrain_height = static_cast<int>(a * 200.0f) + elevation;
            int height = static_cast<int>(
                    m_Noise.OctaveNoiseSingle(k + m_ChunkPosition.z, i + m_ChunkPosition.x, 8) * 5.0f);

            int final_height = height + terrain_height;
            if (final_height > YSIZE)
                final_height = YSIZE;
            m_MinHeight = std::min(m_MinHeight, final_height);
            m_MaxHeight = std::max(m_MaxHeight, final_height);
            m_HeightMap[index++] = final_height;
        }
    }
    m_MaxHeight += 1; // allow sprite generation
}
#endif

void Chunk::FastFill() {
    static constexpr int level_size = XSIZE * ZSIZE;
    auto begin = m_Chunk.GetRawPtr();
    std::fill(begin, begin + level_size, Block::BEDROCK);
    if (m_MinHeight > 1)
        std::fill(begin + level_size, begin + level_size * (m_MinHeight), Block::STONE);
    if (m_MaxHeight < YSIZE - 1)
        std::fill(begin + level_size * (m_MaxHeight + 1), begin + level_size * YSIZE, Block::EMPTY);
}

BlockVec Chunk::CreateSurfaceLayer(const BlockVec &blocksToSet) {
    BlockVec blockVec = {};
    static constexpr int water_level = 63;
    static constexpr int snow_level = 120;
    if (m_MaxHeight < water_level)
        m_MaxHeight = water_level;
    int j = m_MinHeight < 5 ? 0 : m_MinHeight - 5;
    for (; j <= m_MaxHeight; j++) {
        int index = 0;
        for (int i = 0; i < XSIZE; i++) {
            for (int k = 0; k < ZSIZE; k++) {
                int height = m_HeightMap[index++];
                if (m_Chunk(i, j, k) != Block::WOOD && m_Chunk(i, j, k) != Block::LEAVES) {
                    if (j < height) {
                        float dirt = m_Noise.OctaveNoiseSingle(k + m_ChunkPosition.z + 1000.1f,
                                                               i + m_ChunkPosition.x, 4, 0.001f);

                        if (dirt >= 0)
                            m_Chunk(i, j, k) = Block::DIRT;
                        else
                            m_Chunk(i, j, k) = Block::STONE;
                    } else if (j == height) {
                        float noise_chance
                                = m_Noise.OctaveNoiseSingle(k + m_ChunkPosition.z,
                                                            i + m_ChunkPosition.x + 100.0f,
                                                            8, 0.002f);
                        if (j == water_level) {
                            if (noise_chance >= 0.3)
                                m_Chunk(i, j, k) = Block::GRAVEL;
                            else if (noise_chance < -0.3)
                                m_Chunk(i, j, k) = Block::GRASS;
                            else
                                m_Chunk(i, j, k) = Block::SAND;
                        } else if (j < water_level) {
                            if (noise_chance >= 0.2f && noise_chance < 0.4f)
                                m_Chunk(i, j, k) = Block::GRAVEL;
                            else if (noise_chance >= 0.4f)
                                m_Chunk(i, j, k) = Block::DIRT;
                            else
                                m_Chunk(i, j, k) = Block::SAND;
                        } else if (j >= snow_level)
                            m_Chunk(i, j, k) = Block::SNOW;
                        else if (j >= snow_level - 20) {
                            if (Random::GetRand<double>(0, 1) <
                                (3.0f / static_cast<float>(snow_level - j)))
                                m_Chunk(i, j, k) = Block::SNOWY_GRASS;
                            else
                                m_Chunk(i, j, k) = Block::GRASS;
                        } else
                            m_Chunk(i, j, k) = Block::GRASS;
                    } else { // j > height
                        if (j <= water_level)
                            m_Chunk(i, j, k) = Block::WATER;
                        else
                            m_Chunk(i, j, k) = Block::EMPTY;
                    }
                    if (j < snow_level) {
                        CreateTrees(i, j, k, blockVec);
                        CreateSprites(i, j, k, blockVec);
                    }
                }
            }
        }
    }
    SetBlocks(blocksToSet);
    return blockVec;
}

// move this into a terrain generator class
void Chunk::CreateTrees(int i, int j, int k, BlockVec &blockVec) {
    int treeHeight = Random::GetRand<int>(3, 9) + j;
    if (j > 0 && m_Chunk(i, j, k) == Block::EMPTY && m_Chunk(i, j - 1, k) == Block::GRASS) {
        if (Random::GetRand<double>(0, 1) < 0.005) {
            float noise_chance = m_Noise.OctaveNoiseSingle(
                    k + m_ChunkPosition.z, i + m_ChunkPosition.x + 1000.0f, 4, 0.008f);
            if (noise_chance < 0) {
                int height;
                for (height = j; height < treeHeight; height++) {
                    if (height > YSIZE)
                        break;
                    m_Chunk(i, height, k) = Block::WOOD;
                }
                const int top = height + Random::GetRand<int>(1, 3);
                const int bottom = height - Random::GetRand<int>(2, 3);
                int leaves_height;
                for (leaves_height = bottom; leaves_height <= top; leaves_height++) {
                    int size = Random::GetRand<int>(2, 4);
                    if (leaves_height == top || leaves_height == bottom)
                        size = 1;
                    for (int x = -size; x <= size; x++) {
                        for (int z = -size; z <= size; z++) {
                            const int leafx = i + x;
                            const int leafz = k + z;
                            if (leafx >= 0 && leafz >= 0 && leafx < XSIZE && leafz < ZSIZE) {
                                if (m_Chunk(leafx, leaves_height, leafz) != Block::WOOD
                                    && Random::GetRand<double>(0, 1) > 0.2)
                                    m_Chunk(leafx, leaves_height, leafz) = Block::LEAVES;
                            } else {
                                blockVec.emplace_back(
                                        Block::LEAVES, glm::vec3(leafx, leaves_height, leafz));
                            }
                        }
                    }
                }
                if (leaves_height > m_MaxHeight)
                    m_MaxHeight = leaves_height;
            }
        }
    }
}

void Chunk::CreateSprites(int i, int j, int k, BlockVec &blockVec) {
    if (j > 0 && m_Chunk(i, j, k) == Block::EMPTY && m_Chunk(i, j - 1, k) == Block::GRASS) {
        if (Random::GetRand<double>(0, 1) < 0.005) {
            const float noise_chance = m_Noise.OctaveNoiseSingle(
                    k + m_ChunkPosition.z + 1000.1f, i + m_ChunkPosition.x + 1000.1f, 4, 0.02f);
            if (noise_chance < 0) {
                if (Random::GetRand<double>(0, 1) < 0.6)
                    m_Chunk(i, j, k) = Block::FLOWER_BLUE;
                else
                    m_Chunk(i, j, k) = Block::FLOWER_YELLOW;
            }
            return;
        } else if (Random::GetRand<double>(0, 1) < 0.2)
            m_Chunk(i, j, k) = Block::BUSH;
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
    static constexpr int west = 0;
    static constexpr int north = 1;
    static constexpr int east = 2;
    static constexpr int south = 3;

    if (j == 0 || (j > 0 && ((m_Chunk(i, j - 1, k) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[4], textureCoords[5]}, // D
                   {0, 0, 1, 1}, glm::uvec4(0), {1, 0, 0, 1}, 3);
    }
    if (j == YSIZE - 1 || (j < YSIZE - 1 && ((m_Chunk(i, j + 1, k) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[0], textureCoords[1]}, // U
                   {0, 1, 1, 0}, glm::uvec4(1), {1, 1, 0, 0}, 2);
    }
    if ((k == 0 && CheckNeighbor(neighbors[north], {i, j, ZSIZE - 1}, voidBlocks...))
        || (k > 0 && ((m_Chunk(i, j, k - 1) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[2], textureCoords[3]}, // B
                   {1, 0, 0, 1}, {0, 0, 1, 1}, glm::uvec4(0), 5);
    }
    if ((k == ZSIZE - 1 && CheckNeighbor(neighbors[south], {i, j, 0}, voidBlocks...))
        || (k < ZSIZE - 1 && ((m_Chunk(i, j, k + 1) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[2], textureCoords[3]}, // F
                   {0, 1, 1, 0}, {0, 0, 1, 1}, glm::uvec4(1), 4);
    }
    if ((i == 0 && CheckNeighbor(neighbors[west], {XSIZE - 1, j, k}, voidBlocks...))
        || (i > 0 && ((m_Chunk(i - 1, j, k) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[2], textureCoords[3]}, // L
                   glm::uvec4(0), {0, 0, 1, 1}, {0, 1, 1, 0}, 1);
    }
    if ((i == XSIZE - 1 && CheckNeighbor(neighbors[east], {0, j, k}, voidBlocks...))
        || (i < XSIZE - 1 && ((m_Chunk(i + 1, j, k) == voidBlocks) || ...))) {
        CreateQuad(target, {i, j, k}, {textureCoords[2], textureCoords[3]}, // R
                   glm::uvec4(1), {0, 0, 1, 1}, {1, 0, 0, 1}, 0);
    }
}

static void GenSprite(
        int i, int j, int k, std::vector<uint32_t> &target,
        const std::array<uint8_t, 6> &textureCoords) {
    static constexpr int vertices = 4;
    static constexpr std::array<uint8_t, 8> offsetx = {0, 1, 1, 0, 0, 1, 1, 0};
    static constexpr std::array<uint8_t, 8> offsety = {0, 0, 1, 1, 0, 0, 1, 1};
    static constexpr std::array<uint8_t, 8> offsetz = {0, 1, 1, 0, 1, 0, 0, 1};
    std::array<uint32_t, vertices> t = GenerateTextCoords({textureCoords[0], textureCoords[1]});

    // maybe a constant color that changes depending on the angle would be better

    for (int n = 0; n < 8; n++) {
        uint32_t v
                = (n < 4 ? 7 << 29 : 6 << 29) | (i + offsetx[n]) << 24 | (j + offsety[n]) << 15 |
                  (k + offsetz[n]) << 10 |
                  t[n % vertices];
        target.emplace_back(v);
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
    if (m_Mesh.empty() && m_TransparentMesh.empty() && m_SpriteMesh.empty()) {
        // retrieve the 4 pointers from the map and continue only if all of them exist
        std::array<Chunk *, 4> neighbors = {nullptr}; // west, north, east, south
        if (FindNeighbors(neighbors)) {
            if (m_MinHeight < 1)
                m_MinHeight = 1;
            for (int j = m_MinHeight - 1; j <= m_MaxHeight; j++) {
                for (int i = 0; i < XSIZE; i++) {
                    for (int k = 0; k < ZSIZE; k++) {
                        std::array<uint8_t, 6> textureCoords
                                = s_TextureMap.at(m_Chunk(i, j, k));
                        switch (m_Chunk(i, j, k)) {
                            case Block::EMPTY:
                                break;
                            case Block::FLOWER_YELLOW:
                            case Block::FLOWER_BLUE:
                            case Block::BUSH:
                                GenSprite(i, j, k, m_SpriteMesh, textureCoords);
                                break;
                            case Block::WATER:
                            case Block::LEAVES:
                                GenCube(i, j, k, m_TransparentMesh, textureCoords, neighbors,
                                        Block::EMPTY, Block::FLOWER_BLUE, Block::FLOWER_YELLOW,
                                        Block::BUSH);
                                break;
                            default:
                                GenCube(i, j, k, m_Mesh, textureCoords, neighbors, Block::EMPTY,
                                        Block::LEAVES, Block::WATER, Block::FLOWER_BLUE,
                                        Block::FLOWER_YELLOW, Block::BUSH);
                                break;
                        }
                    }
                }
            }
            // solid mesh
            size_t indexCount = m_Mesh.size() / 4 * 6; // num faces * 6
            m_IBOCount = indexCount;
            uint32_t solidSize = m_Mesh.size() * sizeof(uint32_t);
            uint32_t transSize = 0;
            m_VBO.SendData(solidSize, m_Mesh.data(), 0);

            if (!m_TransparentMesh.empty()) {
                indexCount = m_TransparentMesh.size() / 4 * 6;
                m_TIBOCount = indexCount;
                transSize = m_TransparentMesh.size() * sizeof(uint32_t);
                m_VBO.SendData(transSize,
                               m_TransparentMesh.data(), solidSize);
            }
            if (!m_SpriteMesh.empty()) {
                uint32_t offset = solidSize + transSize;
                indexCount = m_SpriteMesh.size() / 4 * 6;
                m_SIBOCount = indexCount;
                m_VBO.SendData(m_SpriteMesh.size() * sizeof(uint32_t),
                               m_SpriteMesh.data(), offset);
            }
            return true;
        } else
            return false;
    }
    return true;
}

void
Chunk::Render(Renderer &renderer, const VertexArray &vao, IndexBuffer &ibo, Shader &shader,
              const Texture &texture, ChunkCoord playerChunk, int radius, const glm::vec3 &skyColor,
              const glm::vec3 &sunColor, const glm::vec3 &viewPos, const glm::vec3 &sunPos, bool isDay,
              float ambientStrength) {
    ibo.SetCount(m_IBOCount);
    m_VBO.Bind(vao.GetId());
    renderer.RenderChunk(vao, ibo, shader, texture, GL_UNSIGNED_INT, m_ChunkPosition, 0, skyColor,
                         sunColor, viewPos, sunPos, isDay, ambientStrength);
    if (!m_TransparentMesh.empty()) {
        ibo.SetCount(m_TIBOCount);
        glDisable(GL_CULL_FACE);
        renderer.RenderChunk(vao, ibo, shader, texture, GL_UNSIGNED_INT, m_ChunkPosition,
                             m_Mesh.size(), skyColor, sunColor, viewPos, sunPos, isDay, ambientStrength);
        glEnable(GL_CULL_FACE);
    }
    if (!m_SpriteMesh.empty() && ChunkIsVisible(playerChunk, radius)) {
        ibo.SetCount(m_SIBOCount);
        glDisable(GL_CULL_FACE);
        renderer.RenderChunk(vao, ibo, shader, texture, GL_UNSIGNED_INT, m_ChunkPosition,
                             m_Mesh.size() + m_TransparentMesh.size(), skyColor, sunColor, viewPos,
                             sunPos, isDay, ambientStrength);
        glEnable(GL_CULL_FACE);
    }
}

void Chunk::RenderOutline(Renderer &renderer, const VertexArray &vao, VertexBuffer &vbo,
                          IndexBuffer &ibo, Shader &shader, const glm::uvec3 &target) {
    std::array<Chunk *, 4> neighbors = {nullptr};
    if (FindNeighbors(neighbors)) {
        int i = target.x;
        int j = target.y;
        int k = target.z;
        if (m_Chunk(i, j, k) != Block::EMPTY && m_Chunk(i, j, k) != Block::WATER) {
            std::vector<uint32_t> outlineMesh;
            std::array<uint8_t, 6> textureCoords = s_TextureMap.at(m_Chunk(i, j, k));
            GenCube(i, j, k, outlineMesh, textureCoords, neighbors, Block::EMPTY, Block::LEAVES,
                    Block::WATER);
            size_t indexCount = outlineMesh.size() / 4 * 6;
            vbo.SendData(outlineMesh.size() * sizeof(uint32_t), outlineMesh.data(), 0);
            ibo.SetCount(indexCount);
            renderer.RenderOutline(vao, ibo, shader, GL_UNSIGNED_INT, m_ChunkPosition, i, j, k);
        }
    }
}

Block Chunk::GetBlock(uint32_t x, uint32_t y, uint32_t z) const { return m_Chunk(x, y, z); }

void Chunk::SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block) {
    assertm(x < XSIZE && y < YSIZE && z < ZSIZE, "indices out of bounds.");
    m_Chunk(x, y, z) = block;
    if (y < m_MinHeight)
        m_MinHeight = y;
    else if (y > m_MaxHeight)
        m_MaxHeight = y;

    ClearMesh();
}

void Chunk::ClearMesh() {
    m_Mesh.clear();
    m_TransparentMesh.clear();
    m_SpriteMesh.clear();
}

glm::vec3 Chunk::GetCenterPosition() const { return m_CenterPosition; }

void Chunk::SetBlocks(const BlockVec &blocksToSet) {
    for (const auto &[block, vec]: blocksToSet) {
        glm::uvec3 pos = vec;
        SetBlock(pos.x, pos.y, pos.z, block);
    }
}

ChunkCoord Chunk::GetCoord() const { return m_Coords; }

bool Chunk::ChunkIsVisible(ChunkCoord playerChunk, int radius) const {
    if (std::abs(playerChunk.x - m_Coords.x) > radius
        || std::abs(playerChunk.z - m_Coords.z) > radius)
        return false;
    else
        return true;
}

void Chunk::UpdateMeshHeighLimit(uint32_t height) {
    if (height > m_MaxHeight)
        m_MaxHeight = height;
    else if (height < m_MinHeight)
        m_MinHeight = height;
}

const std::unordered_map<Block, std::array<uint8_t, 6>> Chunk::s_TextureMap = { // top, side, bottom
        {Block::EMPTY,         {0,  0,  0,  0,  0,  0}},
        {Block::GRASS,         {12, 3,  3,  15, 2,  15}},
        {Block::DIRT,          {2,  15, 2,  15, 2,  15}},
        {Block::STONE,         {1,  15, 1,  15, 1,  15}},
        {Block::DIAMOND,       {2,  12, 2,  12, 2,  12}},
        {Block::GOLD,          {0,  13, 0,  13, 0,  13}},
        {Block::COAL,          {2,  13, 2,  13, 2,  13}},
        {Block::IRON,          {1,  13, 1,  13, 1,  13}},
        {Block::LEAVES,        {15, 4,  15, 4,  15, 4}},
        {Block::WOOD,          {5,  14, 4,  14, 5,  14}},
        {Block::SNOW,          {2,  11, 2,  11, 2,  11}},
        {Block::SNOWY_GRASS,   {2,  11, 4,  11, 2,  15}},
        {Block::WATER,         {15, 3,  15, 3,  15, 3}},
        {Block::SAND,          {2,  14, 2,  14, 2,  14}},
        {Block::GRAVEL,        {3,  14, 3,  14, 3,  14}},
        {Block::BEDROCK,       {1,  14, 1,  14, 1,  14}},
        {Block::FLOWER_BLUE,   {12, 15, 12, 15, 12, 15}},
        {Block::FLOWER_YELLOW, {13, 15, 13, 15, 13, 15}},
        {Block::BUSH,          {14, 4,  14, 4,  14, 4}},
        {Block::WET_DIRT,      {6,  10, 6,  10, 6,  10}},
        {Block::TERRACOTTA,    {15, 14, 15, 14, 15, 14}},
};
