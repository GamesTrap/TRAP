#include "TRAPPCH.h"
#include "TimeStep.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep::TimeStep(const float initialTime)
	: m_timeStep(0.0f), m_lastTime(initialTime)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::TimeStep::Update(const float currentTime)
{
	m_timeStep = currentTime - m_lastTime;
	m_lastTime = currentTime;
}