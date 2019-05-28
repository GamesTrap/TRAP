#pragma once

namespace TRAP
{
	class MouseMovedEvent final : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: m_mouseX(x), m_mouseY(y)
		{			
		}

		float GetX() const { return m_mouseX; }
		float GetY() const { return m_mouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseMoved; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "MouseMoved"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryMouse) | static_cast<int>(EventCategory::EventCategoryInput); }

	private:
		float m_mouseX, m_mouseY;
	};

	class MouseScrolledEvent final : public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_xOffset(xOffset), m_yOffset(yOffset)
		{			
		}

		float GetXOffset() const { return m_xOffset; }
		float GetYOffset() const { return m_yOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseScrolled; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "MouseScrolled"; }
		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryMouse) | static_cast<int>(EventCategory::EventCategoryInput); }

	private:
		float m_xOffset, m_yOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		int GetMouseButton() const { return m_button; }

		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryMouse) | static_cast<int>(EventCategory::EventCategoryInput); }

	protected:
		explicit MouseButtonEvent(const int button)
			: m_button(button)
		{			
		}

		int m_button;
	};

	class MouseButtonPressedEvent final : public MouseButtonEvent
	{
	public:
		explicit MouseButtonPressedEvent(const int button)
			: MouseButtonEvent(button)
		{			
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseButtonPressed; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "MouseButtonPressed"; }
	};

	class MouseButtonReleasedEvent final : public MouseButtonEvent
	{
	public:
		explicit MouseButtonReleasedEvent(const int button)
			: MouseButtonEvent(button)
		{			
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseButtonReleased; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "MouseButtonReleased"; }
	};
}
