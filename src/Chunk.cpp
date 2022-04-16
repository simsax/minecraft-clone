#include "Chunk.h"
#include <iostream>
#include <stdlib.h>
#include "Noise.h"

const float Chunk::s_TextureOffset = 0.0625f; // texture_size/atlas_size

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

// values are precomputed for speed
const std::unordered_map<Block, std::array<float, 24>> Chunk::s_TextureMap =
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
	{ Block::SNOW,		{0.125, 0.6875, 0.1875, 0.6875, 0.1875, 0.75, 0.125, 0.75, 0.25, 0.6875, 0.3125, 0.6875, 0.3125, 0.75, 0.25, 0.75, 0.125, 0.9375, 0.1875, 0.9375, 0.1875, 1.0, 0.125, 1.0 } },
	{ Block::WATER,		{0.9375, 0.125, 1.0, 0.125, 1.0, 0.1875, 0.9375, 0.1875, 0.9375, 0.125, 1.0, 0.125, 1.0, 0.1875, 0.9375, 0.1875, 0.9375, 0.125, 1.0, 0.125, 1.0, 0.1875, 0.9375, 0.1875 } },
	{ Block::SAND,		{0.125, 0.875, 0.1875, 0.875, 0.1875, 0.9375, 0.125, 0.9375, 0.125, 0.875, 0.1875, 0.875, 0.1875, 0.9375, 0.125, 0.9375, 0.125, 0.875, 0.1875, 0.875, 0.1875, 0.9375, 0.125, 0.9375 } }
};

// the chunk has a border so that I know what faces to cull between chunks (I only generate the mesh of the part inside the border)
Chunk::Chunk(unsigned int xLength, unsigned int yLength, unsigned int zLength, glm::vec3 position, unsigned int seed, const VertexBufferLayout& layout,
		unsigned int maxVertexCount, const std::vector<unsigned int>& indices, ChunkCoord worldCoords) : 
	m_XLength(xLength), m_YLength(yLength), m_ZLength(zLength), m_Position(std::move(position)), m_Chunk(Matrix<Block>(m_XLength, m_YLength, m_ZLength)), m_Seed(seed),
	m_MaxVertexCount(maxVertexCount), m_WorldCoords(worldCoords)
{

	m_IBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
	m_VBO = std::make_unique<VertexBuffer>();
	m_VBO->CreateDynamic(sizeof(Vertex) * maxVertexCount);
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, layout);
	
	m_Mesh.reserve(m_MaxVertexCount);
	//SinInit();
	Noise2DInit();
}

// fine tune the values
static float Continentalness(int x, int y) {
	float scale = 256.0f;
	float ampl = 1.0f;
	float freq = 0.3f;
	float height = 0;
	/*
	std::vector<std::vector<int>> octaveOffsets;
	for (unsigned int i = 0; i < 1; i++) {
		octaveOffsets.push_back({ rand() % 10000, rand() % 10000 });
	}*/
	float noise = noise::Perlin2D(x, y, scale, freq, ampl);

	if (noise < 0.3) {
		height = (noise + 2.3f) / 0.026f;
	}
	else if (0.3 <= noise && noise < 0.4) {
		height = 500 * noise - 50;
	}
	else if (noise >= 0.4) {
		height = 150;
	}

	return height;
}


static float Noise(int x, int y, unsigned int octaves, const std::vector<std::vector<int>>& octaveOffsets) {
	float freq = 0.5f;
	float ampl = 20.0f;
	float scale = 64.0f;
	//float terrain_height = Continentalness(x, y);
	float terrain_height = 100;

	float height = noise::Perlin2D(x, y, scale, freq, ampl, octaves, octaveOffsets);

	return terrain_height + height;
}

void Chunk::Noise2DInit() {
	srand(m_Seed);
	std::vector<std::vector<int>> octaveOffsets;
	unsigned int n_octaves = 4;
	for (unsigned int i = 0; i < n_octaves; i++) {
		octaveOffsets.push_back({ rand() % 10000, rand() % 10000 });
	}

	for (unsigned int i = 0; i < m_XLength; i++) {
		for (unsigned int k = 0; k < m_ZLength; k++) {
			unsigned int w = static_cast<unsigned int>(Noise(static_cast<int>(i + m_Position.x), static_cast<int>(k + m_Position.z), n_octaves, octaveOffsets));

			for (unsigned int j = 0; j < m_YLength; j++) {
				if (j < w) {
					if (j < 100)
						m_Chunk(i, j, k) = Block::SAND;
					else if (j == w - 1 || j == m_YLength - 1)
						m_Chunk(i, j, k) = Block::GRASS;
					else
						m_Chunk(i, j, k) = Block::STONE;
				}
				else {
					if (j < 100) // this value may depend on the biome
						m_Chunk(i, j, k) = Block::WATER;
					else
						m_Chunk(i, j, k) = Block::EMPTY;
				}
			}
		}
	}
}

void Chunk::Noise3DInit(unsigned int seed) {
	srand(seed);
	float freq = 0.5f;
	float ampl = 20.0f;
	float scale = 64.0f;
	std::vector<std::vector<int>> octaveOffsets;
	unsigned int n_octaves = 1;
	for (unsigned int i = 0; i < n_octaves; i++) {
		octaveOffsets.push_back({ rand() % 10000, rand() % 10000, rand() % 10000 });
	}

	for (int i = 0; i < (int)m_XLength; i++) {
		for (int k = 0; k < (int)m_ZLength; k++) {
			for (int j = 0; j < (int)m_YLength; j++) {
				float density = noise::Perlin3D(i + (int)m_Position.x, j + (int)m_Position.y, k + (int)m_Position.z, scale, 1, 1);
				if (density <= 0 || j > 150) {
					m_Chunk(i, j, k) = Block::EMPTY;
				}
				else {
					m_Chunk(i, j, k) = Block::STONE;
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

std::array<unsigned int,3> Chunk::GetChunkSize() const
{
	return {m_XLength, m_YLength, m_ZLength};
}

static bool CheckNorthChunk(Chunk* chunk, unsigned int x, unsigned int y) {
	if (chunk->GetMatrix()(x, y, chunk->GetChunkSize()[2] - 1) == Block::EMPTY)
		return true;
	else
		return false;
}

static bool CheckSouthChunk(Chunk* chunk, unsigned int x, unsigned int y) {
	if (chunk->GetMatrix()(x, y, 0) == Block::EMPTY)
		return true;
	else
		return false;
}

static bool CheckWestChunk(Chunk* chunk, unsigned int z, unsigned int y) {
	if (chunk->GetMatrix()(chunk->GetChunkSize()[0] - 1, y, z) == Block::EMPTY)
		return true;
	else
		return false;
}

static bool CheckEastChunk(Chunk* chunk, unsigned int z, unsigned int y) {
	if (chunk->GetMatrix()(0, y, z) == Block::EMPTY)
		return true;
	else
		return false;
}

void Chunk::GenerateMesh(std::unordered_map<ChunkCoord, Chunk, hash_fn>* ChunkMap) {
	if (m_Mesh.empty()) {
		Chunk* northChunk = nullptr;
		Chunk* eastChunk = nullptr;
		Chunk* southChunk = nullptr;
		Chunk* westChunk = nullptr;

		bool borderNorth = false;
		bool borderEast = false;
		bool borderSouth = false;
		bool borderWest = false;

		// if I find it I am not in the borders
		if (ChunkMap->find({ m_WorldCoords.x, m_WorldCoords.z - 1 }) != ChunkMap->end())
			northChunk = &ChunkMap->find({ m_WorldCoords.x, m_WorldCoords.z - 1 })->second;
		else
			borderNorth = true;
		if (ChunkMap->find({ m_WorldCoords.x + 1, m_WorldCoords.z }) != ChunkMap->end())
			eastChunk = &ChunkMap->find({ m_WorldCoords.x + 1, m_WorldCoords.z })->second;
		else
			borderEast = true;
		if (ChunkMap->find({ m_WorldCoords.x, m_WorldCoords.z + 1 }) != ChunkMap->end())
			southChunk = &ChunkMap->find({ m_WorldCoords.x, m_WorldCoords.z + 1 })->second;
		else
			borderSouth = true;
		if (ChunkMap->find({ m_WorldCoords.x - 1, m_WorldCoords.z }) != ChunkMap->end())
			westChunk = &ChunkMap->find({ m_WorldCoords.x - 1, m_WorldCoords.z })->second;
		else
			borderWest = true;

		// I want to render it relative to the center of m_Position
		int xCoord = static_cast<int>(m_Position.x - m_XLength / 2);
		//int yCoord = static_cast<int>(m_Position.y - m_YLength - 1);
		int yCoord = -150;
		int zCoord = static_cast<int>(m_Position.z - m_ZLength / 2);
		glm::vec3 center(xCoord, yCoord, zCoord);

		for (unsigned int i = 0; i < m_XLength; i++) {
			for (unsigned int k = 0; k < m_ZLength; k++) {
				for (unsigned int j = 0; j < m_YLength; j++) {
					if (m_Chunk(i, j, k) != Block::EMPTY) {
						std::array<float, 24> textureCoords = s_TextureMap.at(m_Chunk(i, j, k));
						if (j == 0 || j > 0 && m_Chunk(i, j - 1, k) == Block::EMPTY) { // D
							CreateDQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
						}
						if (j == m_YLength - 1 || j < m_YLength - 1 && m_Chunk(i, j + 1, k) == Block::EMPTY) { // U	
							CreateUQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
						}
						if (!borderNorth && k == 0 && CheckNorthChunk(northChunk, i, j) || k > 0 && m_Chunk(i, j, k - 1) == Block::EMPTY) { // B
							CreateBQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
						}
						if (!borderSouth && k == m_ZLength - 1 && CheckSouthChunk(southChunk, i, j) || k < m_ZLength - 1 && m_Chunk(i, j, k + 1) == Block::EMPTY) { // F
							CreateFQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
						}
						if (!borderWest && i == 0 && CheckWestChunk(westChunk, k, j) || i > 0 && m_Chunk(i - 1, j, k) == Block::EMPTY) { // L
							CreateLQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
						}
						if (!borderEast && i == m_XLength - 1 && CheckEastChunk(eastChunk, k, j) || i < m_XLength - 1 && m_Chunk(i + 1, j, k) == Block::EMPTY) { // R
							CreateRQuad(m_Mesh, center + glm::vec3(i, j, k), textureCoords);
						}
					}
				}
			}
		}
		size_t indexCount = m_Mesh.size() / 4 * 6; // num faces * 6
		m_VBO->SendData(m_Mesh.size() * sizeof(Vertex), m_Mesh.data());
		m_IBO->SetCount(indexCount);
	}
}

void Chunk::UpdateMesh(unsigned int x, unsigned int y, unsigned int z, Block block)
{
	// check the blocks surrounding the modified block and figure out a way to edit the m_Mesh accordingly
	for (unsigned int i = x - 1; i <= x + 1; i++) {
		for (unsigned int k = z - 1; k <= z + 1; k++) {
			for (unsigned int j = y - 1; j <= y + 1; j++) {
				// to figure out
			}
		}
	}
}

void Chunk::Render(const Renderer& renderer)
{
	renderer.Draw(*m_VAO, *m_IBO, GL_UNSIGNED_INT);
}

Matrix<Block> Chunk::GetMatrix() const
{
	return m_Chunk;
}

void Chunk::SetMatrix(unsigned int x, unsigned int y, unsigned int z, Block block)
{
	m_Chunk(x, y, z) = block;
	m_Mesh.clear();
	//UpdateMesh(x,y,z,block);
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


