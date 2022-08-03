#pragma once
#include "GuiRenderer.h"
#include "Gui.h"

class GuiManager {
public:
    void Init();
    void Render();
    void Update();

private:
    GuiRenderer m_GuiRenderer;
    std::vector<Gui> m_GuiElements;
};