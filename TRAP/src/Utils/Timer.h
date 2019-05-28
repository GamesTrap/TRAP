#pragma once

#include <chrono>

namespace TRAP::Utils
{
	class Timer
	{
	public:
		//Creates and starts timer
		Timer();
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