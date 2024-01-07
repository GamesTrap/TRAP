#ifndef TRAP_EVENT_H
#define TRAP_EVENT_H

#include <functional>
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
		None             = 0,
		Window           = BIT(0u),
		Input            = BIT(1u),
		Keyboard         = BIT(2u),
		Mouse            = BIT(3u),
		Controller       = BIT(4u),
		MouseButton      = BIT(5u),
		HotReload        = BIT(6u),
		FileSystemChange = BIT(7u)
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
		[[nodiscard]] virtual std::string ToString() const = 0;

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
		/// @note The event must outlive the event dispatcher!
		explicit constexpr EventDispatcher(Event& event) noexcept;
		/// @brief Destructor.
		constexpr ~EventDispatcher() = default;
		/// @brief Copy constructor.
		constexpr EventDispatcher(const EventDispatcher&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr EventDispatcher& operator=(const EventDispatcher&) noexcept = default;
		/// @brief Move constructor.
		constexpr EventDispatcher(EventDispatcher&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr EventDispatcher& operator=(EventDispatcher&&) noexcept = default;

		/// @brief Dispatch a specific event to a function.
		/// @tparam T Event to dispatch.
		/// @tparam F Function to call.
		/// @param func Function to call.
		/// @return True if the received event matches the event to dispatch, false otherwise.
		template<typename T, typename F>
		constexpr bool Dispatch(F&& func) const& noexcept;

		/// @brief Dispatch a specific event to a function.
		/// @tparam T Event to dispatch.
		/// @tparam F Function to call.
		/// @param func Function to call.
		/// @return True if the received event matches the event to dispatch, false otherwise.
		template<typename T, typename F>
		constexpr bool Dispatch(F&& func) const&& noexcept;

	private:
		std::reference_wrapper<Event> m_event;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventDispatcher::EventDispatcher(Event& event) noexcept
	: m_event(event)
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename F>
constexpr bool TRAP::Events::EventDispatcher::Dispatch(F&& func) const& noexcept
{
	static_assert(std::derived_from<T, TRAP::Events::Event>, "T must be derived from TRAP::Events::Event!");
	static_assert(std::is_invocable_r_v<bool, F, const T&>, "F must have the signature bool Foo(const T&)!");

	if (m_event.get().GetEventType() != T::GetStaticType() || m_event.get().Handled)
		return false;

	m_event.get().Handled |= std::invoke(std::forward<F>(func), static_cast<const T&>(m_event.get()));

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename F>
constexpr bool TRAP::Events::EventDispatcher::Dispatch(F&& func) const&& noexcept
{
	static_assert(std::derived_from<T, TRAP::Events::Event>, "T must be derived from TRAP::Events::Event!");
	static_assert(std::is_invocable_r_v<bool, F, const T&>, "F must have the signature bool Foo(const T&)!");

	if (m_event.get().GetEventType() != T::GetStaticType() || m_event.get().Handled)
		return false;

	m_event.get().Handled |= std::invoke(std::forward<F>(func), std::move(static_cast<const T&>(m_event.get())));

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

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

template<typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of_v<TRAP::Events::Event, T>, char>> : fmt::formatter<std::string>
{
    fmt::format_context::iterator format(const TRAP::Events::Event& event, fmt::format_context& ctx) const
    {
		return fmt::formatter<std::string>::format(event.ToString(), ctx);
    }
};

#endif /*TRAP_EVENT_H*/