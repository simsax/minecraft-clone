#include "ChunkManager.h"
#include "glm/gtx/norm.hpp"
#include <iostream>
#include "../utils/Timer.h"

using namespace std::chrono_literals;

#define MAX_INDEX_COUNT 18432 // each cube has 6 faces, each face has 6 indexes
#define MAX_VERTEX_COUNT 12228 // each cube has 6 faces, each face has 4 vertices
#define VIEW_DISTANCE 24 // how far the player sees
#define MAX_CHUNK_TO_LOAD 32
#define PLAYER_HALF_WIDTH 0.3f
#define PLAYER_TOP_HEIGHT 0.2f
#define PLAYER_BOTTOM_HEIGHT 1.6f

static int mod(int a, int b) {
    int res = a % b;
    return res >= 0 ? res : res + b;
}

ChunkManager::ChunkManager(Camera *camera)
        : m_ChunkSize({XSIZE - 2, YSIZE, ZSIZE - 2}), m_ViewDistance(VIEW_DISTANCE),
          m_Shutdown(false), m_LoadingChunks(false), m_Camera(camera), m_NewChunks(false),
          m_BindingIndex(0), m_Raycast({})
//	m_ThreadPool(ctpl::thread_pool(std::thread::hardware_concurrency()))
{
    m_VertexLayout.Push<uint32_t>(1); // position + texture coords

    m_ChunksToRender.reserve(
            static_cast<uint32_t>((m_ViewDistance * 2 + 1) * (m_ViewDistance * 2 + 1)));

    m_Indices.reserve(MAX_INDEX_COUNT);
    uint32_t offset = 0;

    for (size_t i = 0; i < MAX_INDEX_COUNT * 2; i += 6) {
        m_Indices.push_back(0 + offset);
        m_Indices.push_back(1 + offset);
        m_Indices.push_back(2 + offset);

        m_Indices.push_back(2 + offset);
        m_Indices.push_back(3 + offset);
        m_Indices.push_back(0 + offset);

        offset += 4;
    }

    /*
        m_Thread = std::thread([this](){
                while (true) {
                    std::unique_lock<std::mutex> lk(m_Mtx);

                    m_Cv.wait(lk, [this]() { return !m_ChunksToLoad.empty() || m_Shutdown; });
                    if (m_Shutdown && m_ChunksToLoad.empty())
                        break;
                    ChunkCoord coords = m_ChunksToLoad.front();
                    m_ChunksToLoad.pop();
                    Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2], glm::vec3(coords.x *
       static_cast<int>(m_ChunkSize[0]), 0.0, coords.z * static_cast<int>(m_ChunkSize[2])), m_Seed,
       m_VertexLayout, MAX_VERTEX_COUNT, m_Indices); chunk.GenerateMesh(); m_ChunkMap.insert({
       coords, std::move(chunk) }); m_ChunksToRender.emplace_back(&m_ChunkMap.find(coords)->second);
                }
            });*/
}

ChunkManager::~ChunkManager() { /*
std::unique_lock<std::mutex> lk(m_Mtx);
m_Shutdown = true;
lk.unlock();
m_Cv.notify_one();
m_Thread.join();	*/
}

void ChunkManager::InitWorld() {
    m_IBO.Init(m_Indices.size() * sizeof(uint32_t), m_Indices.data());
    m_VAO.Init();
    m_VAO.AddLayout(m_VertexLayout, m_BindingIndex);
    m_OutlineVBO.Init(m_VertexLayout.GetStride(), m_BindingIndex);
    m_OutlineVBO.CreateDynamic(sizeof(uint32_t) * 24);

    GenerateChunks();
    while (!m_ChunksToLoad.empty())
        LoadChunks();
    SortChunks();
}

void ChunkManager::Render(Renderer &renderer) {
    LoadChunks();
    // UpdateChunksToRender();
    // std::unique_lock<std::mutex> lk(m_Mtx);
    if (m_NewChunks) {
        m_NewChunks = false;
        SortChunks();
    }

    if (m_Raycast.selected) {
        m_OutlineVBO.Bind(m_VAO.GetId());
        m_Raycast.chunk->RenderOutline(renderer, m_VAO, m_OutlineVBO, m_IBO, m_Raycast.localVoxel);
    }
    // frustum culling
    m_Camera->UpdateFrustum();
    for (Chunk *chunk: m_ChunksToRender) {
        glm::vec3 center = chunk->GetCenterPosition();
        if (m_Camera->IsInFrustum(center))
            chunk->Render(renderer, m_VAO, m_IBO);
    }
}

void ChunkManager::UpdateChunk(ChunkCoord chunk) { m_ChunksToUpload.insert(chunk); }

int ChunkManager::GetViewDistance() const { return m_ViewDistance; }

std::array<uint32_t, 3> ChunkManager::GetChunkSize() const { return m_ChunkSize; }

ChunkCoord ChunkManager::CalculateChunkCoord(const glm::vec3 &position) {
    int chunkPosX = static_cast<int>(std::floor((position.x - 1) / m_ChunkSize[0]));
    int chunkPosZ = static_cast<int>(std::floor((position.z - 1) / m_ChunkSize[2]));
    return {chunkPosX, chunkPosZ};
}

void ChunkManager::LoadChunks() {
    //	auto meshFun = [this](ChunkCoord coords) {
    //		Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2], glm::vec3(coords.x *
    // static_cast<int>(m_ChunkSize[0]), 0.0, coords.z * static_cast<int>(m_ChunkSize[2])), m_Seed,
    // m_VertexLayout,
    // MAX_VERTEX_COUNT, m_Indices, coords); 		chunk.GenerateMesh(); 		return
    // chunk;
    //	};
    //	auto meshFun2 = [this](const std::queue<ChunkCoord>& chunksToLoad) {
    //		std::vector<std::pair<ChunkCoord, Chunk>> chunks;
    //		while (!chunksToLoad.empty()) {
    //			ChunkCoord coords = m_ChunksToLoad.front();
    //			m_ChunksToLoad.pop();
    //			Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2],
    // glm::vec3(coords.x
    //*
    // static_cast<int>(m_ChunkSize[0]), 0.0, coords.z * static_cast<int>(m_ChunkSize[2])), m_Seed,
    // m_VertexLayout, MAX_VERTEX_COUNT, m_Indices, coords); chunk.GenerateMesh();
    // chunks.emplace_back(std::make_pair(coords, std::move(chunk)));
    //		}
    //		return chunks;
    //	};

    //	if (!m_ChunksToLoad.empty() && !m_LoadingChunks) {
    //		m_FutureChunks = std::async(std::launch::async, meshFun2, m_ChunksToLoad);
    //		m_LoadingChunks = true;
    //	}
    //	if (m_LoadingChunks && m_FutureChunks.wait_for(1ms) == std::future_status::ready) {
    //		std::vector<std::pair<ChunkCoord, Chunk>> chunks = m_FutureChunks.get();
    //		for (auto& coord_chunk : chunks) {
    //			m_ChunkMap.insert({ coord_chunk.first, std::move(coord_chunk.second) });
    //			m_ChunksToRender.emplace_back(&m_ChunkMap.find(coord_chunk.first)->second);
    //		}
    //		m_ChunksToLoad = {};
    //	}

    bool uploaded = false;
    while (!m_ChunksToUpload.empty()) {
        auto iterator = m_ChunksToUpload.begin();
        auto node = m_ChunksToUpload.extract(iterator);
        ChunkCoord coords = node.value();
        m_ChunkMap.at(coords).GenerateMesh();
        uploaded = true;
    }

    for (int n = 0; n < MAX_CHUNK_TO_LOAD && !m_ChunksToLoad.empty() && !uploaded; n++) {
        ChunkCoord coords = m_ChunksToLoad.front();
        m_ChunksToLoad.pop();
        m_NewChunks = true;
        // m_ChunksLoaded.push_back(m_ThreadPool.push(meshFun, coords));
        //  create new chunk and cache it
        std::vector<std::pair<Block, glm::uvec3>> blockList = {};
        if (m_BlocksToSet.find(coords) != m_BlocksToSet.end()) {
            blockList = m_BlocksToSet.at(coords);
        }
        Chunk chunk(glm::vec3(coords.x * static_cast<int>(m_ChunkSize[0]), 0.0f,
                              coords.z * static_cast<int>(m_ChunkSize[2])),
                    MAX_VERTEX_COUNT, m_Indices, m_VertexLayout, m_BindingIndex, *this, blockList);
        chunk.GenerateMesh();
        m_ChunkMap.insert({coords, std::move(chunk)});
        m_ChunksToRender.emplace_back(&m_ChunkMap.at(coords));
    }
}

void ChunkManager::GenerateChunks() {
    ChunkCoord chunkCoord = CalculateChunkCoord(m_Camera->GetPlayerPosition());
    m_ChunksToRender.clear();

    //	std::unique_lock<std::mutex> lk(m_Mtx);
    // load chunks
    for (int i = -m_ViewDistance + chunkCoord.x; i <= m_ViewDistance + chunkCoord.x; i++) {
        for (int j = -m_ViewDistance + chunkCoord.z; j <= m_ViewDistance + chunkCoord.z; j++) {
            ChunkCoord coords = {i, j};
            // check if this chunk hasn't already been generated
            if (m_ChunkMap.find(coords) == m_ChunkMap.end()) {
                // add chunk to the loading queue
                m_ChunksToLoad.push(coords);
            } else {
                m_ChunksToRender.emplace_back(&m_ChunkMap.at(coords));
            }
        }
    }
    //	m_Cv.notify_one();
}

int ChunkManager::SpawnHeight() {
    Chunk *chunk = &m_ChunkMap.at({0, 0});
    int water_level = 63;
    int i;
    for (i = water_level; i < YSIZE; i++) {
        if (chunk->GetBlock(0, i, 0) == Block::EMPTY)
            break;
    }
    return i + PLAYER_BOTTOM_HEIGHT + 3;
}

void ChunkManager::SortChunks() {
    glm::vec3 playerPos = m_Camera->GetPlayerPosition() * glm::vec3(1.0f, 0, 1.0f);
    std::sort(m_ChunksToRender.begin(), m_ChunksToRender.end(), [&playerPos](Chunk *a, Chunk *b) {
        return glm::length2(playerPos - a->GetCenterPosition())
               > glm::length2(playerPos - b->GetCenterPosition());
    });
}

void ChunkManager::SetNewChunks() { m_NewChunks = true; }

bool ChunkManager::IsVoxelSolid(const glm::vec3 &voxel) {
    std::pair<ChunkCoord, glm::uvec3> target = GlobalToLocal(voxel);
    m_Raycast.prevGlobalVoxel = m_Raycast.globalVoxel;
    m_Raycast.globalVoxel = voxel;
    m_Raycast.prevChunk = m_Raycast.chunk;
    m_Raycast.prevChunkCoord = m_Raycast.chunkCoord;
    m_Raycast.chunkCoord = target.first;
    m_Raycast.prevLocalVoxel = m_Raycast.localVoxel;
    m_Raycast.localVoxel = target.second;
    if (m_ChunkMap.find(m_Raycast.chunkCoord) != m_ChunkMap.end()) {
        m_Raycast.chunk = &m_ChunkMap.at(m_Raycast.chunkCoord);

        if (m_Raycast.chunk->GetBlock(m_Raycast.localVoxel[0], m_Raycast.localVoxel[1],
                                      m_Raycast.localVoxel[2]) != Block::EMPTY) {
            m_Raycast.selected = true;
            return true;
        }
    }
    m_Raycast.selected = false;
    return false;
}

void ChunkManager::DestroyBlock() {
    m_Raycast.chunk->SetBlock(m_Raycast.localVoxel[0], m_Raycast.localVoxel[1],
                              m_Raycast.localVoxel[2], Block::EMPTY);
    UpdateChunk(m_Raycast.chunkCoord);
    // check if the target is in the chunk border
    if (m_Raycast.localVoxel[0] == 1 || m_Raycast.localVoxel[2] == 1 ||
        m_Raycast.localVoxel[0] == m_ChunkSize[0]
        || m_Raycast.localVoxel[2] == m_ChunkSize[2]) {
        UpdateNeighbors(m_Raycast.globalVoxel, m_Raycast.chunkCoord, Block::EMPTY);
    }
}

void ChunkManager::PlaceBlock(Block block) {
    if (!physics::Intersect(
            physics::CreatePlayerAabb(m_Camera->GetPlayerPosition()),
            physics::CreateBlockAabb(m_Raycast.prevGlobalVoxel))) {
        m_Raycast.prevChunk->SetBlock(m_Raycast.prevLocalVoxel[0], m_Raycast.prevLocalVoxel[1],
                                      m_Raycast.prevLocalVoxel[2], block);
        UpdateChunk(m_Raycast.prevChunkCoord);
        // check if the target is in the chunk border
        if (m_Raycast.prevLocalVoxel[0] == 1 || m_Raycast.prevLocalVoxel[2] == 1 ||
            m_Raycast.prevLocalVoxel[0] == m_ChunkSize[0]
            || m_Raycast.prevLocalVoxel[2] == m_ChunkSize[2]) {
            UpdateNeighbors(m_Raycast.prevGlobalVoxel, m_Raycast.prevChunkCoord, block);
        }
    }
}

std::pair<ChunkCoord, glm::uvec3> ChunkManager::GlobalToLocal(const glm::vec3 &playerPosition) {
    uint32_t chunkSize = m_ChunkSize[0];
    ChunkCoord chunkCoord = CalculateChunkCoord(playerPosition);
    uint32_t playerPosX = mod(static_cast<int>(std::floor(playerPosition.x) - 1), chunkSize) + 1;
    uint32_t playerPosZ = mod(static_cast<int>(std::floor(playerPosition.z) - 1), chunkSize) + 1;
    glm::uvec3 playerPos
            = {playerPosX, static_cast<uint32_t>(std::floor(playerPosition.y)), playerPosZ};
    return std::make_pair(chunkCoord, playerPos);
}

void
ChunkManager::UpdateNeighbors(glm::vec3 currentVoxel, ChunkCoord targetLocalCoord, Block block) {
    // take a step of 1 in every direction and update the neighboring chunks found
    currentVoxel.x += 1;
    std::pair<ChunkCoord, glm::uvec3> neighbor = GlobalToLocal(currentVoxel);
    if (neighbor.first != targetLocalCoord) {
        Chunk *neighborChunk = &m_ChunkMap.find(neighbor.first)->second;
        neighborChunk->SetBlock(
                neighbor.second[0] - 1, neighbor.second[1], neighbor.second[2], block);
        UpdateChunk(neighbor.first);
        currentVoxel.x -= 1;
    } else {
        currentVoxel.x -= 2;
        neighbor = GlobalToLocal(currentVoxel);
        if (neighbor.first != targetLocalCoord) {
            Chunk *neighborChunk = &m_ChunkMap.find(neighbor.first)->second;
            neighborChunk->SetBlock(
                    neighbor.second[0] + 1, neighbor.second[1], neighbor.second[2], block);
            UpdateChunk(neighbor.first);
        }
        currentVoxel.x += 1;
    }
    currentVoxel.z += 1;
    neighbor = GlobalToLocal(currentVoxel);
    if (neighbor.first != targetLocalCoord) {
        Chunk *neighborChunk = &m_ChunkMap.find(neighbor.first)->second;
        neighborChunk->SetBlock(
                neighbor.second[0], neighbor.second[1], neighbor.second[2] - 1, block);
        UpdateChunk(neighbor.first);
        currentVoxel.z -= 1;
    } else {
        currentVoxel.z -= 2;
        neighbor = GlobalToLocal(currentVoxel);
        if (neighbor.first != targetLocalCoord) {
            Chunk *neighborChunk = &m_ChunkMap.find(neighbor.first)->second;
            neighborChunk->SetBlock(
                    neighbor.second[0], neighbor.second[1], neighbor.second[2] + 1, block);
            UpdateChunk(neighbor.first);
        }
        currentVoxel.z += 1;
    }
}


bool ChunkManager::CalculateCollision(const glm::vec3& playerSpeed)
{
    glm::vec3& currentPosition = m_Camera->GetPlayerPosition();
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

    physics::Aabb playerBbox = physics::CreatePlayerAabb(currentPosition);
    for (int i = startX; i <= endX; i++) {
        for (int j = startY; j <= endY; j++) {
            for (int k = startZ; k <= endZ; k++) {
                std::pair<ChunkCoord, glm::vec3> localPos = GlobalToLocal(glm::vec3(i, j, k));
                if (m_ChunkMap.find(localPos.first) != m_ChunkMap.end()) {
                    Chunk* chunk = &m_ChunkMap.at(localPos.first);
                    Block block = chunk->GetBlock(
                            localPos.second[0], localPos.second[1], localPos.second[2]);
                    if (block != Block::EMPTY && block != Block::WATER) {
                        physics::Aabb blockBbox = physics::CreateBlockAabb(glm::vec3(i, j, k));
                        physics::SnapAabb(playerBbox, blockBbox, playerSpeed, currentPosition);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void ChunkManager::AddBlocks(const ChunkCoord& chunkCoord, glm::uvec3 localVoxel, Block block) {
    ChunkCoord neighbor = chunkCoord;
    if (localVoxel.x <= 0) {
        neighbor.x -= 1 + static_cast<int>(localVoxel.x / XSIZE);
        localVoxel.x = mod(static_cast<int>(localVoxel.x) - 1, XSIZE);
    } else if (localVoxel.x >= XSIZE - 1) {
        neighbor.x += static_cast<int>(localVoxel.x / XSIZE);
        localVoxel.x = mod(static_cast<int>(localVoxel.x) + 1, XSIZE);
    }
    if (localVoxel.z <= 0) {
        neighbor.z -= 1 + static_cast<int>(localVoxel.z / ZSIZE);
        localVoxel.z = mod(static_cast<int>(localVoxel.z) - 1, XSIZE);
    } else if (localVoxel.z >= ZSIZE - 1) {
        neighbor.z += static_cast<int>(localVoxel.z / ZSIZE);
        localVoxel.z = mod(static_cast<int>(localVoxel.z) + 1, XSIZE);
    }
    if (m_ChunkMap.find(neighbor) != m_ChunkMap.end()) {
        // case 1: the chunk already exists
        Chunk* chunk = &m_ChunkMap.at(neighbor);
        chunk->SetBlock(localVoxel[0], localVoxel[1], localVoxel[2], block);
        UpdateChunk(neighbor);
        UpdateNeighbors2(localVoxel, neighbor, block);
    } else {
        // case 2: chunk still not created
        m_BlocksToSet[neighbor].emplace_back(block, localVoxel);
    }
}

void
ChunkManager::UpdateNeighbors2(const glm::uvec3& voxel, const ChunkCoord& chunkCoord, Block block) {
    if (voxel.x == 0) {
        auto v = voxel;
        auto c = chunkCoord;
        c.x -= 1;
        v.x = XSIZE - 1;
        Chunk *neighborChunk = &m_ChunkMap.at(c);
        neighborChunk->SetBlock(v.x, v.y, v.z, block);
        UpdateChunk(c);
    } else if (voxel.x == XSIZE - 1) {
        auto v = voxel;
        auto c = chunkCoord;
        c.x += 1;
        v.x = 0;
        Chunk *neighborChunk = &m_ChunkMap.at(c);
        neighborChunk->SetBlock(v.x, v.y, v.z, block);
        UpdateChunk(c);
    }
    if (voxel.z == 0) {
        auto v = voxel;
        auto c = chunkCoord;
        c.z -= 1;
        v.z = ZSIZE - 1;
        Chunk *neighborChunk = &m_ChunkMap.at(c);
        neighborChunk->SetBlock(v.x, v.y, v.z, block);
        UpdateChunk(c);
    } else if (voxel.z == ZSIZE - 1) {
        auto v = voxel;
        auto c = chunkCoord;
        c.z += 1;
        v.z = 0;
        Chunk *neighborChunk = &m_ChunkMap.at(c);
        neighborChunk->SetBlock(v.x, v.y, v.z, block);
        UpdateChunk(c);
    }
}
/*
void ChunkManager::UpdateChunksToRender()
{
    for (int i = 0; i < m_ChunksLoaded.size(); i++) {
        if (m_ChunksLoaded[i].wait_for(1ms) == std::future_status::ready) {
    //		std::pair<ChunkCoord, Chunk> item = m_ChunksLoaded[i].get();
    //		m_ChunkMap.insert({ item.first, std::move(item.second) });
    //		m_ChunksToRender.emplace_back(&m_ChunkMap.find(item.first)->second);
    //		m_ChunksLoaded.erase(std::next(m_ChunksLoaded.begin(), i));
        }
    }
}*/
