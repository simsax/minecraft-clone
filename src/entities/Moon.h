#pragma once

#include "Sun.h"

class Moon : public Sun {
public:
    Moon(std::string name, std::string texturePath, const glm::vec3 &position,
        const glm::vec3 &scale, const glm::vec3& color = glm::vec3(1.0f));
    void Render(Renderer &renderer) override;
    void SetPosition(const glm::vec3 &position) override;
};
