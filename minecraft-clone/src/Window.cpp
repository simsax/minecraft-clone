#include "Window.h"
#include "glm/glm.hpp"
#include <iostream>
#include <exception>

namespace glfw {

	bool Window::firstMouse = true;
	float Window::lastX = 960.0f;
	float Window::lastY = 540.0f;
	float Window::mouseSensitivity = 0.1f;
	bool Window::wireframe = false;

	Window::Window(int width, int height, const std::string& name) : m_Width(width), m_Height(height), m_Name(name)
	{
		// initialize GLFW
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // set opengl to be core
#ifdef _DEBUG
		// During init, enable debug output
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif		

		m_Window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
		if (!m_Window) {
			throw std::runtime_error("Failed to open GLFW window");
		}
		glfwMakeContextCurrent(m_Window);

		glfwSwapInterval(1); // enables v-sync

		// initialize glew
		if (glewInit() != GLEW_OK) {
			throw std::runtime_error("Failed to initialize GLEW");
		}
		std::cout << glGetString(GL_VERSION) << std::endl;

		glfwSetCursorPosCallback(m_Window, MouseCallback);
		glfwSetKeyCallback(m_Window, KeyCallback);
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // capture cursor in the center and hide it

#ifdef _DEBUG
		// debug stuff
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(Window::MessageCallback, 0);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
	}

	Window::~Window()
	{
		glfwTerminate();
	}

	void Window::WindowLoop(Game& game) {
		float currentFrame = 0.0f, deltaTime = 0.0f, lastFrame = 0.0f;
#ifdef _DEBUG
		float prevTime = 0.0f, crntTime = 0.0f;
		unsigned int nFrames = 0;
#endif
		while (!glfwWindowShouldClose(m_Window)) {
			//glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

			// exit when 'ESC' key is pressed
			if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwDestroyWindow(m_Window);
				break;
			}

			currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

#ifdef _DEBUG
			crntTime = static_cast<float>(glfwGetTime());
			nFrames++;
			if (crntTime - prevTime >= 1.0) {
				std::string fps = std::to_string(nFrames);
				std::string ms = std::to_string(1000.0/nFrames);
				std::string newTitle = "Minecraft 2 - " + fps + "FPS / " + ms + "ms" + "  -  PlayerPos: " + std::to_string(Game::camera.GetPlayerPosition().x) + "," + std::to_string(Game::camera.GetPlayerPosition().z);
				glfwSetWindowTitle(m_Window, newTitle.c_str());
				prevTime = crntTime;
				nFrames = 0;
			}
#endif

			glfwPollEvents();

			ManageInput(deltaTime);

			game.OnUpdate(deltaTime);
			game.OnRender();

			glfwSwapBuffers(m_Window);
		}
	}

	void Window::ManageInput(float deltaTime)
	{
		// camera inputs
		std::array<cam::Key, static_cast<int>(cam::Key::Key_MAX) + 1> keyPressed;
		keyPressed.fill(cam::Key::NONE);
		int k = 0;

		if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // sprint
			keyPressed[k++] = cam::Key::LEFT_SHIFT;
		if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
			keyPressed[k++] = cam::Key::SPACE;
		if (glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			keyPressed[k++] = cam::Key::LEFT_CONTROL;
		if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
			keyPressed[k++] = cam::Key::UP;
		if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
			keyPressed[k++] = cam::Key::DOWN;
		if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
			keyPressed[k++] = cam::Key::LEFT;
		if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
			keyPressed[k++] = cam::Key::RIGHT;

		Game::camera.ProcessKeyboard(keyPressed, deltaTime);

		// other type of inputs in the callback
	}

}

void glfw::Window::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);
		firstMouse = false;
	}

	// calculate offset between current mouse position and mouse position in the previous frame
	float xoffset = (static_cast<float>(xpos) - lastX) * mouseSensitivity;
	float yoffset = (lastY - static_cast<float>(ypos)) * mouseSensitivity; // y-coords range from bottom to top
	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	Game::camera.ProcessMouse(xoffset, yoffset);
}

void glfw::Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) { // toggle wireframe mode
		wireframe = !wireframe;
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void GLAPIENTRY glfw::Window::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}
