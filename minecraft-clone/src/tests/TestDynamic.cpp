#include "TestDynamic.h"
#include <GL/glew.h>
#include "GLErrorManager.h"
#include "imgui/imgui.h"
#include "VertexBufferLayout.hpp"
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Renderer.h"
#include <array>

namespace test {

	TestDynamic::TestDynamic() :
		m_Proj(glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(320, 240, 0)))
	{

		// wireframe
		//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

		const size_t MaxQuadCount = 1000;
		const size_t MaxVertexCount = MaxQuadCount * 4;
		const size_t MaxIndexCount = MaxQuadCount * 6;

		//unsigned short indices[] = {
		//	0, 1, 2, 2, 3, 0,
		//	4, 5, 6, 6, 7, 4
		//};

		unsigned short indices[MaxIndexCount];
		unsigned short offset = 0;
		for (size_t i = 0; i < MaxIndexCount; i += 6) {
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		// enable blending
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		
		m_VAO = std::make_unique<VertexArray>();

		m_VertexBuffer = std::make_unique<VertexBuffer>(sizeof(Vertex) * MaxVertexCount, nullptr);

		// this is weird and to be fixed (offsetof) - put everything in the vertexbuffer class
		VertexBufferLayout layout;
		layout.Push<float>(3); // position
		layout.Push<float>(2); // texture coords
		layout.Push<float>(4); // colors
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, sizeof(indices));

		m_Shader = std::make_unique<Shader>("res/shaders/TextureAtlas.shader");
		m_Shader->Bind();

		m_Texture1 = std::make_unique<Texture>("res/textures/terrain.png");
		m_Texture1->Bind(0);
		m_Shader->SetUniform1i("u_Texture", 0);
	}

	TestDynamic::~TestDynamic()
	{
	}

	void TestDynamic::OnUpdate(float deltaTime)
	{
	}

	Vertex* TestDynamic::CreateQuad(Vertex* target, float x, float y, float texture_offset, unsigned int x_texture, unsigned int y_texture) {
		float size = 1.0f;

		target->Position = { x, y, 0.0f };
		target->TexCoords = { x_texture * texture_offset, y_texture * texture_offset };
		target->Color = { 0.18f, 0.6f, 0.96f, 1.0f };
		target++; // here I just increment the pointer (to stack memory)

		target->Position = { x + size, y, 0.0f };
		target->TexCoords = { (x_texture + 1) * texture_offset, y_texture * texture_offset };
		target->Color = { 0.18f, 0.6f, 0.96f, 1.0f };
		target++;

		target->Position = { x + size,  y + size, 0.0f };
		target->TexCoords = { (x_texture + 1) * texture_offset, (y_texture + 1) * texture_offset };
		target->Color = { 0.18f, 0.6f, 0.96f, 1.0f };
		target++;

		target->Position = { x,  y + size, 0.0f };
		target->TexCoords = { x_texture * texture_offset, (y_texture + 1) * texture_offset };
		target->Color = { 0.18f, 0.6f, 0.96f, 1.0f };
		target++;

		return target;
	}

	void TestDynamic::OnRender(const glm::mat4& viewMatrix)
	{
		float offset = 0.0625f;
		// set dynamic vertex buffer
		/*float vertices[] = {
			// positions			// texture coords					// colors
			-100.0f, -100.0f, 0.0f,		4 * offset, 1.0f - 1 * offset,		0.18f, 0.6f, 0.96f, 1.0f,
			 100.0f, -100.0f, 0.0f,		5 * offset, 1.0f - 1 * offset,		0.18f, 0.6f, 0.96f, 1.0f,
			 100.0f,  100.0f, 0.0f,		5 * offset, 1.0f - 0 * offset,		0.18f, 0.6f, 0.96f, 1.0f,
			-100.0f,  100.0f, 0.0f,		4 * offset, 1.0f - 0 * offset,		0.18f, 0.6f, 0.96f, 1.0f,

			 101.0f, -100.0f, 0.0f,		0 * offset, 1.0f - 1 * offset,		1.0f, 0.93f, 0.24f, 1.0f,
			 301.0f, -100.0f, 0.0f,		1 * offset, 1.0f - 1 * offset,		1.0f, 0.93f, 0.24f, 1.0f,
			 301.0f,  100.0f, 0.0f,		1 * offset, 1.0f - 0 * offset,		1.0f, 0.93f, 0.24f, 1.0f,
			 101.0f,  100.0f, 0.0f,		0 * offset, 1.0f - 0 * offset,		1.0f, 0.93f, 0.24f, 1.0f
		};*/

		unsigned short indexCount = 0;

		std::array<Vertex, 1000> vertices;
		Vertex* buffer = vertices.data();
		/*for (int y = 0; y < 5; y++) {
			for (int x = 0; x < 5; x++) {
				buffer = CreateQuad(buffer, x, y, offset, 3, 15);
				indexCount += 6;
			}
		}*/

		buffer = CreateQuad(buffer, m_QuadPosition[0], m_QuadPosition[1], offset, 3, 15);
		indexCount += 6;
		m_VertexBuffer->SendData(vertices.size() * sizeof(Vertex), vertices.data());

		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture1->Bind();

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0));
		glm::mat4 mvp = m_Proj * m_View * model;
		m_Shader->Bind(); 
		m_Shader->SetUniformMat4f("u_MVP", mvp);
		m_IndexBuffer->SetCount(indexCount);
		renderer.Draw(*m_VAO, *m_IndexBuffer, GL_UNSIGNED_SHORT, *m_Shader);
	};

	void TestDynamic::OnImGuiRender()
	{
		ImGui::Begin("Controls");
		ImGui::DragFloat2("Quad position", m_QuadPosition, 0.2f);
		ImGui::End();
	}

}