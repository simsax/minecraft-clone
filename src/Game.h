#pragma once

#include "Camera.h"
#include "ChunkManager.h"

class Game
{
public:
	Game();
	void Init();
	void OnUpdate(float deltaTime);
	void OnRender();
	void ProcessMouse(float xoffset, float yoffset);
	std::array<bool, GLFW_KEY_LAST> KeyPressed;

	/* #ifndef  NDEBUG */
	glm::vec3 GetPlayerPosition();
	/* #endif */

private:
	void CheckRayCast();
	void ApplyGravity(float deltaTime);
	void CheckJump();
	void UpdateNeighbor(glm::vec3 currentVoxel, unsigned int chunkSize, ChunkCoord targetLocalCoord, Block block);
	void UpdateChunks();
	void Move(float deltaTime);
	void HandleInput();
	bool CalculateCollision(glm::vec3 *currentPosition, const glm::vec3 &playerSpeed, unsigned int chunkSize);
	std::pair<ChunkCoord, std::array<unsigned int, 3>> GlobalToLocal(const glm::vec3 &playerPosition);

	bool m_Ground;
	bool m_Jump;
	glm::mat4 m_Proj;
	Camera m_Camera;
	Renderer m_Renderer;
	ChunkManager m_ChunkManager;
	ChunkCoord m_LastChunk;
	ChunkCoord m_SortedChunk;
	float m_VerticalVelocity;
	Block m_HoldingBlock;
};