#include "TRAPPCH.h"
#include "MonitorEvent.h"

TRAP::Monitor TRAP::Events::MonitorEvent::GetMonitor() const
{
	return m_monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::MonitorEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorEvent::MonitorEvent(const Monitor monitor)
	: m_monitor(monitor)
{
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorConnectEvent::MonitorConnectEvent(const Monitor monitor)
	: MonitorEvent(monitor)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorConnectEvent::ToString() const
{
	return "MonitorConnectEvent: " + m_monitor.GetName() + " (" + std::to_string(m_monitor.GetID()) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MonitorConnectEvent::GetStaticType()
{
	return EventType::MonitorConnect;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MonitorConnectEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::MonitorConnectEvent::GetName() const
{
	return "MonitorConnect";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MonitorDisconnectEvent::MonitorDisconnectEvent(const Monitor monitor)
	: MonitorEvent(monitor)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::MonitorDisconnectEvent::ToString() const
{
	return "MonitorDisconnectEvent: " + m_monitor.GetName() + " (" + std::to_string(m_monitor.GetID()) + ')';
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MonitorDisconnectEvent::GetStaticType()
{
	return EventType::MonitorDisconnect;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::MonitorDisconnectEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::MonitorDisconnectEvent::GetName() const
{
	return "MonitorDisconnect";
}