#ifndef TRAP_THREADPOOL_H
#define TRAP_THREADPOOL_H

#include <functional>
#include <future>
#include <atomic>
#include <thread>

#include "BlockingQueue.h"

namespace TRAP
{
	/// @brief ThreadPool is a multi-threading task scheduler.
	class ThreadPool
	{
	public:
		/// @brief Constructor.
		/// @param threads Max amount of threads to use for tasks.
		explicit ThreadPool(u32 threads = (std::thread::hardware_concurrency() - 1));
		/// @brief Destructor.
		~ThreadPool();

		/// @brief Copy constructor.
		constexpr ThreadPool(const ThreadPool&) = delete;
		/// @brief Copy assignment operator.
		constexpr ThreadPool& operator=(const ThreadPool&) = delete;
		/// @brief Move constructor.
		constexpr ThreadPool(ThreadPool&&) = delete;
		/// @brief Move assignment operator.
		constexpr ThreadPool& operator=(ThreadPool&&) = delete;

		/// @brief Enqueue work (Call given function on separate thread).
		/// @tparam F Functor to work on.
		/// @tparam Args Parameters of the functor.
		/// @param f Functor to work on.
		/// @param args Optional arguments.
		template<typename F, typename... Args>
		requires std::invocable<F, Args...>
		void EnqueueWork(F&& f, Args&&... args);

		/// @brief Enqueue task (Call given function on seperate thread and get a future).
		/// @tparam F Functor to work on.
		/// @tparam Args Parameters of the functor.
		/// @param f Functor to work on.
		/// @param args Optional arguments.
		/// @return Future for the result of the given functor.
		template<typename F, typename... Args>
		requires std::invocable<F, Args...>
		[[nodiscard]] auto EnqueueTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

	private:
		using Proc = std::function<void()>;
		using Queue = BlockingQueue<Proc>;
		using Queues = std::vector<Queue>;
		Queues m_queues;

		using Threads = std::vector<std::jthread>;
		Threads m_threads;

		u32 m_maxThreadsCount;
		std::atomic<u32> m_index = 0;

		inline static constexpr u32 K = 3;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename F, typename ... Args>
requires std::invocable<F, Args...>
void TRAP::ThreadPool::EnqueueWork(F&& f, Args&&... args)
{
	ZoneNamed(__tracy, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	auto work = [p = std::forward<F>(f), t = { std::forward<Args>(args)... }]()
	{
		std::apply(p, t);
	};
	const u32 i = m_index++;

	for (u32 n = 0; n < m_maxThreadsCount * K; ++n)
	{
		if (m_queues[(i + n) % m_maxThreadsCount].TryPush(std::move(work)))
			return;
	}

	m_queues[i % m_maxThreadsCount].Push(work);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename F, typename ... Args>
requires std::invocable<F, Args...>
[[nodiscard]] auto TRAP::ThreadPool::EnqueueTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
	ZoneNamed(__tracy, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	using TaskReturnType = std::invoke_result_t<F, Args...>;
	using TaskType = std::packaged_task<TaskReturnType()>;

	const auto task = std::make_shared<TaskType>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	const std::future<TaskReturnType> result = task->get_future();

	auto work = [task]()
	{
		(*task)();
	};
	const u32 i = m_index++;

	for (u32 n = 0; n < m_maxThreadsCount * K; ++n)
	{
		if (m_queues[(i + n) % m_maxThreadsCount].TryPush(work))
			return result;
	}

	m_queues[i % m_maxThreadsCount].Push(std::move(work));

	return result;
}

#endif /*TRAP_THREADPOOL_H*/