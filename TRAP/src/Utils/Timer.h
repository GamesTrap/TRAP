#ifndef _TRAP_TIMER_H_
#define _TRAP_TIMER_H_

#include <chrono>

namespace TRAP::Utils
{
	class Timer
	{
	public:
		//Creates and starts timer
		Timer() { Reset(); }
		//Resets and restarts timer
		void Reset();
		//Returns time in seconds
		float Elapsed() const;
		//Returns time in milliseconds
		float ElapsedMilliseconds() const;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Utils::Timer::Reset()
{
	m_start = std::chrono::high_resolution_clock::now();
}

#endif /*_TRAP_TIMER_H_*/