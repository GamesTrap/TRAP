#ifndef _TRAP_APPLICATIONEVENT_H_
#define _TRAP_APPLICATIONEVENT_H_

#include "Event.h"

namespace TRAP
{
	//Unused for now
	class AppTickEvent final : public Event
	{
	public:
		AppTickEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
	};

	//Unused for now
	class AppUpdateEvent final : public Event
	{
	public:
		AppUpdateEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
	};
	
	//Unused for now
	class AppRenderEvent final : public Event
	{
	public:
		AppRenderEvent() = default;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
	};
}

#endif /*_TRAP_APPLICATIONEVENT_H_*/