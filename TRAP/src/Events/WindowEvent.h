#ifndef _TRAP_WINDOWEVENT_H_
#define _TRAP_WINDOWEVENT_H_

#include "Event.h"

namespace TRAP::Events
{
	class WindowResizeEvent final : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height, std::string title);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		const std::string& GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		uint32_t m_width, m_height;
		std::string m_title;
	};

	class WindowMinimizeEvent final : public Event
	{
	public:
		explicit WindowMinimizeEvent(std::string title);

		const std::string& GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string m_title;
	};

	class WindowMaximizeEvent final : public Event
	{
	public:
		explicit WindowMaximizeEvent(std::string title);

		const std::string& GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string m_title;
	};

	class WindowRestoreEvent final : public Event
	{
	public:
		explicit WindowRestoreEvent(std::string title);

		const std::string& GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string m_title;
	};

	class WindowCloseEvent final : public Event
	{
	public:
		explicit WindowCloseEvent(std::string title);

		const std::string& GetWindowTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string m_title;
	};

	class WindowMoveEvent final : public Event
	{
	public:
		WindowMoveEvent(int32_t x, int32_t y, std::string title);

		int32_t GetX() const;
		int32_t GetY() const;
		const std::string& GetTitle() const;

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

		const std::string& GetTitle() const;

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

		const std::string& GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::string m_title;
	};

	class WindowDropEvent final : public Event
	{
	public:
		explicit WindowDropEvent(std::vector<std::string> paths, std::string title);

		const std::vector<std::string>& GetPaths() const;
		const std::string& GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		std::vector<std::string> m_paths{};
		std::string m_title;
	};

	class WindowContentScaleEvent final : public Event
	{
	public:
		explicit WindowContentScaleEvent(float xScale, float yScale, std::string title);

		float GetXScale() const;
		float GetYScale() const;
		const std::string& GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		float m_XScale;
		float m_YScale;
		std::string m_title;
	};

	class FrameBufferResizeEvent final : public Event
	{
	public:
		FrameBufferResizeEvent(uint32_t width, uint32_t height, std::string title);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		const std::string& GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		uint32_t m_width, m_height;
		std::string m_title;
	};
}

#endif /*_TRAP_WINDOWEVENT_H_*/