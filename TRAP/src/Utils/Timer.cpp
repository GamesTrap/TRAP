#include "TRAPPCH.h"
#include "Timer.h"

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::Timer::Elapsed() const
{
	return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
	      (std::chrono::high_resolution_clock::now() - m_start).count() / 1000.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::Timer::ElapsedMilliseconds() const
{
	return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
		(std::chrono::high_resolution_clock::now() - m_start).count();
}