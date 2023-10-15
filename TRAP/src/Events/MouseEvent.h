#ifndef TRAP_MOUSEEVENT_H
#define TRAP_MOUSEEVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Input/Input.h"
#include "Event.h"
#include "Maths/Types.h"
#include "Maths/Vec2.h"

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
		MouseMoveEvent(float x, float y, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~MouseMoveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr MouseMoveEvent(const MouseMoveEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseMoveEvent& operator=(const MouseMoveEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr MouseMoveEvent(MouseMoveEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseMoveEvent& operator=(MouseMoveEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new mouse x position.
		/// </summary>
		/// <returns>Mouse x position.</returns>
		[[nodiscard]] constexpr float GetX() const noexcept;
		/// <summary>
		/// Retrieve the new mouse y position.
		/// </summary>
		/// <returns>Mouse y position.</returns>
		[[nodiscard]] constexpr float GetY() const noexcept;
		/// <summary>
		/// Retrieve the new mouse position.
		/// </summary>
		/// <returns>Mouse position.</returns>
		[[nodiscard]] constexpr Math::Vec2 GetPosition() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the MouseMoveEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

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
		MouseScrollEvent(float xOffset, float yOffset, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~MouseScrollEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr MouseScrollEvent(const MouseScrollEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseScrollEvent& operator=(const MouseScrollEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr MouseScrollEvent(MouseScrollEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseScrollEvent& operator=(MouseScrollEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new mouse scroll wheel x offset.
		/// </summary>
		/// <returns>Mouse scroll wheel x offset.</returns>
		[[nodiscard]] constexpr float GetXOffset() const noexcept;
		/// <summary>
		/// Retrieve the new mouse scroll wheel y offset.
		/// </summary>
		/// <returns>Mouse scroll wheel y offset.</returns>
		[[nodiscard]] constexpr float GetYOffset() const noexcept;
		/// <summary>
		/// Retrieve the new mouse scroll wheel offset.
		/// </summary>
		/// <returns>Mouse scroll wheel offset.</returns>
		[[nodiscard]] constexpr Math::Vec2 GetOffset() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the MouseScrollEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

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
		constexpr ~MouseButtonEvent() override = default;

		/// <summary>
		/// Retrieve the affected mouse button.
		/// </summary>
		/// <returns>Mouse button.</returns>
		[[nodiscard]] constexpr Input::MouseButton GetMouseButton() const noexcept;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="button">Affected mouse button.</param>
		explicit constexpr MouseButtonEvent(Input::MouseButton button) noexcept;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr MouseButtonEvent(const MouseButtonEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr MouseButtonEvent& operator=(const MouseButtonEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr MouseButtonEvent(MouseButtonEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr MouseButtonEvent& operator=(MouseButtonEvent&&) noexcept = default;

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
		MouseButtonPressEvent(Input::MouseButton button, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~MouseButtonPressEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr MouseButtonPressEvent(const MouseButtonPressEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseButtonPressEvent& operator=(const MouseButtonPressEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr MouseButtonPressEvent(MouseButtonPressEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseButtonPressEvent& operator=(MouseButtonPressEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the MouseButtonPressEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr std::string GetName() const override;

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
		MouseButtonReleaseEvent(Input::MouseButton button, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~MouseButtonReleaseEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr MouseButtonReleaseEvent(const MouseButtonReleaseEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseButtonReleaseEvent& operator=(const MouseButtonReleaseEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr MouseButtonReleaseEvent(MouseButtonReleaseEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseButtonReleaseEvent& operator=(MouseButtonReleaseEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the MouseButtonReleaseEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr std::string GetName() const override;

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
		explicit MouseEnterEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~MouseEnterEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr MouseEnterEvent(const MouseEnterEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseEnterEvent& operator=(const MouseEnterEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr MouseEnterEvent(MouseEnterEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseEnterEvent& operator=(MouseEnterEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

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
		explicit MouseLeaveEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~MouseLeaveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr MouseLeaveEvent(const MouseLeaveEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseLeaveEvent& operator=(const MouseLeaveEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr MouseLeaveEvent(MouseLeaveEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseLeaveEvent& operator=(MouseLeaveEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		TRAP::Window* m_window;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Events::MouseMoveEvent::GetX() const noexcept
{
	return m_mouseX;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Events::MouseMoveEvent::GetY() const noexcept
{
	return m_mouseY;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::Events::MouseMoveEvent::GetPosition() const noexcept
{
	return { m_mouseX, m_mouseY };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::MouseMoveEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetStaticType() noexcept
{
	return EventType::MouseMove;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseMoveEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseMoveEvent::GetName() const
{
	return "MouseMove";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Events::MouseScrollEvent::GetXOffset() const noexcept
{
	return m_xOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Events::MouseScrollEvent::GetYOffset() const noexcept
{
	return m_yOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::Events::MouseScrollEvent::GetOffset() const noexcept
{
	return { m_xOffset, m_yOffset };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::MouseScrollEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetStaticType() noexcept
{
	return EventType::MouseScroll;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseScrollEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseScrollEvent::GetName() const
{
	return "MouseScroll";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Input::MouseButton TRAP::Events::MouseButtonEvent::GetMouseButton() const noexcept
{
	return m_button;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseButtonEvent::MouseButtonEvent(const Input::MouseButton button) noexcept
	: m_button(button)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseButtonEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::MouseButtonPressEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetStaticType() noexcept
{
	return EventType::MouseButtonPress;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseButtonPressEvent::GetName() const
{
	return "MouseButtonPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::MouseButtonReleaseEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetStaticType() noexcept
{
	return EventType::MouseButtonRelease;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseButtonReleaseEvent::GetName() const
{
	return "MouseButtonRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::MouseEnterEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetStaticType() noexcept
{
	return EventType::MouseEnter;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseEnterEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseEnterEvent::GetName() const
{
	return "MouseEnter";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::MouseLeaveEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetStaticType() noexcept
{
	return EventType::MouseLeave;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseLeaveEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseLeaveEvent::GetName() const
{
	return "MouseLeave";
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_MOUSEEVENT_H*/