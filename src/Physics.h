#pragma once

#include "glm/glm.hpp"
#include <string>

namespace physics {
struct Aabb {
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;

    // debug
    std::string Print() const;
    glm::vec3 GetPositiveVertex(glm::vec3 normal) const;
};

Aabb CreateBlockAabb(glm::vec3 position);
Aabb CreatePlayerAabb(glm::vec3 position);
Aabb CreateChunkAabb(glm::vec3 position);
bool Intersect(Aabb a, Aabb b);
void SnapAabb(Aabb player, Aabb block, const glm::vec3& playerSpeed, glm::vec3* currentPosition);
float AabbDistance(Aabb a, Aabb b);
};
