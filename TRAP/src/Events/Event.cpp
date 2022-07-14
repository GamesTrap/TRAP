#include "TRAPPCH.h"
#include "Event.h"

std::string TRAP::Events::Event::ToString() const
{
	return GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Events::Event::IsInCategory(const EventCategory category) const
{
	return static_cast<bool>(GetCategoryFlags() & category);
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e)
{
	return os << e.ToString();
}
