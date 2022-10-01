#include "TRAPPCH.h"
#include "WindowEvent.h"

#include "Window/Window.h"

std::string TRAP::Events::WindowResizeEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowResizeEvent: " + std::to_string(m_width) + "x" + std::to_string(m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowResizeEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowResizeEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowResize";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowResizeEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMinimizeEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowMinimizeEvent";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMinimizeEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMinimizeEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowMinimize";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowMinimizeEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMaximizeEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowMaximizeEvent";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMaximizeEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMaximizeEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowMaximize";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowMaximizeEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowRestoreEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowRestoreEvent";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowRestoreEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowRestoreEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowRestore";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowRestoreEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowCloseEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowCloseEvent: " + m_window->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowCloseEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowCloseEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowClose";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowCloseEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMoveEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowMoveEvent: " + std::to_string(m_x) + ", " + std::to_string(m_y);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMoveEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMoveEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowMove";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowMoveEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowFocusEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowFocusEvent: " + m_window->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowFocusEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowFocusEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowFocus";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowFocusEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowLostFocusEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowLostFocusEvent: " + m_window->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowLostFocusEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowLostFocusEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowLostFocus";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowLostFocusEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowDropEvent::WindowDropEvent(std::vector<std::string> paths, TRAP::Window* window)
	: m_paths(std::move(paths)), m_window(window)
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Events::WindowDropEvent::GetPaths() const
{
	ZoneScopedC(tracy::Color::Purple);

	return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowDropEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	std::string result = "WindowDropEvent: ";
	for (uint32_t i = 0; i < m_paths.size(); i++)
		result += "Path " + std::to_string(i) + ": " + m_paths[i] + '\n';
	result.pop_back();
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowDropEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowDropEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowDrop";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowDropEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowContentScaleEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowContentScaleEvent: " + std::to_string(m_XScale) + "x" + std::to_string(m_YScale);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowContentScaleEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowContentScaleEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "WindowContentScale";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowContentScaleEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::FrameBufferResizeEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "FrameBufferResizeEvent: " + std::to_string(m_width) + "x" + std::to_string(m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::FrameBufferResizeEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::FrameBufferResizeEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "FrameBufferResize";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::FrameBufferResizeEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::Window;
}