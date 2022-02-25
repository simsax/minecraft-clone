#include "Test.h"
#include "imgui/imgui.h"

namespace test {
	TestMenu::TestMenu(Test*& currentTestPointer) : m_CurrentTest(currentTestPointer)
	{
	}

	void TestMenu::OnImGuiRender()
	{
		for (auto& test : m_Tests) {
			if (ImGui::Button(test.first.c_str())) // if button is pressed (button exists only inside this if condition)
				m_CurrentTest = test.second(); // construct a test and return a test pointer
		}
	}

}
