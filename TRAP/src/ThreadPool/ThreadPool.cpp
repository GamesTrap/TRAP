#include "TRAPPCH.h"
#include "ThreadPool.h"

#include "BlockingQueue.h"

TRAP::EXPERIMENTAL::ThreadPool::ThreadPool(const uint32_t threads)
	: m_queues(threads), m_maxThreadsCount(threads)
{
	if (!m_maxThreadsCount)
	{
		m_maxThreadsCount = 4; //Fallback to using 4 threads
		m_queues = Queues(4);
	}

	auto worker = [this](auto i)
	{
		while (true)
		{
			Proc f;
			for (uint32_t n = 0; n < m_maxThreadsCount * HyperThreading; ++n)
			{
				if (m_queues[(i + n) % m_maxThreadsCount].TryPop(f))
					break;
			}
			
			if (!f && !m_queues[i].Pop(f))
				break;
			
			f();
		}
	};

	for (uint32_t i = 0; i < threads; ++i)
		m_threads.emplace_back(worker, i);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EXPERIMENTAL::ThreadPool::~ThreadPool() noexcept
{
	for (Queue& queue : m_queues)
		queue.Done();
	
	for (std::thread& thread : m_threads)
		thread.join();
}