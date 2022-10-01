#include "TRAPPCH.h"
#include "MonitorEvent.h"

TRAP::Monitor TRAP::Events::MonitorEvent::GetMonitor() const
{
	ZoneScopedC(tracy::Color::Purple);

	return m_monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MonitorEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorEvent::MonitorEvent(Monitor monitor)
	: m_monitor(std::move(monitor))
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorConnectEvent::MonitorConnectEvent(const Monitor monitor)
	: MonitorEvent(monitor)
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorConnectEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MonitorConnectEvent: " + m_monitor.GetName() + " (" + std::to_string(m_monitor.GetID()) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MonitorConnectEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorConnectEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MonitorConnect";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorDisconnectEvent::MonitorDisconnectEvent(const Monitor monitor)
	: MonitorEvent(monitor)
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorDisconnectEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MonitorDisconnectEvent: " + m_monitor.GetName() + " (" + std::to_string(m_monitor.GetID()) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MonitorDisconnectEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorDisconnectEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "MonitorDisconnect";
}