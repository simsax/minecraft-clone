#pragma once

#include "Test.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "IndexBuffer.h"
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <memory>

namespace test {

	class TestBatchRendering : public Test {
	public:
		TestBatchRendering();
		~TestBatchRendering();

		void OnUpdate(float deltaTime) override;
		void OnRender(const glm::mat4& viewMatrix) override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture1;
		std::unique_ptr<Texture> m_Texture2;
		glm::mat4 m_Proj, m_View;
	};

}