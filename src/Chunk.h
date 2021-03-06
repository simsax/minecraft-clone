#pragma once

#include "IndexBuffer.h"
#include "Matrix3D.hpp"
#include "Noise.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.hpp"
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#define XSIZE 16
#define YSIZE 256
#define ZSIZE 16

enum class Block : unsigned char {
    EMPTY,
    GRASS,
    DIRT,
    STONE,
    SNOW,
    DIAMOND,
    GOLD,
    COAL,
    IRON,
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

class Chunk {
public:
    Chunk(glm::vec3 position, const VertexBufferLayout& layout, unsigned int maxVertexCount,
        const std::vector<unsigned int>& indices, glm::vec3* playerPosition);
    Matrix3D<Block, XSIZE, YSIZE, ZSIZE> GetMatrix() const;
    void GenerateMesh();
    void SetMatrix(unsigned int x, unsigned int y, unsigned int z, Block block);
    void Render(const Renderer& renderer);
    glm::vec3 GetPosition() const;
    glm::vec3 GetCenterPosition() const;

private:
    static const std::unordered_map<Block, std::array<unsigned char, 6>> s_TextureMap;
    /* void UpdateMesh(unsigned int x, unsigned int y, unsigned int z, Block block); */
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
    glm::vec3 m_ChunkPosition;
    Matrix3D<Block, XSIZE, YSIZE, ZSIZE> m_Chunk;
    std::vector<unsigned int> m_Mesh;
    std::vector<unsigned int> m_TransparentMesh;
    unsigned int m_MaxVertexCount;
    glm::vec3* m_PlayerPosition;
    int m_MinHeight;
    int m_MaxHeight;
};
