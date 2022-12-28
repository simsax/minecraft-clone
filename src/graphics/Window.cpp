#include "Window.h"
#include <exception>
#include <iostream>
#include "../utils/Logger.h"

bool Window::firstMouse = true;
float Window::lastX = 960.0f;
float Window::lastY = 540.0f;
float Window::mouseSensitivity = 0.1f;
bool Window::wireframe = false;

#ifndef NDEBUG
bool Window::mouseCaptured = false;
#else
bool Window::mouseCaptured = true;
#endif

Window::Window(WindowListener* windowListener, int width, int height, const char* name)
	: m_WindowListener(windowListener)
	, m_Width(width)
	, m_Height(height)
	, m_Name(name)
	, m_IO(nullptr)
{
	glfwSetErrorCallback(ErrorCallback);
	// initialize GLFW
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // set opengl to be core
#ifndef NDEBUG
	// During init, enable debug output
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	m_Window = glfwCreateWindow(width, height, m_Name, NULL, NULL);
	if (!m_Window) {
		throw std::runtime_error("Failed to open GLFW window");
	}
	glfwMakeContextCurrent(m_Window);

	glfwSwapInterval(0); // enables v-sync

	// initialize glew
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Failed to initialize GLEW");
	}

	//    std::cout << glGetString(GL_VERSION) << "\n";

	// set callbacks
	glfwSetCursorPosCallback(m_Window, MouseCallback);
	glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
	glfwSetKeyCallback(m_Window, KeyCallback);
	glfwSetWindowUserPointer(m_Window, (void*)m_WindowListener);
	glfwSetScrollCallback(m_Window, ScrollCallback);
	glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

	// capture cursor in the center and hide it
	if (mouseCaptured)
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	m_IO = &ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	m_IO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (m_IO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 450");


#ifndef NDEBUG
	// debug stuff
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(Window::MessageCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
}

void Window::Loop()
{
	double currentFrame = 0.0, deltaTime = 0.0, lastFrame = 0.0;
	//#ifndef NDEBUG
	float prevTime = 0.0f;
	uint32_t nFrames = 0;
	//#endif
	while (!glfwWindowShouldClose(m_Window)) {
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//#ifndef NDEBUG
		nFrames++;
		if (currentFrame - prevTime >= 1.0) {
			m_WindowListener->UpdateFPS(nFrames);
			prevTime = currentFrame;
			nFrames = 0;
		}
		//#endif

		m_WindowListener->UpdateTime(deltaTime);

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (m_IO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(m_Window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

// camera input
void Window::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	auto* windowListener = static_cast<WindowListener*>(glfwGetWindowUserPointer(window));

	if (firstMouse) {
		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);
		firstMouse = false;
	}

	// calculate offset between current mouse position and mouse position in the previous frame
	float xoffset = (static_cast<float>(xpos) - lastX) * mouseSensitivity;
	float yoffset = (lastY - static_cast<float>(ypos))
		* mouseSensitivity; // y-coords range from bottom to top
	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	windowListener->MouseMoved(xoffset, yoffset);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto* windowListener = static_cast<WindowListener*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS)
		windowListener->KeyPressed(button);
	else if (action == GLFW_RELEASE)
		windowListener->KeyReleased(button);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto* windowListener = static_cast<WindowListener*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		mouseCaptured = !mouseCaptured;
		if (mouseCaptured)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) { // toggle wireframe mode
		wireframe = !wireframe;
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (key == GLFW_KEY_UNKNOWN)
		return;
	if (action == GLFW_PRESS)
		windowListener->KeyPressed(key);
	else if (action == GLFW_RELEASE)
		windowListener->KeyReleased(key);
}

void GLAPIENTRY Window::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
		GL_LOG_ERROR("type = {}, severity = {}, message = {}", type, severity, message);
	else {
		// GL_LOG_WARN("type = {}, severity = {}, message = {}", type, severity, message);

		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
	}
}

void Window::ErrorCallback(int error, const char* msg) { GL_LOG_ERROR("Error {}: {}", error, msg); }

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto* windowListener = static_cast<WindowListener*>(glfwGetWindowUserPointer(window));
	windowListener->MouseScroll(yoffset);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	auto* windowListener = static_cast<WindowListener*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	windowListener->Resize(width, height);
}

void Window::ChangeTitle(const std::string& newTitle)
{
	glfwSetWindowTitle(m_Window, newTitle.c_str());
}
