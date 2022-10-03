#include "TRAPPCH.h"
#include "TimeStep.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep::TimeStep(const float time)
	: m_time(time)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep::operator float() const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::TimeStep::GetSeconds() const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Utils::TimeStep::GetMilliseconds() const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_time * 1000.0f;
}