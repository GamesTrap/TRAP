#include "TRAPPCH.h"
#include "MonitorEvent.h"

TRAP::Monitor TRAP::Events::MonitorEvent::GetMonitor() const
{
	return m_monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::MonitorEvent::GetCategoryFlags() const
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorEvent::MonitorEvent(Monitor monitor)
	: m_monitor(std::move(monitor))
{}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorConnectEvent::MonitorConnectEvent(const Monitor monitor)
	: MonitorEvent(monitor)
{}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorConnectEvent::ToString() const
{
	return "MonitorConnectEvent: " + m_monitor.GetName() + " (" + std::to_string(m_monitor.GetID()) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MonitorConnectEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorConnectEvent::GetName() const
{
	return "MonitorConnect";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorDisconnectEvent::MonitorDisconnectEvent(const Monitor monitor)
	: MonitorEvent(monitor)
{}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorDisconnectEvent::ToString() const
{
	return "MonitorDisconnectEvent: " + m_monitor.GetName() + " (" + std::to_string(m_monitor.GetID()) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MonitorDisconnectEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorDisconnectEvent::GetName() const
{
	return "MonitorDisconnect";
}