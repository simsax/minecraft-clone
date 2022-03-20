#include "Game.h"
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
bool Game::s_Jump = false;
bool Game::s_Ground = false;
bool Game::s_RightButton = false;
bool Game::s_LeftButton = false;

Game::Game() :
	m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 500.0f)),
	m_LastChunk({0,0}),
	m_ChunkSize(16),
	m_ViewDistance(6),
	m_GameStart(true),
	m_VerticalVelocity(0.0f),
	m_MaxIndexCount(18432), // each cube has 6 faces, each face has 6 indexes
	m_MaxVertexCount(12288) // each cube has 6 faces, each face has 4 vertices
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

	m_VertexLayout.Push<float>(3); // position
	m_VertexLayout.Push<float>(2); // texture coords

	m_ChunksToRender.reserve(static_cast<const unsigned int>(pow(m_ViewDistance * 2 + 1, 2)));

	m_Indices.reserve(m_MaxIndexCount);
	unsigned int offset = 0;
	for (size_t i = 0; i < m_MaxIndexCount * 2; i += 6) {
		m_Indices.push_back(0 + offset);
		m_Indices.push_back(1 + offset);
		m_Indices.push_back(2 + offset);

		m_Indices.push_back(2 + offset);
		m_Indices.push_back(3 + offset);
		m_Indices.push_back(0 + offset);

		offset += 4;
	}

	m_Seed = static_cast<unsigned int>(time(NULL));

	GenerateChunks();
}

void Game::OnRender()
{
	glClearColor(173.0f / 255.0f, 223.0f / 255.0f, 230.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = m_Proj * camera.GetViewMatrix() * model;

	for (auto& chunk : m_ChunksToRender) {
		chunk->Render(mvp);
	}
}

void Game::ProcessKey(cam::Key key)
{
	switch (key)
	{
	case cam::Key::F:
		s_FlyMode = !s_FlyMode;
		camera.SetFlyMode(s_FlyMode);
		break;
	case cam::Key::SPACE:
		if (s_Ground && !s_FlyMode && !s_Jump) {
			s_Jump = true;
		}
		break;
	default:
		break;
	}
}

void Game::ProcessLeftMouseButton()
{
	s_LeftButton = true;
}

void Game::ProcessRightMouseButton()
{
	s_RightButton = true;
}

void Game::GenerateChunks()
{
	auto meshFun = [this]() {
		glm::vec3* playerPos = camera.GetPlayerPosition();
		int size = static_cast<int>(m_ChunkSize);
		int playerPosX = static_cast<int>(round(playerPos->x / size));
		int playerPosZ = static_cast<int>(round(playerPos->z / size));

		m_ChunksToRender.clear();

		// load chunks
		for (int i = -m_ViewDistance + playerPosX; i <= m_ViewDistance + playerPosX; i++) {
			for (int j = -m_ViewDistance + playerPosZ; j <= m_ViewDistance + playerPosZ; j++) {
				ChunkCoord coords = { i, j };
				// check if this chunk hasn't already been generated
				if (Chunk::s_ChunkMap.find(coords) == Chunk::s_ChunkMap.end()) {
					// create new chunk and cache it
					Chunk chunk(m_ChunkSize, m_ChunkSize * 16, m_ChunkSize, glm::vec3(i * size, 0.0, j * size), coords, m_Seed, m_VertexLayout, m_MaxVertexCount, m_Indices);
					chunk.GenerateMesh();
					Chunk::s_ChunkMap.insert({ coords, std::move(chunk) });
				}
				m_ChunksToRender.push_back(&Chunk::s_ChunkMap.find(coords)->second);
			}
		}
	};
	
	meshFun();

}

void Game::CheckCollision(glm::vec3*& playerPos, ChunkCoord currentChunk)
{
	Chunk* chunk = &Chunk::s_ChunkMap.find(currentChunk)->second;
	float playerPosX = playerPos->x - (float)currentChunk.x * m_ChunkSize + (float)(m_ChunkSize + 2) / 2; // +2 because each chunk->has a border that doesn't render
	float playerPosY = playerPos->y + 150;
	float playerPosZ = playerPos->z - (float)currentChunk.z * m_ChunkSize + (float)(m_ChunkSize + 2) / 2;
	bool checked = false;

	if (m_VerticalVelocity >= 0 && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY)) {
		float diff = playerPosY - static_cast<unsigned int>(floor(playerPosY));
		playerPos->y -= diff;
		playerPosY -= diff;
	}
	if (m_VerticalVelocity < 0 && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX-0.29f)), static_cast<unsigned int>(floor(playerPosY-1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
		 chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX+0.29f)), static_cast<unsigned int>(floor(playerPosY-1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
		 chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX-0.29f)), static_cast<unsigned int>(floor(playerPosY-1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY ||
		 chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX+0.29f)), static_cast<unsigned int>(floor(playerPosY-1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY)) {
		float diff = static_cast<unsigned int>(ceil(playerPosY - 1.8)) - playerPosY + 1.8f;
		playerPos->y += diff;
		playerPosY += diff;
		s_Ground = true;
	}
	else {
		s_Ground = false;
	}

	if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
		playerPos->z += static_cast<unsigned int>(ceil(playerPosZ - 0.3f)) - playerPosZ + 0.3f; 
		checked = true;
	}
	if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY)) {
		playerPos->z -= playerPosZ + 0.3f - static_cast<unsigned int>(floor(playerPosZ + 0.3f));
		checked = true;
	}
	if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
		playerPos->x += static_cast<unsigned int>(ceil(playerPosX - 0.3f)) - playerPosX + 0.3f; 
		checked = true;
	}
	if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
		playerPos->x -= playerPosX + 0.3f - static_cast<unsigned int>(floor(playerPosX + 0.3f));
		checked = true;
	}

	if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
		float dx = static_cast<unsigned int>(ceil(playerPosX - 0.3f)) - playerPosX + 0.3f; 
		float dz = static_cast<unsigned int>(ceil(playerPosZ - 0.3f)) - playerPosZ + 0.3f; 
		if (dx > dz)
			playerPos->z += dz;
		else
			playerPos->x += dx;
		checked = true;
	}
	if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
		float dx = playerPosX + 0.3f - static_cast<unsigned int>(floor(playerPosX + 0.3f));
		float dz = static_cast<unsigned int>(ceil(playerPosZ - 0.3f)) - playerPosZ + 0.3f; 
		if (dx > dz)
			playerPos->z += dz;
		else
			playerPos->x -= dx;
		checked = true;
	}
	if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY)) {
		float dx = static_cast<unsigned int>(ceil(playerPosX - 0.3f)) - playerPosX + 0.3f; 
		float dz = playerPosZ + 0.3f - static_cast<unsigned int>(floor(playerPosZ + 0.3f));
		if (dx > dz)
			playerPos->z -= dz;
		else
			playerPos->x += dx;
		checked = true;
	}
	if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < m_ChunkSize * 16 &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY)) {
		float dx = playerPosX + 0.3f - static_cast<unsigned int>(floor(playerPosX + 0.3f));
		float dz = playerPosZ + 0.3f - static_cast<unsigned int>(floor(playerPosZ + 0.3f));
		if (dx > dz)
			playerPos->z -= dz;
		else
			playerPos->x -= dx;
		checked = true;
	}
}

void Game::ApplyGravity(glm::vec3*& playerPos, float deltaTime)
{
	if (!s_FlyMode) {
		float gravity = 35.0f;
		if (!s_Ground)
			m_VerticalVelocity += -gravity * deltaTime;
		else
			m_VerticalVelocity = -gravity * deltaTime;
		playerPos->y += m_VerticalVelocity * deltaTime;
	}
}

void Game::CheckRayCast(glm::vec3*& playerPos, ChunkCoord currentChunk) {
	Chunk* chunk = &Chunk::s_ChunkMap.find(currentChunk)->second;
	float playerPosX = playerPos->x - (float)currentChunk.x * m_ChunkSize + (float)(m_ChunkSize + 2) / 2; // +2 because each chunk has a border that doesn't render
	float playerPosY = playerPos->y + 150;
	float playerPosZ = playerPos->z - (float)currentChunk.z * m_ChunkSize + (float)(m_ChunkSize + 2) / 2;

	glm::vec3 playerDir = camera.GetPlayerDirection();
	float Sx = sqrt(1 + (playerDir.x / playerDir.y) * (playerDir.x / playerDir.y) + (playerDir.x / playerDir.z) * (playerDir.x / playerDir.z));
	float Sy = sqrt(1 + (playerDir.y / playerDir.x) * (playerDir.y / playerDir.x) + (playerDir.y / playerDir.z) * (playerDir.y / playerDir.z));
	float Sz = sqrt(1 + (playerDir.z / playerDir.y) * (playerDir.z / playerDir.y) + (playerDir.z / playerDir.x) * (playerDir.z / playerDir.x));
	glm::vec3 currentVoxel = { floor(playerPosX), floor(playerPosY), floor(playerPosZ) };
	glm::vec3 rayLength;
	glm::vec3 step;

	if (playerDir.x < 0) {
		step.x = -1;
		rayLength.x = (playerPosX - currentVoxel.x) * Sx;
	}
	else {
		step.x = 1;
		rayLength.x = (currentVoxel.x + 1 - playerPosX) * Sx;
	}
	if (playerDir.y < 0) {
		step.y = -1;
		rayLength.y = (playerPosY - currentVoxel.y) * Sy;
	}
	else {
		step.y = 1;
		rayLength.y = (currentVoxel.y + 1 - playerPosY) * Sy;
	}
	if (playerDir.z < 0) {
		step.z = -1;
		rayLength.z = (playerPosZ - currentVoxel.z) * Sz;
	}
	else {
		step.z = 1;
		rayLength.z = (currentVoxel.z + 1 - playerPosZ) * Sz;
	}
	
	bool voxelFound = false;
	float maxDistance = 5.0f;
	float distance = 0.0f;
	while (!voxelFound && distance < maxDistance) {
		// walk
		if (rayLength.x < rayLength.y) {
			if (rayLength.x < rayLength.z) {
				currentVoxel.x += step.x;
				distance = rayLength.x;
				rayLength.x += Sx;
			}
			else {
				currentVoxel.z += step.z;
				distance = rayLength.z;
				rayLength.z += Sz;
			}
		}
		else {
			if (rayLength.y < rayLength.z) {
				currentVoxel.y += step.y;
				distance = rayLength.y;
				rayLength.y += Sy;
			}
			else {
				currentVoxel.z += step.z;
				distance = rayLength.z;
				rayLength.z += Sz;
			}
		}

		if (currentVoxel.x >= 0 && currentVoxel.y >= 0 && currentVoxel.z >= 0 && currentVoxel.x < m_ChunkSize &&
			currentVoxel.y < m_ChunkSize * 16 && currentVoxel.z < m_ChunkSize &&
			chunk->GetMatrix()(static_cast<unsigned int>(currentVoxel.x), static_cast<unsigned int>(currentVoxel.y), static_cast<unsigned int>(currentVoxel.z)) != Block::EMPTY) {
			voxelFound = true;
		}
	}

	if (voxelFound && s_LeftButton) {
		chunk->SetMatrix(static_cast<unsigned int>(currentVoxel.x), static_cast<unsigned int>(currentVoxel.y), static_cast<unsigned int>(currentVoxel.z), Block::EMPTY);
		std::cout << "colpito blocco (" << currentVoxel.x << "," << currentVoxel.y << "," << currentVoxel.z << ")\n";
		s_LeftButton = false;
		// shuld update only the current chunk (use same code as generate chunks, just push back all existing pointers and send data to vbo, it should be fast enough)
		// the setmatrix function has to update only the mesh surrounding the block tho
	}
	else if (voxelFound && s_RightButton) {
		chunk->SetMatrix(static_cast<unsigned int>(currentVoxel.x), static_cast<unsigned int>(currentVoxel.y), static_cast<unsigned int>(currentVoxel.z), Block::STONE);
		s_RightButton = false;
	}
}

void Game::CheckJump()
{
	if (s_Jump) {
		m_VerticalVelocity += 8.7f;
		s_Jump = false;
		s_Ground = false;
	}
}

void Game::OnUpdate(float deltaTime)
{
	// move those into the functions
	glm::vec3* playerPos = camera.GetPlayerPosition();
	ChunkCoord currentChunk = { static_cast<int>(round(playerPos->x / m_ChunkSize)), static_cast<int>(round(playerPos->z / m_ChunkSize)) };

	CheckJump();
	CheckRayCast(playerPos, currentChunk);
	ApplyGravity(playerPos, deltaTime);
	CheckCollision(playerPos, currentChunk);
	
	// update chunks (LOGIC TO BE IMPROVED)
	if (m_LastChunk - currentChunk >= m_ViewDistance / 4) {
		GenerateChunks();
		m_LastChunk = currentChunk;
	}
}



