#ifndef TRAP_CONTROLLEREVENT_H
#define TRAP_CONTROLLEREVENT_H

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
		/// Destructor.
		/// </summary>
		~ControllerEvent() override = default;

		/// <summary>
		/// Retrieve the affected controller.
		/// </summary>
		/// <returns>Controller.</returns>
		constexpr Input::Controller GetController() const;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="controller">Affected controller.</param>
		explicit ControllerEvent(Input::Controller controller);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ControllerEvent(const ControllerEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ControllerEvent& operator=(const ControllerEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ControllerEvent(ControllerEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ControllerEvent& operator=(ControllerEvent&&) = default;

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
		/// <param name="controller">Newly connected controller.</param>
		explicit ControllerConnectEvent(Input::Controller controller);
		/// <summary>
		/// Destructor.
		/// </summary>
		~ControllerConnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ControllerConnectEvent(const ControllerConnectEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ControllerConnectEvent& operator=(const ControllerConnectEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ControllerConnectEvent(ControllerConnectEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ControllerConnectEvent& operator=(ControllerConnectEvent&&) = default;

		/// <summary>
		/// Get a string representation of the ControllerConnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
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
		/// <param name="controller">Disconnected controller.</param>
		explicit ControllerDisconnectEvent(Input::Controller controller);
		/// <summary>
		/// Destructor.
		/// </summary>
		~ControllerDisconnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ControllerDisconnectEvent(const ControllerDisconnectEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ControllerDisconnectEvent& operator=(const ControllerDisconnectEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ControllerDisconnectEvent(ControllerDisconnectEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ControllerDisconnectEvent& operator=(ControllerDisconnectEvent&&) = default;

		/// <summary>
		/// Get a string representation of the ControllerDisconnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Input::Controller TRAP::Events::ControllerEvent::GetController() const
{
	return m_controller;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::ControllerConnectEvent::GetStaticType()
{
	return EventType::ControllerConnect;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::ControllerDisconnectEvent::GetStaticType()
{
	return EventType::ControlledDisconnect;
}

#endif /*TRAP_CONTROLLEREVENT_H*/