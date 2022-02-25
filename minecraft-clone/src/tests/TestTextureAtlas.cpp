#include "TestTextureAtlas.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLErrorManager.h"
#include "imgui/imgui.h"
#include "VertexBufferLayout.hpp"
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Renderer.h"

namespace test {

	TestTextureAtlas::TestTextureAtlas() :
		m_Proj(glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(320, 240, 0.0f)))
	{
		float offset = 0.125f;

		float positions[] = {
			// positions			// texture coords					// colors				
			-100.0f, -100.0f,		4 * offset, 1.0f - 1 * offset,		0.18f, 0.6f, 0.96f, 1.0f,
			 100.0f, -100.0f,		5 * offset, 1.0f - 1 * offset,		0.18f, 0.6f, 0.96f, 1.0f,
			 100.0f,  100.0f,		5 * offset, 1.0f - 0 * offset,		0.18f, 0.6f, 0.96f, 1.0f,
			-100.0f,  100.0f,		4 * offset, 1.0f - 0 * offset,		0.18f, 0.6f, 0.96f, 1.0f,

			 101.0f, -100.0f,		0 * offset, 1.0f - 1 * offset,		1.0f, 0.93f, 0.24f, 1.0f,
			 301.0f, -100.0f,		1 * offset, 1.0f - 1 * offset,		1.0f, 0.93f, 0.24f, 1.0f,
			 301.0f,  100.0f,		1 * offset, 1.0f - 0 * offset,		1.0f, 0.93f, 0.24f, 1.0f,
			 101.0f,  100.0f,		0 * offset, 1.0f - 0 * offset,		1.0f, 0.93f, 0.24f, 1.0f
		};

		// could also used unsigned char or shorts to save memory
		unsigned char indices[] = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		// enable blending
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		
		m_VAO = std::make_unique<VertexArray>();

		m_VertexBuffer = std::make_unique<VertexBuffer>(sizeof(positions), positions);

		VertexBufferLayout layout;
		layout.Push<float>(2); // position
		layout.Push<float>(2); // texture coords
		layout.Push<float>(4); // colors
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, sizeof(indices));

		m_Shader = std::make_unique<Shader>("res/shaders/TextureAtlas.shader");
		m_Shader->Bind();

		m_Texture1 = std::make_unique<Texture>("res/textures/TextureAtlas.png");
		m_Texture1->Bind(0);
		m_Shader->SetUniform1i("u_Texture", 0);



	}

	TestTextureAtlas::~TestTextureAtlas()
	{
	}

	void TestTextureAtlas::OnUpdate(float deltaTime)
	{
	}

	void TestTextureAtlas::OnRender(const glm::mat4& viewMatrix)
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture1->Bind();

		/*glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(320, 240, 0));
		const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius * 0.01;
		float camZ = cos(glfwGetTime()) * radius * 0.01;
		m_View = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));*/
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		m_View = glm::translate(glm::mat4(1.0f), glm::vec3(320, 240, 0.0f));
		m_Proj = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);
		glm::mat4 mvp = m_Proj * m_View * model;
		m_Shader->Bind(); 
		m_Shader->SetUniformMat4f("u_MVP", mvp);
		m_IndexBuffer->SetCount(12);
		renderer.Draw(*m_VAO, *m_IndexBuffer, GL_UNSIGNED_BYTE, *m_Shader);
	};

	void TestTextureAtlas::OnImGuiRender()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}