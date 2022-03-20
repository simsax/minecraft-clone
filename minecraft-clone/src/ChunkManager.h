#pragma once
#include "Chunk.h"

class ChunkManager
{
public:
	ChunkManager();
	void GenerateChunks(const glm::vec3& playerPosition);
	void Render(const glm::mat4& mvp);
	int GetViewDistance() const;
	std::array<unsigned int, 3> GetChunkSize() const;
	std::unordered_map<ChunkCoord, Chunk, hash_fn> m_ChunkMap;

private:
	std::array<unsigned int, 3> m_ChunkSize;
	VertexBufferLayout m_VertexLayout;
	std::vector<unsigned int> m_Indices;
	std::vector<Chunk*> m_ChunksToRender;
	unsigned int m_Seed;
	int m_ViewDistance;
};

