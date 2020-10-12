#ifndef _TRAP_TIMESTEP_H_
#define _TRAP_TIMESTEP_H_

namespace TRAP::Utils
{
	/// <summary>
	/// Struct for a single step in Time.
	/// </summary>
	struct TimeStep
	{
		/// <summary>
		/// Constructor for a single TimeStep.
		/// </summary>
		/// <param name="time">Time for the TimeStep.</param>
		explicit TimeStep(float time);

		/// <summary>
		/// Default Destructor.
		/// </summary>
		~TimeStep() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		TimeStep(const TimeStep&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		TimeStep(TimeStep&&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		TimeStep& operator=(const TimeStep&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		TimeStep& operator=(TimeStep&&) = default;

		/// <summary>
		/// Convenience operator.<br>
		/// Same as GetSeconds();
		/// </summary>
		/// <returns>Time of TimeStep in seconds.</returns>
		operator float() const;

		/// <summary>
		/// Get time of TimeStep in seconds.
		/// </summary>
		/// <returns>Time in seconds.</returns>
		float GetSeconds() const;
		/// <summary>
		/// Get time of TimeStep in milliseconds.
		/// </summary>
		/// <returns>Time in milliseconds.</returns>
		float GetMilliseconds() const;

	private:
		float m_time;
	};
}

#endif /*_TRAP_TIMESTEP_H_*/