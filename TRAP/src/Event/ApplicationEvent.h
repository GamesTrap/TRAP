#pragma once

namespace TRAP
{
	class WindowResizeEvent final : public Event
	{
	public:
		WindowResizeEvent(const unsigned int width, const unsigned int height)
			: m_width(width), m_height(height)
		{			
		}

		unsigned int GetWidth() const { return m_width;  }
		unsigned int GetHeight() const { return m_height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowResize; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "WindowResize"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryApplication); }

	private:
		unsigned int m_width, m_height;
	};

	class WindowCloseEvent final : public Event
	{
	public:
		WindowCloseEvent() = default;

		static EventType GetStaticType() { return EventType::WindowClose; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "WindowClose"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryApplication); }
	};

	class WindowMovedEvent final : public Event
	{
	public:
		WindowMovedEvent(const int x, const int y)
			: m_x(x), m_y(y)
		{			
		}

		int GetX() const { return m_x; }
		int GetY() const { return m_y; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_x << ", " << m_y;

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowMoved; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "WindowMoved"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryApplication); }

	private:
		int m_x, m_y;
	};

	class WindowFocusEvent final : public Event
	{
	public:
		WindowFocusEvent() = default;

		static EventType GetStaticType() { return EventType::WindowFocus; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "WindowFocus"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryApplication); }
	};

	class WindowLostFocusEvent final : public Event
	{
	public:
		WindowLostFocusEvent() = default;

		static EventType GetStaticType() { return EventType::WindowLostFocus; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "WindowLostFocus"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryApplication); }
	};

	class AppTickEvent final : public Event
	{
	public:
		AppTickEvent() = default;

		static EventType GetStaticType() { return EventType::AppTick; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "AppTick"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryApplication); }
	};

	class AppUpdateEvent final : public Event
	{
	public:
		AppUpdateEvent() = default;

		static EventType GetStaticType() { return EventType::AppUpdate; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "AppUpdate"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryApplication); }
	};

	class AppRenderEvent final : public Event
	{
	public:
		AppRenderEvent() = default;

		static EventType GetStaticType() { return EventType::AppRender; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "AppRender"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryApplication); }
	};
}