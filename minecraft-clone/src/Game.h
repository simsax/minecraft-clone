#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>

class Game
{
public:
	Game();
	~Game();
	void OnUpdate(float deltaTime);
	void OnRender(const glm::mat4& viewMatrix);

private:
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IBO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Texture> m_Texture1;
	glm::mat4 m_Proj, m_View;
};

