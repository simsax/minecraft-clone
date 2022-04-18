#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include <memory>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.h"
#include "Renderer.h"
#include <unordered_map>
#include <array>
#include <vector>
#include "Matrix.hpp"

enum class Block : unsigned char {
    EMPTY,
    GRASS,
    DIRT,
    STONE,
    SNOW,
    DIAMOND,
    GOLD,
    COAL,
    STEEL,
    LEAVES,
    WOOD,
    WATER,
    SAND
};

struct ChunkCoord {
    int x;
    int z;
};

struct hash_fn {
    std::size_t operator()(const ChunkCoord& coord) const;
};

bool operator==(const ChunkCoord& l, const ChunkCoord& r);
bool operator!=(const ChunkCoord& l, const ChunkCoord& r);
int operator-(const ChunkCoord& l, const ChunkCoord& r);

struct Vertex {
    Vertex(glm::vec3 position, glm::vec2 texCoords) : Position(std::move(position)), TexCoords(std::move(texCoords)) {}

    glm::vec3 Position;
    glm::vec2 TexCoords;
};

class Chunk
{
public:
    Chunk(unsigned int xLength, unsigned int yLength, unsigned int zLength, glm::vec3 position, unsigned int seed, const VertexBufferLayout& layout,
          unsigned int maxVertexCount, const std::vector<unsigned int>& indices);
    Matrix<Block> GetMatrix() const;
    void GenerateMesh();
    void SetMatrix(unsigned int x, unsigned int y, unsigned int z, Block block);
    void Render(const Renderer& renderer);
private:
    static const std::unordered_map<Block, std::array<float, 24>> s_TextureMap;
    void UpdateMesh(unsigned int x, unsigned int y, unsigned int z, Block block);
    void SinInit();
    void Noise2DInit();
    void Noise3DInit(unsigned int seed);

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<IndexBuffer> m_IBO;
    std::unique_ptr<VertexBuffer> m_VBO;

    static const float s_TextureOffset; // depends on the texture atlas
    unsigned int m_XLength;
    unsigned int m_YLength;
    unsigned int m_ZLength;
    glm::vec3 m_Position;
    Matrix<Block> m_Chunk;
    ChunkCoord m_WorldCoords;
    std::vector<Vertex> m_Mesh;
    unsigned int m_Seed;
    unsigned int m_MaxVertexCount;
};
