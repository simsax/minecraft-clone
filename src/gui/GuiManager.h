#pragma once
#include "../graphics/GuiRenderer.h"
#include "../entities/Entity.hpp"

class GuiManager {
public:
    void Init(int width, int height);
    void Render(GuiRenderer& renderer);
    void Resize(int width, int height);
    void OnNotify(int key);

private:
    void MakeGui(int width, int height);

    std::vector<Entity<glm::vec2>> m_GuiElements;
    float m_SquareBase;
};
