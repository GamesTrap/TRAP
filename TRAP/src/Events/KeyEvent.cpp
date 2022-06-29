#include "TRAPPCH.h"
#include "KeyEvent.h"

#include "Window/WindowingAPI.h"

TRAP::Input::Key TRAP::Events::KeyEvent::GetKey() const
{
	return m_key;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::KeyEvent::GetCategoryFlags() const
{
	return EventCategory::Keyboard | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyEvent::KeyEvent(const Input::Key key)
	: m_key(key)
{
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyPressEvent::KeyPressEvent(const Input::Key key, const uint32_t repeatCount, TRAP::Window* window)
	: KeyEvent(key), m_repeatCount(repeatCount), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Events::KeyPressEvent::GetRepeatCount() const
{
	return m_repeatCount;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyPressEvent::ToString() const
{
	std::string name = TRAP::Input::GetKeyName(m_key);

	return "KeyPressEvent: " + name + "(" + std::to_string(static_cast<int32_t>(m_key)) + ") (" +
	       std::to_string(m_repeatCount) + " repeats)";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::KeyPressEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetStaticType()
{
	return EventType::KeyPress;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::KeyPressEvent::GetName() const
{
	return "KeyPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyReleaseEvent::KeyReleaseEvent(const Input::Key key, TRAP::Window* window)
	: KeyEvent(key), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyReleaseEvent::ToString() const
{
	std::string name = TRAP::Input::GetKeyName(m_key);

	return "KeyReleaseEvent: " + name + "(" + std::to_string(static_cast<int32_t>(m_key)) + ")";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::KeyReleaseEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetStaticType()
{
	return EventType::KeyRelease;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::KeyReleaseEvent::GetName() const
{
	return "KeyRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyTypeEvent::KeyTypeEvent(const uint32_t codePoint, TRAP::Window* window)
	: m_window(window), m_codePoint(codePoint)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyTypeEvent::ToString() const
{
	return "KeyTypeEvent: " + EncodeUTF8(m_codePoint) + "(" + std::to_string(m_codePoint) + ")";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::KeyTypeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Events::KeyTypeEvent::GetCodePoint() const
{
	return m_codePoint;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetStaticType()
{
	return EventType::KeyType;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::KeyTypeEvent::GetName() const
{
	return "KeyType";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::KeyTypeEvent::GetCategoryFlags() const
{
	return EventCategory::Keyboard | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyTypeEvent::EncodeUTF8(const uint32_t codePoint)
{
	std::string result{};

	if (codePoint < 0x80)
		result.push_back(static_cast<char>(codePoint));
	else if (codePoint < 0x800)
	{
		result.push_back(static_cast<char>((codePoint >> 6) | 0xC0u));
		result.push_back(static_cast<char>((codePoint & 0x3F) | 0x80));
	}
	else if (codePoint < 0x10000)
	{
		result.push_back(static_cast<char>((codePoint >> 12) | 0xE0u));
		result.push_back(static_cast<char>(((codePoint >> 6) & 0x3F) | 0x80));
		result.push_back(static_cast<char>((codePoint & 0x3F) | 0x80));
	}
	else if (codePoint < 0x110000)
	{
		result.push_back(static_cast<char>((codePoint >> 18) | 0xF0u));
		result.push_back(static_cast<char>(((codePoint >> 12) & 0x3F) | 0x80));
		result.push_back(static_cast<char>(((codePoint >> 6) & 0x3F) | 0x80));
		result.push_back(static_cast<char>((codePoint & 0x3F) | 0x80));
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyLayoutEvent::KeyLayoutEvent(std::string layout)
	: m_layout(std::move(layout))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::KeyLayoutEvent::GetLayout() const
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyLayoutEvent::ToString() const
{
	return "KeyLayoutEvent: " + m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetStaticType()
{
	return EventType::KeyLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::KeyLayoutEvent::GetName() const
{
	return "KeyLayout";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::KeyLayoutEvent::GetCategoryFlags() const
{
	return EventCategory::Keyboard | EventCategory::Input;
}