#ifndef TRAP_EVENT_H
#define TRAP_EVENT_H

#include <string>

#include <fmt/ostream.h>

#include "Core/Base.h"

namespace TRAP::Events
{
	//TODO
	//Events in TRAP are currently blocking, meaning when an event occurs it immediately gets
	//dispatched and must be dealt with right then and there.
	//For the future, a better strategy might be to buffer events in an event bus and process
	//them during the "event" part of the update stage.

	/// <summary>
	/// Different event types.
	/// </summary>
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowMinimize, WindowMaximize, WindowRestore, WindowFocus, WindowLostFocus,
		WindowMove, WindowDrop, WindowContentScale, FrameBufferResize,
		KeyPress, KeyRepeat, KeyRelease, KeyType, KeyLayout,
		MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll, MouseEnter, MouseLeave,
		ControllerConnect, ControlledDisconnect,
		MonitorConnect, MonitorDisconnect,
		TextureReload, ShaderReload, FileChange
	};

	/// <summary>
	/// Categories for events.
	/// </summary>
	enum class EventCategory : uint32_t
	{
		None        = 0,
		Window      = BIT(0u),
		Input       = BIT(1u),
		Keyboard    = BIT(2u),
		Mouse       = BIT(3u),
		Controller  = BIT(4u),
		MouseButton = BIT(5u),
		HotReload   = BIT(6u),
		FileChange  = BIT(7u)
	};

	/// <summary>
	/// Event base class.
	/// </summary>
	class Event
	{
	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr Event() noexcept = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Event(const Event&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Event& operator=(const Event&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Event(Event&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr Event& operator=(Event&&) noexcept = default;

	public:
		/// <summary>
		/// Virtual destructor.
		/// </summary>
		constexpr virtual ~Event() = default;

		/// <summary>
		/// If set to true then this event won't be passed on to other event handlers.
		/// </summary>
		bool Handled = false;

		/// <summary>
		/// Get a string representation of the event.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr virtual std::string ToString() const;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] constexpr virtual EventType GetEventType() const noexcept = 0;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr virtual std::string GetName() const = 0;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr virtual EventCategory GetCategoryFlags() const noexcept = 0;

		/// <summary>
		/// Check if an event is in the specified category.
		/// </summary>
		/// <param name="category">Category to check.</param>
		/// <returns>True if event is in the category, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsInCategory(EventCategory category) const noexcept;
	};

	/// <summary>
	/// Dispatcher for events.
	/// </summary>
	class EventDispatcher
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="event">Event to dispatch.</param>
		explicit constexpr EventDispatcher(Event& event) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~EventDispatcher() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr EventDispatcher(const EventDispatcher&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr EventDispatcher& operator=(const EventDispatcher&) noexcept = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr EventDispatcher(EventDispatcher&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr EventDispatcher& operator=(EventDispatcher&&) noexcept = delete;

		/// <summary>
		/// Dispatch a specific event to a function.
		/// </summary>
		/// <typeparam name="T">Event to dispatch.</typeparam>
		/// <typeparam name="F">Function to call.</typeparam>
		/// <param name="func">Function to call.</param>
		/// <returns>True if the received event matches the event to dispatch, false otherwise.</returns>
		template<typename T, typename F>
		constexpr bool Dispatch(const F& func) noexcept;


		/// <summary>
		/// Dispatch a specific event to a member function.
		/// </summary>
		/// <typeparam name="T">Event to dispatch.</typeparam>
		/// <typeparam name="ClassType">Pointer to class from which to call member function.</typeparam>
		/// <typeparam name="F">Member function to call.</typeparam>
		/// <param name="func">Member function to call.</param>
		/// <returns>True if the received event matches the event to dispatch, false otherwise.</returns>
		template<typename T, typename ClassType, typename F>
		constexpr bool Dispatch(ClassType* obj, const F& func) noexcept;

	private:
		Event& m_event;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::Event::ToString() const
{
	return GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventDispatcher::EventDispatcher(Event& event) noexcept
	: m_event(event)
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename F>
constexpr bool TRAP::Events::EventDispatcher::Dispatch(const F& func) noexcept
{
	if (m_event.GetEventType() != T::GetStaticType() || m_event.Handled)
		return false;

	m_event.Handled = func(static_cast<T&>(m_event));

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename ClassType, typename F>
constexpr bool TRAP::Events::EventDispatcher::Dispatch(ClassType* const obj, const F& func) noexcept
{
	if (m_event.GetEventType() != T::GetStaticType() || m_event.Handled)
		return false;

	m_event.Handled = (obj->*func)(static_cast<T&>(m_event));

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e);

MAKE_ENUM_FLAG(TRAP::Events::EventCategory)

[[nodiscard]] constexpr TRAP::Events::EventCategory operator ^(const TRAP::Events::EventCategory lhs,
                                                               const TRAP::Events::EventCategory rhs) noexcept
{
	return static_cast<TRAP::Events::EventCategory>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
}
[[nodiscard]] constexpr TRAP::Events::EventCategory operator ~(const TRAP::Events::EventCategory rhs) noexcept
{
	return static_cast<TRAP::Events::EventCategory>(~std::to_underlying(rhs));
}
[[nodiscard]] constexpr TRAP::Events::EventCategory& operator ^=(TRAP::Events::EventCategory& lhs,
                                                                 const TRAP::Events::EventCategory rhs) noexcept
{
	lhs = lhs ^ rhs;

	return lhs;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Events::Event::IsInCategory(const EventCategory category) const noexcept
{
	return (GetCategoryFlags() & category) != EventCategory::None;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Events::Event> : fmt::ostream_formatter
{};

#endif /*TRAP_EVENT_H*/