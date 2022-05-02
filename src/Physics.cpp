#include "Physics.h"

Aabb physics::CreateBlockAabb(glm::vec3 position) {
    Aabb bbox = { position.x, position.x + 1.0f,
                  position.y, position.y + 1.0f,
                  position.z, position.z + 1.0f};
    return bbox;
}

Aabb physics::CreatePlayerAabb(glm::vec3 position) {
    Aabb bbox = { position.x - 0.3f, position.x + 0.3f,
                  position.y - 1.6f, position.y + 0.2f,
                  position.z - 0.3f, position.z + 0.3f};
    return bbox;
}

bool physics::Intersect(Aabb a, Aabb b) {
    return (a.minX <= b.maxX && a.maxX >= b.minX) &&
           (a.minY <= b.maxY && a.maxY >= b.minY) &&
           (a.minZ <= b.maxZ && a.maxZ >= b.minZ);
}

void physics::SnapAabb(Aabb player, Aabb block, const glm::vec3& playerSpeed, glm::vec3* currentPosition) {
    float epsilon = 0.0001f;
    if (playerSpeed.x > 0)
        currentPosition->x += block.minX - player.maxX - epsilon;
    else if (playerSpeed.x < 0)
        currentPosition->x += block.maxX - player.minX + epsilon;
    if (playerSpeed.y > 0)
        currentPosition->y += block.minY - player.maxY - epsilon;
    else if (playerSpeed.y < 0)
        currentPosition->y += block.maxY - player.minY + epsilon;
    if (playerSpeed.z > 0)
        currentPosition->z += block.minZ - player.maxZ - epsilon;
    else if (playerSpeed.z < 0)
        currentPosition->z += block.maxZ - player.minZ + epsilon;
}

float physics::AabbDistance(Aabb a, Aabb b) {
    return std::sqrt(
            (b.minX - a.minX) * (b.minX - a.minX) +
            (b.minY - a.minY) * (b.minY - a.minY) +
            (b.minZ - a.minZ) * (b.minZ - a.minZ)
            );
}
