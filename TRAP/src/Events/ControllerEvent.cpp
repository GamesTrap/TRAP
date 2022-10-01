#include "TRAPPCH.h"
#include "ControllerEvent.h"

TRAP::Events::EventCategory TRAP::Events::ControllerEvent::GetCategoryFlags() const
{
	ZoneScoped;

	return EventCategory::Controller | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerEvent::ControllerEvent(const Input::Controller controller)
	: m_controller(controller)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerConnectEvent::ControllerConnectEvent(const Input::Controller controller)
	: ControllerEvent(controller)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ControllerConnectEvent::ToString() const
{
	ZoneScoped;

	return "ControllerConnectEvent: " + std::to_string(static_cast<uint32_t>(m_controller) + 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::ControllerConnectEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ControllerConnectEvent::GetName() const
{
	ZoneScoped;

	return "ControllerConnect";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerDisconnectEvent::ControllerDisconnectEvent(const Input::Controller controller)
	: ControllerEvent(controller)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ControllerDisconnectEvent::ToString() const
{
	ZoneScoped;

	return "ControllerDisconnectEvent: " + std::to_string(static_cast<uint32_t>(m_controller) + 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::ControllerDisconnectEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ControllerDisconnectEvent::GetName() const
{
	ZoneScoped;

	return "ControllerDisconnect";
}