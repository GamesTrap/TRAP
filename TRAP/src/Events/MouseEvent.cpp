#include "TRAPPCH.h"
#include "MouseEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"

TRAP::Events::MouseMoveEvent::MouseMoveEvent(const float x, const float y, TRAP::Window* window)
	: m_mouseX(x), m_mouseY(y), m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseMoveEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MouseMoveEvent: {}, {}", m_mouseX, m_mouseY);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseScrollEvent::MouseScrollEvent(const float xOffset, const float yOffset,
                                                 TRAP::Window* const window)
	: m_xOffset(xOffset), m_yOffset(yOffset), m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseScrollEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MouseScrollEvent: {}, {}", GetXOffset(), GetYOffset());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseButtonPressEvent::MouseButtonPressEvent(const Input::MouseButton button,
   														   TRAP::Window* const window)
	: MouseButtonEvent(button), m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseButtonPressEvent(): Window is nullptr!");
}

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

TRAP::Events::MouseButtonReleaseEvent::MouseButtonReleaseEvent(const Input::MouseButton button,
															   TRAP::Window* const window)
	: MouseButtonEvent(button), m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseButtonReleaseEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("MouseButtonReleaseEvent: {}({})",
	                   Utils::String::ConvertToString<Input::MouseButton>(m_button), std::to_underlying(m_button));
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseEnterEvent::MouseEnterEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseEnterEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseLeaveEvent::MouseLeaveEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseLeaveEvent(): Window is nullptr!");
}

#endif /*TRAP_HEADLESS_MODE*/