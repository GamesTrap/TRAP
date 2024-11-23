#ifndef TRAP_MOUSEEVENT_H
#define TRAP_MOUSEEVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Input/Input.h"
#include "Event.h"
#include "Maths/Types.h"
#include "Maths/Vec2.h"

namespace TRAP::Events
{
	/// @brief Mouse moved.
	/// @remark @headless This class is not available in headless mode.
	class MouseMoveEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param x New mouse x position.
		/// @param y New mouse y position.
		/// @param window The affected window.
		/// @note The window must outlive the MouseMoveEvent!
		MouseMoveEvent(f32 x, f32 y, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~MouseMoveEvent() override = default;
		/// @brief Copy constructor.
		consteval MouseMoveEvent(const MouseMoveEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MouseMoveEvent& operator=(const MouseMoveEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MouseMoveEvent(MouseMoveEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MouseMoveEvent& operator=(MouseMoveEvent&&) noexcept = default;

		/// @brief Retrieve the new mouse x position.
		/// @return Mouse x position.
		[[nodiscard]] constexpr f32 GetX() const noexcept;
		/// @brief Retrieve the new mouse y position.
		/// @return Mouse y position.
		[[nodiscard]] constexpr f32 GetY() const noexcept;
		/// @brief Retrieve the new mouse position.
		/// @return Mouse position.
		[[nodiscard]] constexpr Math::Vec2 GetPosition() const noexcept;
		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
		f32 m_mouseX;
		f32 m_mouseY;
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Mouse scrolled.
	/// @remark @headless This class is not available in headless mode.
	class MouseScrollEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param xOffset New mouse scroll wheel x offset.
		/// @param yOffset New mouse scroll wheel y offset.
		/// @param window The affected window.
		/// @note The window must outlive the MouseScrollEvent!
		MouseScrollEvent(f32 xOffset, f32 yOffset, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~MouseScrollEvent() override = default;
		/// @brief Copy constructor.
		consteval MouseScrollEvent(const MouseScrollEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MouseScrollEvent& operator=(const MouseScrollEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MouseScrollEvent(MouseScrollEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MouseScrollEvent& operator=(MouseScrollEvent&&) noexcept = default;

		/// @brief Retrieve the new mouse scroll wheel x offset.
		/// @return Mouse scroll wheel x offset.
		[[nodiscard]] constexpr f32 GetXOffset() const noexcept;
		/// @brief Retrieve the new mouse scroll wheel y offset.
		/// @return Mouse scroll wheel y offset.
		[[nodiscard]] constexpr f32 GetYOffset() const noexcept;
		/// @brief Retrieve the new mouse scroll wheel offset.
		/// @return Mouse scroll wheel offset.
		[[nodiscard]] constexpr Math::Vec2 GetOffset() const noexcept;
		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
		f32 m_xOffset;
		f32 m_yOffset;
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Mouse button event base class.
	/// @remark @headless This class is not available in headless mode.
	class MouseButtonEvent : public Event
	{
	public:
		/// @brief Copy constructor.
		consteval MouseButtonEvent(const MouseButtonEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MouseButtonEvent& operator=(const MouseButtonEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MouseButtonEvent(MouseButtonEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr MouseButtonEvent& operator=(MouseButtonEvent&&) noexcept = default;

		/// @brief Destructor.
		constexpr ~MouseButtonEvent() override = default;

		/// @brief Retrieve the affected mouse button.
		/// @return Mouse button.
		[[nodiscard]] constexpr Input::MouseButton GetMouseButton() const noexcept;

		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	protected:
		/// @brief Constructor.
		/// @param button Affected mouse button.
		explicit constexpr MouseButtonEvent(Input::MouseButton button) noexcept;

	private:
		Input::MouseButton m_button;
	};

	/// @brief Mouse button pressed.
	/// @remark @headless This class is not available in headless mode.
	class MouseButtonPressEvent final : public MouseButtonEvent
	{
	public:
		/// @brief Constructor.
		/// @param button Pressed mouse button.
		/// @param window The affected window.
		/// @note The window must outlive the MouseButtonPressEvent!
		MouseButtonPressEvent(Input::MouseButton button, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~MouseButtonPressEvent() override = default;
		/// @brief Copy constructor.
		consteval MouseButtonPressEvent(const MouseButtonPressEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MouseButtonPressEvent& operator=(const MouseButtonPressEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MouseButtonPressEvent(MouseButtonPressEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MouseButtonPressEvent& operator=(MouseButtonPressEvent&&) noexcept = default;

		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

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

	private:
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Mouse button released.
	/// @remark @headless This class is not available in headless mode.
	class MouseButtonReleaseEvent final : public MouseButtonEvent
	{
	public:
		/// @brief Constructor.
		/// @param button Released mouse button.
		/// @param window The affected window.
		/// @note The window must outlive the MouseButtonReleaseEvent!
		MouseButtonReleaseEvent(Input::MouseButton button, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~MouseButtonReleaseEvent() override = default;
		/// @brief Copy constructor.
		consteval MouseButtonReleaseEvent(const MouseButtonReleaseEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MouseButtonReleaseEvent& operator=(const MouseButtonReleaseEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MouseButtonReleaseEvent(MouseButtonReleaseEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MouseButtonReleaseEvent& operator=(MouseButtonReleaseEvent&&) noexcept = default;

		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

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

	private:
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Mouse entered.
	/// @remark @headless This class is not available in headless mode.
	class MouseEnterEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window The affected window.
		/// @note The window must outlive the MouseEnterEvent!
		explicit MouseEnterEvent(TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~MouseEnterEvent() override = default;
		/// @brief Copy constructor.
		consteval MouseEnterEvent(const MouseEnterEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MouseEnterEvent& operator=(const MouseEnterEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MouseEnterEvent(MouseEnterEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MouseEnterEvent& operator=(MouseEnterEvent&&) noexcept = default;

		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Mouse left.
	/// @remark @headless This class is not available in headless mode.
	class MouseLeaveEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window The affected window.
		/// @note The window must outlive the MouseLeaveEvent!
		explicit MouseLeaveEvent(TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~MouseLeaveEvent() override = default;
		/// @brief Copy constructor.
		consteval MouseLeaveEvent(const MouseLeaveEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval MouseLeaveEvent& operator=(const MouseLeaveEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr MouseLeaveEvent(MouseLeaveEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		MouseLeaveEvent& operator=(MouseLeaveEvent&&) noexcept = default;

		/// @brief Retrieve a reference to the affected window.
		/// @return Window reference.
		[[nodiscard]] constexpr TRAP::Window& GetWindow() const noexcept;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
		std::reference_wrapper<TRAP::Window> m_window;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Events::MouseMoveEvent::GetX() const noexcept
{
	return m_mouseX;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Events::MouseMoveEvent::GetY() const noexcept
{
	return m_mouseY;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::Events::MouseMoveEvent::GetPosition() const noexcept
{
	return { m_mouseX, m_mouseY };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::MouseMoveEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetStaticType() noexcept
{
	return EventType::MouseMove;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseMoveEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseMoveEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseMoveEvent::GetName() const
{
	return "MouseMove";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Events::MouseScrollEvent::GetXOffset() const noexcept
{
	return m_xOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Events::MouseScrollEvent::GetYOffset() const noexcept
{
	return m_yOffset;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::Events::MouseScrollEvent::GetOffset() const noexcept
{
	return { m_xOffset, m_yOffset };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::MouseScrollEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetStaticType() noexcept
{
	return EventType::MouseScroll;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseScrollEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseScrollEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseScrollEvent::GetName() const
{
	return "MouseScroll";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Input::MouseButton TRAP::Events::MouseButtonEvent::GetMouseButton() const noexcept
{
	return m_button;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::MouseButtonEvent::MouseButtonEvent(const Input::MouseButton button) noexcept
	: m_button(button)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseButtonEvent::GetCategoryFlags() const noexcept
{
	using enum TRAP::Events::EventCategory;
	return Mouse | Input | MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::MouseButtonPressEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetStaticType() noexcept
{
	return EventType::MouseButtonPress;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseButtonPressEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseButtonPressEvent::GetName() const
{
	return "MouseButtonPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::MouseButtonReleaseEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetStaticType() noexcept
{
	return EventType::MouseButtonRelease;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseButtonReleaseEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseButtonReleaseEvent::GetName() const
{
	return "MouseButtonRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::MouseEnterEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetStaticType() noexcept
{
	return EventType::MouseEnter;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseEnterEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseEnterEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseEnterEvent::GetName() const
{
	return "MouseEnter";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::MouseLeaveEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetStaticType() noexcept
{
	return EventType::MouseLeave;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::MouseLeaveEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::MouseLeaveEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Mouse | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::MouseLeaveEvent::GetName() const
{
	return "MouseLeave";
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_MOUSEEVENT_H*/
