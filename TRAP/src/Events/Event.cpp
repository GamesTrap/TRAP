#include "TRAPPCH.h"
#include "Event.h"

std::string TRAP::Events::Event::ToString() const
{
	ZoneScoped;

	return GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Events::Event::IsInCategory(const EventCategory category) const
{
	ZoneScoped;

	return static_cast<bool>(GetCategoryFlags() & category);
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e)
{
	ZoneScoped;

	return os << e.ToString();
}
