#ifndef TRAP_TIMER_H
#define TRAP_TIMER_H

#include <chrono>

#include "Core/Types.h"

namespace TRAP::Utils
{
	/// @brief Timer class.
	class Timer
	{
	public:
		/// @brief Constructor.
		/// Creates and starts a new timer.
		Timer() noexcept;

		constexpr Timer(const Timer&) = default;
		Timer& operator=(const Timer&) = default;
		constexpr Timer(Timer&&) noexcept = default;
		Timer& operator=(Timer&&) noexcept = default;

		/// @brief Destructor.
		constexpr ~Timer() = default;

		/// @brief Resets and restarts the timer.
		void Reset() noexcept;
		/// @brief Retrieve elapsed time in seconds.
		/// @return Elapsed time in seconds.
		[[nodiscard]] f32 Elapsed() const;
		/// @brief Retrieve elapsed time in milliseconds.
		/// @return Elapsed time in milliseconds.
		[[nodiscard]] f32 ElapsedMilliseconds() const;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	};
}

#endif /*TRAP_TIMER_H*/