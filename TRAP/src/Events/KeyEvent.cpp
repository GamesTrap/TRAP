#include "TRAPPCH.h"
#include "KeyEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"
#include "Window/WindowingAPI.h"

[[nodiscard]] std::string TRAP::Events::KeyPressEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyPressEvent: {}({})", name, ToUnderlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyRepeatEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyRepeatEvent: {}({})", name, ToUnderlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyReleaseEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyReleaseEvent: {}({})", name, ToUnderlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyTypeEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("KeyTypeEvent: {}({})", Utils::String::EncodeUTF8(m_codePoint), m_codePoint);
}

#endif /*TRAP_HEADLESS_MODE*/