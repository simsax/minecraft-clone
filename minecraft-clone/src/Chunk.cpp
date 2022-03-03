#include "Chunk.h"
#include <memory>

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

// coords are top.x,top.y,side.x,side.y, bottom.x, bottom.y (for most block it's redundant, but for now it makes my life easier. I will fix it later)
std::unordered_map<Block, std::array<unsigned int, 6>> Chunk::s_TextureMap =
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

// also try to flatten the array
Chunk::Chunk(unsigned int xLength, unsigned int yLength, unsigned int zLength, const glm::vec3& position, ChunkCoord worldCoords) : 
	m_XLength(xLength), m_YLength(yLength), m_ZLength(zLength), m_Position(position), m_Chunk(Matrix<Block>(xLength, yLength, zLength)), m_WorldCoords(worldCoords)
{
	
	m_Chunk.fill(Block::STONE);
	for (unsigned int i = 0; i < m_XLength; i++) {
		for (unsigned int j = 0; j < m_ZLength; j++) {
			m_Chunk(i,m_YLength - 1,j) = Block::EMPTY;
		}
	}
	m_Chunk(0, m_YLength - 1, 0) = Block::GRASS;
	//SinInit(xCoord, zCoord);
	//GenerateMesh();
}

// even this can be parallelized probably

void Chunk::SinInit(int xCoord, int zCoord) {
	for (unsigned int i = 0; i < m_XLength; i++) {
		unsigned int h = static_cast<unsigned int>(round(m_YLength + sin(i + xCoord) * 5));
		for (unsigned int k = 0; k < m_ZLength; k++) {
			for (unsigned int j = 0; j < m_YLength; j++) {
				if (j < h) {
					m_Chunk(i, j, k) = Block::STONE;
				}
				else {
					m_Chunk(i, j, k) = Block::EMPTY;
				}
			}
		}
	}
}

bool Chunk::CheckNorthChunk(Chunk* chunk, unsigned int x, unsigned int y) {
	if (chunk->m_Chunk(x, y, chunk->m_ZLength - 1) == Block::EMPTY)
		return true;
	else
		return false;
}

bool Chunk::CheckSouthChunk(Chunk* chunk, unsigned int x, unsigned int y) {
	if (chunk->m_Chunk(x, y, 0) == Block::EMPTY)
		return true;
	else
		return false;
}

bool Chunk::CheckWestChunk(Chunk* chunk, unsigned int z, unsigned int y) {
	if (chunk->m_Chunk(chunk->m_XLength - 1, y, z) == Block::EMPTY)
		return true;
	else
		return false;
}

bool Chunk::CheckEastChunk(Chunk* chunk, unsigned int z, unsigned int y) {
	if (chunk->m_Chunk(0, y, z) == Block::EMPTY)
		return true;
	else
		return false;
}

void Chunk::GenerateMesh() {
	std::vector<Vertex> vertices;
	vertices.reserve(4096);

	Chunk* northChunk = nullptr;
	Chunk* eastChunk = nullptr;
	Chunk* southChunk = nullptr;
	Chunk* westChunk = nullptr;


	bool borderNorth = false;
	bool borderEast = false;
	bool borderSouth = false;
	bool borderWest = false;

	// if I find it I am not in the borders
	if (Chunk::s_ChunkMap.find({ m_WorldCoords.x, m_WorldCoords.z - 1 }) != Chunk::s_ChunkMap.end())
		northChunk = &Chunk::s_ChunkMap.find({ m_WorldCoords.x, m_WorldCoords.z - 1 })->second;
	else
		borderNorth = true;
	if (Chunk::s_ChunkMap.find({ m_WorldCoords.x + 1, m_WorldCoords.z }) != Chunk::s_ChunkMap.end())
		eastChunk = &Chunk::s_ChunkMap.find({ m_WorldCoords.x + 1, m_WorldCoords.z })->second;
	else
		borderEast = true;
	if (Chunk::s_ChunkMap.find({ m_WorldCoords.x, m_WorldCoords.z + 1 }) != Chunk::s_ChunkMap.end())
		southChunk = &Chunk::s_ChunkMap.find({ m_WorldCoords.x, m_WorldCoords.z + 1 })->second;
	else
		borderSouth = true;
	if (Chunk::s_ChunkMap.find({ m_WorldCoords.x - 1, m_WorldCoords.z }) != Chunk::s_ChunkMap.end())
		westChunk = &Chunk::s_ChunkMap.find({ m_WorldCoords.x - 1, m_WorldCoords.z })->second;
	else
		borderWest = true;

	// I want to render it relative to the center of m_Position
	int xCoord = static_cast<int>(m_Position.x - m_XLength / 2);
	int yCoord = static_cast<int>(m_Position.y - m_YLength - 1);
	int zCoord = static_cast<int>(m_Position.z - m_ZLength / 2);
	glm::vec3 center(xCoord, yCoord, zCoord);

	// there must be a better way to write this but I have no time for now
	for (unsigned int i = 0; i < m_XLength; i++) {
		for (unsigned int k = 0; k < m_ZLength; k++) {
			for (unsigned int j = 0; j < m_YLength; j++) {
				if (m_Chunk(i, j, k) != Block::EMPTY) {
					std::array<unsigned int, 6> textureCoords = s_TextureMap[m_Chunk(i, j, k)];
					if (j == 0 || j > 0 && m_Chunk(i, j - 1, k) == Block::EMPTY) { // D
						CreateDQuad(vertices, center + glm::vec3(i, j, k), { textureCoords[4], textureCoords[5] });
					}
					if (j == m_YLength - 1 || j < m_YLength - 1 && m_Chunk(i, j + 1, k) == Block::EMPTY) { // U	
						CreateUQuad(vertices, center + glm::vec3(i, j, k), { textureCoords[0], textureCoords[1] });
					}
					if (!borderNorth && k == 0 && CheckNorthChunk(northChunk, i, j) || k > 0 && m_Chunk(i, j, k - 1) == Block::EMPTY) { // B
						CreateBQuad(vertices, center + glm::vec3(i, j, k), { textureCoords[2], textureCoords[3] });
					}
					if (!borderSouth && k == m_ZLength - 1 && CheckSouthChunk(southChunk, i, j) || k < m_ZLength - 1 && m_Chunk(i, j, k + 1) == Block::EMPTY) { // F
						CreateFQuad(vertices, center + glm::vec3(i, j, k), { textureCoords[2], textureCoords[3] });
					}
					if (!borderWest && i == 0 && CheckWestChunk(westChunk, k, j) || i > 0 && m_Chunk(i - 1, j, k) == Block::EMPTY) { // L
						CreateLQuad(vertices, center + glm::vec3(i, j, k), { textureCoords[2], textureCoords[3] });
					}
					if (!borderEast && i == m_XLength - 1 && CheckEastChunk(eastChunk, k, j) || i < m_XLength - 1 && m_Chunk(i + 1, j, k) == Block::EMPTY) { // R
						CreateRQuad(vertices, center + glm::vec3(i, j, k), { textureCoords[2], textureCoords[3] });
					}
				}
			}
		}
	}

	m_RenderData = vertices;
}

// there must be a more elegant way of writing this
std::vector<Vertex> Chunk::GetRenderData() const
{
	return m_RenderData;
}

void Chunk::CreateUQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords) {
	float size = 1.0f;

	Vertex v;

	v.Position = { position[0], position[1] + size, position[2] + size };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size, position[1] + size, position[2] + size };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size,  position[1] + size, position[2] };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0],  position[1] + size, position[2] };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);
}

void Chunk::CreateDQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords) {
	float size = 1.0f;

	Vertex v;

	v.Position = { position[0], position[1], position[2] + size };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0], position[1], position[2] };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size,  position[1], position[2] };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size,  position[1], position[2] + size };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);
}

void Chunk::CreateFQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords) {
	float size = 1.0f;

	Vertex v;

	v.Position = { position[0], position[1], position[2] + size };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size, position[1], position[2] + size };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size,  position[1] + size, position[2] + size };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0],  position[1] + size, position[2] + size };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);
}

void Chunk::CreateBQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords) {
	float size = 1.0f;

	Vertex v;

	v.Position = { position[0] + size, position[1], position[2] };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0], position[1], position[2] };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0],  position[1] + size, position[2] };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size,  position[1] + size, position[2] };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);
}

void Chunk::CreateRQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords) {
	float size = 1.0f;

	Vertex v;

	v.Position = { position[0] + size, position[1], position[2] + size };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size, position[1], position[2] };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size,  position[1] + size, position[2] };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0] + size,  position[1] + size, position[2] + size };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);
}

void Chunk::CreateLQuad(std::vector<Vertex>& target, const glm::vec3& position, const std::array<unsigned int, 2>& textureCoords) {
	float size = 1.0f;

	Vertex v;

	v.Position = { position[0], position[1], position[2] };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0], position[1], position[2] + size };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, textureCoords[1] * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0],  position[1] + size, position[2] + size };
	v.TexCoords = { (textureCoords[0] + 1) * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);

	v.Position = { position[0],  position[1] + size, position[2] };
	v.TexCoords = { textureCoords[0] * s_TextureOffset, (textureCoords[1] + 1) * s_TextureOffset };
	target.push_back(v);
}

