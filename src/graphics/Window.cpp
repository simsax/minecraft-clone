#include "Window.h"
#include <exception>
#include <iostream>
#include "../utils/Logger.h"

bool Window::firstMouse = true;
float Window::lastX = 960.0f;
float Window::lastY = 540.0f;
float Window::mouseSensitivity = 0.1f;
bool Window::wireframe = false;
bool Window::mouseCaptured = true;

Window::Window(WindowListener* windowListener, int width, int height, const char *name)
        : m_WindowListener(windowListener), m_Width(width), m_Height(height), m_Name(name) {
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
    glfwSetWindowUserPointer(m_Window, (void *) m_WindowListener);
    glfwSetScrollCallback(m_Window, ScrollCallback);
    glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

    // capture cursor in the center and hide it
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#ifndef NDEBUG
    // debug stuff
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(Window::MessageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
}

void Window::Loop() {
    float currentFrame = 0.0f, deltaTime = 0.0f, lastFrame = 0.0f;
    //#ifndef NDEBUG
    float prevTime = 0.0f;
    uint32_t nFrames = 0;
    //#endif
    while (!glfwWindowShouldClose(m_Window)) {
        currentFrame = static_cast<float>(glfwGetTime());
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

        glfwPollEvents();

        m_WindowListener->UpdateTime(deltaTime);

        glfwSwapBuffers(m_Window);
    }
    glfwTerminate();
}

// camera input
void Window::MouseCallback(GLFWwindow *window, double xpos, double ypos) {
    auto* windowListener = static_cast<WindowListener *>(glfwGetWindowUserPointer(window));

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

void Window::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    auto* windowListener = static_cast<WindowListener *>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS)
        windowListener->KeyPressed(button);
    else if (action == GLFW_RELEASE)
        windowListener->KeyReleased(button);
}

void Window::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto* windowListener = static_cast<WindowListener *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (mouseCaptured)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseCaptured = !mouseCaptured;
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
                                        GLsizei length, const GLchar *message,
                                        const void *userParam) {
    if (type == GL_DEBUG_TYPE_ERROR)
        GL_LOG_ERROR("type = {}, severity = {}, message = {}", type, severity, message);
    else
        GL_LOG_WARN("type = {}, severity = {}, message = {}", type, severity, message);
//    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
//            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

void Window::ErrorCallback(int error, const char *msg) {
    GL_LOG_ERROR("Error {}: {}", error, msg);
}

void Window::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    auto* windowListener = static_cast<WindowListener *>(glfwGetWindowUserPointer(window));
    windowListener->MouseScroll(yoffset);
}

void Window::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    auto* windowListener = static_cast<WindowListener *>(glfwGetWindowUserPointer(window));
    glViewport(0, 0, width, height);
    windowListener->Resize(width, height);
}

void Window::ChangeTitle(const std::string &newTitle) {
    glfwSetWindowTitle(m_Window, newTitle.c_str());
}
