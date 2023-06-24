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
		constexpr ~ControllerEvent() override = default;

		/// <summary>
		/// Retrieve the affected controller.
		/// </summary>
		/// <returns>Controller.</returns>
		[[nodiscard]] constexpr Input::Controller GetController() const;

		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="controller">Affected controller.</param>
		constexpr explicit ControllerEvent(Input::Controller controller) noexcept;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr ControllerEvent(const ControllerEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr ControllerEvent& operator=(const ControllerEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr ControllerEvent(ControllerEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr ControllerEvent& operator=(ControllerEvent&&) noexcept = default;

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
		constexpr explicit ControllerConnectEvent(Input::Controller controller) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~ControllerConnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr ControllerConnectEvent(const ControllerConnectEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr ControllerConnectEvent& operator=(const ControllerConnectEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr ControllerConnectEvent(ControllerConnectEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr ControllerConnectEvent& operator=(ControllerConnectEvent&&) noexcept = default;

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
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr std::string GetName() const override;
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
		constexpr explicit ControllerDisconnectEvent(Input::Controller controller) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~ControllerDisconnectEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr ControllerDisconnectEvent(const ControllerDisconnectEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr ControllerDisconnectEvent& operator=(const ControllerDisconnectEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr ControllerDisconnectEvent(ControllerDisconnectEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr ControllerDisconnectEvent& operator=(ControllerDisconnectEvent&&) noexcept = default;

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
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] constexpr std::string GetName() const override;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::ControllerEvent::ControllerEvent(const Input::Controller controller) noexcept
	: m_controller(controller)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::ControllerEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Controller | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Input::Controller TRAP::Events::ControllerEvent::GetController() const
{
	return m_controller;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::ControllerConnectEvent::ControllerConnectEvent(const Input::Controller controller) noexcept
	: ControllerEvent(controller)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ControllerConnectEvent::GetStaticType() noexcept
{
	return EventType::ControllerConnect;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ControllerConnectEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::ControllerConnectEvent::GetName() const
{
	return "ControllerConnect";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::ControllerDisconnectEvent::ControllerDisconnectEvent(const Input::Controller controller) noexcept
	: ControllerEvent(controller)
{
}

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ControllerDisconnectEvent::GetStaticType() noexcept
{
	return EventType::ControlledDisconnect;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ControllerDisconnectEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::ControllerDisconnectEvent::GetName() const
{
	return "ControllerDisconnect";
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_CONTROLLEREVENT_H*/