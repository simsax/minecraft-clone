#include "ChunkManager.h"
#include "glm/gtx/norm.hpp"
#include <iostream>
#include "../utils/Timer.h"
#include "Config.h"

using namespace std::chrono_literals;

#define MAX_INDEX_COUNT 18432 // each cube has 6 faces, each face has 6 indexes
#define MAX_VERTEX_COUNT 24000 // each cube has 6 faces, each face has 4 vertices
#define VIEW_DISTANCE 24 // how far the player sees
#define MAX_CHUNK_TO_LOAD 8
#define PLAYER_HALF_WIDTH 0.3f
#define PLAYER_TOP_HEIGHT 0.2f
#define PLAYER_BOTTOM_HEIGHT 1.6f

static int mod(int a, int b) {
    int res = a % b;
    return res >= 0 ? res : res + b;
}

ChunkManager::ChunkManager(Camera *camera)
        : m_ChunkSize({XSIZE, YSIZE, ZSIZE}), m_ViewDistance(VIEW_DISTANCE),
          m_TextureAtlas(std::string(SOURCE_DIR) + "/res/textures/terrain.png"),
          m_Camera(camera), m_SortChunks(false), m_ChunksReadyToMesh(false),
          m_BindingIndex(0), m_Raycast({}), m_LastChunk({0, 0}), m_CurrentChunk({0, 0}) {
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
}

void ChunkManager::InitWorld() {
    m_TextureAtlas.Init();
    m_ChunkShader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_chunk.vert",
                  std::string(SOURCE_DIR) + "/res/shaders/shader_chunk.frag");
    m_ChunkShader.Bind();
    m_ChunkShader.SetUniform1i("u_Texture", 0);

    m_OutlineShader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_outline.vert",
                         std::string(SOURCE_DIR) +
                         "/res/shaders/shader_outline.frag");

//    m_OutlineShader.Bind();
//    m_OutlineShader.SetUniform1i("u_Texture", 0);

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
    MeshChunks();
    if (m_SortChunks) {
        m_SortChunks = false;
        SortChunks();
    }

    // frustum culling
    m_Camera->UpdateFrustum();
    ChunkCoord playerChunk = CalculateChunkCoord(m_Camera->GetPlayerPosition());
    static constexpr int radius = 20;
    for (Chunk *chunk: m_ChunksToRender) {
        glm::vec3 center = chunk->GetCenterPosition();
        if (m_Camera->IsInFrustum(center))
            chunk->Render(renderer, m_VAO, m_IBO, m_ChunkShader, m_TextureAtlas, playerChunk, radius);
    }

    if (m_Raycast.selected) {
        m_OutlineVBO.Bind(m_VAO.GetId());
        m_Raycast.chunk->RenderOutline(renderer, m_VAO, m_OutlineVBO, m_IBO, m_OutlineShader,
                                       m_Raycast.localVoxel);
    }
}

std::array<uint32_t, 3> ChunkManager::GetChunkSize() const { return m_ChunkSize; }

ChunkCoord ChunkManager::CalculateChunkCoord(const glm::vec3 &position) {
    int chunkPosX = static_cast<int>(std::floor(position.x / m_ChunkSize[0]));
    int chunkPosZ = static_cast<int>(std::floor(position.z / m_ChunkSize[2]));
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
                             coords.z * static_cast<int>(m_ChunkSize[2])},
                    MAX_VERTEX_COUNT, m_Indices, m_VertexLayout, m_BindingIndex, &m_ChunkMap);
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
    ChunkCoord playerChunk = CalculateChunkCoord(m_Camera->GetPlayerPosition());

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

bool ChunkManager::IsVoxelSolid(const glm::vec3 &voxel) {
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
            if (m_Raycast.chunk->GetBlock(m_Raycast.localVoxel[0], m_Raycast.localVoxel[1],
                                          m_Raycast.localVoxel[2]) != Block::EMPTY) {
                m_Raycast.selected = true;
                return true;
            }
        }
    }
    m_Raycast.selected = false;
    return false;
}

void ChunkManager::DestroyBlock() {
    if (m_Raycast.chunk->GetBlock(m_Raycast.localVoxel[0], m_Raycast.localVoxel[1],
                                  m_Raycast.localVoxel[2]) != Block::BEDROCK) {
        m_Raycast.chunk->SetBlock(m_Raycast.localVoxel[0], m_Raycast.localVoxel[1],
                                  m_Raycast.localVoxel[2], Block::EMPTY);
        m_ChunksToUpload.insert(m_Raycast.chunkCoord);
        // check if the target is in the chunk border
        UpdateNeighbors(m_Raycast.localVoxel, m_Raycast.chunkCoord);
    }
}

void ChunkManager::PlaceBlock(Block block) {
    glm::uvec3 localVoxel = m_Raycast.localVoxel;
    ChunkCoord chunkCoord = m_Raycast.chunkCoord;
    glm::vec3 globalVoxel = m_Raycast.globalVoxel;
    Block target = m_Raycast.chunk->GetBlock(localVoxel[0], localVoxel[1], localVoxel[2]);
    if (target != Block::FLOWER_BLUE &&
        target != Block::FLOWER_YELLOW &&
        target != Block::BUSH) {
        localVoxel = m_Raycast.prevLocalVoxel;
        chunkCoord = m_Raycast.prevChunkCoord;
        globalVoxel = m_Raycast.prevGlobalVoxel;
    }
    if (!physics::Intersect(
            physics::CreatePlayerAabb(m_Camera->GetPlayerPosition()),
            physics::CreateBlockAabb(globalVoxel))) {
        m_Raycast.prevChunk->SetBlock(localVoxel[0], localVoxel[1], localVoxel[2], block);
        m_ChunksToUpload.insert(chunkCoord);
        // check if the target is in the chunk border
        UpdateNeighbors(localVoxel, chunkCoord);
    }
}

std::pair<ChunkCoord, glm::uvec3> ChunkManager::GlobalToLocal(const glm::vec3 &playerPosition) {
    ChunkCoord chunkCoord = CalculateChunkCoord(playerPosition);
    uint32_t playerPosX =
            mod(static_cast<int>(std::floor(playerPosition.x)), m_ChunkSize[0]);
    uint32_t playerPosZ =
            mod(static_cast<int>(std::floor(playerPosition.z)), m_ChunkSize[2]);
    glm::uvec3 playerPos
            = {playerPosX, static_cast<uint32_t>(std::floor(playerPosition.y)), playerPosZ};
    return std::make_pair(chunkCoord, playerPos);
}

bool ChunkManager::CalculateCollision(const glm::vec3 &playerSpeed) {
    glm::vec3 &currentPosition = m_Camera->GetPlayerPosition();
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
                std::pair<ChunkCoord, glm::vec3> localPos = GlobalToLocal({i, j, k});
                if (const auto it = m_ChunkMap.find(localPos.first); it != m_ChunkMap.end()) {
                    Chunk *chunk = &it->second;
                    Block block = chunk->GetBlock(
                            localPos.second[0], localPos.second[1], localPos.second[2]);
                    if (block != Block::EMPTY && block != Block::WATER &&
                        block != Block::FLOWER_BLUE && block != Block::FLOWER_YELLOW
                        && block != Block::BUSH) {
                        physics::Aabb blockBbox = physics::CreateBlockAabb({i, j, k});
                        physics::SnapAabb(playerBbox, blockBbox, playerSpeed, currentPosition);
                        return true;
                    }
                }
            }
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
    ChunkCoord currentChunk = CalculateChunkCoord(m_Camera->GetPlayerPosition());
    if (m_CurrentChunk != currentChunk) {
        m_CurrentChunk = currentChunk;
        m_SortChunks = true;
    }
    if (m_LastChunk - currentChunk >= m_ViewDistance / 3) {
        GenerateChunks();
        m_LastChunk = currentChunk;
    }
}
