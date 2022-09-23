#include "Game.h"
#include <memory>
#include "utils/Logger.h"
#include "entities/Player.h"

Game::Game()
        : m_Width(1920), m_Height(1080),
          m_Window(&m_InputHandler, m_Width, m_Height, "Minecraft clone"),
          m_Camera(glm::vec3(0.0f, 0.0f, 0.0f), 1920, 1080),
          m_ChunkManager(&m_Camera),
          m_SkyColor(0, 0, 0, 1),
          m_ShowGui(true),
          m_Sun("sun", "sun.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f)),
          m_Moon("moon", "moon.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f)),
          m_Pause(false),
          m_AmbientStrength(0.5f),
          m_Player(&m_Camera),
          m_RayCast(5.0f, [this](const glm::vec3 &block) { return m_ChunkManager.IsBlockCastable(block); }) {
    Logger::Init();
    Logger::GetGLLogger()->set_level(spdlog::level::off); // opengl logger level
    this->Init();
}

void Game::Init() {
    BindCommands();
    m_GuiManager.Init(m_Width, m_Height);
    m_ChunkManager.InitWorld(m_Renderer.chunkRenderer.GetStride());

    m_Renderer.Init(m_Width, m_Height);

    physics::Spawn(m_Player, m_ChunkManager);
}

void Game::Run() {
    m_Window.Loop();
}

void Game::UpdateTime(float deltaTime) {
    m_InputHandler.HandleInput();
    if (!m_Pause) {
        OnUpdate(deltaTime);
        OnRender();
    }
}

void Game::OnUpdate(float deltaTime) {
    CheckRayCast();
    physics::UpdatePlayer(m_Player, m_ChunkManager, deltaTime);
    UpdateChunks();
    m_Renderer.chunkRenderer.SetDeltaTime(deltaTime);
    m_Sun.IncrTime(deltaTime);
    m_Sun.SetPosition(m_Camera.GetCameraPosition());
    m_Moon.IncrTime(deltaTime);
    m_Moon.SetPosition(m_Camera.GetCameraPosition());
}

void Game::OnRender() {
    Renderer::Clear(m_SkyColor);
    auto &cameraPos = m_Camera.GetCameraPosition();
    const glm::vec3 sunDir = glm::normalize(m_Sun.GetPosition() - cameraPos); // ???
    UpdateSkyColor(sunDir);
    UpdateAmbient(sunDir);
    m_Renderer.SetViewMatrix(m_Camera.GetViewMatrix());
    m_Renderer.skyRenderer.Render();
    m_Sun.Render(m_Renderer.quadRenderer);
    m_Moon.Render(m_Renderer.quadRenderer);

    m_Renderer.chunkRenderer.SetViewPos(cameraPos);
    m_Renderer.chunkRenderer.SetAmbientStrength(m_AmbientStrength);
    m_Renderer.chunkRenderer.SetSkyColor(m_SkyColor);

    // this is dumb, find alternative
    m_Renderer.chunkRenderer.SetIsDay(m_Sun.IsDay());
    if (m_Sun.IsDay()) {
        m_Renderer.chunkRenderer.SetSunDir(-sunDir);
        m_Renderer.chunkRenderer.SetSunColor(m_Sun.GetColor());
    } else {
        m_Renderer.chunkRenderer.SetSunDir(sunDir);
        m_Renderer.chunkRenderer.SetSunColor(m_Moon.GetColor());
    }

    m_ChunkManager.Render(m_Renderer.chunkRenderer);
    if (m_ShowGui)
        m_GuiManager.Render(m_Renderer.guiRenderer);
}

void Game::CheckRayCast() {
    glm::vec3 playerPos = m_Player.GetPosition();
    glm::vec3 playerDir = m_Player.GetDirection();
    std::optional<glm::vec3> castedBlock = m_RayCast.Cast(playerPos, playerDir);
    if (castedBlock) {
        if (m_InputHandler.LeftMouseClick()) {
            m_ChunkManager.DestroyBlock();
        } else if (m_InputHandler.RightMouseClick()) {
            m_ChunkManager.PlaceBlock(m_Player.GetHoldingBlock());
        }
    }
}

void Game::UpdateChunks() {
    m_ChunkManager.UpdateChunks();
}

void Game::Resize(int width, int height) {
    m_Width = width;
    m_Height = height;
    m_Camera.Resize(m_Width, m_Height);
    m_GuiManager.Resize(m_Width, m_Height);
    m_Renderer.Resize(m_Width, m_Height);
}

void Game::UpdateFPS(uint32_t numFrames) {
    std::string fps = std::to_string(numFrames);
    std::string ms = std::to_string(1000.0 / numFrames);
    LOG_INFO("{} FPS / {} ms", fps, ms);
}

void Game::UpdateSkyColor(const glm::vec3 &sunDir) {
    static constexpr glm::vec3 upVector = {0, 1, 0};
    const glm::vec3 frontVector = m_Camera.GetFrontVector();
    const float d = glm::dot(upVector, sunDir);
    const float x = glm::dot(upVector, frontVector);

    if (d >= 0) {
        glm::vec4 color = d * SkyColors::tc1 + (1 - d) * SkyColors::tc2;
        m_SkyColor = d * SkyColors::bc1 + (1 - d) * SkyColors::bc2;
        m_Renderer.skyRenderer.SetSkyColor(color);
        m_Renderer.skyRenderer.SetFogColor(m_SkyColor);
    } else {
        m_SkyColor = -d * SkyColors::tc2 + (1 + d) * SkyColors::bc2;
        m_Renderer.skyRenderer.SetSkyColor(SkyColors::tc2);
        m_Renderer.skyRenderer.SetFogColor(m_SkyColor);
    }

    float lowerLimit = m_Height * (-glm::asin(x) * 0.5f / glm::radians(35.0f) + 0.5f);
    m_Renderer.skyRenderer.SetLowerLimit(lowerLimit);
}

void Game::UpdateAmbient(const glm::vec3 &sunDir) {
    static constexpr glm::vec3 upVector = {0, 1, 0};
    float d = glm::dot(upVector, sunDir);

    if (d >= 0) {
        m_AmbientStrength = d * 0.5f + (1 - d) * 0.1f;
    } else {
        m_AmbientStrength = -d * 0.05f + (1 + d) * 0.1f;
    }
}

void Game::BindCommands() {
    // commands
    m_InputHandler.BindW([this]() { m_Player.Move(Movement::FORWARD); });
    m_InputHandler.BindA([this]() { m_Player.Move(Movement::LEFT); });
    m_InputHandler.BindS([this]() { m_Player.Move(Movement::BACKWARD); });
    m_InputHandler.BindD([this]() { m_Player.Move(Movement::RIGHT); });
    m_InputHandler.BindF([this]() { m_Player.Move(Movement::FLY); });
    m_InputHandler.BindSpace([this]() { m_Player.Move(Movement::UP); });
    m_InputHandler.BindShift([this]() { m_Player.Move(Movement::SPRINT); });
    m_InputHandler.BindCtrl([this]() { m_Player.Move(Movement::DOWN); });
    m_InputHandler.BindP([this]() { this->Pause(); });
    m_InputHandler.BindG([this]() { this->ShowGui(); });

    // events
    m_InputHandler.NumberKeyPressedEvent.AddObserver([this](int key) { m_Player.OnNotify(key); });
    m_InputHandler.MouseScrolledEvent.AddObserver(
            [this](float offset) { m_Player.OnNotify(offset); });
    m_Player.HoldingBlockChanged().AddObserver([this](int key) { m_GuiManager.OnNotify(key); });
    m_InputHandler.MouseMovedEvent.AddObserver(
            [this](float xOffset, float yOffset) { m_Camera.Watch(xOffset, yOffset); });
    m_InputHandler.WindowResizedEvent.AddObserver(
            [this](int width, int height) { this->Resize(width, height); });
    m_InputHandler.UpdateFPSEvent.AddObserver(
            [this](uint32_t numFrames) { this->UpdateFPS(numFrames); });
    m_InputHandler.UpdateTimeEvent.AddObserver(
            [this](float deltaTime) { this->UpdateTime(deltaTime); });
}

void Game::Pause() {
    m_Pause = !m_Pause;
}

void Game::ShowGui() {
    m_ShowGui = !m_ShowGui;
}
