#include "TRAPPCH.h"
#include "Event.h"

std::string TRAP::Events::Event::ToString() const
{
	return GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Events::Event::IsInCategory(EventCategory category) const
{
	return GetCategoryFlags() & static_cast<int32_t>(category);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventDispatcher::EventDispatcher(Event& event)
	: m_event(event)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e)
{
	return os << e.ToString();
}