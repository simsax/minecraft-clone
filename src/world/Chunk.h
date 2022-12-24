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
#include <queue>

static constexpr uint32_t XSIZE = 16;
static constexpr uint32_t YSIZE = 256;
static constexpr uint32_t ZSIZE = 16;

struct ChunkCoord {
	int x;
	int z;
};

struct hash_fn {
	std::size_t operator()(const ChunkCoord& coord) const;
};

struct LightAddNode;
struct LightRemNode;

bool operator==(const ChunkCoord& l, const ChunkCoord& r);

bool operator!=(const ChunkCoord& l, const ChunkCoord& r);

int operator-(const ChunkCoord& l, const ChunkCoord& r);

using BlockVec = std::vector<std::pair<Block, glm::vec3>>;

class Chunk {
public:
	using ChunkMap = std::unordered_map<ChunkCoord, Chunk, hash_fn>;

	Chunk(const ChunkCoord& coords, const glm::vec3& position, uint32_t stride, ChunkMap* chunkMap);
	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;
	Chunk(Chunk&& other) = default;
	Chunk& operator=(Chunk&& other) = default;

	void GenerateMesh();
	Block GetBlock(uint32_t x, uint32_t y, uint32_t z) const;
	void SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block);
	void Render(ChunkRenderer& renderer, const ChunkCoord& playerChunk, int radius);
	void RenderOutline(ChunkRenderer& renderer, const glm::uvec3& target);
	glm::vec3 GetCenterPosition() const;
	BlockVec CreateSurfaceLayer(const BlockVec& blocksToSet);
	ChunkCoord GetCoord() const;
	bool ChunkIsVisible(ChunkCoord playerChunk, int radius) const;
	void ClearMesh();
	void UpdateMeshHeighLimit(uint32_t height);
	void SetSunLight(int i, int j, int k, uint8_t val);
	uint8_t GetSunLight(int i, int j, int k);
	void SetRedLight(int i, int j, int k, uint8_t val);
	uint8_t GetRedLight(int i, int j, int k);
	void SetGreenLight(int i, int j, int k, uint8_t val);
	uint8_t GetGreenLight(int i, int j, int k);
	void SetBlueLight(int i, int j, int k, uint8_t val);
	uint8_t GetBlueLight(int i, int j, int k);
	std::array<Chunk*, 8> GetNeighbors() const;
	uint32_t GetIndex(uint8_t i, uint8_t j, uint8_t k) const;
	glm::uvec3 GetCoordsFromIndex(uint32_t index) const;
	bool FindNeighbors();
	int GetMaxHeight() const;
	void FillSunLight();

private:
	struct Vertex {
		Vertex(uint32_t b, uint32_t l)
			: base(b)
			, light(l)
		{
		}
		uint32_t base;
		uint32_t light;
	};

	enum class Face : uint8_t {
		Up, Front, Back, Left, Right, Down
	};

	void CreateHeightMap();
	void FastFill();

	template <typename... Args>
	void GenCube(int i, int j, int k, std::vector<Vertex>& target,
		const TextureAtlas::Coords& textureCoords, bool lightBlock, Args... voidBlocks);

	void GenCubeOutline(int i, int j, int k, std::vector<uint32_t>& target);

	void GenSprite(int i, int j, int k, std::vector<Vertex>& target,
		const std::array<uint32_t, 4>& textureCoords);

	void CreateQuad(std::vector<Vertex>& target, const glm::uvec3& position,
		const std::array<uint32_t, 4>& textureCoords, const glm::uvec4& offsetx,
		const glm::uvec4& offsety, const glm::uvec4& offsetz, uint8_t normalIndex,
		uint32_t lightVal, Face face);

	void CreateQuadOutline(std::vector<uint32_t>& target, const glm::uvec3& position,
		const glm::uvec4& offsetx, const glm::uvec4& offsety, const glm::uvec4& offsetz);

	void SetBlocks(const BlockVec& blocksToSet);
	void CreateTrees(int i, int j, int k, BlockVec& blockVec);
	void CreateSprites(int i, int j, int k, BlockVec& blockVec);

	template <typename... Args>
	bool CheckNeighbor(const Chunk* chunk, const glm::uvec3& position, Args... voidBlocks);

	std::array<uint32_t, 4> CalculateAO(int i, int j, int k,
		const std::array<std::array<int, 3>, 4>& offsets, uint8_t side);

	Chunk* RetrieveChunk(int x, int z);

	VertexBuffer m_VBO;
	std::array<int, XSIZE* ZSIZE> m_HeightMap;
	Noise m_Noise;
	ChunkCoord m_Coords;
	glm::vec3 m_ChunkPosition;
	Matrix3D<Block, XSIZE, YSIZE, ZSIZE> m_Chunk;
	Matrix3D<uint32_t, XSIZE, YSIZE, ZSIZE> m_LightMap;
	std::vector<Vertex> m_Mesh;
	std::vector<Vertex> m_TransparentMesh;
	std::vector<Vertex> m_SpriteMesh;
	std::array<Chunk*, 8> m_Neighbors;
	int m_MinHeight;
	int m_MaxHeight;
	size_t m_IBOCount;
	size_t m_TIBOCount;
	size_t m_SIBOCount;
	ChunkMap* m_ChunkMap;
	glm::vec3 m_CenterPosition;
};
