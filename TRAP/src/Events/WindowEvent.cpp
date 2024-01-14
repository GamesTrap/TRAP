#include "TRAPPCH.h"
#include "WindowEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Window/Window.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowResizeEvent::WindowResizeEvent(const u32 width, const u32 height, TRAP::Window& window)
	: m_width(width), m_height(height), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowResizeEvent::ToString() const
{
	return fmt::format("WindowResizeEvent: {}x{} (\"{}\")", m_width, m_height, m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMinimizeEvent::WindowMinimizeEvent(TRAP::Window& window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowMinimizeEvent::ToString() const
{
	return fmt::format("WindowMinimizeEvent (\"{}\")", m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMaximizeEvent::WindowMaximizeEvent(TRAP::Window& window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowMaximizeEvent::ToString() const
{
	return fmt::format("WindowMaximizeEvent (\"{}\")", m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowRestoreEvent::WindowRestoreEvent(TRAP::Window& window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowRestoreEvent::ToString() const
{
	return fmt::format("WindowRestoreEvent (\"{}\")", m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMoveEvent::WindowMoveEvent(const i32 x, const i32 y, TRAP::Window& window)
	: m_x(x), m_y(y), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowMoveEvent::ToString() const
{
	return fmt::format("WindowMoveEvent: {}, {} (\"{}\")", m_x, m_y, m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowFocusEvent::WindowFocusEvent(TRAP::Window& window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowFocusEvent::ToString() const
{
	return fmt::format("WindowMoveEvent (\"{}\")", m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowLostFocusEvent::WindowLostFocusEvent(TRAP::Window& window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowLostFocusEvent::ToString() const
{
	return fmt::format("WindowLostFocusEvent (\"{}\")", m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowDropEvent::WindowDropEvent(std::vector<std::string> paths, TRAP::Window& window)
	: m_paths(std::move(paths)), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowDropEvent::ToString() const
{
	std::string out = fmt::format("WindowDropEvent: ");

	if(!m_paths.empty())
		out += fmt::format("Path(s): \"{}\" ", fmt::join(m_paths, "\", \""));

	out += fmt::format("(\"{}\")", m_window.get().GetTitle());

	return out;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowCloseEvent::WindowCloseEvent(TRAP::Window& window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowCloseEvent::ToString() const
{
	return fmt::format("WindowCloseEvent (\"{}\")", m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowContentScaleEvent::WindowContentScaleEvent(const f32 xScale, const f32 yScale, TRAP::Window& window)
	: m_XScale(xScale), m_YScale(yScale), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::WindowContentScaleEvent::ToString() const
{
	return fmt::format("WindowContentScaleEvent: {}x{} (\"{}\")", m_XScale, m_YScale, m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::FrameBufferResizeEvent::FrameBufferResizeEvent(const u32 width, const u32 height, TRAP::Window& window)
	: m_width(width), m_height(height), m_window(window)
{
	TRAP_ASSERT(m_width > 0, "Events::FrameBufferResizeEvent(): Width is 0!");
	TRAP_ASSERT(m_height > 0, "Events::FrameBufferResizeEvent(): Height is 0!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::FrameBufferResizeEvent::ToString() const
{
	return fmt::format("FrameBufferResizeEvent: {}x{} (\"{}\")", m_width, m_height, m_window.get().GetTitle());
}

#endif /*TRAP_HEADLESS_MODE*/
