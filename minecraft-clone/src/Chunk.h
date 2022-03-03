#pragma once
#include "glm/glm.hpp"
#include <unordered_map>
#include <array>
#include <vector>
#include "Matrix.hpp"

// in the future could be better to have a block class with those as types and some other properties
enum class Block : unsigned char {
	EMPTY,
	GRASS,
	DIRT,
	STONE,
	SNOW,
	DIAMOND,
	GOLD,
	COAL,
	STEEL,
	LEAVES,
	WOOD
};

struct ChunkCoord {
	int x;
	int z;
};

struct hash_fn {
	std::size_t operator()(const ChunkCoord& coord) const;
};

bool operator==(const ChunkCoord& l, const ChunkCoord& r);
bool operator!=(const ChunkCoord& l, const ChunkCoord& r);
int operator-(const ChunkCoord& l, const ChunkCoord& r);

struct Vertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
};

class Chunk
{
public:
	Chunk(unsigned int xLength, unsigned int yLength, unsigned int zLength, const glm::vec3& position, ChunkCoord worldCoords);

	// contains the coordinates of each face in the texture atlas
	static std::unordered_map<Block, std::array<unsigned int, 6>> s_TextureMap;
	static std::unordered_map<ChunkCoord, Chunk, hash_fn> s_ChunkMap;
	std::vector<Vertex> GetRenderData() const;
	void GenerateMesh();
private:
	static bool CheckNorthChunk(Chunk* chunk, unsigned int x, unsigned int y);
	static bool CheckSouthChunk(Chunk* chunk, unsigned int x, unsigned int y);
	static bool CheckWestChunk(Chunk* chunk, unsigned int z, unsigned int y);
	static bool CheckEastChunk(Chunk* chunk, unsigned int z, unsigned int y);

	void SinInit(int xCoord, int zCoord);
	void CreateUQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateDQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateFQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateBQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateRQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateLQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);	

	static const float s_TextureOffset; // depends on the texture atlas
	unsigned int m_XLength;
	unsigned int m_YLength;
	unsigned int m_ZLength;
	glm::vec3 m_Position;
	Matrix<Block> m_Chunk;
	std::vector<Vertex> m_RenderData;
	ChunkCoord m_WorldCoords;
};

// I need a 3d array of blocks, where I track the position of each block and a type (each block type contains different texture coords)

// consider octrees