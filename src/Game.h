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
	static cam::Camera camera; // to refactor
    static void ProcessKeyboard(const std::array<cam::Key, static_cast<int>(cam::Key::Key_MAX) + 1>& keyPressed);

private:
	void CheckRayCast();
	void ApplyGravity(float deltaTime);
	void CheckJump();
    void UpdateNeighbor(glm::vec3 currentVoxel, unsigned int chunkSize, ChunkCoord targetLocalCoord, Block block);
    void Move(float deltaTime);
    float CalculateCollision(const glm::vec3& currentPosition, const glm::vec3& playerSpeed, unsigned int chunkSize,
                                   bool& min_collx, bool& min_colly, bool& min_collz);

	static bool s_FlyMode;
	static bool s_Ground;
	static bool s_Jump;
	static bool s_RightButton;
	static bool s_LeftButton;

	glm::mat4 m_Proj;
	Renderer m_Renderer;
	ChunkManager m_ChunkManager;
	ChunkCoord m_LastChunk;
	float m_VerticalVelocity;
	Block m_HoldingBlock;
};