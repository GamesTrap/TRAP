#ifndef _TRAP_MOUSEEVENT_H_
#define _TRAP_MOUSEEVENT_H_

namespace TRAP
{
	class MouseMovedEvent final : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: m_mouseX(x), m_mouseY(y)
		{			
		}

		float GetX() const;
		float GetY() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;

			return ss.str();
		}

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;

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

		float GetXOffset() const;
		float GetYOffset() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();

			return ss.str();
		}

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int GetCategoryFlags() const override;

	private:
		float m_xOffset, m_yOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		int GetMouseButton() const;

		int GetCategoryFlags() const override;

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

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
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

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
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

inline int TRAP::MouseMovedEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryMouse) | static_cast<int>(EventCategory::EventCategoryInput);
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

inline int TRAP::MouseScrolledEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryMouse) | static_cast<int>(EventCategory::EventCategoryInput);
}

//-------------------------------------------------------------------------------------------------------------------//

inline int TRAP::MouseButtonEvent::GetMouseButton() const
{
	return m_button;
}

//-------------------------------------------------------------------------------------------------------------------//

inline int TRAP::MouseButtonEvent::GetCategoryFlags() const
{
	return static_cast<int>(EventCategory::EventCategoryMouse) | static_cast<int>(EventCategory::EventCategoryInput);
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

#endif /*_TRAP_MOUSEEVENT_H_*/