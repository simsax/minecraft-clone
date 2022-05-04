#include "Chunk.h"
#include <iostream>
#include <cstdlib>
#include <map>
#include "Noise.h"
#include "Camera.h"
#include <glm/gtx/norm.hpp>

const float Chunk::s_TextureOffset = 0.0625f; // texture_size/atlas_size
const std::vector<std::array<int, 2>> Chunk::n1_offsets = [](){
    srand(time(NULL));
    std::vector<std::array<int, 2>> octaveOffsets;
    for (unsigned int i = 0; i < 8; i++) {
        octaveOffsets.push_back({ rand() % 10000, rand() % 10000 });
    }
    return octaveOffsets;
}();
const std::vector<std::array<int, 2>> Chunk::n2_offsets = [](){
    srand(time(NULL));
    std::vector<std::array<int, 2>> octaveOffsets{};
    for (unsigned int i = 0; i < 8; i++) {
        octaveOffsets.push_back({ rand() % 10000, rand() % 10000 });
    }
    return octaveOffsets;
}();
const std::vector<std::array<int, 2>> Chunk::n3_offsets = [](){
    srand(time(NULL));
    std::vector<std::array<int, 2>> octaveOffsets{};
    for (unsigned int i = 0; i < 6; i++) {
        octaveOffsets.push_back({ rand() % 10000, rand() % 10000 });
    }
    return octaveOffsets;
}();
const std::vector<std::array<int, 2>> Chunk::n4_offsets = [](){
    srand(time(NULL));
    std::vector<std::array<int, 2>> octaveOffsets{};
    for (unsigned int i = 0; i < 8; i++) {
        octaveOffsets.push_back({ rand() % 10000, rand() % 10000 });
    }
    return octaveOffsets;
}();

bool operator==(const ChunkCoord& l, const ChunkCoord& r)
{
    return l.x == r.x && l.z == r.z;
}

bool operator!=(const ChunkCoord& l, const ChunkCoord& r)
{
    return l.x != r.x || l.z != r.z;
}

int operator-(const ChunkCoord& l, const ChunkCoord& r) {
    return static_cast<int>(round(sqrt(pow(l.x - r.x, 2) + pow(l.z - r.z, 2))));
}

std::size_t hash_fn::operator()(const ChunkCoord& coord) const
{
    std::size_t h1 = std::hash<int>()(coord.x);
    std::size_t h2 = std::hash<int>()(coord.z);
    return h1 ^ h2;
}

static void CreateUQuad(std::vector<Vertex>& target, const glm::vec3& position,
                        const std::array<float, 24>& textureCoords) {
    float size = 1.0f;

    target.emplace_back(glm::vec3(position[0], position[1] + size, position[2] + size),
                        glm::vec2(textureCoords[0], textureCoords[1]));
    target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2] + size),
                        glm::vec2(textureCoords[2], textureCoords[3]));
    target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2]),
                        glm::vec2(textureCoords[4], textureCoords[5]));
    target.emplace_back(glm::vec3(position[0], position[1] + size, position[2]),
                        glm::vec2(textureCoords[6], textureCoords[7]));
}

static void CreateDQuad(std::vector<Vertex>& target, const glm::vec3& position,
                        const std::array<float, 24>& textureCoords) {
    float size = 1.0f;

    target.emplace_back(glm::vec3(position[0], position[1], position[2] + size),
                        glm::vec2(textureCoords[16], textureCoords[17]));
    target.emplace_back(glm::vec3(position[0], position[1], position[2]),
                        glm::vec2(textureCoords[18], textureCoords[19]));
    target.emplace_back(glm::vec3(position[0] + size, position[1], position[2]),
                        glm::vec2(textureCoords[20], textureCoords[21]));
    target.emplace_back(glm::vec3(position[0] + size, position[1], position[2] + size),
                        glm::vec2(textureCoords[22], textureCoords[23]));
}

static void CreateFQuad(std::vector<Vertex>& target, const glm::vec3& position,
                        const std::array<float, 24>& textureCoords) {
    float size = 1.0f;

    target.emplace_back(glm::vec3(position[0], position[1], position[2] + size),
                        glm::vec2(textureCoords[8], textureCoords[9]));
    target.emplace_back(glm::vec3(position[0] + size, position[1], position[2] + size),
                        glm::vec2(textureCoords[10], textureCoords[11]));
    target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2] + size),
                        glm::vec2(textureCoords[12], textureCoords[13]));
    target.emplace_back(glm::vec3(position[0], position[1] + size, position[2] + size),
                        glm::vec2(textureCoords[14], textureCoords[15]));
}

static void CreateBQuad(std::vector<Vertex>& target, const glm::vec3& position,
                        const std::array<float, 24>& textureCoords) {
    float size = 1.0f;

    target.emplace_back(glm::vec3(position[0] + size, position[1], position[2]),
                        glm::vec2(textureCoords[8], textureCoords[9]));
    target.emplace_back(glm::vec3(position[0], position[1], position[2]),
                        glm::vec2(textureCoords[10], textureCoords[11]));
    target.emplace_back(glm::vec3(position[0], position[1] + size, position[2]),
                        glm::vec2(textureCoords[12], textureCoords[13]));
    target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2]),
                        glm::vec2(textureCoords[14], textureCoords[15]));
}

static void CreateRQuad(std::vector<Vertex>& target, const glm::vec3& position,
                        const std::array<float, 24>& textureCoords) {
    float size = 1.0f;

    target.emplace_back(glm::vec3(position[0] + size, position[1], position[2] + size),
                        glm::vec2(textureCoords[8], textureCoords[9]));
    target.emplace_back(glm::vec3(position[0] + size, position[1], position[2]),
                        glm::vec2(textureCoords[10], textureCoords[11]));
    target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2]),
                        glm::vec2(textureCoords[12], textureCoords[13]));
    target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2] + size),
                        glm::vec2(textureCoords[14], textureCoords[15]));
}

static void CreateLQuad(std::vector<Vertex>& target, const glm::vec3& position,
                        const std::array<float, 24>& textureCoords) {
    float size = 1.0f;

    target.emplace_back(glm::vec3(position[0], position[1], position[2]),
                        glm::vec2(textureCoords[8], textureCoords[9]));
    target.emplace_back(glm::vec3(position[0], position[1], position[2] + size),
                        glm::vec2(textureCoords[10], textureCoords[11]));
    target.emplace_back(glm::vec3(position[0], position[1] + size, position[2] + size),
                        glm::vec2(textureCoords[12], textureCoords[13]));
    target.emplace_back(glm::vec3(position[0], position[1] + size, position[2]),
                        glm::vec2(textureCoords[14], textureCoords[15]));
}

// values are precomputed for speed
const std::unordered_map<Block, std::array<float, 24>> Chunk::s_TextureMap =
    {
        { Block::GRASS,{0.75, 0.1875, 0.8125, 0.1875, 0.8125, 0.25, 0.75, 0.25, // top
                            0.1875, 0.9375, 0.25, 0.9375, 0.25, 1.0, 0.1875, 1.0, // side
                            0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0 } }, //bottom
        { Block::DIRT,{0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0,
                       0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0,
                       0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0 } },
        { Block::STONE,{0.0625, 0.9375, 0.125, 0.9375, 0.125, 1.0, 0.0625, 1.0,
                            0.0625, 0.9375, 0.125, 0.9375, 0.125, 1.0, 0.0625, 1.0,
                            0.0625, 0.9375, 0.125, 0.9375, 0.125, 1.0, 0.0625, 1.0 } },
        { Block::DIAMOND,{0.125, 0.75, 0.1875, 0.75, 0.1875, 0.8125, 0.125, 0.8125,
                               0.125, 0.75, 0.1875, 0.75, 0.1875, 0.8125, 0.125, 0.8125,
                               0.125, 0.75, 0.1875, 0.75, 0.1875, 0.8125, 0.125, 0.8125 } },
        { Block::GOLD,	{0.0, 0.8125, 0.0625, 0.8125, 0.0625, 0.875, 0.0, 0.875,
                                0.0, 0.8125, 0.0625, 0.8125, 0.0625, 0.875, 0.0, 0.875,
                                0.0, 0.8125, 0.0625, 0.8125, 0.0625, 0.875, 0.0, 0.875 } },
        { Block::COAL,	{0.125, 0.8125, 0.1875, 0.8125, 0.1875, 0.875, 0.125, 0.875,
                                0.125, 0.8125, 0.1875, 0.8125, 0.1875, 0.875, 0.125, 0.875,
                                0.125, 0.8125, 0.1875, 0.8125, 0.1875, 0.875, 0.125, 0.875 } },
        { Block::STEEL,	{0.0625, 0.8125, 0.125, 0.8125, 0.125, 0.875, 0.0625, 0.875,
                                0.0625, 0.8125, 0.125, 0.8125, 0.125, 0.875, 0.0625, 0.875,
                                0.0625, 0.8125, 0.125, 0.8125, 0.125, 0.875, 0.0625, 0.875 } },
        { Block::LEAVES,	{0.6875, 0.0625, 0.75, 0.0625, 0.75, 0.125, 0.6875, 0.125,
                                0.6875, 0.0625, 0.75, 0.0625, 0.75, 0.125, 0.6875, 0.125,
                                0.6875, 0.0625, 0.75, 0.0625, 0.75, 0.125, 0.6875, 0.125 } },
        { Block::WOOD,	{0.3125, 0.875, 0.375, 0.875, 0.375, 0.9375, 0.3125, 0.9375,
                                0.25, 0.875, 0.3125, 0.875, 0.3125, 0.9375, 0.25, 0.9375,
                                0.3125, 0.875, 0.375, 0.875, 0.375, 0.9375, 0.3125, 0.9375 } },
        { Block::SNOW,	{0.125, 0.6875, 0.1875, 0.6875, 0.1875, 0.75, 0.125, 0.75,
                                0.125, 0.6875, 0.1875, 0.6875, 0.1875, 0.75, 0.125, 0.75,
                                0.125, 0.6875, 0.1875, 0.6875, 0.1875, 0.75, 0.125, 0.75,} },
        { Block::SNOWY_GRASS,{0.125, 0.6875, 0.1875, 0.6875, 0.1875, 0.75, 0.125, 0.75,
                                  0.25, 0.6875, 0.3125, 0.6875, 0.3125, 0.75, 0.25, 0.75,
                                  0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0 } },
        { Block::WATER,	{0.9375, 0.125, 1.0, 0.125, 1.0, 0.1875, 0.9375, 0.1875,
                                0.9375, 0.125, 1.0, 0.125, 1.0, 0.1875, 0.9375, 0.1875,
                                0.9375, 0.125, 1.0, 0.125, 1.0, 0.1875, 0.9375, 0.1875 } },
        { Block::SAND,	{0.125, 0.875, 0.1875, 0.875, 0.1875, 0.9375, 0.125, 0.9375,
                                0.125, 0.875, 0.1875, 0.875, 0.1875, 0.9375, 0.125, 0.9375,
                                0.125, 0.875, 0.1875, 0.875, 0.1875, 0.9375, 0.125, 0.9375 } },
        { Block::GRAVEL,	{0.1875, 0.875, 0.25, 0.875, 0.25, 0.9375, 0.1875, 0.9375,
                                0.1875, 0.875, 0.25, 0.875, 0.25, 0.9375, 0.1875, 0.9375,
                                0.1875, 0.875, 0.25, 0.875, 0.25, 0.9375, 0.1875, 0.9375 } },
        { Block::BEDROCK,   {0.0625, 0.875, 0.125, 0.875, 0.125, 0.9375, 0.0625, 0.9375,
                            0.0625, 0.875, 0.125, 0.875, 0.125, 0.9375, 0.0625, 0.9375,
                            0.0625, 0.875, 0.125, 0.875, 0.125, 0.9375, 0.0625, 0.9375} },
    };

// the chunk has a border so that I know what faces to cull between chunks
// (I only generate the mesh of the part inside the border)
Chunk::Chunk(glm::vec3 position,
             const VertexBufferLayout& layout, unsigned int maxVertexCount, const std::vector<unsigned int>& indices,
             glm::vec3* playerPosition) :
        m_Position(position), m_Chunk(Matrix3D<Block, XSIZE, YSIZE, ZSIZE>()), m_MaxVertexCount(maxVertexCount),
        m_PlayerPosition(playerPosition)
{

    m_IBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
    m_TransparentIBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
    m_VBO = std::make_unique<VertexBuffer>();
    m_TransparentVBO = std::make_unique<VertexBuffer>();
    m_VBO->CreateDynamic(sizeof(Vertex) * maxVertexCount);
    m_TransparentVBO->CreateDynamic(sizeof(Vertex) * maxVertexCount);
    m_VAO = std::make_unique<VertexArray>();
    m_TransparentVAO = std::make_unique<VertexArray>();
    m_VAO->AddBuffer(*m_VBO, layout);
    m_TransparentVAO->AddBuffer(*m_TransparentVBO, layout);

    m_Mesh.reserve(m_MaxVertexCount);
    TerrainHeightGeneration();
}

// fine tune the values
static float Continentalness(int x, int y) {
    float scale = 256.0f;
    float ampl = 1.0f;
    float freq = 1.0f;
    float height = 0;

    float noise = noise::Perlin2D(x, y, scale, freq, ampl, 8);

    if (noise < 0.3) {
        //height = (noise + 2.3f) / 0.026f;
        height = (200.0f * noise + 980)/13.0f;
    }
    else if (0.3 <= noise && noise < 0.8) {
        //height = 40 * noise + 88;
        height = 40 * noise + 68;
    }
    else if (noise >= 0.8) {
        //height = 120;
        height = 100;
    }

    return height;
}


void Chunk::TerrainHeightGeneration() {
    int water_level = 63;
    int snow_level = 100;

    for (unsigned int i = 0; i < XSIZE; i++) {
        for (unsigned int k = 0; k < ZSIZE; k++) {
            float terrain_height = Continentalness(i + m_Position.x, k + m_Position.z);
            if (terrain_height < water_level)
                terrain_height = water_level;
            float hHigh;
            float hLow = noise::CombinedNoise((i + m_Position.x) * 1.3f,
                                              (k + m_Position.z) * 1.3f,
                                              1,1,1,8,
                                              n1_offsets) / 6 - 4;
            float height = hLow;

            if (noise::OctaveNoise(i + m_Position.x, k + m_Position.z,
                                   1,1,1,6, n3_offsets) <= 0 ) {
                hHigh = noise::CombinedNoise((i + m_Position.x) * 1.3f,
                                             (k + m_Position.z) * 1.3f,
                                             1,1,1,8,
                                             n2_offsets) / 5 + 6;
                height = std::max(hLow, hHigh);
            }

            height *= 0.5f;
            if (height < 0)
                height *= 0.8f;

            auto maxHeight = static_cast<unsigned int>(height + terrain_height);

            int dirtThickness = static_cast<int>(
                    noise::OctaveNoise(i + m_Position.x,
                                       k + m_Position.z,
                                       1, 1,1,8,
                                       n4_offsets) / 6) ;

            for (unsigned int j = 0; j < YSIZE; j++) {
                if (j <= 1)
                    m_Chunk(i,j,k) = Block::BEDROCK;
                else {
                    if (j < maxHeight) {
                        if (j < terrain_height){
                            m_Chunk(i, j, k) = Block::STONE;
                        } else if (j < terrain_height + dirtThickness) {
                            m_Chunk(i, j, k) = Block::DIRT;
                        } else {
                            m_Chunk(i, j, k) = Block::STONE;
                        }
                    } else {
                        if (j < water_level) {
                            m_Chunk(i,j,k) = Block::WATER;
                        } else {
                            m_Chunk(i,j,k) = Block::EMPTY;
                        }
                    }
                }
            }
            // create surface layer
            if (maxHeight < YSIZE - 1) {
                bool sandChance = noise::OctaveNoise(i + m_Position.x, k + m_Position.z,
                                                   1, 1, 1, 8, n1_offsets) > 8;
                bool gravelChance = noise::OctaveNoise(i + m_Position.x, k + m_Position.z,
                                                   1, 1, 1, 8, n2_offsets) > 12;
                bool snowChance = noise::OctaveNoise(i + m_Position.x, k + m_Position.z,
                                                   1, 1, 1, 8, n2_offsets) > 8;

                Block blockAbove = m_Chunk(i,maxHeight+1,k);
                if (blockAbove == Block::WATER && gravelChance) {
                    m_Chunk(i, maxHeight, k) = Block::GRAVEL;
                }
                else if (blockAbove == Block::EMPTY) {
                    if (maxHeight <= water_level && sandChance) {
                        m_Chunk(i, maxHeight, k) = Block::SAND;
                    } else if (maxHeight >= snow_level && maxHeight < snow_level + 5 && snowChance ||
                                maxHeight >= snow_level + 5) {
                        m_Chunk(i, maxHeight, k) = Block::SNOW;
                    } else if (maxHeight >= snow_level - 5 && snowChance) {
                        m_Chunk(i, maxHeight, k) = Block::SNOWY_GRASS;
                    } else {
                        m_Chunk(i, maxHeight, k) = Block::GRASS;
                    }
                }
            }
        }
    }
}

void Chunk::Noise3DInit(unsigned int seed) {
    srand(seed);
    float freq = 0.5f;
    float ampl = 20.0f;
    float scale = 64.0f;
    std::vector<std::vector<int>> octaveOffsets;
    unsigned int n_octaves = 1;
    for (unsigned int i = 0; i < n_octaves; i++) {
        octaveOffsets.push_back({ rand() % 10000, rand() % 10000, rand() % 10000 });
    }

    for (int i = 0; i < XSIZE; i++) {
        for (int k = 0; k < ZSIZE; k++) {
            for (int j = 0; j < YSIZE; j++) {
                float density = noise::Perlin3D(i + (int)m_Position.x,
                                                j + (int)m_Position.y,
                                                k + (int)m_Position.z, scale, 1, 1);
                if (density <= 0 || j > 150) {
                    m_Chunk(i, j, k) = Block::EMPTY;
                }
                else {
                    m_Chunk(i, j, k) = Block::STONE;
                }
            }
        }
    }
}

static std::vector<Vertex> GenerateTransVector(const std::multimap<float, std::vector<Vertex>,
        std::greater<>>& transparentMeshMap) {
    std::vector<Vertex> transparentMesh;
    transparentMesh.reserve(transparentMeshMap.size() * 4);
    for (const auto& [key, val] : transparentMeshMap) {
        transparentMesh.insert(transparentMesh.end(), std::make_move_iterator(val.begin()),
                               std::make_move_iterator(val.end()));
    }
    return transparentMesh;
}

void Chunk::GenerateMesh() {
    if (m_Mesh.empty() && m_TransparentMesh.empty()) {
        // I want to render it relative to the center of m_Position
        int xCoord = static_cast<int>(m_Position.x - XSIZE / 2);
        //int yCoord = static_cast<int>(m_Position.y - YSIZE - 1);
        int yCoord = 0;
        int zCoord = static_cast<int>(m_Position.z - ZSIZE / 2);
        glm::vec3 center(xCoord, yCoord, zCoord);
//        std::multimap<float, std::vector<Vertex>, std::greater<>> transparentMeshMap;

        for (unsigned int i = 1; i < XSIZE - 1; i++) {
            for (unsigned int k = 1; k < ZSIZE - 1; k++) {
                for (unsigned int j = 1; j < YSIZE - 1; j++) {
                    if (m_Chunk(i, j, k) != Block::EMPTY) {
                        std::array<float, 24> textureCoords = s_TextureMap.at(m_Chunk(i, j, k));
                        if (m_Chunk(i,j,k) != Block::WATER) {
                            if (j > 0 && (m_Chunk(i, j - 1, k) == Block::EMPTY || m_Chunk(i, j - 1, k) == Block::WATER)) { // D
                                CreateDQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (j < YSIZE - 1 && (m_Chunk(i, j + 1, k) == Block::EMPTY || m_Chunk(i, j + 1, k) == Block::WATER)) { // U
                                CreateUQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (k > 0 && (m_Chunk(i, j, k - 1) == Block::EMPTY || m_Chunk(i, j, k - 1) == Block::WATER)) { // B
                                CreateBQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (k < ZSIZE - 1 && (m_Chunk(i, j, k + 1) == Block::EMPTY || m_Chunk(i, j, k + 1) == Block::WATER)) { // F
                                CreateFQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (i > 0 && (m_Chunk(i - 1, j, k) == Block::EMPTY || m_Chunk(i - 1, j, k) == Block::WATER)) { // L
                                CreateLQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (i < XSIZE - 1 && (m_Chunk(i + 1, j, k) == Block::EMPTY || m_Chunk(i + 1, j, k) == Block::WATER)) { // R
                                CreateRQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                        } else { // add to transparent buffer
//                            std::vector<Vertex> mesh;
//                            mesh.reserve(4);
                            if (j > 0 && m_Chunk(i, j - 1, k) == Block::EMPTY) { // D
                                CreateDQuad(m_TransparentMesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (j < YSIZE - 1 && m_Chunk(i, j + 1, k) == Block::EMPTY) { // U
                                CreateUQuad(m_TransparentMesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (k > 0 && m_Chunk(i, j, k - 1) == Block::EMPTY) { // B
                                CreateBQuad(m_TransparentMesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (k < ZSIZE - 1 && m_Chunk(i, j, k + 1) == Block::EMPTY) { // F
                                CreateFQuad(m_TransparentMesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (i > 0 && m_Chunk(i - 1, j, k) == Block::EMPTY) { // L
                                CreateLQuad(m_TransparentMesh, center + glm::vec3(i, j, k), textureCoords);
                            }
                            if (i < XSIZE - 1 && m_Chunk(i + 1, j, k) == Block::EMPTY) { // R
                                CreateRQuad(m_TransparentMesh, center + glm::vec3(i, j, k), textureCoords);
                            }
//                            float distance = glm::length2(*m_PlayerPosition -
//                                    glm::vec3((float)i + m_Position.x, (float)j + m_Position.y, (float)k + m_Position.z));
//                            transparentMeshMap.insert({ distance, std::move(mesh) });
                        }
                    }
                }
            }
        }
        // solid mesh
        size_t indexCount = m_Mesh.size() / 4 * 6; // num faces * 6
        m_VBO->SendData(m_Mesh.size() * sizeof(Vertex), m_Mesh.data());
        m_IBO->SetCount(indexCount);
        // transparent mesh
//        if (!transparentMeshMap.empty()) {
            //m_TransparentMesh = GenerateTransVector(transparentMeshMap);
        if (!m_TransparentMesh.empty()) {
            indexCount = m_TransparentMesh.size() / 4 * 6;
            m_TransparentVBO->SendData(m_TransparentMesh.size() * sizeof(Vertex), m_TransparentMesh.data());
            m_TransparentIBO->SetCount(indexCount);
        }
    }
}

// can be called with multithreading if slow
void Chunk::UpdateMesh(unsigned int x, unsigned int y, unsigned int z, Block block)
{
    // take care of border (I think, maybe I also have to do for y or not do this at all)
    x += 1;
    z += 1;
    // check the blocks surrounding the modified block and figure out a way to edit the m_Mesh accordingly
    for (unsigned int i = x - 1; i <= x + 1; i++) {
        for (unsigned int k = z - 1; k <= z + 1; k++) {
            for (unsigned int j = y - 1; j <= y + 1; j++) {
                // to figure out
            }
        }
    }
}

void Chunk::Render(const Renderer& renderer)
{
    renderer.Draw(*m_VAO, *m_IBO, GL_UNSIGNED_INT);
    if (!m_TransparentMesh.empty())
        renderer.Draw(*m_TransparentVAO, *m_TransparentIBO, GL_UNSIGNED_INT);
}

Matrix3D<Block, XSIZE, YSIZE, ZSIZE> Chunk::GetMatrix() const
{
    return m_Chunk;
}

void Chunk::SetMatrix(unsigned int x, unsigned int y, unsigned int z, Block block)
{
    m_Chunk(x, y, z) = block;
    m_Mesh.clear();
    m_TransparentMesh.clear();
    //UpdateMesh(x,y,z,block);
}

glm::vec3 Chunk::GetPosition() const {
    return m_Position;
}


