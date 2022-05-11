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
#include <array>
#include "Matrix3D.hpp"
#include "Noise.h"

#define XSIZE 18
#define YSIZE 256
#define ZSIZE 18

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
    SAND,
    GRAVEL,
    SNOWY_GRASS,
    BEDROCK
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
    Vertex(glm::vec3 position, glm::vec2 texCoords) :
        Position(position), TexCoords(texCoords) {}

    glm::vec3 Position;
    glm::vec2 TexCoords;
};

class Chunk
{
public:
    Chunk(glm::vec3 position, const VertexBufferLayout& layout,
          unsigned int maxVertexCount, const std::vector<unsigned int>& indices, glm::vec3* playerPosition);
    Matrix3D<Block, XSIZE, YSIZE, ZSIZE> GetMatrix() const;
    void GenerateMesh();
    void SetMatrix(unsigned int x, unsigned int y, unsigned int z, Block block);
    void Render(const Renderer& renderer);
    glm::vec3 GetPosition() const;
private:
    static const std::unordered_map<Block, std::array<float, 24>> s_TextureMap;
    void UpdateMesh(unsigned int x, unsigned int y, unsigned int z, Block block);
    void CreateHeightMap();
    void FastFill();
    void CreateSurfaceLayer();
    float Continentalness(int x, int y);

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexArray> m_TransparentVAO;
    std::unique_ptr<IndexBuffer> m_IBO;
    std::unique_ptr<IndexBuffer> m_TransparentIBO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexBuffer> m_TransparentVBO;

    std::array<int, XSIZE * ZSIZE> m_HeightMap;
    Noise m_Noise;
    glm::vec3 m_Position;
    Matrix3D<Block, XSIZE, YSIZE, ZSIZE> m_Chunk;
    std::vector<Vertex> m_Mesh;
    std::vector<Vertex> m_TransparentMesh;
    unsigned int m_MaxVertexCount;
    glm::vec3* m_PlayerPosition;
    int m_MinHeight;
    int m_MaxHeight;
};
