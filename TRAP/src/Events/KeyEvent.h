#ifndef _TRAP_KEYEVENT_H_
#define _TRAP_KEYEVENT_H_

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
		/// Virtual Default Destructor.
		/// </summary>
		virtual ~KeyEvent() = default;

		/// <summary>
		/// Retrieve the affected key.
		/// </summary>
		/// <returns>Key.</returns>
		Input::Key GetKey() const;

		/// <summary>
		/// Retrieve the category flags of the Event.
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
		/// Default Copy Constructor.
		/// </summary>
		KeyEvent(const KeyEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		KeyEvent& operator=(const KeyEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		KeyEvent(KeyEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		KeyEvent& operator=(KeyEvent&&) = default;

		/// <summary>
		/// Get a string representation for a non printable key (Space key for example).
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
		/// <param name="window">Pointer to the affected Window.</param>
		KeyPressEvent(Input::Key key, uint32_t repeatCount, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~KeyPressEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		KeyPressEvent(const KeyPressEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		KeyPressEvent& operator=(const KeyPressEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		KeyPressEvent(KeyPressEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		KeyPressEvent& operator=(KeyPressEvent&&) = default;

		/// <summary>
		/// Retrieve tha amount of key press repeats.
		/// </summary>
		/// <returns>Repeat count.</returns>
		uint32_t GetRepeatCount() const;
		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the KeyPressEvent.
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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit KeyReleaseEvent(Input::Key key, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~KeyReleaseEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		KeyReleaseEvent(const KeyReleaseEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		KeyReleaseEvent& operator=(const KeyReleaseEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		KeyReleaseEvent(KeyReleaseEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		KeyReleaseEvent& operator=(KeyReleaseEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the KeyReleaseEvent.
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
	/// Key typed.
	/// </summary>
	class KeyTypeEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="codePoint">Unicode code point entered.</param>
		/// <param name="window">Pointer to the affected Window.</param>
		explicit KeyTypeEvent(uint32_t codePoint, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~KeyTypeEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		KeyTypeEvent(const KeyTypeEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		KeyTypeEvent& operator=(const KeyTypeEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		KeyTypeEvent(KeyTypeEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		KeyTypeEvent& operator=(KeyTypeEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
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
		/// Default Destructor.
		/// </summary>
		~KeyLayoutEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		KeyLayoutEvent(const KeyLayoutEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		KeyLayoutEvent& operator=(const KeyLayoutEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		KeyLayoutEvent(KeyLayoutEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
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
		/// <returns></returns>
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
		std::string m_layout;
	};
}

#endif /*_TRAP_KEYEVENT_H_*/