#pragma once
#include "GuiRenderer.h"
#include "Gui.h"

class GuiManager {
public:
    void Init(int width, int height);
    void MakeGui(int width, int height);
    void Render();
    void PressKey(int key);
    void Resize(int width, int height);

private:
    GuiRenderer m_GuiRenderer;
    std::vector<Gui> m_GuiElements;
    float m_SquareBase;
};
