#include "Chunk.h"
#include <iostream>
#include <glm/gtc/noise.hpp>

const float Chunk::s_TextureOffset = 0.0625f; // texture_size/atlas_size

std::unordered_map<ChunkCoord, Chunk, hash_fn> Chunk::s_ChunkMap;

bool operator==(const ChunkCoord& l, const ChunkCoord& r)
{
	return l.x == r.x && l.z == r.z;
}

bool operator!=(const ChunkCoord& l, const ChunkCoord& r)
{
	return l.x != r.x || l.z != r.z;
}

int operator-(const ChunkCoord& l, const ChunkCoord& r) {
	return static_cast<int>(round(sqrt(pow(l.x - r.x, 2) + pow(l.z - r.z, 2))));
}

std::size_t hash_fn::operator()(const ChunkCoord& coord) const
{
	std::size_t h1 = std::hash<int>()(coord.x);
	std::size_t h2 = std::hash<int>()(coord.z);
	return h1 ^ h2;
}
/*
// coords are top.x,top.y,side.x,side.y, bottom.x, bottom.y (for most block it's redundant, but for now it makes my life easier. I will fix it later)
const std::unordered_map<Block, std::array<unsigned int, 6>> Chunk::s_TextureMap =
{
	{ Block::GRASS,		{ 12, 3, 3, 15, 2, 15 } },
	{ Block::DIRT,		{ 2, 15, 2, 15, 2, 15 } },
	{ Block::STONE,		{ 1, 15, 1, 15, 1, 15 } },
	{ Block::SNOW,		{ 2, 11, 4, 11, 2, 15 } },
	{ Block::DIAMOND,	{ 2, 12, 2, 12, 2, 12 } },
	{ Block::GOLD,		{ 0, 13, 0, 13, 0, 13 } },
	{ Block::COAL,		{ 2, 13, 2, 13, 2, 13 } },
	{ Block::STEEL,		{ 1, 13, 1, 13, 1, 13 } },
	{ Block::LEAVES,	{ 11, 1, 11, 1, 11, 1 } },
	{ Block::WOOD,		{ 5, 14, 4, 14, 5, 14 } }
};
*/
// values are precomputed for speed
const std::unordered_map<Block, std::array<float, 24>> Chunk::s_AltTextureMap =
{
	{ Block::GRASS,		{0.75, 0.1875, 0.8125, 0.1875, 0.8125, 0.25, 0.75, 0.25, 0.1875, 0.9375, 0.25, 0.9375, 0.25, 1.0, 0.1875, 1.0, 0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0 } },
	{ Block::DIRT,		{0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0, 0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0, 0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0 } },
	{ Block::STONE,		{0.0625, 0.9375, 0.125, 0.9375, 0.125, 1.0, 0.0625, 1.0, 0.0625, 0.9375, 0.125, 0.9375, 0.125, 1.0, 0.0625, 1.0, 0.0625, 0.9375, 0.125, 0.9375, 0.125, 1.0, 0.0625, 1.0 } },
	{ Block::DIAMOND,		{0.125, 0.75, 0.1875, 0.75, 0.1875, 0.8125, 0.125, 0.8125, 0.125, 0.75, 0.1875, 0.75, 0.1875, 0.8125, 0.125, 0.8125, 0.125, 0.75, 0.1875, 0.75, 0.1875, 0.8125, 0.125, 0.8125 } },
	{ Block::GOLD,	{0.0, 0.8125, 0.0625, 0.8125, 0.0625, 0.875, 0.0, 0.875, 0.0, 0.8125, 0.0625, 0.8125, 0.0625, 0.875, 0.0, 0.875, 0.0, 0.8125, 0.0625, 0.8125, 0.0625, 0.875, 0.0, 0.875 } },
	{ Block::COAL,		{0.125, 0.8125, 0.1875, 0.8125, 0.1875, 0.875, 0.125, 0.875, 0.125, 0.8125, 0.1875, 0.8125, 0.1875, 0.875, 0.125, 0.875, 0.125, 0.8125, 0.1875, 0.8125, 0.1875, 0.875, 0.125, 0.875 } },
	{ Block::STEEL,		{0.0625, 0.8125, 0.125, 0.8125, 0.125, 0.875, 0.0625, 0.875, 0.0625, 0.8125, 0.125, 0.8125, 0.125, 0.875, 0.0625, 0.875, 0.0625, 0.8125, 0.125, 0.8125, 0.125, 0.875, 0.0625, 0.875 } },
	{ Block::LEAVES,		{0.6875, 0.0625, 0.75, 0.0625, 0.75, 0.125, 0.6875, 0.125, 0.6875, 0.0625, 0.75, 0.0625, 0.75, 0.125, 0.6875, 0.125, 0.6875, 0.0625, 0.75, 0.0625, 0.75, 0.125, 0.6875, 0.125 } },
	{ Block::WOOD,	{0.3125, 0.875, 0.375, 0.875, 0.375, 0.9375, 0.3125, 0.9375, 0.25, 0.875, 0.3125, 0.875, 0.3125, 0.9375, 0.25, 0.9375, 0.3125, 0.875, 0.375, 0.875, 0.375, 0.9375, 0.3125, 0.9375 } },
	{ Block::SNOW,		{0.125, 0.6875, 0.1875, 0.6875, 0.1875, 0.75, 0.125, 0.75, 0.25, 0.6875, 0.3125, 0.6875, 0.3125, 0.75, 0.25, 0.75, 0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0 } }
};

// the chunk has a border so that I know what faces to cull between chunks (I only generate the mesh of the part inside the border)
Chunk::Chunk(unsigned int xLength, unsigned int yLength, unsigned int zLength, glm::vec3 position, ChunkCoord worldCoords) : 
	m_XLength(xLength+2), m_YLength(yLength+2), m_ZLength(zLength+2), m_Position(std::move(position)), m_Chunk(Matrix<Block>(m_XLength, m_YLength, m_ZLength)), m_WorldCoords(worldCoords)
{
	//SinInit();
	NoiseInit();
}

// can be optimized
static float noise(int x, int y) {
	float freq = 0.1f;
	float ampl = 50.0f;

	float xf = x / 40.0f;
	float yf = y / 40.0f;

	float sum = 0.0f;
	for (int oct = 0; oct < 6; oct++) {
		glm::vec2 p(xf * freq, yf * freq);
		float val = glm::perlin(p) * ampl;
		sum += val;
		freq *= 2;
		ampl /= 2;
	}

	return 100 + sum;
}

void Chunk::NoiseInit() {
	for (unsigned int i = 0; i < m_XLength; i++) {
		for (unsigned int k = 0; k < m_ZLength; k++) {
			unsigned int w = static_cast<unsigned int>(noise(static_cast<int>(i + m_Position.x), static_cast<int>(k + m_Position.z)));
			for (unsigned int j = 0; j < m_YLength; j++) {
				if (j < w) {
					if (j == w - 1 || j == m_YLength - 1)
						m_Chunk(i, j, k) = Block::GRASS;
					else
						m_Chunk(i, j, k) = Block::STONE;
				}
				else {
					m_Chunk(i, j, k) = Block::EMPTY;
				}
			}
		}
	}
}

void Chunk::SinInit() {
	float freq = 0.2f;
	float ampl = 10.0f;

	for (unsigned int i = 0; i < m_XLength; i++) {
		unsigned int h = static_cast<unsigned int>(round(100 + sin((i + m_Position.x) * freq) * ampl));
		for (unsigned int k = 0; k < m_ZLength; k++) {
			unsigned int w = h + static_cast<unsigned int>(round(100 + sin((k + m_Position.z) * freq) * ampl));
			for (unsigned int j = 0; j < m_YLength; j++) {
				if (j < w) {
					if (j == w - 1 || j == m_YLength - 1)
						m_Chunk(i, j, k) = Block::GRASS;
					else
						m_Chunk(i, j, k) = Block::STONE;
				}
				else {
					m_Chunk(i, j, k) = Block::EMPTY;
				}
			}
		}
	}
}

void Chunk::GenerateMesh() {
	m_Mesh.reserve(4096);

	// I want to render it relative to the center of m_Position
	int xCoord = static_cast<int>(m_Position.x - m_XLength / 2);
	//int yCoord = static_cast<int>(m_Position.y - m_YLength - 1);
	int yCoord = -150;
	int zCoord = static_cast<int>(m_Position.z - m_ZLength / 2);
	glm::vec3 center(xCoord, yCoord, zCoord);

	for (unsigned int i = 1; i < m_XLength - 1; i++) {
		for (unsigned int k = 1; k < m_ZLength - 1; k++) {
			for (unsigned int j = 1; j < m_YLength - 1; j++) {
				if (m_Chunk(i, j, k) != Block::EMPTY) {
					std::array<float, 24> textureCoords = s_AltTextureMap.at(m_Chunk(i, j, k));
					if (j > 0 && m_Chunk(i, j - 1, k) == Block::EMPTY) { // D
						CreateDQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
					}
					if (j < m_YLength - 1 && m_Chunk(i, j + 1, k) == Block::EMPTY) { // U	
						CreateUQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
					}
					if (k > 0 && m_Chunk(i, j, k - 1) == Block::EMPTY) { // B
						CreateBQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
					}
					if (k < m_ZLength - 1 && m_Chunk(i, j, k + 1) == Block::EMPTY) { // F
						CreateFQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
					}
					if (i > 0 && m_Chunk(i - 1, j, k) == Block::EMPTY) { // L
						CreateLQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
					}
					if (i < m_XLength - 1 && m_Chunk(i + 1, j, k) == Block::EMPTY) { // R
						CreateRQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
					}
				}
			}
		}
	}
}

std::vector<Vertex> Chunk::GetMesh()
{
	if (m_Mesh.empty())
		GenerateMesh();
	return m_Mesh;
}


void Chunk::CreateUQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords) {
	float size = 1.0f;

	target.emplace_back(glm::vec3(position[0], position[1] + size, position[2] + size),glm::vec2(textureCoords[0], textureCoords[1]));
	target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2] + size),glm::vec2(textureCoords[2], textureCoords[3]));
	target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2]),glm::vec2(textureCoords[4], textureCoords[5]));
	target.emplace_back(glm::vec3(position[0], position[1] + size, position[2]),glm::vec2(textureCoords[6], textureCoords[7]));
}

void Chunk::CreateDQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords) {
	float size = 1.0f;

	target.emplace_back(glm::vec3(position[0], position[1], position[2] + size), glm::vec2(textureCoords[16], textureCoords[17]));
	target.emplace_back(glm::vec3(position[0], position[1], position[2]), glm::vec2(textureCoords[18], textureCoords[19]));
	target.emplace_back(glm::vec3(position[0] + size, position[1], position[2]), glm::vec2(textureCoords[20], textureCoords[21]));
	target.emplace_back(glm::vec3(position[0] + size, position[1], position[2] + size), glm::vec2(textureCoords[22], textureCoords[23]));
}

void Chunk::CreateFQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords) {
	float size = 1.0f;

	target.emplace_back(glm::vec3(position[0], position[1], position[2] + size), glm::vec2(textureCoords[8], textureCoords[9]));
	target.emplace_back(glm::vec3(position[0] + size, position[1], position[2] + size), glm::vec2(textureCoords[10], textureCoords[11]));
	target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2] + size), glm::vec2(textureCoords[12], textureCoords[13]));
	target.emplace_back(glm::vec3(position[0], position[1] + size, position[2] + size), glm::vec2(textureCoords[14], textureCoords[15]));
}

void Chunk::CreateBQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords) {
	float size = 1.0f;

	target.emplace_back(glm::vec3(position[0] + size, position[1], position[2]), glm::vec2(textureCoords[8], textureCoords[9]));
	target.emplace_back(glm::vec3(position[0], position[1], position[2]), glm::vec2(textureCoords[10], textureCoords[11]));
	target.emplace_back(glm::vec3(position[0], position[1] + size, position[2]), glm::vec2(textureCoords[12], textureCoords[13]));
	target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2]), glm::vec2(textureCoords[14], textureCoords[15]));
}

void Chunk::CreateRQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords) {
	float size = 1.0f;

	target.emplace_back(glm::vec3(position[0] + size, position[1], position[2] + size), glm::vec2(textureCoords[8], textureCoords[9]));
	target.emplace_back(glm::vec3(position[0] + size, position[1], position[2]), glm::vec2(textureCoords[10], textureCoords[11]));
	target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2]), glm::vec2(textureCoords[12], textureCoords[13]));
	target.emplace_back(glm::vec3(position[0] + size, position[1] + size, position[2] + size), glm::vec2(textureCoords[14], textureCoords[15]));
}

void Chunk::CreateLQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<float, 24>& textureCoords) {
	float size = 1.0f;

	target.emplace_back(glm::vec3(position[0], position[1], position[2]), glm::vec2(textureCoords[8], textureCoords[9]));
	target.emplace_back(glm::vec3(position[0], position[1], position[2] + size), glm::vec2(textureCoords[10], textureCoords[11]));
	target.emplace_back(glm::vec3(position[0], position[1] + size, position[2] + size), glm::vec2(textureCoords[12], textureCoords[13]));
	target.emplace_back(glm::vec3(position[0], position[1] + size, position[2]), glm::vec2(textureCoords[14], textureCoords[15]));
}

