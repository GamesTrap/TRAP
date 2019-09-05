#ifndef _TRAP_MOUSEEVENT_H_
#define _TRAP_MOUSEEVENT_H_

namespace TRAP
{
	class MouseMovedEvent final : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y, std::string title)
			: m_mouseX(x), m_mouseY(y), m_title(std::move(title))
		{			
		}

		float GetX() const;
		float GetY() const;
		std::string GetTitle() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;

			return ss.str();
		}

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		float m_mouseX, m_mouseY;
		std::string m_title;
	};

	class MouseScrolledEvent final : public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset, std::string title)
			: m_xOffset(xOffset), m_yOffset(yOffset), m_title(std::move(title))
		{			
		}

		float GetXOffset() const;
		float GetYOffset() const;
		std::string GetTitle() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();

			return ss.str();
		}

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;

	private:
		float m_xOffset, m_yOffset;
		std::string m_title;
	};

	class MouseButtonEvent : public Event
	{
	public:
		int32_t GetMouseButton() const;

		int32_t GetCategoryFlags() const override;

	protected:
		explicit MouseButtonEvent(const int32_t button)
			: m_button(button)
		{			
		}

		int32_t m_button;
	};

	class MouseButtonPressedEvent final : public MouseButtonEvent
	{
	public:
		explicit MouseButtonPressedEvent(const int32_t button, std::string title)
			: MouseButtonEvent(button), m_title(std::move(title))
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;

			return ss.str();
		}

		std::string GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		std::string m_title;
	};

	class MouseButtonReleasedEvent final : public MouseButtonEvent
	{
	public:
		explicit MouseButtonReleasedEvent(const int32_t button, std::string title)
			: MouseButtonEvent(button), m_title(std::move(title))
		{			
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;

			return ss.str();
		}

		std::string GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		std::string m_title;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::MouseMovedEvent::GetX() const
{
	return m_mouseX;
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::MouseMovedEvent::GetY() const
{
	return m_mouseY;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::MouseMovedEvent::GetStaticType()
{
	return EventType::MouseMoved;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::MouseMovedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::MouseMovedEvent::GetName() const
{
	return "MouseMoved";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::MouseMovedEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::EventCategoryMouse) | static_cast<int32_t>(EventCategory::EventCategoryInput);
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::MouseScrolledEvent::GetXOffset() const
{
	return m_xOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::MouseScrolledEvent::GetYOffset() const
{
	return m_yOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::MouseScrolledEvent::GetStaticType()
{
	return EventType::MouseScrolled;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::MouseScrolledEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::MouseScrolledEvent::GetName() const
{
	return "MouseScrolled";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::MouseScrolledEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::EventCategoryMouse) | static_cast<int32_t>(EventCategory::EventCategoryInput);
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::MouseButtonEvent::GetMouseButton() const
{
	return m_button;
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::MouseButtonEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::EventCategoryMouse) | static_cast<int32_t>(EventCategory::EventCategoryInput);
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::MouseButtonPressedEvent::GetStaticType()
{
	return EventType::MouseButtonPressed;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::MouseButtonPressedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::MouseButtonPressedEvent::GetName() const
{
	return "MouseButtonPressed";
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::MouseButtonReleasedEvent::GetStaticType()
{
	return EventType::MouseButtonReleased;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::MouseButtonReleasedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::MouseButtonReleasedEvent::GetName() const
{
	return "MouseButtonReleased";
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::MouseMovedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::MouseScrolledEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::MouseButtonPressedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::MouseButtonReleasedEvent::GetTitle() const
{
	return m_title;
}

#endif /*_TRAP_MOUSEEVENT_H_*/