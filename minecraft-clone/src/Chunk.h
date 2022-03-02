#pragma once
#include "glm/glm.hpp"
#include <unordered_map>
#include <array>
#include <vector>

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

struct Vertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
};

class Chunk
{
public:
	Chunk(unsigned int xLength, unsigned int yLength, unsigned int zLength, const glm::vec3& position);
	~Chunk();

	// contains the coordinates of each face in the texture atlas
	static std::unordered_map<Block, std::array<unsigned int, 6>> s_TextureMap;
	std::vector<Vertex> GetRenderData() const;
private:
	void CalculateVBOData();
	void CreateUQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateDQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateFQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateBQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateRQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	void CreateLQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords);
	

	std::vector<std::vector<std::vector<Block>>> m_Chunk;
	static const float s_TextureOffset; // depends on the texture atlas
	unsigned int m_XLength;
	unsigned int m_YLength;
	unsigned int m_ZLength;
	glm::vec3 m_Position;
	std::vector<Vertex> m_RenderData;
};

// I need a 3d array of blocks, where I track the position of each block and a type (each block type contains different texture coords)

// consider octrees