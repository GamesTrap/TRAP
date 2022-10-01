#include "TRAPPCH.h"
#include "Timer.h"

TRAP::Utils::Timer::Timer()
{
	ZoneScoped;

	Reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Timer::Reset()
{
	ZoneScoped;

	m_start = std::chrono::high_resolution_clock::now();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::Timer::Elapsed() const
{
	ZoneScoped;

	return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
		(
			std::chrono::high_resolution_clock::now() - m_start
		).count() / 1000.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::Timer::ElapsedMilliseconds() const
{
	ZoneScoped;

	return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
		(
			std::chrono::high_resolution_clock::now() - m_start
		).count();
}