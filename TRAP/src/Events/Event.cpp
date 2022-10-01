#include "TRAPPCH.h"
#include "Event.h"

std::string TRAP::Events::Event::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Events::Event::IsInCategory(const EventCategory category) const
{
	ZoneScopedC(tracy::Color::Purple);

	return static_cast<bool>(GetCategoryFlags() & category);
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e)
{
	ZoneScopedC(tracy::Color::Purple);

	return os << e.ToString();
}
