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
		Input::Key GetKey() const;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="key">Affected key.</param>
		explicit KeyEvent(Input::Key key);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		KeyEvent(const KeyEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyEvent& operator=(const KeyEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyEvent(KeyEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyEvent& operator=(KeyEvent&&) = default;

		/// <summary>
		/// Get a string representation for a non printable key (space key for example).
		/// </summary>
		/// <param name="key">Key to get.</param>
		/// <returns>String representation.</returns>
		static std::string NonPrintableKeyToString(Input::Key key);

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
		KeyPressEvent(const KeyPressEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyPressEvent& operator=(const KeyPressEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyPressEvent(KeyPressEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyPressEvent& operator=(KeyPressEvent&&) = default;

		/// <summary>
		/// Retrieve tha amount of key press repeats.
		/// </summary>
		/// <returns>Repeat count.</returns>
		uint32_t GetRepeatCount() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the KeyPressEvent.
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
		KeyReleaseEvent(const KeyReleaseEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyReleaseEvent& operator=(const KeyReleaseEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyReleaseEvent(KeyReleaseEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyReleaseEvent& operator=(KeyReleaseEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the KeyReleaseEvent.
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
		KeyTypeEvent(const KeyTypeEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyTypeEvent& operator=(const KeyTypeEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyTypeEvent(KeyTypeEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyTypeEvent& operator=(KeyTypeEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;
		/// <summary>
		/// Retrieve the entered Unicode code point.
		/// </summary>
		/// <returns>Unicode code point.</returns>
		uint32_t GetCodePoint() const;

		/// <summary>
		/// Get a string representation of the KeyTypeEvent.
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
		/// <summary>
		/// Encode a single Unicode code point to a UTF-8 string.
		/// </summary>
		/// <param name="codePoint">Unicode code point</param>
		/// <returns>UTF-8 string.</returns>
		static std::string EncodeUTF8(uint32_t codePoint);

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
		explicit KeyLayoutEvent(std::string layout);
		/// <summary>
		/// Destructor.
		/// </summary>
		~KeyLayoutEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		KeyLayoutEvent(const KeyLayoutEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		KeyLayoutEvent& operator=(const KeyLayoutEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		KeyLayoutEvent(KeyLayoutEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		KeyLayoutEvent& operator=(KeyLayoutEvent&&) = default;

		/// <summary>
		/// Retrieve the human-readable name of the new keyboard layout.
		/// </summary>
		/// <returns>Name of new keyboard layout.</returns>
		const std::string& GetLayout() const;

		/// <summary>
		/// Get a string representation of the KeyLayoutEvent.
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
		std::string m_layout;
	};
}

#endif /*TRAP_KEYEVENT_H*/