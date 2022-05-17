#pragma once

#include <string>
#include "glm/glm.hpp"

struct Aabb
{
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;

    // debug
    std::string Print() const
    {
        return "minX: " + std::to_string(minX) + ", maxX: " + std::to_string(maxX) + "\n" +
               "minY: " + std::to_string(minY) + ", maxY: " + std::to_string(maxY) + "\n" +
               "minZ: " + std::to_string(minZ) + ", maxZ: " + std::to_string(maxZ) + "\n";
    }
};

namespace physics
{
    Aabb CreateBlockAabb(glm::vec3 position);
    Aabb CreatePlayerAabb(glm::vec3 position);
    bool Intersect(Aabb a, Aabb b);
    void SnapAabb(Aabb player, Aabb block, const glm::vec3 &playerSpeed, glm::vec3 *currentPosition);
    float AabbDistance(Aabb a, Aabb b);
};
