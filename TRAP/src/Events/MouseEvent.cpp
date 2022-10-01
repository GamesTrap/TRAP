#include "TRAPPCH.h"
#include "MouseEvent.h"

std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	ZoneScoped;

	return "MouseMoveEvent: " + std::to_string(m_mouseX) + ", " + std::to_string(m_mouseY);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseMoveEvent::GetName() const
{
	ZoneScoped;

	return "MouseMove";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseMoveEvent::GetCategoryFlags() const
{
	ZoneScoped;

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	ZoneScoped;

	return "MouseScrollEvent: " + std::to_string(GetXOffset()) + ", " + std::to_string(GetYOffset());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::GetName() const
{
	ZoneScoped;

	return "MouseScroll";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseScrollEvent::GetCategoryFlags() const
{
	ZoneScoped;

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseButtonEvent::GetCategoryFlags() const
{
	ZoneScoped;

	return EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonEvent::MouseButtonToString(const Input::MouseButton button)
{
	ZoneScoped;

	switch(button)
	{
	case Input::MouseButton::One:
		return "Left";

	case Input::MouseButton::Two:
		return "Right";

	case Input::MouseButton::Three:
		return "Middle";

	default: //For every other value just return its numerical value as string
		return std::to_string(static_cast<uint32_t>(button) + 1);
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonPressEvent::ToString() const
{
	ZoneScoped;

	return "MouseButtonPressEvent: " + MouseButtonToString(m_button) +
	       "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonPressEvent::GetName() const
{
	ZoneScoped;

	return "MouseButtonPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	ZoneScoped;

	return "MouseButtonReleaseEvent: " + MouseButtonToString(m_button) +
	       "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::GetName() const
{
	ZoneScoped;

	return "MouseButtonRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseEnterEvent::GetName() const
{
	ZoneScoped;

	return "MouseEnter";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseEnterEvent::GetCategoryFlags() const
{
	ZoneScoped;

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseLeaveEvent::GetName() const
{
	ZoneScoped;

	return "MouseLeave";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseLeaveEvent::GetCategoryFlags() const
{
	ZoneScoped;

	return EventCategory::Mouse | EventCategory::Input;
}