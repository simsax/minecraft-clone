#include "Game.h"
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
#include <iostream>
#include <chrono>
#include <limits>
#include <algorithm>
#include <set>
#include "Physics.h"

using namespace std::chrono_literals;

cam::Camera Game::camera = glm::vec3(0.0f, 0.0f, 0.0f);
bool Game::s_FlyMode = true;
bool Game::s_Jump = false;
bool Game::s_Ground = false;
bool Game::s_RightButton = false;
bool Game::s_LeftButton = false;

static const float GRAVITY = 35.0f;

static ChunkCoord CalculateChunkCoord(glm::vec3 playerPosition, unsigned int chunk_size) {
    ChunkCoord chunkCoords = { static_cast<int>(std::round(playerPosition.x / chunk_size)),
                               static_cast<int>(std::round(playerPosition.z / chunk_size))};
    if (playerPosition.x < 0 && (static_cast<unsigned int>(std::floor(playerPosition.x)) + 8) % chunk_size == 0)
        chunkCoords.x += 1;
    if (playerPosition.z < 0 && (static_cast<unsigned int>(std::floor(playerPosition.z)) + 8) % chunk_size == 0)
        chunkCoords.z += 1;
    return chunkCoords;
}

static std::pair<ChunkCoord, glm::vec3> GlobalToLocal(glm::vec3 playerPosition, unsigned int chunk_size) {
    ChunkCoord chunkCoords = CalculateChunkCoord(playerPosition, chunk_size);
    unsigned int playerPosX = (static_cast<unsigned int>(std::floor(playerPosition.x)) + 8) % chunk_size + 1;
    unsigned int playerPosZ = (static_cast<unsigned int>(std::floor(playerPosition.z)) + 8) % chunk_size + 1;
    glm::vec3 playerPos = { playerPosX, playerPosition.y, playerPosZ};
    return std::make_pair(chunkCoords, playerPos);
}

Game::Game() :
        m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 600.0f)),
        m_ChunkManager(camera.GetPlayerPosition()),
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
    // spawn player over a block
    camera.GetPlayerPosition()->y += static_cast<float>(m_ChunkManager.SpawnHeight() + 1.6 + 1);
}

void Game::OnUpdate(float deltaTime)
{
    CheckJump();
    CheckRayCast();
    ApplyGravity(deltaTime);
    Move(deltaTime);
    UpdateChunks();
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

bool Game::CalculateCollision(glm::vec3* currentPosition, const glm::vec3& playerSpeed, unsigned int chunkSize) {
    glm::vec3 finalPosition = *currentPosition + playerSpeed;
    int startX, endX, startY, endY, startZ, endZ;
    if (finalPosition.x >= currentPosition->x) {
        startX = std::floor(currentPosition->x - 0.3);
        endX = std::floor(finalPosition.x + 0.3);
    } else {
        startX = std::floor(finalPosition.x - 0.3);
        endX = std::floor(currentPosition->x + 0.3);
    }
    if (finalPosition.y >= currentPosition->y) {
        startY = std::floor(currentPosition->y - 1.6);
        endY = std::floor(finalPosition.y + 0.2);
    } else {
        startY = std::floor(finalPosition.y - 1.6);
        endY = std::floor(currentPosition->y + 0.2);
    }
    if (finalPosition.z >= currentPosition->z) {
        startZ = std::floor(currentPosition->z - 0.3);
        endZ = std::floor(finalPosition.z + 0.3);
    } else {
        startZ = std::floor(finalPosition.z - 0.3);
        endZ = std::floor(currentPosition->z + 0.3);
    }

    Aabb playerBbox = physics::CreatePlayerAabb(*currentPosition);
    for (int i = startX; i <= endX; i++) {
        for (int j = startY; j <= endY; j++) {
            for (int k = startZ; k <= endZ; k++) {
                std::pair<ChunkCoord, glm::vec3> localPos = GlobalToLocal(glm::vec3(i, j, k), chunkSize);
                Chunk *chunk = &m_ChunkManager.m_ChunkMap.find(localPos.first)->second;
                if (chunk->GetMatrix()(static_cast<unsigned int>(localPos.second.x),
                                       static_cast<unsigned int>(localPos.second.y),
                                       static_cast<unsigned int>(localPos.second.z)) != Block::EMPTY) {
                    Aabb blockBbox = physics::CreateBlockAabb(glm::vec3(i, j, k));
                    physics::SnapAabb(playerBbox, blockBbox, playerSpeed, currentPosition);
                    return true;
                }
            }
        }
    }
    return false;
}

void Game::Move(float deltaTime) {
    glm::vec3 *currentPosition = camera.GetPlayerPosition();
    unsigned int chunkSize = m_ChunkManager.GetChunkSize()[0];
    glm::vec3 playerSpeed = camera.GetCameraSpeed() * deltaTime; // the destination in the next frame

    bool collidedx, collidedy, collidedz;
    if (playerSpeed.x < playerSpeed.y && playerSpeed.x < playerSpeed.z) {
        collidedx = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
        if (playerSpeed.y < playerSpeed.z) {
            collidedy = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
            collidedz = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
        } else {
            collidedz = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
            collidedy = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
        }
    } else if (playerSpeed.y < playerSpeed.z) {
        collidedy = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
        if (playerSpeed.x < playerSpeed.z) {
            collidedx = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
            collidedz = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
        } else {
            collidedz = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
            collidedx = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
        }
    } else {
        collidedz = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
        if (playerSpeed.x < playerSpeed.y) {
            collidedx = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
            collidedy = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
        } else {
            collidedy = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
            collidedx = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
        }
    }

    if (collidedx)
        playerSpeed.x = 0;
    if (collidedy)
        playerSpeed.y = 0;
    if (collidedz)
        playerSpeed.z = 0;

    *currentPosition += playerSpeed;

    if (!s_FlyMode) {
        // check if block below me is solid
        std::pair<ChunkCoord, glm::vec3> localPos = GlobalToLocal(*currentPosition, chunkSize);
        Chunk *chunk = &m_ChunkManager.m_ChunkMap.find(localPos.first)->second;
        if (chunk->GetMatrix()(static_cast<unsigned int>(localPos.second.x),
                               static_cast<unsigned int>(std::floor(localPos.second.y - 1.61)),
                               static_cast<unsigned int>(localPos.second.z)) != Block::EMPTY) {
            s_Ground = true;
        } else {
            s_Ground = false;
        }
    }
    if (currentPosition->y < 0)
        currentPosition->y = 0;
    else if (currentPosition->y > m_ChunkManager.GetChunkSize()[1] - 1)
        currentPosition->y = m_ChunkManager.GetChunkSize()[1] - 1;
}

void Game::ApplyGravity(float deltaTime)
{
    if (!s_FlyMode) {
        if (!s_Ground)
            m_VerticalVelocity += -GRAVITY * deltaTime;
        else
            m_VerticalVelocity = 0;
        glm::vec3 velocity = camera.GetCameraSpeed();
        camera.SetCameraSpeed(glm::vec3(velocity.x, velocity.y + m_VerticalVelocity, velocity.z));
    }
}

void Game::CheckRayCast() {
    glm::vec3* playerPos = camera.GetPlayerPosition();
	unsigned int chunkSize = m_ChunkManager.GetChunkSize()[0];
	glm::vec3 playerDir = camera.GetPlayerDirection();
	float Sx = std::abs(1 / playerDir.x);
	float Sy = std::abs(1 / playerDir.y);
	float Sz = std::abs(1 / playerDir.z);
	glm::vec3 currentVoxel = { std::floor(playerPos->x), std::floor(playerPos->y), std::floor(playerPos->z) };
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
		rayLength.y = (playerPos->y - currentVoxel.y) * Sy;
	}
	else {
		step.y = 1;
		rayLength.y = (currentVoxel.y + 1 - playerPos->y) * Sy;
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
    Chunk* targetChunk = nullptr;
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
			targetChunk->GetMatrix()(static_cast<unsigned int>(targetLocalVoxel.x),
                                     static_cast<unsigned int>(targetLocalVoxel.y),
                                     static_cast<unsigned int>(targetLocalVoxel.z)) != Block::EMPTY) {
			voxelFound = true;
		}
	}

    if (voxelFound) {
        if (s_LeftButton) {
            m_HoldingBlock = targetChunk->GetMatrix()(static_cast<unsigned int>(targetLocalVoxel.x),
                                                static_cast<unsigned int>(targetLocalVoxel.y),
                                                static_cast<unsigned int>(targetLocalVoxel.z));
            if (m_HoldingBlock == Block::BEDROCK) {
                m_HoldingBlock = Block::EMPTY;
            } else {
                targetChunk->SetMatrix(static_cast<unsigned int>(targetLocalVoxel.x),
                                       static_cast<unsigned int>(targetLocalVoxel.y),
                                       static_cast<unsigned int>(targetLocalVoxel.z), Block::EMPTY);
                m_ChunkManager.UpdateChunk(targetLocalCoord);
                // check if the target block is in the chunk border
                if (targetLocalVoxel.x == 1 || targetLocalVoxel.z == 1 ||
                    targetLocalVoxel.x == 16 || targetLocalVoxel.z == 16) {
                    UpdateNeighbor(currentVoxel, chunkSize, targetLocalCoord, Block::EMPTY);
                }
            }
        } else if (s_RightButton &&
                !(previousVoxel.x == std::floor(playerPos->x) && previousVoxel.y == std::floor(playerPos->y) &&
                  previousVoxel.z == std::floor(playerPos->z)) && !(previousVoxel.x == std::floor(playerPos->x) &&
                  previousVoxel.y == std::floor(playerPos->y - 1.5) && previousVoxel.z == std::floor(playerPos->z))) {
            previousChunk->SetMatrix(static_cast<unsigned int>(previousLocalVoxel.x),
                                     static_cast<unsigned int>(previousLocalVoxel.y),
                                     static_cast<unsigned int>(previousLocalVoxel.z), m_HoldingBlock);
            m_ChunkManager.UpdateChunk(previousChunkCoord);
            // check if the target is in the chunk border
            if (previousLocalVoxel.x == 1 || previousLocalVoxel.z == 1 ||
                previousLocalVoxel.x == 16 || previousLocalVoxel.z == 16) {
                UpdateNeighbor(previousVoxel, chunkSize, previousChunkCoord, m_HoldingBlock);
            }
        }
    }
    s_LeftButton = false;
    s_RightButton = false;
}


void Game::CheckJump()
{
    if (s_Jump) {
        m_VerticalVelocity += 9.0f;
        s_Jump = false;
        s_Ground = false;
    }
}

void Game::UpdateChunks() {
    glm::vec3* playerPos = camera.GetPlayerPosition();
    ChunkCoord currentChunk = CalculateChunkCoord(*playerPos, m_ChunkManager.GetChunkSize()[0]);
    if (m_LastChunk != currentChunk)
        m_ChunkManager.SetNewChunks();
    if (m_LastChunk - currentChunk >= m_ChunkManager.GetViewDistance() / 4) {
        m_ChunkManager.GenerateChunks();
        m_LastChunk = currentChunk;
    }
}


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

void Game::ProcessKeyboard(const std::array<cam::Key, (int) cam::Key::Key_MAX + 1> &keyPressed) {
    camera.ProcessKeyboard(keyPressed);
}
