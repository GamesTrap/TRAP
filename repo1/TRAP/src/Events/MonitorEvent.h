#ifndef _TRAP_MONITOREVENT_H_
#define _TRAP_MONITOREVENT_H_

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

#endif /*_TRAP_MONITOREVENT_H_*/