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

TRAP::Events::EventDispatcher::EventDispatcher(Event& event)
	: m_event(event)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e)
{
	return os << e.ToString();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory operator |(TRAP::Events::EventCategory lhs, TRAP::Events::EventCategory rhs)
{
	return static_cast<TRAP::Events::EventCategory>
		(
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(lhs) |
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);
}

TRAP::Events::EventCategory operator &(TRAP::Events::EventCategory lhs, TRAP::Events::EventCategory rhs)
{
	return static_cast<TRAP::Events::EventCategory>
		(
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(lhs) &
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);
}

TRAP::Events::EventCategory operator ^(TRAP::Events::EventCategory lhs, TRAP::Events::EventCategory rhs)
{
	return static_cast<TRAP::Events::EventCategory>
		(
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(lhs) ^
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);
}

TRAP::Events::EventCategory operator ~(TRAP::Events::EventCategory rhs)
{
	return static_cast<TRAP::Events::EventCategory>
		(
			~static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);
}

TRAP::Events::EventCategory& operator |=(TRAP::Events::EventCategory& lhs, TRAP::Events::EventCategory rhs)
{
	lhs = static_cast<TRAP::Events::EventCategory>
		(
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(lhs) |
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);

	return lhs;
}

TRAP::Events::EventCategory& operator &=(TRAP::Events::EventCategory& lhs, TRAP::Events::EventCategory rhs)
{
	lhs = static_cast<TRAP::Events::EventCategory>
		(
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(lhs) &
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);

	return lhs;
}

TRAP::Events::EventCategory& operator ^=(TRAP::Events::EventCategory& lhs, TRAP::Events::EventCategory rhs)
{
	lhs = static_cast<TRAP::Events::EventCategory>
		(
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(lhs) ^
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);

	return lhs;
}