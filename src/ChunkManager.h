#pragma once
#include "Chunk.h"
#include <queue>
#include <future>

class ChunkManager
{
public:
	ChunkManager(glm::vec3 *playerPosition);
	~ChunkManager();
	void InitWorld();
	int SpawnHeight();
	void GenerateChunks();
	ChunkCoord CalculateChunkCoord(const glm::vec3 &position);
	// void UpdateChunksToRender();
	void Render(const Renderer &renderer);
	void UpdateChunk(ChunkCoord chunk);
	int GetViewDistance() const;
	void SetNewChunks();
	std::array<unsigned int, 3> GetChunkSize() const;
	std::unordered_map<ChunkCoord, Chunk, hash_fn> m_ChunkMap;

private:
	void SortChunks();
	void LoadChunks();
	bool IsInFrustum(Chunk *chunk);
	std::array<unsigned int, 3> m_ChunkSize;
	std::future<std::vector<std::pair<ChunkCoord, Chunk>>> m_FutureChunks;
	VertexBufferLayout m_VertexLayout;
	std::vector<unsigned int> m_Indices;
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
	glm::vec3 *m_PlayerPosition;
	bool m_NewChunks;
};
