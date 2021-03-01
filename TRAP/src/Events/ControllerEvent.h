#ifndef _TRAP_CONTROLLEREVENT_H_
#define _TRAP_CONTROLLEREVENT_H_

#include "Event.h"
#include "Input/Input.h"

namespace TRAP::Events
{
	/// <summary>
	/// Controller event base class.
	/// </summary>
	class ControllerEvent : public Event
	{
	public:
		/// <summary>
		/// Retrieve the affected Controller.
		/// </summary>
		/// <returns>Controller.</returns>
		Input::Controller GetController() const;

		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="controller">Affected Controller.</param>
		explicit ControllerEvent(Input::Controller controller);

		Input::Controller m_controller;
	};

	/// <summary>
	/// Controller connected.
	/// </summary>
	class ControllerConnectEvent final : public ControllerEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="controller">Newly connected Controller.</param>
		explicit ControllerConnectEvent(Input::Controller controller);

		/// <summary>
		/// Get a string representation of the ControllerConnectEvent.
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
	/// Controller disconnected.
	/// </summary>
	class ControllerDisconnectEvent final : public ControllerEvent
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="controller">Disconnected Controller.</param>
		explicit ControllerDisconnectEvent(Input::Controller controller);

		/// <summary>
		/// Get a string representation of the ControllerDisconnectEvent.
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

#endif /*_TRAP_CONTROLLEREVENT_H_*/