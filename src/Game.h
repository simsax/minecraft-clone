#pragma once

#include "Camera.h"
#include "ChunkManager.h"

class Game
{
public:
	Game();
	void OnUpdate(float deltaTime);
	void OnRender();
	static void ProcessKey(cam::Key key);
	static void ProcessLeftMouseButton();
	static void ProcessRightMouseButton();
	static cam::Camera camera;
	 
private:
	void CheckRayCast(glm::vec3*& playerPos);
	void CheckCollision(glm::vec3*& playerPos, ChunkCoord currentChunk);
	void ApplyGravity(glm::vec3*& playerPos, float deltaTime);
	void CheckJump();
    void UpdateNeighbor(glm::vec3 currentVoxel, unsigned int chunkSize, ChunkCoord targetLocalCoord, Block block);

	static bool s_FlyMode;
	static bool s_Ground;
	static bool s_Jump;
	static bool s_RightButton;
	static bool s_LeftButton;

	glm::mat4 m_Proj;
	Renderer m_Renderer;
	ChunkManager m_ChunkManager;
	bool m_GameStart;
	ChunkCoord m_LastChunk;
	float m_VerticalVelocity;
	Block m_HoldingBlock;
};