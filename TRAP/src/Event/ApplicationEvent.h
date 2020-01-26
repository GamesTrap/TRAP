#ifndef _TRAP_APPLICATIONEVENT_H_
#define _TRAP_APPLICATIONEVENT_H_

#include "Event.h"

namespace TRAP
{
	class WindowResizeEvent final : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height, std::string_view title);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		uint32_t m_width, m_height;
		std::string_view m_title;
	};

	class WindowCloseEvent final : public Event
	{
	public:
		explicit WindowCloseEvent(std::string_view title);

		std::string_view GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string_view m_title;
	};

	class WindowMoveEvent final : public Event
	{
	public:
		WindowMoveEvent(int32_t x, int32_t y, std::string_view title);

		int32_t GetX() const;
		int32_t GetY() const;
		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		int32_t m_x, m_y;
		std::string_view m_title;
	};

	class WindowFocusEvent final : public Event
	{
	public:
		explicit WindowFocusEvent(std::string_view title);

		std::string_view GetTitle() const;
		
		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string_view m_title;
	};

	class WindowLostFocusEvent final : public Event
	{
	public:
		explicit WindowLostFocusEvent(std::string_view title);

		std::string_view GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string_view m_title;
	};

	//Unused for now
	class AppTickEvent final : public Event
	{
	public:
		AppTickEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
	};

	//Unused for now
	class AppUpdateEvent final : public Event
	{
	public:
		AppUpdateEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
	};
	
	//Unused for now
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