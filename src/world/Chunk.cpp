#include "Chunk.h"
#include "glm/gtx/norm.hpp"
#include <map>
#include <queue>
#include "../camera/Camera.h"
#include "../utils/Random.hpp"
#include "../utils/Logger.h"

#define MAX_VERTEX_COUNT 24000 // each cube has 6 faces, each face has 4 vertices
#define assertm(exp, msg) assert(((void)msg, exp))

static constexpr int WEST = 0;
static constexpr int NORTH = 1;
static constexpr int EAST = 2;
static constexpr int SOUTH = 3;

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

static int mod(int a, int b)
{
    int res = a % b;
    return res >= 0 ? res : res + b;
}

uint32_t Chunk::GetIndex(uint8_t i, uint8_t j, uint8_t k) const
{
    return m_Chunk.GetIndex(i, j, k);
}

void Chunk::CreateQuad(std::vector<Vertex>& target, const glm::uvec3& position,
    const std::array<uint32_t, 4>& textureCoords, const glm::uvec4& offsetx,
    const glm::uvec4& offsety, const glm::uvec4& offsetz, uint8_t normalIndex, uint16_t lightVal)
{
    static constexpr int vertices = 4;

    for (int i = 0; i < vertices; i++) {
        uint32_t v = normalIndex << 29 | (position[0] + offsetx[i]) << 24
            | (position[1] + offsety[i]) << 15 | (position[2] + offsetz[i]) << 10
            | textureCoords[i];
        target.emplace_back(v, lightVal);
    }
}

void Chunk::CreateQuadOutline(std::vector<uint32_t>& target, const glm::uvec3& position,
    const glm::uvec4& offsetx, const glm::uvec4& offsety, const glm::uvec4& offsetz)
{
    static constexpr int vertices = 4;

    for (int i = 0; i < vertices; i++) {
        uint32_t v = (position[0] + offsetx[i]) << 24 | (position[1] + offsety[i]) << 15
            | (position[2] + offsetz[i]) << 10;
        target.emplace_back(v);
    }
}

Chunk::Chunk(
    const ChunkCoord& coords, const glm::vec3& position, uint32_t stride, ChunkMap* chunkMap)
    : m_HeightMap({})
    , m_Coords(coords)
    , m_ChunkPosition(position)
    , m_LightMap(0)
    , m_Neighbors({ nullptr })
    , m_MinHeight(YSIZE)
    , m_MaxHeight(0)
    , m_IBOCount(0)
    , m_TIBOCount(0)
    , m_SIBOCount(0)
    , m_ChunkMap(chunkMap)
    , m_CenterPosition({ m_ChunkPosition.x + static_cast<float>(XSIZE) / 2, 0,
          m_ChunkPosition.z + static_cast<float>(ZSIZE) / 2 })
{
    m_VBO.Init(stride, 0);
    m_VBO.CreateDynamic(sizeof(Vertex) * MAX_VERTEX_COUNT);

    m_Mesh.reserve(MAX_VERTEX_COUNT);
    m_TransparentMesh.reserve(MAX_VERTEX_COUNT / 4);
    m_SpriteMesh.reserve(2048); // 8 vertices * 16x16
    CreateHeightMap();
    FastFill();
}

#if 1

void Chunk::CreateHeightMap()
{
    int index = 0;
    static constexpr float fudge_factor = 1.0f;
    static constexpr float exp = 3.0f;

    std::array<float, ZSIZE* XSIZE> noiseMapA
        = m_Noise.OctaveNoiseGrid<ZSIZE, XSIZE>(m_ChunkPosition.z, m_ChunkPosition.x, 6, 0.001f);

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
void Chunk::CreateHeightMap()
{
    static constexpr int elevation = 50; // increase for less water
    int index = 0;

    std::array<float, ZSIZE* XSIZE> noiseMapA
        = m_Noise.OctaveNoiseGrid<ZSIZE, XSIZE>(m_ChunkPosition.z, m_ChunkPosition.x, 6, 0.002f);
    std::array<float, XSIZE* ZSIZE> noiseMapM = m_Noise.OctaveNoiseGrid<XSIZE, ZSIZE>(
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

void Chunk::FastFill()
{
    assertm(m_MinHeight > 1, "minHeight too small");
    assertm(m_MaxHeight < YSIZE - 1, "maxHeight too big");
    static constexpr int level_size = XSIZE * ZSIZE;
    auto begin = m_Chunk.GetRawPtr();
    std::fill(begin, begin + level_size, Block::BEDROCK);
    std::fill(begin + level_size, begin + level_size * (m_MinHeight), Block::STONE);
    std::fill(begin + level_size * (m_MaxHeight + 1), begin + level_size * YSIZE, Block::EMPTY);
}

void Chunk::FillSunLight()
{
    static constexpr int level_size = XSIZE * ZSIZE;
    static constexpr int sunLight = 0xF000;
    auto lightBegin = m_LightMap.GetRawPtr();
    std::fill(
        lightBegin + level_size * (m_MaxHeight + 1), lightBegin + level_size * YSIZE, sunLight);
}

BlockVec Chunk::CreateSurfaceLayer(const BlockVec& blocksToSet)
{
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
                        m_LightMap(i, j, k) = 0;
                        float dirt = m_Noise.OctaveNoiseSingle(
                            k + m_ChunkPosition.z + 1000.1f, i + m_ChunkPosition.x, 4, 0.001f);

                        if (dirt >= 0)
                            m_Chunk(i, j, k) = Block::DIRT;
                        else
                            m_Chunk(i, j, k) = Block::STONE;
                    } else if (j == height) {
                        float noise_chance = m_Noise.OctaveNoiseSingle(
                            k + m_ChunkPosition.z, i + m_ChunkPosition.x + 100.0f, 8, 0.002f);
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
                            if (Random::GetRand<double>(0, 1)
                                < (3.0f / static_cast<float>(snow_level - j)))
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
void Chunk::CreateTrees(int i, int j, int k, BlockVec& blockVec)
{
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
                                    && Random::GetRand<double>(0, 1) > 0.2) {
                                    m_Chunk(leafx, leaves_height, leafz) = Block::LEAVES;
                                }
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
        VertexBuffer m_VBO;
    }
}

void Chunk::CreateSprites(int i, int j, int k, BlockVec& blockVec)
{
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

template <typename... Args>
bool Chunk::CheckNeighbor(const Chunk* const chunk, const glm::uvec3& position, Args... voidBlocks)
{
    if (((chunk->m_Chunk(position.x, position.y, position.z) == voidBlocks) || ...))
        return true;
    else
        return false;
}

void Chunk::GenCubeOutline(int i, int j, int k, std::vector<uint32_t>& target)
{
    CreateQuadOutline(target, { i, j, k }, // D
        { 0, 0, 1, 1 }, glm::uvec4(0), { 1, 0, 0, 1 });
    CreateQuadOutline(target, { i, j, k }, // U
        { 1, 1, 0, 0 }, glm::uvec4(1), { 1, 0, 0, 1 });
    CreateQuadOutline(target, { i, j, k }, // F
        { 0, 0, 1, 1 }, { 1, 0, 0, 1 }, glm::uvec4(1));
    CreateQuadOutline(target, { i, j, k }, // B
        { 1, 1, 0, 0 }, { 1, 0, 0, 1 }, glm::uvec4(0));
    CreateQuadOutline(target, { i, j, k }, // L
        glm::uvec4(0), { 1, 0, 0, 1 }, { 0, 0, 1, 1 });
    CreateQuadOutline(target, { i, j, k }, // R
        glm::uvec4(1), { 1, 0, 0, 1 }, { 1, 1, 0, 0 });
}

template <typename... Args>
void Chunk::GenCube(int i, int j, int k, std::vector<Vertex>& target,
    const TextureAtlas::Coords& textureCoords, bool lightBlock, Args... voidBlocks)
{
    if (j > 0 && ((m_Chunk(i, j - 1, k) == voidBlocks) || ...)) {
        int16_t lightVal;
        if (lightBlock)
            lightVal = m_LightMap(i, j, k);
        else
            lightVal = m_LightMap(i, j - 1, k);
        CreateQuad(target, { i, j, k }, textureCoords.bottom, // D
            { 0, 0, 1, 1 }, glm::uvec4(0), { 1, 0, 0, 1 }, 3, lightVal);
    }
    if (j < YSIZE - 1 && ((m_Chunk(i, j + 1, k) == voidBlocks) || ...)) {
        int16_t lightVal;
        if (lightBlock)
            lightVal = m_LightMap(i, j, k);
        else
            lightVal = m_LightMap(i, j + 1, k);
        CreateQuad(target, { i, j, k }, textureCoords.top, // U
            { 0, 1, 1, 0 }, glm::uvec4(1), { 1, 1, 0, 0 }, 2, lightVal);
    }
    if ((k == 0 && CheckNeighbor(m_Neighbors[NORTH], { i, j, ZSIZE - 1 }, voidBlocks...))
        || (k > 0 && ((m_Chunk(i, j, k - 1) == voidBlocks) || ...))) {
        uint16_t lightVal;
        if (lightBlock)
            lightVal = m_LightMap(i, j, k);
        else if (k == 0) {
            lightVal = m_Neighbors[NORTH]->m_LightMap(i, j, ZSIZE - 1);
        } else {
            lightVal = m_LightMap(i, j, k - 1);
        }
        CreateQuad(target, { i, j, k }, textureCoords.side, // B
            { 1, 0, 0, 1 }, { 0, 0, 1, 1 }, glm::uvec4(0), 5, lightVal);
    }
    if ((k == ZSIZE - 1 && CheckNeighbor(m_Neighbors[SOUTH], { i, j, 0 }, voidBlocks...))
        || (k < ZSIZE - 1 && ((m_Chunk(i, j, k + 1) == voidBlocks) || ...))) {
        uint16_t lightVal;
        if (lightBlock)
            lightVal = m_LightMap(i, j, k);
        else if (k == ZSIZE - 1) {
            lightVal = m_Neighbors[SOUTH]->m_LightMap(i, j, 0);
        } else {
            lightVal = m_LightMap(i, j, k + 1);
        }
        CreateQuad(target, { i, j, k }, textureCoords.side, // F
            { 0, 1, 1, 0 }, { 0, 0, 1, 1 }, glm::uvec4(1), 4, lightVal);
    }
    if ((i == 0 && CheckNeighbor(m_Neighbors[WEST], { XSIZE - 1, j, k }, voidBlocks...))
        || (i > 0 && ((m_Chunk(i - 1, j, k) == voidBlocks) || ...))) {
        uint16_t lightVal;
        if (lightBlock)
            lightVal = m_LightMap(i, j, k);
        else if (i == 0) {
            lightVal = m_Neighbors[WEST]->m_LightMap(XSIZE - 1, j, k);
        } else {
            lightVal = m_LightMap(i - 1, j, k);
        }
        CreateQuad(target, { i, j, k }, textureCoords.side, // L
            glm::uvec4(0), { 0, 0, 1, 1 }, { 0, 1, 1, 0 }, 1, lightVal);
    }
    if ((i == XSIZE - 1 && CheckNeighbor(m_Neighbors[EAST], { 0, j, k }, voidBlocks...))
        || (i < XSIZE - 1 && ((m_Chunk(i + 1, j, k) == voidBlocks) || ...))) {
        uint16_t lightVal;
        if (lightBlock)
            lightVal = m_LightMap(i, j, k);
        else if (i == XSIZE - 1) {
            lightVal = m_Neighbors[EAST]->m_LightMap(0, j, k);
        } else {
            lightVal = m_LightMap(i + 1, j, k);
        }
        CreateQuad(target, { i, j, k }, textureCoords.side, // R
            glm::uvec4(1), { 0, 0, 1, 1 }, { 1, 0, 0, 1 }, 0, lightVal);
    }
}

void Chunk::GenSprite(
    int i, int j, int k, std::vector<Vertex>& target, const std::array<uint32_t, 4>& textureCoords)
{
    static constexpr int vertices = 4;
    static constexpr int upNormal = 2;
    static constexpr std::array<uint8_t, 8> offsetx = { 0, 1, 1, 0, 0, 1, 1, 0 };
    static constexpr std::array<uint8_t, 8> offsety = { 0, 0, 1, 1, 0, 0, 1, 1 };
    static constexpr std::array<uint8_t, 8> offsetz = { 0, 1, 1, 0, 1, 0, 0, 1 };

    for (int n = 0; n < 8; n++) {
        uint32_t v = (upNormal << 29) | (i + offsetx[n]) << 24 | (j + offsety[n]) << 15
            | (k + offsetz[n]) << 10 | textureCoords[n % vertices];
        uint16_t l = m_LightMap(i, j, k);
        target.emplace_back(v, l);
    }
}

bool Chunk::FindNeighbors()
{
    m_Neighbors = { nullptr }; // west, north, east, south
    int n = 0;
    for (int i = -1; i <= 1; i += 2) {
        if (const auto it = m_ChunkMap->find({ m_Coords.x + i, m_Coords.z });
            it != m_ChunkMap->end()) {
            m_Neighbors[n++] = &it->second;
        } else { // border chunk, don't generate the mesh
            return false;
        }

        if (const auto it = m_ChunkMap->find({ m_Coords.x, m_Coords.z + i });
            it != m_ChunkMap->end()) {
            m_Neighbors[n++] = &it->second;
        } else { // border chunk, don't generate the mesh
            return false;
        }
    }
    return true;
}

void Chunk::GenerateMesh()
{
    if (m_Mesh.empty() && m_TransparentMesh.empty() && m_SpriteMesh.empty()) {
        // retrieve the 4 pointers from the map and continue only if all of them exist
        if (m_MinHeight < 1)
            m_MinHeight = 1;
        for (int j = m_MinHeight - 1; j <= m_MaxHeight; j++) {
            for (int i = 0; i < XSIZE; i++) {
                for (int k = 0; k < ZSIZE; k++) {
                    TextureAtlas::Coords textureCoords = TextureAtlas::At(m_Chunk(i, j, k));
                    switch (m_Chunk(i, j, k)) {
                    case Block::EMPTY:
                        break;
                    case Block::FLOWER_YELLOW:
                    case Block::FLOWER_BLUE:
                    case Block::BUSH:
                        GenSprite(i, j, k, m_SpriteMesh, textureCoords.side);
                        break;
                    case Block::WATER:
                    case Block::LEAVES:
                        GenCube(i, j, k, m_TransparentMesh, textureCoords, false, Block::EMPTY,
                            Block::FLOWER_BLUE, Block::FLOWER_YELLOW, Block::BUSH);
                        break;
                    case Block::LIGHT_RED:
                    case Block::LIGHT_BLUE:
                    case Block::LIGHT_GREEN:
                        GenCube(i, j, k, m_Mesh, textureCoords, true, Block::EMPTY, Block::LEAVES,
                            Block::WATER, Block::FLOWER_BLUE, Block::FLOWER_YELLOW, Block::BUSH);
                    default:
                        GenCube(i, j, k, m_Mesh, textureCoords, false, Block::EMPTY, Block::LEAVES,
                            Block::WATER, Block::FLOWER_BLUE, Block::FLOWER_YELLOW, Block::BUSH);
                        break;
                    }
                }
            }
        }
        // solid mesh
        size_t indexCount = m_Mesh.size() / 4 * 6; // num faces * 6
        m_IBOCount = indexCount;
        uint32_t solidSize = m_Mesh.size() * sizeof(Vertex);
        uint32_t transSize = 0;
        m_VBO.SendData(solidSize, m_Mesh.data(), 0);

        if (!m_TransparentMesh.empty()) {
            indexCount = m_TransparentMesh.size() / 4 * 6;
            m_TIBOCount = indexCount;
            transSize = m_TransparentMesh.size() * sizeof(Vertex);
            m_VBO.SendData(transSize, m_TransparentMesh.data(), solidSize);
        }
        if (!m_SpriteMesh.empty()) {
            uint32_t offset = solidSize + transSize;
            indexCount = m_SpriteMesh.size() / 4 * 6;
            m_SIBOCount = indexCount;
            m_VBO.SendData(m_SpriteMesh.size() * sizeof(Vertex), m_SpriteMesh.data(), offset);
        }
    }
}

void Chunk::Render(ChunkRenderer& renderer, const ChunkCoord& playerChunk, int radius)
{
    renderer.SetIboCount(m_IBOCount);
    m_VBO.Bind(renderer.GetVaoId());
    renderer.Render(m_ChunkPosition, 0);
    if (!m_TransparentMesh.empty()) {
        renderer.SetIboCount(m_TIBOCount);
        glDisable(GL_CULL_FACE);
        renderer.Render(m_ChunkPosition, m_Mesh.size());
        glEnable(GL_CULL_FACE);
    }
    if (!m_SpriteMesh.empty() && ChunkIsVisible(playerChunk, radius)) {
        renderer.SetIboCount(m_SIBOCount);
        glDisable(GL_CULL_FACE);
        renderer.Render(m_ChunkPosition, m_Mesh.size() + m_TransparentMesh.size());
        glEnable(GL_CULL_FACE);
    }
}

void Chunk::RenderOutline(ChunkRenderer& renderer, const glm::uvec3& target)
{
    m_Neighbors = { nullptr };
    if (FindNeighbors()) {
        int i = static_cast<int>(target.x);
        int j = static_cast<int>(target.y);
        int k = static_cast<int>(target.z);
        if (m_Chunk(i, j, k) != Block::EMPTY && m_Chunk(i, j, k) != Block::WATER) {
            std::vector<uint32_t> outlineMesh;
            GenCubeOutline(i, j, k, outlineMesh);
            size_t indexCount = outlineMesh.size();
            renderer.SendOutlineData(outlineMesh.size() * sizeof(uint32_t), outlineMesh.data(), 0);
            renderer.RenderOutline(m_ChunkPosition, target, indexCount);
        }
    }
}

Block Chunk::GetBlock(uint32_t x, uint32_t y, uint32_t z) const { return m_Chunk(x, y, z); }

void Chunk::SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block)
{
    assertm(x < XSIZE && y < YSIZE && z < ZSIZE, "indices out of bounds.");
    m_Chunk(x, y, z) = block;
    if (y < m_MinHeight)
        m_MinHeight = y;
    else if (y > m_MaxHeight)
        m_MaxHeight = y;

    ClearMesh();
}

void Chunk::ClearMesh()
{
    m_Mesh.clear();
    m_TransparentMesh.clear();
    m_SpriteMesh.clear();
}

glm::vec3 Chunk::GetCenterPosition() const { return m_CenterPosition; }

void Chunk::SetBlocks(const BlockVec& blocksToSet)
{
    for (const auto& [block, vec] : blocksToSet) {
        glm::uvec3 pos = vec;
        SetBlock(pos.x, pos.y, pos.z, block);
    }
}

ChunkCoord Chunk::GetCoord() const { return m_Coords; }

bool Chunk::ChunkIsVisible(ChunkCoord playerChunk, int radius) const
{
    if (std::abs(playerChunk.x - m_Coords.x) > radius
        || std::abs(playerChunk.z - m_Coords.z) > radius)
        return false;
    else
        return true;
}

void Chunk::UpdateMeshHeighLimit(uint32_t height)
{
    if (height > m_MaxHeight)
        m_MaxHeight = height;
    else if (height < m_MinHeight)
        m_MinHeight = height;
}

// SSSS RRRR GGGG BBBB
void Chunk::SetSunLight(int i, int j, int k, uint8_t val)
{
    m_LightMap(i, j, k) = (m_LightMap(i, j, k) & 0x0FFF) | (val & 0xF) << 12;
}

uint8_t Chunk::GetSunLight(int i, int j, int k) { return m_LightMap(i, j, k) >> 12 & 0xF; }

void Chunk::SetRedLight(int i, int j, int k, uint8_t val)
{
    m_LightMap(i, j, k) = (m_LightMap(i, j, k) & 0xF0FF) | (val & 0xF) << 8;
}

uint8_t Chunk::GetRedLight(int i, int j, int k) { return m_LightMap(i, j, k) >> 8 & 0xF; }

void Chunk::SetGreenLight(int i, int j, int k, uint8_t val)
{
    m_LightMap(i, j, k) = (m_LightMap(i, j, k) & 0xFF0F) | (val & 0xF) << 4;
}

uint8_t Chunk::GetGreenLight(int i, int j, int k) { return m_LightMap(i, j, k) >> 4 & 0xF; }

void Chunk::SetBlueLight(int i, int j, int k, uint8_t val)
{
    m_LightMap(i, j, k) = (m_LightMap(i, j, k) & 0xFFF0) | (val & 0xF);
}

uint8_t Chunk::GetBlueLight(int i, int j, int k) { return m_LightMap(i, j, k) & 0xF; }

std::array<Chunk*, 4> Chunk::GetNeighbors() const { return m_Neighbors; }

glm::uvec3 Chunk::GetCoordsFromIndex(uint32_t index) const
{
    return m_Chunk.GetCoordsFromIndex(index);
}

int Chunk::GetMaxHeight() const { return m_MaxHeight; }