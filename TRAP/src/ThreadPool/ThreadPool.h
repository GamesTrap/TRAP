#ifndef _TRAP_THREADPOOL_H_
#define _TRAP_THREADPOOL_H_

namespace TRAP::EXPERIMENTAL
{
	namespace INTERNAL
	{
		template<typename T>
		class BlockingQueue;
	}
	
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
		using Queue = INTERNAL::BlockingQueue<Proc>;
		using Queues = std::vector<Queue>;
		Queues m_queues;

		using Threads = std::vector<std::thread>;
		Threads m_threads;

		uint32_t m_maxThreadsCount;
		std::atomic_uint m_index = 0;

		inline static const uint32_t K = 2;
	};
}

#endif /*_TRAP_THREADPOOL_H_*/