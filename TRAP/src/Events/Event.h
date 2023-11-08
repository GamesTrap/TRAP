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

	/// @brief Different event types.
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

	/// @brief Categories for events.
	enum class EventCategory : u32
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

	/// @brief Event base class.
	class Event
	{
	protected:
		/// @brief Constructor.
		constexpr Event() noexcept = default;
		/// @brief Move constructor.
		constexpr Event(Event&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr Event& operator=(Event&&) noexcept = default;

	public:
		/// @brief Copy constructor.
		consteval Event(const Event&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Event& operator=(const Event&) noexcept = delete;

		/// @brief Virtual destructor.
		constexpr virtual ~Event() = default;

		/// @brief If set to true then this event won't be passed on to other event handlers.
		bool Handled = false;

		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] constexpr virtual std::string ToString() const;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr virtual EventType GetEventType() const noexcept = 0;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr virtual std::string GetName() const = 0;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr virtual EventCategory GetCategoryFlags() const noexcept = 0;

		/// @brief Check if an event is in the specified category.
		/// @param category Category to check.
		/// @return True if event is in the category, false otherwise.
		[[nodiscard]] constexpr bool IsInCategory(EventCategory category) const noexcept;
	};

	/// @brief Dispatcher for events.
	class EventDispatcher
	{
	public:
		/// @brief Constructor.
		/// @param event Event to dispatch.
		explicit constexpr EventDispatcher(Event& event) noexcept;
		/// @brief Destructor.
		constexpr ~EventDispatcher() = default;
		/// @brief Copy constructor.
		consteval EventDispatcher(const EventDispatcher&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval EventDispatcher& operator=(const EventDispatcher&) noexcept = delete;
		/// @brief Move constructor.
		constexpr EventDispatcher(EventDispatcher&&) noexcept = default;
		/// @brief Move assignment operator.
		consteval EventDispatcher& operator=(EventDispatcher&&) noexcept = delete;

		/// @brief Dispatch a specific event to a function.
		/// @tparam T Event to dispatch.
		/// @tparam F Function to call.
		/// @param func Function to call.
		/// @return True if the received event matches the event to dispatch, false otherwise.
		template<typename T, typename F>
		requires std::is_invocable_r_v<bool, F, T&>
		constexpr bool Dispatch(const F& func) noexcept;

		/// @brief Dispatch a specific event to a member function.
		/// @tparam T Event to dispatch.
		/// @tparam ClassType Class to use member function from.
		/// @tparam F Member function to call.
		/// @param obj Pointer to class from which to call member function.
		/// @param func Member function to call.
		/// @return True if the received event matches the event to dispatch, false otherwise.
		template<typename T, typename ClassType, typename F>
		requires std::is_class_v<ClassType> && std::is_member_function_pointer_v<F> &&
		         std::is_invocable_r_v<bool, typename std::remove_pointer_t<F>, ClassType*, T&>
		bool Dispatch(ClassType* obj, const F& func) noexcept;

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
requires std::is_invocable_r_v<bool, F, T&>
constexpr bool TRAP::Events::EventDispatcher::Dispatch(const F& func) noexcept
{
	if (m_event.GetEventType() != T::GetStaticType() || m_event.Handled)
		return false;

	m_event.Handled = func(static_cast<T&>(m_event));

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename ClassType, typename F>
requires std::is_class_v<ClassType> && std::is_member_function_pointer_v<F> &&
         std::is_invocable_r_v<bool, typename std::remove_pointer_t<F>, ClassType*, T&>
inline bool TRAP::Events::EventDispatcher::Dispatch(ClassType* obj, const F& func) noexcept
{
	TRAP_ASSERT(obj, "EventDispatcher::Dispatch(): obj is nullptr!");

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