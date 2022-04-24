#include "Game.h"
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

static std::pair<ChunkCoord, glm::vec3> GlobalToLocal(glm::vec3 playerPosition, unsigned int chunk_size) {
    ChunkCoord chunkCoords = { static_cast<int>(std::round(playerPosition.x / chunk_size)), static_cast<int>(std::round(playerPosition.z / chunk_size))};
    if (playerPosition.x < 0 && (static_cast<unsigned int>(std::floor(playerPosition.x)) + 8) % chunk_size == 0)
        chunkCoords.x += 1;
    if (playerPosition.z < 0 && (static_cast<unsigned int>(std::floor(playerPosition.z)) + 8) % chunk_size == 0)
        chunkCoords.z += 1;
    unsigned int playerPosX = (static_cast<unsigned int>(std::floor(playerPosition.x)) + 8) % chunk_size + 1;
    unsigned int playerPosZ = (static_cast<unsigned int>(std::floor(playerPosition.z)) + 8) % chunk_size + 1;
    glm::vec3 playerPos = { playerPosX, playerPosition.y, playerPosZ};
    return std::make_pair(chunkCoords, playerPos);
}

Game::Game() :
        m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 500.0f)),
        m_GameStart(true),
        m_VerticalVelocity(0.0f),
        m_LastChunk({0,0}),
        m_HoldingBlock(Block::STONE)
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
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.29f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY)) {
        float diff = playerPosY - static_cast<unsigned int>(std::floor(playerPosY));
        playerPos->y -= diff;
        playerPosY -= diff;
    }
    if (m_VerticalVelocity < 0 && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX-0.29f)), static_cast<unsigned int>(floor(playerPosY-1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX+0.29f)), static_cast<unsigned int>(floor(playerPosY-1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.29f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX-0.29f)), static_cast<unsigned int>(floor(playerPosY-1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX+0.29f)), static_cast<unsigned int>(floor(playerPosY-1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.29f))) != Block::EMPTY)) {
        float diff = static_cast<unsigned int>(std::ceil(playerPosY - 1.8)) - playerPosY + 1.8f;
        playerPos->y += diff;
        playerPosY += diff;
        s_Ground = true;
    }
    else {
        s_Ground = false;
    }

    if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY &&
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY &&
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
        playerPos->z += static_cast<unsigned int>(std::ceil(playerPosZ - 0.3f)) - playerPosZ + 0.3f;
        checked = true;
    }
    if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY)) {
        playerPos->z -= playerPosZ + 0.3f - static_cast<unsigned int>(std::floor(playerPosZ + 0.3f));
        checked = true;
    }
    if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
        playerPos->x += static_cast<unsigned int>(std::ceil(playerPosX - 0.3f)) - playerPosX + 0.3f;
        checked = true;
    }
    if (playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY &&
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
        playerPos->x -= playerPosX + 0.3f - static_cast<unsigned int>(std::floor(playerPosX + 0.3f));
        checked = true;
    }

    if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
        float dx = static_cast<unsigned int>(std::ceil(playerPosX - 0.3f)) - playerPosX + 0.3f;
        float dz = static_cast<unsigned int>(std::ceil(playerPosZ - 0.3f)) - playerPosZ + 0.3f;
        if (dx > dz)
            playerPos->z += dz;
        else
            playerPos->x += dx;
        checked = true;
    }
    if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ - 0.3f))) != Block::EMPTY)) {
        float dx = playerPosX + 0.3f - static_cast<unsigned int>(std::floor(playerPosX + 0.3f));
        float dz = static_cast<unsigned int>(std::ceil(playerPosZ - 0.3f)) - playerPosZ + 0.3f;
        if (dx > dz)
            playerPos->z += dz;
        else
            playerPos->x -= dx;
        checked = true;
    }
    if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX - 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY)) {
        float dx = static_cast<unsigned int>(std::ceil(playerPosX - 0.3f)) - playerPosX + 0.3f;
        float dz = playerPosZ + 0.3f - static_cast<unsigned int>(std::floor(playerPosZ + 0.3f));
        if (dx > dz)
            playerPos->z -= dz;
        else
            playerPos->x += dx;
        checked = true;
    }
    if (!checked && playerPosX >= 0.3 && playerPosZ >= 0.3 && playerPosY < chunkSize[1] &&
        (chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY ||
         chunk->GetMatrix()(static_cast<unsigned int>(std::floor(playerPosX + 0.3f)), static_cast<unsigned int>(floor(playerPosY - 1.8f)), static_cast<unsigned int>(floor(playerPosZ + 0.3f))) != Block::EMPTY)) {
        float dx = playerPosX + 0.3f - static_cast<unsigned int>(std::floor(playerPosX + 0.3f));
        float dz = playerPosZ + 0.3f - static_cast<unsigned int>(std::floor(playerPosZ + 0.3f));
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

void Game::CheckRayCast(glm::vec3*& playerPos) {
	unsigned int chunkSize = m_ChunkManager.GetChunkSize()[0];
	glm::vec3 playerDir = camera.GetPlayerDirection();
	float Sx = std::abs(1 / playerDir.x);
	float Sy = std::abs(1 / playerDir.y);
	float Sz = std::abs(1 / playerDir.z);
    float playerPosY = playerPos->y + 150;
	glm::vec3 currentVoxel = { std::floor(playerPos->x), std::floor(playerPosY), std::floor(playerPos->z) };
	glm::vec3 rayLength;
	glm::vec3 step;

	if (playerDir.x < 0) {
		step.x = -1;
		rayLength.x = (playerPos->x - currentVoxel.x) * Sx;
	}
	else {
		step.x = 1;
		rayLength.x = (currentVoxel.x + 1 - playerPos->x) * Sx;
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
		rayLength.z = (playerPos->z - currentVoxel.z) * Sz;
	}
	else {
		step.z = 1;
		rayLength.z = (currentVoxel.z + 1 - playerPos->z) * Sz;
	}

	bool voxelFound = false;
	float maxDistance = 5.0f;
	float distance = 0.0f;
    ChunkCoord targetLocalCoord;
    ChunkCoord previousChunkCoord;
    glm::vec3 targetLocalVoxel;
    glm::vec3 previousLocalVoxel;
    Chunk* targetChunk;
    Chunk* previousChunk;
    glm::vec3 previousVoxel;
	while (!voxelFound && distance < maxDistance) {
        previousVoxel = currentVoxel;
		previousLocalVoxel = targetLocalVoxel;
        previousChunkCoord = targetLocalCoord;
        previousChunk = targetChunk;
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

        std::pair<ChunkCoord, glm::vec3> target = GlobalToLocal(currentVoxel, chunkSize);
        targetLocalCoord = target.first;
        targetLocalVoxel = target.second;
        targetChunk = &m_ChunkManager.m_ChunkMap.find(targetLocalCoord)->second;

		if (currentVoxel.y >= 0 && currentVoxel.y < m_ChunkManager.GetChunkSize()[1] &&
			targetChunk->GetMatrix()(static_cast<unsigned int>(targetLocalVoxel.x), static_cast<unsigned int>(targetLocalVoxel.y), static_cast<unsigned int>(targetLocalVoxel.z)) != Block::EMPTY) {
			voxelFound = true;
		}
	}

    if (voxelFound) {
        if (s_LeftButton) {
            m_HoldingBlock = targetChunk->GetMatrix()(static_cast<unsigned int>(targetLocalVoxel.x),
                                                static_cast<unsigned int>(targetLocalVoxel.y),
                                                static_cast<unsigned int>(targetLocalVoxel.z));
            targetChunk->SetMatrix(static_cast<unsigned int>(targetLocalVoxel.x), static_cast<unsigned int>(targetLocalVoxel.y),
                             static_cast<unsigned int>(targetLocalVoxel.z), Block::EMPTY);
            s_LeftButton = false;
            m_ChunkManager.UpdateChunk(targetLocalCoord);
            // check if the target block is in the chunk border
            if (targetLocalVoxel.x == 1 || targetLocalVoxel.z == 1 || targetLocalVoxel.x == 16 || targetLocalVoxel.z == 16) {
                UpdateNeighbor(currentVoxel, chunkSize, targetLocalCoord, Block::EMPTY);
            }
        } else if (s_RightButton &&
                !(previousVoxel.x == std::floor(playerPos->x) && previousVoxel.y == std::floor(playerPosY) && previousVoxel.z == std::floor(playerPos->z)) &&
                !(previousVoxel.x == std::floor(playerPos->x) && previousVoxel.y == std::floor(playerPosY - 1.5) && previousVoxel.z == std::floor(playerPos->z))) {
            previousChunk->SetMatrix(static_cast<unsigned int>(previousLocalVoxel.x), static_cast<unsigned int>(previousLocalVoxel.y),
                             static_cast<unsigned int>(previousLocalVoxel.z), m_HoldingBlock);
            s_RightButton = false;
            m_ChunkManager.UpdateChunk(previousChunkCoord);
            // check if the target is in the chunk border
            if (previousLocalVoxel.x == 1 || previousLocalVoxel.z == 1 || previousLocalVoxel.x == 16 || previousLocalVoxel.z == 16) {
                UpdateNeighbor(previousVoxel, chunkSize, previousChunkCoord, m_HoldingBlock);
            }
        }
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


// 2 problems: raycasting neighboring chunk, s_LeftButton must become false after X frames otherwise I click in the sky, then I move until I encounter a block and hit it

void Game::OnUpdate(float deltaTime)
{
    bool clear_btn = false;
    if (s_LeftButton || s_RightButton)
        clear_btn = true;

    // move those into the functions
    glm::vec3* playerPos = camera.GetPlayerPosition();
    ChunkCoord currentChunk = { static_cast<int>(std::round(playerPos->x / m_ChunkManager.GetChunkSize()[0])), static_cast<int>(std::round(playerPos->z / m_ChunkManager.GetChunkSize()[2]))};

    CheckJump();
    CheckRayCast(playerPos);
    ApplyGravity(playerPos, deltaTime);
    CheckCollision(playerPos, currentChunk);

    if (clear_btn) {
        if (s_LeftButton)
            s_LeftButton = false;
        if (s_RightButton)
            s_RightButton = false;
    }

    // update chunks (LOGIC TO BE IMPROVED)
    if (m_LastChunk - currentChunk >= m_ChunkManager.GetViewDistance() / 4) {
        m_ChunkManager.GenerateChunks(*playerPos);
        m_LastChunk = currentChunk;
    }
}

// should update only voxels around the mesh and not the whole chunk (write an update function that takes coordinates of specific voxel as input)

void Game::UpdateNeighbor(glm::vec3 currentVoxel, unsigned int chunkSize, ChunkCoord targetLocalCoord, Block block) {
    // take a step of 1 in every direction and update the neighboring chunks found
    currentVoxel.x += 1;
    std::pair<ChunkCoord, glm::vec3> neighbor = GlobalToLocal(currentVoxel, chunkSize);
    if (neighbor.first != targetLocalCoord) {
        Chunk* neighborChunk = &m_ChunkManager.m_ChunkMap.find(neighbor.first)->second;
        neighborChunk->SetMatrix(static_cast<unsigned int>(neighbor.second.x - 1), static_cast<unsigned int>(neighbor.second.y), static_cast<unsigned int>(neighbor.second.z), block);
        m_ChunkManager.UpdateChunk(neighbor.first);
        currentVoxel.x -= 1;
    } else {
        currentVoxel.x -= 2;
        neighbor = GlobalToLocal(currentVoxel, chunkSize);
        if (neighbor.first != targetLocalCoord) {
            Chunk* neighborChunk = &m_ChunkManager.m_ChunkMap.find(neighbor.first)->second;
            neighborChunk->SetMatrix(static_cast<unsigned int>(neighbor.second.x + 1), static_cast<unsigned int>(neighbor.second.y), static_cast<unsigned int>(neighbor.second.z), block);
            m_ChunkManager.UpdateChunk(neighbor.first);
        }
        currentVoxel.x += 1;
    }
    currentVoxel.z += 1;
    neighbor = GlobalToLocal(currentVoxel, chunkSize);
    if (neighbor.first != targetLocalCoord) {
        Chunk* neighborChunk = &m_ChunkManager.m_ChunkMap.find(neighbor.first)->second;
        neighborChunk->SetMatrix(static_cast<unsigned int>(neighbor.second.x), static_cast<unsigned int>(neighbor.second.y), static_cast<unsigned int>(neighbor.second.z - 1), block);
        m_ChunkManager.UpdateChunk(neighbor.first);
        currentVoxel.z -= 1;
    } else {
        currentVoxel.z -= 2;
        neighbor = GlobalToLocal(currentVoxel, chunkSize);
        if (neighbor.first != targetLocalCoord) {
            Chunk* neighborChunk = &m_ChunkManager.m_ChunkMap.find(neighbor.first)->second;
            neighborChunk->SetMatrix(static_cast<unsigned int>(neighbor.second.x), static_cast<unsigned int>(neighbor.second.y), static_cast<unsigned int>(neighbor.second.z + 1), block);
            m_ChunkManager.UpdateChunk(neighbor.first);
        }
        currentVoxel.z += 1;
    }
}
