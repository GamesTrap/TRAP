#include "TRAPPCH.h"
#include "MouseEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"

TRAP::Events::MouseMoveEvent::MouseMoveEvent(const f32 x, const f32 y, TRAP::Window* window)
	: m_mouseX(x), m_mouseY(y), m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseMoveEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseMoveEvent::ToString() const
{
	return fmt::format("MouseMoveEvent: {}, {} (\"{}\")", m_mouseX, m_mouseY, m_window->GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseScrollEvent::MouseScrollEvent(const f32 xOffset, const f32 yOffset,
                                                 TRAP::Window* const window)
	: m_xOffset(xOffset), m_yOffset(yOffset), m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseScrollEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseScrollEvent::ToString() const
{
	return fmt::format("MouseScrollEvent: {}, {} (\"{}\")", m_xOffset, m_yOffset, m_window->GetTitle());
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
	return fmt::format("MouseButtonPressEvent: {}({}) (\"{}\")", m_button, std::to_underlying(m_button),
	                   m_window->GetTitle());
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
	return fmt::format("MouseButtonReleaseEvent: {}({}) (\"{}\")", m_button, std::to_underlying(m_button),
	                   m_window->GetTitle());
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

[[nodiscard]] std::string TRAP::Events::MouseEnterEvent::ToString() const
{
	return fmt::format("MouseEnterEvent (\"{}\")", m_window->GetTitle());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::MouseLeaveEvent::MouseLeaveEvent(TRAP::Window* const window)
	: m_window(window)
{
	TRAP_ASSERT(window, "Events::MouseLeaveEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::MouseLeaveEvent::ToString() const
{
	return fmt::format("MouseLeaveEvent (\"{}\")", m_window->GetTitle());
}

#endif /*TRAP_HEADLESS_MODE*/