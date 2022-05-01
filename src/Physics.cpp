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

float physics::SweptAabb(Aabb player, Aabb block, glm::vec3 playerSpeed, bool& x_collision, bool& y_collision, bool& z_collision) {
    float xInvEntry, yInvEntry, zInvEntry;
    float xInvExit, yInvExit, zInvExit;

    if (playerSpeed.x >= 0.0f) {
        xInvEntry = block.minX - player.maxX;
        xInvExit = block.maxX - player.minX;
    } else {
        xInvExit = block.minX - player.maxX;
        xInvEntry = block.maxX - player.minX;
    }
    if (playerSpeed.y >= 0.0f) {
        yInvEntry = block.minY - player.maxY;
        yInvExit = block.maxY - player.minY;
    } else {
        yInvExit = block.minY - player.maxY;
        yInvEntry = block.maxY - player.minY;
    }
    if (playerSpeed.z >= 0.0f) {
        zInvEntry = block.minZ - player.maxZ;
        zInvExit = block.maxZ - player.minZ;
    } else {
        zInvExit = block.minZ - player.maxZ;
        zInvEntry = block.maxZ - player.minZ;
    }
    // find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
    float xEntry, yEntry, zEntry;
    float xExit, yExit, zExit;

    if (std::abs(playerSpeed.x) < 0.0001f)
    {
        xEntry = -std::numeric_limits<float>::infinity();
        xExit = std::numeric_limits<float>::infinity();
    }
    else
    {
        xEntry = xInvEntry / playerSpeed.x;
        xExit = xInvExit / playerSpeed.x;
    }
    if (std::abs(playerSpeed.y) < 0.0001f)
    {
        yEntry = -std::numeric_limits<float>::infinity();
        yExit = std::numeric_limits<float>::infinity();
    }
    else
    {
        yEntry = yInvEntry / playerSpeed.y;
        yExit = yInvExit / playerSpeed.y;
    }
    if (std::abs(playerSpeed.z) < 0.0001f)
    {
        zEntry = -std::numeric_limits<float>::infinity();
        zExit = std::numeric_limits<float>::infinity();
    }
    else
    {
        zEntry = zInvEntry / playerSpeed.z;
        zExit = zInvExit / playerSpeed.z;
    }
    // find which axis collided first
    float entryTime = std::max({ xEntry, yEntry, zEntry });
    float exitTime = std::min({ xExit, yExit, zExit });
    // if there was no collision
    if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f && zEntry < 0.0f ||
        xEntry > 1.0f || yEntry > 1.0f || zEntry > 1.0f)
    {
        x_collision = false;
        y_collision = false;
        z_collision = false;
        return 1.0f;
    }
    else // if there was a collision
    {
        // calculate normal of collided surface
        if (xEntry > yEntry && xEntry > zEntry) {
            x_collision = true;
            y_collision = false;
            z_collision = false;
        }
        else if (yEntry > zEntry) {
            x_collision = false;
            y_collision = true;
            z_collision = false;
        }
        else {
            x_collision = false;
            y_collision = false;
            z_collision = true;
        }
        // return the time of collision
        return entryTime;
    }
}

float physics::AabbDistance(Aabb a, Aabb b) {
    return std::sqrt(
            (b.minX - a.minX) * (b.minX - a.minX) +
            (b.minY - a.minY) * (b.minY - a.minY) +
            (b.minZ - a.minZ) * (b.minZ - a.minZ)
            );
}
