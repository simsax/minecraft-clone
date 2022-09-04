#pragma once

class WindowListener {
public:
    virtual void Update(float deltaTime) = 0;
    virtual void KeyPressed(int key) = 0;
    virtual void KeyReleased(int key) = 0;
    virtual void MouseScroll(float offset) = 0;
    virtual void MouseMoved(float xOffset, float yOffset) = 0;
    virtual void Resize(int width, int height) = 0;
    virtual void UpdateFPS(uint32_t numFrames) = 0;
};
