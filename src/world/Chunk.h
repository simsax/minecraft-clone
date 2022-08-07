#pragma once

#include "../graphics/IndexBuffer.h"
#include "../utils/Matrix3D.hpp"
#include "../utils/Noise.h"
#include "../graphics/Renderer.h"
#include "../graphics/VertexArray.h"
#include "../graphics/VertexBuffer.h"
#include "../graphics/VertexBufferLayout.hpp"
#include "glm/glm.hpp"
#include "GL/glew.h"
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#define XSIZE 16
#define YSIZE 256
#define ZSIZE 16

enum class Block : uint8_t {
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
    std::size_t operator()(const ChunkCoord &coord) const;
};

bool operator==(const ChunkCoord &l, const ChunkCoord &r);

bool operator!=(const ChunkCoord &l, const ChunkCoord &r);

int operator-(const ChunkCoord &l, const ChunkCoord &r);

class Chunk {
public:
    Chunk(glm::vec3 position, const VertexBufferLayout &layout, uint32_t maxVertexCount,
          const std::vector<uint32_t> &indices, glm::vec3 *playerPosition);

    void GenerateMesh(IndexBuffer *ibo, IndexBuffer *t_ibo);

    Block GetBlock(uint32_t x, uint32_t y, uint32_t z) const;

    void SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block);

    void Render(const Renderer &renderer, VertexArray *vao, VertexArray *t_vao, IndexBuffer *ibo,
                IndexBuffer *t_ibo);

    void RenderOutline(Renderer &renderer, VertexArray *vao, VertexBuffer *vbo,
                       IndexBuffer *ibo, const glm::vec3 &target,
                       std::vector<uint32_t> &outlineMesh);

    glm::vec3 GetPosition() const;

    glm::vec3 GetCenterPosition() const;

private:
    static const std::unordered_map<Block, std::array<uint8_t, 6>> s_TextureMap;

    /* void UpdateMesh(uint32_t x, uint32_t y, uint32_t z, Block block); */
    void CreateHeightMap();

    void FastFill();

    void CreateSurfaceLayer();

    float Continentalness(int x, int y);

    void GenSolidCube(int i, int j, int k, std::vector<uint32_t> &target,
                      const std::array<uint8_t, 6> &textureCoords);

    void GenWaterCube(int i, int j, int k, std::vector<uint32_t> &target,
                      const std::array<uint8_t, 6> &textureCoords);

//    std::unique_ptr<VertexArray> m_VAO;
//    std::unique_ptr<VertexArray> m_TransparentVAO;
//    std::unique_ptr<IndexBuffer> m_IBO;
//    std::unique_ptr<IndexBuffer> m_TransparentIBO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexBuffer> m_TransparentVBO;

    std::array<int, XSIZE * ZSIZE> m_HeightMap;
    Noise m_Noise;
    glm::vec3 m_ChunkPosition;
    Matrix3D<Block, XSIZE, YSIZE, ZSIZE> m_Chunk;
    std::vector<uint32_t> m_Mesh;
    std::vector<uint32_t> m_TransparentMesh;
    uint32_t m_MaxVertexCount;
    glm::vec3 *m_PlayerPosition;
    int m_MinHeight;
    int m_MaxHeight;
    VertexBufferLayout m_Layout;
    size_t m_IBOCount;
    size_t m_TIBOCount;
};
