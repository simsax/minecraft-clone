#pragma once

#include "camera/Camera.h"
#include "world/ChunkManager.h"
#include "gui/GuiManager.h"

class Game {
public:
    Game();
    void Init();
    void OnUpdate(float deltaTime);
    void OnRender();
    void ProcessMouse(float xoffset, float yoffset);
    void PressKey(int key);
    void ReleaseKey(int key);
    void Scroll(float offset);

    // debug
    glm::vec3 GetPlayerPosition();

private:
    void CheckRayCast();
    void ApplyGravity(float deltaTime);
    void Jump();
    void UpdateChunks();
    void Move(float deltaTime);
    void HandleInput();

    std::array<bool, GLFW_KEY_LAST> m_KeyPressed;
    bool m_Ground;
    Camera m_Camera;
    Renderer m_Renderer;
    ChunkManager m_ChunkManager;
    ChunkCoord m_LastChunk;
    ChunkCoord m_SortedChunk;
    float m_VerticalVelocity;
    std::vector<Block> m_Blocks;
    int m_HoldingBlock;
    glm::vec3 m_SkyColor;
    GuiManager m_GuiManager;
    bool m_ShowGui;
};
