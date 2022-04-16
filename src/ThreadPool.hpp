#pragma once

#include <queue>
#include <future>
#include "Chunk.h"

class ThreadPool {
public:
	ThreadPool();
	~ThreadPool();

	//	template <typename T, typename F>
	std::future<Chunk> Submit(std::function<Chunk(ChunkCoord)> job);
	void End();
private:
	std::queue<std::packaged_task<Chunk(ChunkCoord)>> m_Jobs;
	std::thread m_Thread;
	bool m_Shutdown;
	std::mutex m_Mtx;
	std::condition_variable m_Cv;
};

ThreadPool::ThreadPool() : 
	m_Shutdown(false)
{
	int nThreads = std::thread::hardware_concurrency();
	for (int i = 0; i < nThreads; i++) {
		m_Threads.emplace_back(std::thread([this](){
			while (true) {
				std::unique_lock<std::mutex> lk(m_Mtx);

				m_Cv.wait(lk, [this]() { return !m_Jobs.empty() || m_Shutdown; });
				if (m_Shutdown && m_Jobs.empty())
					break;
				auto& job = m_Jobs.front();
				m_Jobs.pop();
				lk.unlock();
				job();
			}
		}));
	}
}

ThreadPool::~ThreadPool()
{
	End();
}

std::future<Chunk> ThreadPool::Submit(std::function<Chunk(ChunkCoord)> job)
{
	std::unique_lock<std::mutex> lk(m_Mtx);
	std::packaged_task<Chunk(ChunkCoord)> task(job);
	std::future<Chunk> chunkFut = task.get_future();
	m_Jobs.push(std::move(task));
	m_Cv.notify_one();
	return chunkFut;
}

void ThreadPool::End()
{
	std::unique_lock<std::mutex> lk(m_Mtx);
	m_Shutdown = true;
	lk.unlock();
	m_Cv.notify_all();
	for (auto& thread : m_Threads)
		thread.join();
}

