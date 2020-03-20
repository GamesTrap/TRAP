#include "TRAPPCH.h"
#include "BlockingQueue.h"

template <typename T>
template <typename Q>
typename std::enable_if<std::is_copy_constructible<Q>::value, void>::type TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::Push(const T& item)
{
	{
		std::unique_lock lock(m_mutex);
		m_queue.push(item);
	}

	m_ready.notify_one();
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
typename std::enable_if<std::is_move_constructible<Q>::value, void>::type TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::Push(T&& item)
{
	{
		std::unique_lock lock(m_mutex);
		m_queue.emplace(std::forward<T>(item));
	}

	m_ready.notify_one();
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
typename std::enable_if<std::is_copy_constructible<Q>::value, bool>::type TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::TryPush(const T& item)
{
	{
		const std::unique_lock lock(m_mutex, std::try_to_lock);
		if (!lock)
			return false;
		
		m_queue.push(item);
	}

	m_ready.notify_one();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
typename std::enable_if<std::is_move_constructible<Q>::value, bool>::type TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::TryPush(T&& item)
{
	{
		const std::unique_lock lock(m_mutex, std::try_to_lock);
		if (!lock)
			return false;

		m_queue.emplace(std::forward<T>(item));
	}

	m_ready.notify_one();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
typename std::enable_if<std::is_copy_assignable<Q>::value && !std::is_move_assignable<Q>::value, bool>::type TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::Pop(T& item)
{
	std::unique_lock lock(m_mutex);
	while (m_queue.empty() && !m_done)
		m_ready.wait(lock);

	if (m_queue.empty())
		return false;

	item = m_queue.front();
	m_queue.pop();
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
typename std::enable_if<std::is_move_assignable<Q>::value, bool>::type TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::Pop(T& item)
{
	std::unique_lock lock(m_mutex);
	while (m_queue.empty() && !m_done)
		m_ready.wait(lock);

	if (m_queue.empty())
		return false;

	item = std::move(m_queue.front());
	m_queue.pop();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
typename std::enable_if<std::is_copy_assignable<Q>::value && !std::is_move_assignable<Q>::value, bool>::type TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::TryPop(T& item)
{
	const std::unique_lock lock(m_mutex, std::try_to_lock);
	if (!lock || m_queue.empty())
		return false;

	item = m_queue.front();
	m_queue.pop();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
template <typename Q>
typename std::enable_if<std::is_move_assignable<Q>::value, bool>::type TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::TryPop(T& item)
{
	const std::unique_lock lock(m_mutex, std::try_to_lock);
	if (!lock || m_queue.empty())
		return false;

	item = std::move(m_queue.front());
	m_queue.pop();
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
void TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::Done() noexcept
{
	{
		std::unique_lock lock(m_mutex);
		m_done = true;
	}

	m_ready.notify_all();
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
void TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::Empty() const noexcept
{
	std::scoped_lock lock(m_mutex);
	return m_queue.empty();
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
uint32_t TRAP::EXPERIMENTAL::INTERNAL::BlockingQueue<T>::Size() const noexcept
{
	std::scoped_lock lock(m_mutex);
	return m_queue.size();
}