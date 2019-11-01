#include "TRAPPCH.h"
#include "MouseEvent.h"

TRAP::MouseMovedEvent::MouseMovedEvent(const float x, const float y, const std::string_view title)
	: m_mouseX(x), m_mouseY(y), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::MouseMovedEvent::GetX() const
{
	return m_mouseX;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::MouseMovedEvent::GetY() const
{
	return m_mouseY;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::MouseMovedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MouseMovedEvent::ToString() const
{
	std::stringstream ss;
	ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseMovedEvent::GetStaticType()
{
	return EventType::MouseMoved;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseMovedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MouseMovedEvent::GetName() const
{
	return "MouseMoved";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::MouseMovedEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Mouse) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::MouseScrolledEvent::MouseScrolledEvent(const float xOffset, const float yOffset, const std::string_view title)
	: m_xOffset(xOffset), m_yOffset(yOffset), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::MouseScrolledEvent::GetXOffset() const
{
	return m_xOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::MouseScrolledEvent::GetYOffset() const
{
	return m_yOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::MouseScrolledEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MouseScrolledEvent::ToString() const
{
	std::stringstream ss;
	ss << "MouseScrollEvent: " << GetXOffset() << ", " << GetYOffset();

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseScrolledEvent::GetStaticType()
{
	return EventType::MouseScrolled;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseScrolledEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MouseScrolledEvent::GetName() const
{
	return "MouseScrolled";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::MouseScrolledEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Mouse) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::MouseButton TRAP::MouseButtonEvent::GetMouseButton() const
{
	return m_button;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::MouseButtonEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Mouse) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::MouseButtonEvent::MouseButtonEvent(const Input::MouseButton button)
	: m_button(button)
{
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::MouseButtonPressedEvent::MouseButtonPressedEvent(const Input::MouseButton button, const std::string_view title)
	: MouseButtonEvent(button), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MouseButtonPressedEvent::ToString() const
{
	std::stringstream ss;
	ss << "MouseButtonPressedEvent: " << static_cast<int32_t>(m_button);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::MouseButtonPressedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseButtonPressedEvent::GetStaticType()
{
	return EventType::MouseButtonPressed;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseButtonPressedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MouseButtonPressedEvent::GetName() const
{
	return "MouseButtonPressed";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::MouseButtonReleasedEvent::MouseButtonReleasedEvent(const Input::MouseButton button, const std::string_view title)
	: MouseButtonEvent(button), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MouseButtonReleasedEvent::ToString() const
{
	std::stringstream ss;
	ss << "MouseButtonReleasedEvent: " << static_cast<int32_t>(m_button);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::MouseButtonReleasedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseButtonReleasedEvent::GetStaticType()
{
	return EventType::MouseButtonReleased;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseButtonReleasedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MouseButtonReleasedEvent::GetName() const
{
	return "MouseButtonReleased";
}