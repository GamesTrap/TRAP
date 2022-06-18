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
		explicit MonitorEvent(Monitor monitor);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MonitorEvent(const MonitorEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MonitorEvent& operator=(const MonitorEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MonitorEvent(MonitorEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MonitorEvent& operator=(MonitorEvent&&) = default;

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
		explicit MonitorConnectEvent(Monitor monitor);
		/// <summary>
		/// Destructor.
		/// </summary>
		~MonitorConnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MonitorConnectEvent(const MonitorConnectEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MonitorConnectEvent& operator=(const MonitorConnectEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MonitorConnectEvent(MonitorConnectEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MonitorConnectEvent& operator=(MonitorConnectEvent&&) = default;

		/// <summary>
		/// Get a string representation of the MonitorConnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
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
		explicit MonitorDisconnectEvent(Monitor monitor);
		/// <summary>
		/// Destructor.
		/// </summary>
		~MonitorDisconnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		MonitorDisconnectEvent(const MonitorDisconnectEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		MonitorDisconnectEvent& operator=(const MonitorDisconnectEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		MonitorDisconnectEvent(MonitorDisconnectEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		MonitorDisconnectEvent& operator=(MonitorDisconnectEvent&&) = default;

		/// <summary>
		/// Get a string representation of the MonitorDisconnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
	};
}

#endif /*TRAP_MONITOREVENT_H*/