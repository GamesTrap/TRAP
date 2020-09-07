#include "TRAPPCH.h"
#include "WindowEvent.h"

TRAP::Events::WindowResizeEvent::WindowResizeEvent(const uint32_t width, const uint32_t height, std::string title)
	: m_width(width), m_height(height), m_title(std::move(title))
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

const std::string& TRAP::Events::WindowResizeEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowResizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMinimizeEvent::WindowMinimizeEvent(std::string title)
	: m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::WindowMinimizeEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowMinimizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMaximizeEvent::WindowMaximizeEvent(std::string title)
	: m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::WindowMaximizeEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowMaximizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowRestoreEvent::WindowRestoreEvent(std::string title)
	: m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::WindowRestoreEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowRestoreEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowCloseEvent::WindowCloseEvent(std::string title)
	: m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::WindowCloseEvent::GetWindowTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowCloseEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowMoveEvent::WindowMoveEvent(const int32_t x, const int32_t y, std::string title)
	: m_x(x), m_y(y), m_title(std::move(title))
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

const std::string&TRAP::Events::WindowMoveEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowMoveEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowFocusEvent::WindowFocusEvent(std::string title)
	: m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::WindowFocusEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowFocusEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowLostFocusEvent::WindowLostFocusEvent(std::string title)
	: m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::WindowLostFocusEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowLostFocusEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowDropEvent::WindowDropEvent(std::vector<std::string> paths, std::string title)
	: m_paths(std::move(paths)), m_title(std::move(title))
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Events::WindowDropEvent::GetPaths() const
{
	return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Events::WindowDropEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowDropEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application)  | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::WindowContentScaleEvent::WindowContentScaleEvent(const float xScale, const float yScale, std::string title)
	: m_XScale(xScale), m_YScale(yScale), m_title(std::move(title))
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

const std::string& TRAP::Events::WindowContentScaleEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::WindowContentScaleEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::FrameBufferResizeEvent::FrameBufferResizeEvent(const uint32_t width, const uint32_t height,
                                                             std::string title)
	: m_width(width), m_height(height), m_title(std::move(title))
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

const std::string& TRAP::Events::FrameBufferResizeEvent::GetTitle() const
{
	return m_title;
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

int32_t TRAP::Events::FrameBufferResizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}