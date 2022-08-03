#include "GuiManager.h"
#include "Config.h"

#define HEIGHT 1080.0f
#define WIDTH 1920.0f
#define CURSOR_SCALE 30.0f
#define BAR_HEIGHT 100.0f
#define BAR_WIDTH 9 * BAR_HEIGHT
#define ACTIVE_SQUARE_SCALE 110.0f

void GuiManager::Init()
{
    m_GuiRenderer.Init();
    std::string baseDir = std::string(SOURCE_DIR) + "/res/textures/";
    m_GuiElements = { Gui("cursor", baseDir + "cursor.png", glm::vec2(CURSOR_SCALE, CURSOR_SCALE),
                          glm::vec2(WIDTH / 2.0f, HEIGHT / 2.0f)),

        Gui("bar", baseDir + "bottom_bar.png", glm::vec2(BAR_WIDTH, BAR_HEIGHT),
            glm::vec2(WIDTH / 2.0f, BAR_HEIGHT / 2.0f)),

        Gui("active_square", baseDir + "active_square.png",
            glm::vec2(ACTIVE_SQUARE_SCALE, ACTIVE_SQUARE_SCALE),
            glm::vec2(WIDTH / 2.0f - 395.0f, BAR_HEIGHT / 2.0f)) };
}

void GuiManager::Update() { }

void GuiManager::Render()
{
    for (const Gui& gui : m_GuiElements) {
        if (gui.GetName() == "cursor")
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
        m_GuiRenderer.Render(gui.GetTexture(), gui.GetPosition(), gui.GetScale());
        if (gui.GetName() == "cursor")
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}