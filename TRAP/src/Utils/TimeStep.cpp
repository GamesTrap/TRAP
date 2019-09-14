#include "TRAPPCH.h"
#include "TimeStep.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep::TimeStep(const float time)
	: m_time(time)
{
}

//-------------------------------------------------------------------------------------------------------------------//

/*void TRAP::Utils::TimeStep::Update(const float currentTime)
{
	m_timeStep = currentTime - m_lastTime;
	m_lastTime = currentTime;
}*/