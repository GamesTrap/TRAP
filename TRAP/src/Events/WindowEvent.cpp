#include "TRAPPCH.h"
#include "WindowEvent.h"

#include "Window/Window.h"

TRAP::Events::WindowResizeEvent::WindowResizeEvent(const uint32_t width, const uint32_t height, TRAP::Window* window)
	: m_width(width), m_height(height), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Events::WindowResizeEvent::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Events::WindowResizeEvent::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowResizeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowResizeEvent::ToString() const
{
	return "WindowResizeEvent: " + std::to_string(m_width) + "x" + std::to_string(m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowResizeEvent::GetStaticType()
{
	return EventType::WindowResize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowResizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowResizeEvent::GetName() const
{
	return "WindowResize";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowResizeEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMinimizeEvent::WindowMinimizeEvent(TRAP::Window* window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowMinimizeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMinimizeEvent::ToString() const
{
	return "WindowMinimizeEvent";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMinimizeEvent::GetStaticType()
{
	return EventType::WindowMinimize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMinimizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowMinimizeEvent::GetName() const
{
	return "WindowMinimize";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowMinimizeEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMaximizeEvent::WindowMaximizeEvent(TRAP::Window* window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowMaximizeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMaximizeEvent::ToString() const
{
	return "WindowMaximizeEvent";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMaximizeEvent::GetStaticType()
{
	return EventType::WindowMaximize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMaximizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowMaximizeEvent::GetName() const
{
	return "WindowMaximize";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowMaximizeEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowRestoreEvent::WindowRestoreEvent(TRAP::Window* window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowRestoreEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowRestoreEvent::ToString() const
{
	return "WindowRestoreEvent";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowRestoreEvent::GetStaticType()
{
	return EventType::WindowRestore;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowRestoreEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowRestoreEvent::GetName() const
{
	return "WindowRestoreEvent";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowRestoreEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowCloseEvent::WindowCloseEvent(TRAP::Window* window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowCloseEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowCloseEvent::ToString() const
{
	return "WindowCloseEvent: " + m_window->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowCloseEvent::GetStaticType()
{
	return EventType::WindowClose;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowCloseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowCloseEvent::GetName() const
{
	return "WindowClose";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowCloseEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMoveEvent::WindowMoveEvent(const int32_t x, const int32_t y, TRAP::Window* window)
	: m_x(x), m_y(y), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::WindowMoveEvent::GetX() const
{
	return m_x;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::WindowMoveEvent::GetY() const
{
	return m_y;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2i TRAP::Events::WindowMoveEvent::GetPosition() const
{
	return Math::Vec2i(m_x, m_y);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowMoveEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowMoveEvent::ToString() const
{
	return "WindowMoveEvent: " + std::to_string(m_x) + ", " + std::to_string(m_y);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMoveEvent::GetStaticType()
{
	return EventType::WindowMove;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowMoveEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowMoveEvent::GetName() const
{
	return "WindowMove";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowMoveEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowFocusEvent::WindowFocusEvent(TRAP::Window* window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowFocusEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowFocusEvent::ToString() const
{
	return "WindowFocusEvent: " + m_window->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowFocusEvent::GetStaticType()
{
	return EventType::WindowFocus;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowFocusEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowFocusEvent::GetName() const
{
	return "WindowFocus";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowFocusEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowLostFocusEvent::WindowLostFocusEvent(TRAP::Window* window)
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowLostFocusEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowLostFocusEvent::ToString() const
{
	return "WindowLostFocusEvent: " + m_window->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowLostFocusEvent::GetStaticType()
{
	return EventType::WindowLostFocus;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowLostFocusEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowLostFocusEvent::GetName() const
{
	return "WindowLostFocus";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowLostFocusEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowDropEvent::WindowDropEvent(std::vector<std::string> paths, TRAP::Window* window)
	: m_paths(std::move(paths)), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Events::WindowDropEvent::GetPaths() const
{
	return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowDropEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowDropEvent::ToString() const
{
	std::string result = "WindowDropEvent: ";
	for (uint32_t i = 0; i < m_paths.size(); i++)
		result += "Path " + std::to_string(i) + ": " + m_paths[i] + '\n';
	result.pop_back();
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowDropEvent::GetStaticType()
{
	return EventType::WindowDrop;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowDropEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowDropEvent::GetName() const
{
	return "WindowDrop";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowDropEvent::GetCategoryFlags() const
{
	return EventCategory::Application | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowContentScaleEvent::WindowContentScaleEvent(const float xScale, const float yScale,
	TRAP::Window* window)
	: m_XScale(xScale), m_YScale(yScale), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Events::WindowContentScaleEvent::GetXScale() const
{
	return m_XScale;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Events::WindowContentScaleEvent::GetYScale() const
{
	return m_YScale;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Events::WindowContentScaleEvent::GetScale() const
{
	return Math::Vec2(m_XScale, m_YScale);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::WindowContentScaleEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::WindowContentScaleEvent::ToString() const
{
	return "WindowContentScaleEvent: " + std::to_string(m_XScale) + "x" + std::to_string(m_YScale);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowContentScaleEvent::GetStaticType()
{
	return EventType::WindowContentScale;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::WindowContentScaleEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::WindowContentScaleEvent::GetName() const
{
	return "WindowContentScale";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::WindowContentScaleEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::FrameBufferResizeEvent::FrameBufferResizeEvent(const uint32_t width, const uint32_t height,
                                                             TRAP::Window* window)
	: m_width(width), m_height(height), m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Events::FrameBufferResizeEvent::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Events::FrameBufferResizeEvent::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Events::FrameBufferResizeEvent::GetSize() const
{
	return Math::Vec2ui(m_width, m_height);
}


//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Events::FrameBufferResizeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::FrameBufferResizeEvent::ToString() const
{
	return "FrameBufferResizeEvent: " + std::to_string(m_width) + "x" + std::to_string(m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::FrameBufferResizeEvent::GetStaticType()
{
	return EventType::FrameBufferResize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::FrameBufferResizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::FrameBufferResizeEvent::GetName() const
{
	return "FrameBufferResize";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::FrameBufferResizeEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}