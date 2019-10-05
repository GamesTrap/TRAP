#ifndef _TRAP_EVENT_H_
#define _TRAP_EVENT_H_

namespace TRAP
{
	//TODO
	//Events in TRAP are currently blocking, meaning when an event occurs it immediately gets dispatched and must be dealt with right then and there.
	//For the future, a better strategy might be to buffer events in an event bus and process them during the "event" part of the update stage.	

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum class EventCategory
	{
		None        = 0,
		Application = BIT(0),
		Input       = BIT(1),
		Keyboard    = BIT(2),
		Mouse       = BIT(3),
		MouseButton = BIT(4)
	};

	class Event
	{
	public:
		Event() = default;
		Event(const Event&) = default;
		Event& operator=(const Event&) = default;
		Event(Event&&) = default;
		Event& operator=(Event&&) = default;
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int32_t GetCategoryFlags() const = 0;
		virtual std::string ToString() const;

		bool IsInCategory(EventCategory category) const;
	};

	class EventDispatcher
	{
	public:
		explicit EventDispatcher(Event& event)
			: m_event(event)
		{			
		}

		template<typename T, typename F>
		bool Dispatch(const F& func);

	private:
		Event& m_event;
	};

	template <typename T, typename F>
	bool EventDispatcher::Dispatch(const F& func)
	{
		if (m_event.GetEventType() == T::GetStaticType())
		{
			m_event.Handled = func(static_cast<T&>(m_event));

			return true;
		}

		return false;
	}

	std::ostream& operator<<(std::ostream& os, const Event& e);
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::ostream& TRAP::operator<<(std::ostream& os, const Event& e)
{
	return os << e.ToString();
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::Event::ToString() const
{
	return GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::Event::IsInCategory(const EventCategory category) const
{
	return GetCategoryFlags() & static_cast<int32_t>(category);
}

#endif /*_TRAP_EVENT_H_*/