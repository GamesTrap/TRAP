#ifndef _TRAP_CONTROLLEREVENT_H_
#define _TRAP_CONTROLLEREVENT_H_

#include "Event.h"
#include "Input/Input.h"

namespace TRAP::Events
{
	class ControllerEvent : public Event
	{
	public:
		Input::Controller GetController() const;

		int32_t GetCategoryFlags() const override;

	protected:
		explicit ControllerEvent(Input::Controller controller);
		
		Input::Controller m_controller;
	};
	
	class ControllerConnectEvent final : public ControllerEvent
	{
	public:
		explicit ControllerConnectEvent(Input::Controller controller);
		
		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
	};

	class ControllerDisconnectEvent final : public ControllerEvent
	{
	public:
		explicit ControllerDisconnectEvent(Input::Controller controller);
		
		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
	};
}

#endif /*_TRAP_CONTROLLEREVENT_H_*/