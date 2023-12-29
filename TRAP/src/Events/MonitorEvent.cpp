#include "TRAPPCH.h"
#include "MonitorEvent.h"

#ifndef TRAP_HEADLESS_MODE

[[nodiscard]] TRAP::Monitor TRAP::Events::MonitorEvent::GetMonitor() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (GetTRAPProfileSystems() & ProfileSystems::Events) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_monitor;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MonitorConnectEvent::ToString() const
{
	return fmt::format("MonitorConnectEvent: {} ({})", m_monitor.GetName(), m_monitor.GetID());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MonitorDisconnectEvent::ToString() const
{
	return fmt::format("MonitorDisconnectEvent: {} ({})", m_monitor.GetName(), m_monitor.GetID());
}

#endif /*TRAP_HEADLESS_MODE*/