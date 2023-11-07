#ifndef TRAP_TIMESTEP_H
#define TRAP_TIMESTEP_H

#include "Core/Types.h"

namespace TRAP::Utils
{
	/// @brief Struct for a single step in time.
	struct TimeStep
	{
		/// @brief Constructor for a single step in time.
		/// @param time Time for the time step in seconds.
		constexpr explicit TimeStep(f32 time) noexcept;

		/// @brief Convenience operator.
		/// Same as GetSeconds();
		/// @return Time of the time step in seconds.
		[[nodiscard]] constexpr operator f32() const noexcept;

		/// @brief Get time of the time step in seconds.
		/// @return Time in seconds.
		[[nodiscard]] constexpr f32 GetSeconds() const noexcept;
		/// @brief Get time of the time step in milliseconds.
		/// @return Time in milliseconds.
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