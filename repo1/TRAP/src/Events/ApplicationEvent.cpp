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

TRAP::Events::EventCategory TRAP::Events::AppTickEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
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

TRAP::Events::EventCategory TRAP::Events::AppUpdateEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
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

TRAP::Events::EventCategory TRAP::Events::AppRenderEvent::GetCategoryFlags() const
{
	return EventCategory::Application;
}