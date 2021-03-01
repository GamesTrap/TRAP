#include "TRAPPCH.h"
#include "MouseEvent.h"

TRAP::Events::MouseMoveEvent::MouseMoveEvent(const float x, const float y, TRAP::Window* window)
	: m_mouseX(x), m_mouseY(y), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Events::MouseMoveEvent::GetX() const
{
	return m_mouseX;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Events::MouseMoveEvent::GetY() const
{
	return m_mouseY;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Events::MouseMoveEvent::GetPosition() const
{
	return { m_mouseX, m_mouseY };
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::MouseMoveEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	return "MouseMoveEvent: " + std::to_string(m_mouseX) + ", " + std::to_string(m_mouseY);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetStaticType()
{
	return EventType::MouseMove;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::MouseMoveEvent::GetName() const
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

TRAP::Events::MouseScrollEvent::MouseScrollEvent(const float xOffset, const float yOffset, TRAP::Window* window)
	: m_xOffset(xOffset), m_yOffset(yOffset), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Events::MouseScrollEvent::GetXOffset() const
{
	return m_xOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Events::MouseScrollEvent::GetYOffset() const
{
	return m_yOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Events::MouseScrollEvent::GetOffset() const
{
	return { m_xOffset, m_yOffset };
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::MouseScrollEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	return "MouseScrollEvent: " + std::to_string(GetXOffset()) + ", " + std::to_string(GetYOffset());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetStaticType()
{
	return EventType::MouseScroll;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::MouseScrollEvent::GetName() const
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

TRAP::Input::MouseButton TRAP::Events::MouseButtonEvent::GetMouseButton() const
{
	return m_button;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseButtonEvent::GetCategoryFlags() const
{
	return EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseButtonEvent::MouseButtonEvent(const Input::MouseButton button)
	: m_button(button)
{
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

	case Input::MouseButton::Four:
		return "4";

	case Input::MouseButton::Five:
		return "5";

	case Input::MouseButton::Six:
		return "6";

	case Input::MouseButton::Seven:
		return "7";

	case Input::MouseButton::Eight:
		return "8";

	default:
		return "";
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseButtonPressEvent::MouseButtonPressEvent(const Input::MouseButton button, TRAP::Window* window)
	: MouseButtonEvent(button), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonPressEvent::ToString() const
{
	return "MouseButtonPressEvent: " + MouseButtonToString(m_button) + "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::MouseButtonPressEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetStaticType()
{
	return EventType::MouseButtonPress;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::MouseButtonPressEvent::GetName() const
{
	return "MouseButtonPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseButtonReleaseEvent::MouseButtonReleaseEvent(const Input::MouseButton button, TRAP::Window* window)
	: MouseButtonEvent(button), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	return "MouseButtonReleaseEvent: " + MouseButtonToString(m_button) + "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::MouseButtonReleaseEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetStaticType()
{
	return EventType::MouseButtonRelease;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::MouseButtonReleaseEvent::GetName() const
{
	return "MouseButtonRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseEnterEvent::MouseEnterEvent(TRAP::Window* window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::MouseEnterEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetStaticType()
{
	return EventType::MouseEnter;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::MouseEnterEvent::GetName() const
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

TRAP::Events::MouseLeaveEvent::MouseLeaveEvent(TRAP::Window* window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::MouseLeaveEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetStaticType()
{
	return EventType::MouseLeave;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::MouseLeaveEvent::GetName() const
{
	return "MouseLeave";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MouseLeaveEvent::GetCategoryFlags() const
{
	return EventCategory::Mouse | EventCategory::Input;
}