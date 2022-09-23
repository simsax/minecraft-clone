#include "GuiManager.h"
#include "Config.h"

#define CURSOR_SCALE 20.0f
#define BAR_HEIGHT 80.0f
#define BAR_WIDTH (9 * BAR_HEIGHT)
#define ACTIVE_SQUARE_SCALE 90.0f
#define BAR_OFFSET 79.0f
#define BLOCK_SIZE 40.0f

void GuiManager::Init(int width, int height) {
    MakeGui(width, height);
}

void GuiManager::MakeGui(int width, int height) {
    float offset = (BAR_WIDTH - BAR_HEIGHT) / 2;
    m_SquareBase = static_cast<float>(width) / 2.0f - offset + 4.0f;
    std::string baseDir = std::string(SOURCE_DIR) + "/res/textures/gui/";
    m_GuiElements.emplace_back("cursor", baseDir + "cursor.png",
                               glm::vec2(CURSOR_SCALE, CURSOR_SCALE),
                               glm::vec2(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f));

    m_GuiElements.emplace_back("bar", baseDir + "bottom_bar.png", glm::vec2(BAR_WIDTH, BAR_HEIGHT),
                               glm::vec2(static_cast<float>(width) / 2.0f, BAR_HEIGHT / 2.0f));

    m_GuiElements.emplace_back("active_square", baseDir + "active_square.png",
                               glm::vec2(ACTIVE_SQUARE_SCALE, ACTIVE_SQUARE_SCALE),
                               glm::vec2(m_SquareBase, BAR_HEIGHT / 2.0f));

    m_GuiElements.emplace_back("dirt", baseDir + "dirt.png", glm::vec2(BLOCK_SIZE, BLOCK_SIZE),
                               glm::vec2(m_SquareBase, BAR_HEIGHT / 2.0f));

    m_GuiElements.emplace_back("grass", baseDir + "grass.png", glm::vec2(BLOCK_SIZE, BLOCK_SIZE),
                               glm::vec2(m_SquareBase + BAR_OFFSET, BAR_HEIGHT / 2.0f));

    m_GuiElements.emplace_back("sand", baseDir + "sand.png", glm::vec2(BLOCK_SIZE, BLOCK_SIZE),
                               glm::vec2(m_SquareBase + BAR_OFFSET * 2, BAR_HEIGHT / 2.0f));

    m_GuiElements.emplace_back("snow", baseDir + "snow.png", glm::vec2(BLOCK_SIZE, BLOCK_SIZE),
                               glm::vec2(m_SquareBase + BAR_OFFSET * 3, BAR_HEIGHT / 2.0f));

    m_GuiElements.emplace_back("stone", baseDir + "stone.png", glm::vec2(BLOCK_SIZE, BLOCK_SIZE),
                               glm::vec2(m_SquareBase + BAR_OFFSET * 4, BAR_HEIGHT / 2.0f));

    m_GuiElements.emplace_back("wood", baseDir + "wood.png", glm::vec2(BLOCK_SIZE, BLOCK_SIZE),
                               glm::vec2(m_SquareBase + BAR_OFFSET * 5, BAR_HEIGHT / 2.0f));

    m_GuiElements.emplace_back("diamond", baseDir + "diamond.png",
                               glm::vec2(BLOCK_SIZE, BLOCK_SIZE),
                               glm::vec2(m_SquareBase + BAR_OFFSET * 6, BAR_HEIGHT / 2.0f));
}

void GuiManager::Render(GuiRenderer& renderer) {
    for (auto &gui: m_GuiElements) {
        if (gui.GetName() == "cursor")
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
        renderer.Render(gui.GetTexture(), gui.GetPosition(), gui.GetScale());
        if (gui.GetName() == "cursor")
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
void GuiManager::Resize(int width, int height) {
    m_GuiElements.clear();
    MakeGui(width, height);
}

void GuiManager::OnNotify(int key) {
    glm::vec2 squarePosition = m_GuiElements[2].GetPosition();
    squarePosition.x = m_SquareBase + BAR_OFFSET * key;
    m_GuiElements[2].SetPosition(squarePosition);
}
