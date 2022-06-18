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
		MouseMoveEvent(float x, float y, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseMoveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseMoveEvent(const MouseMoveEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseMoveEvent& operator=(const MouseMoveEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseMoveEvent(MouseMoveEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseMoveEvent& operator=(MouseMoveEvent&&) = default;

		/// <summary>
		/// Retrieve the new mouse x position.
		/// </summary>
		/// <returns>Mouse x position.</returns>
		float GetX() const;
		/// <summary>
		/// Retrieve the new mouse y position.
		/// </summary>
		/// <returns>Mouse y position.</returns>
		float GetY() const;
		/// <summary>
		/// Retrieve the new mouse position.
		/// </summary>
		/// <returns>Mouse position.</returns>
		Math::Vec2 GetPosition() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseMoveEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
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
		MouseScrollEvent(float xOffset, float yOffset, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseScrollEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseScrollEvent(const MouseScrollEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseScrollEvent& operator=(const MouseScrollEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseScrollEvent(MouseScrollEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseScrollEvent& operator=(MouseScrollEvent&&) = default;

		/// <summary>
		/// Retrieve the new mouse scroll wheel x offset.
		/// </summary>
		/// <returns>Mouse scroll wheel x offset.</returns>
		float GetXOffset() const;
		/// <summary>
		/// Retrieve the new mouse scroll wheel y offset.
		/// </summary>
		/// <returns>Mouse scroll wheel y offset.</returns>
		float GetYOffset() const;
		/// <summary>
		/// Retrieve the new mouse scroll wheel offset.
		/// </summary>
		/// <returns>Mouse scroll wheel offset.</returns>
		Math::Vec2 GetOffset() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseScrollEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
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
		Input::MouseButton GetMouseButton() const;

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
		explicit MouseButtonEvent(Input::MouseButton button);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseButtonEvent(const MouseButtonEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseButtonEvent& operator=(const MouseButtonEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseButtonEvent(MouseButtonEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseButtonEvent& operator=(MouseButtonEvent&&) = default;

		/// <summary>
		/// Convert a mouse button to a string representation.
		/// </summary>
		/// <param name="button">Mouse button.</param>
		/// <returns>String representation.</returns>
		static std::string MouseButtonToString(Input::MouseButton button);

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
		explicit MouseButtonPressEvent(Input::MouseButton button, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseButtonPressEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseButtonPressEvent(const MouseButtonPressEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseButtonPressEvent& operator=(const MouseButtonPressEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseButtonPressEvent(MouseButtonPressEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseButtonPressEvent& operator=(MouseButtonPressEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseButtonPressEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;

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
		explicit MouseButtonReleaseEvent(Input::MouseButton button, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseButtonReleaseEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseButtonReleaseEvent(const MouseButtonReleaseEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseButtonReleaseEvent& operator=(const MouseButtonReleaseEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseButtonReleaseEvent(MouseButtonReleaseEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseButtonReleaseEvent& operator=(MouseButtonReleaseEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseButtonReleaseEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;

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
		~MouseEnterEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseEnterEvent(const MouseEnterEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseEnterEvent& operator=(const MouseEnterEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseEnterEvent(MouseEnterEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseEnterEvent& operator=(MouseEnterEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
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
		explicit MouseLeaveEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~MouseLeaveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MouseLeaveEvent(const MouseLeaveEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MouseLeaveEvent& operator=(const MouseLeaveEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MouseLeaveEvent(MouseLeaveEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MouseLeaveEvent& operator=(MouseLeaveEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		TRAP::Window* m_window;
	};
}

#endif /*TRAP_MOUSEEVENT_H*/