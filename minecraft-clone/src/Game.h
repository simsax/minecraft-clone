#pragma once

#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Chunk.h"

struct ChunkCoord {
	int x;
	int y;
};

struct hash_fn {
	std::size_t operator()(const ChunkCoord& coord) const;
};

bool operator==(const ChunkCoord& l, const ChunkCoord& r);

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
	std::unordered_map<ChunkCoord, Chunk, hash_fn> m_ChunkMap;
	ChunkCoord m_LastChunk;
	unsigned int m_ChunkSize; // refactor later
};

//3d noise, terrain generation and octaves can probably be calculated in different threads