#pragma once

#include "Test.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>

namespace test {

	struct Vertex {
		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec4 Color;
	};

	class TestDynamic : public Test {
	public:
		TestDynamic();
		~TestDynamic();

		void OnUpdate(float deltaTime) override;
		void OnRender(const glm::mat4& viewMatrix) override;
		void OnImGuiRender() override;
	private:
		Vertex* CreateQuad(Vertex* target, float x, float y, float texture_offset, unsigned int x_texture, unsigned int y_texture);

		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture1;
		std::unique_ptr<Texture> m_Texture2;
		glm::mat4 m_Proj, m_View;

		float m_QuadPosition[2] = { -100.0f, -100.0f };
	};

}