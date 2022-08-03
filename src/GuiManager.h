#pragma once
#include "GuiRenderer.h"
#include "Gui.h"

class GuiManager {
public:
    void Init();
    void Render();
    void Update();
    void PressKey(int key);

private:
    GuiRenderer m_GuiRenderer;
    std::vector<Gui> m_GuiElements;
    float m_SquareBase;
};