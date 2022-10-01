#include "TRAPPCH.h"
#include "TimeStep.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep::TimeStep(const float time)
	: m_time(time)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep::operator float() const
{
	ZoneScoped;

	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::TimeStep::GetSeconds() const
{
	ZoneScoped;

	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::TimeStep::GetMilliseconds() const
{
	ZoneScoped;

	return m_time * 1000.0f;
}