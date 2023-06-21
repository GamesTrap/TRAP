#include "TRAPPCH.h"
#include "ControllerEvent.h"

#ifndef TRAP_HEADLESS_MODE

[[nodiscard]] TRAP::Events::EventCategory TRAP::Events::ControllerEvent::GetCategoryFlags() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Controller | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerEvent::ControllerEvent(const Input::Controller controller) noexcept
	: m_controller(controller)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerConnectEvent::ControllerConnectEvent(const Input::Controller controller) noexcept
	: ControllerEvent(controller)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ControllerConnectEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("ControllerConnectEvent: {}", (ToUnderlying(m_controller) + 1));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventType TRAP::Events::ControllerConnectEvent::GetEventType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ControllerConnectEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "ControllerConnect";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ControllerDisconnectEvent::ControllerDisconnectEvent(const Input::Controller controller) noexcept
	: ControllerEvent(controller)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ControllerDisconnectEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("ControllerDisconnectEvent: {}", (ToUnderlying(m_controller) + 1u));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventType TRAP::Events::ControllerDisconnectEvent::GetEventType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ControllerDisconnectEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "ControllerDisconnect";
}

#endif /*TRAP_HEADLESS_MODE*/