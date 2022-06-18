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
		/// <param name="title">Title of the affected Window.</param>
		MouseMoveEvent(float x, float y, std::string title);

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
		/// Retrieve the title of the affected Window.
		/// </summary>
		/// <returns>Window title.</returns>
		const std::string& GetTitle() const;

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
		std::string m_title;
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
		/// <param name="title">Title of the affected Window.</param>
		MouseScrollEvent(float xOffset, float yOffset, std::string title);

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
		/// Retrieve the title of the affected Window.
		/// </summary>
		/// <returns>Window title.</returns>
		const std::string& GetTitle() const;

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
		std::string m_title;
	};

	/// <summary>
	/// Mouse button event base class.
	/// </summary>
	class MouseButtonEvent : public Event
	{
	public:
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
		/// <param name="title">Title of the affected Window.</param>
		explicit MouseButtonPressEvent(Input::MouseButton button, std::string title);

		/// <summary>
		/// Retrieve the title of the affected Window.
		/// </summary>
		/// <returns>Window title.</returns>
		const std::string& GetTitle() const;

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
		std::string m_title;
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
		/// <param name="title">Title of the affected Window.</param>
		explicit MouseButtonReleaseEvent(Input::MouseButton button, std::string title);

		/// <summary>
		/// Retrieve the title of the affected Window.
		/// </summary>
		/// <returns>Window title.</returns>
		const std::string& GetTitle() const;

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
		std::string m_title;
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
		/// <param name="title">Title of the affected Window.</param>
		explicit MouseEnterEvent(std::string title);

		/// <summary>
		/// Retrieve the title of the affected Window.
		/// </summary>
		/// <returns>Window title.</returns>
		const std::string& GetTitle() const;

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
		std::string m_title;
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
		/// <param name="title">Title of the affected Window.</param>
		explicit MouseLeaveEvent(std::string title);

		/// <summary>
		/// Retrieve the title of the affected Window.
		/// </summary>
		/// <returns>Window title.</returns>
		const std::string& GetTitle() const;

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
		std::string m_title;
	};
}

#endif /*_TRAP_MOUSEEVENT_H_*/