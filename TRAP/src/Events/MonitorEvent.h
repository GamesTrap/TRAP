#ifndef TRAP_MONITOREVENT_H
#define TRAP_MONITOREVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Event.h"
#include "Window/Monitor.h"

namespace TRAP::Events
{
	/// @brief Monitor event base class.
	/// @remark @headless This class is not available in headless mode.
	class MonitorEvent : public Event
	{
	public:
		/// @brief Copy constructor.
		consteval MonitorEvent(const MonitorEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MonitorEvent& operator=(const MonitorEvent&) noexcept = delete;

		/// @brief Destructor.
		constexpr ~MonitorEvent() override = default;

		/// @brief Retrieve the affected monitor.
		/// @return Monitor.
		[[nodiscard]] Monitor GetMonitor() const noexcept;

		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// @brief Constructor.
		/// @param monitor Affected monitor.
		explicit constexpr MonitorEvent(const Monitor& monitor) noexcept;
		/// @brief Move constructor.
		constexpr MonitorEvent(MonitorEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MonitorEvent& operator=(MonitorEvent&&) noexcept = default;

	private:
		Monitor m_monitor;
	};

	/// @brief Monitor connected.
	/// @remark @headless This class is not available in headless mode.
	class MonitorConnectEvent final : public MonitorEvent
	{
	public:
		/// @brief Constructor.
		/// @param monitor Newly connected monitor.
		explicit constexpr MonitorConnectEvent(const Monitor& monitor) noexcept;
		/// @brief Destructor.
		constexpr ~MonitorConnectEvent() override = default;
		/// @brief Copy constructor.
		consteval MonitorConnectEvent(const MonitorConnectEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MonitorConnectEvent& operator=(const MonitorConnectEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MonitorConnectEvent(MonitorConnectEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MonitorConnectEvent& operator=(MonitorConnectEvent&&) noexcept = default;

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
	};

	/// @brief Monitor disconnected.
	/// @remark @headless This class is not available in headless mode.
	class MonitorDisconnectEvent final : public MonitorEvent
	{
	public:
		/// @brief Constructor.
		/// @param monitor Disconnected monitor.
		explicit constexpr MonitorDisconnectEvent(const Monitor& monitor) noexcept;
		/// @brief Destructor.
		constexpr ~MonitorDisconnectEvent() override = default;
		/// @brief Copy constructor.
		consteval MonitorDisconnectEvent(const MonitorDisconnectEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MonitorDisconnectEvent& operator=(const MonitorDisconnectEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MonitorDisconnectEvent(MonitorDisconnectEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MonitorDisconnectEvent& operator=(MonitorDisconnectEvent&&) noexcept = default;

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
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MonitorEvent::MonitorEvent(const Monitor& monitor) noexcept
	: m_monitor(monitor)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MonitorEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MonitorConnectEvent::MonitorConnectEvent(const Monitor& monitor) noexcept
	: MonitorEvent(monitor)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MonitorConnectEvent::GetStaticType() noexcept
{
	return EventType::MonitorConnect;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MonitorConnectEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MonitorConnectEvent::GetName() const
{
	return "MonitorConnect";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MonitorDisconnectEvent::MonitorDisconnectEvent(const Monitor& monitor) noexcept
	: MonitorEvent(monitor)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MonitorDisconnectEvent::GetStaticType() noexcept
{
	return EventType::MonitorDisconnect;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MonitorDisconnectEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MonitorDisconnectEvent::GetName() const
{
	return "MonitorDisconnect";
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_MONITOREVENT_H*/
