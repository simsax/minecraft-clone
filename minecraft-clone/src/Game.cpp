#include "Game.h"
#include <GL/glew.h>
#include "Renderer.h"
#include "VertexBufferLayout.hpp"
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <array>
#include <iostream>
#include "Chunk.h"

// ancora da fixare il costruttore
Game::Game() :
	m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 300.0f)), // remember to increase z distance
	m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0)))
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

	// use a render_chunk function
	Chunk chunk(10, 10, 10);

	VertexBufferLayout layout;
	layout.Push<float>(3); // position
	layout.Push<float>(2); // texture coords

	std::vector<Vertex> buffer = chunk.GetVBOData();
	size_t indexCount = buffer.size()/4 * 6; // num faces * 6

	std::vector<unsigned int> indices;
	indices.reserve(indexCount);
	unsigned int offset = 0;
	for (size_t i = 0; i < indexCount; i += 6) {
		indices.push_back(0 + offset);
		indices.push_back(1 + offset);
		indices.push_back(2 + offset);

		indices.push_back(2 + offset);
		indices.push_back(3 + offset);
		indices.push_back(0 + offset);

		offset += 4;
	}

	m_VBO = std::make_unique<VertexBuffer>();
	//m_VBO->CreateDynamic(sizeof(Vertex) * MaxVertexCount);
	m_VBO->CreateStatic(buffer.size() * sizeof(Vertex), buffer.data());
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, layout);

	m_IBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
	m_IBO->SetCount(indexCount);

	m_Shader = std::make_unique<Shader>("C:/dev/minecraft-clone/minecraft-clone/res/shaders/Basic.shader");
	m_Shader->Bind();

	m_Texture1 = std::make_unique<Texture>("C:/dev/minecraft-clone/minecraft-clone/res/textures/terrain.png");
	m_Texture1->Bind(0);
	m_Shader->SetUniform1i("u_Texture", 0);
	/*
	// tentativi
	VertexBufferLayout layout;
	layout.Push<float>(3); // position
	layout.Push<float>(2); // texture coords

	std::vector<Vertex> vertices;
	vertices.reserve(2);
	std::array<unsigned int, 4> textureCoords = { 12, 3, 3, 15 };
	chunk.CreateUQuad(vertices, glm::vec3(0, 0, 0), { textureCoords[0], textureCoords[1] });

	size_t indexCount = 6; // num faces * 6

	std::array<unsigned int, 6> indices2 = {0,1,2,2,3,0};
	std::vector<unsigned int> indices = { 0,1,2,2,3,0 };

	//indices.reserve(indexCount);
	unsigned int offset = 0;
	for (size_t i = 0; i < 6; i += 6) {
		indices.push_back(0 + offset);
		indices.push_back(1 + offset);
		indices.push_back(2 + offset);

		indices.push_back(2 + offset);
		indices.push_back(3 + offset);
		indices.push_back(0 + offset);

		offset += 4;
	}

	m_VBO = std::make_unique<VertexBuffer>();
	//m_VBO->CreateDynamic(sizeof(Vertex) * MaxVertexCount);
	m_VBO->CreateStatic(vertices.size() * sizeof(Vertex), vertices.data());
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, layout);

	m_IBO = std::make_unique<IndexBuffer>(indices.size() * sizeof(unsigned int), indices.data());
	m_IBO->SetCount(indexCount);

	m_Shader = std::make_unique<Shader>("C:/dev/minecraft-clone/minecraft-clone/res/shaders/Basic.shader");
	m_Shader->Bind();

	m_Texture1 = std::make_unique<Texture>("C:/dev/minecraft-clone/minecraft-clone/res/textures/terrain.png");
	m_Texture1->Bind(0);
	m_Shader->SetUniform1i("u_Texture", 0);*/
}

void Game::OnRender(const glm::mat4& viewMatrix)
{

	/*
	//unsigned short indexCount = 0;

	unsigned int texture_side[2]{ 3, 15 };
	unsigned int texture_top[2]{ 12, 3 };
	unsigned int texture_bottom[2]{ 2, 15 };
	
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};



	std::array<Vertex, 100> vertices;
	Vertex* buffer = vertices.data();
	
	for (int i = 0; i < 10; i++) {
		buffer = CreateCube(buffer, cubePositions[i], texture_offset, texture_side, texture_top, texture_bottom); // grass block
		indexCount += 36;
	}
	m_VBO->SendData(vertices.size() * sizeof(Vertex), vertices.data());
	*/
	glClearColor(173.0f / 255.0f, 223.0f / 255.0f, 230.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer renderer;

	m_Texture1->Bind();

	// position, target, up
	m_View = viewMatrix;
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = m_Proj * m_View * model;
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_MVP", mvp);
	//m_IBO->SetCount(indexCount);
	renderer.Draw(*m_VAO, *m_IBO, GL_UNSIGNED_INT, *m_Shader);
}

// position inside onUpdate??

Game::~Game()
{
}

void Game::OnUpdate(float deltaTime)
{
}