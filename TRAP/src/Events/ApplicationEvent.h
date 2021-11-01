#ifndef _TRAP_APPLICATIONEVENT_H_
#define _TRAP_APPLICATIONEVENT_H_

#include "Event.h"

namespace TRAP::Events
{
	/// <summary>
	/// Application tick.<br>
	/// Note: Unused for now!
	/// </summary>
	class AppTickEvent final : public Event
	{
	public:
		/// <summary>
		/// Default Constructor.
		/// </summary>
		AppTickEvent() = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~AppTickEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		AppTickEvent(const AppTickEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		AppTickEvent& operator=(const AppTickEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		AppTickEvent(AppTickEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		AppTickEvent& operator=(AppTickEvent&&) = default;

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
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;
	};

	/// <summary>
	/// Application update.<br>
	/// Note: Unused for now!
	/// </summary>
	class AppUpdateEvent final : public Event
	{
	public:
		/// <summary>
		/// Default Constructor.
		/// </summary>
		AppUpdateEvent() = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~AppUpdateEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		AppUpdateEvent(const AppUpdateEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		AppUpdateEvent& operator=(const AppUpdateEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		AppUpdateEvent(AppUpdateEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		AppUpdateEvent& operator=(AppUpdateEvent&&) = default;

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
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;
	};

	/// <summary>
	/// Application render.<br>
	/// Note: Unused for now!
	/// </summary>
	class AppRenderEvent final : public Event
	{
	public:
		/// <summary>
		/// Default Constructor.
		/// </summary>
		AppRenderEvent() = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~AppRenderEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		AppRenderEvent(const AppRenderEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		AppRenderEvent& operator=(const AppRenderEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		AppRenderEvent(AppRenderEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		AppRenderEvent& operator=(AppRenderEvent&&) = default;

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
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;
	};
}

#endif /*_TRAP_APPLICATIONEVENT_H_*/