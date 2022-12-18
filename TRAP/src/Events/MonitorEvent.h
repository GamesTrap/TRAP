#ifndef TRAP_MONITOREVENT_H
#define TRAP_MONITOREVENT_H

#include "Event.h"
#include "Window/Monitor.h"

namespace TRAP::Events
{
	/// <summary>
	/// Monitor event base class.
	/// </summary>
	class MonitorEvent : public Event
	{
	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		~MonitorEvent() override = default;

		/// <summary>
		/// Retrieve the affected monitor.
		/// </summary>
		/// <returns>Monitor.</returns>
		Monitor GetMonitor() const;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="monitor">Affected monitor.</param>
		explicit MonitorEvent(Monitor monitor) noexcept;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MonitorEvent(const MonitorEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MonitorEvent& operator=(const MonitorEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MonitorEvent(MonitorEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MonitorEvent& operator=(MonitorEvent&&) noexcept = default;

		Monitor m_monitor;
	};

	/// <summary>
	/// Monitor connected.
	/// </summary>
	class MonitorConnectEvent final : public MonitorEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="monitor">Newly connected monitor.</param>
		explicit MonitorConnectEvent(Monitor monitor)noexcept ;
		/// <summary>
		/// Destructor.
		/// </summary>
		~MonitorConnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MonitorConnectEvent(const MonitorConnectEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MonitorConnectEvent& operator=(const MonitorConnectEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MonitorConnectEvent(MonitorConnectEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MonitorConnectEvent& operator=(MonitorConnectEvent&&) noexcept = default;

		/// <summary>
		/// Get a string representation of the MonitorConnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
	};

	/// <summary>
	/// Monitor disconnected.
	/// </summary>
	class MonitorDisconnectEvent final : public MonitorEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="monitor">Disconnected monitor.</param>
		explicit MonitorDisconnectEvent(Monitor monitor) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~MonitorDisconnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MonitorDisconnectEvent(const MonitorDisconnectEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MonitorDisconnectEvent& operator=(const MonitorDisconnectEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MonitorDisconnectEvent(MonitorDisconnectEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MonitorDisconnectEvent& operator=(MonitorDisconnectEvent&&) noexcept = default;

		/// <summary>
		/// Get a string representation of the MonitorDisconnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::MonitorConnectEvent::GetStaticType()
{
	return EventType::MonitorConnect;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::MonitorDisconnectEvent::GetStaticType()
{
	return EventType::MonitorDisconnect;
}

#endif /*TRAP_MONITOREVENT_H*/