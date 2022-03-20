#pragma once

#include "Camera.h"
#include "Chunk.h"

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
	struct bbox {
		float width;
		float height;
	};

	void GenerateChunks();
	void CheckRayCast(glm::vec3*& playerPos, ChunkCoord currentChunk);
	void CheckCollision(glm::vec3*& playerPos, ChunkCoord currentChunk);
	void ApplyGravity(glm::vec3*& playerPos, float deltaTime);
	void CheckJump();

	VertexBufferLayout m_VertexLayout;
	
	static bool s_FlyMode;
	static bool s_Ground;
	static bool s_Jump;
	static bool s_RightButton;
	static bool s_LeftButton;
	glm::mat4 m_Proj;
	ChunkCoord m_LastChunk;
	unsigned int m_ChunkSize; // refactor later
	int m_ViewDistance;
	bool m_GameStart;
	float m_VerticalVelocity;
	unsigned int m_MaxIndexCount;
	unsigned int m_MaxVertexCount;
	std::vector<unsigned int> m_Indices;
	std::vector<Chunk*> m_ChunksToRender;
	unsigned int m_Seed;
};