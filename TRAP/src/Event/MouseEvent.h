#ifndef _TRAP_MOUSEEVENT_H_
#define _TRAP_MOUSEEVENT_H_

#include "Input/Input.h"
#include "Event.h"

namespace TRAP
{
	class MouseMovedEvent final : public Event
	{
	public:
		MouseMovedEvent(float x, float y, std::string_view title);

		float GetX() const;
		float GetY() const;
		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		float m_mouseX, m_mouseY;
		std::string_view m_title;
	};

	class MouseScrolledEvent final : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset, std::string_view title);

		float GetXOffset() const;
		float GetYOffset() const;
		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		float m_xOffset, m_yOffset;
		std::string_view m_title;
	};

	class MouseButtonEvent : public Event
	{
	public:
		Input::MouseButton GetMouseButton() const;

		int32_t GetCategoryFlags() const override;

	protected:
		explicit MouseButtonEvent(Input::MouseButton button);

		Input::MouseButton m_button;
	};

	class MouseButtonPressedEvent final : public MouseButtonEvent
	{
	public:
		explicit MouseButtonPressedEvent(Input::MouseButton button, std::string_view title);

		std::string_view GetTitle() const;
		
		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		std::string_view m_title;
	};

	class MouseButtonReleasedEvent final : public MouseButtonEvent
	{
	public:
		explicit MouseButtonReleasedEvent(Input::MouseButton button, std::string_view title);

		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		std::string_view m_title;
	};
}

#endif /*_TRAP_MOUSEEVENT_H_*/