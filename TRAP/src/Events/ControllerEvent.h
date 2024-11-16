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
		/// @brief Copy constructor.
		consteval ControllerEvent(const ControllerEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval ControllerEvent& operator=(const ControllerEvent&) noexcept = delete;

		/// @brief Destructor.
		constexpr ~ControllerEvent() override = default;

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

		/// @brief Move constructor.
		constexpr ControllerEvent(ControllerEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr ControllerEvent& operator=(ControllerEvent&&) noexcept = default;

	private:
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
		consteval ControllerConnectEvent(const ControllerConnectEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval ControllerConnectEvent& operator=(const ControllerConnectEvent&) noexcept = delete;

		/// @brief Move constructor.
		constexpr ControllerConnectEvent(ControllerConnectEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr ControllerConnectEvent& operator=(ControllerConnectEvent&&) noexcept = default;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;
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
		consteval ControllerDisconnectEvent(const ControllerDisconnectEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval ControllerDisconnectEvent& operator=(const ControllerDisconnectEvent&) noexcept = delete;

		/// @brief Move constructor.
		constexpr ControllerDisconnectEvent(ControllerDisconnectEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr ControllerDisconnectEvent& operator=(ControllerDisconnectEvent&&) noexcept = default;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;
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
	return EventType::ControllerDisconnect;
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
