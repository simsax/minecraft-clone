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

static constexpr uint8_t log2Z = MyLog2(ZSIZE);
static constexpr uint8_t log2XZ = MyLog2(XSIZE * ZSIZE);

static constexpr int WEST = 0;
static constexpr int NORTH = 1;
static constexpr int EAST = 2;
static constexpr int SOUTH = 3;

static int mod(int a, int b) {
    int res = a % b;
    return res >= 0 ? res : res + b;
}


static int MakeIndex(int i, int j, int k) {
    return (j << log2XZ) + (i << log2Z) + k;
}

ChunkManager::ChunkManager(Camera *camera)
        : m_ChunkSize({XSIZE, YSIZE, ZSIZE}), m_ViewDistance(VIEW_DISTANCE),
          m_Camera(camera), m_SortChunks(true), m_ChunksReadyToMesh(false),
          m_Raycast({}), m_LastChunk({0, 0}), m_CurrentChunk({0, 0}), m_Stride(0) {

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
        else
            m_ChunksToRender.emplace_back(chunk);
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

static bool IsLight(Block block) {
    return block == Block::LIGHT_BLUE ||
           block == Block::LIGHT_RED;
}

void ChunkManager::DestroyBlock() {
    uint8_t x = m_Raycast.localVoxel[0];
    uint8_t y = m_Raycast.localVoxel[1];
    uint8_t z = m_Raycast.localVoxel[2];
    Block block = m_Raycast.chunk->GetBlock(x, y, z);
    if (block != Block::BEDROCK) {
        m_Raycast.chunk->SetBlock(x, y, z, Block::EMPTY);
        if (IsLight(block)) {
            m_Raycast.chunk->SetTorchLight(x, y, z, 0);
            std::queue<LightRemNode> lightRemQueue;
            uint16_t startIndex = MakeIndex(x, y, z);
            lightRemQueue.emplace(m_Raycast.chunk, startIndex, 15);
            LightRemovedBFS(std::move(lightRemQueue));
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
        // if block is light source
        if (IsLight(block)) {
            chunk->SetTorchLight(x, y, z, 15);
            std::queue<LightAddNode> lightQueue;
            uint16_t startIndex = MakeIndex(x, y, z);
            lightQueue.emplace(chunk, startIndex);
            LightPlacedBFS(std::move(lightQueue));
        }
        m_ChunksToUpload.insert(chunkCoord);
        // check if the target is in the chunk border
        UpdateNeighbors(localVoxel, chunkCoord);
    }

}

void
ChunkManager::UpdateLightPlacedQueue(std::queue<LightAddNode> &queue, int lightLevel, uint8_t i,
                                     uint8_t j,
                                     uint8_t k,
                                     Chunk *chunk) {
    if (/*IsTransparent(chunk->GetBlock(i, j, k)) && */chunk->GetTorchLight(i, j, k) <
                                                       lightLevel - 1) {
        chunk->SetTorchLight(i, j, k, lightLevel - 1);
        queue.emplace(chunk, MakeIndex(i, j, k));
    }
}

void ChunkManager::LightPlacedBFS(std::queue<LightAddNode> lightQueue) {
    while (!lightQueue.empty()) {
        LightAddNode &node = lightQueue.front();
        Chunk *chunk = node.chunk;
        uint16_t index = node.index;
        lightQueue.pop();
        int z = index & 0xF;
        int x = index >> log2Z & 0xF;
        int y = index >> log2XZ;
        int lightLevel = chunk->GetTorchLight(x, y, z);
        Chunk *other = chunk;

        int westX = x - 1;
        if (westX < 0) {
            other = other->GetNeighbors()[WEST];
            westX = mod(westX, XSIZE);
            m_ChunksToUpload.insert(other->GetCoord());
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, westX, y, z, other);

        int eastX = x + 1;
        other = chunk;
        if (eastX >= XSIZE) {
            other = other->GetNeighbors()[EAST];
            eastX = mod(eastX, XSIZE);
            m_ChunksToUpload.insert(other->GetCoord());
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, eastX, y, z, other);

        int northZ = z - 1;
        other = chunk;
        if (northZ < 0) {
            other = other->GetNeighbors()[NORTH];
            northZ = mod(northZ, ZSIZE);
            m_ChunksToUpload.insert(other->GetCoord());
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, x, y, northZ, other);

        int southZ = z + 1;
        other = chunk;
        if (southZ >= ZSIZE) {
            other = other->GetNeighbors()[SOUTH];
            southZ = mod(southZ, ZSIZE);
            m_ChunksToUpload.insert(other->GetCoord());
        }
        UpdateLightPlacedQueue(lightQueue, lightLevel, x, y, southZ, other);

        other = chunk;
        int downY = y - 1;
        if (downY >= 0)
            UpdateLightPlacedQueue(lightQueue, lightLevel, x, downY, z, other);

        int upY = y + 1;
        if (upY < YSIZE)
            UpdateLightPlacedQueue(lightQueue, lightLevel, x, upY, z, other);
    }
}

void
ChunkManager::UpdateLightRemovedQueue(std::queue<LightAddNode> &placeQueue,
                                      std::queue<LightRemNode> &removeQueue,
                                      int lightLevel, uint8_t i, uint8_t j, uint8_t k,
                                      Chunk *chunk) {
    uint8_t neighborLevel = chunk->GetTorchLight(i, j, k);
    if (neighborLevel != 0 && neighborLevel < lightLevel) {
        chunk->SetTorchLight(i, j, k, 0);
        removeQueue.emplace(chunk, MakeIndex(i, j, k), neighborLevel);
    } else if (neighborLevel >= lightLevel) {
        placeQueue.emplace(chunk, MakeIndex(i, j, k));
    }
}

void ChunkManager::LightRemovedBFS(std::queue<LightRemNode> lightRemQueue) {
    std::queue<LightAddNode> lightAddQueue;
    while (!lightRemQueue.empty()) {
        LightRemNode &node = lightRemQueue.front();
        Chunk *chunk = node.chunk;
        uint16_t index = node.index;
        uint8_t lightLevel = node.val;
        lightRemQueue.pop();
        int z = index & 0xF;
        int x = index >> log2Z & 0xF;
        int y = index >> log2XZ;
        Chunk *other = chunk;

        int westX = x - 1;
        if (westX < 0) {
            other = other->GetNeighbors()[WEST];
            westX = mod(westX, XSIZE);
            m_ChunksToUpload.insert(other->GetCoord());
        }
        UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, westX, y, z, other);

        int eastX = x + 1;
        other = chunk;
        if (eastX >= XSIZE) {
            other = other->GetNeighbors()[EAST];
            eastX = mod(eastX, XSIZE);
            m_ChunksToUpload.insert(other->GetCoord());
        }
        UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, eastX, y, z, other);

        int northZ = z - 1;
        other = chunk;
        if (northZ < 0) {
            other = other->GetNeighbors()[NORTH];
            northZ = mod(northZ, ZSIZE);
            m_ChunksToUpload.insert(other->GetCoord());
        }
        UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, y, northZ, other);

        int southZ = z + 1;
        other = chunk;
        if (southZ >= ZSIZE) {
            other = other->GetNeighbors()[SOUTH];
            southZ = mod(southZ, ZSIZE);
            m_ChunksToUpload.insert(other->GetCoord());
        }
        UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, y, southZ, other);

        other = chunk;
        int downY = y - 1;
        if (downY >= 0)
            UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, downY, z, other);

        int upY = y + 1;
        if (upY < YSIZE)
            UpdateLightRemovedQueue(lightAddQueue, lightRemQueue, lightLevel, x, upY, z, other);
    }
    LightPlacedBFS(std::move(lightAddQueue));
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
