#include "TRAPPCH.h"
#include "ThreadPool.h"

#include "BlockingQueue.h"

TRAP::EXPERIMENTAL::ThreadPool::ThreadPool(const uint32_t threads)
	: m_queues(threads), m_maxThreadsCount(threads)
{
	if (!m_maxThreadsCount)
		m_maxThreadsCount = 4; //Fallback to using 4 threads

	auto worker = [this](auto i)
	{
		while (true)
		{
			Proc f;
			for (uint32_t n = 0; n < m_maxThreadsCount * K; ++n)
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

//-------------------------------------------------------------------------------------------------------------------//

template <typename F, typename ... Args>
void TRAP::EXPERIMENTAL::ThreadPool::EnqueueWork(F&& f, Args&&... args)
{
	auto work = [p = std::forward<F>(f), t = { std::forward<Args>(args)... }]()
	{
		std::apply(p, t);
	};
	const auto i = m_index++;

	for(uint32_t n = 0; n < m_maxThreadsCount * K; ++n)
	{
		if (m_queues[(i + n) % m_maxThreadsCount].TryPush(work))
			return;
	}

	m_queues[i % m_maxThreadsCount].Push(std::move(work));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename F, typename ... Args>
auto TRAP::EXPERIMENTAL::ThreadPool::EnqueueTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
	using TaskReturnType = std::invoke_result_t<F, Args...>;
	using TaskType = std::packaged_task<TaskReturnType()>;

	auto task = std::make_shared<TaskType>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	auto work = [task]()
	{
		(*task)();
	};
	auto result = task->get_future();
	const auto i = m_index++;

	for(uint32_t n = 0; n < m_maxThreadsCount * K; ++n)
	{
		if (m_queues[(i + n) % m_maxThreadsCount].TryPush(work))
			return result;
	}

	m_queues[i % m_maxThreadsCount].Push(std::move(work));

	return result;
}