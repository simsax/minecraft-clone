#include "ChunkManager.h"
#include "glm/gtx/norm.hpp"
#include <iostream>
#include "../utils/Timer.h"
#include "../utils/Logger.h"
#include "Config.h"

using namespace std::chrono_literals;

#ifndef NDEBUG
#define VIEW_DISTANCE 4
#else
#define VIEW_DISTANCE 24 // how far the player sees
#endif
#define MAX_CHUNK_TO_LOAD 8

static constexpr int WEST = 0;
static constexpr int NORTH = 1;
static constexpr int EAST = 2;
static constexpr int SOUTH = 3;

static int mod(int a, int b) {
    int res = a % b;
    return res >= 0 ? res : res + b;
}

ChunkManager::ChunkManager(Camera *camera)
        : m_ChunkSize({XSIZE, YSIZE, ZSIZE}), m_ViewDistance(VIEW_DISTANCE),
          m_Camera(camera), m_SortChunks(true), m_ChunksReadyToMesh(false),
          m_Raycast({}), m_LastChunk({0, 0}), m_CurrentChunk({0, 0}), m_Stride(0) {
    m_LightBlocks[Block::LIGHT_RED] = {15, 0, 0};
    m_LightBlocks[Block::LIGHT_GREEN] = {0, 15, 0};
    m_LightBlocks[Block::LIGHT_BLUE] = {0, 0, 15};
    m_ChunksToRender.reserve(
            static_cast<uint32_t>((m_ViewDistance * 2 + 1) * (m_ViewDistance * 2 + 1)));
}

void ChunkManager::InitWorld(uint32_t stride) {
    m_Stride = stride;
    GenerateChunks();
    while (!m_ChunksToLoad.empty())
        LoadChunks();
    while (m_ChunksReadyToMesh && !m_ChunksToMesh.empty())
        MeshChunks();
}

void ChunkManager::Render(ChunkRenderer &renderer) {
    auto &cameraPos = m_Camera->GetCameraPosition();
    LoadChunks();
    MeshChunks();
    if (m_SortChunks) {
        m_SortChunks = false;
        SortChunks(cameraPos);
    }

    // frustum culling
    m_Camera->UpdateFrustum();
    ChunkCoord playerChunk = CalculateChunkCoord(cameraPos);
    static constexpr int spireradius = 20;
    for (Chunk *chunk: m_ChunksToRender) {
        glm::vec3 center = chunk->GetCenterPosition();
        if (m_Camera->IsInFrustum(center))
            chunk->Render(renderer, playerChunk, spireradius);
    }

    if (m_Raycast.selected) {
        m_Raycast.chunk->RenderOutline(renderer, m_Raycast.localVoxel);
        Chunk *chunk = m_Raycast.chunk;
        glm::uvec3 &vox = m_Raycast.localVoxel;
        uint8_t x = vox.x;
        uint8_t y = vox.y;
        uint8_t z = vox.z;
        LOG_INFO("({}, {}, {}, {})", chunk->GetSunLight(x, y, z), chunk->GetRedLight(x, y, z),
                 m_Raycast.chunk->GetGreenLight(x, y, z), m_Raycast.chunk->GetBlueLight(x, y, z));
    }
}

glm::vec3 ChunkManager::GetChunkSize() const { return m_ChunkSize; }

ChunkCoord ChunkManager::CalculateChunkCoord(const glm::vec3 &position) const {
    int chunkPosX = static_cast<int>(std::floor(position.x / XSIZE));
    int chunkPosZ = static_cast<int>(std::floor(position.z / ZSIZE));
    return {chunkPosX, chunkPosZ};
}

void ChunkManager::LoadChunks() {
    for (int n = 0; !m_ChunksToLoad.empty() && n < MAX_CHUNK_TO_LOAD; n++) {
        ChunkCoord coords = m_ChunksToLoad.front();
        m_ChunksToLoad.pop();
        m_SortChunks = true;
        //  create new chunk and cache it
        BlockVec blockList = {};
        if (const auto it = m_BlocksToSet.find(coords); it != m_BlocksToSet.end()) {
            blockList = it->second;
            m_BlocksToSet.erase(it);
        }

        Chunk chunk(coords, {coords.x * static_cast<int>(m_ChunkSize[0]), 0.0f,
                             coords.z * static_cast<int>(m_ChunkSize[2])}, m_Stride, &m_ChunkMap);
        BlockVec blocksToSet = chunk.CreateSurfaceLayer(blockList);
        AddBlocks(coords, blocksToSet);
        m_ChunkMap.insert({coords, std::move(chunk)});
        m_ChunksToMesh.push(&m_ChunkMap.at(coords));

        if (m_ChunksToLoad.empty())
            m_ChunksReadyToMesh = true;
    }
}

void ChunkManager::MeshChunks() {
    bool uploaded = false;
    while (!m_ChunksToUpload.empty()) {
        auto iterator = m_ChunksToUpload.begin();
        auto node = m_ChunksToUpload.extract(iterator);
        ChunkCoord coords = node.value();
        auto chunk = &m_ChunkMap.at(coords);
        chunk->ClearMesh();
        chunk->GenerateMesh();
        uploaded = true;
    }

    for (int n = 0; !uploaded && m_ChunksReadyToMesh && !m_ChunksToMesh.empty() &&
                    n < MAX_CHUNK_TO_LOAD; n++) {
        Chunk *chunk = m_ChunksToMesh.front();
        m_ChunksToMesh.pop();
        if (!chunk->GenerateMesh())
            m_ChunksInBorder.push(chunk);
        else {
            m_ChunksToRender.emplace_back(chunk);
            LightPlacedBFS(std::move(chunk->GetSunQueueRef()), Channel::SUN);
//            // mesh needs to be regenerated after the bfs
//            chunk->ClearMesh();
//            chunk->GenerateMesh();
        }
    }
    if (m_ChunksToMesh.empty()) {
        m_ChunksReadyToMesh = false;
        m_ChunksToMesh = std::move(m_ChunksInBorder);
    }
}

void ChunkManager::GenerateChunks() {
//    std::cout << "Chunks in RAM: " << m_ChunkMap.size() << "\n";
    ChunkCoord playerChunk = CalculateChunkCoord(m_Camera->GetCameraPosition());

    // remove chunks outside of render distance (should be further than this when I'll add serialization)
//    for (auto& chunk : m_ChunksToRender) {
//        if (chunk->NotVisible(playerChunk, m_ViewDistance))
//            m_ChunkMap.erase(chunk->GetCoord());
//    }
    m_ChunksToRender.clear();

    // load chunks
    for (int i = -m_ViewDistance + playerChunk.x; i <= m_ViewDistance + playerChunk.x; i++) {
        for (int j = -m_ViewDistance + playerChunk.z; j <= m_ViewDistance + playerChunk.z; j++) {
            ChunkCoord coords = {i, j};
            // check if this chunk hasn't already been generated
            if (!m_ChunkMap.contains(coords)) {
                // add chunk to the loading queue
                m_ChunksToLoad.push(coords);
            } else {
                m_ChunksToRender.emplace_back(&m_ChunkMap.at(coords));
            }
        }
    }
}

void ChunkManager::SortChunks(const glm::vec3 &cameraPos) {
    glm::vec3 playerPos = cameraPos * glm::vec3(1.0f, 0, 1.0f);
    std::sort(m_ChunksToRender.begin(), m_ChunksToRender.end(), [&playerPos](Chunk *a, Chunk *b) {
        return glm::length2(playerPos - a->GetCenterPosition())
               > glm::length2(playerPos - b->GetCenterPosition());
    });
}

bool ChunkManager::IsBlockCastable(const glm::vec3 &voxel) {
    std::pair<ChunkCoord, glm::uvec3> target = GlobalToLocal(voxel);
    m_Raycast.prevGlobalVoxel = m_Raycast.globalVoxel;
    m_Raycast.globalVoxel = voxel;
    m_Raycast.prevChunk = m_Raycast.chunk;
    m_Raycast.prevChunkCoord = m_Raycast.chunkCoord;
    m_Raycast.chunkCoord = target.first;
    m_Raycast.prevLocalVoxel = m_Raycast.localVoxel;
    m_Raycast.localVoxel = target.second;
    if (m_Raycast.localVoxel.y >= 0 && m_Raycast.localVoxel.y < YSIZE) {
        if (const auto it = m_ChunkMap.find(m_Raycast.chunkCoord); it != m_ChunkMap.end()) {
            m_Raycast.chunk = &it->second;
            Block block = m_Raycast.chunk->GetBlock(m_Raycast.localVoxel[0],
                                                    m_Raycast.localVoxel[1],
                                                    m_Raycast.localVoxel[2]);
            if (block != Block::EMPTY &&
                block != Block::WATER) {
                m_Raycast.selected = true;
                return true;
            }
        }
    }
    m_Raycast.selected = false;
    return false;
}

static bool IsTransparent(Block block) {
    return block == Block::EMPTY ||
           block == Block::BUSH ||
           block == Block::LEAVES ||
           block == Block::FLOWER_YELLOW ||
           block == Block::FLOWER_BLUE ||
           block == Block::WATER;
}

void ChunkManager::DestroyBlock() {
    uint8_t x = m_Raycast.localVoxel[0];
    uint8_t y = m_Raycast.localVoxel[1];
    uint8_t z = m_Raycast.localVoxel[2];
    Block block = m_Raycast.chunk->GetBlock(x, y, z);
    if (block != Block::BEDROCK) {
        m_Raycast.chunk->SetBlock(x, y, z, Block::EMPTY);
        if (m_LightBlocks.contains(block)) {
            glm::uvec3 lightLevel = m_LightBlocks[block];
            // red
            if (lightLevel[0] != 0) {
                m_Raycast.chunk->SetRedLight(x, y, z, 0);
                std::queue<LightRemNode> redRemQueue;
                uint16_t startIndex = m_Raycast.chunk->GetIndex(x, y, z);
                redRemQueue.emplace(m_Raycast.chunk, startIndex, lightLevel[0]);
                std::queue<LightAddNode> redQ = LightRemovedBFS(std::move(redRemQueue),
                                                                Channel::RED);
                LightPlacedBFS(std::move(redQ), Channel::RED);
            }
            // green
            if (lightLevel[1] != 0) {
                m_Raycast.chunk->SetGreenLight(x, y, z, 0);
                std::queue<LightRemNode> greenRemQueue;
                uint16_t startIndex = m_Raycast.chunk->GetIndex(x, y, z);
                greenRemQueue.emplace(m_Raycast.chunk, startIndex, lightLevel[1]);
                std::queue<LightAddNode> greenQ = LightRemovedBFS(std::move(greenRemQueue),
                                                                  Channel::GREEN);
                LightPlacedBFS(std::move(greenQ), Channel::GREEN);
            }
            // blue
            if (lightLevel[2] != 0) {
                m_Raycast.chunk->SetBlueLight(x, y, z, 0);
                std::queue<LightRemNode> blueRemQueue;
                uint16_t startIndex = m_Raycast.chunk->GetIndex(x, y, z);
                blueRemQueue.emplace(m_Raycast.chunk, startIndex, lightLevel[2]);
                std::queue<LightAddNode> blueQ = LightRemovedBFS(std::move(blueRemQueue),
                                                                 Channel::BLUE);
                LightPlacedBFS(std::move(blueQ), Channel::BLUE);
            }
        }
        m_ChunksToUpload.insert(m_Raycast.chunkCoord);
        // check if the target is in the chunk border
        UpdateNeighbors(m_Raycast.localVoxel, m_Raycast.chunkCoord);
    }
}

void ChunkManager::PlaceBlock(Block block) {
    glm::uvec3 localVoxel = m_Raycast.localVoxel;
    ChunkCoord chunkCoord = m_Raycast.chunkCoord;
    glm::vec3 globalVoxel = m_Raycast.globalVoxel;
    Chunk *chunk = m_Raycast.chunk;
    Block target = chunk->GetBlock(localVoxel[0], localVoxel[1], localVoxel[2]);
    if (target != Block::FLOWER_BLUE &&
        target != Block::FLOWER_YELLOW &&
        target != Block::BUSH) {
        localVoxel = m_Raycast.prevLocalVoxel;
        chunkCoord = m_Raycast.prevChunkCoord;
        globalVoxel = m_Raycast.prevGlobalVoxel;
        chunk = m_Raycast.prevChunk;
    }
    uint8_t x = localVoxel[0];
    uint8_t y = localVoxel[1];
    uint8_t z = localVoxel[2];
    if (!physics::Intersect(
            physics::CreatePlayerAabb(m_Camera->GetCameraPosition()),
            physics::CreateBlockAabb(globalVoxel))) {
        chunk->SetBlock(x, y, z, block);
        // if block is a light source
        if (m_LightBlocks.contains(block)) {
            glm::uvec3 lightLevel = m_LightBlocks[block];
            // red
            if (lightLevel[0] != 0) {
                chunk->SetRedLight(x, y, z, lightLevel[0]);
                std::queue<LightAddNode> redQueue;
                uint16_t startIndex = m_Raycast.chunk->GetIndex(x, y, z);
                redQueue.emplace(chunk, startIndex);
                LightPlacedBFS(std::move(redQueue), Channel::RED);
            }
            // green
            if (lightLevel[1] != 0) {
                chunk->SetGreenLight(x, y, z, lightLevel[1]);
                std::queue<LightAddNode> greenQueue;
                uint16_t startIndex = m_Raycast.chunk->GetIndex(x, y, z);
                greenQueue.emplace(chunk, startIndex);
                LightPlacedBFS(std::move(greenQueue), Channel::GREEN);
            }
            // blue
            if (lightLevel[2] != 0) {
                chunk->SetBlueLight(x, y, z, lightLevel[2]);
                std::queue<LightAddNode> blueQueue;
                uint16_t startIndex = m_Raycast.chunk->GetIndex(x, y, z);
                blueQueue.emplace(chunk, startIndex);
                LightPlacedBFS(std::move(blueQueue), Channel::BLUE);
            }
        }
        m_ChunksToUpload.insert(chunkCoord);
        // check if the target is in the chunk border
        UpdateNeighbors(localVoxel, chunkCoord);
    }

}

void
ChunkManager::UpdateLightPlacedQueue(std::queue<LightAddNode> &queue, uint8_t lightLevel, uint8_t i,
                                     uint8_t j, uint8_t k, Chunk *chunk, Channel channel) {
    if (chunk->FindNeighbors()) {
        switch (channel) {
            case Channel::SUN:
                if (/*IsTransparent(chunk->GetBlock(i, j, k)) && */chunk->GetSunLight(i, j, k) <
                                                                   lightLevel - 1) {
                    chunk->SetSunLight(i, j, k, lightLevel - 1);
                    queue.emplace(chunk, chunk->GetIndex(i, j, k));
                }
                break;
            case Channel::RED:
                if (/*IsTransparent(chunk->GetBlock(i, j, k)) && */chunk->GetRedLight(i, j, k) <
                                                                   lightLevel - 1) {
                    chunk->SetRedLight(i, j, k, lightLevel - 1);
                    queue.emplace(chunk, chunk->GetIndex(i, j, k));
                }
                break;
            case Channel::GREEN:
                if (/*IsTransparent(chunk->GetBlock(i, j, k)) && */chunk->GetGreenLight(i, j, k) <
                                                                   lightLevel - 1) {
                    chunk->SetGreenLight(i, j, k, lightLevel - 1);
                    queue.emplace(chunk, chunk->GetIndex(i, j, k));
                }
                break;
            case Channel::BLUE:
                if (/*IsTransparent(chunk->GetBlock(i, j, k)) && */chunk->GetBlueLight(i, j, k) <
                                                                   lightLevel - 1) {
                    chunk->SetBlueLight(i, j, k, lightLevel - 1);
                    queue.emplace(chunk, chunk->GetIndex(i, j, k));
                }
                break;
        }
        m_ChunksToUpload.insert(chunk->GetCoord());
    }
}

void ChunkManager::LightPlacedBFS(std::queue<LightAddNode> lightQueue, Channel channel) {
    while (!lightQueue.empty()) {
        LightAddNode &node = lightQueue.front();
        Chunk *chunk = node.chunk;
        uint16_t index = node.index;
        lightQueue.pop();
        glm::uvec3 coords = chunk->GetCoordsFromIndex(index);
        uint8_t x = coords[0];
        uint8_t y = coords[1];
        uint8_t z = coords[2];
        LOG_INFO("index: {}, coords: ({},{},{})", index, x, y, z);
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
        Chunk *other = chunk;

        int westX = x - 1;
        if (westX < 0) {
            other = other->GetNeighbors()[WEST];
            westX = mod(westX, XSIZE);
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, westX, y, z, other, channel);

        int eastX = x + 1;
        other = chunk;
        if (eastX >= XSIZE) {
            other = other->GetNeighbors()[EAST];
            eastX = mod(eastX, XSIZE);
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, eastX, y, z, other, channel);

        int northZ = z - 1;
        other = chunk;
        if (northZ < 0) {
            other = other->GetNeighbors()[NORTH];
            northZ = mod(northZ, ZSIZE);
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, x, y, northZ, other, channel);

        int southZ = z + 1;
        other = chunk;
        if (southZ >= ZSIZE) {
            other = other->GetNeighbors()[SOUTH];
            southZ = mod(southZ, ZSIZE);
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, x, y, southZ, other, channel);

        other = chunk;
        int downY = y - 1;
        if (downY >= 0) {
            if (channel == Channel::SUN) {
                if (IsTransparent(other->GetBlock(x, downY, z)) &&
                    other->GetSunLight(x, downY, z) <
                    lightLevel) {
                    other->SetSunLight(x, downY, z, lightLevel);
                    lightQueue.emplace(other, other->GetIndex(x, downY, z));
                    m_ChunksToUpload.insert(other->GetCoord());
                }

            } else {
                UpdateLightPlacedQueue(lightQueue, lightLevel, x, downY, z, other, channel);
            }
        }

        int upY = y + 1;
        if (upY < YSIZE)
            UpdateLightPlacedQueue(lightQueue, lightLevel, x, upY, z, other, channel);
    }
}

void
ChunkManager::UpdateLightRemovedQueue(std::queue<LightAddNode> &placeQueue,
                                      std::queue<LightRemNode> &removeQueue,
                                      int lightLevel, uint8_t i, uint8_t j, uint8_t k,
                                      Chunk *chunk, Channel channel) {
    switch (channel) {
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
    m_ChunksToUpload.insert(chunk->GetCoord());
}

std::queue<LightAddNode>
ChunkManager::LightRemovedBFS(std::queue<LightRemNode> lightRemQueue,
                              ChunkManager::Channel channel) {
    std::queue<LightAddNode> lightAddQueue;
    while (!lightRemQueue.empty()) {
        LightRemNode &node = lightRemQueue.front();
        Chunk *chunk = node.chunk;
        uint16_t index = node.index;
        uint8_t lightLevel = node.val;
        lightRemQueue.pop();
        glm::uvec3 coords = chunk->GetCoordsFromIndex(index);
        uint8_t x = coords[0];
        uint8_t y = coords[1];
        uint8_t z = coords[2];
        Chunk *other = chunk;

        int westX = x - 1;
        if (westX < 0) {
            other = other->GetNeighbors()[WEST];
            westX = mod(westX, XSIZE);
        }
        UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, westX, y, z, other,
                                channel);

        int eastX = x + 1;
        other = chunk;
        if (eastX >= XSIZE) {
            other = other->GetNeighbors()[EAST];
            eastX = mod(eastX, XSIZE);
        }
        UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, eastX, y, z, other,
                                channel);

        int northZ = z - 1;
        other = chunk;
        if (northZ < 0) {
            other = other->GetNeighbors()[NORTH];
            northZ = mod(northZ, ZSIZE);
        }
        UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, y, northZ, other,
                                channel);

        int southZ = z + 1;
        other = chunk;
        if (southZ >= ZSIZE) {
            other = other->GetNeighbors()[SOUTH];
            southZ = mod(southZ, ZSIZE);
        }
        UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, y, southZ, other,
                                channel);

        other = chunk;
        int downY = y - 1;
        if (downY >= 0)
            UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, downY, z, other,
                                    channel);

        int upY = y + 1;
        if (upY < YSIZE)
            UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, upY, z, other,
                                    channel);
    }
    return lightAddQueue;
}

std::pair<ChunkCoord, glm::uvec3>
ChunkManager::GlobalToLocal(const glm::vec3 &playerPosition) const {
    ChunkCoord chunkCoord = CalculateChunkCoord(playerPosition);
    uint32_t playerPosX =
            mod(static_cast<int>(std::floor(playerPosition.x)), m_ChunkSize[0]);
    uint32_t playerPosZ =
            mod(static_cast<int>(std::floor(playerPosition.z)), m_ChunkSize[2]);
    glm::uvec3 playerPos
            = {playerPosX, static_cast<uint32_t>(std::floor(playerPosition.y)), playerPosZ};
    return std::make_pair(chunkCoord, playerPos);
}

bool ChunkManager::IsBlockSolid(const glm::vec3 &globalCoords) const {
    std::pair<ChunkCoord, glm::vec3> localPos = GlobalToLocal(globalCoords);
    if (const auto it = m_ChunkMap.find(localPos.first); it != m_ChunkMap.end()) {
        const Chunk *chunk = &it->second;
        Block block = chunk->GetBlock(
                localPos.second[0], localPos.second[1], localPos.second[2]);
        if (block != Block::EMPTY &&
            block != Block::WATER &&
            block != Block::FLOWER_BLUE &&
            block != Block::FLOWER_YELLOW &&
            block != Block::BUSH) {
            return true;
        }
    }
    return false;
}

void ChunkManager::AddBlocks(const ChunkCoord &chunkCoord, BlockVec &blockVec) {
    for (auto &[block, voxel]: blockVec) {
        ChunkCoord neighbor = {
                chunkCoord.x + static_cast<int>(std::floor(voxel.x / XSIZE)),
                chunkCoord.z + static_cast<int>(std::floor(voxel.z / ZSIZE))
        };
        glm::uvec3 neighborVoxel = {mod(static_cast<int>(voxel.x), XSIZE),
                                    voxel.y,
                                    mod(static_cast<int>(voxel.z), ZSIZE)};

        if (const auto it = m_ChunkMap.find(neighbor); it != m_ChunkMap.end()) {
            // if chunk exists, update it
            Chunk *neighborChunk = &it->second;
            neighborChunk->SetBlock(neighborVoxel.x, neighborVoxel.y, neighborVoxel.z, block);
            m_ChunksToUpload.insert(neighbor);
        } else {
            // otherwise save the blocks to set for the chunk when it will be created
            m_BlocksToSet[neighbor].emplace_back(block, neighborVoxel);
        }
    }
}

void ChunkManager::UpdateNeighbors(const glm::uvec3 &voxel, const ChunkCoord &chunkCoord) {
    auto neighborCoord = chunkCoord;
    if (voxel.x == 0)
        neighborCoord.x -= 1;
    else if (voxel.x == XSIZE - 1)
        neighborCoord.x += 1;
    m_ChunksToUpload.insert(neighborCoord);
    m_ChunkMap.at(neighborCoord).UpdateMeshHeighLimit(voxel.y);

    neighborCoord = chunkCoord;
    if (voxel.z == 0)
        neighborCoord.z -= 1;
    else if (voxel.z == ZSIZE - 1)
        neighborCoord.z += 1;
    m_ChunksToUpload.insert(neighborCoord);
    m_ChunkMap.at(neighborCoord).UpdateMeshHeighLimit(voxel.y);
}

void ChunkManager::UpdateChunks() {
    ChunkCoord currentChunk = CalculateChunkCoord(m_Camera->GetCameraPosition());
    if (m_CurrentChunk != currentChunk) {
        m_CurrentChunk = currentChunk;
        m_SortChunks = true;
    }
    if (m_LastChunk - currentChunk >= m_ViewDistance / 3) {
        GenerateChunks();
        m_LastChunk = currentChunk;
    }
}
