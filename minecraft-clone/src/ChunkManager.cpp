#include "ChunkManager.h"
#include "time.h"
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

#define MAX_INDEX_COUNT 18432 // each cube has 6 faces, each face has 6 indexes
#define MAX_VERTEX_COUNT 12228 // each cube has 6 faces, each face has 4 vertices 
#define VIEW_DISTANCE 6 // how far the player sees
#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 256
#define CHUNK_SIZE_Z 16
#define MAX_CHUNK_TO_LOAD 1

ChunkManager::ChunkManager():
	m_ViewDistance(VIEW_DISTANCE),
	m_Shutdown(false)
//	m_ThreadPool(ctpl::thread_pool(std::thread::hardware_concurrency()))
{
	m_VertexLayout.Push<float>(3); // position
	m_VertexLayout.Push<float>(2); // texture coords
		
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

	m_ChunkSize = { CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z };
	m_Seed = static_cast<unsigned int>(time(NULL));
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
	glm::vec3 playerPos(0.0f, 0.0f, 0.0f);
	GenerateChunks(playerPos);
	while (!m_ChunksToLoad.empty())
		LoadChunks();
}

void ChunkManager::Render(const glm::mat4& mvp)
{
	LoadChunks();
	//UpdateChunksToRender();
	//std::unique_lock<std::mutex> lk(m_Mtx);
	for (auto& chunk : m_ChunksToRender) {
		chunk->Render(mvp);
	}
}

int ChunkManager::GetViewDistance() const
{
	return m_ViewDistance;
}

std::array<unsigned int, 3> ChunkManager::GetChunkSize() const
{
	return m_ChunkSize;
}

void ChunkManager::LoadChunks()
{
	auto meshFun = [this](ChunkCoord coords) {
		Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2], glm::vec3(coords.x * static_cast<int>(m_ChunkSize[0]), 0.0, coords.z * static_cast<int>(m_ChunkSize[2])), m_Seed, m_VertexLayout, MAX_VERTEX_COUNT, m_Indices);
		chunk.GenerateMesh();
		return chunk;
	};

	for (int n = 0; n < MAX_CHUNK_TO_LOAD && !m_ChunksToLoad.empty(); n++) {
		ChunkCoord coords = m_ChunksToLoad.front();
		m_ChunksToLoad.pop();
		if (m_ChunkMap.find(coords) != m_ChunkMap.end())
			m_ChunkMap.find(coords)->second.GenerateMesh();
		else {
			//m_ChunksLoaded.push_back(m_ThreadPool.push(meshFun, coords));
			// create new chunk and cache it
			Chunk chunk(m_ChunkSize[0], m_ChunkSize[1], m_ChunkSize[2], glm::vec3(coords.x * static_cast<int>(m_ChunkSize[0]), 0.0, coords.z * static_cast<int>(m_ChunkSize[2])), m_Seed, m_VertexLayout, MAX_VERTEX_COUNT, m_Indices);
			chunk.GenerateMesh();
			m_ChunkMap.insert({ coords, std::move(chunk) });
			m_ChunksToRender.emplace_back(&m_ChunkMap.find(coords)->second);
		}
	}
}

void ChunkManager::GenerateChunks(const glm::vec3& playerPosition)
{
	int playerPosX = static_cast<int>(round(playerPosition.x / static_cast<int>(m_ChunkSize[0])));
	int playerPosZ = static_cast<int>(round(playerPosition.z / static_cast<int>(m_ChunkSize[2])));

	m_ChunksToRender.clear();

//	std::unique_lock<std::mutex> lk(m_Mtx);
	// load chunks
	for (int i = -m_ViewDistance + playerPosX; i <= m_ViewDistance + playerPosX; i++) {
		for (int j = -m_ViewDistance + playerPosZ; j <= m_ViewDistance + playerPosZ; j++) {
			ChunkCoord coords = { i, j };
			// check if this chunk hasn't already been generated
			if (m_ChunkMap.find(coords) == m_ChunkMap.end() || m_ChunkMap.find(coords)->second.GetNeedUpdate()) {
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

