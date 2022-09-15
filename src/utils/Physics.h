#pragma once

#include "glm/glm.hpp"
#include "../entities/Player.h"
#include <string>

#define BLOCK_SIZE 1.0f
#define PLAYER_HALF_WIDTH 0.3f
#define PLAYER_TOP_HEIGHT 0.2f
#define PLAYER_BOTTOM_HEIGHT 1.6f

template <typename T>
concept ChunkInfo = requires(T t, const glm::vec3& block)
{
    { t.IsBlockSolid(block) } -> std::same_as<bool>;
    { t.GetChunkSize() } -> std::same_as<glm::vec3>;
};

namespace physics {

    constexpr float GRAVITY = 37.0f;

    struct Aabb {
        float minX;
        float maxX;
        float minY;
        float maxY;
        float minZ;
        float maxZ;

        // debug
        std::string Print() const;

        glm::vec3 GetPositiveVertex(const glm::vec3 &normal) const;
    };

    Aabb CreateBlockAabb(const glm::vec3 &position);

    Aabb CreatePlayerAabb(const glm::vec3 &position);

    Aabb CreateChunkAabb(const glm::vec3 &position);

    bool Intersect(Aabb a, Aabb b);

    void
    SnapAabb(Aabb player, Aabb block, const glm::vec3 &playerSpeed, glm::vec3 &currentPosition);

    float AabbDistance(Aabb a, Aabb b);

    void ApplyGravity(Player &player, float deltaTime);

    template<typename T>
    requires ChunkInfo<T>
    void Spawn(Player &player, const T& chunkInfo);

    template<typename T>
    requires ChunkInfo<T>
    void UpdatePlayer(Player &player, const T &chunkInfo, float deltaTime);

    template<typename T>
    requires ChunkInfo<T>
    bool CalculateCollision(const glm::vec3 &playerSpeed, const T &chunkInfo,
                            glm::vec3 &currentPosition);

    template<typename T>
    requires ChunkInfo<T>
    void CollisionResolution(Player &player, const T &chunkInfo, float deltaTime);
};


template<typename T> requires ChunkInfo<T>
void physics::UpdatePlayer(Player& player, const T& chunkInfo, float deltaTime) {
    ApplyGravity(player, deltaTime);
    CollisionResolution(player, chunkInfo, deltaTime);
}

template<typename T>
requires ChunkInfo<T>
void physics::Spawn(Player &player, const T& chunkInfo) {
    static constexpr int water_level = 63;
    glm::vec3& playerPosition = player.GetPosition();
    int i = water_level;
    while (i < chunkInfo.GetChunkSize()[1] && chunkInfo.IsBlockSolid({0, i, 0})) i++;
    float y = i + 2;
    playerPosition.y = y;
    int x = 0;
    while (chunkInfo.IsBlockSolid({x, playerPosition.y, 0})) x++;
    playerPosition.x = x;
}


template<typename T> requires ChunkInfo<T>
bool physics::CalculateCollision(const glm::vec3 &playerSpeed, const T& chunkInfo, glm::vec3& currentPosition) {
    glm::vec3 finalPosition = currentPosition + playerSpeed;
    int startX, endX, startY, endY, startZ, endZ;
    if (finalPosition.x >= currentPosition.x) {
        startX = std::floor(currentPosition.x - PLAYER_HALF_WIDTH);
        endX = std::floor(finalPosition.x + PLAYER_HALF_WIDTH);
    } else {
        startX = std::floor(finalPosition.x - PLAYER_HALF_WIDTH);
        endX = std::floor(currentPosition.x + PLAYER_HALF_WIDTH);
    }
    if (finalPosition.y >= currentPosition.y) {
        startY = std::floor(currentPosition.y - PLAYER_BOTTOM_HEIGHT);
        endY = std::floor(finalPosition.y + PLAYER_TOP_HEIGHT);
    } else {
        startY = std::floor(finalPosition.y - PLAYER_BOTTOM_HEIGHT);
        endY = std::floor(currentPosition.y + PLAYER_TOP_HEIGHT);
    }
    if (finalPosition.z >= currentPosition.z) {
        startZ = std::floor(currentPosition.z - PLAYER_HALF_WIDTH);
        endZ = std::floor(finalPosition.z + PLAYER_HALF_WIDTH);
    } else {
        startZ = std::floor(finalPosition.z - PLAYER_HALF_WIDTH);
        endZ = std::floor(currentPosition.z + PLAYER_HALF_WIDTH);
    }

    Aabb playerBbox = CreatePlayerAabb(currentPosition);
    for (int i = startX; i <= endX; i++) {
        for (int j = startY; j <= endY; j++) {
            for (int k = startZ; k <= endZ; k++) {
                if (chunkInfo.IsBlockSolid({i, j, k})) {
                    Aabb blockBbox = CreateBlockAabb({i, j, k});
                    SnapAabb(playerBbox, blockBbox, playerSpeed, currentPosition);
                    return true;
                }
            }
        }
    }
    return false;
}

template<typename T> requires ChunkInfo<T>
void physics::CollisionResolution(Player& player, const T& chunkInfo, float deltaTime) {
    glm::vec3 &currentPosition = player.GetPosition();
    glm::vec3 playerSpeed = player.GetSpeed();
    playerSpeed.y += player.GetVerticalVelocity();
    glm::vec3 distance = playerSpeed * deltaTime;
    glm::vec3 absDistance = {std::abs(distance.x), std::abs(distance.y), std::abs(distance.z)};

    bool collidedx, collidedy, collidedz;
    if (absDistance.x < absDistance.y && absDistance.x < absDistance.z) {
        collidedx = CalculateCollision({distance.x, 0, 0}, chunkInfo, currentPosition);
        if (absDistance.y < absDistance.z) {
            collidedy = CalculateCollision({0, distance.y, 0}, chunkInfo, currentPosition);
            collidedz = CalculateCollision({0, 0, distance.z}, chunkInfo, currentPosition);
        } else {
            collidedz = CalculateCollision({0, 0, distance.z}, chunkInfo, currentPosition);
            collidedy = CalculateCollision({0, distance.y, 0}, chunkInfo, currentPosition);
        }
    } else if (absDistance.y < absDistance.z) {
        collidedy = CalculateCollision({0, distance.y, 0}, chunkInfo, currentPosition);
        if (absDistance.x < absDistance.z) {
            collidedx = CalculateCollision({distance.x, 0, 0}, chunkInfo, currentPosition);
            collidedz = CalculateCollision({0, 0, distance.z}, chunkInfo, currentPosition);
        } else {
            collidedz = CalculateCollision({0, 0, distance.z}, chunkInfo, currentPosition);
            collidedx = CalculateCollision({distance.x, 0, 0}, chunkInfo, currentPosition);
        }
    } else {
        collidedz = CalculateCollision({0, 0, distance.z}, chunkInfo, currentPosition);
        if (absDistance.x < absDistance.y) {
            collidedx = CalculateCollision({distance.x, 0, 0}, chunkInfo, currentPosition);
            collidedy = CalculateCollision({0, distance.y, 0}, chunkInfo, currentPosition);
        } else {
            collidedy = CalculateCollision({0, distance.y, 0}, chunkInfo, currentPosition);
            collidedx = CalculateCollision({distance.x, 0, 0}, chunkInfo, currentPosition);
        }
    }

    if (collidedx)
        distance.x = 0;
    if (collidedy) {
        if (!player.IsFlying() && distance.y < 0) {
            player.SetOnGround(true);
        }
        distance.y = 0;
    } else {
        player.SetOnGround(false);
    }
    if (collidedz)
        distance.z = 0;

    currentPosition += distance;

    // don't let the player go outside of world borders
    if (currentPosition.y < 0)
        currentPosition.y = 0;
    else if (currentPosition.y > chunkInfo.GetChunkSize()[1] - 1)
        currentPosition.y = chunkInfo.GetChunkSize()[1] - 1;

    player.ResetSpeed();
}
