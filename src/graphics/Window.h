#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../Game.h"

class Window {
public:
    Window(Game* game, int width, int height, const char* name);
    void WindowLoop();

private:
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam);
    static void ErrorCallback(int error, const char* msg);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    void ManageInput();

    static bool firstMouse;
    static float lastX;
    static float lastY;
    static float mouseSensitivity;
    static bool wireframe;

    int m_Width;
    int m_Height;
    const char* m_Name;
    GLFWwindow* m_Window;
    Game* m_Game;
};
