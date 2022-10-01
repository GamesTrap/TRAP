#include "TRAPPCH.h"
#include "Timer.h"

TRAP::Utils::Timer::Timer()
{
	ZoneScopedC(tracy::Color::Violet);

	Reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Timer::Reset()
{
	ZoneScopedC(tracy::Color::Violet);

	m_start = std::chrono::high_resolution_clock::now();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::Timer::Elapsed() const
{
	ZoneScopedC(tracy::Color::Violet);

	return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
		(
			std::chrono::high_resolution_clock::now() - m_start
		).count() / 1000.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::Timer::ElapsedMilliseconds() const
{
	ZoneScopedC(tracy::Color::Violet);

	return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
		(
			std::chrono::high_resolution_clock::now() - m_start
		).count();
}