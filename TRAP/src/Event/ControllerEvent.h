#ifndef _TRAP_CONTROLLEREVENT_H_
#define _TRAP_CONTROLLEREVENT_H_

#include "Event.h"
#include "Input/Input.h"

namespace TRAP
{
	class ControllerConnectEvent final : public Event
	{
	public:
		explicit ControllerConnectEvent(TRAP::Input::Controller controller);

		TRAP::Input::Controller GetController() const;
		
		std::string ToString() const override;

		int32_t GetCategoryFlags() const override;
		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		TRAP::Input::Controller m_controller;
	};

	class ControllerDisconnectEvent final : public Event
	{
	public:
		explicit ControllerDisconnectEvent(TRAP::Input::Controller controller);

		TRAP::Input::Controller GetController() const;
		
		std::string ToString() const override;

		int32_t GetCategoryFlags() const override;
		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		TRAP::Input::Controller m_controller;
	};
}

#endif /*_TRAP_CONTROLLEREVENT_H_*/