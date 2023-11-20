#include "TRAPPCH.h"
#include "KeyEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"

TRAP::Events::KeyPressEvent::KeyPressEvent(const Input::Key key, TRAP::Window& window)
	: KeyEvent(key), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyPressEvent::ToString() const
{
	const std::string name = TRAP::Input::GetKeyName(m_key);
	return fmt::format("KeyPressEvent: {}({}) (\"{}\")", name, std::to_underlying(m_key), m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyRepeatEvent::KeyRepeatEvent(const Input::Key key, TRAP::Window& window)
	: KeyEvent(key), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyRepeatEvent::ToString() const
{
	const std::string name = TRAP::Input::GetKeyName(m_key);
	return fmt::format("KeyRepeatEvent: {}({}) (\"{}\")", name, std::to_underlying(m_key), m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyReleaseEvent::KeyReleaseEvent(const Input::Key key, TRAP::Window& window)
	: KeyEvent(key), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyReleaseEvent::ToString() const
{
	const std::string name = TRAP::Input::GetKeyName(m_key);
	return fmt::format("KeyReleaseEvent: {}({}) (\"{}\")", name, std::to_underlying(m_key), m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyTypeEvent::KeyTypeEvent(const u32 codePoint, TRAP::Window& window)
	: m_window(window), m_codePoint(codePoint)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyTypeEvent::ToString() const
{
	return fmt::format("KeyTypeEvent: {}({}) (\"{}\")", TRAP::Utils::String::EncodeUTF8(m_codePoint), m_codePoint,
	                   m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyLayoutEvent::ToString() const
{
	return fmt::format("KeyLayoutEvent: {}", m_layout);
}

#endif /*TRAP_HEADLESS_MODE*/