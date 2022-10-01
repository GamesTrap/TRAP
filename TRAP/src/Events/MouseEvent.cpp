#include "TRAPPCH.h"
#include "MouseEvent.h"

std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseMoveEvent: " + std::to_string(m_mouseX) + ", " + std::to_string(m_mouseY);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseMoveEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseMove";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseMoveEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseScrollEvent: " + std::to_string(GetXOffset()) + ", " + std::to_string(GetYOffset());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseScroll";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseScrollEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseButtonEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonEvent::MouseButtonToString(const Input::MouseButton button)
{
	ZoneScopedC(tracy::Color::Purple);

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
	ZoneScopedC(tracy::Color::Purple);

	return "MouseButtonPressEvent: " + MouseButtonToString(m_button) +
	       "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonPressEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseButtonPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseButtonReleaseEvent: " + MouseButtonToString(m_button) +
	       "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseButtonRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseEnterEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseEnter";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseEnterEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseLeaveEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MouseLeave";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseLeaveEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Mouse | EventCategory::Input;
}