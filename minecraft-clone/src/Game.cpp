#include "Game.h"
#include <GL/glew.h>
#include <math.h>
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
#include <iostream>
#include <thread>

cam::Camera Game::camera = glm::vec3(0.0f, 0.0f, 0.0f);

Game::Game() :
	m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 300.0f)),
	m_LastChunk({0,0}),
	m_ChunkSize(16),
	m_ViewDistance(6)
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
	glm::vec3 playerPos = camera.GetPlayerPosition();
	int size = static_cast<int>(m_ChunkSize);
	int playerPosX = static_cast<int>(round(playerPos.x / size));
	int playerPosZ = static_cast<int>(round(playerPos.z / size));

	std::vector<Vertex> buffer;
	buffer.reserve(131072); // 16*16*64*8 is the max num of vertices I can have for a mesh

	// se uso una m_ViewDistance fissa posso sostituire un array qui
	std::vector<Chunk*> chunksToRender;
	chunksToRender.reserve(static_cast<const unsigned int>(pow(m_ViewDistance * 2 + 1, 2)));

	// load chunks
	for (int i = -m_ViewDistance + playerPosX; i <= m_ViewDistance + playerPosX; i++) {
		for (int j = -m_ViewDistance + playerPosZ; j <= m_ViewDistance + playerPosZ; j++) {
			ChunkCoord coords = { i, j };
			// check if this chunk hasn't already been generated
			if (Chunk::s_ChunkMap.find(coords) == Chunk::s_ChunkMap.end()) {
				// create new chunk and cache it 
				Chunk chunk(m_ChunkSize, m_ChunkSize*4, m_ChunkSize, glm::vec3(i * size, 0.0, j * size), coords);
				Chunk::s_ChunkMap.insert({ coords, std::move(chunk) });
			}
			chunksToRender.push_back(&Chunk::s_ChunkMap.find(coords)->second);
		}
	}
	
	// render chunks
	for (Chunk* chunk : chunksToRender) {
		chunk->GenerateMesh(buffer);
	}
	
	size_t indexCount = buffer.size() / 4 * 6; // num faces * 6

	m_VBO = std::make_unique<VertexBuffer>();
	//m_VBO->CreateDynamic(sizeof(Vertex) * MaxVertexCount);
	m_VBO->CreateStatic(buffer.size() * sizeof(Vertex), buffer.data());
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, m_VertexLayout);
	m_IBO->SetCount(indexCount);
}

Game::~Game()
{
}

void Game::OnUpdate(float deltaTime)
{
	// update chunks
	glm::vec3 playerPos = camera.GetPlayerPosition();
	
	ChunkCoord currentChunk = { static_cast<int>(round(playerPos.x / m_ChunkSize)), static_cast<int>(round(playerPos.z / m_ChunkSize)) };
	if (m_LastChunk - currentChunk >= m_ViewDistance / 3) {
		//std::thread render(&Game::GenerateChunks, this);
		//render.detach();
		GenerateChunks();
		m_LastChunk = currentChunk;
	}

}

