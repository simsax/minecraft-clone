#pragma once

#include "Test.h"

namespace test {

	class TestClearColor : public Test {
	public:
		TestClearColor();
		~TestClearColor();

		void OnUpdate(float deltaTime) override;
		void OnRender(const glm::mat4& viewMatrix) override;
		void OnImGuiRender() override;
	private:
		float m_ClearColor[4];
	};

}