#ifndef TRAP_KEYEVENT_H
#define TRAP_KEYEVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Input/Input.h"
#include "Event.h"

namespace TRAP::Events
{
	/// <summary>
	/// Key event base class.
	/// </summary>
	class KeyEvent : public Event
	{
	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~KeyEvent() override = default;

		/// <summary>
		/// Retrieve the affected key.
		/// </summary>
		/// <returns>Key.</returns>
		[[nodiscard]] constexpr Input::Key GetKey() const noexcept;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="key">Affected key.</param>
		explicit constexpr KeyEvent(Input::Key key) noexcept;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr KeyEvent(const KeyEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr KeyEvent& operator=(const KeyEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr KeyEvent(KeyEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr KeyEvent& operator=(KeyEvent&&) noexcept = default;

		Input::Key m_key;
	};

	/// <summary>
	/// Key pressed.
	/// </summary>
	class KeyPressEvent final : public KeyEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="key">Pressed key.</param>
		/// <param name="window">Pointer to the affected window.</param>
		KeyPressEvent(Input::Key key, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~KeyPressEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr KeyPressEvent(const KeyPressEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyPressEvent& operator=(const KeyPressEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr KeyPressEvent(KeyPressEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyPressEvent& operator=(KeyPressEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyPressEvent.
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
	/// Key repeated.
	/// </summary>
	class KeyRepeatEvent final : public KeyEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="key">Repeated key.</param>
		/// <param name="window">Pointer to the affected window.</param>
		KeyRepeatEvent(Input::Key key, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~KeyRepeatEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr KeyRepeatEvent(const KeyRepeatEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyRepeatEvent& operator=(const KeyRepeatEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr KeyRepeatEvent(KeyRepeatEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyRepeatEvent& operator=(KeyRepeatEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyRepeatEvent.
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
	/// Key released.
	/// </summary>
	class KeyReleaseEvent final : public KeyEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="key">Released key.</param>
		/// <param name="window">Pointer to the affected window.</param>
		explicit KeyReleaseEvent(Input::Key key, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~KeyReleaseEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr KeyReleaseEvent(const KeyReleaseEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyReleaseEvent& operator=(const KeyReleaseEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr KeyReleaseEvent(KeyReleaseEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyReleaseEvent& operator=(KeyReleaseEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyReleaseEvent.
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
	/// Key typed.
	/// </summary>
	class KeyTypeEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="codePoint">Unicode code point entered.</param>
		/// <param name="window">Pointer to the affected window.</param>
		explicit KeyTypeEvent(u32 codePoint, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~KeyTypeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr KeyTypeEvent(const KeyTypeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyTypeEvent& operator=(const KeyTypeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr KeyTypeEvent(KeyTypeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyTypeEvent& operator=(KeyTypeEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;
		/// <summary>
		/// Retrieve the entered Unicode code point.
		/// </summary>
		/// <returns>Unicode code point.</returns>
		[[nodiscard]] constexpr u32 GetCodePoint() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyTypeEvent.
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
		TRAP::Window* m_window;
		u32 m_codePoint;
	};

	/// <summary>
	/// Key layout changed.
	/// </summary>
	class KeyLayoutEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="layout">New keyboard layout.</param>
		constexpr explicit KeyLayoutEvent(std::string layout) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~KeyLayoutEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr KeyLayoutEvent(const KeyLayoutEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr KeyLayoutEvent& operator=(const KeyLayoutEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr KeyLayoutEvent(KeyLayoutEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr KeyLayoutEvent& operator=(KeyLayoutEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the human-readable name of the new keyboard layout.
		/// </summary>
		/// <returns>Name of new keyboard layout.</returns>
		[[nodiscard]] constexpr std::string GetLayout() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyLayoutEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr std::string ToString() const override;

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
		std::string m_layout;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Input::Key TRAP::Events::KeyEvent::GetKey() const noexcept
{
	return m_key;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::KeyEvent::KeyEvent(const Input::Key key) noexcept
	: m_key(key)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::KeyEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Keyboard | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::KeyPressEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetStaticType() noexcept
{
	return EventType::KeyPress;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::KeyPressEvent::GetName() const
{
	return "KeyPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::KeyRepeatEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyRepeatEvent::GetStaticType() noexcept
{
	return EventType::KeyRepeat;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyRepeatEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::KeyRepeatEvent::GetName() const
{
	return "KeyRepeat";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::KeyReleaseEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetStaticType() noexcept
{
	return EventType::KeyRelease;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::KeyReleaseEvent::GetName() const
{
	return "KeyRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::KeyTypeEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Events::KeyTypeEvent::GetCodePoint() const noexcept
{
	return m_codePoint;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetStaticType() noexcept
{
	return EventType::KeyType;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::KeyTypeEvent::GetName() const
{
	return "KeyType";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::KeyTypeEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Keyboard | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::KeyLayoutEvent::KeyLayoutEvent(std::string layout) noexcept
	: m_layout(std::move(layout))
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetStaticType() noexcept
{
	return EventType::KeyLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::KeyLayoutEvent::GetName() const
{
	return "KeyLayout";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::KeyLayoutEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Keyboard | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::KeyLayoutEvent::GetLayout() const noexcept
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::KeyLayoutEvent::ToString() const
{
	return std::string("KeyLayoutEvent: ") + m_layout;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_KEYEVENT_H*/