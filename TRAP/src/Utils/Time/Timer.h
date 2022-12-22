#ifndef TRAP_TIMER_H
#define TRAP_TIMER_H

#include <chrono>

namespace TRAP::Utils
{
	/// <summary>
	/// Timer class.
	/// </summary>
	class Timer
	{
	public:
		/// <summary>
		/// Constructor.
		/// Creates and starts a new timer.
		/// </summary>
		Timer() noexcept;

		/// <summary>
		/// Resets and restarts the timer.
		/// </summary>
		void Reset() noexcept;
		/// <summary>
		/// Retrieve elapsed time in seconds.
		/// </summary>
		/// <returns>Elapsed time in seconds.</returns>
		[[nodiscard]] float Elapsed() const;
		/// <summary>
		/// Retrieve elapsed time in milliseconds.
		/// </summary>
		/// <returns>Elapsed time in milliseconds.</returns>
		[[nodiscard]] float ElapsedMilliseconds() const;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	};
}

#endif /*TRAP_TIMER_H*/