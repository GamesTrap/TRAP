#ifndef TRAP_CONTROLLEREVENT_H
#define TRAP_CONTROLLEREVENT_H

#ifndef TRAP_HEADLESS_MODE

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
		[[nodiscard]] constexpr Input::Controller GetController() const;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="controller">Affected controller.</param>
		explicit ControllerEvent(Input::Controller controller) noexcept;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ControllerEvent(const ControllerEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ControllerEvent& operator=(const ControllerEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ControllerEvent(ControllerEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ControllerEvent& operator=(ControllerEvent&&) noexcept = default;

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
		explicit ControllerConnectEvent(Input::Controller controller) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~ControllerConnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ControllerConnectEvent(const ControllerConnectEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ControllerConnectEvent& operator=(const ControllerConnectEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ControllerConnectEvent(ControllerConnectEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ControllerConnectEvent& operator=(ControllerConnectEvent&&) noexcept = default;

		/// <summary>
		/// Get a string representation of the ControllerConnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] std::string GetName() const override;
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
		explicit ControllerDisconnectEvent(Input::Controller controller) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~ControllerDisconnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ControllerDisconnectEvent(const ControllerDisconnectEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ControllerDisconnectEvent& operator=(const ControllerDisconnectEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ControllerDisconnectEvent(ControllerDisconnectEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ControllerDisconnectEvent& operator=(ControllerDisconnectEvent&&) noexcept = default;

		/// <summary>
		/// Get a string representation of the ControllerDisconnectEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] std::string GetName() const override;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Input::Controller TRAP::Events::ControllerEvent::GetController() const
{
	return m_controller;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ControllerConnectEvent::GetStaticType() noexcept
{
	return EventType::ControllerConnect;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ControllerDisconnectEvent::GetStaticType() noexcept
{
	return EventType::ControlledDisconnect;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_CONTROLLEREVENT_H*/