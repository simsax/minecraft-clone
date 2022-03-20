#include "ChunkManager.h"
#include "time.h"
#include <iostream>

#define MAX_INDEX_COUNT 18432 // each cube has 6 faces, each face has 6 indexes
#define MAX_VERTEX_COUNT 12228 // each cube has 6 faces, each face has 4 vertices 
#define VIEW_DISTANCE 6 // how far the player sees
#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 256
#define CHUNK_SIZE_Z 16


ChunkManager::ChunkManager():
	m_ViewDistance(VIEW_DISTANCE)
{
	m_VertexLayout.Push<float>(3); // position
	m_VertexLayout.Push<float>(2); // texture coords
		
	m_ChunksToRender.reserve(static_cast<const unsigned int>((m_ViewDistance * 2 + 1) * (m_ViewDistance * 2 + 1)));

	m_Indices.reserve(MAX_INDEX_COUNT);
	unsigned int offset = 0;
	
	for (size_t i = 0; i < MAX_INDEX_COUNT * 2; i += 6) {
		m_Indices.push_back(0 + offset);
		m_Indices.push_back(1 + offset);
		m_Indices.push_back(2 + offset);

		m_Indices.push_back(2 + offset);
		m_Indices.push_back(3 + offset);
		m_Indices.push_back(0 + offset);

		offset += 4;
	}

	m_ChunkSize = { CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z };
	m_Seed = static_cast<unsigned int>(time(NULL));
}

void ChunkManager::Render(const glm::mat4& mvp)
{
	for (auto& chunk : m_ChunksToRender) {
		chunk->Render(mvp);
	}
}

int ChunkManager::GetViewDistance() const
{
	return m_ViewDistance;
}

std::array<unsigned int, 3> ChunkManager::GetChunkSize() const
{
	return m_ChunkSize;
}

void ChunkManager::GenerateChunks(const glm::vec3& playerPosition)
{
	int playerPosX = static_cast<int>(round(playerPosition.x / static_cast<int>(m_ChunkSize[0])));
	int playerPosZ = static_cast<int>(round(playerPosition.z / static_cast<int>(m_ChunkSize[2])));

	m_ChunksToRender.clear();

	// load chunks
	for (int i = -m_ViewDistance + playerPosX; i <= m_ViewDistance + playerPosX; i++) {
		for (int j = -m_ViewDistance + playerPosZ; j <= m_ViewDistance + playerPosZ; j++) {
			ChunkCoord coords = { i, j };
			// check if this chunk hasn't already been generated
			if (m_ChunkMap.find(coords) == m_ChunkMap.end()) {
				// create new chunk and cache it
				Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2], glm::vec3(i *static_cast<int>(m_ChunkSize[0]), 0.0, j *  static_cast<int>(m_ChunkSize[2])), m_Seed, m_VertexLayout, MAX_VERTEX_COUNT, m_Indices);
				chunk.GenerateMesh();
				m_ChunkMap.insert({ coords, std::move(chunk) });
			}
			m_ChunksToRender.push_back(&m_ChunkMap.find(coords)->second);
		}
	}
}

