#include "TRAPPCH.h"
#include "ControllerEvent.h"

TRAP::Input::Controller TRAP::ControllerEvent::GetController() const
{
	return m_controller;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::ControllerEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Controller) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ControllerEvent::ControllerEvent(const Input::Controller controller)
	: m_controller(controller)
{	
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::ControllerConnectEvent::ControllerConnectEvent(const Input::Controller controller)
	: ControllerEvent(controller)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::ControllerConnectEvent::ToString() const
{
	std::stringstream ss;
	ss << "ControllerConnectEvent: " << (static_cast<uint32_t>(m_controller) + 1);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::ControllerConnectEvent::GetStaticType()
{
	return EventType::ControllerConnect;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::ControllerConnectEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::ControllerConnectEvent::GetName() const
{
	return "ControllerConnect";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ControllerDisconnectEvent::ControllerDisconnectEvent(const Input::Controller controller)
	: ControllerEvent(controller)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::ControllerDisconnectEvent::ToString() const
{
	std::stringstream ss;
	ss << "ControllerDisconnectEvent: " << (static_cast<uint32_t>(m_controller) + 1);

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::ControllerDisconnectEvent::GetStaticType()
{
	return EventType::ControlledDisconnect;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::ControllerDisconnectEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::ControllerDisconnectEvent::GetName() const
{
	return "ControllerDisconnect";
}