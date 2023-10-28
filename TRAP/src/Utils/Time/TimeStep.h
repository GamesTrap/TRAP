#ifndef TRAP_TIMESTEP_H
#define TRAP_TIMESTEP_H

#include "Core/Types.h"

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
		constexpr explicit TimeStep(f32 time) noexcept;

		/// <summary>
		/// Convenience operator.
		/// Same as GetSeconds();
		/// </summary>
		/// <returns>Time of the time step in seconds.</returns>
		[[nodiscard]] constexpr operator f32() const noexcept;

		/// <summary>
		/// Get time of the time step in seconds.
		/// </summary>
		/// <returns>Time in seconds.</returns>
		[[nodiscard]] constexpr f32 GetSeconds() const noexcept;
		/// <summary>
		/// Get time of the time step in milliseconds.
		/// </summary>
		/// <returns>Time in milliseconds.</returns>
		[[nodiscard]] constexpr f32 GetMilliseconds() const noexcept;

	private:
		f32 m_time;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Utils::TimeStep::TimeStep(const f32 time) noexcept
	: m_time(time)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Utils::TimeStep::operator f32() const noexcept
{
	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Utils::TimeStep::GetSeconds() const noexcept
{
	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Utils::TimeStep::GetMilliseconds() const noexcept
{
	return m_time * 1000.0f;
}

#endif /*TRAP_TIMESTEP_H*/