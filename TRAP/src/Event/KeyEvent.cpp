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

TRAP::KeyPressEvent::KeyPressEvent(const Input::Key keyCode, const int32_t repeatCount, const std::string_view title)
	: KeyEvent(keyCode), m_repeatCount(repeatCount), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::KeyPressEvent::GetRepeatCount() const
{
	return m_repeatCount;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::KeyPressEvent::ToString() const
{
	std::stringstream ss;
	ss << "KeyPressEvent: " << static_cast<int32_t>(m_keyCode) << " (" << m_repeatCount << " repeats)";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::KeyPressEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyPressEvent::GetStaticType()
{
	return EventType::KeyPress;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyPressEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::KeyPressEvent::GetName() const
{
	return "KeyPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::KeyReleaseEvent::KeyReleaseEvent(const Input::Key keyCode, const std::string_view title)
	: KeyEvent(keyCode), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::KeyReleaseEvent::ToString() const
{
	std::stringstream ss;
	ss << "KeyReleaseEvent: " << static_cast<int32_t>(m_keyCode);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::KeyReleaseEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyReleaseEvent::GetStaticType()
{
	return EventType::KeyRelease;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyReleaseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::KeyReleaseEvent::GetName() const
{
	return "KeyRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::KeyTypeEvent::KeyTypeEvent(const Input::Key keyCode, const std::string_view title)
	: KeyEvent(keyCode), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::KeyTypeEvent::ToString() const
{
	std::stringstream ss;
	ss << "KeyTypeEvent: " << static_cast<int32_t>(m_keyCode);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::KeyTypeEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyTypeEvent::GetStaticType()
{
	return EventType::KeyType;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::KeyTypeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::KeyTypeEvent::GetName() const
{
	return "KeyType";
}