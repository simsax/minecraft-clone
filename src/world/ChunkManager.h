#pragma once

#include "../camera/Camera.h"
#include "Chunk.h"
#include "../utils/Physics.h"
#include <future>
#include <queue>

class ChunkManager {
public:
    explicit ChunkManager(Camera *camera);

    ~ChunkManager();

    void InitWorld();

    int SpawnHeight();

    void GenerateChunks();

    ChunkCoord CalculateChunkCoord(const glm::vec3 &position);

    // void UpdateChunksToRender();
    void Render(Renderer &renderer);

    void UpdateChunk(ChunkCoord chunk);

    int GetViewDistance() const;

    void SetNewChunks();

    std::array<uint32_t, 3> GetChunkSize() const;

    bool IsVoxelSolid(const glm::vec3 &voxel);

    void DestroyBlock();
    void PlaceBlock(Block block);
    bool CalculateCollision(const glm::vec3 &playerSpeed);

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
    std::pair<ChunkCoord, glm::uvec3> GlobalToLocal(const glm::vec3 &playerPosition);
    void UpdateNeighbors(glm::vec3 currentVoxel, ChunkCoord targetLocalCoord, Block block);

    std::array<uint32_t, 3> m_ChunkSize;
    // std::future<std::vector<std::pair<ChunkCoord, Chunk>>> m_FutureChunks;
    std::unordered_map<ChunkCoord, Chunk, hash_fn> m_ChunkMap;
    VertexBufferLayout m_VertexLayout;
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    VertexBuffer m_OutlineVBO;
    std::vector<uint32_t> m_Indices;
    std::vector<Chunk *> m_ChunksToRender;
    std::queue<ChunkCoord> m_ChunksToLoad;
    std::queue<ChunkCoord> m_ChunksToUpload;
    int m_ViewDistance;
    // std::vector<std::future<Chunk>> m_ChunksLoaded;
    // std::thread m_Thread;
    // std::mutex m_Mtx;
    // std::condition_variable m_Cv;
    bool m_Shutdown;
    bool m_LoadingChunks;
    Camera *m_Camera;
    bool m_NewChunks;
    std::pair<ChunkCoord, glm::vec3> m_SelectedBlock;
    int m_BindingIndex;
    Raycast m_Raycast;

};
