#ifndef _TRAP_WINDOWEVENT_H_
#define _TRAP_WINDOWEVENT_H_

#include "Event.h"

namespace TRAP::Events
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

	class WindowMinimizeEvent final : public Event
	{
	public:
		WindowMinimizeEvent(std::string_view title);

		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string_view m_title;
	};

	class WindowMaximizeEvent final : public Event
	{
	public:
		WindowMaximizeEvent(std::string_view title);

		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string_view m_title;
	};

	class WindowRestoreEvent final : public Event
	{
	public:
		WindowRestoreEvent(std::string_view title);

		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
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

	class WindowDropEvent final : public Event
	{
	public:
		explicit WindowDropEvent(std::vector<std::string> paths, std::string_view title);

		std::vector<std::string> GetPaths() const;
		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::vector<std::string> m_paths{};
		std::string_view m_title;
	};

	class WindowContentScaleEvent final : public Event
	{
	public:
		explicit WindowContentScaleEvent(float xScale, float yScale, std::string_view title);

		float GetXScale() const;
		float GetYScale() const;
		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		float m_XScale;
		float m_YScale;
		std::string_view m_title;
	};

	class FrameBufferResizeEvent final : public Event
	{
	public:
		FrameBufferResizeEvent(uint32_t width, uint32_t height, std::string_view title);

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
}

#endif /*_TRAP_WINDOWEVENT_H_*/