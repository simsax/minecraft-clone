#include "InputHandler.h"

InputHandler::InputHandler() : m_KeyPressed({}){
}

void InputHandler::KeyPressed(int key) {
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
        int keyIndex = key - GLFW_KEY_0 - 1;
        NumberKeyPressedEvent.Notify(keyIndex);
    }
    m_KeyPressed[key] = true;
}

void InputHandler::KeyReleased(int key) {
    m_KeyPressed[key] = false;
}

void InputHandler::MouseScroll(float offset) {
    MouseScrolledEvent.Notify(offset);
}

void InputHandler::MouseMoved(float xOffset, float yOffset) {
    MouseMovedEvent.Notify(xOffset, yOffset);
}

void InputHandler::HandleInput() {
    if (m_KeyPressed[GLFW_KEY_SPACE])
        m_Space();
    if (m_KeyPressed[GLFW_KEY_LEFT_CONTROL])
        m_Ctrl();
    if (m_KeyPressed[GLFW_KEY_W])
        m_W();
    if (m_KeyPressed[GLFW_KEY_S])
        m_S();
    if (m_KeyPressed[GLFW_KEY_A])
        m_A();
    if (m_KeyPressed[GLFW_KEY_D])
        m_D();
    if (m_KeyPressed[GLFW_KEY_LEFT_SHIFT])
        m_Shift();
    if (m_KeyPressed[GLFW_KEY_F]) {
        m_KeyPressed[GLFW_KEY_F] = false;
        m_F();
    }
    if (m_KeyPressed[GLFW_KEY_G]) {
        m_KeyPressed[GLFW_KEY_G] = false;
        m_G();
    }
    if (m_KeyPressed[GLFW_KEY_P]) {
        m_KeyPressed[GLFW_KEY_P] = false;
        m_P();
    }
}

void InputHandler::BindW(std::function<void()> command) {
    m_W = std::move(command);
}

void InputHandler::BindA(std::function<void()> command) {
    m_A = std::move(command);
}

void InputHandler::BindS(std::function<void()> command) {
    m_S = std::move(command);
}

void InputHandler::BindD(std::function<void()> command) {
    m_D = std::move(command);
}

void InputHandler::BindF(std::function<void()> command) {
    m_F = std::move(command);
}

void InputHandler::BindP(std::function<void()> command) {
    m_P = std::move(command);
}

void InputHandler::BindG(std::function<void()> command) {
    m_G = std::move(command);
}

void InputHandler::BindSpace(std::function<void()> command) {
    m_Space = std::move(command);
}

void InputHandler::BindShift(std::function<void()> command) {
    m_Shift = std::move(command);
}

void InputHandler::BindCtrl(std::function<void()> command) {
    m_Ctrl = std::move(command);
}

void InputHandler::UpdateTime(float deltaTime) {
    UpdateTimeEvent.Notify(deltaTime);
}

void InputHandler::Resize(int width, int height) {
    WindowResizedEvent.Notify(width, height);
}

void InputHandler::UpdateFPS(uint32_t numFrames) {
    UpdateFPSEvent.Notify(numFrames);
}
