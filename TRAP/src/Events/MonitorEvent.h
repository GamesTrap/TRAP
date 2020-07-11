#ifndef _TRAP_MONITOREVENT_H_
#define _TRAP_MONITOREVENT_H_

#include "Event.h"
#include "Window/Monitor.h"

namespace TRAP::Events
{	
	class MonitorEvent : public Event
	{
	public:
		Monitor GetMonitor() const;

		int32_t GetCategoryFlags() const override;

	protected:
		explicit MonitorEvent(Monitor monitor);

		Monitor m_monitor;
	};

	class MonitorConnectEvent final : public MonitorEvent
	{
	public:
		explicit MonitorConnectEvent(Monitor monitor);

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
	};

	class MonitorDisconnectEvent final : public MonitorEvent
	{
	public:
		explicit MonitorDisconnectEvent(Monitor monitor);

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
	};
}

#endif /*_TRAP_MONITOREVENT_H_*/