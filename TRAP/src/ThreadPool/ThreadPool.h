#ifndef _TRAP_THREADPOOL_H_
#define _TRAP_THREADPOOL_H_

#include "BlockingQueue.h"

namespace TRAP
{
	/// <summary>
	/// ThreadPool is a multi-thread task scheduler.<br>
	/// Note: ThreadPool should only be used for small tasks!
	/// </summary>
	class ThreadPool
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="threads">Max amount of threads to use for tasks.</param>
		explicit ThreadPool(uint32_t threads = (std::thread::hardware_concurrency() - 1));
		/// <summary>
		/// Destructor.
		/// </summary>
		~ThreadPool() noexcept;

		/// <summary>
		/// Deleted Copy Constructor.
		/// </summary>
		ThreadPool(const ThreadPool&) = delete;
		/// <summary>
		/// Deleted Copy Assignment Operator.
		/// </summary>
		ThreadPool& operator=(const ThreadPool&) = delete;
		/// <summary>
		/// Deleted Move Constructor.
		/// </summary>
		ThreadPool(ThreadPool&&) = delete;
		/// <summary>
		/// Deleted Move Assignment Operator.
		/// </summary>
		ThreadPool& operator=(ThreadPool&&) = delete;

		/// <summary>
		/// Enqueue work (Call given function on separate thread).
		/// </summary>
		/// <typeparam name="F">Functor to work on.</typeparam>
		/// <typeparam name="...Args">Parameters of the functor.</typeparam>
		/// <param name="f">Functor to work on.</param>
		template<typename F, typename... Args>
		void EnqueueWork(F&& f, Args&&... args);

		/// <summary>
		/// Enqueue task (Call given function on seperate thread and get a future).
		/// </summary>
		/// <typeparam name="F">Functor to work on.</typeparam>
		/// <typeparam name="...Args">Parameters of the functor.</typeparam>
		/// <param name="f">Functor to work on.</param>
		/// <returns>Future for the result of the given functor.</returns>
		template<typename F, typename... Args>
		[[nodiscard]] auto EnqueueTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

	private:
		using Proc = std::function<void()>;
		using Queue = BlockingQueue<Proc>;
		using Queues = std::vector<Queue>;
		Queues m_queues;

		using Threads = std::vector<std::thread>;
		Threads m_threads;

		uint32_t m_maxThreadsCount;
		std::atomic_uint m_index = 0;

		inline static constexpr uint32_t K = 3;
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
	const uint32_t i = m_index++;

	for (uint32_t n = 0; n < m_maxThreadsCount * K; ++n)
	{
		if (m_queues[(i + n) % m_maxThreadsCount].TryPush(std::move(work)))
			return;
	}

	m_queues[i % m_maxThreadsCount].Push(work);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename F, typename ... Args>
auto TRAP::ThreadPool::EnqueueTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
	using TaskReturnType = std::invoke_result_t<F, Args...>;
	using TaskType = std::packaged_task<TaskReturnType()>;
	
	auto task = std::make_shared<TaskType>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	std::future<TaskReturnType> result = task->get_future();
	
	auto work = [task]()
	{
		(*task)();
	};
	const uint32_t i = m_index++;

	for (uint32_t n = 0; n < m_maxThreadsCount * K; ++n)
	{
		if (m_queues[(i + n) % m_maxThreadsCount].TryPush(work))
			return result;
	}

	m_queues[i % m_maxThreadsCount].Push(std::move(work));

	return result;
}

#endif /*_TRAP_THREADPOOL_H_*/