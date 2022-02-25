#include "TestTexture2D.h"
#include <GL/glew.h>
#include "GLErrorManager.h"
#include "imgui/imgui.h"
#include "VertexBufferLayout.hpp"
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Renderer.h"

namespace test {

	TestTexture2D::TestTexture2D() :
		m_Proj(glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0)
	{

		float positions[] = {
			// positions	  // texture coords
			-50.0f, -50.0f,		0.0f, 0.0f, // 0
			50.0f, -50.0f,		1.0f, 0.0f, // 1
			50.0f, 50.0f,		1.0f, 1.0f, // 2
			-50.0f, 50.0f,		0.0f, 1.0f, // 3
		};

		// could also used unsigned char or shorts to save memory
		unsigned char indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		// enable blending
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		
		m_VAO = std::make_unique<VertexArray>(); // need to allocate them on the heap otherwise they would be deallocated from gpu once constructor ends (look at destructors implementations)

		m_VertexBuffer = std::make_unique<VertexBuffer>(4 * 4 * sizeof(float), positions);

		// TODO: capire come fa lo shader a capire quali sono le coordinate dei vertici e quali quelle delle texture
		/*
			glVertexAttribPointer setta un attributo e il relativo id. Quell'id è lo stesso che prendo quando faccio layout(location = id), in quel modo
			lo shader riesce a distinguere le posizioni dalle coordinate per le texture, e conoscendo stride e offset riesce a ricavarsi la coordinata corretta successiva
		*/

		VertexBufferLayout layout;
		layout.Push<float>(2); // the vertex are 2-dimensional floats
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, sizeof(indices));

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();

		m_Texture = std::make_unique<Texture>("res/textures/grass_block_side.png");
		m_Shader->SetUniform1i("u_Texture", 0);

	}

	TestTexture2D::~TestTexture2D()
	{
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}

	void TestTexture2D::OnRender(const glm::mat4& viewMatrix)
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture->Bind();

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind(); // if you call bind shader on a shader already bound is redundant, ideally I should implement a caching system to avoid binding a shader already bound
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			m_IndexBuffer->SetCount(12);
			renderer.Draw(*m_VAO, *m_IndexBuffer, GL_UNSIGNED_BYTE, *m_Shader);
		}
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			m_IndexBuffer->SetCount(12);
			renderer.Draw(*m_VAO, *m_IndexBuffer, GL_UNSIGNED_BYTE, *m_Shader);
		}
	};

	void TestTexture2D::OnImGuiRender()
	{
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 640.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 640.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}