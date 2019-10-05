#ifndef _TRAP_APPLICATIONEVENT_H_
#define _TRAP_APPLICATIONEVENT_H_

#include "Event.h"

namespace TRAP
{
	class WindowResizeEvent final : public Event
	{
	public:
		WindowResizeEvent(const uint32_t width, const uint32_t height, std::string title)
			: m_width(width), m_height(height), m_title(std::move(title))
		{			
		}

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		std::string GetTitle() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << "x" << m_height;

			return ss.str();
		}

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		uint32_t m_width, m_height;
		std::string m_title;
	};

	class WindowCloseEvent final : public Event
	{
	public:
		explicit WindowCloseEvent(std::string title)
			: m_title(std::move(title))
		{			
		}

		std::string GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string m_title;
	};

	class WindowMovedEvent final : public Event
	{
	public:
		WindowMovedEvent(const int32_t x, const int32_t y, std::string title)
			: m_x(x), m_y(y), m_title(std::move(title))
		{
		}

		int32_t GetX() const;
		int32_t GetY() const;
		std::string GetTitle() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_x << ", " << m_y;

			return ss.str();
		}

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		int32_t m_x, m_y;
		std::string m_title;
	};

	class WindowFocusEvent final : public Event
	{
	public:
		explicit WindowFocusEvent(std::string title)
			: m_title(std::move(title))
		{			
		}

		std::string GetTitle() const;
		
		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string m_title;
	};

	class WindowLostFocusEvent final : public Event
	{
	public:
		explicit WindowLostFocusEvent(std::string title)
			: m_title(std::move(title))
		{			
		}

		std::string GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string m_title;
	};

	class AppTickEvent final : public Event
	{
	public:
		AppTickEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
	};

	class AppUpdateEvent final : public Event
	{
	public:
		AppUpdateEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
	};

	class AppRenderEvent final : public Event
	{
	public:
		AppRenderEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned TRAP::WindowResizeEvent::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned TRAP::WindowResizeEvent::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowResizeEvent::GetStaticType()
{
	return EventType::WindowResize;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowResizeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::WindowResizeEvent::GetName() const
{
	return "WindowResize";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::WindowResizeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowCloseEvent::GetStaticType()
{
	return EventType::WindowClose;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowCloseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::WindowCloseEvent::GetName() const
{
	return "WindowClose";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::WindowCloseEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::WindowMovedEvent::GetX() const
{
	return m_x;
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::WindowMovedEvent::GetY() const
{
	return m_y;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowMovedEvent::GetStaticType()
{
	return EventType::WindowMoved;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowMovedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::WindowMovedEvent::GetName() const
{
	return "WindowMove";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::WindowMovedEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowFocusEvent::GetStaticType()
{
	return EventType::WindowFocus;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowFocusEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::WindowFocusEvent::GetName() const
{
	return "WindowFocus";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::WindowFocusEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowLostFocusEvent::GetStaticType()
{
	return EventType::WindowLostFocus;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::WindowLostFocusEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::WindowLostFocusEvent::GetName() const
{
	return "WindowLostFocus";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::WindowLostFocusEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::AppTickEvent::GetStaticType()
{
	return EventType::AppTick;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::AppTickEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::AppTickEvent::GetName() const
{
	return "AppTick";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::AppTickEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::AppUpdateEvent::GetStaticType()
{
	return EventType::AppUpdate;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::AppUpdateEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::AppUpdateEvent::GetName() const
{
	return "AppUpdate";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::AppUpdateEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::AppRenderEvent::GetStaticType()
{
	return EventType::AppRender;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::AppRenderEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::AppRenderEvent::GetName() const
{
	return "AppRender";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::AppRenderEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::WindowResizeEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::WindowCloseEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::WindowMovedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::WindowFocusEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::WindowLostFocusEvent::GetTitle() const
{
	return m_title;
}

#endif /*_TRAP_APPLICATIONEVENT_H_*/