#include "TRAPPCH.h"
#include "KeyEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"

TRAP::Events::KeyPressEvent::KeyPressEvent(const Input::Key key, TRAP::Window* window)
	: KeyEvent(key), m_window(window)
{
	TRAP_ASSERT(window, "Events::KeyPressEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyPressEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyPressEvent: {}({})", name, std::to_underlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyRepeatEvent::KeyRepeatEvent(const Input::Key key, TRAP::Window* window)
	: KeyEvent(key), m_window(window)
{
	TRAP_ASSERT(window, "Events::KeyRepeatEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyRepeatEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyRepeatEvent: {}({})", name, std::to_underlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyReleaseEvent::KeyReleaseEvent(const Input::Key key, TRAP::Window* window)
	: KeyEvent(key), m_window(window)
{
	TRAP_ASSERT(window, "Events::KeyReleaseEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyReleaseEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyReleaseEvent: {}({})", name, std::to_underlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyTypeEvent::KeyTypeEvent(const u32 codePoint, TRAP::Window* window)
	: m_window(window), m_codePoint(codePoint)
{
	TRAP_ASSERT(window, "Events::KeyTypeEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyTypeEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("KeyTypeEvent: {}({})", Utils::String::EncodeUTF8(m_codePoint), m_codePoint);
}

#endif /*TRAP_HEADLESS_MODE*/