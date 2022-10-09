#include "ChunkManager.h"
#include "glm/gtx/norm.hpp"
#include <iostream>
#include "../utils/Timer.h"
#include "../utils/Logger.h"
#include "Config.h"
#include "Light.h"

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

static int mod(int a, int b)
{
    int res = a % b;
    return res >= 0 ? res : res + b;
}

ChunkManager::ChunkManager(Camera* camera)
    : m_ChunkSize({ XSIZE, YSIZE, ZSIZE })
    , m_ViewDistance(VIEW_DISTANCE)
    , m_Camera(camera)
    , m_SortChunks(true)
    , m_ChunksReadyToMesh(false)
    , m_ChunksReadyToLight(false)
    , m_Raycast({})
    , m_LastChunk({ 0, 0 })
    , m_CurrentChunk({ 0, 0 })
    , m_Stride(0)
{
    m_LightBlocks[Block::LIGHT_RED] = { 15, 0, 0 };
    m_LightBlocks[Block::LIGHT_GREEN] = { 0, 15, 0 };
    m_LightBlocks[Block::LIGHT_BLUE] = { 0, 0, 15 };
    m_ChunksToRender.reserve(
        static_cast<uint32_t>((m_ViewDistance * 2 + 1) * (m_ViewDistance * 2 + 1)));
}

void ChunkManager::InitWorld(uint32_t stride)
{
    m_Stride = stride;
    GenerateChunks();
    while (!m_ChunksToLoad.empty())
        LoadChunks();
    while (m_ChunksReadyToLight && !m_ChunksToLight.empty())
        LightChunks();
    while (m_ChunksReadyToMesh && !m_ChunksToMesh.empty())
        MeshChunks();
}

void ChunkManager::Render(ChunkRenderer& renderer)
{
    auto& cameraPos = m_Camera->GetCameraPosition();
    LoadChunks();
    LightChunks();
    MeshChunks();
    if (m_SortChunks) {
        m_SortChunks = false;
        SortChunks(cameraPos);
    }

    // frustum culling
    m_Camera->UpdateFrustum();
    ChunkCoord playerChunk = CalculateChunkCoord(cameraPos);
    static constexpr int spireradius = 20;
    for (Chunk* chunk : m_ChunksToRender) {
        glm::vec3 center = chunk->GetCenterPosition();
        if (m_Camera->IsInFrustum(center))
            chunk->Render(renderer, playerChunk, spireradius);
    }

    if (m_Raycast.selected) {
        m_Raycast.chunk->RenderOutline(renderer, m_Raycast.localVoxel);
        // Chunk* chunk = m_Raycast.chunk;
        // glm::uvec3& vox = m_Raycast.localVoxel;
        // uint8_t x = vox.x;
        // uint8_t y = vox.y;
        // uint8_t z = vox.z;
        // LOG_INFO("({}, {}, {}, {})", chunk->GetSunLight(x, y + 1, z), chunk->GetRedLight(x, y,
        // z),
        //     m_Raycast.chunk->GetGreenLight(x, y, z), m_Raycast.chunk->GetBlueLight(x, y, z));
    }
}

glm::vec3 ChunkManager::GetChunkSize() const { return m_ChunkSize; }

ChunkCoord ChunkManager::CalculateChunkCoord(const glm::vec3& position) const
{
    int chunkPosX = static_cast<int>(std::floor(position.x / XSIZE));
    int chunkPosZ = static_cast<int>(std::floor(position.z / ZSIZE));
    return { chunkPosX, chunkPosZ };
}

void ChunkManager::LoadChunks()
{
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

        m_ChunkMap.emplace(coords,
            Chunk(coords,
                { coords.x * static_cast<int>(m_ChunkSize[0]), 0.0f,
                    coords.z * static_cast<int>(m_ChunkSize[2]) },
                m_Stride, &m_ChunkMap));
        Chunk* chunk = &m_ChunkMap.find(coords)->second;
        BlockVec blocksToSet = chunk->CreateSurfaceLayer(blockList);
        AddBlocks(coords, blocksToSet);
        m_ChunksToLight.push(chunk);
    }
    if (m_ChunksToLoad.empty()) {
        m_ChunksReadyToLight = true;
    }
}

void ChunkManager::LightChunks()
{
    for (int n = 0; m_ChunksReadyToLight && !m_ChunksToLight.empty() && n < MAX_CHUNK_TO_LOAD;
         n++) {
        Chunk* chunk = m_ChunksToLight.front();
        m_ChunksToLight.pop();
        if (chunk->FindNeighbors()) {
            Light::AddSunLightSimplified(chunk, m_ChunksToUpload);
            m_ChunksToMesh.push(chunk);
        } else {
            m_ChunksInBorder.push(chunk);
        }
    }
    if (m_ChunksToLight.empty()) {
        m_ChunksReadyToMesh = true;
        m_ChunksReadyToLight = false;
        m_ChunksToLight = std::move(m_ChunksInBorder);
    }
}

void ChunkManager::MeshChunks()
{
    bool uploaded = false;
    while (!m_ChunksToUpload.empty()) {
        auto iterator = m_ChunksToUpload.begin();
        auto node = m_ChunksToUpload.extract(iterator);
        ChunkCoord coords = node.value();
        auto chunk = &m_ChunkMap.at(coords);
        if (chunk->FindNeighbors()) {
            chunk->ClearMesh();
            chunk->GenerateMesh();
            uploaded = true;
        }
    }

    for (int n = 0;
         !uploaded && m_ChunksReadyToMesh && !m_ChunksToMesh.empty() && n < MAX_CHUNK_TO_LOAD;
         n++) {
        Chunk* chunk = m_ChunksToMesh.front();
        m_ChunksToMesh.pop();
        chunk->GenerateMesh();
        m_ChunksToRender.emplace_back(chunk);
    }
    if (m_ChunksToMesh.empty()) {
        m_ChunksReadyToMesh = false;
    }
}

void ChunkManager::GenerateChunks()
{
    //    std::cout << "Chunks in RAM: " << m_ChunkMap.size() << "\n";
    ChunkCoord playerChunk = CalculateChunkCoord(m_Camera->GetCameraPosition());

    // remove chunks outside of render distance (should be further than this when I'll add
    // serialization)
    //    for (auto& chunk : m_ChunksToRender) {
    //        if (chunk->NotVisible(playerChunk, m_ViewDistance))
    //            m_ChunkMap.erase(chunk->GetCoord());
    //    }
    m_ChunksToRender.clear();

    // load chunks
    for (int i = -m_ViewDistance + playerChunk.x; i <= m_ViewDistance + playerChunk.x; i++) {
        for (int j = -m_ViewDistance + playerChunk.z; j <= m_ViewDistance + playerChunk.z; j++) {
            ChunkCoord coords = { i, j };
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

void ChunkManager::SortChunks(const glm::vec3& cameraPos)
{
    glm::vec3 playerPos = cameraPos * glm::vec3(1.0f, 0, 1.0f);
    std::sort(m_ChunksToRender.begin(), m_ChunksToRender.end(), [&playerPos](Chunk* a, Chunk* b) {
        return glm::length2(playerPos - a->GetCenterPosition())
            > glm::length2(playerPos - b->GetCenterPosition());
    });
}

bool ChunkManager::IsBlockCastable(const glm::vec3& voxel)
{
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
            Block block = m_Raycast.chunk->GetBlock(
                m_Raycast.localVoxel[0], m_Raycast.localVoxel[1], m_Raycast.localVoxel[2]);
            if (block != Block::EMPTY && block != Block::WATER) {
                m_Raycast.selected = true;
                return true;
            }
        }
    }
    m_Raycast.selected = false;
    return false;
}

bool ChunkManager::IsTransparent(Block block)
{
    return block == Block::EMPTY || block == Block::BUSH || block == Block::LEAVES
        || block == Block::FLOWER_YELLOW || block == Block::FLOWER_BLUE || block == Block::WATER;
}

void ChunkManager::ExpandLight(
    uint8_t x, uint8_t y, uint8_t z, const glm::uvec4& lightLevel, Chunk* chunk)
{
    // sun
    if (lightLevel[0] != 0) {
        std::queue<LightAddNode> sunQueue;
        uint16_t startIndex = chunk->GetIndex(x, y, z);
        sunQueue.emplace(chunk, startIndex);
        Light::SunBFS(std::move(sunQueue), m_ChunksToUpload);
    }
    // red
    if (lightLevel[1] != 0) {
        std::queue<LightAddNode> redQueue;
        uint16_t startIndex = chunk->GetIndex(x, y, z);
        redQueue.emplace(chunk, startIndex);
        Light::LightPlacedBFS(std::move(redQueue), Channel::RED, m_ChunksToUpload);
    }
    // green
    if (lightLevel[2] != 0) {
        std::queue<LightAddNode> greenQueue;
        uint16_t startIndex = chunk->GetIndex(x, y, z);
        greenQueue.emplace(chunk, startIndex);
        Light::LightPlacedBFS(std::move(greenQueue), Channel::GREEN, m_ChunksToUpload);
    }
    // blue
    if (lightLevel[3] != 0) {
        std::queue<LightAddNode> blueQueue;
        uint16_t startIndex = chunk->GetIndex(x, y, z);
        blueQueue.emplace(chunk, startIndex);
        Light::LightPlacedBFS(std::move(blueQueue), Channel::BLUE, m_ChunksToUpload);
    }
}

void ChunkManager::RemoveLight(
    uint8_t x, uint8_t y, uint8_t z, const glm::uvec4& lightLevel, Chunk* chunk)
{
    // sun
    if (lightLevel[0] != 0) {
        chunk->SetSunLight(x, y, z, 0);
        std::queue<LightRemNode> sunRemQueue;
        uint16_t startIndex = chunk->GetIndex(x, y, z);
        sunRemQueue.emplace(chunk, startIndex, lightLevel[0]);
        std::queue<LightAddNode> sunQ
            = Light::LightRemovedBFS(std::move(sunRemQueue), Channel::SUN, m_ChunksToUpload);
        Light::SunBFS(std::move(sunQ), m_ChunksToUpload);
    }
    // red
    if (lightLevel[1] != 0) {
        chunk->SetRedLight(x, y, z, 0);
        std::queue<LightRemNode> redRemQueue;
        uint16_t startIndex = chunk->GetIndex(x, y, z);
        redRemQueue.emplace(chunk, startIndex, lightLevel[1]);
        std::queue<LightAddNode> redQ
            = Light::LightRemovedBFS(std::move(redRemQueue), Channel::RED, m_ChunksToUpload);
        Light::LightPlacedBFS(std::move(redQ), Channel::RED, m_ChunksToUpload);
    }
    // green
    if (lightLevel[2] != 0) {
        chunk->SetGreenLight(x, y, z, 0);
        std::queue<LightRemNode> greenRemQueue;
        uint16_t startIndex = chunk->GetIndex(x, y, z);
        greenRemQueue.emplace(chunk, startIndex, lightLevel[2]);
        std::queue<LightAddNode> greenQ
            = Light::LightRemovedBFS(std::move(greenRemQueue), Channel::GREEN, m_ChunksToUpload);
        Light::LightPlacedBFS(std::move(greenQ), Channel::GREEN, m_ChunksToUpload);
    }
    // blue
    if (lightLevel[3] != 0) {
        chunk->SetBlueLight(x, y, z, 0);
        std::queue<LightRemNode> blueRemQueue;
        uint16_t startIndex = chunk->GetIndex(x, y, z);
        blueRemQueue.emplace(chunk, startIndex, lightLevel[3]);
        std::queue<LightAddNode> blueQ
            = Light::LightRemovedBFS(std::move(blueRemQueue), Channel::BLUE, m_ChunksToUpload);
        Light::LightPlacedBFS(std::move(blueQ), Channel::BLUE, m_ChunksToUpload);
    }
}

void ChunkManager::DestroyBlock()
{
    uint8_t x = m_Raycast.localVoxel[0];
    uint8_t y = m_Raycast.localVoxel[1];
    uint8_t z = m_Raycast.localVoxel[2];
    Block block = m_Raycast.chunk->GetBlock(x, y, z);
    if (block != Block::BEDROCK) {
        m_Raycast.chunk->SetBlock(x, y, z, Block::EMPTY);
        if (m_LightBlocks.contains(block)) {
            glm::uvec3 lightLevel = m_LightBlocks[block];
            RemoveLight(
                x, y, z, { 0, lightLevel[0], lightLevel[1], lightLevel[2] }, m_Raycast.chunk);
        }
        if (!IsTransparent(block)) { // opaque block
            Light::UpdateOpaqueBlockLight(m_Raycast.chunk, x, y, z);
            glm::uvec4 lightLevel = { m_Raycast.chunk->GetSunLight(x, y, z),
                m_Raycast.chunk->GetRedLight(x, y, z), m_Raycast.chunk->GetGreenLight(x, y, z),
                m_Raycast.chunk->GetBlueLight(x, y, z) };
            ExpandLight(x, y, z, lightLevel, m_Raycast.chunk);
        }
        m_ChunksToUpload.insert(m_Raycast.chunkCoord);
        // check if the target is in the chunk border
        UpdateNeighbors(m_Raycast.localVoxel, m_Raycast.chunkCoord);
    }
}

void ChunkManager::PlaceBlock(Block block)
{
    glm::uvec3 localVoxel = m_Raycast.localVoxel;
    ChunkCoord chunkCoord = m_Raycast.chunkCoord;
    glm::vec3 globalVoxel = m_Raycast.globalVoxel;
    Chunk* chunk = m_Raycast.chunk;
    Block target = chunk->GetBlock(localVoxel[0], localVoxel[1], localVoxel[2]);
    if (target != Block::FLOWER_BLUE && target != Block::FLOWER_YELLOW && target != Block::BUSH) {
        localVoxel = m_Raycast.prevLocalVoxel;
        chunkCoord = m_Raycast.prevChunkCoord;
        globalVoxel = m_Raycast.prevGlobalVoxel;
        chunk = m_Raycast.prevChunk;
    }
    uint8_t x = localVoxel[0];
    uint8_t y = localVoxel[1];
    uint8_t z = localVoxel[2];
    if (!physics::Intersect(physics::CreatePlayerAabb(m_Camera->GetCameraPosition()),
            physics::CreateBlockAabb(globalVoxel))) {
        chunk->SetBlock(x, y, z, block);
        // if block is a light source
        if (m_LightBlocks.contains(block)) {
            glm::uvec3 lightLevel = m_LightBlocks[block];
            if (lightLevel[0] != 0)
                chunk->SetRedLight(x, y, z, lightLevel[0]);
            if (lightLevel[1] != 0)
                chunk->SetGreenLight(x, y, z, lightLevel[1]);
            if (lightLevel[2] != 0)
                chunk->SetBlueLight(x, y, z, lightLevel[2]);
            ExpandLight(x, y, z, { 0, lightLevel[0], lightLevel[1], lightLevel[2] }, chunk);
        } else if (!IsTransparent(block)) { // opaque block
            glm::uvec4 lightLevel = { chunk->GetSunLight(x, y, z), chunk->GetRedLight(x, y, z),
                chunk->GetGreenLight(x, y, z), chunk->GetBlueLight(x, y, z) };
            RemoveLight(x, y, z, lightLevel, chunk);
        }
        m_ChunksToUpload.insert(chunkCoord);
        // check if the target is in the chunk border
        UpdateNeighbors(localVoxel, chunkCoord);
    }
}

std::pair<ChunkCoord, glm::uvec3> ChunkManager::GlobalToLocal(const glm::vec3& playerPosition) const
{
    ChunkCoord chunkCoord = CalculateChunkCoord(playerPosition);
    uint32_t playerPosX = mod(static_cast<int>(std::floor(playerPosition.x)), m_ChunkSize[0]);
    uint32_t playerPosZ = mod(static_cast<int>(std::floor(playerPosition.z)), m_ChunkSize[2]);
    glm::uvec3 playerPos
        = { playerPosX, static_cast<uint32_t>(std::floor(playerPosition.y)), playerPosZ };
    return std::make_pair(chunkCoord, playerPos);
}

bool ChunkManager::IsBlockSolid(const glm::vec3& globalCoords) const
{
    std::pair<ChunkCoord, glm::vec3> localPos = GlobalToLocal(globalCoords);
    if (const auto it = m_ChunkMap.find(localPos.first); it != m_ChunkMap.end()) {
        const Chunk* chunk = &it->second;
        Block block = chunk->GetBlock(localPos.second[0], localPos.second[1], localPos.second[2]);
        if (block != Block::EMPTY && block != Block::WATER && block != Block::FLOWER_BLUE
            && block != Block::FLOWER_YELLOW && block != Block::BUSH) {
            return true;
        }
    }
    return false;
}

void ChunkManager::AddBlocks(const ChunkCoord& chunkCoord, BlockVec& blockVec)
{
    for (auto& [block, voxel] : blockVec) {
        ChunkCoord neighbor = { chunkCoord.x + static_cast<int>(std::floor(voxel.x / XSIZE)),
            chunkCoord.z + static_cast<int>(std::floor(voxel.z / ZSIZE)) };
        glm::uvec3 neighborVoxel = { mod(static_cast<int>(voxel.x), XSIZE), voxel.y,
            mod(static_cast<int>(voxel.z), ZSIZE) };

        if (const auto it = m_ChunkMap.find(neighbor); it != m_ChunkMap.end()) {
            // if chunk exists, update it
            Chunk* neighborChunk = &it->second;
            neighborChunk->SetBlock(neighborVoxel.x, neighborVoxel.y, neighborVoxel.z, block);
            m_ChunksToUpload.insert(neighbor);
        } else {
            // otherwise save the blocks to set for the chunk when it will be created
            m_BlocksToSet[neighbor].emplace_back(block, neighborVoxel);
        }
    }
}

void ChunkManager::UpdateNeighbors(const glm::uvec3& voxel, const ChunkCoord& chunkCoord)
{
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

void ChunkManager::UpdateChunks()
{
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