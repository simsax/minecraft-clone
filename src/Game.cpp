#include "Game.h"
#include "Physics.h"
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <set>

#define PLAYER_HALF_WIDTH 0.3f
#define PLAYER_TOP_HEIGHT 0.2f
#define PLAYER_BOTTOM_HEIGHT 1.6f

using namespace std::chrono_literals;

static const float GRAVITY = 37.0f;

static int mod(int a, int b)
{
    int res = a % b;
    return res >= 0 ? res : res + b;
}

std::pair<ChunkCoord, glm::vec3> Game::GlobalToLocal(const glm::vec3& playerPosition)
{
    uint32_t chunkSize = m_ChunkManager.GetChunkSize()[0];
    ChunkCoord chunkCoord = m_ChunkManager.CalculateChunkCoord(playerPosition);
    uint32_t playerPosX = mod(static_cast<int>(std::floor(playerPosition.x) - 1), chunkSize) + 1;
    uint32_t playerPosZ = mod(static_cast<int>(std::floor(playerPosition.z) - 1), chunkSize) + 1;
    glm::vec3 playerPos
        = { playerPosX, static_cast<uint32_t>(std::floor(playerPosition.y)), playerPosZ };
    return std::make_pair(chunkCoord, playerPos);
}

Game::Game()
    : m_KeyPressed({})
    , m_Ground(false)
    , m_Jump(false)
    , m_Camera(glm::vec3(0.0f, 0.0f, 0.0f))
    , m_ChunkManager(&m_Camera)
    , m_VerticalVelocity(0.0f)
    , m_LastChunk({ 0, 0 })
    , m_SortedChunk({ 0, 0 })
    , m_HoldingBlock(Block::STONE)
{
}

void Game::Init()
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
    m_Camera.GetPlayerPosition()->y
        += static_cast<float>(m_ChunkManager.SpawnHeight() + PLAYER_BOTTOM_HEIGHT + 3);
}

void Game::OnUpdate(float deltaTime)
{
    HandleInput();
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

    m_Renderer.SetMVP(m_Camera.GetMVP());
    m_ChunkManager.Render(m_Renderer);
}

void Game::HandleInput()
{
    m_Camera.HandleInput(m_KeyPressed);
    if (m_KeyPressed[GLFW_KEY_F]) {
        m_Camera.ToggleFlyMode();
        m_KeyPressed[GLFW_KEY_F] = false;
    }
    if (m_KeyPressed[GLFW_KEY_SPACE] && m_Ground && !m_Camera.GetFlyMode() && !m_Jump) {
        m_Jump = true;
        m_KeyPressed[GLFW_KEY_SPACE] = false;
    }
}

void Game::ProcessMouse(float xoffset, float yoffset) { m_Camera.ProcessMouse(xoffset, yoffset); }

void Game::PressKey(int key) { m_KeyPressed[key] = true; }

void Game::ReleaseKey(int key) { m_KeyPressed[key] = false; }

glm::vec3 Game::GetPlayerPosition() { return *m_Camera.GetPlayerPosition(); }

bool Game::CalculateCollision(
    glm::vec3* currentPosition, const glm::vec3& playerSpeed, uint32_t chunkSize)
{
    glm::vec3 finalPosition = *currentPosition + playerSpeed;
    int startX, endX, startY, endY, startZ, endZ;
    if (finalPosition.x >= currentPosition->x) {
        startX = std::floor(currentPosition->x - PLAYER_HALF_WIDTH);
        endX = std::floor(finalPosition.x + PLAYER_HALF_WIDTH);
    } else {
        startX = std::floor(finalPosition.x - PLAYER_HALF_WIDTH);
        endX = std::floor(currentPosition->x + PLAYER_HALF_WIDTH);
    }
    if (finalPosition.y >= currentPosition->y) {
        startY = std::floor(currentPosition->y - PLAYER_BOTTOM_HEIGHT);
        endY = std::floor(finalPosition.y + PLAYER_TOP_HEIGHT);
    } else {
        startY = std::floor(finalPosition.y - PLAYER_BOTTOM_HEIGHT);
        endY = std::floor(currentPosition->y + PLAYER_TOP_HEIGHT);
    }
    if (finalPosition.z >= currentPosition->z) {
        startZ = std::floor(currentPosition->z - PLAYER_HALF_WIDTH);
        endZ = std::floor(finalPosition.z + PLAYER_HALF_WIDTH);
    } else {
        startZ = std::floor(finalPosition.z - PLAYER_HALF_WIDTH);
        endZ = std::floor(currentPosition->z + PLAYER_HALF_WIDTH);
    }

    physics::Aabb playerBbox = physics::CreatePlayerAabb(*currentPosition);
    for (int i = startX; i <= endX; i++) {
        for (int j = startY; j <= endY; j++) {
            for (int k = startZ; k <= endZ; k++) {
                std::pair<ChunkCoord, glm::vec3> localPos = GlobalToLocal(glm::vec3(i, j, k));
                if (m_ChunkManager.m_ChunkMap.find(localPos.first)
                    != m_ChunkManager.m_ChunkMap.end()) {
                    Chunk* chunk = &m_ChunkManager.m_ChunkMap.find(localPos.first)->second;
                    if (chunk->GetMatrix()(
                            localPos.second[0], localPos.second[1], localPos.second[2])
                        != Block::EMPTY) {
                        physics::Aabb blockBbox = physics::CreateBlockAabb(glm::vec3(i, j, k));
                        physics::SnapAabb(playerBbox, blockBbox, playerSpeed, currentPosition);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Game::Move(float deltaTime)
{
    glm::vec3* currentPosition = m_Camera.GetPlayerPosition();
    uint32_t chunkSize = m_ChunkManager.GetChunkSize()[0];
    glm::vec3 playerSpeed
        = m_Camera.GetCameraSpeed() * deltaTime; // the destination in the next frame

    bool collidedx, collidedy, collidedz;
    if (playerSpeed.x < playerSpeed.y && playerSpeed.x < playerSpeed.z) {
        collidedx = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
        if (playerSpeed.y < playerSpeed.z) {
            collidedy
                = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
            collidedz
                = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
        } else {
            collidedz
                = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
            collidedy
                = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
        }
    } else if (playerSpeed.y < playerSpeed.z) {
        collidedy = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
        if (playerSpeed.x < playerSpeed.z) {
            collidedx
                = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
            collidedz
                = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
        } else {
            collidedz
                = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
            collidedx
                = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
        }
    } else {
        collidedz = CalculateCollision(currentPosition, glm::vec3(0, 0, playerSpeed.z), chunkSize);
        if (playerSpeed.x < playerSpeed.y) {
            collidedx
                = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
            collidedy
                = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
        } else {
            collidedy
                = CalculateCollision(currentPosition, glm::vec3(0, playerSpeed.y, 0), chunkSize);
            collidedx
                = CalculateCollision(currentPosition, glm::vec3(playerSpeed.x, 0, 0), chunkSize);
        }
    }

    if (collidedx)
        playerSpeed.x = 0;
    if (collidedy) {
        if (!m_Camera.GetFlyMode() && playerSpeed.y < 0) {
            m_Ground = true;
        }
        playerSpeed.y = 0;
    } else {
        m_Ground = false;
    }
    if (collidedz)
        playerSpeed.z = 0;

    *currentPosition += playerSpeed;

    // don't let the player go outside of world borders
    if (currentPosition->y < 0)
        currentPosition->y = 0;
    else if (currentPosition->y > m_ChunkManager.GetChunkSize()[1] - 1)
        currentPosition->y = m_ChunkManager.GetChunkSize()[1] - 1;
}

void Game::ApplyGravity(float deltaTime)
{
    if (!m_Camera.GetFlyMode()) {
        if (!m_Ground)
            m_VerticalVelocity += -GRAVITY * deltaTime;
        else
            m_VerticalVelocity = -GRAVITY * deltaTime;
        glm::vec3 velocity = m_Camera.GetCameraSpeed();
        m_Camera.SetCameraSpeed(glm::vec3(velocity.x, velocity.y + m_VerticalVelocity, velocity.z));
    }
}

void Game::CheckRayCast()
{
    glm::vec3* playerPos = m_Camera.GetPlayerPosition();
    uint32_t chunkSize = m_ChunkManager.GetChunkSize()[0];
    glm::vec3 playerDir = m_Camera.GetPlayerDirection();
    float Sx = std::abs(1 / playerDir.x);
    float Sy = std::abs(1 / playerDir.y);
    float Sz = std::abs(1 / playerDir.z);
    glm::vec3 currentVoxel
        = { std::floor(playerPos->x), std::floor(playerPos->y), std::floor(playerPos->z) };
    glm::vec3 rayLength;
    glm::vec3 step;

    if (playerDir.x < 0) {
        step.x = -1;
        rayLength.x = (playerPos->x - currentVoxel.x) * Sx;
    } else {
        step.x = 1;
        rayLength.x = (currentVoxel.x + 1 - playerPos->x) * Sx;
    }
    if (playerDir.y < 0) {
        step.y = -1;
        rayLength.y = (playerPos->y - currentVoxel.y) * Sy;
    } else {
        step.y = 1;
        rayLength.y = (currentVoxel.y + 1 - playerPos->y) * Sy;
    }
    if (playerDir.z < 0) {
        step.z = -1;
        rayLength.z = (playerPos->z - currentVoxel.z) * Sz;
    } else {
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
    Chunk* previousChunk = nullptr;
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
            } else {
                currentVoxel.z += step.z;
                distance = rayLength.z;
                rayLength.z += Sz;
            }
        } else {
            if (rayLength.y < rayLength.z) {
                currentVoxel.y += step.y;
                distance = rayLength.y;
                rayLength.y += Sy;
            } else {
                currentVoxel.z += step.z;
                distance = rayLength.z;
                rayLength.z += Sz;
            }
        }

        std::pair<ChunkCoord, glm::vec3> target = GlobalToLocal(currentVoxel);
        targetLocalCoord = target.first;
        targetLocalVoxel = target.second;
        if (m_ChunkManager.m_ChunkMap.find(targetLocalCoord) != m_ChunkManager.m_ChunkMap.end()) {
            targetChunk = &m_ChunkManager.m_ChunkMap.find(targetLocalCoord)->second;

            if (currentVoxel.y >= 0 && currentVoxel.y < m_ChunkManager.GetChunkSize()[1]
                && targetChunk->GetMatrix()(
                       targetLocalVoxel[0], targetLocalVoxel[1], targetLocalVoxel[2])
                    != Block::EMPTY) {
                voxelFound = true;
            }
        }
    }

    if (voxelFound) {
        if (m_KeyPressed[GLFW_MOUSE_BUTTON_LEFT]) {
            m_HoldingBlock = targetChunk->GetMatrix()(
                targetLocalVoxel[0], targetLocalVoxel[1], targetLocalVoxel[2]);
            if (m_HoldingBlock == Block::BEDROCK) {
                m_HoldingBlock = Block::EMPTY;
            } else {
                PlaceBlock(Block::EMPTY, targetChunk, targetLocalCoord, targetLocalVoxel,
                    currentVoxel, chunkSize);
            }
            m_KeyPressed[GLFW_MOUSE_BUTTON_LEFT] = false;
        } else if (m_KeyPressed[GLFW_MOUSE_BUTTON_RIGHT] && previousChunk != nullptr
            && !physics::Intersect(
                physics::CreatePlayerAabb(*playerPos), physics::CreateBlockAabb(previousVoxel))) {
            PlaceBlock(m_HoldingBlock, previousChunk, previousChunkCoord, previousLocalVoxel,
                previousVoxel, chunkSize);
            m_KeyPressed[GLFW_MOUSE_BUTTON_RIGHT] = false;
        }
    }
}

void Game::PlaceBlock(Block block, Chunk* chunk, const ChunkCoord& chunkCoord,
    const glm::vec3& localVoxel, const glm::vec3& globalVoxel, uint32_t chunkSize)
{
    chunk->SetMatrix(localVoxel[0], localVoxel[1], localVoxel[2], block);
    m_ChunkManager.UpdateChunk(chunkCoord);
    // check if the target is in the chunk border
    if (localVoxel[0] == 1 || localVoxel[2] == 1 || localVoxel[0] == chunkSize
        || localVoxel[2] == chunkSize) {
        UpdateNeighbor(globalVoxel, chunkSize, chunkCoord, block);
    }
}

void Game::CheckJump()
{
    if (m_Jump) {
        m_VerticalVelocity += 10.0f;
        m_Jump = false;
        m_Ground = false;
    }
}

void Game::UpdateChunks()
{
    ChunkCoord currentChunk = m_ChunkManager.CalculateChunkCoord(*m_Camera.GetPlayerPosition());
    if (m_SortedChunk != currentChunk) {
        m_SortedChunk = currentChunk;
        m_ChunkManager.SetNewChunks();
    }
    if (m_LastChunk - currentChunk >= m_ChunkManager.GetViewDistance() / 3) {
        m_ChunkManager.GenerateChunks();
        m_LastChunk = currentChunk;
    }
}

void Game::UpdateNeighbor(
    glm::vec3 currentVoxel, uint32_t chunkSize, ChunkCoord targetLocalCoord, Block block)
{
    // take a step of 1 in every direction and update the neighboring chunks found
    currentVoxel.x += 1;
    std::pair<ChunkCoord, glm::vec3> neighbor = GlobalToLocal(currentVoxel);
    if (neighbor.first != targetLocalCoord) {
        Chunk* neighborChunk = &m_ChunkManager.m_ChunkMap.find(neighbor.first)->second;
        neighborChunk->SetMatrix(
            neighbor.second[0] - 1, neighbor.second[1], neighbor.second[2], block);
        m_ChunkManager.UpdateChunk(neighbor.first);
        currentVoxel.x -= 1;
    } else {
        currentVoxel.x -= 2;
        neighbor = GlobalToLocal(currentVoxel);
        if (neighbor.first != targetLocalCoord) {
            Chunk* neighborChunk = &m_ChunkManager.m_ChunkMap.find(neighbor.first)->second;
            neighborChunk->SetMatrix(
                neighbor.second[0] + 1, neighbor.second[1], neighbor.second[2], block);
            m_ChunkManager.UpdateChunk(neighbor.first);
        }
        currentVoxel.x += 1;
    }
    currentVoxel.z += 1;
    neighbor = GlobalToLocal(currentVoxel);
    if (neighbor.first != targetLocalCoord) {
        Chunk* neighborChunk = &m_ChunkManager.m_ChunkMap.find(neighbor.first)->second;
        neighborChunk->SetMatrix(
            neighbor.second[0], neighbor.second[1], neighbor.second[2] - 1, block);
        m_ChunkManager.UpdateChunk(neighbor.first);
        currentVoxel.z -= 1;
    } else {
        currentVoxel.z -= 2;
        neighbor = GlobalToLocal(currentVoxel);
        if (neighbor.first != targetLocalCoord) {
            Chunk* neighborChunk = &m_ChunkManager.m_ChunkMap.find(neighbor.first)->second;
            neighborChunk->SetMatrix(
                neighbor.second[0], neighbor.second[1], neighbor.second[2] + 1, block);
            m_ChunkManager.UpdateChunk(neighbor.first);
        }
        currentVoxel.z += 1;
    }
}
