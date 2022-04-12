#ifndef TRAP_TIMER_H
#define TRAP_TIMER_H

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
		Timer();

		/// <summary>
		/// Resets and restarts the timer.
		/// </summary>
		void Reset();
		/// <summary>
		/// Retrieve elapsed time in seconds.
		/// </summary>
		/// <returns>Elapsed time in seconds.</returns>
		float Elapsed() const;
		/// <summary>
		/// Retrieve elapsed time in milliseconds.
		/// </summary>
		/// <returns>Elapsed time in milliseconds.</returns>
		float ElapsedMilliseconds() const;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	};
}

#endif /*TRAP_TIMER_H*/