#ifndef _TRAP_MOUSEEVENT_H_
#define _TRAP_MOUSEEVENT_H_

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
		/// <param name="window">Pointer to the affected Window.</param>
		MouseMoveEvent(float x, float y, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~MouseMoveEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		MouseMoveEvent(const MouseMoveEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MouseMoveEvent& operator=(const MouseMoveEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MouseMoveEvent(MouseMoveEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
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
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseMoveEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
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
		/// <param name="window">Pointer to the affected Window.</param>
		MouseScrollEvent(float xOffset, float yOffset, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~MouseScrollEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		MouseScrollEvent(const MouseScrollEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MouseScrollEvent& operator=(const MouseScrollEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MouseScrollEvent(MouseScrollEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
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
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseScrollEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
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
		/// Virtual Default Destructor.
		/// </summary>
		~MouseButtonEvent() override = default;

		/// <summary>
		/// Retrieve the affected mouse button.
		/// </summary>
		/// <returns>Mouse button.</returns>
		Input::MouseButton GetMouseButton() const;

		/// <summary>
		/// Retrieve the category flags of the Event.
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
		/// Default Copy Constructor.
		/// </summary>
		MouseButtonEvent(const MouseButtonEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MouseButtonEvent& operator=(const MouseButtonEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MouseButtonEvent(MouseButtonEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit MouseButtonPressEvent(Input::MouseButton button, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~MouseButtonPressEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		MouseButtonPressEvent(const MouseButtonPressEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MouseButtonPressEvent& operator=(const MouseButtonPressEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MouseButtonPressEvent(MouseButtonPressEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		MouseButtonPressEvent& operator=(MouseButtonPressEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseButtonPressEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit MouseButtonReleaseEvent(Input::MouseButton button, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~MouseButtonReleaseEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		MouseButtonReleaseEvent(const MouseButtonReleaseEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MouseButtonReleaseEvent& operator=(const MouseButtonReleaseEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MouseButtonReleaseEvent(MouseButtonReleaseEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		MouseButtonReleaseEvent& operator=(MouseButtonReleaseEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the MouseButtonReleaseEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit MouseEnterEvent(TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~MouseEnterEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		MouseEnterEvent(const MouseEnterEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MouseEnterEvent& operator=(const MouseEnterEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MouseEnterEvent(MouseEnterEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		MouseEnterEvent& operator=(MouseEnterEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Mouse leaved.
	/// </summary>
	class MouseLeaveEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected Window.</param>
		explicit MouseLeaveEvent(TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~MouseLeaveEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		MouseLeaveEvent(const MouseLeaveEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MouseLeaveEvent& operator=(const MouseLeaveEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MouseLeaveEvent(MouseLeaveEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		MouseLeaveEvent& operator=(MouseLeaveEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		TRAP::Window* m_window;
	};
}

#endif /*_TRAP_MOUSEEVENT_H_*/