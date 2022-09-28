#pragma once

#include <memory>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "WindowListener.h"
#include <string>

class Window {
public:
    Window(WindowListener* windowListener, int width, int height, const char* name);
    void Loop();
    void ChangeTitle(const std::string& newTitle);

private:
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam);
    static void ErrorCallback(int error, const char* msg);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    static bool firstMouse;
    static float lastX;
    static float lastY;
    static float mouseSensitivity;
    static bool wireframe;
    static bool mouseCaptured;

    int m_Width;
    int m_Height;
    const char* m_Name;
    GLFWwindow* m_Window;
    WindowListener* m_WindowListener;
};
