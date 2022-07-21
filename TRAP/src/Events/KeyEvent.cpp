#include "TRAPPCH.h"
#include "KeyEvent.h"

#include "Window/WindowingAPI.h"

TRAP::Events::EventCategory TRAP::Events::KeyEvent::GetCategoryFlags() const
{
	return EventCategory::Keyboard | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyPressEvent::ToString() const
{
	const std::string name = TRAP::Input::GetKeyName(m_key);

	return "KeyPressEvent: " + name + "(" + std::to_string(static_cast<int32_t>(m_key)) + ") (" +
	       std::to_string(m_repeatCount) + " repeats)";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyPressEvent::GetName() const
{
	return "KeyPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyReleaseEvent::ToString() const
{
	const std::string name = TRAP::Input::GetKeyName(m_key);

	return "KeyReleaseEvent: " + name + "(" + std::to_string(static_cast<int32_t>(m_key)) + ")";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyReleaseEvent::GetName() const
{
	return "KeyRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyTypeEvent::ToString() const
{
	return "KeyTypeEvent: " + EncodeUTF8(m_codePoint) + "(" + std::to_string(m_codePoint) + ")";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyTypeEvent::GetName() const
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
	result.reserve(4);

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
{}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyLayoutEvent::GetLayout() const
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyLayoutEvent::ToString() const
{
	return "KeyLayoutEvent: " + m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyLayoutEvent::GetName() const
{
	return "KeyLayout";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::KeyLayoutEvent::GetCategoryFlags() const
{
	return EventCategory::Keyboard | EventCategory::Input;
}