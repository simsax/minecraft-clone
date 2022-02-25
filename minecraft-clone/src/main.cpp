#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestBatchRendering.h"
#include "tests/TestTextureAtlas.h"
#include "tests/TestDynamic.h"
#include "tests/Test3D.h"
#include "Camera.h"
#include <array>

// should probably make a window class handling glfw (remember one class should take care of one resource)

bool firstMouse = true;
float lastX = 960.0f;
float lastY = 540.0f;
float sensitivity = 0.1f;
cam::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// calculate offset between current mouse position and mouse position in the previous frame
	float xoffset = (xpos - lastX) * sensitivity;
	float yoffset = (lastY - ypos) * sensitivity; // y-coords range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouse(xoffset, yoffset);
}

int main() {
	// initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // set opengl to be core

	GLFWwindow* window;
	window = glfwCreateWindow(1920, 1080, "Hello world", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to open GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // enables v-sync

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW\n";
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		// enable blending
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		Renderer renderer;

		// initialize imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui::StyleColorsDark();
		ImGui_ImplOpenGL3_Init((char*)glGetString(330));

		test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear color");
		testMenu->RegisterTest<test::TestTexture2D>("Texture 2D");
		testMenu->RegisterTest<test::TestBatchRendering>("Batch rendering");
		testMenu->RegisterTest<test::TestTextureAtlas>("Texture atlas");
		testMenu->RegisterTest<test::TestDynamic>("Dynamic geometry");
		testMenu->RegisterTest<test::Test3D>("Test 3D");

		float currentFrame = 0.0f, deltaTime = 0.0f, lastFrame = 0.0f;
		bool end = false;

		while (!glfwWindowShouldClose(window)) {
			//GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();

			currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			std::array<cam::Key, 7> keyPressed = { 
				cam::Key::NONE,
				cam::Key::NONE, 
				cam::Key::NONE,
				cam::Key::NONE, 
				cam::Key::NONE,
				cam::Key::NONE,
				cam::Key::NONE
			};
			int k = 0;
			if (currentTest) {
				if (!end && currentTest != testMenu)
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // capture cursor in the center and hide it
				else
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
					end = true;
				if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // sprint
					keyPressed[k++] = cam::Key::LEFT_SHIFT;
				if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
					keyPressed[k++] = cam::Key::SPACE;
				if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
					keyPressed[k++] = cam::Key::LEFT_CONTROL;
				if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
					keyPressed[k++] = cam::Key::UP;
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
					keyPressed[k++] = cam::Key::DOWN;
				if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
					keyPressed[k++] = cam::Key::LEFT;
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
					keyPressed[k++] = cam::Key::RIGHT;

				camera.ProcessKeyboard(keyPressed, deltaTime);

				if (!end && currentTest != testMenu)
					glfwSetCursorPosCallback(window, MouseCallback);

				currentTest->OnUpdate(0.0f);
				currentTest->OnRender(camera.GetViewMatrix());
				
				ImGui::Begin("Test");
				if (currentTest != testMenu && ImGui::Button("<-")) { // if it's not the menu and the back button is clicked
					delete currentTest;
					currentTest = testMenu;
				}
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		delete currentTest;
		if (currentTest != testMenu)
			delete testMenu;
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}
