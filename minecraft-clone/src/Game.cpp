#include "Game.h"
#include <math.h>
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

cam::Camera Game::camera = glm::vec3(0.0f, 0.0f, 0.0f);
bool Game::s_FlyMode = true;
bool Game::s_Jump = false;
bool Game::s_Ground = false;
bool Game::s_RightButton = false;
bool Game::s_LeftButton = false;

Game::Game() :
	m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 500.0f)),
	m_GameStart(true),
	m_VerticalVelocity(0.0f),
	m_LastChunk({0,0})
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

	m_Renderer.Init();
	m_ChunkManager.InitWorld();
}

void Game::OnRender()
{
	glClearColor(173.0f / 255.0f, 223.0f / 255.0f, 230.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 mvp = m_Proj * camera.GetViewMatrix() * glm::mat4(1.0f);
	m_Renderer.SetMVP(mvp);

	m_ChunkManager.Render(m_Renderer);
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


void Game::CheckCollision(glm::vec3*& playerPos, ChunkCoord currentChunk)
{
	Chunk* chunk = &m_ChunkManager.m_ChunkMap.find(currentChunk)->second;
	std::array<unsigned int, 3> chunkSize = m_ChunkManager.GetChunkSize();
	float playerPosX = playerPos->x - (float)currentChunk.x * chunkSize[0] + (float)(chunkSize[0] + 2) / 2; // +2 because each chunk->has a border that doesn't render
	float playerPosY = playerPos->y + 150;
	float playerPosZ = playerPos->z - (float)currentChunk.z * chunkSize[2] + (float)(chunkSize[2] + 2) / 2;
	bool checked = false;

	if (m_VerticalVelocity >= 0 && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY)) {
		float diff = playerPosY - static_cast<unsigned int>(floor(playerPosY));
		playerPos->y -= diff;
		playerPosY -= diff;
	}
	if (m_VerticalVelocity < 0 && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
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

	if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
		playerPos->z += static_cast<unsigned int>(ceil(playerPosZ - 0.3f)) - playerPosZ + 0.3f; 
		checked = true;
	}
	if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY)) {
		playerPos->z -= playerPosZ + 0.3f - static_cast<unsigned int>(floor(playerPosZ + 0.3f));
		checked = true;
	}
	if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
		playerPos->x += static_cast<unsigned int>(ceil(playerPosX - 0.3f)) - playerPosX + 0.3f; 
		checked = true;
	}
	if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
		(chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
		chunk->GetMatrix()(static_cast<unsigned int>(floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
		playerPos->x -= playerPosX + 0.3f - static_cast<unsigned int>(floor(playerPosX + 0.3f));
		checked = true;
	}

	if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
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
	if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
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
	if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
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
	if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
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

// should also check ray cast for neighboring chunks
void Game::CheckRayCast(glm::vec3*& playerPos, ChunkCoord currentChunk) {
	Chunk* chunk = &m_ChunkManager.m_ChunkMap.find(currentChunk)->second;
	std::array<unsigned int, 3> chunkSize = m_ChunkManager.GetChunkSize();
	float playerPosX = playerPos->x - (float)currentChunk.x * chunkSize[0] + (float)(chunkSize[0] + 2) / 2; // +2 because each chunk->has a border that doesn't render
	float playerPosY = playerPos->y + 150;
	float playerPosZ = playerPos->z - (float)currentChunk.z * chunkSize[2] + (float)(chunkSize[2] + 2) / 2;

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

		if (currentVoxel.x >= 0 && currentVoxel.y >= 0 && currentVoxel.z >= 0 && currentVoxel.x < chunkSize[0] &&
			currentVoxel.y < chunkSize[1] && currentVoxel.z < chunkSize[2] &&
			chunk->GetMatrix()(static_cast<unsigned int>(currentVoxel.x), static_cast<unsigned int>(currentVoxel.y), static_cast<unsigned int>(currentVoxel.z)) != Block::EMPTY) {
			voxelFound = true;
		}
	}

	if (voxelFound && s_LeftButton) {
		chunk->SetMatrix(static_cast<unsigned int>(currentVoxel.x), static_cast<unsigned int>(currentVoxel.y), static_cast<unsigned int>(currentVoxel.z), Block::EMPTY);
		std::cout << "colpito blocco (" << currentVoxel.x << "," << currentVoxel.y << "," << currentVoxel.z << ")\n";
		s_LeftButton = false;
		m_ChunkManager.UpdateChunk(currentChunk);
	}
	else if (voxelFound && s_RightButton) {
		chunk->SetMatrix(static_cast<unsigned int>(currentVoxel.x), static_cast<unsigned int>(currentVoxel.y), static_cast<unsigned int>(currentVoxel.z), Block::STONE);
		s_RightButton = false;
		m_ChunkManager.UpdateChunk(currentChunk);
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
	ChunkCoord currentChunk = { static_cast<int>(round(playerPos->x / m_ChunkManager.GetChunkSize()[0])), static_cast<int>(round(playerPos->z / m_ChunkManager.GetChunkSize()[2]))};

	CheckJump();
	CheckRayCast(playerPos, currentChunk);
	ApplyGravity(playerPos, deltaTime);
	CheckCollision(playerPos, currentChunk);
	
	// update chunks (LOGIC TO BE IMPROVED)
	if (m_LastChunk - currentChunk >= m_ChunkManager.GetViewDistance() / 4) {
		m_ChunkManager.GenerateChunks(*playerPos);
		m_LastChunk = currentChunk;
	}
}



