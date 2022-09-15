#pragma once
#include "../entities/GuiEntity.h"

class GuiManager {
public:
    void Init(int width, int height);
    void MakeGui(int width, int height);
    void Render(Renderer& renderer);
    void Resize(int width, int height);
    void OnNotify(int key);

private:
    std::vector<std::unique_ptr<GuiEntity>> m_GuiElements;
    float m_SquareBase;
};
