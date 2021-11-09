#ifndef TRAP_TIMESTEP_H
#define TRAP_TIMESTEP_H

namespace TRAP::Utils
{
	/// <summary>
	/// Struct for a single step in time.
	/// </summary>
	struct TimeStep
	{
		/// <summary>
		/// Constructor for a single step in time.
		/// </summary>
		/// <param name="time">Time for the time step.</param>
		explicit TimeStep(float time);

		/// <summary>
		/// Destructor.
		/// </summary>
		~TimeStep() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		TimeStep(const TimeStep&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		TimeStep(TimeStep&&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		TimeStep& operator=(const TimeStep&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		TimeStep& operator=(TimeStep&&) = default;

		/// <summary>
		/// Convenience operator.
		/// Same as GetSeconds();
		/// </summary>
		/// <returns>Time of the time step in seconds.</returns>
		operator float() const;

		/// <summary>
		/// Get time of the time step in seconds.
		/// </summary>
		/// <returns>Time in seconds.</returns>
		float GetSeconds() const;
		/// <summary>
		/// Get time of the time step in milliseconds.
		/// </summary>
		/// <returns>Time in milliseconds.</returns>
		float GetMilliseconds() const;

	private:
		float m_time;
	};
}

#endif /*TRAP_TIMESTEP_H*/