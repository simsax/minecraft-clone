#pragma once

#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Chunk.h"

class Game
{
public:
	Game();
	~Game();
	void OnUpdate(float deltaTime);
	void OnRender();
	static cam::Camera camera;
private:
	void GenerateChunks();

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IBO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Texture> m_Texture1;
	
	glm::mat4 m_Proj;
	Renderer m_Renderer;
	ChunkCoord m_LastChunk;
	unsigned int m_ChunkSize; // refactor later
	VertexBufferLayout m_VertexLayout;
	int m_ViewDistance;
};

//3d noise, terrain generation and octaves can probably be calculated in different threads