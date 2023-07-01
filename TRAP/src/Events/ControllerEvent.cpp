#include "TRAPPCH.h"
#include "ControllerEvent.h"

#ifndef TRAP_HEADLESS_MODE

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ControllerConnectEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("ControllerConnectEvent: {}", (std::to_underlying(m_controller) + 1));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ControllerDisconnectEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("ControllerDisconnectEvent: {}", (std::to_underlying(m_controller) + 1u));
}

#endif /*TRAP_HEADLESS_MODE*/