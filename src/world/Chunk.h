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
#include "../utils/Items.h"
#include "../graphics/TextureAtlas.h"
#include "../graphics/ChunkRenderer.h"

#define XSIZE 16
#define YSIZE 256
#define ZSIZE 16

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

using BlockVec = std::vector<std::pair<Block, glm::vec3>>;

class Chunk {
public:
    using ChunkMap = std::unordered_map<ChunkCoord, Chunk, hash_fn>;

    Chunk(const ChunkCoord &coords, const glm::vec3 &position, uint32_t stride, ChunkMap *chunkMap);
    Chunk(const Chunk &) = delete;
    Chunk &operator=(const Chunk &) = delete;
    Chunk(Chunk &&other) = default;
    Chunk &operator=(Chunk &&other) = default;

    bool GenerateMesh();
    Block GetBlock(uint32_t x, uint32_t y, uint32_t z) const;
    void SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block);
    void Render(ChunkRenderer& renderer, const ChunkCoord& playerChunk, int radius);
    void RenderOutline(ChunkRenderer &renderer, const glm::uvec3 &target);
    glm::vec3 GetCenterPosition() const;
    BlockVec CreateSurfaceLayer(const BlockVec &blocksToSet);
    ChunkCoord GetCoord() const;
    bool ChunkIsVisible(ChunkCoord playerChunk, int radius) const;
    void ClearMesh();
    void UpdateMeshHeighLimit(uint32_t height);

private:
    void CreateHeightMap();
    void FastFill();

    template<typename... Args>
    void GenCube(int i, int j, int k, std::vector<uint32_t> &target,
                 const TextureAtlas::Coords &textureCoords,
                 const std::array<Chunk *, 4> &neighbors,
                 Args... voidBlocks);

    void SetBlocks(const BlockVec &blocksToSet);
    void CreateTrees(int i, int j, int k, BlockVec &blockVec);
    void CreateSprites(int i, int j, int k, BlockVec &blockVec);
    bool FindNeighbors(std::array<Chunk *, 4> &neighbors);

    template<typename... Args>
    bool CheckNeighbor(const Chunk *chunk, const glm::uvec3 &position, Args... voidBlocks);

    VertexBuffer m_VBO;
    std::array<int, XSIZE * ZSIZE> m_HeightMap;
    Noise m_Noise;
    ChunkCoord m_Coords;
    glm::vec3 m_ChunkPosition;
    Matrix3D<Block, XSIZE, YSIZE, ZSIZE> m_Chunk;
    std::vector<uint32_t> m_Mesh;
    std::vector<uint32_t> m_TransparentMesh;
    std::vector<uint32_t> m_SpriteMesh;
    int m_MinHeight;
    int m_MaxHeight;
    size_t m_IBOCount;
    size_t m_TIBOCount;
    size_t m_SIBOCount;
    ChunkMap *m_ChunkMap;
    glm::vec3 m_CenterPosition;
};
