#include "TRAPPCH.h"
#include "ThreadPool.h"

#include "BlockingQueue.h"

TRAP::ThreadPool::ThreadPool(const uint32_t threads)
	: m_queues(threads), m_maxThreadsCount(threads)
{
	if (!m_maxThreadsCount)
	{
		m_maxThreadsCount = 3; //Fallback to 3 threads
		m_queues = Queues(3);
	}

	auto worker = [&](const uint32_t i)
	{
		while (true)
		{
			Proc f;
			for (uint32_t n = 0; n < m_maxThreadsCount; ++n)
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

TRAP::ThreadPool::~ThreadPool() noexcept
{
	for (Queue& queue : m_queues)
		queue.Done();

	for (std::thread& thread : m_threads)
		thread.join();
}