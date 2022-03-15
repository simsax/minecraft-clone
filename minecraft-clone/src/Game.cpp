#include "Game.h"
#include <GL/glew.h>
#include <math.h>
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
#include <iostream>
#include <chrono>
#include "time.h"

using namespace std::chrono_literals;

cam::Camera Game::camera = glm::vec3(0.0f, 0.0f, 0.0f);
bool Game::s_FlyMode = true;

Game::Game() :
	m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 500.0f)),
	m_LastChunk({0,0}),
	m_ChunkSize(16),
	m_ViewDistance(6),
	m_LoadingChunks(false),
	m_GameStart(true),
	m_FallTime(0),
	m_Ground(false)
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

	m_VertexLayout.Push<float>(3); // position
	m_VertexLayout.Push<float>(2); // texture coords

	// initialize element buffer
	unsigned int maxIndexCount = static_cast<unsigned int>(pow(m_ChunkSize, 2) * pow(2 * m_ViewDistance + 1, 2) * 36); // each cube has 6 faces, each face has 6 indexes
	unsigned int maxVertexCount = static_cast<unsigned int>(pow(m_ChunkSize, 2) * pow(2 * m_ViewDistance + 1, 2) * 24); // each cube has 6 faces, each face has 4 vertices

	std::vector<unsigned int> indices;
	indices.reserve(maxIndexCount);
	unsigned int offset = 0;
	for (size_t i = 0; i < maxIndexCount * 2; i += 6) {
		indices.push_back(0 + offset);
		indices.push_back(1 + offset);
		indices.push_back(2 + offset);

		indices.push_back(2 + offset);
		indices.push_back(3 + offset);
		indices.push_back(0 + offset);

		offset += 4;
	}

	m_IBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
	m_VBO = std::make_unique<VertexBuffer>();
	m_VBO->CreateDynamic(sizeof(Vertex) * maxVertexCount);
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, m_VertexLayout);
	
	m_Seed = static_cast<unsigned int>(time(NULL));

	GenerateChunks();
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

void Game::GenerateChunks()
{
	auto meshFun = [this]() {
		glm::vec3* playerPos = camera.GetPlayerPosition();
		int size = static_cast<int>(m_ChunkSize);
		int playerPosX = static_cast<int>(round(playerPos->x / size));
		int playerPosZ = static_cast<int>(round(playerPos->z / size));

		std::vector<Vertex> buffer;
		buffer.reserve(692224); // 16*16*8*169*2 is the max num of vertices I can see for a mesh

		std::vector<Chunk*> chunksToRender;
		chunksToRender.reserve(static_cast<const unsigned int>(pow(m_ViewDistance * 2 + 1, 2)));

		// load chunks
		for (int i = -m_ViewDistance + playerPosX; i <= m_ViewDistance + playerPosX; i++) {
			for (int j = -m_ViewDistance + playerPosZ; j <= m_ViewDistance + playerPosZ; j++) {
				ChunkCoord coords = { i, j };
				// check if this chunk hasn't already been generated
				if (Chunk::s_ChunkMap.find(coords) == Chunk::s_ChunkMap.end()) {
					// create new chunk and cache it
					Chunk chunk(m_ChunkSize, m_ChunkSize * 16, m_ChunkSize, glm::vec3(i * size, 0.0, j * size), coords, m_Seed);
					Chunk::s_ChunkMap.insert({ coords, std::move(chunk) });
				}
				chunksToRender.push_back(&Chunk::s_ChunkMap.find(coords)->second);
			}
		}

		// render chunks
		for (Chunk* chunk : chunksToRender) {
			std::vector<Vertex> mesh = chunk->GetMesh();
			buffer.insert(buffer.end(), std::make_move_iterator(mesh.begin()), std::make_move_iterator(mesh.end()));
		}

		return buffer;
	};
	
	if (!m_LoadingChunks) {
		m_BufferFut = std::async(meshFun);
		m_LoadingChunks = true;
	}

	if (m_GameStart || m_BufferFut.wait_for(0.5ms) == std::future_status::ready) {
		if (m_GameStart)
			m_GameStart = false;

		std::vector<Vertex> buffer = m_BufferFut.get();
		size_t indexCount = buffer.size() / 4 * 6; // num faces * 6
		m_VBO->SendData(buffer.size() * sizeof(Vertex), buffer.data());
		m_IBO->SetCount(indexCount);
		m_LoadingChunks = false;
	}
}

void Game::CheckCollision(glm::vec3*& playerPos, ChunkCoord currentChunk)
{
	Chunk chunk = Chunk::s_ChunkMap.find(currentChunk)->second;
	float playerPosX = playerPos->x - (float)currentChunk.x * m_ChunkSize + (float)(m_ChunkSize+2) / 2; // +2 because each chunk has a border that doesn't render
	float playerPosY = playerPos->y + 150;
	float playerPosZ = playerPos->z - (float)currentChunk.z * m_ChunkSize + (float)(m_ChunkSize+2) / 2;
	
	std::cout << "position (" << static_cast<unsigned int>(floor(playerPosX)) << "," << static_cast<unsigned int>(floor(playerPosY)) << "," << static_cast<unsigned int>(floor(playerPosZ)) << ")\r";
	if (playerPosY < m_ChunkSize * 16 && (chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX+0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ))) != Block::EMPTY || chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY-1)), static_cast<unsigned int>(floor(playerPosZ))) != Block::EMPTY))
		playerPos->x -= playerPosX + 0.3f - static_cast<unsigned int>(floor(playerPosX + 0.3f));
	if (playerPosX >= 0.3 && playerPosY < m_ChunkSize * 16 && (chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ))) != Block::EMPTY || chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY-1)), static_cast<unsigned int>(floor(playerPosZ))) != Block::EMPTY))
		playerPos->x += static_cast<unsigned int>(ceil(playerPosX - 0.3f)) - playerPosX + 0.3f;
	if (playerPosY < m_ChunkSize * 16 && (chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY || chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX)), static_cast<unsigned int>(floor(playerPosY-1)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY))
		playerPos->z -= playerPosZ + 0.3f - static_cast<unsigned int>(floor(playerPosZ + 0.3f));
	if (playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 && (chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY || chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX)), static_cast<unsigned int>(floor(playerPosY-1)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY))
		playerPos->z += static_cast<unsigned int>(ceil(playerPosZ - 0.3f)) - playerPosZ + 0.3f;
	if (playerPosY < m_ChunkSize * 16 && chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ))) != Block::EMPTY)
		playerPos->y -= playerPosY - static_cast<unsigned int>(floor(playerPosY));
	if (playerPosY < m_ChunkSize * 16 && chunk.GetMatrix()(static_cast<unsigned int>(floor(playerPosX)), static_cast<unsigned int>(floor(playerPosY - 1.8)), static_cast<unsigned int>(floor(playerPosZ))) != Block::EMPTY) {
		playerPos->y += static_cast<unsigned int>(ceil(playerPosY - 1.8)) - playerPosY + 1.8f;
		m_Ground = true;
		m_FallTime = 0;
	}
	else {
		m_Ground = false;
	}
}

void Game::Gravity(glm::vec3*& playerPos, float deltaTime)
{
	if (!m_Ground)
		playerPos->y -= 0.5f * 0.981f * deltaTime * deltaTime;
}

Game::~Game()
{
}

void Game::OnUpdate(float deltaTime)
{
	glm::vec3* playerPos = camera.GetPlayerPosition();
	ChunkCoord currentChunk = { static_cast<int>(round(playerPos->x / m_ChunkSize)), static_cast<int>(round(playerPos->z / m_ChunkSize)) };

	if (!s_FlyMode) {
		m_FallTime += deltaTime;
		Gravity(playerPos, m_FallTime);
	}
	else {
		m_FallTime = 0;
	}
	CheckCollision(playerPos, currentChunk);

	// update chunks (LOGIC TO BE IMPROVED)
	if (m_LastChunk - currentChunk >= m_ViewDistance / 4 || m_LoadingChunks) {
		GenerateChunks();
		m_LastChunk = currentChunk;
	}
}



