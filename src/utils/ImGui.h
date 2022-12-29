#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class MyImGui {
public:
	MyImGui(const MyImGui&) = delete;
	MyImGui& operator=(const MyImGui&) = delete;

	static MyImGui& GetInstance();
	void Init(GLFWwindow* window);
	void StartFrame();
	void Render();
	void ShutDown();

private:
	MyImGui();
};