#include "ChunkManager.h"
#include "time.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

using namespace std::chrono_literals;

#define MAX_INDEX_COUNT 18432 // each cube has 6 faces, each face has 6 indexes
#define MAX_VERTEX_COUNT 12228 // each cube has 6 faces, each face has 4 vertices 
#define VIEW_DISTANCE 24 // how far the player sees
#define MAX_CHUNK_TO_LOAD 1

ChunkManager::ChunkManager(glm::vec3* playerPosition):
	m_ViewDistance(VIEW_DISTANCE),
	m_Shutdown(false),
	m_LoadingChunks(false),
    m_PlayerPosition(playerPosition),
    m_NewChunks(false)
//	m_ThreadPool(ctpl::thread_pool(std::thread::hardware_concurrency()))
{
	m_VertexLayout.Push<unsigned int>(1); // position + texture coords
		
	m_ChunksToRender.reserve(static_cast<const unsigned int>((m_ViewDistance * 2 + 1) * (m_ViewDistance * 2 + 1)));

	m_Indices.reserve(MAX_INDEX_COUNT);
	unsigned int offset = 0;
	
	for (size_t i = 0; i < MAX_INDEX_COUNT * 2; i += 6) {
		m_Indices.push_back(0 + offset);
		m_Indices.push_back(1 + offset);
		m_Indices.push_back(2 + offset);

		m_Indices.push_back(2 + offset);
		m_Indices.push_back(3 + offset);
		m_Indices.push_back(0 + offset);

		offset += 4;
	}

	m_ChunkSize = { XSIZE - 2, YSIZE, ZSIZE - 2}; // -2 because each chunk has a border
/*
	m_Thread = std::thread([this](){
			while (true) {
				std::unique_lock<std::mutex> lk(m_Mtx);

				m_Cv.wait(lk, [this]() { return !m_ChunksToLoad.empty() || m_Shutdown; });
				if (m_Shutdown && m_ChunksToLoad.empty())
					break;
				ChunkCoord coords = m_ChunksToLoad.front();
				m_ChunksToLoad.pop();
				Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2], glm::vec3(coords.x * static_cast<int>(m_ChunkSize[0]), 0.0, coords.z * static_cast<int>(m_ChunkSize[2])), m_Seed, m_VertexLayout, MAX_VERTEX_COUNT, m_Indices);
				chunk.GenerateMesh();
				m_ChunkMap.insert({ coords, std::move(chunk) });
				m_ChunksToRender.emplace_back(&m_ChunkMap.find(coords)->second);
			}
		});*/
}

ChunkManager::~ChunkManager()
{/*
	std::unique_lock<std::mutex> lk(m_Mtx);
	m_Shutdown = true;
	lk.unlock();
	m_Cv.notify_one();
	m_Thread.join();	*/
}

void ChunkManager::InitWorld()
{
	GenerateChunks();
	while (!m_ChunksToLoad.empty())
		LoadChunks();
    SortChunks();
}

void ChunkManager::Render(const Renderer& renderer)
{
	LoadChunks();
	//UpdateChunksToRender();
	//std::unique_lock<std::mutex> lk(m_Mtx);
    if (m_NewChunks) {
        m_NewChunks = false;
        SortChunks();
    }
	for (Chunk* chunk : m_ChunksToRender) {
		if (IsInFrustum(chunk))
			chunk->Render(renderer);
	}
}

bool ChunkManager::IsInFrustum(Chunk* chunk) {
	if (chunk->GetCenterPosition().z <= 0)
		return true;
	else
		return false;
}

void ChunkManager::UpdateChunk(ChunkCoord chunk)
{
	m_ChunksToLoad.push(chunk);
}

int ChunkManager::GetViewDistance() const
{
	return m_ViewDistance;
}

std::array<unsigned int, 3> ChunkManager::GetChunkSize() const
{
	return m_ChunkSize;
}

ChunkCoord ChunkManager::CalculateChunkCoord(const glm::vec3& position) {
    int chunkPosX = static_cast<int>(std::floor((position.x - 1) / m_ChunkSize[0]));
    int chunkPosZ = static_cast<int>(std::floor((position.z - 1) / m_ChunkSize[2]));
	return { chunkPosX, chunkPosZ };
}

void ChunkManager::LoadChunks()
{
//	auto meshFun = [this](ChunkCoord coords) {
//		Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2], glm::vec3(coords.x * static_cast<int>(m_ChunkSize[0]), 0.0, coords.z * static_cast<int>(m_ChunkSize[2])), m_Seed, m_VertexLayout, MAX_VERTEX_COUNT, m_Indices, coords);
//		chunk.GenerateMesh();
//		return chunk;
//	};
//	auto meshFun2 = [this](const std::queue<ChunkCoord>& chunksToLoad) {
//		std::vector<std::pair<ChunkCoord, Chunk>> chunks;
//		while (!chunksToLoad.empty()) {
//			ChunkCoord coords = m_ChunksToLoad.front();
//			m_ChunksToLoad.pop();
//			Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2], glm::vec3(coords.x * static_cast<int>(m_ChunkSize[0]), 0.0, coords.z * static_cast<int>(m_ChunkSize[2])), m_Seed, m_VertexLayout, MAX_VERTEX_COUNT, m_Indices, coords);
//			chunk.GenerateMesh();
//			chunks.emplace_back(std::make_pair(coords, std::move(chunk)));
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
    for (int n = 0; n < MAX_CHUNK_TO_LOAD && !m_ChunksToLoad.empty(); n++) {
        ChunkCoord coords = m_ChunksToLoad.front();
        m_ChunksToLoad.pop();
        if (m_ChunkMap.find(coords) != m_ChunkMap.end())
            m_ChunkMap.find(coords)->second.GenerateMesh();
        else {
            m_NewChunks = true;
            //m_ChunksLoaded.push_back(m_ThreadPool.push(meshFun, coords));
            // create new chunk and cache it
            Chunk chunk(glm::vec3(coords.x * static_cast<int>(m_ChunkSize[0]), 0.0f,
                                  coords.z * static_cast<int>(m_ChunkSize[2])),
                                  m_VertexLayout, MAX_VERTEX_COUNT, m_Indices, m_PlayerPosition);
            chunk.GenerateMesh();
            m_ChunkMap.insert({ coords, std::move(chunk) });
            m_ChunksToRender.emplace_back(&m_ChunkMap.find(coords)->second);
        }
    }
}

void ChunkManager::GenerateChunks()
{
	ChunkCoord chunkCoord = CalculateChunkCoord(*m_PlayerPosition);
    m_ChunksToRender.clear();

//	std::unique_lock<std::mutex> lk(m_Mtx);
    // load chunks
    for (int i = -m_ViewDistance + chunkCoord.x; i <= m_ViewDistance + chunkCoord.x; i++) {
        for (int j = -m_ViewDistance + chunkCoord.z; j <= m_ViewDistance + chunkCoord.z; j++) {
            ChunkCoord coords = { i, j };
            // check if this chunk hasn't already been generated
            if (m_ChunkMap.find(coords) == m_ChunkMap.end()) {
                // add chunk to the loading queue
                m_ChunksToLoad.push(coords);
            }
            else {
                m_ChunksToRender.emplace_back(&m_ChunkMap.find(coords)->second);
            }
        }
    }
//	m_Cv.notify_one();
}

int ChunkManager::SpawnHeight() {
   Chunk* chunk = &m_ChunkMap.find({ 0,0 })->second;
   int water_level = 63;
   int i;
   for (i = water_level; i < YSIZE; i++) {
        if (chunk->GetMatrix()(0, i, 0) == Block::EMPTY)
            break;
   }
   return i;
}

void ChunkManager::SortChunks() {
    glm::vec3 playerPos = glm::vec3(m_PlayerPosition->x, 0, m_PlayerPosition->z);
    std::sort(m_ChunksToRender.begin(), m_ChunksToRender.end(), [&playerPos](Chunk* a, Chunk* b) {
        return glm::length2(playerPos - a->GetCenterPosition()) > glm::length2(playerPos - b->GetCenterPosition());
    });
}

void ChunkManager::SetNewChunks() {
    m_NewChunks = true;
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

