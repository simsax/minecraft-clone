#pragma once

#include "../camera/Camera.h"
#include "Chunk.h"
#include "../utils/Physics.h"
#include "../sky/Sun.h"
#include "../graphics/ChunkRenderer.h"
#include <future>
#include <queue>
#include <unordered_set>
#include <optional>

class ChunkManager {
public:
    explicit ChunkManager(Camera *camera);
    ChunkManager(const ChunkManager &) = delete;
    ChunkManager &operator=(const ChunkManager &) = delete;

    void InitWorld(uint32_t stride);
    ChunkCoord CalculateChunkCoord(const glm::vec3 &position) const;
    void Render(ChunkRenderer &renderer);
    glm::vec3 GetChunkSize() const;
    bool IsBlockSolid(const glm::vec3& globalCoords) const;
    bool IsBlockCastable(const glm::vec3& voxel);
    void DestroyBlock();
    void PlaceBlock(Block block);
    void UpdateChunks();

private:
    struct Raycast {
        Chunk *chunk;
        ChunkCoord chunkCoord;
        glm::uvec3 localVoxel;
        glm::vec3 globalVoxel;
        Chunk *prevChunk;
        ChunkCoord prevChunkCoord;
        glm::uvec3 prevLocalVoxel;
        glm::vec3 prevGlobalVoxel;
        bool selected;
    };

    void SortChunks();
    void LoadChunks();
    void MeshChunks();
    std::pair<ChunkCoord, glm::uvec3> GlobalToLocal(const glm::vec3 &playerPosition) const;
    void UpdateNeighbors(const glm::uvec3 &voxel, const ChunkCoord &chunkCoord);
    void AddBlocks(const ChunkCoord &chunkCoord, BlockVec &blockVec);
    void GenerateChunks();
    void LightPlacedBFS(uint8_t i, uint8_t j, uint8_t k, Chunk* start);

    glm::vec3 m_ChunkSize;
    std::unordered_map<ChunkCoord, Chunk, hash_fn> m_ChunkMap;
    std::vector<Chunk *> m_ChunksToRender;
    std::queue<Chunk *> m_ChunksToMesh;
    std::queue<Chunk *> m_ChunksInBorder;
    std::queue<ChunkCoord> m_ChunksToLoad;
    std::unordered_set<ChunkCoord, hash_fn> m_ChunksToUpload;
    std::unordered_map<ChunkCoord, BlockVec, hash_fn> m_BlocksToSet;
    int m_ViewDistance;
    Camera *m_Camera;
    bool m_SortChunks;
    bool m_ChunksReadyToMesh;
    std::pair<ChunkCoord, glm::vec3> m_SelectedBlock;
    Raycast m_Raycast;
    ChunkCoord m_LastChunk;
    ChunkCoord m_CurrentChunk;
    uint32_t m_Stride;
};
