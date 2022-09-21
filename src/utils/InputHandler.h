#pragma once

#include <array>
#include "Keycodes.h"
#include "Subject.hpp"
#include "../graphics/WindowListener.h"

class InputHandler : public WindowListener {
public:
    InputHandler();
    InputHandler(const InputHandler &) = delete;
    InputHandler &operator=(const InputHandler &) = delete;

    bool LeftMouseClick();
    bool RightMouseClick();
    void UpdateTime(float deltaTime) override;
    void KeyPressed(int key) override;
    void KeyReleased(int key) override;
    void MouseScroll(float offset) override;
    void MouseMoved(float xOffset, float yOffset) override;
    void Resize(int width, int height) override;
    void UpdateFPS(uint32_t numFrames) override;
    void HandleInput();
    void BindW(std::function<void()> command);
    void BindA(std::function<void()> command);
    void BindS(std::function<void()> command);
    void BindD(std::function<void()> command);
    void BindF(std::function<void()> command);
    void BindP(std::function<void()> command);
    void BindG(std::function<void()> command);
    void BindSpace(std::function<void()> command);
    void BindShift(std::function<void()> command);
    void BindCtrl(std::function<void()> command);

    // events
    Subject<int> NumberKeyPressedEvent;
    Subject<float> MouseScrolledEvent;
    Subject<float, float> MouseMovedEvent;
    Subject<int, int> WindowResizedEvent;
    Subject<uint32_t> UpdateFPSEvent;
    Subject<float> UpdateTimeEvent;

private:
    std::array<bool, GLFW_KEY_LAST> m_KeyPressed;

    // commands
    std::function<void()> m_W;
    std::function<void()> m_A;
    std::function<void()> m_S;
    std::function<void()> m_D;
    std::function<void()> m_F;
    std::function<void()> m_P;
    std::function<void()> m_G;
    std::function<void()> m_Space;
    std::function<void()> m_Shift;
    std::function<void()> m_Ctrl;
};
