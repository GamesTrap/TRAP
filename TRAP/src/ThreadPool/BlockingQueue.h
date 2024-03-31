#ifndef TRAP_BLOCKINGQUEUE_H
#define TRAP_BLOCKINGQUEUE_H

#include "Utils/Concurrency/Safe.h"
#include <queue>
#include <condition_variable>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//Tracy - Profiler
#include <tracy/Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Core/Types.h"

namespace TRAP
{
	template<typename T>
	class BlockingQueue
	{
	public:
		/// @brief Push a copy constructable item to the queue.
		/// @param item Item to push.
		template<typename Q = T>
		requires std::copy_constructible<Q>
		void Push(const T& item);

		/// @brief Push a move constructable item to the queue.
		/// @param item Item to push.
		template<typename Q = T>
		requires std::move_constructible<Q>
		void Push(T&& item);

		/// @brief Try to push a copy constructable item to the queue.
		/// @param item Item to push.
		/// @return True on success, false otherwise.
		template<typename Q = T>
		requires std::copy_constructible<Q>
		bool TryPush(const T& item);

		/// @brief Try to push a move constructable item to the queue.
		/// @param item Item to push.
		/// @return True on success, false otherwise.
		template<typename Q = T>
		requires std::move_constructible<Q>
		bool TryPush(T&& item);

		/// @brief Pop a copy assignable item from the queue.
		/// @param item Item to pop.
		template<typename Q = T>
		requires (std::is_copy_assignable_v<Q> && !std::is_move_assignable_v<Q>)
		bool Pop(T& item);

		/// @brief Pop a move assignable item from the queue.
		/// @param item Item to pop.
		template<typename Q = T>
		requires std::is_move_assignable_v<Q>
		bool Pop(T& item);

		/// @brief Try to pop a copy assignable item from the queue.
		/// @param item Item to pop.
		/// @return True on success, false otherwise.
		template<typename Q = T>
		requires (std::is_copy_assignable_v<Q> && !std::is_move_assignable_v<Q>)
		bool TryPop(T& item);

		/// @brief Try to pop a move assignable item from the queue.
		/// @param item Item to pop.
		/// @return True on success, false otherwise.
		template<typename Q = T>
		requires std::is_move_assignable_v<Q>
		bool TryPop(T& item);

		/// @brief Mark the queue as done.
		void Done() noexcept;

		/// @brief Check if queue is empty.
		/// @return True if queue is empty, false otherwise.
		[[nodiscard]] bool Empty() const noexcept;

		/// @brief Retrieve the size of the queue.
		/// @return Queue size.
		[[nodiscard]] u32 Size() const noexcept;

	private:
		struct UnsafeData
		{
			std::queue<T> Queue{};
			bool Done = false;
		};
		Utils::Safe<UnsafeData> m_safeData{};
		std::condition_variable_any m_ready;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
requires std::copy_constructible<Q>
void TRAP::BlockingQueue<T>::Push(const T& item)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	m_safeData.WriteLock()->Queue.push(item);

	m_ready.notify_one();
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
requires std::move_constructible<Q>
void TRAP::BlockingQueue<T>::Push(T&& item)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	m_safeData.WriteLock()->Queue.emplace(std::forward<T>(item));

	m_ready.notify_one();
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
requires std::copy_constructible<Q>
bool TRAP::BlockingQueue<T>::TryPush(const T& item)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	{
		auto data = m_safeData.WriteLock(std::try_to_lock);
		if(!data.Lock)
			return false;

		data->Queue.push(item);
	}

	m_ready.notify_one();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
requires std::move_constructible<Q>
bool TRAP::BlockingQueue<T>::TryPush(T&& item)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	{
		auto data = m_safeData.WriteLock(std::try_to_lock);
		if(!data.Lock)
			return false;

		data->Queue.emplace(std::forward<T>(item));
	}

	m_ready.notify_one();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
requires (std::is_copy_assignable_v<Q> && !std::is_move_assignable_v<Q>)
bool TRAP::BlockingQueue<T>::Pop(T& item)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	auto data = m_safeData.WriteLock();

	while (data->Queue.empty() && !data->Done)
		m_ready.wait(data.Lock);

	if (data->Queue.empty())
		return false;

	item = data->Queue.front();
	data->Queue.pop();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
requires std::is_move_assignable_v<Q>
bool TRAP::BlockingQueue<T>::Pop(T& item)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	auto data = m_safeData.WriteLock();

	while (data->Queue.empty() && !data->Done)
		m_ready.wait(data.Lock);

	if (data->Queue.empty())
		return false;

	item = std::move(data->Queue.front());
	data->Queue.pop();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
requires (std::is_copy_assignable_v<Q> && !std::is_move_assignable_v<Q>)
bool TRAP::BlockingQueue<T>::TryPop(T& item)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	auto data = m_safeData.WriteLock(std::try_to_lock);

	if (!data.Lock || data->Queue.empty())
		return false;

	item = data->Queue.front();
	data->Queue.pop();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
requires std::is_move_assignable_v<Q>
bool TRAP::BlockingQueue<T>::TryPop(T& item)
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	auto data = m_safeData.WriteLock(std::try_to_lock);

	if (!data.Lock || data->Queue.empty())
		return false;

	item = std::move(data->Queue.front());
	data->Queue.pop();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
void TRAP::BlockingQueue<T>::Done() noexcept
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	m_safeData.WriteLock()->Done = true;

	m_ready.notify_all();
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
[[nodiscard]] bool TRAP::BlockingQueue<T>::Empty() const noexcept
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	return m_safeData.ReadLock()->Queue.empty();
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
[[nodiscard]] u32 TRAP::BlockingQueue<T>::Size() const noexcept
{
	ZoneNamed(__tracy, (GetTRAPProfileSystems() & ProfileSystems::ThreadPool) != ProfileSystems::None);

	return m_safeData.ReadLock()->Queue.size();
}

#endif /*TRAP_BLOCKINGQUEUE_H*/