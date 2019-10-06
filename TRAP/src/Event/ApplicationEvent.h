#ifndef _TRAP_APPLICATIONEVENT_H_
#define _TRAP_APPLICATIONEVENT_H_

#include "Event.h"

namespace TRAP
{
	class WindowResizeEvent final : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height, std::string title);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		std::string GetTitle() const;

		std::string ToString() const override;

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
		explicit WindowCloseEvent(std::string title);

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
		WindowMovedEvent(int32_t x, int32_t y, std::string title);

		int32_t GetX() const;
		int32_t GetY() const;
		std::string GetTitle() const;

		std::string ToString() const override;

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
		explicit WindowFocusEvent(std::string title);

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
		explicit WindowLostFocusEvent(std::string title);

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

#endif /*_TRAP_APPLICATIONEVENT_H_*/