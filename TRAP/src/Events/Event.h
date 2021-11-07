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
		AppTick, AppUpdate, AppRender,
		KeyPress, KeyRelease, KeyType, KeyLayout,
		MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll, MouseEnter, MouseLeave,
		ControllerConnect, ControlledDisconnect,
		MonitorConnect, MonitorDisconnect,
		TextureReload, ShaderReload
	};

	/// <summary>
	/// Category's for events.
	/// </summary>
	enum class EventCategory
	{
		None        = 0,
		Application = BIT(0),
		Input       = BIT(1),
		Keyboard    = BIT(2),
		Mouse       = BIT(3),
		Controller  = BIT(4),
		MouseButton = BIT(5),
		HotReload   = BIT(6)
	};

	/// <summary>
	/// Event base class.
	/// </summary>
	class Event
	{
	protected:
		/// <summary>
		/// Default Constructor.
		/// </summary>
		Event() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Event(const Event&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Event& operator=(const Event&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Event(Event&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Event& operator=(Event&&) = default;

	public:
		/// <summary>
		/// Virtual Default Destructor.
		/// </summary>
		virtual ~Event() = default;

		bool Handled = false;

		/// <summary>
		/// Get a string representation of the Event.
		/// </summary>
		/// <returns>String representation.</returns>
		virtual std::string ToString() const;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		virtual EventType GetEventType() const = 0;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		virtual const char* GetName() const = 0;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		virtual EventCategory GetCategoryFlags() const = 0;

		/// <summary>
		/// Check if an Event is in the specified category.
		/// </summary>
		/// <param name="category">Category to check.</param>
		/// <returns>True if Event is in the category, false otherwise.</returns>
		bool IsInCategory(EventCategory category) const;
	};

	/// <summary>
	/// Dispatcher for Events.
	/// </summary>
	class EventDispatcher
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="event">Event to dispatch.</param>
		explicit EventDispatcher(Event& event);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~EventDispatcher() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		EventDispatcher(const EventDispatcher&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		EventDispatcher& operator=(const EventDispatcher&) = delete;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		EventDispatcher(EventDispatcher&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		EventDispatcher& operator=(EventDispatcher&&) = delete;

		/// <summary>
		/// Dispatch a specific Event to a function.
		/// </summary>
		/// <typeparam name="T">Event to dispatch.</typeparam>
		/// <typeparam name="F">Function to call.</typeparam>
		/// <param name="func">Function to call.</param>
		/// <returns>True if received Event matches the Event to dispatch, false otherwise.</returns>
		template<typename T, typename F>
		bool Dispatch(const F& func);

	private:
		Event& m_event;
	};

	template <typename T, typename F>
	bool EventDispatcher::Dispatch(const F& func)
	{
		if (m_event.GetEventType() != T::GetStaticType())
			return false;

		m_event.Handled |= func(static_cast<T&>(m_event));

		return true;
	}

}

std::ostream& operator<<(std::ostream& os, const TRAP::Events::Event& e);

MAKE_ENUM_FLAG(TRAP::Events::EventCategory)

TRAP::Events::EventCategory operator ^(TRAP::Events::EventCategory lhs, TRAP::Events::EventCategory rhs);
TRAP::Events::EventCategory operator ~(TRAP::Events::EventCategory rhs);
TRAP::Events::EventCategory& operator ^=(TRAP::Events::EventCategory& lhs, TRAP::Events::EventCategory rhs);

#endif /*TRAP_EVENT_H*/