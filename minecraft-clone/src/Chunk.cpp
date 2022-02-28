#include "Chunk.h"
#include "glm/glm.hpp"

const float Chunk::s_TextureOffset = 0.0625f; // texture_size/atlas_size

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

Chunk::Chunk(int xLength, int yLength, int zLength) : m_XLength(xLength), m_YLength(yLength), m_ZLength(zLength)
{
	m_Chunk = std::vector<std::vector<std::vector<Block>>>(xLength, std::vector<std::vector<Block>>(yLength, std::vector<Block>(zLength, Block::DIAMOND))); // for now initialized with grass blocks only
}

Chunk::~Chunk()
{
}

// there must be a more elegant way of writing this
// also don't do this computation everytime for every chunk
// to allow for a dynamic world I probably just run this computation on blocks near to my player
std::vector<Vertex> Chunk::GetVBOData()
{
	std::vector<Vertex> vertices;
	vertices.reserve(4096);
		
	for (int i = 0; i < m_XLength; i++) {
		for (int j = 0; j < m_YLength; j++) {
			for (int k = 0; k < m_ZLength; k++) {
				std::array<unsigned int, 6> textureCoords = s_TextureMap[m_Chunk[i][j][k]];
				// probably want to avoid first condition with multiple chunks
				if (j == 0 || j > 0 && m_Chunk[i][j - 1][k] == Block::EMPTY) { // D
					CreateDQuad(vertices, glm::vec3(i, j, k), {textureCoords[4], textureCoords[5]});
				}
				if (j == m_YLength - 1 || j < m_YLength - 1 && m_Chunk[i][j + 1][k] == Block::EMPTY) { // U	
					CreateUQuad(vertices, glm::vec3(i, j, k), { textureCoords[0], textureCoords[1] });
				}
				if (k == 0 || k > 0 && m_Chunk[i][j][k - 1] == Block::EMPTY) { // B
					CreateBQuad(vertices, glm::vec3(i, j, k), { textureCoords[2], textureCoords[3] });
				}
				if (k == m_ZLength - 1 || k < m_ZLength - 1 && m_Chunk[i][j][k + 1] == Block::EMPTY) { // F
					CreateFQuad(vertices, glm::vec3(i, j, k), { textureCoords[2], textureCoords[3] });
				}
				if (i == 0 || i > 0 && m_Chunk[i - 1][j][k] == Block::EMPTY) { // L
					CreateLQuad(vertices, glm::vec3(i, j, k), { textureCoords[2], textureCoords[3] });
				}
				if (i == m_XLength - 1 || i < m_XLength - 1 && m_Chunk[i + 1][j][k] == Block::EMPTY) { // R
					CreateRQuad(vertices, glm::vec3(i, j, k), { textureCoords[2], textureCoords[3] });
				}
			}
		}
	}

	return vertices;
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

