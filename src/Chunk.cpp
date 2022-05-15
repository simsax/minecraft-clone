#include "Chunk.h"
#include <iostream>
#include <cstdlib>
#include <map>
#include "Camera.h"
#include <glm/gtx/norm.hpp>

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

static std::array<unsigned int, 4> GenerateTextCoords(const std::array<unsigned char, 2>& textureCoords) {
    unsigned int t0 = (textureCoords[0] << 4 | textureCoords[1]) << 2;
    unsigned int t1 = t0 | 1;
    unsigned int t2 = t0 | 2;
    unsigned int t3 = t0 | 3;
    return { t0, t1, t2, t3 };
}

static void PushVertexPos(std::vector<unsigned int>& target, const std::array<unsigned int, 4>& v) {
    target.emplace_back(v[0]);
    target.emplace_back(v[1]);
    target.emplace_back(v[2]);
    target.emplace_back(v[3]);
}

static void CreateUQuad(std::vector<unsigned int>& target, const glm::vec3& position,
                        const std::array<unsigned char, 6>& textureCoords) {
    std::array<unsigned int, 4> t = GenerateTextCoords({ textureCoords[0], textureCoords[1] });
    
    unsigned int v0 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 1) << 11 | t[0];

    unsigned int v1 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 1) << 11 | t[1];

    unsigned int v2 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 0) << 11 | t[2];

    unsigned int v3 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 0) << 11 | t[3];

    PushVertexPos(target, { v0, v1, v2, v3 });
}

static void CreateDQuad(std::vector<unsigned int>& target, const glm::vec3& position,
                        const std::array<unsigned char, 6>& textureCoords) {
    std::array<unsigned int, 4> t = GenerateTextCoords({ textureCoords[4], textureCoords[5] });

    unsigned int v0 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 1) << 11 | t[0];

    unsigned int v1 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 0) << 11 | t[1];

    unsigned int v2 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 0) << 11 | t[2];

    unsigned int v3 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 1) << 11 | t[3];

    PushVertexPos(target, { v0, v1, v2, v3 });
}

static void CreateFQuad(std::vector<unsigned int>& target, const glm::vec3& position,
                        const std::array<unsigned char, 6>& textureCoords) {
    std::array<unsigned int, 4> t = GenerateTextCoords({ textureCoords[2], textureCoords[3] });

    unsigned int v0 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 1) << 11 | t[0];

    unsigned int v1 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 1) << 11 | t[1];

    unsigned int v2 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 1) << 11 | t[2];

    unsigned int v3 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 1) << 11 | t[3];

    PushVertexPos(target, { v0, v1, v2, v3 });
}

static void CreateBQuad(std::vector<unsigned int>& target, const glm::vec3& position,
                        const std::array<unsigned char, 6>& textureCoords) {
    std::array<unsigned int, 4> t = GenerateTextCoords({ textureCoords[2], textureCoords[3] });
    
    unsigned int v0 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 0) << 11 | t[0];

    unsigned int v1 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 0) << 11 | t[1];

    unsigned int v2 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 0) << 11 | t[2];

    unsigned int v3 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 0) << 11 | t[3];

    PushVertexPos(target, { v0, v1, v2, v3 });
}

static void CreateRQuad(std::vector<unsigned int>& target, const glm::vec3& position,
                        const std::array<unsigned char, 6>& textureCoords) {
    std::array<unsigned int, 4> t = GenerateTextCoords({ textureCoords[2], textureCoords[3] });

    unsigned int v0 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 1) << 11 | t[0];

    unsigned int v1 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 0) << 11 | t[1];

    unsigned int v2 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 0) << 11 | t[2];

    unsigned int v3 = ((int)position[0] + 1) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 1) << 11 | t[3];

    PushVertexPos(target, { v0, v1, v2, v3 });
}

static void CreateLQuad(std::vector<unsigned int>& target, const glm::vec3& position,
                        const std::array<unsigned char, 6>& textureCoords) {
    std::array<unsigned int, 4> t = GenerateTextCoords({ textureCoords[2], textureCoords[3] });

    unsigned int v0 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 0) << 11 | t[0];

    unsigned int v1 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 0) << 15 |
                      ((int)position[2] + 1) << 11 | t[1];

    unsigned int v2 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 1) << 11 | t[2];

    unsigned int v3 = ((int)position[0] + 0) << 23 |
                      ((int)position[1] + 1) << 15 |
                      ((int)position[2] + 0) << 11 | t[3];

    PushVertexPos(target, { v0, v1, v2, v3 });
}

// the chunk has a border so that I know what faces to cull between chunks
// (I only generate the mesh of the part inside the border)
Chunk::Chunk(glm::vec3 position,
             const VertexBufferLayout& layout, unsigned int maxVertexCount, const std::vector<unsigned int>& indices,
             glm::vec3* playerPosition) :
        m_ChunkPosition(position), m_Chunk(Matrix3D<Block, XSIZE, YSIZE, ZSIZE>()), m_MaxVertexCount(maxVertexCount),
        m_PlayerPosition(playerPosition), m_MinHeight(YSIZE), m_MaxHeight(0)
{

    m_IBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
    m_TransparentIBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
    m_VBO = std::make_unique<VertexBuffer>();
    m_TransparentVBO = std::make_unique<VertexBuffer>();
    m_VBO->CreateDynamic(sizeof(unsigned int) * maxVertexCount);
    m_TransparentVBO->CreateDynamic(sizeof(unsigned int) * maxVertexCount);
    m_VAO = std::make_unique<VertexArray>();
    m_TransparentVAO = std::make_unique<VertexArray>();
    m_VAO->AddBuffer(*m_VBO, layout);
    m_TransparentVAO->AddBuffer(*m_TransparentVBO, layout);

    m_Mesh.reserve(m_MaxVertexCount);
    CreateHeightMap();
    FastFill();
    CreateSurfaceLayer();
}

float Chunk::Continentalness(int x, int y) {
    float scale = 256.0f;
    float height = 0;

    float noise = m_Noise.OctaveNoise(x, y, 8, 0.01f);

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

void Chunk::CreateHeightMap() {
    int elevation = 50; // increase for less water
    int index = 0;

    for (int i = 0; i < XSIZE; i++) {
        for (int k = 0; k < ZSIZE; k++) {
            float a = (m_Noise.OctaveNoise(i + m_ChunkPosition.x, k + m_ChunkPosition.z, 6, 0.002f) + 1) / 2;
            float m = (m_Noise.OctaveNoise(i + m_ChunkPosition.x + 123, k + m_ChunkPosition.z + 456, 8, 0.01f) + 1) / 2;
            a *= a;
            float terrain_height;
            if (a < 0.5)
                terrain_height = m * a * a * 2;
            else
                terrain_height = m * (1 - (1-a) * (1-a) * 2);
            terrain_height *= 200.0f;
            terrain_height = static_cast<int>(terrain_height + elevation);
            if (terrain_height < 40)
                terrain_height = 40;
            int height = static_cast<int>(m_Noise.OctaveNoise(i + m_ChunkPosition.x, k + m_ChunkPosition.z, 8) * 5.0f);

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
    int level_size = XSIZE * ZSIZE;
    memset(m_Chunk.GetRawPtr(), (int)Block::BEDROCK, level_size);
    memset(m_Chunk.GetRawPtr() + level_size, (int)Block::STONE, level_size * (m_MinHeight - 1));
    memset(m_Chunk.GetRawPtr() + level_size * (m_MaxHeight + 1),
             (int)Block::EMPTY, level_size * (YSIZE - m_MaxHeight - 1));
}

void Chunk::CreateSurfaceLayer() {
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
                    int dirtThickness = static_cast<int>(
                            (m_Noise.OctaveNoise(i + m_ChunkPosition.x + 111, k + m_ChunkPosition.z + 111, 8) + 1)/2 * 10) - j;
                    if (dirtThickness > 0)
                        m_Chunk(i, j, k) = Block::DIRT;
                    else
                        m_Chunk(i, j, k) = Block::STONE;
                }
                else if (j == height) {
                    float noise_chance = m_Noise.OctaveNoise(i + m_ChunkPosition.x, k + m_ChunkPosition.z, 8);
                    if (j == water_level && noise_chance >= 0) 
                        m_Chunk(i, j, k) = Block::SAND;
                    else if (j < water_level) {
                        if (noise_chance >= 0.2)
                            m_Chunk(i, j, k) = Block::GRAVEL;
                        else
                            m_Chunk(i, j, k) = Block::SAND;
                    }
                    else if (j >= snow_level)
                        m_Chunk(i, j, k) = Block::SNOW;
                    else if (j >= snow_level - 10 && noise_chance >= -0.2)
                        m_Chunk(i, j, k) = Block::SNOWY_GRASS;
                    else
                        m_Chunk(i, j, k) = Block::GRASS;
                }
                else {
                    if (j < water_level)
                        m_Chunk(i, j, k) = Block::WATER;
                    else
                        m_Chunk(i, j, k) = Block::EMPTY;
                }
            }
        }
    }
}

void Chunk::GenerateMesh() {
    if (m_Mesh.empty() && m_TransparentMesh.empty()) {
        if (m_MinHeight < 1)
            m_MinHeight = 1;
        for (int j = m_MinHeight - 1; j <= m_MaxHeight; j++) {
            for (int i = 1; i < XSIZE - 1; i++) {
                for (int k = 1; k < ZSIZE - 1; k++) {
                    if (m_Chunk(i, j, k) != Block::EMPTY) {
                        std::array<unsigned char, 6> textureCoords = s_TextureMap.at(m_Chunk(i, j, k));
                        if (m_Chunk(i,j,k) != Block::WATER) {
                            if (j > 0 && 
                                (m_Chunk(i, j - 1, k) == Block::EMPTY || m_Chunk(i, j - 1, k) == Block::WATER)) { // D
                                CreateDQuad(m_Mesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (j < YSIZE - 1 && 
                                (m_Chunk(i, j + 1, k) == Block::EMPTY || m_Chunk(i, j + 1, k) == Block::WATER)) { // U
                                CreateUQuad(m_Mesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (k > 0 && 
                                (m_Chunk(i, j, k - 1) == Block::EMPTY || m_Chunk(i, j, k - 1) == Block::WATER)) { // B
                                CreateBQuad(m_Mesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (k < ZSIZE - 1 && 
                                (m_Chunk(i, j, k + 1) == Block::EMPTY || m_Chunk(i, j, k + 1) == Block::WATER)) { // F
                                CreateFQuad(m_Mesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (i > 0 && 
                                (m_Chunk(i - 1, j, k) == Block::EMPTY || m_Chunk(i - 1, j, k) == Block::WATER)) { // L
                                CreateLQuad(m_Mesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (i < XSIZE - 1 && 
                                (m_Chunk(i + 1, j, k) == Block::EMPTY || m_Chunk(i + 1, j, k) == Block::WATER)) { // R
                                CreateRQuad(m_Mesh, glm::vec3(i, j, k), textureCoords);
                            }
                        } else { // add to transparent buffer
                            if (j > 0 && m_Chunk(i, j - 1, k) == Block::EMPTY) { // D
                                CreateDQuad(m_TransparentMesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (j < YSIZE - 1 && m_Chunk(i, j + 1, k) == Block::EMPTY) { // U
                                CreateUQuad(m_TransparentMesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (k > 0 && m_Chunk(i, j, k - 1) == Block::EMPTY) { // B
                                CreateBQuad(m_TransparentMesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (k < ZSIZE - 1 && m_Chunk(i, j, k + 1) == Block::EMPTY) { // F
                                CreateFQuad(m_TransparentMesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (i > 0 && m_Chunk(i - 1, j, k) == Block::EMPTY) { // L
                                CreateLQuad(m_TransparentMesh, glm::vec3(i, j, k), textureCoords);
                            }
                            if (i < XSIZE - 1 && m_Chunk(i + 1, j, k) == Block::EMPTY) { // R
                                CreateRQuad(m_TransparentMesh, glm::vec3(i, j, k), textureCoords);
                            }
                        }
                    }
                }
            }
        }
        // solid mesh
        size_t indexCount = m_Mesh.size() / 4 * 6; // num faces * 6
        m_VBO->SendData(m_Mesh.size() * sizeof(unsigned int), m_Mesh.data());
        m_IBO->SetCount(indexCount);
        // transparent mesh
        if (!m_TransparentMesh.empty()) {
            indexCount = m_TransparentMesh.size() / 4 * 6;
            m_TransparentVBO->SendData(m_TransparentMesh.size() * sizeof(unsigned int), m_TransparentMesh.data());
            m_TransparentIBO->SetCount(indexCount);
        }
    }
}

/* void Chunk::UpdateMesh(unsigned int x, unsigned int y, unsigned int z, Block block) */
/* { */
/*     // take care of border (I think, maybe I also have to do for y or not do this at all) */
/*     x += 1; */
/*     z += 1; */
/*     // check the blocks surrounding the modified block and figure out a way to edit the m_Mesh accordingly */
/*     for (unsigned int i = x - 1; i <= x + 1; i++) { */
/*         for (unsigned int k = z - 1; k <= z + 1; k++) { */
/*             for (unsigned int j = y - 1; j <= y + 1; j++) { */
/*                 // to figure out */
/*             } */
/*         } */
/*     } */
/* } */

void Chunk::Render(const Renderer& renderer)
{
    renderer.Draw(*m_VAO, *m_IBO, GL_UNSIGNED_INT, m_ChunkPosition);
    if (!m_TransparentMesh.empty())
        renderer.Draw(*m_TransparentVAO, *m_TransparentIBO, GL_UNSIGNED_INT, m_ChunkPosition);
}

Matrix3D<Block, XSIZE, YSIZE, ZSIZE> Chunk::GetMatrix() const
{
    return m_Chunk;
}

void Chunk::SetMatrix(unsigned int x, unsigned int y, unsigned int z, Block block)
{
    m_Chunk(x, y, z) = block;
    if (y < m_MinHeight)
        m_MinHeight = y;
    else if (y > m_MaxHeight)
        m_MaxHeight = y;
    m_Mesh.clear();
    m_TransparentMesh.clear();
    //UpdateMesh(x,y,z,block);
}

glm::vec3 Chunk::GetPosition() const {
    return m_ChunkPosition;
}

glm::vec3 Chunk::GetCenterPosition() const {
    return glm::vec3(m_ChunkPosition.x + XSIZE/2, m_ChunkPosition.y, m_ChunkPosition.z + ZSIZE/2);
}

const std::unordered_map<Block, std::array<unsigned char, 6>> Chunk::s_TextureMap =
    {                           // top, side, bottom
        { Block::GRASS,         { 12, 3, 3, 15, 2, 15 }},
        { Block::DIRT,          { 2, 15, 2, 15, 2, 15 }},
        { Block::STONE,         { 1, 15, 1, 15, 1, 15 }},
        { Block::DIAMOND,       { 2, 12, 2, 12, 2, 12 }},
        { Block::GOLD,          { 0, 13, 0, 13, 0, 13 }},
        { Block::COAL,          { 2, 13, 2, 13, 2, 13 }},
        { Block::IRON,          { 1, 13, 1, 13, 1, 13 }}, 
        { Block::LEAVES,        { 11, 1, 11, 1, 11, 1 }},
        { Block::WOOD,          { 5, 14, 4, 14, 5, 14 }},
        { Block::SNOW,          { 2, 11, 2, 11, 2, 11 }},
        { Block::SNOWY_GRASS,   { 2, 11, 4, 11, 2, 15 }},
        { Block::WATER,	        { 15, 3, 15, 3, 15, 3 }},
        { Block::SAND,	        { 2, 14, 2, 14, 2, 14 }},
        { Block::GRAVEL,	    { 3, 14, 3, 14, 3, 14 }},
        { Block::BEDROCK,       { 1, 14, 1, 14, 1, 14 }}    
    };