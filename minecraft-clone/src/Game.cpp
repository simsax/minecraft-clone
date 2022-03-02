#include "Game.h"
#include <GL/glew.h>
#include "VertexBufferLayout.hpp"
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
#include <iostream>

cam::Camera Game::camera = glm::vec3(0.0f, 0.0f, 0.0f);

bool operator==(const ChunkCoord& l, const ChunkCoord& r)
{
	return l.x == r.x && l.y == r.y;
}

bool operator!=(const ChunkCoord& l, const ChunkCoord& r)
{
	return l.x != r.x || l.y != r.y;
}

std::size_t hash_fn::operator()(const ChunkCoord& coord) const
{
	std::size_t h1 = std::hash<int>()(coord.x);
	std::size_t h2 = std::hash<int>()(coord.y);
	return h1 ^ h2;
}

Game::Game() :
	m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 300.0f)), // remember to fine tune zFar
	m_LastChunk({0,0}),
	m_ChunkSize(16)
{
	// depth testing
	glEnable(GL_DEPTH_TEST);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	m_Shader = std::make_unique<Shader>("C:/dev/minecraft-clone/minecraft-clone/res/shaders/Basic.shader");
	m_Shader->Bind();

	m_Texture1 = std::make_unique<Texture>("C:/dev/minecraft-clone/minecraft-clone/res/textures/terrain.png");
	m_Texture1->Bind(0);
	m_Shader->SetUniform1i("u_Texture", 0);

	GenerateChunks(4);
}

void Game::OnRender()
{
	glClearColor(173.0f / 255.0f, 223.0f / 255.0f, 230.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//m_Texture1->Bind(); // I need to bind texture and shaders if I use different textures or shaders in my code
	//m_Shader->Bind();

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = m_Proj * camera.GetViewMatrix() * model;
	m_Shader->SetUniformMat4f("u_MVP", mvp);

	m_Renderer.Draw(*m_VAO, *m_IBO, GL_UNSIGNED_INT, *m_Shader);
}

// then as I move I track the player and when needed I update the chunk hashmap, saving the old ones on the disk.

void Game::GenerateChunks(unsigned int n)
{
	glm::vec3 playerPos = camera.GetPlayerPosition() * glm::vec3(1.0, 0.0, 1.0); // zero the y-coord
	glm::vec3 north = playerPos + glm::vec3(0.0, 0.0, -16.0);
	glm::vec3 northEast = playerPos + glm::vec3(16.0, 0.0, -16.0);
	glm::vec3 east = playerPos + glm::vec3(16.0, 0.0, 0.0);
	glm::vec3 southEast = playerPos + glm::vec3(16.0, 0.0, 16.0);
	glm::vec3 south = playerPos + glm::vec3(0.0, 0.0, 16.0);
	glm::vec3 southWest = playerPos + glm::vec3(-16.0, 0.0, 16.0);
	glm::vec3 west = playerPos + glm::vec3(-16.0, 0.0, 0.0);
	glm::vec3 northWest = playerPos + glm::vec3(-16.0, 0.0, -16.0);

	std::array<glm::vec3, 9> positions = { playerPos, north, northEast, east, southEast, south, southWest, west, northWest };

	VertexBufferLayout layout;
	layout.Push<float>(3); // position
	layout.Push<float>(2); // texture coords

	std::vector<Vertex> buffer;
	buffer.reserve(4096);

	for (auto& position : positions) {
		std::vector<Vertex> chunkData;
		
		int xCoord = static_cast<int>(position.x / m_ChunkSize);
		int zCoord = static_cast<int>(position.z / m_ChunkSize);

		ChunkCoord coords = { xCoord, zCoord };
		// check if this chunk has already been generated
		if (m_ChunkMap.find(coords) != m_ChunkMap.end()) {
			chunkData = m_ChunkMap.find(coords)->second.GetRenderData();
		}
		else { // create new chunk and cache it 
			Chunk chunk(m_ChunkSize, m_ChunkSize, m_ChunkSize, position);
			chunkData = chunk.GetRenderData();
			m_ChunkMap.insert({coords, chunk});
		}

		buffer.insert(buffer.end(), std::make_move_iterator(chunkData.begin()), std::make_move_iterator(chunkData.end()));
	}

	size_t indexCount = buffer.size() / 4 * 6; // num faces * 6

	std::vector<unsigned int> indices;
	indices.reserve(indexCount);
	unsigned int offset = 0;
	for (size_t i = 0; i < indexCount; i += 6) {
		indices.push_back(0 + offset);
		indices.push_back(1 + offset);
		indices.push_back(2 + offset);

		indices.push_back(2 + offset);
		indices.push_back(3 + offset);
		indices.push_back(0 + offset);

		offset += 4;
	}

	m_VBO = std::make_unique<VertexBuffer>();
	//m_VBO->CreateDynamic(sizeof(Vertex) * MaxVertexCount);
	m_VBO->CreateStatic(buffer.size() * sizeof(Vertex), buffer.data());
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, layout);

	m_IBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
	m_IBO->SetCount(indexCount);
}

Game::~Game()
{
}

void Game::OnUpdate(float deltaTime)
{
	glm::vec3 playerPos = camera.GetPlayerPosition();

	// should do this with based on some length probably
	ChunkCoord currentChunk = { static_cast<int>(playerPos.x / m_ChunkSize), static_cast<int>(playerPos.z / m_ChunkSize) };
	if (m_LastChunk != currentChunk) {
		std::cout << "Generating new chunk\n";
		GenerateChunks(4);
		m_LastChunk = currentChunk;
	}

}

