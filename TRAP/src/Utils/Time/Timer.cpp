#include "TRAPPCH.h"
#include "Timer.h"

TRAP::Utils::Timer::Timer() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	Reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Timer::Reset() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	m_start = std::chrono::high_resolution_clock::now();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Utils::Timer::Elapsed() const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	return std::chrono::duration_cast<std::chrono::duration<f32, std::milli>>
		(
			std::chrono::high_resolution_clock::now() - m_start
		).count() / 1000.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Utils::Timer::ElapsedMilliseconds() const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	return std::chrono::duration_cast<std::chrono::duration<f32, std::milli>>
		(
			std::chrono::high_resolution_clock::now() - m_start
		).count();
}