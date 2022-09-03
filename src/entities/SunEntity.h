#pragma once

#include "QuadEntity.h"

class SunEntity : public QuadEntity {
public:
    SunEntity(std::string name, std::string texturePath, const glm::vec3 &position,
              const glm::vec3 &scale);
    void IncrTime(float deltaTime);
    void Render(Renderer &renderer) override;
private:
    float m_Time;
};
