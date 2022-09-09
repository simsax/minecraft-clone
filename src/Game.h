#pragma once

#include "camera/Camera.h"
#include "world/ChunkManager.h"
#include "gui/GuiManager.h"
#include "graphics/WindowListener.h"
#include "entities/Sun.h"
#include "entities/Moon.h"
#include "graphics/Window.h"
#include "entities/Sky.h"

class Game : public WindowListener {
public:
    Game();
    void Update(float deltaTime) override;
    void KeyPressed(int key) override;
    void KeyReleased(int key) override;
    void MouseScroll(float offset) override;
    void MouseMoved(float xOffset, float yOffset) override;
    void Resize(int width, int height) override;
    void UpdateFPS(uint32_t numFrames) override;

    void Run();

private:
    void Init();
    void OnUpdate(float deltaTime);
    void OnRender();
    void CheckRayCast();
    void ApplyGravity(float deltaTime);
    void Jump();
    void UpdateChunks();
    void Move(float deltaTime);
    void HandleInput();
    void SetSkyColor(const glm::vec4& topColor, const glm::vec4& bottomColor);

    int m_Width;
    int m_Height;
    Window m_Window;
    std::array<bool, GLFW_KEY_LAST> m_KeyPressed;
    bool m_Ground;
    Camera m_Camera;
    Renderer m_Renderer;
    ChunkManager m_ChunkManager;
    std::vector<Block> m_Blocks;
    int m_HoldingBlock;
    glm::vec3 m_SkyColor;
    GuiManager m_GuiManager;
    bool m_ShowGui;
    float m_VerticalVelocity;
    Sun m_Sun;
    Moon m_Moon;
    bool m_Pause;
    Sky m_Sky;
};
