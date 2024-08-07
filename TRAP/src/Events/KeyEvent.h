#ifndef TRAP_KEYEVENT_H
#define TRAP_KEYEVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Input/Input.h"
#include "Event.h"

namespace TRAP::Events
{
	/// @brief Key event base class.
	/// @remark @headless This class is not available in headless mode.
	class KeyEvent : public Event
	{
	public:
		/// @brief Copy constructor.
		consteval KeyEvent(const KeyEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval KeyEvent& operator=(const KeyEvent&) noexcept = delete;

		/// @brief Destructor.
		constexpr ~KeyEvent() override = default;

		/// @brief Retrieve the affected key.
		/// @return Key.
		[[nodiscard]] constexpr Input::Key GetKey() const noexcept;

		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// @brief Constructor.
		/// @param key Affected key.
		explicit constexpr KeyEvent(Input::Key key) noexcept;
		/// @brief Move constructor.
		constexpr KeyEvent(KeyEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr KeyEvent& operator=(KeyEvent&&) noexcept = default;

	private:
		Input::Key m_key;
	};

	/// @brief Key pressed event.
	/// @remark @headless This class is not available in headless mode.
	class KeyPressEvent final : public KeyEvent
	{
	public:
		/// @brief Constructor.
		/// @param key Pressed key.
		/// @param window The affected window.
		/// @note The window must outlive the KeyPressEvent!
		KeyPressEvent(Input::Key key, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~KeyPressEvent() override = default;
		/// @brief Copy constructor.
		consteval KeyPressEvent(const KeyPressEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval KeyPressEvent& operator=(const KeyPressEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr KeyPressEvent(KeyPressEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		KeyPressEvent& operator=(KeyPressEvent&&) noexcept = default;

		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;

		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Key repeated event.
	/// @remark @headless This class is not available in headless mode.
	class KeyRepeatEvent final : public KeyEvent
	{
	public:
		/// @brief Constructor.
		/// @param key Repeated key.
		/// @param window The affected window.
		/// @note The window must outlive the KeyRepeatEvent!
		KeyRepeatEvent(Input::Key key, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~KeyRepeatEvent() override = default;
		/// @brief Copy constructor.
		consteval KeyRepeatEvent(const KeyRepeatEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval KeyRepeatEvent& operator=(const KeyRepeatEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr KeyRepeatEvent(KeyRepeatEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		KeyRepeatEvent& operator=(KeyRepeatEvent&&) noexcept = default;

		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Key released.
	/// @remark @headless This class is not available in headless mode.
	class KeyReleaseEvent final : public KeyEvent
	{
	public:
		/// @brief Constructor.
		/// @param key Released key.
		/// @param window The affected window.
		/// @note The window must outlive the KeyReleaseEvent!
		KeyReleaseEvent(Input::Key key, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~KeyReleaseEvent() override = default;
		/// @brief Copy constructor.
		consteval KeyReleaseEvent(const KeyReleaseEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval KeyReleaseEvent& operator=(const KeyReleaseEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr KeyReleaseEvent(KeyReleaseEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		KeyReleaseEvent& operator=(KeyReleaseEvent&&) noexcept = default;

		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Key typed.
	/// @remark @headless This class is not available in headless mode.
	class KeyTypeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param codePoint Unicode code point entered.
		/// @param window The affected window.
		/// @note The window must outlive the KeyTypeEvent!
		explicit KeyTypeEvent(u32 codePoint, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~KeyTypeEvent() override = default;
		/// @brief Copy constructor.
		consteval KeyTypeEvent(const KeyTypeEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval KeyTypeEvent& operator=(const KeyTypeEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr KeyTypeEvent(KeyTypeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		KeyTypeEvent& operator=(KeyTypeEvent&&) noexcept = default;

		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;
		/// @brief Retrieve the entered Unicode code point.
		/// @return Unicode code point.
		[[nodiscard]] constexpr u32 GetCodePoint() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
		std::reference_wrapper<TRAP::Window> m_window;
		u32 m_codePoint;
	};

	/// @brief Key layout changed.
	/// @remark @headless This class is not available in headless mode.
	class KeyLayoutEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param layout New keyboard layout.
		constexpr explicit KeyLayoutEvent(std::string layout) noexcept;
		/// @brief Destructor.
		constexpr ~KeyLayoutEvent() override = default;
		/// @brief Copy constructor.
		consteval KeyLayoutEvent(const KeyLayoutEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval KeyLayoutEvent& operator=(const KeyLayoutEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr KeyLayoutEvent(KeyLayoutEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr KeyLayoutEvent& operator=(KeyLayoutEvent&&) noexcept = default;

		/// @brief Retrieve the human-readable name of the new keyboard layout.
		/// @return Name of new keyboard layout.
		[[nodiscard]] constexpr std::string GetLayout() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

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

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::KeyPressEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::KeyRepeatEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::KeyReleaseEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::KeyTypeEvent::GetWindow() const noexcept
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

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_KEYEVENT_H*/
