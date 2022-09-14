#include "Game.h"
#include "utils/Logger.h"

Game::Game()
        : m_Width(1920), m_Height(1080),
          m_Window(this, m_Width, m_Height, "Minecraft clone"), m_KeyPressed({}), m_Ground(false),
          m_Camera(glm::vec3(0.0f, 0.0f, 0.0f), 1920, 1080),
          m_ChunkManager(&m_Camera),
          m_Blocks(std::vector({Block::DIRT, Block::GRASS, Block::SAND, Block::SNOW, Block::STONE,
                                Block::WOOD, Block::DIAMOND, Block::EMPTY, Block::EMPTY})),
          m_HoldingBlock(0),
          m_SkyColor(0, 0, 0),
          m_ShowGui(true), m_VerticalVelocity(0.0f),
          m_Sun("sun", "sun.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f)),
          m_Moon("moon", "moon.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f)),
          m_Pause(false),
          m_Sky("sky", "",
                {static_cast<float>(m_Width) / 2.0f, 0.0f, static_cast<float>(m_Height) / 2.0f},
                {m_Width, 0.0f, m_Height}),
                m_AmbientStrength(0.5f)
{
    Logger::Init();
    Logger::GetGLLogger()->set_level(spdlog::level::off); // opengl logger level
    this->Init();
}

void Game::Init() {
    m_Renderer.Init(m_Width, m_Height);
    m_GuiManager.Init(m_Width, m_Height);
    m_ChunkManager.InitWorld();
    QuadEntity::InitBuffers();
    Sun::InitShaders("shader_quad.vert", "shader_quad.frag");
    Sky::InitShaders("shader_sky.vert", "shader_sky.frag");
    m_Sun.InitTexture();
    m_Moon.InitTexture();

    m_ChunkManager.Spawn();
}

void Game::Run() {
    m_Window.WindowLoop();
}

void Game::Update(float deltaTime) {
    HandleInput();
    if (!m_Pause) {
        OnUpdate(deltaTime);
        OnRender();
    }
}

void Game::KeyPressed(int key) {
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
        m_HoldingBlock = key - GLFW_KEY_0 - 1;
        m_GuiManager.PressKey(m_HoldingBlock);
    }
    m_KeyPressed[key] = true;
}

void Game::KeyReleased(int key) {
    m_KeyPressed[key] = false;
}

void Game::MouseScroll(float offset) {
    m_HoldingBlock += static_cast<int>(offset);
    if (m_HoldingBlock < 0)
        m_HoldingBlock = 0;
    if (m_HoldingBlock > 8)
        m_HoldingBlock = 8;
    m_GuiManager.PressKey(m_HoldingBlock);
}

void Game::MouseMoved(float xOffset, float yOffset) {
    m_Camera.ProcessMouse(xOffset, yOffset);
}

void Game::OnUpdate(float deltaTime) {
    CheckRayCast();
    ApplyGravity(deltaTime);
    Move(deltaTime);
    UpdateChunks();
    m_Renderer.SetDeltaTime(deltaTime);
    m_Sun.IncrTime(deltaTime);
    m_Sun.SetPosition(m_Camera.GetPlayerPosition());
    m_Moon.IncrTime(deltaTime);
    m_Moon.SetPosition(m_Camera.GetPlayerPosition());
}

void Game::OnRender() {
//    Renderer::Clear(m_Sky.GetColor());
    Renderer::Clear(glm::vec3(0));
    const glm::vec3 sunDir = glm::normalize(m_Sun.GetPosition() - m_Camera.GetPlayerPosition());
    UpdateSkyColor(sunDir);
    UpdateAmbient(sunDir);
    m_Renderer.SetViewMatrix(m_Camera.GetViewMatrix());
    m_Sky.Render(m_Renderer);
    m_Sun.Render(m_Renderer);
    m_Moon.Render(m_Renderer);
//    m_ChunkManager.Render(m_Renderer, m_Sky.GetColor(), m_Sun.IsDay() ? m_Sun : m_Moon);
    m_ChunkManager.Render(m_Renderer, m_SkyColor, m_Sun.IsDay() ? m_Sun : m_Moon,
                          m_Sun.IsDay() ? -sunDir : sunDir, m_AmbientStrength);
    if (m_ShowGui)
        m_GuiManager.Render(m_Renderer);
}

void Game::HandleInput() {
    m_Camera.HandleInput(m_KeyPressed);
    if (m_KeyPressed[GLFW_KEY_F]) {
        m_Camera.ToggleFlyMode();
        m_VerticalVelocity = 0.0f;
        m_KeyPressed[GLFW_KEY_F] = false;
    }
    if (m_KeyPressed[GLFW_KEY_SPACE] && !m_Camera.GetFlyMode() && m_Ground) {
        Jump();
    }
    if (m_KeyPressed[GLFW_KEY_G]) {
        m_ShowGui = !m_ShowGui;
        m_KeyPressed[GLFW_KEY_G] = false;
    }
    if (m_KeyPressed[GLFW_KEY_P]) {
        m_Pause = !m_Pause;
        m_KeyPressed[GLFW_KEY_P] = false;
    }
}

void Game::Move(float deltaTime) {
    glm::vec3 &currentPosition = m_Camera.GetPlayerPosition();
    glm::vec3 playerSpeed = m_Camera.GetCameraSpeed();
    playerSpeed.y += m_VerticalVelocity;
    glm::vec3 distance = playerSpeed * deltaTime;
    glm::vec3 absDistance = {std::abs(distance.x), std::abs(distance.y), std::abs(distance.z)};

    bool collidedx, collidedy, collidedz;
    if (absDistance.x < absDistance.y && absDistance.x < absDistance.z) {
        collidedx = m_ChunkManager.CalculateCollision({distance.x, 0, 0});
        if (absDistance.y < absDistance.z) {
            collidedy = m_ChunkManager.CalculateCollision({0, distance.y, 0});
            collidedz = m_ChunkManager.CalculateCollision({0, 0, distance.z});
        } else {
            collidedz = m_ChunkManager.CalculateCollision({0, 0, distance.z});
            collidedy = m_ChunkManager.CalculateCollision({0, distance.y, 0});
        }
    } else if (absDistance.y < absDistance.z) {
        collidedy = m_ChunkManager.CalculateCollision({0, distance.y, 0});
        if (absDistance.x < absDistance.z) {
            collidedx = m_ChunkManager.CalculateCollision({distance.x, 0, 0});
            collidedz = m_ChunkManager.CalculateCollision({0, 0, distance.z});
        } else {
            collidedz = m_ChunkManager.CalculateCollision({0, 0, distance.z});
            collidedx = m_ChunkManager.CalculateCollision({distance.x, 0, 0});
        }
    } else {
        collidedz = m_ChunkManager.CalculateCollision({0, 0, distance.z});
        if (absDistance.x < absDistance.y) {
            collidedx = m_ChunkManager.CalculateCollision({distance.x, 0, 0});
            collidedy = m_ChunkManager.CalculateCollision({0, distance.y, 0});
        } else {
            collidedy = m_ChunkManager.CalculateCollision({0, distance.y, 0});
            collidedx = m_ChunkManager.CalculateCollision({distance.x, 0, 0});
        }
    }

    if (collidedx)
        distance.x = 0;
    if (collidedy) {
        if (!m_Camera.GetFlyMode() && distance.y < 0) {
            m_Ground = true;
        }
        distance.y = 0;
    } else {
        m_Ground = false;
    }
    if (collidedz)
        distance.z = 0;

    currentPosition += distance;

    // don't let the player go outside of world borders
    if (currentPosition.y < 0)
        currentPosition.y = 0;
    else if (currentPosition.y > m_ChunkManager.GetChunkSize()[1] - 1)
        currentPosition.y = m_ChunkManager.GetChunkSize()[1] - 1;
}

void Game::ApplyGravity(float deltaTime) {
    if (!m_Camera.GetFlyMode()) {
        if (!m_Ground)
            m_VerticalVelocity += -physics::GRAVITY * deltaTime;
        else
            m_VerticalVelocity = -physics::GRAVITY * deltaTime;
    }
}

void Game::Jump() {
    static constexpr float jump = 10.0f;
    m_VerticalVelocity = jump;
    m_Ground = false;
}

void Game::CheckRayCast() {
    glm::vec3 &playerPos = m_Camera.GetPlayerPosition();
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

    if (voxelFound) {
        if (m_KeyPressed[GLFW_MOUSE_BUTTON_LEFT]) {
            m_ChunkManager.DestroyBlock();
            m_KeyPressed[GLFW_MOUSE_BUTTON_LEFT] = false;
        } else if (m_KeyPressed[GLFW_MOUSE_BUTTON_RIGHT]) {
            m_ChunkManager.PlaceBlock(m_Blocks[m_HoldingBlock]);
            m_KeyPressed[GLFW_MOUSE_BUTTON_RIGHT] = false;
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
    m_Sky.SetPosition({static_cast<float>(m_Width) / 2.0f, 0.0f, static_cast<float>(m_Height) / 2.0f});
    m_Sky.SetScale({m_Width, 0.0f, m_Height});
}

void Game::UpdateFPS(uint32_t numFrames) {
    std::string fps = std::to_string(numFrames);
    std::string ms = std::to_string(1000.0 / numFrames);
    LOG_INFO("{} FPS / {} ms", fps, ms);
//    std::string newTitle = "Minecraft 2 - " + fps + "FPS / " + ms + "ms";
//    m_Window.ChangeTitle(newTitle);
}

void Game::SetSkyColor(const glm::vec4& topColor, const glm::vec4& bottomColor) {
    static constexpr glm::vec3 upVector = {0, 1, 0};
    const glm::vec3 frontVector = m_Camera.GetFrontVector();
//    float d = std::max(glm::dot(upVector, frontVector), 0.0f);
    if (frontVector.y >= 0) {
        float d = glm::dot(upVector, frontVector);
        glm::vec4 resultBottomColor = d * topColor + (1 - d) * bottomColor;
        m_Sky.SetColor(topColor, resultBottomColor);
    } else {
        static constexpr float gradStrenght = 0.8f;
        float d = - glm::dot(upVector, frontVector) * gradStrenght;
        glm::vec4 resultTopColor = d * bottomColor + (1 - d) * topColor;
        m_Sky.SetColor(resultTopColor, bottomColor);
    }
}

void Game::UpdateSkyColor(const glm::vec3& sunDir) {
    static constexpr glm::vec3 upVector = {0, 1, 0};
    float d = glm::dot(upVector, sunDir);

    if (d >= 0) {
        glm::vec4 resultBottomColor = d * SkyColors::bc1 + (1 - d) * SkyColors::bc2;
        glm::vec4 resultTopColor = d * SkyColors::tc1 + (1 - d) * SkyColors::tc2;
        SetSkyColor(resultTopColor, resultBottomColor);
    } else {
        glm::vec4 resultBottomColor = -d * SkyColors::tc3 + (1 + d) * SkyColors::bc2;
        glm::vec4 resultTopColor = -d * SkyColors::tc3 + (1 + d) * SkyColors::tc2;
        SetSkyColor(resultTopColor, resultBottomColor);
    }
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
