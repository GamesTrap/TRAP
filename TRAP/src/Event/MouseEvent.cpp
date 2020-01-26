#include "TRAPPCH.h"
#include "MouseEvent.h"

TRAP::MouseMoveEvent::MouseMoveEvent(const float x, const float y, const std::string_view title)
	: m_mouseX(x), m_mouseY(y), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::MouseMoveEvent::GetX() const
{
	return m_mouseX;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::MouseMoveEvent::GetY() const
{
	return m_mouseY;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::MouseMoveEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MouseMoveEvent::ToString() const
{
	std::stringstream ss;
	ss << "MouseMoveEvent: " << m_mouseX << ", " << m_mouseY;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseMoveEvent::GetStaticType()
{
	return EventType::MouseMove;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseMoveEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MouseMoveEvent::GetName() const
{
	return "MouseMove";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::MouseMoveEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Mouse) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::MouseScrollEvent::MouseScrollEvent(const float xOffset, const float yOffset, const std::string_view title)
	: m_xOffset(xOffset), m_yOffset(yOffset), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::MouseScrollEvent::GetXOffset() const
{
	return m_xOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::MouseScrollEvent::GetYOffset() const
{
	return m_yOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::MouseScrollEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MouseScrollEvent::ToString() const
{
	std::stringstream ss;
	ss << "MouseScrollEvent: " << GetXOffset() << ", " << GetYOffset();

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseScrollEvent::GetStaticType()
{
	return EventType::MouseScroll;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseScrollEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MouseScrollEvent::GetName() const
{
	return "MouseScroll";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::MouseScrollEvent::GetCategoryFlags() const
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

TRAP::MouseButtonPressEvent::MouseButtonPressEvent(const Input::MouseButton button, const std::string_view title)
	: MouseButtonEvent(button), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MouseButtonPressEvent::ToString() const
{
	std::stringstream ss;
	ss << "MouseButtonPressEvent: " << static_cast<int32_t>(m_button);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::MouseButtonPressEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseButtonPressEvent::GetStaticType()
{
	return EventType::MouseButtonPress;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseButtonPressEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MouseButtonPressEvent::GetName() const
{
	return "MouseButtonPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::MouseButtonReleaseEvent::MouseButtonReleaseEvent(const Input::MouseButton button, const std::string_view title)
	: MouseButtonEvent(button), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MouseButtonReleaseEvent::ToString() const
{
	std::stringstream ss;
	ss << "MouseButtonReleaseEvent: " << static_cast<int32_t>(m_button);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::MouseButtonReleaseEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseButtonReleaseEvent::GetStaticType()
{
	return EventType::MouseButtonRelease;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MouseButtonReleaseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MouseButtonReleaseEvent::GetName() const
{
	return "MouseButtonRelease";
}