#pragma once

#include "Camera.h"
#include "ChunkManager.h"
#include "SpriteRenderer.h"

class Game {
public:
    Game();
    void Init();
    void OnUpdate(float deltaTime);
    void OnRender();
    void ProcessMouse(float xoffset, float yoffset);
    void PressKey(int key);
    void ReleaseKey(int key);

    /* #ifndef  NDEBUG */
    glm::vec3 GetPlayerPosition();
    /* #endif */

private:
    void CheckRayCast();
    void ApplyGravity(float deltaTime);
    void CheckJump();
    void UpdateNeighbor(
        glm::vec3 currentVoxel, uint32_t chunkSize, ChunkCoord targetLocalCoord, Block block);
    void UpdateChunks();
    void Move(float deltaTime);
    void HandleInput();
    bool CalculateCollision(
        glm::vec3* currentPosition, const glm::vec3& playerSpeed, uint32_t chunkSize);
    std::pair<ChunkCoord, glm::vec3> GlobalToLocal(const glm::vec3& playerPosition);
    void PlaceBlock(Block block, Chunk* chunk, const ChunkCoord& chunkCoord,
        const glm::vec3& localVoxel, const glm::vec3& globalVoxel, uint32_t chunkSize);

    std::array<bool, GLFW_KEY_LAST> m_KeyPressed;
    bool m_Ground;
    bool m_Jump;
    glm::mat4 m_Proj;
    Camera m_Camera;
    Renderer m_Renderer;
    SpriteRenderer m_SpriteRenderer;
    ChunkManager m_ChunkManager;
    ChunkCoord m_LastChunk;
    ChunkCoord m_SortedChunk;
    float m_VerticalVelocity;
    Block m_HoldingBlock;
    glm::vec3 m_SkyColor;
};