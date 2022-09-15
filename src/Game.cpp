#include "Game.h"
#include <memory>
#include "utils/Logger.h"
#include "entities/Player.h"

Game::Game()
        : m_Width(1920), m_Height(1080),
          m_Window(&m_InputHandler, m_Width, m_Height, "Minecraft clone"),
          m_Camera(glm::vec3(0.0f, 0.0f, 0.0f), 1920, 1080),
          m_ChunkManager(&m_Camera),
          m_Blocks(std::vector({Block::DIRT, Block::GRASS, Block::SAND, Block::SNOW, Block::STONE,
                                Block::WOOD, Block::DIAMOND, Block::EMPTY, Block::EMPTY})),
          m_SkyColor(0, 0, 0, 1),
          m_ShowGui(true),
          m_Sun("sun", "sun.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f)),
          m_Moon("moon", "moon.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f)),
          m_Pause(false),
          m_Sky("sky", "",
                {static_cast<float>(m_Width) / 2.0f, 0.0f, static_cast<float>(m_Height) / 2.0f},
                {m_Width, 0.0f, m_Height}),
          m_AmbientStrength(0.5f),
          m_Player(&m_Camera) {
    Logger::Init();
    Logger::GetGLLogger()->set_level(spdlog::level::off); // opengl logger level
    this->Init();
}

void Game::Init() {
    BindCommands();
    m_Renderer.Init(m_Width, m_Height);
    m_GuiManager.Init(m_Width, m_Height);
    m_ChunkManager.InitWorld();
    QuadEntity::InitBuffers();
    Sun::InitShaders("shader_quad.vert", "shader_quad.frag");
    Sky::InitShaders("shader_sky.vert", "shader_sky.frag");
    m_Sun.InitTexture();
    m_Moon.InitTexture();

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
//    CheckRayCast();
    physics::UpdatePlayer(m_Player, m_ChunkManager, deltaTime);
    UpdateChunks();
    m_Renderer.SetDeltaTime(deltaTime);
    m_Sun.IncrTime(deltaTime);
    m_Sun.SetPosition(m_Camera.GetCameraPosition());
    m_Moon.IncrTime(deltaTime);
    m_Moon.SetPosition(m_Camera.GetCameraPosition());
}

void Game::OnRender() {
    Renderer::Clear(m_SkyColor);
    const glm::vec3 sunDir = glm::normalize(m_Sun.GetPosition() - m_Camera.GetCameraPosition());
    UpdateSkyColor(sunDir);
    UpdateAmbient(sunDir);
    m_Renderer.SetViewMatrix(m_Camera.GetViewMatrix());
    m_Sky.Render(m_Renderer);
    m_Sun.Render(m_Renderer);
    m_Moon.Render(m_Renderer);
    m_ChunkManager.Render(m_Renderer, m_SkyColor, m_Sun.IsDay() ? m_Sun : m_Moon,
                          m_Sun.IsDay() ? -sunDir : sunDir, m_AmbientStrength);
    if (m_ShowGui)
        m_GuiManager.Render(m_Renderer);
}

void Game::CheckRayCast() {
    glm::vec3 &playerPos = m_Camera.GetCameraPosition();
    glm::vec3 playerDir = m_Camera.GetPlayerDirection();
    float Sx = std::abs(1 / playerDir.x);
    float Sy = std::abs(1 / playerDir.y);
    float Sz = std::abs(1 / playerDir.z);
    glm::vec3 currentVoxel
            = {std::floor(playerPos.x), std::floor(playerPos.y), std::floor(playerPos.z)};
    glm::vec3 rayLength;
    glm::vec3 step;

    if (playerDir.x < 0) {
        step.x = -1;
        rayLength.x = (playerPos.x - currentVoxel.x) * Sx;
    } else {
        step.x = 1;
        rayLength.x = (currentVoxel.x + 1 - playerPos.x) * Sx;
    }
    if (playerDir.y < 0) {
        step.y = -1;
        rayLength.y = (playerPos.y - currentVoxel.y) * Sy;
    } else {
        step.y = 1;
        rayLength.y = (currentVoxel.y + 1 - playerPos.y) * Sy;
    }
    if (playerDir.z < 0) {
        step.z = -1;
        rayLength.z = (playerPos.z - currentVoxel.z) * Sz;
    } else {
        step.z = 1;
        rayLength.z = (currentVoxel.z + 1 - playerPos.z) * Sz;
    }

    bool voxelFound = false;
    static constexpr float maxDistance = 5.0f;
    float distance = 0.0f;
    while (!voxelFound && distance < maxDistance) {
        // walk
        if (rayLength.x < rayLength.y) {
            if (rayLength.x < rayLength.z) {
                currentVoxel.x += step.x;
                distance = rayLength.x;
                rayLength.x += Sx;
            } else {
                currentVoxel.z += step.z;
                distance = rayLength.z;
                rayLength.z += Sz;
            }
        } else {
            if (rayLength.y < rayLength.z) {
                currentVoxel.y += step.y;
                distance = rayLength.y;
                rayLength.y += Sy;
            } else {
                currentVoxel.z += step.z;
                distance = rayLength.z;
                rayLength.z += Sz;
            }
        }

        voxelFound = m_ChunkManager.IsVoxelSolid(currentVoxel);
    }

//    if (voxelFound) {
//        if (m_KeyPressed[GLFW_MOUSE_BUTTON_LEFT]) {
//            m_ChunkManager.DestroyBlock();
//            m_KeyPressed[GLFW_MOUSE_BUTTON_LEFT] = false;
//        } else if (m_KeyPressed[GLFW_MOUSE_BUTTON_RIGHT]) {
//            m_ChunkManager.PlaceBlock(m_Blocks[m_HoldingBlock]);
//            m_KeyPressed[GLFW_MOUSE_BUTTON_RIGHT] = false;
//        }
//    }
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
    m_Sky.SetPosition(
            {static_cast<float>(m_Width) / 2.0f, 0.0f, static_cast<float>(m_Height) / 2.0f});
    m_Sky.SetScale({m_Width, 0.0f, m_Height});
}

void Game::UpdateFPS(uint32_t numFrames) {
    std::string fps = std::to_string(numFrames);
    std::string ms = std::to_string(1000.0 / numFrames);
    LOG_INFO("{} FPS / {} ms", fps, ms);
//    std::string newTitle = "Minecraft 2 - " + fps + "FPS / " + ms + "ms";
//    m_Window.ChangeTitle(newTitle);
}

void Game::UpdateSkyColor(const glm::vec3 &sunDir) {
    static constexpr glm::vec3 upVector = {0, 1, 0};
    const glm::vec3 frontVector = m_Camera.GetFrontVector();
    const float d = glm::dot(upVector, sunDir);
    const float x = glm::dot(upVector, frontVector);

    if (d >= 0) {
        glm::vec4 color = d * SkyColors::tc1 + (1 - d) * SkyColors::tc2;
        m_SkyColor = d * SkyColors::bc1 + (1 - d) * SkyColors::bc2;
        m_Sky.SetColor(color, m_SkyColor);
    } else {
        m_SkyColor = -d * SkyColors::tc2 + (1 + d) * SkyColors::bc2;
        m_Sky.SetColor(SkyColors::tc2, m_SkyColor);
    }

    float lowerLimit = m_Height * (-glm::asin(x) * 0.5f / glm::radians(35.0f) + 0.5f);
    m_Sky.SetLowerLimit(lowerLimit);
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
