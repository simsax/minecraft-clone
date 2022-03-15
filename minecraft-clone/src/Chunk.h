#pragma once
#include "glm/glm.hpp"
#include <unordered_map>
#include <array>
#include <vector>
#include "Matrix.hpp"

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
	WOOD,
	WATER,
	SAND
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
	Vertex(glm::vec3 position, glm::vec2 texCoords) : Position(std::move(position)), TexCoords(std::move(texCoords)) {}

	glm::vec3 Position;
	glm::vec2 TexCoords;
};

class Chunk
{
public:
	Chunk(unsigned int xLength, unsigned int yLength, unsigned int zLength, glm::vec3 position, ChunkCoord worldCoords, unsigned int seed);
	static std::unordered_map<ChunkCoord, Chunk, hash_fn> s_ChunkMap;
	std::vector<Vertex> GetMesh();
	Matrix<Block> GetMatrix() const;
private:
	static const std::unordered_map<Block, std::array<float, 24>> s_TextureMap;
	void GenerateMesh();
	void SinInit();
	void Noise2DInit(unsigned int seed);
	void Noise3DInit(unsigned int seed);
	void CreateUQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords);
	void CreateDQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords);
	void CreateFQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords);
	void CreateBQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords);
	void CreateRQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords);
	void CreateLQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords);	

	static const float s_TextureOffset; // depends on the texture atlas
	unsigned int m_XLength;
	unsigned int m_YLength;
	unsigned int m_ZLength;
	glm::vec3 m_Position;
	Matrix<Block> m_Chunk;
	ChunkCoord m_WorldCoords;
	std::vector<Vertex> m_Mesh;
};