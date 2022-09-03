#pragma once
#include "../entities/GuiEntity.h"

class GuiManager {
public:
    void Init(int width, int height);
    void MakeGui(int width, int height);
    void Render(Renderer& renderer);
    void PressKey(int key);
    void Resize(int width, int height);

private:
    std::vector<std::unique_ptr<GuiEntity>> m_GuiElements;
    float m_SquareBase;
};
