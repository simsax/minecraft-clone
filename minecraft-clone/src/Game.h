#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
};

class Game
{
public:
	Game();
	~Game();
	void OnUpdate(float deltaTime);
	void OnRender(const glm::mat4& viewMatrix);

private:
	Vertex* CreateQuad(Vertex* target, const glm::vec3& position, float texture_offset, unsigned int texture[2]);
	Vertex* CreateCube(Vertex* target, const glm::vec3& position, float texture_offset, unsigned int texture_side[2], unsigned int texture_top[2], unsigned int texture_bottom[2]);

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Texture> m_Texture1;
	glm::mat4 m_Proj, m_View;
	//Renderer m_Renderer;
};

