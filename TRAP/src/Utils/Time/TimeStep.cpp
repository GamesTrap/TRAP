#include "TRAPPCH.h"
#include "TimeStep.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep::TimeStep(const float time)
	: m_time(time)
{
	ZoneScopedC(tracy::Color::Violet);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep::operator float() const
{
	ZoneScopedC(tracy::Color::Violet);

	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::TimeStep::GetSeconds() const
{
	ZoneScopedC(tracy::Color::Violet);

	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::TimeStep::GetMilliseconds() const
{
	ZoneScopedC(tracy::Color::Violet);

	return m_time * 1000.0f;
}