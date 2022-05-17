#include "Window.h"
#include <iostream>
#include <exception>

bool Window::firstMouse = true;
float Window::lastX = 960.0f;
float Window::lastY = 540.0f;
float Window::mouseSensitivity = 0.1f;
bool Window::wireframe = false;

Window::Window(Game *game, int width, int height, std::string name) : m_Game(game), m_Width(width), m_Height(height), m_Name(std::move(name))
{
	// initialize GLFW
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // set opengl to be core
#ifndef NDEBUG
	// During init, enable debug output
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	m_Window = glfwCreateWindow(width, height, m_Name.c_str(), NULL, NULL);
	if (!m_Window)
	{
		throw std::runtime_error("Failed to open GLFW window");
	}
	glfwMakeContextCurrent(m_Window);

	glfwSwapInterval(0); // enables v-sync

	// initialize glew
	if (glewInit() != GLEW_OK)
	{
		throw std::runtime_error("Failed to initialize GLEW");
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	glfwSetCursorPosCallback(m_Window, MouseCallback);
	glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
	glfwSetKeyCallback(m_Window, KeyCallback);
	glfwSetWindowUserPointer(m_Window, (void *)m_Game);
	// capture cursor in the center and hide it
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#ifndef NDEBUG
	// debug stuff
//	glEnable(GL_DEBUG_OUTPUT);
//	glDebugMessageCallback(Window::MessageCallback, 0);
//	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

	m_Game->Init();
}

void Window::WindowLoop()
{
	float currentFrame = 0.0f, deltaTime = 0.0f, lastFrame = 0.0f;
	//#ifndef NDEBUG
	float prevTime = 0.0f, crntTime = 0.0f;
	unsigned int nFrames = 0;
	//#endif
	while (!glfwWindowShouldClose(m_Window))
	{
		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//#ifndef NDEBUG
		crntTime = static_cast<float>(glfwGetTime());
		nFrames++;
		if (crntTime - prevTime >= 1.0)
		{
			std::string fps = std::to_string(nFrames);
			std::string ms = std::to_string(1000.0 / nFrames);
			glm::vec3 playerPos = m_Game->GetPlayerPosition();
			std::string newTitle = "Minecraft 2 - " + fps + "FPS / " + ms + "ms" +
								   "  -  PlayerPos: " + std::to_string(playerPos.x) +
								   "," + std::to_string(playerPos.y) +
								   "," + std::to_string(playerPos.z);
			glfwSetWindowTitle(m_Window, newTitle.c_str());
			prevTime = crntTime;
			nFrames = 0;
		}
		//#endif

		glfwPollEvents();

		m_Game->OnUpdate(deltaTime);
		m_Game->OnRender();

		glfwSwapBuffers(m_Window);
	}
	glfwTerminate();
}

// camera input
void Window::MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
	Game *game;
	game = (Game *)glfwGetWindowUserPointer(window);
	if (firstMouse)
	{
		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);
		firstMouse = false;
	}

	// calculate offset between current mouse position and mouse position in the previous frame
	float xoffset = (static_cast<float>(xpos) - lastX) * mouseSensitivity;
	float yoffset = (lastY - static_cast<float>(ypos)) * mouseSensitivity; // y-coords range from bottom to top
	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	game->ProcessMouse(xoffset, yoffset);
}

void Window::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	Game *game;
	game = (Game *)glfwGetWindowUserPointer(window);

	if (action == GLFW_PRESS)
		game->KeyPressed[button] = true;
	else if (action == GLFW_RELEASE)
		game->KeyPressed[button] = false;
}

void Window::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Game *game;
	game = (Game *)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_CAPS_LOCK && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{ // toggle wireframe mode
		wireframe = !wireframe;
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (key == GLFW_KEY_UNKNOWN)
		return;
	if (action == GLFW_PRESS)
		game->KeyPressed[key] = true;
	else if (action == GLFW_RELEASE)
		game->KeyPressed[key] = false;
}

void GLAPIENTRY Window::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
										GLsizei length, const GLchar *message, const void *userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
}
