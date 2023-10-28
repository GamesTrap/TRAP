#include "TRAPPCH.h"
#include "WindowEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Window/Window.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowResizeEvent::WindowResizeEvent(const u32 width, const u32 height,
   												   TRAP::Window* const window)
	: m_width(width), m_height(height), m_window(window)
{
	TRAP_ASSERT(window, "Events::WindowResizeEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowResizeEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("WindowResizeEvent: {}x{} (\"{}\")", m_width, m_height, m_window->GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMinimizeEvent::WindowMinimizeEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::WindowMinimizeEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMaximizeEvent::WindowMaximizeEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::WindowMaximizeEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowRestoreEvent::WindowRestoreEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::WindowRestoreEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMoveEvent::WindowMoveEvent(const i32 x, const i32 y, TRAP::Window* const window)
	: m_x(x), m_y(y), m_window(window)
{
	TRAP_ASSERT(window, "Events::WindowMoveEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowMoveEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("WindowMoveEvent: {}, {} (\"{}\")", m_x, m_y, m_window->GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowFocusEvent::WindowFocusEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::WindowFocusEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowLostFocusEvent::WindowLostFocusEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::WindowLostFocusEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowDropEvent::WindowDropEvent(std::vector<std::string> paths, TRAP::Window* const window)
	: m_paths(std::move(paths)), m_window(window)
{
	TRAP_ASSERT(m_window, "Events::WindowDropEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowCloseEvent::WindowCloseEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::WindowCloseEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowContentScaleEvent::WindowContentScaleEvent(const f32 xScale, const f32 yScale,
	                                                           TRAP::Window* const window)
	: m_XScale(xScale), m_YScale(yScale), m_window(window)
{
	TRAP_ASSERT(m_window, "Events::WindowContentScaleEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowContentScaleEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("WindowContentScaleEvent: {}x{} (\"{}\")", m_XScale, m_YScale, m_window->GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::FrameBufferResizeEvent::FrameBufferResizeEvent(const u32 width, const u32 height,
                                                             TRAP::Window* const window)
	: m_width(width), m_height(height), m_window(window)
{
	TRAP_ASSERT(m_window, "Events::FrameBufferResizeEvent(): Window is nullptr!");
	TRAP_ASSERT(m_width > 0, "Events::FrameBufferResizeEvent(): Width is 0!");
	TRAP_ASSERT(m_height > 0, "Events::FrameBufferResizeEvent(): Height is 0!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::FrameBufferResizeEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("FrameBufferResizeEvent: {}x{} (\"{}\")", m_width, m_height, m_window->GetTitle());
}

#endif /*TRAP_HEADLESS_MODE*/