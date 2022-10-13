#include "TRAPPCH.h"
#include "MouseEvent.h"

#include "Utils/String/String.h"

std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseMoveEvent: " + std::to_string(m_mouseX) + ", " + std::to_string(m_mouseY);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetEventType() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseMoveEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseMove";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseMoveEvent::GetCategoryFlags() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseScrollEvent: " + std::to_string(GetXOffset()) + ", " + std::to_string(GetYOffset());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetEventType() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseScroll";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseScrollEvent::GetCategoryFlags() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseButtonEvent::GetCategoryFlags() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonPressEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseButtonPressEvent: " + Utils::String::ConvertToString<Input::MouseButton>(m_button) +
	       "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetEventType() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonPressEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseButtonPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseButtonReleaseEvent: " + Utils::String::ConvertToString<Input::MouseButton>(m_button) +
	       "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetEventType() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseButtonRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetEventType() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseEnterEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseEnter";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseEnterEvent::GetCategoryFlags() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetEventType() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseLeaveEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "MouseLeave";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseLeaveEvent::GetCategoryFlags() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Mouse | EventCategory::Input;
}