#ifndef _TRAP_APPLICATIONEVENT_H_
#define _TRAP_APPLICATIONEVENT_H_

namespace TRAP
{
	class WindowResizeEvent final : public Event
	{
	public:
		WindowResizeEvent(const unsigned int width, const unsigned int height)
			: m_width(width), m_height(height)
		{			
		}

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;

			return ss.str();
		}

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;

	private:
		unsigned int m_width, m_height;
	};

	class WindowCloseEvent final : public Event
	{
	public:
		WindowCloseEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;
	};

	class WindowMovedEvent final : public Event
	{
	public:
		WindowMovedEvent(const int x, const int y)
			: m_x(x), m_y(y)
		{			
		}

		int GetX() const;
		int GetY() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_x << ", " << m_y;

			return ss.str();
		}

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;

	private:
		int m_x, m_y;
	};

	class WindowFocusEvent final : public Event
	{
	public:
		WindowFocusEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;
	};

	class WindowLostFocusEvent final : public Event
	{
	public:
		WindowLostFocusEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;
	};

	class AppTickEvent final : public Event
	{
	public:
		AppTickEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;
	};

	class AppUpdateEvent final : public Event
	{
	public:
		AppUpdateEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;
	};

	class AppRenderEvent final : public Event
	{
	public:
		AppRenderEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;
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

inline int TRAP::WindowResizeEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryApplication);
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

inline int TRAP::WindowCloseEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryApplication);
}

//-------------------------------------------------------------------------------------------------------------------//

inline int TRAP::WindowMovedEvent::GetX() const
{
	return m_x;
}

//-------------------------------------------------------------------------------------------------------------------//

inline int TRAP::WindowMovedEvent::GetY() const
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
	return "WindowMoved";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int TRAP::WindowMovedEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryApplication);
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

inline int TRAP::WindowFocusEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryApplication);
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

inline int TRAP::WindowLostFocusEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryApplication);
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

inline int TRAP::AppTickEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryApplication);
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

inline int TRAP::AppUpdateEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryApplication);
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

inline int TRAP::AppRenderEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryApplication);
}

#endif /*_TRAP_APPLICATIONEVENT_H_*/