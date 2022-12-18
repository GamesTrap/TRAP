#ifndef TRAP_EVENT_H
#define TRAP_EVENT_H

#include <string>

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
		KeyPress, KeyRelease, KeyType, KeyLayout,
		MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll, MouseEnter, MouseLeave,
		ControllerConnect, ControlledDisconnect,
		MonitorConnect, MonitorDisconnect,
		TextureReload, ShaderReload, FileChange
	};

	/// <summary>
	/// Categories for events.
	/// </summary>
	enum class EventCategory
	{
		None        = 0,
		Window      = BIT(0),
		Input       = BIT(1),
		Keyboard    = BIT(2),
		Mouse       = BIT(3),
		Controller  = BIT(4),
		MouseButton = BIT(5),
		HotReload   = BIT(6),
		FileChange  = BIT(7)
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
		Event() noexcept = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Event(const Event&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Event& operator=(const Event&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Event(Event&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Event& operator=(Event&&) noexcept = default;

	public:
		/// <summary>
		/// Virtual destructor.
		/// </summary>
		virtual ~Event() = default;

		/// <summary>
		/// If set to true then this event won't be passed on to other event handlers.
		/// </summary>
		bool Handled = false;

		/// <summary>
		/// Get a string representation of the event.
		/// </summary>
		/// <returns>String representation.</returns>
		virtual std::string ToString() const;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		virtual EventType GetEventType() const = 0;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		virtual std::string GetName() const = 0;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		virtual EventCategory GetCategoryFlags() const = 0;

		/// <summary>
		/// Check if an event is in the specified category.
		/// </summary>
		/// <param name="category">Category to check.</param>
		/// <returns>True if event is in the category, false otherwise.</returns>
		bool IsInCategory(EventCategory category) const;
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
		~EventDispatcher() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		EventDispatcher(const EventDispatcher&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		EventDispatcher& operator=(const EventDispatcher&) noexcept = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		EventDispatcher(EventDispatcher&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		EventDispatcher& operator=(EventDispatcher&&) noexcept = delete;

		/// <summary>
		/// Dispatch a specific event to a function.
		/// </summary>
		/// <typeparam name="T">Event to dispatch.</typeparam>
		/// <typeparam name="F">Function to call.</typeparam>
		/// <param name="func">Function to call.</param>
		/// <returns>True if the received event matches the event to dispatch, false otherwise.</returns>
		template<typename T, typename F>
		constexpr bool Dispatch(const F& func);

	private:
		Event& m_event;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventDispatcher::EventDispatcher(Event& event) noexcept
	: m_event(event)
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename F>
constexpr bool TRAP::Events::EventDispatcher::Dispatch(const F& func)
{
	if (m_event.GetEventType() != T::GetStaticType() || m_event.Handled)
		return false;

	m_event.Handled = func(static_cast<T&>(m_event));

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e);

MAKE_ENUM_FLAG(TRAP::Events::EventCategory)

constexpr TRAP::Events::EventCategory operator ^(const TRAP::Events::EventCategory lhs,
                                                 const TRAP::Events::EventCategory rhs)
{
	return static_cast<TRAP::Events::EventCategory>
		(
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(lhs) ^
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);
}
constexpr TRAP::Events::EventCategory operator ~(const TRAP::Events::EventCategory rhs)
{
	return static_cast<TRAP::Events::EventCategory>
		(
			~static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);
}
constexpr TRAP::Events::EventCategory& operator ^=(TRAP::Events::EventCategory& lhs,
                                                   const TRAP::Events::EventCategory rhs)
{
	lhs = static_cast<TRAP::Events::EventCategory>
		(
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(lhs) ^
			static_cast<std::underlying_type<TRAP::Events::EventCategory>::type>(rhs)
		);

	return lhs;
}

#endif /*TRAP_EVENT_H*/