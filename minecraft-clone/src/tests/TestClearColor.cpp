#include "TestClearColor.h"
#include <GL/glew.h>
#include "GLErrorManager.h"
#include "imgui/imgui.h"

namespace test {

	TestClearColor::TestClearColor() : m_ClearColor {0.2f, 0.3f, 0.8f, 1.0f}
	{
	}

	TestClearColor::~TestClearColor()
	{
	}

	void TestClearColor::OnUpdate(float deltaTime)
	{
	}

	void TestClearColor::OnRender(const glm::mat4& viewMatrix)
	{
		GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	};

	void TestClearColor::OnImGuiRender()
	{
		ImGui::ColorEdit4("Clear color", m_ClearColor);
	}

}