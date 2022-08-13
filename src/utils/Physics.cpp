#include "Physics.h"

#define BLOCK_SIZE 1.0f
#define PLAYER_HALF_WIDTH 0.3f
#define PLAYER_TOP_HEIGHT 0.2f
#define PLAYER_BOTTOM_HEIGHT 1.6f
#define CHUNK_HALF_WIDTH 7.0f
#define CHUNK_HEIGHT 255.0f

namespace physics {

std::string Aabb::Print() const
{
    return "minX: " + std::to_string(minX) + ", maxX: " + std::to_string(maxX) + "\n"
        + "minY: " + std::to_string(minY) + ", maxY: " + std::to_string(maxY) + "\n"
        + "minZ: " + std::to_string(minZ) + ", maxZ: " + std::to_string(maxZ) + "\n";
}

glm::vec3 Aabb::GetPositiveVertex(const glm::vec3& normal) const
{
    glm::vec3 p = { minX, minY, minZ };
    if (normal.x >= 0)
        p.x = maxX;
    if (normal.y >= 0)
        p.y = maxY;
    if (normal.z >= 0)
        p.z = maxZ;
    return p;
}

Aabb CreateBlockAabb(const glm::vec3& position)
{
    Aabb bbox = { position.x, position.x + BLOCK_SIZE, position.y, position.y + BLOCK_SIZE,
        position.z, position.z + BLOCK_SIZE };
    return bbox;
}

Aabb CreatePlayerAabb(const glm::vec3& position)
{
    Aabb bbox = { position.x - PLAYER_HALF_WIDTH, position.x + PLAYER_HALF_WIDTH,
        position.y - PLAYER_BOTTOM_HEIGHT, position.y + PLAYER_TOP_HEIGHT,
        position.z - PLAYER_HALF_WIDTH, position.z + PLAYER_HALF_WIDTH };
    return bbox;
}

Aabb CreateChunkAabb(const glm::vec3& position)
{
    Aabb bbox = { position.x - CHUNK_HALF_WIDTH, position.x + CHUNK_HALF_WIDTH, position.y,
        position.y + CHUNK_HEIGHT, position.z - CHUNK_HALF_WIDTH, position.z + CHUNK_HALF_WIDTH };
    return bbox;
}

bool Intersect(Aabb a, Aabb b)
{
    return (a.minX <= b.maxX && a.maxX >= b.minX) && (a.minY <= b.maxY && a.maxY >= b.minY)
        && (a.minZ <= b.maxZ && a.maxZ >= b.minZ);
}

void SnapAabb(Aabb player, Aabb block, const glm::vec3& playerSpeed, glm::vec3* currentPosition)
{
    constexpr float eps = 0.0001f;
    if (playerSpeed.x > 0)
        currentPosition->x += block.minX - player.maxX - eps;
    else if (playerSpeed.x < 0)
        currentPosition->x += block.maxX - player.minX + eps;
    if (playerSpeed.y > 0)
        currentPosition->y += block.minY - player.maxY - eps;
    else if (playerSpeed.y < 0)
        currentPosition->y += block.maxY - player.minY + eps;
    if (playerSpeed.z > 0)
        currentPosition->z += block.minZ - player.maxZ - eps;
    else if (playerSpeed.z < 0)
        currentPosition->z += block.maxZ - player.minZ + eps;
}

float AabbDistance(Aabb a, Aabb b)
{
    return std::sqrt((b.minX - a.minX) * (b.minX - a.minX) + (b.minY - a.minY) * (b.minY - a.minY)
        + (b.minZ - a.minZ) * (b.minZ - a.minZ));
}
}
