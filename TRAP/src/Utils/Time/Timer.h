#ifndef _TRAP_TIMER_H_
#define _TRAP_TIMER_H_

namespace TRAP::Utils
{
	/// <summary>
	/// Timer class.
	/// </summary>
	class Timer
	{
	public:
		/// <summary>
		/// Default Constructor.<br>
		/// Creates and starts a new Timer.
		/// </summary>
		Timer();

		/// <summary>
		/// Default Destructor.
		/// </summary>
		~Timer() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Timer(const Timer&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Timer(Timer&&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Timer& operator=(const Timer&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Timer& operator=(Timer&&) = default;
		
		/// <summary>
		/// Resets and restarts the Timer.
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

#endif /*_TRAP_TIMER_H_*/