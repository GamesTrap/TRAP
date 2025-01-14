#include "TRAPPCH.h"
#include "KeyEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"
#include "Window/Window.h"

[[nodiscard]] std::string TRAP::Events::KeyPressEvent::ToString() const
{
	const std::string name = TRAP::Input::GetKeyName(GetKey());
	return fmt::format("KeyPressEvent: {}({}) (\"{}\")", name, std::to_underlying(GetKey()), GetWindow().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyRepeatEvent::ToString() const
{
	const std::string name = TRAP::Input::GetKeyName(GetKey());
	return fmt::format("KeyRepeatEvent: {}({}) (\"{}\")", name, std::to_underlying(GetKey()), GetWindow().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyReleaseEvent::ToString() const
{
	const std::string name = TRAP::Input::GetKeyName(GetKey());
	return fmt::format("KeyReleaseEvent: {}({}) (\"{}\")", name, std::to_underlying(GetKey()), GetWindow().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
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
