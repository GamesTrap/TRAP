#include "TRAPPCH.h"
#include "MouseEvent.h"

TRAP::Events::MouseMoveEvent::MouseMoveEvent(const float x, const float y, std::string title)
	: m_mouseX(x), m_mouseY(y), m_title(std::move(title))
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

const std::string& TRAP::Events::MouseMoveEvent::GetTitle() const
{
	return m_title;
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

TRAP::Events::MouseScrollEvent::MouseScrollEvent(const float xOffset, const float yOffset, std::string title)
	: m_xOffset(xOffset), m_yOffset(yOffset), m_title(std::move(title))
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

const std::string& TRAP::Events::MouseScrollEvent::GetTitle() const
{
	return m_title;
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
	return EventCategory::Mouse | EventCategory::Input;
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

TRAP::Events::MouseButtonPressEvent::MouseButtonPressEvent(const Input::MouseButton button, std::string title)
	: MouseButtonEvent(button), m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonPressEvent::ToString() const
{
	return "MouseButtonPressEvent: " + MouseButtonToString(m_button) + "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::MouseButtonPressEvent::GetTitle() const
{
	return m_title;
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

TRAP::Events::MouseButtonReleaseEvent::MouseButtonReleaseEvent(const Input::MouseButton button, std::string title)
	: MouseButtonEvent(button), m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	return "MouseButtonReleaseEvent: " + MouseButtonToString(m_button) + "(" + std::to_string(static_cast<int32_t>(m_button)) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::MouseButtonReleaseEvent::GetTitle() const
{
	return m_title;
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

TRAP::Events::MouseEnterEvent::MouseEnterEvent(std::string title)
	: m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::MouseEnterEvent::GetTitle() const
{
	return m_title;
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

TRAP::Events::MouseLeaveEvent::MouseLeaveEvent(std::string title)
	: m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::MouseLeaveEvent::GetTitle() const
{
	return m_title;
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