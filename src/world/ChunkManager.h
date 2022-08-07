#pragma once
#include "../camera/Camera.h"
#include "Chunk.h"
#include <future>
#include <queue>

class ChunkManager {
public:
    ChunkManager(Camera* camera);
    ~ChunkManager();
    void InitWorld();
    int SpawnHeight();
    void GenerateChunks();
    ChunkCoord CalculateChunkCoord(const glm::vec3& position);
    // void UpdateChunksToRender();
    void Render(Renderer& renderer);
    void UpdateChunk(ChunkCoord chunk);
    int GetViewDistance() const;
    void SetNewChunks();
    std::array<uint32_t, 3> GetChunkSize() const;
    void SetSelectedBlock(const std::pair<ChunkCoord, glm::vec3>& target);
    void ClearSelectedBlock();
    std::unordered_map<ChunkCoord, Chunk, hash_fn> m_ChunkMap;

private:
    void SortChunks();
    void LoadChunks();
    std::array<uint32_t, 3> m_ChunkSize;
    std::future<std::vector<std::pair<ChunkCoord, Chunk>>> m_FutureChunks;
    VertexBufferLayout m_VertexLayout;
    VertexArray* m_VAO;
    VertexArray* m_TransparentVAO;
    IndexBuffer* m_IBO;
    IndexBuffer* m_TransparentIBO;
    std::vector<uint32_t> m_Indices;
    std::vector<Chunk*> m_ChunksToRender;
    std::queue<ChunkCoord> m_ChunksToLoad;
    std::queue<ChunkCoord> m_ChunksToUpload;
    int m_ViewDistance;
    // std::vector<std::future<Chunk>> m_ChunksLoaded;
    // std::thread m_Thread;
    // std::mutex m_Mtx;
    // std::condition_variable m_Cv;
    bool m_Shutdown;
    bool m_LoadingChunks;
    Camera* m_Camera;
    bool m_NewChunks;
    float m_HalfChunkDiag;
    std::pair<ChunkCoord, glm::vec3> m_SelectedBlock;
    IndexBuffer* m_OutlineIBO;
    VertexBuffer* m_OutlineVBO;
    VertexArray* m_OutlineVAO;
    std::vector<uint32_t> m_OutlineMesh;
    bool m_Selected;
};
