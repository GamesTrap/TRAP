#include "TRAPPCH.h"
#include "ApplicationEvent.h"

TRAP::EventType TRAP::AppTickEvent::GetStaticType()
{
	return EventType::AppTick;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::AppTickEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::AppTickEvent::GetName() const
{
	return "AppTick";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::AppTickEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::AppUpdateEvent::GetStaticType()
{
	return EventType::AppUpdate;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::AppUpdateEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::AppUpdateEvent::GetName() const
{
	return "AppUpdate";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::AppUpdateEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::AppRenderEvent::GetStaticType()
{
	return EventType::AppRender;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::EventType TRAP::AppRenderEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::AppRenderEvent::GetName() const
{
	return "AppRender";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::AppRenderEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Application);
}