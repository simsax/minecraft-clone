#include "Light.h"

static constexpr int WEST = 0;
static constexpr int NORTH = 1;
static constexpr int EAST = 2;
static constexpr int SOUTH = 3;

static int mod(int a, int b)
{
    int res = a % b;
    return res >= 0 ? res : res + b;
}

void Light::UpdateLightPlacedQueue(std::queue<LightAddNode>& queue, uint8_t lightLevel, uint8_t i,
    uint8_t j, uint8_t k, Chunk* chunk, Channel channel, ChunkSet& chunksToUpload)
{
    if (chunk != nullptr) {
        switch (channel) {
        case Channel::SUN:
            if (ChunkManager::IsTransparent(chunk->GetBlock(i, j, k))
                && chunk->GetSunLight(i, j, k) < lightLevel - 1) {
                chunk->SetSunLight(i, j, k, lightLevel - 1);
                queue.emplace(chunk, chunk->GetIndex(i, j, k));
            }
            break;
        case Channel::RED:
            if (ChunkManager::IsTransparent(chunk->GetBlock(i, j, k))
                && chunk->GetRedLight(i, j, k) < lightLevel - 1) {
                chunk->SetRedLight(i, j, k, lightLevel - 1);
                queue.emplace(chunk, chunk->GetIndex(i, j, k));
            }
            break;
        case Channel::GREEN:
            if (ChunkManager::IsTransparent(chunk->GetBlock(i, j, k))
                && chunk->GetGreenLight(i, j, k) < lightLevel - 1) {
                chunk->SetGreenLight(i, j, k, lightLevel - 1);
                queue.emplace(chunk, chunk->GetIndex(i, j, k));
            }
            break;
        case Channel::BLUE:
            if (ChunkManager::IsTransparent(chunk->GetBlock(i, j, k))
                && chunk->GetBlueLight(i, j, k) < lightLevel - 1) {
                chunk->SetBlueLight(i, j, k, lightLevel - 1);
                queue.emplace(chunk, chunk->GetIndex(i, j, k));
            }
            break;
        }
        chunksToUpload.insert(chunk->GetCoord());
    }
}

void Light::SunBFS(std::queue<LightAddNode> lightQueue, ChunkSet& chunksToUpload)
{
    while (!lightQueue.empty()) {
        LightAddNode& node = lightQueue.front();
        Chunk* chunk = node.chunk;
        uint16_t index = node.index;
        lightQueue.pop();
        glm::uvec3 coords = chunk->GetCoordsFromIndex(index);
        uint8_t x = coords[0];
        uint8_t y = coords[1];
        uint8_t z = coords[2];
        uint8_t lightLevel;
        lightLevel = chunk->GetSunLight(x, y, z);
        Chunk* other = chunk;

        int westX = x - 1;
        if (westX < 0) {
            other = other->GetNeighbors()[WEST];
            westX = mod(westX, XSIZE);
        }
        if (other != nullptr && ChunkManager::IsTransparent(other->GetBlock(westX, y, z))
            && other->GetSunLight(westX, y, z) < lightLevel - 1) {
            other->SetSunLight(westX, y, z, lightLevel - 1);
            lightQueue.emplace(other, other->GetIndex(westX, y, z));
            chunksToUpload.insert(other->GetCoord());
        }

        int eastX = x + 1;
        other = chunk;
        if (eastX >= XSIZE) {
            other = other->GetNeighbors()[EAST];
            eastX = mod(eastX, XSIZE);
        }
        if (other != nullptr && ChunkManager::IsTransparent(other->GetBlock(eastX, y, z))
            && other->GetSunLight(eastX, y, z) < lightLevel - 1) {
            other->SetSunLight(eastX, y, z, lightLevel - 1);
            lightQueue.emplace(other, other->GetIndex(eastX, y, z));
            chunksToUpload.insert(other->GetCoord());
        }

        int northZ = z - 1;
        other = chunk;
        if (northZ < 0) {
            other = other->GetNeighbors()[NORTH];
            northZ = mod(northZ, ZSIZE);
        }
        if (other != nullptr && ChunkManager::IsTransparent(other->GetBlock(x, y, northZ))
            && other->GetSunLight(x, y, northZ) < lightLevel - 1) {
            other->SetSunLight(x, y, northZ, lightLevel - 1);
            lightQueue.emplace(other, other->GetIndex(x, y, northZ));
            chunksToUpload.insert(other->GetCoord());
        }

        int southZ = z + 1;
        other = chunk;
        if (southZ >= ZSIZE) {
            other = other->GetNeighbors()[SOUTH];
            southZ = mod(southZ, ZSIZE);
        }
        if (other != nullptr && ChunkManager::IsTransparent(other->GetBlock(x, y, southZ))
            && other->GetSunLight(x, y, southZ) < lightLevel - 1) {
            other->SetSunLight(x, y, southZ, lightLevel - 1);
            lightQueue.emplace(other, other->GetIndex(x, y, southZ));
            chunksToUpload.insert(other->GetCoord());
        }

        other = chunk;
        int downY = y - 1;
        if (downY >= 0) {
            if (ChunkManager::IsTransparent(other->GetBlock(x, downY, z))
                && other->GetSunLight(x, downY, z) < lightLevel) {
                other->SetSunLight(x, downY, z, lightLevel == 15 ? lightLevel : lightLevel - 1);
                lightQueue.emplace(other, other->GetIndex(x, downY, z));
                chunksToUpload.insert(other->GetCoord());
            }
        }
    }
}

void Light::LightPlacedBFS(
    std::queue<LightAddNode> lightQueue, Channel channel, ChunkSet& chunksToUpload)
{
    while (!lightQueue.empty()) {
        LightAddNode& node = lightQueue.front();
        Chunk* chunk = node.chunk;
        uint16_t index = node.index;
        lightQueue.pop();
        glm::uvec3 coords = chunk->GetCoordsFromIndex(index);
        uint8_t x = coords[0];
        uint8_t y = coords[1];
        uint8_t z = coords[2];
        uint8_t lightLevel;
        switch (channel) {
        case Channel::SUN:
            lightLevel = chunk->GetSunLight(x, y, z);
            break;
        case Channel::RED:
            lightLevel = chunk->GetRedLight(x, y, z);
            break;
        case Channel::GREEN:
            lightLevel = chunk->GetGreenLight(x, y, z);
            break;
        case Channel::BLUE:
            lightLevel = chunk->GetBlueLight(x, y, z);
            break;
        }
        Chunk* other = chunk;

        int westX = x - 1;
        if (westX < 0) {
            other = other->GetNeighbors()[WEST];
            westX = mod(westX, XSIZE);
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, westX, y, z, other, channel, chunksToUpload);

        int eastX = x + 1;
        other = chunk;
        if (eastX >= XSIZE) {
            other = other->GetNeighbors()[EAST];
            eastX = mod(eastX, XSIZE);
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, eastX, y, z, other, channel, chunksToUpload);

        int northZ = z - 1;
        other = chunk;
        if (northZ < 0) {
            other = other->GetNeighbors()[NORTH];
            northZ = mod(northZ, ZSIZE);
        }
        UpdateLightPlacedQueue(
            lightQueue, lightLevel, x, y, northZ, other, channel, chunksToUpload);

        int southZ = z + 1;
        other = chunk;
        if (southZ >= ZSIZE) {
            other = other->GetNeighbors()[SOUTH];
            southZ = mod(southZ, ZSIZE);
        }
        UpdateLightPlacedQueue(
            lightQueue, lightLevel, x, y, southZ, other, channel, chunksToUpload);

        other = chunk;
        int downY = y - 1;
        if (downY >= 0) {
            if (channel == Channel::SUN) {
                if (ChunkManager::IsTransparent(other->GetBlock(x, downY, z))
                    && other->GetSunLight(x, downY, z) < lightLevel) {
                    other->SetSunLight(x, downY, z, lightLevel);
                    lightQueue.emplace(other, other->GetIndex(x, downY, z));
                    chunksToUpload.insert(other->GetCoord());
                }

            } else {
                UpdateLightPlacedQueue(
                    lightQueue, lightLevel, x, downY, z, other, channel, chunksToUpload);
            }
        }

        int upY = y + 1;
        if (upY < YSIZE)
            UpdateLightPlacedQueue(
                lightQueue, lightLevel, x, upY, z, other, channel, chunksToUpload);
    }
}

void Light::UpdateLightRemovedQueue(std::queue<LightAddNode>& placeQueue,
    std::queue<LightRemNode>& removeQueue, int lightLevel, uint8_t i, uint8_t j, uint8_t k,
    Chunk* chunk, Channel channel, ChunkSet& chunksToUpload)
{
    switch (channel) {
    case Channel::SUN: {
        uint8_t neighborLevel = chunk->GetSunLight(i, j, k);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            chunk->SetSunLight(i, j, k, 0);
            removeQueue.emplace(chunk, chunk->GetIndex(i, j, k), neighborLevel);
        } else if (neighborLevel >= lightLevel) {
            placeQueue.emplace(chunk, chunk->GetIndex(i, j, k));
        }
        break;
    }
    case Channel::RED: {
        uint8_t neighborLevel = chunk->GetRedLight(i, j, k);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            chunk->SetRedLight(i, j, k, 0);
            removeQueue.emplace(chunk, chunk->GetIndex(i, j, k), neighborLevel);
        } else if (neighborLevel >= lightLevel) {
            placeQueue.emplace(chunk, chunk->GetIndex(i, j, k));
        }
        break;
    }
    case Channel::GREEN: {
        uint8_t neighborLevel = chunk->GetGreenLight(i, j, k);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            chunk->SetGreenLight(i, j, k, 0);
            removeQueue.emplace(chunk, chunk->GetIndex(i, j, k), neighborLevel);
        } else if (neighborLevel >= lightLevel) {
            placeQueue.emplace(chunk, chunk->GetIndex(i, j, k));
        }
        break;
    }
    case Channel::BLUE: {
        uint8_t neighborLevel = chunk->GetBlueLight(i, j, k);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            chunk->SetBlueLight(i, j, k, 0);
            removeQueue.emplace(chunk, chunk->GetIndex(i, j, k), neighborLevel);
        } else if (neighborLevel >= lightLevel) {
            placeQueue.emplace(chunk, chunk->GetIndex(i, j, k));
        }
        break;
    }
    }
    chunksToUpload.insert(chunk->GetCoord());
}

std::queue<LightAddNode> Light::LightRemovedBFS(
    std::queue<LightRemNode> lightRemQueue, Channel channel, ChunkSet& chunksToUpload)
{
    std::queue<LightAddNode> lightAddQueue;
    while (!lightRemQueue.empty()) {
        LightRemNode& node = lightRemQueue.front();
        Chunk* chunk = node.chunk;
        uint16_t index = node.index;
        uint8_t lightLevel = node.val;
        lightRemQueue.pop();
        glm::uvec3 coords = chunk->GetCoordsFromIndex(index);
        uint8_t x = coords[0];
        uint8_t y = coords[1];
        uint8_t z = coords[2];
        Chunk* other = chunk;

        int westX = x - 1;
        if (westX < 0) {
            other = other->GetNeighbors()[WEST];
            westX = mod(westX, XSIZE);
        }
        UpdateLightRemovedQueue(
            lightAddQueue, lightRemQueue, lightLevel, westX, y, z, other, channel, chunksToUpload);

        int eastX = x + 1;
        other = chunk;
        if (eastX >= XSIZE) {
            other = other->GetNeighbors()[EAST];
            eastX = mod(eastX, XSIZE);
        }
        UpdateLightRemovedQueue(
            lightAddQueue, lightRemQueue, lightLevel, eastX, y, z, other, channel, chunksToUpload);

        int northZ = z - 1;
        other = chunk;
        if (northZ < 0) {
            other = other->GetNeighbors()[NORTH];
            northZ = mod(northZ, ZSIZE);
        }
        UpdateLightRemovedQueue(
            lightAddQueue, lightRemQueue, lightLevel, x, y, northZ, other, channel, chunksToUpload);

        int southZ = z + 1;
        other = chunk;
        if (southZ >= ZSIZE) {
            other = other->GetNeighbors()[SOUTH];
            southZ = mod(southZ, ZSIZE);
        }
        UpdateLightRemovedQueue(
            lightAddQueue, lightRemQueue, lightLevel, x, y, southZ, other, channel, chunksToUpload);

        other = chunk;
        int downY = y - 1;
        if (downY >= 0) {
            if (channel == Channel::SUN && lightLevel == 15) {
                uint8_t neighborLevel = chunk->GetSunLight(x, downY, z);
                chunk->SetSunLight(x, downY, z, 0);
                lightRemQueue.emplace(chunk, chunk->GetIndex(x, downY, z), neighborLevel);
            } else {
                UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, downY, z,
                    other, channel, chunksToUpload);
            }
        }

        int upY = y + 1;
        if (upY < YSIZE)
            UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, upY, z, other,
                channel, chunksToUpload);
    }
    return lightAddQueue;
}

void Light::UpdateLightOpaque(Chunk* chunk, Chunk* other, uint8_t x, uint8_t y, uint8_t z,
    uint8_t otherX, uint8_t otherY, uint8_t otherZ)
{
    uint8_t lightVal = other->GetRedLight(otherX, otherY, otherZ);
    if (lightVal > chunk->GetRedLight(x, y, z) + 1)
        chunk->SetRedLight(x, y, z, lightVal - 1);
    lightVal = other->GetGreenLight(otherX, otherY, otherZ);
    if (lightVal > chunk->GetGreenLight(x, y, z) + 1)
        chunk->SetGreenLight(x, y, z, lightVal - 1);
    lightVal = other->GetBlueLight(otherX, otherY, otherZ);
    if (lightVal > chunk->GetBlueLight(x, y, z) + 1)
        chunk->SetBlueLight(x, y, z, lightVal - 1);
    lightVal = other->GetSunLight(otherX, otherY, otherZ);
    if (otherY == y + 1 && lightVal == 15) {
        chunk->SetSunLight(x, y, z, lightVal);
    } else if (lightVal > chunk->GetSunLight(x, y, z) + 1) {
        chunk->SetSunLight(x, y, z, lightVal - 1);
    }
}

void Light::UpdateOpaqueBlockLight(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z)
{
    Chunk* other = chunk;

    int westX = x - 1;
    if (westX < 0) {
        other = other->GetNeighbors()[WEST];
        westX = mod(westX, XSIZE);
    }
    UpdateLightOpaque(chunk, other, x, y, z, westX, y, z);

    int eastX = x + 1;
    other = chunk;
    if (eastX >= XSIZE) {
        other = other->GetNeighbors()[EAST];
        eastX = mod(eastX, XSIZE);
    }
    UpdateLightOpaque(chunk, other, x, y, z, eastX, y, z);

    int northZ = z - 1;
    other = chunk;
    if (northZ < 0) {
        other = other->GetNeighbors()[NORTH];
        northZ = mod(northZ, ZSIZE);
    }
    UpdateLightOpaque(chunk, other, x, y, z, x, y, northZ);

    int southZ = z + 1;
    other = chunk;
    if (southZ >= ZSIZE) {
        other = other->GetNeighbors()[SOUTH];
        southZ = mod(southZ, ZSIZE);
    }
    UpdateLightOpaque(chunk, other, x, y, z, x, y, southZ);

    other = chunk;
    int downY = y - 1;
    if (downY >= 0)
        UpdateLightOpaque(chunk, other, x, y, z, x, downY, z);

    int upY = y + 1;
    if (upY < YSIZE)
        UpdateLightOpaque(chunk, other, x, y, z, x, upY, z);
}

void Light::AddSunLight(Chunk* chunk, ChunkSet& chunksToUpload)
{
    chunk->FillSunLight();
    uint8_t top = chunk->GetMaxHeight() + 1; // 1 block over the highest opaque block
    std::queue<LightAddNode> sunQueue;
    for (int i = 0; i < XSIZE; i++) {
        for (int k = 0; k < ZSIZE; k++) {
            sunQueue.emplace(chunk, chunk->GetIndex(i, top, k));
        }
    }
    SunBFS(sunQueue, chunksToUpload);
}

void Light::AddSunLightSimplified(Chunk* chunk, ChunkSet& chunksToUpload)
{
    chunk->FillSunLight();
    std::array<Chunk*, 4> neighbors = chunk->GetNeighbors();
    std::array<bool, 4> updated = { false };
    for (int i = 0; i < XSIZE; i++) {
        for (int k = 0; k < ZSIZE; k++) {
            uint8_t y = chunk->GetMaxHeight() + 1;
            for (auto& neighbor : neighbors) {
                if (neighbor->GetMaxHeight() + 1 > y)
                    y = neighbor->GetMaxHeight() + 1;
            }
            while (ChunkManager::IsTransparent(chunk->GetBlock(i, --y, k))) {
                // not a cache friendly iteration, but I can't come up with anything better rn
                chunk->SetSunLight(i, y, k, 15);
            }
        }
    }
    for (auto& neighbor : neighbors)
        chunksToUpload.insert(neighbor->GetCoord());
    chunksToUpload.insert(chunk->GetCoord());
}