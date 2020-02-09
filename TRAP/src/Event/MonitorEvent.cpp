#include "TRAPPCH.h"
#include "MonitorEvent.h"

TRAP::Monitor TRAP::MonitorEvent::GetMonitor() const
{
	return m_monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::MonitorEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::MonitorEvent::MonitorEvent(const Monitor monitor)
	: m_monitor(monitor)
{
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::MonitorConnectEvent::MonitorConnectEvent(const Monitor monitor)
	: MonitorEvent(monitor)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MonitorConnectEvent::ToString() const
{
	std::stringstream ss;
	ss << "MonitorConnectEvent: " << m_monitor.GetName() << " (" << m_monitor.GetID() << ")";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MonitorConnectEvent::GetStaticType()
{
	return EventType::MonitorConnect;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MonitorConnectEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MonitorConnectEvent::GetName() const
{
	return "MonitorConnect";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::MonitorDisconnectEvent::MonitorDisconnectEvent(const Monitor monitor)
	: MonitorEvent(monitor)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::MonitorDisconnectEvent::ToString() const
{
	std::stringstream ss;
	ss << "MonitorDisconnectEvent: " << m_monitor.GetName() << " (" << m_monitor.GetID() << ")";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MonitorDisconnectEvent::GetStaticType()
{
	return EventType::MonitorDisconnect;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::MonitorDisconnectEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::MonitorDisconnectEvent::GetName() const
{
	return "MonitorDisconnect";
}