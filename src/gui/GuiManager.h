#pragma once
#include "../graphics/GuiRenderer.h"
#include "../utils/Entity.hpp"

class GuiManager {
public:
    void Init(int width, int height);
    void Render(GuiRenderer& renderer);
    void Resize(int width, int height);
    void OnNotify(int key);

private:
    void MakeGui(int width, int height);

    std::vector<std::unique_ptr<Entity<glm::vec2, GuiRenderer>>> m_GuiElements;
    float m_SquareBase;
};
