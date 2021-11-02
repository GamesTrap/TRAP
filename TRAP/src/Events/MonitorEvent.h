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
		/// Virtual Default Destructor.
		/// </summary>
		~MonitorEvent() override = default;

		/// <summary>
		/// Retrieve the affected monitor.
		/// </summary>
		/// <returns>Monitor.</returns>
		Monitor GetMonitor() const;

		/// <summary>
		/// Retrieve the category flags of the Event.
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
		/// Default Copy Constructor.
		/// </summary>
		MonitorEvent(const MonitorEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MonitorEvent& operator=(const MonitorEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MonitorEvent(MonitorEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
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
		/// <param name="monitor">New connected monitor.</param>
		explicit MonitorConnectEvent(Monitor monitor);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~MonitorConnectEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		MonitorConnectEvent(const MonitorConnectEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MonitorConnectEvent& operator=(const MonitorConnectEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MonitorConnectEvent(MonitorConnectEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		MonitorConnectEvent& operator=(MonitorConnectEvent&&) = default;

		/// <summary>
		/// Get a string representation of the MonitorConnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
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
		/// Default Destructor.
		/// </summary>
		~MonitorDisconnectEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		MonitorDisconnectEvent(const MonitorDisconnectEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		MonitorDisconnectEvent& operator=(const MonitorDisconnectEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		MonitorDisconnectEvent(MonitorDisconnectEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		MonitorDisconnectEvent& operator=(MonitorDisconnectEvent&&) = default;

		/// <summary>
		/// Get a string representation of the MonitorDisconnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
	};
}

#endif /*TRAP_MONITOREVENT_H*/