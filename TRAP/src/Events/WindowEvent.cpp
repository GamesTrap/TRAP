#include "TRAPPCH.h"
#include "WindowEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Window/Window.h"

[[nodiscard]] std::string TRAP::Events::WindowResizeEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("WindowResizeEvent: {}x{} (\"{}\")", m_width, m_height, m_window->GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowMoveEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("WindowMoveEvent: {}, {} (\"{}\")", m_x, m_y, m_window->GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowContentScaleEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("WindowContentScaleEvent: {}x{} (\"{}\")", m_XScale, m_YScale, m_window->GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::FrameBufferResizeEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("FrameBufferResizeEvent: {}x{} (\"{}\")", m_width, m_height, m_window->GetTitle());
}

#endif /*TRAP_HEADLESS_MODE*/