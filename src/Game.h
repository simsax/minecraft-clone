#pragma once

#include "camera/Camera.h"
#include "world/ChunkManager.h"
#include "gui/GuiManager.h"
#include "entities/Sun.h"
#include "entities/Moon.h"
#include "graphics/Window.h"
#include "entities/Sky.h"
#include "utils/InputHandler.h"
#include "entities/Player.h"

class Game {
public:
    Game();
    void Run();
    void Pause();
    void ShowGui();

private:
    void Init();
    void OnUpdate(float deltaTime);
    void OnRender();
    void CheckRayCast();
    void UpdateChunks();
    void SetSkyColor(const glm::vec4& topColor, const glm::vec4& bottomColor);
    void UpdateSkyColor(const glm::vec3& sunDir);
    void UpdateAmbient(const glm::vec3& sunDir);
    void BindCommands();
    void UpdateTime(float deltaTime);
    void UpdateFPS(uint32_t numFrames);
    void Resize(int width, int height);

    int m_Width;
    int m_Height;
    InputHandler m_InputHandler;
    Window m_Window;
    Camera m_Camera;
    Renderer m_Renderer;
    ChunkManager m_ChunkManager;
    std::vector<Block> m_Blocks;
    glm::vec4 m_SkyColor;
    GuiManager m_GuiManager;
    bool m_ShowGui;
    Sun m_Sun;
    Moon m_Moon;
    bool m_Pause;
    Sky m_Sky;
    float m_AmbientStrength;
    Player m_Player;
};
