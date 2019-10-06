#include "TRAPPCH.h"
#include "Event.h"

std::string TRAP::Event::ToString() const
{
	return GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Event::IsInCategory(EventCategory category) const
{
	return GetCategoryFlags() & static_cast<int32_t>(category);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventDispatcher::EventDispatcher(Event& event)
	: m_event(event)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& TRAP::operator<<(std::ostream& os, const TRAP::Event& e)
{
	return os << e.ToString();
}