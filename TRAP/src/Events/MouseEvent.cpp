#include "TRAPPCH.h"
#include "MouseEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"

TRAP::Events::MouseMoveEvent::MouseMoveEvent(const f32 x, const f32 y, TRAP::Window& window)
	: m_mouseX(x), m_mouseY(y), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	return fmt::format("MouseMoveEvent: {}, {} (\"{}\")", m_mouseX, m_mouseY, m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseScrollEvent::MouseScrollEvent(const f32 xOffset, const f32 yOffset, TRAP::Window& window)
	: m_xOffset(xOffset), m_yOffset(yOffset), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	return fmt::format("MouseScrollEvent: {}, {} (\"{}\")", m_xOffset, m_yOffset, m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseButtonPressEvent::MouseButtonPressEvent(const Input::MouseButton button, TRAP::Window& window)
	: MouseButtonEvent(button), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseButtonPressEvent::ToString() const
{
	return fmt::format("MouseButtonPressEvent: {}({}) (\"{}\")", GetMouseButton(), std::to_underlying(GetMouseButton()),
	                   m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseButtonReleaseEvent::MouseButtonReleaseEvent(const Input::MouseButton button, TRAP::Window& window)
	: MouseButtonEvent(button), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseButtonReleaseEvent::ToString() const
{
	return fmt::format("MouseButtonReleaseEvent: {}({}) (\"{}\")", GetMouseButton(), std::to_underlying(GetMouseButton()),
	                   m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseEnterEvent::MouseEnterEvent(TRAP::Window& window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseEnterEvent::ToString() const
{
	return fmt::format("MouseEnterEvent (\"{}\")", m_window.get().GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseLeaveEvent::MouseLeaveEvent(TRAP::Window& window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseLeaveEvent::ToString() const
{
	return fmt::format("MouseLeaveEvent (\"{}\")", m_window.get().GetTitle());
}

#endif /*TRAP_HEADLESS_MODE*/
