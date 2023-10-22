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
		constexpr explicit TimeStep(float time) noexcept;

		/// <summary>
		/// Convenience operator.
		/// Same as GetSeconds();
		/// </summary>
		/// <returns>Time of the time step in seconds.</returns>
		[[nodiscard]] constexpr operator float() const noexcept;

		/// <summary>
		/// Get time of the time step in seconds.
		/// </summary>
		/// <returns>Time in seconds.</returns>
		[[nodiscard]] constexpr float GetSeconds() const noexcept;
		/// <summary>
		/// Get time of the time step in milliseconds.
		/// </summary>
		/// <returns>Time in milliseconds.</returns>
		[[nodiscard]] constexpr float GetMilliseconds() const noexcept;

	private:
		float m_time;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Utils::TimeStep::TimeStep(const float time) noexcept
	: m_time(time)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Utils::TimeStep::operator float() const noexcept
{
	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Utils::TimeStep::GetSeconds() const noexcept
{
	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Utils::TimeStep::GetMilliseconds() const noexcept
{
	return m_time * 1000.0f;
}

#endif /*TRAP_TIMESTEP_H*/