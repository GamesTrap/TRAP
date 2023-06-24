#include "TRAPPCH.h"
#include "MonitorEvent.h"

#ifndef TRAP_HEADLESS_MODE

[[nodiscard]] TRAP::Monitor TRAP::Events::MonitorEvent::GetMonitor() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_monitor;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MonitorConnectEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MonitorConnectEvent: {} ({})", m_monitor.GetName(), m_monitor.GetID());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MonitorDisconnectEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MonitorDisconnectEvent: {} ({})", m_monitor.GetName(), m_monitor.GetID());
}

#endif /*TRAP_HEADLESS_MODE*/