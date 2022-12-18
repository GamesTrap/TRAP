#ifndef TRAP_MOUSEEVENT_H
#define TRAP_MOUSEEVENT_H

#include "Input/Input.h"
#include "Event.h"

namespace TRAP::Events
{
	/// <summary>
	/// Mouse moved.
	/// </summary>
	class MouseMoveEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="x">New mouse x position.</param>
		/// <param name="y">New mouse y position.</param>
		/// <param name="window">Pointer to the affected window.</param>
		constexpr MouseMoveEvent(float x, float y, TRAP::Window* window) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseMoveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseMoveEvent(const MouseMoveEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseMoveEvent& operator=(const MouseMoveEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseMoveEvent(MouseMoveEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseMoveEvent& operator=(MouseMoveEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new mouse x position.
		/// </summary>
		/// <returns>Mouse x position.</returns>
		constexpr float GetX() const;
		/// <summary>
		/// Retrieve the new mouse y position.
		/// </summary>
		/// <returns>Mouse y position.</returns>
		constexpr float GetY() const;
		/// <summary>
		/// Retrieve the new mouse position.
		/// </summary>
		/// <returns>Mouse position.</returns>
		constexpr Math::Vec2 GetPosition() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseMoveEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		float m_mouseX, m_mouseY;
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Mouse scrolled.
	/// </summary>
	class MouseScrollEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="xOffset">New mouse scroll wheel x offset.</param>
		/// <param name="yOffset">New mouse scroll wheel y offset.</param>
		/// <param name="window">Pointer to the affected window.</param>
		constexpr MouseScrollEvent(float xOffset, float yOffset, TRAP::Window* window) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseScrollEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseScrollEvent(const MouseScrollEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseScrollEvent& operator=(const MouseScrollEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseScrollEvent(MouseScrollEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseScrollEvent& operator=(MouseScrollEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new mouse scroll wheel x offset.
		/// </summary>
		/// <returns>Mouse scroll wheel x offset.</returns>
		constexpr float GetXOffset() const;
		/// <summary>
		/// Retrieve the new mouse scroll wheel y offset.
		/// </summary>
		/// <returns>Mouse scroll wheel y offset.</returns>
		constexpr float GetYOffset() const;
		/// <summary>
		/// Retrieve the new mouse scroll wheel offset.
		/// </summary>
		/// <returns>Mouse scroll wheel offset.</returns>
		constexpr Math::Vec2 GetOffset() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseScrollEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		float m_xOffset, m_yOffset;
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Mouse button event base class.
	/// </summary>
	class MouseButtonEvent : public Event
	{
	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseButtonEvent() override = default;

		/// <summary>
		/// Retrieve the affected mouse button.
		/// </summary>
		/// <returns>Mouse button.</returns>
		constexpr Input::MouseButton GetMouseButton() const;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="button">Affected mouse button.</param>
		explicit constexpr MouseButtonEvent(Input::MouseButton button) noexcept;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseButtonEvent(const MouseButtonEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseButtonEvent& operator=(const MouseButtonEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseButtonEvent(MouseButtonEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseButtonEvent& operator=(MouseButtonEvent&&) noexcept = default;

		Input::MouseButton m_button;
	};

	/// <summary>
	/// Mouse button pressed.
	/// </summary>
	class MouseButtonPressEvent final : public MouseButtonEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="button">Pressed mouse button.</param>
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr MouseButtonPressEvent(Input::MouseButton button, TRAP::Window* window) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseButtonPressEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseButtonPressEvent(const MouseButtonPressEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseButtonPressEvent& operator=(const MouseButtonPressEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseButtonPressEvent(MouseButtonPressEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseButtonPressEvent& operator=(MouseButtonPressEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseButtonPressEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Mouse button released.
	/// </summary>
	class MouseButtonReleaseEvent final : public MouseButtonEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="button">Released mouse button.</param>
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr MouseButtonReleaseEvent(Input::MouseButton button, TRAP::Window* window) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseButtonReleaseEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseButtonReleaseEvent(const MouseButtonReleaseEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseButtonReleaseEvent& operator=(const MouseButtonReleaseEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseButtonReleaseEvent(MouseButtonReleaseEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseButtonReleaseEvent& operator=(MouseButtonReleaseEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseButtonReleaseEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Mouse entered.
	/// </summary>
	class MouseEnterEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr MouseEnterEvent(TRAP::Window* window) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseEnterEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseEnterEvent(const MouseEnterEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseEnterEvent& operator=(const MouseEnterEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseEnterEvent(MouseEnterEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseEnterEvent& operator=(MouseEnterEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Mouse left.
	/// </summary>
	class MouseLeaveEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr MouseLeaveEvent(TRAP::Window* window) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseLeaveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseLeaveEvent(const MouseLeaveEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseLeaveEvent& operator=(const MouseLeaveEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseLeaveEvent(MouseLeaveEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseLeaveEvent& operator=(MouseLeaveEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		TRAP::Window* m_window;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseMoveEvent::MouseMoveEvent(const float x, const float y, TRAP::Window* window) noexcept
	: m_mouseX(x), m_mouseY(y), m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//


constexpr float TRAP::Events::MouseMoveEvent::GetX() const
{
	return m_mouseX;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr float TRAP::Events::MouseMoveEvent::GetY() const
{
	return m_mouseY;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Math::Vec2 TRAP::Events::MouseMoveEvent::GetPosition() const
{
	return { m_mouseX, m_mouseY };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::MouseMoveEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetStaticType()
{
	return EventType::MouseMove;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseScrollEvent::MouseScrollEvent(const float xOffset, const float yOffset,
														   TRAP::Window* window) noexcept
	: m_xOffset(xOffset), m_yOffset(yOffset), m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr float TRAP::Events::MouseScrollEvent::GetXOffset() const
{
	return m_xOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr float TRAP::Events::MouseScrollEvent::GetYOffset() const
{
	return m_yOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Math::Vec2 TRAP::Events::MouseScrollEvent::GetOffset() const
{
	return { m_xOffset, m_yOffset };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::MouseScrollEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetStaticType()
{
	return EventType::MouseScroll;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Input::MouseButton TRAP::Events::MouseButtonEvent::GetMouseButton() const
{
	return m_button;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseButtonEvent::MouseButtonEvent(const Input::MouseButton button) noexcept
	: m_button(button)
{}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseButtonPressEvent::MouseButtonPressEvent(const Input::MouseButton button,
   																	 TRAP::Window* window) noexcept
	: MouseButtonEvent(button), m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::MouseButtonPressEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetStaticType()
{
	return EventType::MouseButtonPress;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseButtonReleaseEvent::MouseButtonReleaseEvent(const Input::MouseButton button,
																		 TRAP::Window* window) noexcept
	: MouseButtonEvent(button), m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::MouseButtonReleaseEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetStaticType()
{
	return EventType::MouseButtonRelease;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseEnterEvent::MouseEnterEvent(TRAP::Window* window) noexcept
	: m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::MouseEnterEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetStaticType()
{
	return EventType::MouseEnter;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseLeaveEvent::MouseLeaveEvent(TRAP::Window* window) noexcept
	: m_window(window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::MouseLeaveEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetStaticType()
{
	return EventType::MouseLeave;
}

#endif /*TRAP_MOUSEEVENT_H*/