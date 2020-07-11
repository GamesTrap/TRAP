#include "TRAPPCH.h"
#include "ApplicationEvent.h"

TRAP::Events::EventType TRAP::Events::AppTickEvent::GetStaticType()
{
	return EventType::AppTick;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::AppTickEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::AppTickEvent::GetName() const
{
	return "AppTick";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::AppTickEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::AppUpdateEvent::GetStaticType()
{
	return EventType::AppUpdate;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::AppUpdateEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::AppUpdateEvent::GetName() const
{
	return "AppUpdate";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::AppUpdateEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::AppRenderEvent::GetStaticType()
{
	return EventType::AppRender;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::AppRenderEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::AppRenderEvent::GetName() const
{
	return "AppRender";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::AppRenderEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}