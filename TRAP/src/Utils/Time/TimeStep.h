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
		/// <param name="time">Time for the time step in seconds.</param>
		explicit TimeStep(float time);

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