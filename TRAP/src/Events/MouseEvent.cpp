#include "TRAPPCH.h"
#include "MouseEvent.h"

std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	return "MouseMoveEvent: " + std::to_string(m_mouseX) + ", " + std::to_string(m_mouseY);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseMoveEvent::GetName() const
{
	return "MouseMove";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseMoveEvent::GetCategoryFlags() const
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	return "MouseScrollEvent: " + std::to_string(GetXOffset()) + ", " + std::to_string(GetYOffset());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::GetName() const
{
	return "MouseScroll";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseScrollEvent::GetCategoryFlags() const
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseButtonEvent::GetCategoryFlags() const
{
	return EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonEvent::MouseButtonToString(const Input::MouseButton button)
{
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
	return "MouseButtonPressEvent: " + MouseButtonToString(m_button) +
	       "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonPressEvent::GetName() const
{
	return "MouseButtonPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	return "MouseButtonReleaseEvent: " + MouseButtonToString(m_button) +
	       "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::GetName() const
{
	return "MouseButtonRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseEnterEvent::GetName() const
{
	return "MouseEnter";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseEnterEvent::GetCategoryFlags() const
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseLeaveEvent::GetName() const
{
	return "MouseLeave";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseLeaveEvent::GetCategoryFlags() const
{
	return EventCategory::Mouse | EventCategory::Input;
}