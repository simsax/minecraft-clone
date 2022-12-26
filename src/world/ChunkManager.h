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

enum class Channel { SUN, RED, GREEN, BLUE };

class ChunkManager {
public:
	explicit ChunkManager(Player& player);
	ChunkManager(const ChunkManager&) = delete;
	ChunkManager& operator=(const ChunkManager&) = delete;

	void InitWorld(uint32_t stride);
	ChunkCoord CalculateChunkCoord(const glm::vec3& position) const;
	void Render(ChunkRenderer& renderer, Camera& camera);
	glm::vec3 GetChunkSize() const;
	bool IsBlockSolid(const glm::vec3& globalCoords) const;
	bool IsBlockCastable(const glm::vec3& voxel);
	void DestroyBlock();
	void PlaceBlock(Block block);
	void UpdateChunks();
	static bool IsTransparent(Block block);

private:
	struct Raycast {
		Chunk* chunk;
		ChunkCoord chunkCoord;
		glm::uvec3 localVoxel;
		glm::vec3 globalVoxel;
		Chunk* prevChunk;
		ChunkCoord prevChunkCoord;
		glm::uvec3 prevLocalVoxel;
		glm::vec3 prevGlobalVoxel;
		bool selected;
	};

	void SortChunks(const glm::vec3& cameraPos);
	void LoadChunks();
	void LightChunks();
	void MeshChunks();
	std::pair<ChunkCoord, glm::uvec3> GlobalToLocal(const glm::vec3& playerPosition) const;
	void UpdateNeighbors(const glm::uvec3& voxel, const ChunkCoord& chunkCoord);
	void AddBlocks(const ChunkCoord& chunkCoord, BlockVec& blockVec);
	void GenerateChunks();
	void ExpandLight(uint8_t x, uint8_t y, uint8_t z, const glm::uvec4& lightLevel, Chunk* chunk);
	void RemoveLight(uint8_t x, uint8_t y, uint8_t z, const glm::uvec4& lightLevel, Chunk* chunk);

	glm::vec3 m_ChunkSize;
	std::unordered_map<ChunkCoord, Chunk, hash_fn> m_ChunkMap;
	std::vector<Chunk*> m_ChunksToRender;
	std::queue<Chunk*> m_ChunksToMesh;
	std::queue<Chunk*> m_ChunksToLight;
	std::queue<Chunk*> m_ChunksInBorder;
	std::queue<ChunkCoord> m_ChunksToLoad;
	std::unordered_set<ChunkCoord, hash_fn> m_ChunksToUpload;
	std::unordered_map<ChunkCoord, BlockVec, hash_fn> m_BlocksToSet;
	int m_ViewDistance;
	Player& m_Player;
	bool m_SortChunks;
	bool m_ChunksReadyToMesh;
	bool m_ChunksReadyToLight;
	std::pair<ChunkCoord, glm::vec3> m_SelectedBlock;
	Raycast m_Raycast;
	ChunkCoord m_LastChunk;
	ChunkCoord m_CurrentChunk;
	uint32_t m_Stride;
	std::unordered_map<Block, glm::uvec3> m_LightBlocks;
};
