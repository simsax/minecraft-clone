#include "GuiManager.h"
#include "../entities/CursorEntity.h"

#define CURSOR_SCALE 20.0f
#define BAR_HEIGHT 80.0f
#define BAR_WIDTH (9 * BAR_HEIGHT)
#define ACTIVE_SQUARE_SCALE 90.0f
#define BAR_OFFSET 79.0f
#define BLOCK_SIZE 40.0f

void GuiManager::Init(int width, int height) {
    GuiEntity::InitShaders("shader_gui.vert", "shader_quad.frag");
    MakeGui(width, height);
}

void GuiManager::MakeGui(int width, int height) {
    float offset = (BAR_WIDTH - BAR_HEIGHT) / 2;
    m_SquareBase = static_cast<float>(width) / 2.0f - offset + 4.0f;

    m_GuiElements.push_back(std::make_unique<CursorEntity>("cursor", "cursor.png", glm::vec3{
                                                                   static_cast<float>(width) / 2.0f, 0.0f,
                                                                   static_cast<float>(height) / 2.0f},
                                                           glm::vec3{CURSOR_SCALE, 0.0f,
                                                                     CURSOR_SCALE}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("bar", "bottom_bar.png",
                                                        glm::vec3{static_cast<float>(width) / 2.0f,
                                                                  0.0f, BAR_HEIGHT / 2.0f},
                                                        glm::vec3{BAR_WIDTH, 0.0f, BAR_HEIGHT}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("active_square", "active_square.png",
                                                        glm::vec3{m_SquareBase, 0.0f,
                                                                  BAR_HEIGHT / 2.0f},
                                                        glm::vec3{ACTIVE_SQUARE_SCALE, 0.0f,
                                                                  ACTIVE_SQUARE_SCALE}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("dirt", "dirt.png",
                                                        glm::vec3{m_SquareBase, 0.0f,
                                                                  BAR_HEIGHT / 2.0f},
                                                        glm::vec3{BLOCK_SIZE, 0.0f, BLOCK_SIZE}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("grass", "grass.png",
                                                        glm::vec3{m_SquareBase + BAR_OFFSET, 0.0f,
                                                                  BAR_HEIGHT / 2.0f},
                                                        glm::vec3{BLOCK_SIZE, 0.0f, BLOCK_SIZE}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("sand", "sand.png",
                                                        glm::vec3{m_SquareBase + BAR_OFFSET * 2,
                                                                  0.0f, BAR_HEIGHT / 2.0f},
                                                        glm::vec3{BLOCK_SIZE, 0.0f, BLOCK_SIZE}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("snow", "snow.png",
                                                        glm::vec3{m_SquareBase + BAR_OFFSET * 3,
                                                                  0.0f, BAR_HEIGHT / 2.0f},
                                                        glm::vec3{BLOCK_SIZE, 0.0f, BLOCK_SIZE}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("stone", "stone.png",
                                                        glm::vec3{m_SquareBase + BAR_OFFSET * 4,
                                                                  0.0f, BAR_HEIGHT / 2.0f},
                                                        glm::vec3{BLOCK_SIZE, 0.0f, BLOCK_SIZE}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("wood", "wood.png",
                                                        glm::vec3{m_SquareBase + BAR_OFFSET * 5,
                                                                  0.0f, BAR_HEIGHT / 2.0f},
                                                        glm::vec3{BLOCK_SIZE, 0.0f, BLOCK_SIZE}));

    m_GuiElements.push_back(std::make_unique<GuiEntity>("diamond", "diamond.png",
                                                        glm::vec3{m_SquareBase + BAR_OFFSET * 6,
                                                                  0.0f, BAR_HEIGHT / 2.0f},
                                                        glm::vec3{BLOCK_SIZE, 0.0f, BLOCK_SIZE}));

    for (auto &gui: m_GuiElements) {
        gui->InitTexture();
    }
}

void GuiManager::Render(Renderer &renderer) {
    for (auto &gui: m_GuiElements) {
        gui->Render(renderer);
    }
}

void GuiManager::PressKey(int key) {
    glm::vec3 squarePosition = m_GuiElements[2]->GetPosition();
    squarePosition.x = m_SquareBase + BAR_OFFSET * key;
    m_GuiElements[2]->SetPosition({squarePosition.x, 0, squarePosition.z});
}

void GuiManager::Resize(int width, int height) {
    m_GuiElements.clear();
    MakeGui(width, height);
}
