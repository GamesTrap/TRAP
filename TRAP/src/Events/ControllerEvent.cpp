#include "TRAPPCH.h"
#include "ControllerEvent.h"

#ifndef TRAP_HEADLESS_MODE

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ControllerConnectEvent::ToString() const
{
	return fmt::format("ControllerConnectEvent: {}", (std::to_underlying(m_controller) + 1));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ControllerDisconnectEvent::ToString() const
{
	return fmt::format("ControllerDisconnectEvent: {}", (std::to_underlying(m_controller) + 1));
}

#endif /*TRAP_HEADLESS_MODE*/