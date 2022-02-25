#include "Test3D.h"
#include <GL/glew.h>
#include "GLErrorManager.h"
#include "imgui/imgui.h"
#include "VertexBufferLayout.hpp"
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Renderer.h"
#include <array>

namespace test {

	Test3D::Test3D() :
		m_Proj(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f)), // remember to increase z distance
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0)))
	{
		// wireframe
		//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

		// depth testing
		GLCall(glEnable(GL_DEPTH_TEST));

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

		m_VertexBuffer = std::make_unique<VertexBuffer>(sizeof(VertexM) * MaxVertexCount, nullptr);

		// this is weird and to be fixed (offsetof) - put everything in the vertexbuffer class
		VertexBufferLayout layout;
		layout.Push<float>(3); // position
		layout.Push<float>(2); // texture coords
		//layout.Push<float>(4); // colors
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, sizeof(indices));

		m_Shader = std::make_unique<Shader>("res/shaders/TextureAtlas.shader");
		m_Shader->Bind();

		m_Texture1 = std::make_unique<Texture>("res/textures/terrain.png");
		m_Texture1->Bind(0);
		m_Shader->SetUniform1i("u_Texture", 0);
	}

	Test3D::~Test3D()
	{
	}

	void Test3D::OnUpdate(float deltaTime)
	{
	}

	VertexM* Test3D::CreateQuad(VertexM* target, const glm::vec3& position, float texture_offset, unsigned int texture[2]) {
		float size = 1.0f;

		target->Position = { position[0] + size, position[1], position[2] };
		target->TexCoords = { texture[0] * texture_offset, texture[1] * texture_offset };
		target++;

		target->Position = { position[0], position[1], position[2] };
		target->TexCoords = { (texture[0] + 1) * texture_offset, texture[1] * texture_offset };
		target++;

		target->Position = { position[0],  position[1] + size, position[2] };
		target->TexCoords = { (texture[0] + 1) * texture_offset, (texture[1] + 1) * texture_offset };
		target++;

		target->Position = { position[0] + size,  position[1] + size, position[2] };
		target->TexCoords = { texture[0] * texture_offset, (texture[1] + 1) * texture_offset };
		target++;

		return target;
	}

	VertexM* Test3D::CreateCube(VertexM* target, const glm::vec3& position, float texture_offset, unsigned int texture_side[2], unsigned int texture_top[2], unsigned int texture_bottom[2]) {
		float size = 1.0f;

		// back quad
		target->Position = { position[0] + size, position[1], position[2] };
		target->TexCoords = { texture_side[0] * texture_offset, texture_side[1] * texture_offset };
		target++;

		target->Position = { position[0], position[1], position[2] };
		target->TexCoords = { (texture_side[0] + 1) * texture_offset, texture_side[1] * texture_offset };
		target++;

		target->Position = { position[0],  position[1] + size, position[2] };
		target->TexCoords = { (texture_side[0] + 1) * texture_offset, (texture_side[1] + 1) * texture_offset };
		target++;
		 
		target->Position = { position[0] + size,  position[1] + size, position[2] };
		target->TexCoords = { texture_side[0] * texture_offset, (texture_side[1] + 1) * texture_offset };
		target++;

		// front quad
		target->Position = { position[0], position[1], position[2] + size };
		target->TexCoords = { texture_side[0] * texture_offset, texture_side[1] * texture_offset };
		target++;

		target->Position = { position[0] + size, position[1], position[2] + size };
		target->TexCoords = { (texture_side[0] + 1) * texture_offset, texture_side[1] * texture_offset };
		target++;

		target->Position = { position[0] + size,  position[1] + size, position[2] + size };
		target->TexCoords = { (texture_side[0] + 1) * texture_offset, (texture_side[1] + 1) * texture_offset };
		target++;

		target->Position = { position[0],  position[1] + size, position[2] + size };
		target->TexCoords = { texture_side[0] * texture_offset, (texture_side[1] + 1) * texture_offset };
		target++;

		// right quad
		target->Position = { position[0] + size, position[1], position[2] + size };
		target->TexCoords = { texture_side[0] * texture_offset, texture_side[1] * texture_offset };
		target++;

		target->Position = { position[0] + size, position[1], position[2]  };
		target->TexCoords = { (texture_side[0] + 1) * texture_offset, texture_side[1] * texture_offset };
		target++;

		target->Position = { position[0] + size,  position[1] + size, position[2] };
		target->TexCoords = { (texture_side[0] + 1) * texture_offset, (texture_side[1] + 1) * texture_offset };
		target++;

		target->Position = { position[0] + size,  position[1] + size, position[2] + size };
		target->TexCoords = { texture_side[0] * texture_offset, (texture_side[1] + 1) * texture_offset };
		target++;

		// left quad
		target->Position = { position[0], position[1], position[2] };
		target->TexCoords = { texture_side[0] * texture_offset, texture_side[1] * texture_offset };
		target++;

		target->Position = { position[0], position[1], position[2] + size };
		target->TexCoords = { (texture_side[0] + 1) * texture_offset, texture_side[1] * texture_offset };
		target++;

		target->Position = { position[0],  position[1] + size, position[2] + size };
		target->TexCoords = { (texture_side[0] + 1) * texture_offset, (texture_side[1] + 1) * texture_offset };
		target++;

		target->Position = { position[0],  position[1] + size, position[2]  };
		target->TexCoords = { texture_side[0] * texture_offset, (texture_side[1] + 1) * texture_offset };
		target++;

		// top quad
		target->Position = { position[0], position[1] + size, position[2] + size };
		target->TexCoords = { texture_top[0] * texture_offset, texture_top[1] * texture_offset };
		target++;

		target->Position = { position[0] + size, position[1] + size, position[2] + size };
		target->TexCoords = { (texture_top[0] + 1) * texture_offset, texture_top[1] * texture_offset };
		target++;

		target->Position = { position[0] + size,  position[1] + size, position[2] };
		target->TexCoords = { (texture_top[0] + 1) * texture_offset, (texture_top[1] + 1) * texture_offset };
		target++;

		target->Position = { position[0],  position[1] + size, position[2] };
		target->TexCoords = { texture_top[0] * texture_offset, (texture_top[1] + 1) * texture_offset };
		target++;

		// bottom quad
		target->Position = { position[0], position[1], position[2] + size };
		target->TexCoords = { texture_bottom[0] * texture_offset, texture_bottom[1] * texture_offset };
		target++;

		target->Position = { position[0] + size, position[1], position[2] + size };
		target->TexCoords = { (texture_bottom[0] + 1) * texture_offset, texture_bottom[1] * texture_offset };
		target++;

		target->Position = { position[0] + size,  position[1], position[2] };
		target->TexCoords = { (texture_bottom[0] + 1) * texture_offset, (texture_bottom[1] + 1) * texture_offset };
		target++;

		target->Position = { position[0],  position[1], position[2] };
		target->TexCoords = { texture_bottom[0] * texture_offset, (texture_bottom[1] + 1) * texture_offset };
		target++;

		return target;
	}

	void Test3D::OnRender(const glm::mat4& viewMatrix)
	{

		float texture_offset = 0.0625f; // texture_size/atlas_size
			
		unsigned short indexCount = 0;

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

		std::array<VertexM, 1000> vertices;
		VertexM* buffer = vertices.data();
		for (int i = 0; i < 10; i++) {
			buffer = CreateCube(buffer, cubePositions[i], texture_offset, texture_side, texture_top, texture_bottom); // grass block
			indexCount += 36;
		}
		m_VertexBuffer->SendData(vertices.size() * sizeof(VertexM), vertices.data());

		GLCall(glClearColor(173.0f / 255.0f, 223.0f / 255.0f, 230.0f / 255.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT););

		Renderer renderer;

		m_Texture1->Bind();

		// position, target, up
		m_View = viewMatrix;
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = m_Proj * m_View * model;
		m_Shader->Bind(); 
		m_Shader->SetUniformMat4f("u_MVP", mvp);
		m_IndexBuffer->SetCount(indexCount);
		renderer.Draw(*m_VAO, *m_IndexBuffer, GL_UNSIGNED_SHORT, *m_Shader);
	};

	void Test3D::OnImGuiRender()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}



}