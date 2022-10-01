#include "TRAPPCH.h"
#include "ControllerEvent.h"

TRAP::Events::EventCategory TRAP::Events::ControllerEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Controller | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerEvent::ControllerEvent(const Input::Controller controller)
	: m_controller(controller)
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerConnectEvent::ControllerConnectEvent(const Input::Controller controller)
	: ControllerEvent(controller)
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ControllerConnectEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "ControllerConnectEvent: " + std::to_string(static_cast<uint32_t>(m_controller) + 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::ControllerConnectEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ControllerConnectEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "ControllerConnect";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerDisconnectEvent::ControllerDisconnectEvent(const Input::Controller controller)
	: ControllerEvent(controller)
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ControllerDisconnectEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "ControllerDisconnectEvent: " + std::to_string(static_cast<uint32_t>(m_controller) + 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::ControllerDisconnectEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ControllerDisconnectEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "ControllerDisconnect";
}