#include "TRAPPCH.h"
#include "ThreadPool.h"

#include "Utils/Utils.h"

TRAP::ThreadPool::ThreadPool(const u32 threads)
	: m_queues(threads), m_maxThreadsCount(threads)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	if (m_maxThreadsCount == 0)
	{
		m_maxThreadsCount = 3; //Fallback to 3 threads
		m_queues = Queues(3);
	}

	auto worker = [&](const u32 i)
	{
		//Set Thread name for profiler
		Utils::SetThreadName(fmt::format("Worker {}", i));

		while (true)
		{
			Proc f;
			for (u32 n = 0; n < m_maxThreadsCount; ++n)
			{
				if (m_queues[(i + n) % m_maxThreadsCount].TryPop(f))
					break;
			}

			if (!f && !m_queues[i].Pop(f))
				break;

			f();
		}
	};

	for (u32 i = 0; i < m_maxThreadsCount; ++i)
		m_threads.emplace_back(worker, i);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ThreadPool::~ThreadPool()
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	for (Queue& queue : m_queues)
		queue.Done();
}