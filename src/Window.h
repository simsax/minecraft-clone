#pragma once

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "Game.h"

namespace glfw {

	class Window
	{
	public:
		Window(int width, int height, std::string name);
		~Window();

		void WindowLoop(Game& game);

	private:
		static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
		void ManageInput();

		static bool firstMouse;
		static float lastX;
		static float lastY;
		static float mouseSensitivity;
		static bool wireframe;

		int m_Width;
		int m_Height;
		std::string m_Name;
		GLFWwindow* m_Window;
	};

}