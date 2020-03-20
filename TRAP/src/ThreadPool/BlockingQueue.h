#ifndef _TRAP_BLOCKINGQUEUE_H_
#define _TRAP_BLOCKINGQUEUE_H_

namespace TRAP::EXPERIMENTAL::INTERNAL
{
	template<typename T>
	class BlockingQueue
	{
	public:
		template<typename Q = T>
		typename std::enable_if<std::is_copy_constructible<Q>::value, void>::type
		Push(const T& item);

		template<typename Q = T>
		typename std::enable_if<std::is_move_constructible<Q>::value, void>::type
		Push(T&& item);

		template<typename Q = T>
		typename std::enable_if<std::is_copy_constructible<Q>::value, bool>::type
		TryPush(const T& item);

		template<typename Q = T>
		typename std::enable_if<std::is_move_constructible<Q>::value, bool>::type
		TryPush(T&& item);

		template<typename Q = T>
		typename std::enable_if<std::is_copy_assignable<Q>::value && !std::is_move_assignable<Q>::value, bool>::type
		Pop(T& item);

		template<typename Q = T>
		typename std::enable_if<std::is_move_assignable<Q>::value, bool>::type
		Pop(T& item);

		template<typename Q = T>
		typename std::enable_if<std::is_copy_assignable<Q>::value && !std::is_move_assignable<Q>::value, bool>::type
		TryPop(T& item);

		template<typename Q = T>
		typename std::enable_if<std::is_move_assignable<Q>::value, bool>::type
		TryPop(T& item);

		void Done() noexcept;

		void Empty() const noexcept;

		uint32_t Size() const noexcept;

	private:
		std::queue<T> m_queue;
		mutable std::mutex m_mutex;
		std::condition_variable m_ready;
		bool m_done = false;
	};
}

#endif /*_TRAP_BLOCKINGQUEUE_H_*/