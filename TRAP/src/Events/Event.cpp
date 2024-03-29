#include "TRAPPCH.h"
#include "Event.h"

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return os << e.ToString();
}
