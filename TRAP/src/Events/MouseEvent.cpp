#include "TRAPPCH.h"
#include "MouseEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"

[[nodiscard]] std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MouseMoveEvent: {}, {}", m_mouseX, m_mouseY);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MouseScrollEvent: {}, {}", GetXOffset(), GetYOffset());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseButtonPressEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MouseButtonPressEvent: {}({})", Utils::String::ConvertToString<Input::MouseButton>(m_button),
	                   std::to_underlying(m_button));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MouseButtonReleaseEvent: {}({})",
	                   Utils::String::ConvertToString<Input::MouseButton>(m_button), std::to_underlying(m_button));
}

#endif /*TRAP_HEADLESS_MODE*/