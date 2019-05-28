#include "TRAPPCH.h"
#include "TimeStep.h"

namespace TRAP::Utils
{
	TimeStep::TimeStep(const float initialTime)
		: m_timeStep(0.0f), m_lastTime(initialTime)
	{
	}

	void TimeStep::Update(const float currentTime)
	{
		m_timeStep = currentTime - m_lastTime;
		m_lastTime = currentTime;
	}

	float TimeStep::GetMilliseconds() const
	{
		return m_timeStep;
	}

	float TimeStep::GetSeconds() const
	{
		return m_timeStep * 0.001f;
	}
}