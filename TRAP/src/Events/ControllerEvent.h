#ifndef TRAP_CONTROLLEREVENT_H
#define TRAP_CONTROLLEREVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Event.h"
#include "Input/Input.h"

namespace TRAP::Events
{
	/// @brief Controller event base class.
	/// @remark @headless This class is not available in headless mode.
	class ControllerEvent : public Event
	{
	public:
		/// @brief Destructor.
		constexpr  ~ControllerEvent() override = default;

		/// @brief Retrieve the affected controller.
		/// @return Controller.
		[[nodiscard]] constexpr Input::Controller GetController() const;

		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// @brief Constructor.
		/// @param controller Affected controller.
		constexpr explicit ControllerEvent(Input::Controller controller) noexcept;
		/// @brief Copy constructor.
		constexpr ControllerEvent(const ControllerEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr ControllerEvent& operator=(const ControllerEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr ControllerEvent(ControllerEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr ControllerEvent& operator=(ControllerEvent&&) noexcept = default;

		Input::Controller m_controller;
	};

	/// @brief Controller connected event.
	/// @remark @headless This class is not available in headless mode.
	class ControllerConnectEvent final : public ControllerEvent
	{
	public:
		/// @brief Constructor.
		/// @param controller Newly connected controller.
		constexpr explicit ControllerConnectEvent(Input::Controller controller) noexcept;
		/// @brief Destructor.
		constexpr ~ControllerConnectEvent() override = default;
		/// @brief Copy constructor.
		constexpr ControllerConnectEvent(const ControllerConnectEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr ControllerConnectEvent& operator=(const ControllerConnectEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr ControllerConnectEvent(ControllerConnectEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr ControllerConnectEvent& operator=(ControllerConnectEvent&&) noexcept = default;

		/// @brief Get a string representation of the ControllerConnectEvent.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
	};

	/// @brief Controller disconnected event.
	/// @remark @headless This class is not available in headless mode.
	class ControllerDisconnectEvent final : public ControllerEvent
	{
	public:
		/// @brief Constructor.
		/// @param controller Disconnected controller.
		constexpr explicit ControllerDisconnectEvent(Input::Controller controller) noexcept;
		/// @brief Destructor.
		constexpr ~ControllerDisconnectEvent() override = default;
		/// @brief Copy constructor.
		constexpr ControllerDisconnectEvent(const ControllerDisconnectEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr ControllerDisconnectEvent& operator=(const ControllerDisconnectEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr ControllerDisconnectEvent(ControllerDisconnectEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr ControllerDisconnectEvent& operator=(ControllerDisconnectEvent&&) noexcept = default;

		/// @brief Get a string representation of the ControllerDisconnectEvent.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
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