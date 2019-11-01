#include "TRAPPCH.h"
#include "KeyEvent.h"

TRAP::Input::Key TRAP::KeyEvent::GetKeyCode() const
{
	return m_keyCode;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::KeyEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Keyboard) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::KeyEvent::KeyEvent(const Input::Key keyCode)
	: m_keyCode(keyCode)
{
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::KeyPressedEvent::KeyPressedEvent(const Input::Key keyCode, const int32_t repeatCount, const std::string_view title)
	: KeyEvent(keyCode), m_repeatCount(repeatCount), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::KeyPressedEvent::GetRepeatCount() const
{
	return m_repeatCount;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::KeyPressedEvent::ToString() const
{
	std::stringstream ss;
	ss << "KeyPressedEvent: " << static_cast<int32_t>(m_keyCode) << " (" << m_repeatCount << " repeats)";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::KeyPressedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyPressedEvent::GetStaticType()
{
	return EventType::KeyPressed;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyPressedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::KeyPressedEvent::GetName() const
{
	return "KeyPressed";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::KeyReleasedEvent::KeyReleasedEvent(const Input::Key keyCode, const std::string_view title)
	: KeyEvent(keyCode), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::KeyReleasedEvent::ToString() const
{
	std::stringstream ss;
	ss << "KeyReleasedEvent: " << static_cast<int32_t>(m_keyCode);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::KeyReleasedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyReleasedEvent::GetStaticType()
{
	return EventType::KeyReleased;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyReleasedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::KeyReleasedEvent::GetName() const
{
	return "KeyReleased";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::KeyTypedEvent::KeyTypedEvent(const Input::Key keyCode, const std::string_view title)
	: KeyEvent(keyCode), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::KeyTypedEvent::ToString() const
{
	std::stringstream ss;
	ss << "KeyTypedEvent: " << static_cast<int32_t>(m_keyCode);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::KeyTypedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyTypedEvent::GetStaticType()
{
	return EventType::KeyTyped;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyTypedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::KeyTypedEvent::GetName() const
{
	return "KeyTyped";
}