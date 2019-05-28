#pragma once

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
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput       = BIT(1),
		EventCategoryKeyboard    = BIT(2),
		EventCategoryMouse       = BIT(3),
		EventCategoryMouseButton = BIT(4)
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
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(const EventCategory category) const
		{
			return GetCategoryFlags() & static_cast<int>(category);
		}
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		explicit EventDispatcher(Event& event)
			: m_event(event)
		{			
		}

		template<typename T>
		bool Dispatch(EventFn<T> func);

	private:
		Event& m_event;
	};

	template <typename T>
	bool EventDispatcher::Dispatch(EventFn<T> func)
	{
		if (m_event.GetEventType() == T::GetStaticType())
		{
			m_event.Handled = func(*static_cast<T*>(&m_event));

			return true;
		}

		return false;
	}

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}
