#ifndef TRAP_KEYEVENT_H
#define TRAP_KEYEVENT_H

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
		~KeyEvent() override = default;

		/// <summary>
		/// Retrieve the affected key.
		/// </summary>
		/// <returns>Key.</returns>
		constexpr Input::Key GetKey() const noexcept;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="key">Affected key.</param>
		explicit constexpr KeyEvent(Input::Key key) noexcept;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		KeyEvent(const KeyEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyEvent& operator=(const KeyEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyEvent(KeyEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyEvent& operator=(KeyEvent&&) noexcept = default;

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
		/// <param name="repeatCount">Amount of key press repeats.</param>
		/// <param name="window">Pointer to the affected window.</param>
		KeyPressEvent(Input::Key key, uint32_t repeatCount, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~KeyPressEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		KeyPressEvent(const KeyPressEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyPressEvent& operator=(const KeyPressEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyPressEvent(KeyPressEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyPressEvent& operator=(KeyPressEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve tha amount of key press repeats.
		/// </summary>
		/// <returns>Repeat count.</returns>
		constexpr uint32_t GetRepeatCount() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyPressEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;

	private:
		uint32_t m_repeatCount;
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
		~KeyReleaseEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		KeyReleaseEvent(const KeyReleaseEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyReleaseEvent& operator=(const KeyReleaseEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyReleaseEvent(KeyReleaseEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyReleaseEvent& operator=(KeyReleaseEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyReleaseEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;

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
		explicit KeyTypeEvent(uint32_t codePoint, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~KeyTypeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		KeyTypeEvent(const KeyTypeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyTypeEvent& operator=(const KeyTypeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyTypeEvent(KeyTypeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyTypeEvent& operator=(KeyTypeEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const noexcept;
		/// <summary>
		/// Retrieve the entered Unicode code point.
		/// </summary>
		/// <returns>Unicode code point.</returns>
		constexpr uint32_t GetCodePoint() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyTypeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const noexcept override;

	private:
		TRAP::Window* m_window;
		uint32_t m_codePoint;
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
		explicit KeyLayoutEvent(std::string layout) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~KeyLayoutEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		KeyLayoutEvent(const KeyLayoutEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyLayoutEvent& operator=(const KeyLayoutEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyLayoutEvent(KeyLayoutEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyLayoutEvent& operator=(KeyLayoutEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the human-readable name of the new keyboard layout.
		/// </summary>
		/// <returns>Name of new keyboard layout.</returns>
		std::string GetLayout() const noexcept;

		/// <summary>
		/// Get a string representation of the KeyLayoutEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const noexcept override;

	private:
		std::string m_layout;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Input::Key TRAP::Events::KeyEvent::GetKey() const noexcept
{
	return m_key;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::KeyEvent::KeyEvent(const Input::Key key) noexcept
	: m_key(key)
{}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP::Events::KeyPressEvent::GetRepeatCount() const noexcept
{
	return m_repeatCount;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::KeyPressEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetStaticType() noexcept
{
	return EventType::KeyPress;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::KeyReleaseEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetStaticType() noexcept
{
	return EventType::KeyRelease;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::KeyTypeEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP::Events::KeyTypeEvent::GetCodePoint() const noexcept
{
	return m_codePoint;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetStaticType() noexcept
{
	return EventType::KeyType;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetStaticType() noexcept
{
	return EventType::KeyLayout;
}

#endif /*TRAP_KEYEVENT_H*/