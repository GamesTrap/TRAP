#include "TRAPPCH.h"
#include "WindowEvent.h"

TRAP::WindowResizeEvent::WindowResizeEvent(const uint32_t width, const uint32_t height, const std::string_view title)
	: m_width(width), m_height(height), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::WindowResizeEvent::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::WindowResizeEvent::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowResizeEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::WindowResizeEvent::ToString() const
{
	std::stringstream ss;
	ss << "WindowResizeEvent: " << m_width << "x" << m_height;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowResizeEvent::GetStaticType()
{
	return EventType::WindowResize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowResizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowResizeEvent::GetName() const
{
	return "WindowResize";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowResizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowMinimizeEvent::WindowMinimizeEvent(const std::string_view title)
	: m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowMinimizeEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::WindowMinimizeEvent::ToString() const
{
	std::stringstream ss;
	ss << "WindowMinimizeEvent";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowMinimizeEvent::GetStaticType()
{
	return EventType::WindowMinimize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowMinimizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowMinimizeEvent::GetName() const
{
	return "WindowMinimize";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowMinimizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowMaximizeEvent::WindowMaximizeEvent(const std::string_view title)
	: m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowMaximizeEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::WindowMaximizeEvent::ToString() const
{
	std::stringstream ss;
	ss << "WindowMaximizeEvent";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowMaximizeEvent::GetStaticType()
{
	return EventType::WindowMaximize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowMaximizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowMaximizeEvent::GetName() const
{
	return "WindowMaximize";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowMaximizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowRestoreEvent::WindowRestoreEvent(const std::string_view title)
	: m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowRestoreEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::WindowRestoreEvent::ToString() const
{
	std::stringstream ss;
	ss << "WindowRestoreEvent";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowRestoreEvent::GetStaticType()
{
	return EventType::WindowRestore;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowRestoreEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowRestoreEvent::GetName() const
{
	return "WindowRestoreEvent";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowRestoreEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowCloseEvent::WindowCloseEvent(const std::string_view title)
	: m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowCloseEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowCloseEvent::GetStaticType()
{
	return EventType::WindowClose;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowCloseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowCloseEvent::GetName() const
{
	return "WindowClose";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowCloseEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowMoveEvent::WindowMoveEvent(const int32_t x, const int32_t y, const std::string_view title)
	: m_x(x), m_y(y), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowMoveEvent::GetX() const
{
	return m_x;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowMoveEvent::GetY() const
{
	return m_y;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowMoveEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::WindowMoveEvent::ToString() const
{
	std::stringstream ss;
	ss << "WindowMoveEvent: " << m_x << ", " << m_y;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowMoveEvent::GetStaticType()
{
	return EventType::WindowMove;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowMoveEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowMoveEvent::GetName() const
{
	return "WindowMove";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowMoveEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowFocusEvent::WindowFocusEvent(const std::string_view title)
	: m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowFocusEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowFocusEvent::GetStaticType()
{
	return EventType::WindowFocus;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowFocusEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowFocusEvent::GetName() const
{
	return "WindowFocus";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowFocusEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowLostFocusEvent::WindowLostFocusEvent(const std::string_view title)
	: m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowLostFocusEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowLostFocusEvent::GetStaticType()
{
	return EventType::WindowLostFocus;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowLostFocusEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowLostFocusEvent::GetName() const
{
	return "WindowLostFocus";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowLostFocusEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowDropEvent::WindowDropEvent(std::vector<std::string> paths, const std::string_view title)
	: m_paths(std::move(paths)), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::WindowDropEvent::GetPaths() const
{
	return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowDropEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::WindowDropEvent::ToString() const
{
	std::stringstream ss;
	ss << "WindowDropEvent: ";
	for (uint32_t i = 0; i < m_paths.size(); i++)
		ss << "Path " << i << ": " << m_paths[i] << " ";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowDropEvent::GetStaticType()
{
	return EventType::WindowDrop;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowDropEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowDropEvent::GetName() const
{
	return "WindowDrop";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowDropEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application)  | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowContentScaleEvent::WindowContentScaleEvent(const float xScale, const float yScale, const std::string_view title)
	: m_XScale(xScale), m_YScale(yScale), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::WindowContentScaleEvent::GetXScale() const
{
	return m_XScale;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::WindowContentScaleEvent::GetYScale() const
{
	return m_YScale;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::WindowContentScaleEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::WindowContentScaleEvent::ToString() const
{
	std::stringstream ss;
	ss << "WindowContentScaleEvent: " << m_XScale << "x" << m_YScale;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowContentScaleEvent::GetStaticType()
{
	return EventType::WindowContentScale;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::WindowContentScaleEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::WindowContentScaleEvent::GetName() const
{
	return "WindowContentScale";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::WindowContentScaleEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::FrameBufferResizeEvent::FrameBufferResizeEvent(const uint32_t width, const uint32_t height, const std::string_view title)
	: m_width(width), m_height(height), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::FrameBufferResizeEvent::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::FrameBufferResizeEvent::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::FrameBufferResizeEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FrameBufferResizeEvent::ToString() const
{
	std::stringstream ss;
	ss << "FrameBufferResizeEvent: " << m_width << "x" << m_height;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::FrameBufferResizeEvent::GetStaticType()
{
	return EventType::FrameBufferResize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::FrameBufferResizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::FrameBufferResizeEvent::GetName() const
{
	return "FrameBufferResize";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::FrameBufferResizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}