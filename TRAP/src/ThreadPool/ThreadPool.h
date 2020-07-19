#ifndef _TRAP_THREADPOOL_H_
#define _TRAP_THREADPOOL_H_

#include "BlockingQueue.h"

namespace TRAP
{	
	class ThreadPool
	{
	public:
		explicit ThreadPool(uint32_t threads = std::thread::hardware_concurrency());
		~ThreadPool() noexcept;

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		template<typename F, typename... Args>
		void EnqueueWork(F&& f, Args&&... args);

		template<typename F, typename... Args>
		[[nodiscard]] auto EnqueueTask(F&& f, Args&&... args)->std::future<std::invoke_result_t<F, Args...>>;

	private:
		using Proc = std::function<void()>;
		using Queue = BlockingQueue<Proc>;
		using Queues = std::vector<Queue>;
		Queues m_queues;

		using Threads = std::vector<std::thread>;
		Threads m_threads;

		uint32_t m_maxThreadsCount;
		std::atomic_uint m_index = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename F, typename ... Args>
void TRAP::ThreadPool::EnqueueWork(F&& f, Args&&... args)
{
	auto work = [p = std::forward<F>(f), t = { std::forward<Args>(args)... }]()
	{
		std::apply(p, t);
	};
	const auto i = m_index++;

	for (uint32_t n = 0; n < m_maxThreadsCount; ++n)
	{
		if (m_queues[(i + n) % m_maxThreadsCount].TryPush(work))
			return;
	}

	m_queues[i % m_maxThreadsCount].Push(std::move(work));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename F, typename ... Args>
auto TRAP::ThreadPool::EnqueueTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
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

	for (uint32_t n = 0; n < m_maxThreadsCount; ++n)
	{
		if (m_queues[(i + n) % m_maxThreadsCount].TryPush(work))
			return result;
	}

	m_queues[i % m_maxThreadsCount].Push(std::move(work));

	return result;
}

#endif /*_TRAP_THREADPOOL_H_*/