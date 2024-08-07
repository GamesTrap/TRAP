#ifndef TRAP_WINDOWEVENT_H
#define TRAP_WINDOWEVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Event.h"
#include "Window/Window.h"

namespace TRAP::Events
{
	/// @brief Window resized.
	/// @remark @headless This class is not available in headless mode.
	class WindowResizeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param width New Window width.
		/// @param height New Window height.
		/// @param window The affected window.
		/// @note The window must outlive the WindowResizeEvent!
		WindowResizeEvent(u32 width, u32 height, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowResizeEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowResizeEvent(const WindowResizeEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowResizeEvent& operator=(const WindowResizeEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowResizeEvent(WindowResizeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowResizeEvent& operator=(WindowResizeEvent&&) noexcept = default;

		/// @brief Retrieve the new window width.
		/// @return Width.
		[[nodiscard]] constexpr u32 GetWidth() const noexcept;
		/// @brief Retrieve the new window height.
		/// @return Height.
		[[nodiscard]] constexpr u32 GetHeight() const noexcept;
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
		u32 m_width;
		u32 m_height;
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Window minimized.
	/// @remark @headless This class is not available in headless mode.
	class WindowMinimizeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window The affected window.
		/// @note The window must outlive the WindowMinimizeEvent!
		explicit WindowMinimizeEvent(TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowMinimizeEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowMinimizeEvent(const WindowMinimizeEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowMinimizeEvent& operator=(const WindowMinimizeEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowMinimizeEvent(WindowMinimizeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowMinimizeEvent& operator=(WindowMinimizeEvent&&) noexcept = default;

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

	/// @brief Window maximized.
	/// @remark @headless This class is not available in headless mode.
	class WindowMaximizeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window The affected window.
		/// @note The window must outlive the WindowMaximizeEvent!
		explicit WindowMaximizeEvent(TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowMaximizeEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowMaximizeEvent(const WindowMaximizeEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowMaximizeEvent& operator=(const WindowMaximizeEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowMaximizeEvent(WindowMaximizeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowMaximizeEvent& operator=(WindowMaximizeEvent&&) noexcept = default;

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

	/// @brief Window restored.
	/// @remark @headless This class is not available in headless mode.
	class WindowRestoreEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window The affected window.
		/// @note The window must outlive the WindowRestoreEvent!
		explicit WindowRestoreEvent(TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowRestoreEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowRestoreEvent(const WindowRestoreEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowRestoreEvent& operator=(const WindowRestoreEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowRestoreEvent(WindowRestoreEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowRestoreEvent& operator=(WindowRestoreEvent&&) noexcept = default;

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

	/// @brief Window closed.
	/// @remark @headless This class is not available in headless mode.
	class WindowCloseEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window The affected window.
		/// @note The window must outlive the WindowCloseEvent!
		explicit WindowCloseEvent(TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowCloseEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowCloseEvent(const WindowCloseEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowCloseEvent& operator=(const WindowCloseEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowCloseEvent(WindowCloseEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowCloseEvent& operator=(WindowCloseEvent&&) noexcept = default;

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

	/// @brief Window moved.
	/// @remark @headless This class is not available in headless mode.
	class WindowMoveEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param x New x position.
		/// @param y New y position.
		/// @param window The affected window.
		/// @note The window must outlive the WindowMoveEvent!
		WindowMoveEvent(i32 x, i32 y, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowMoveEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowMoveEvent(const WindowMoveEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowMoveEvent& operator=(const WindowMoveEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowMoveEvent(WindowMoveEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowMoveEvent& operator=(WindowMoveEvent&&) noexcept = default;

		/// @brief Retrieve the new window x position.
		/// @return Window x position.
		[[nodiscard]] constexpr i32 GetX() const noexcept;
		/// @brief Retrieve the new window y position.
		/// @return Window y position.
		[[nodiscard]] constexpr i32 GetY() const noexcept;
		/// @brief Retrieve the new window position.
		/// @return Window position
		[[nodiscard]] constexpr Math::Vec2i GetPosition() const noexcept;
		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
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
		i32 m_x;
		i32 m_y;
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Window focused.
	/// @remark @headless This class is not available in headless mode.
	class WindowFocusEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window The affected window.
		/// @note The window must outlive the WindowFocusEvent!
		explicit WindowFocusEvent(TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowFocusEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowFocusEvent(const WindowFocusEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowFocusEvent& operator=(const WindowFocusEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowFocusEvent(WindowFocusEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowFocusEvent& operator=(WindowFocusEvent&&) noexcept = default;

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

	/// @brief Window unfocused.
	/// @remark @headless This class is not available in headless mode.
	class WindowLostFocusEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window The affected window.
		/// @note The window must outlive the WindowLostFocusEvent!
		explicit WindowLostFocusEvent(TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowLostFocusEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowLostFocusEvent(const WindowLostFocusEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowLostFocusEvent& operator=(const WindowLostFocusEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowLostFocusEvent(WindowLostFocusEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowLostFocusEvent& operator=(WindowLostFocusEvent&&) noexcept = default;

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

	/// @brief Window drag and drop.
	/// @remark @headless This class is not available in headless mode.
	class WindowDropEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param paths Path(s) to the file(s) or folder(s) dropped onto the window.
		/// @param window The affected window.
		/// @note The window must outlive the WindowDropEvent!
		WindowDropEvent(std::vector<std::string> paths, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowDropEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowDropEvent(const WindowDropEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowDropEvent& operator=(const WindowDropEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowDropEvent(WindowDropEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowDropEvent& operator=(WindowDropEvent&&) noexcept = default;

		/// @brief Retrieve the dropped paths.
		/// @return Vector of file or folder paths.
		[[nodiscard]] constexpr std::vector<std::string> GetPaths() const noexcept;
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
		std::vector<std::string> m_paths{};
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Window content scale.
	/// @remark @headless This class is not available in headless mode.
	class WindowContentScaleEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param xScale New x content scale.
		/// @param yScale New y content scale.
		/// @param window The affected window.
		/// @note The window must outlive the WindowContentScaleEvent!
		explicit WindowContentScaleEvent(f32 xScale, f32 yScale, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~WindowContentScaleEvent() override = default;
		/// @brief Copy constructor.
		consteval WindowContentScaleEvent(const WindowContentScaleEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval WindowContentScaleEvent& operator=(const WindowContentScaleEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr WindowContentScaleEvent(WindowContentScaleEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowContentScaleEvent& operator=(WindowContentScaleEvent&&) noexcept = default;

		/// @brief Retrieve the new x content scale.
		/// @return Window x content scale.
		[[nodiscard]] constexpr f32 GetXScale() const noexcept;
		/// @brief Retrieve the new y content scale.
		/// @return Window y content scale.
		[[nodiscard]] constexpr f32 GetYScale() const noexcept;
		/// @brief Retrieve the new content scale.
		/// @return Window content scale.
		[[nodiscard]] constexpr Math::Vec2 GetScale() const noexcept;
		/// @brief Retrieve a reference to the affected window.
		/// @return Window pointer.
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
		f32 m_XScale;
		f32 m_YScale;
		std::reference_wrapper<TRAP::Window> m_window;
	};

	/// @brief Window framebuffer resized.
	/// @remark @headless This class is not available in headless mode.
	class FrameBufferResizeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param width New framebuffer width.
		/// @param height New framebuffer height.
		/// @param window The affected window.
		/// @note The window must outlive the FrameBufferResizeEvent!
		FrameBufferResizeEvent(u32 width, u32 height, TRAP::Window& window);
		/// @brief Destructor.
		constexpr ~FrameBufferResizeEvent() override = default;
		/// @brief Copy constructor.
		consteval FrameBufferResizeEvent(const FrameBufferResizeEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval FrameBufferResizeEvent& operator=(const FrameBufferResizeEvent&) noexcept = delete;
		/// @brief Move constructor.
		constexpr FrameBufferResizeEvent(FrameBufferResizeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		FrameBufferResizeEvent& operator=(FrameBufferResizeEvent&&) noexcept = default;

		/// @brief Retrieve the new framebuffer width.
		/// @return Framebuffer width.
		[[nodiscard]] constexpr u32 GetWidth() const noexcept;
		/// @brief Retrieve the new framebuffer height.
		/// @return Framebuffer height.
		[[nodiscard]] constexpr u32 GetHeight() const noexcept;
		/// @brief Retrieve the new framebuffer size.
		/// @return Framebuffer size.
		[[nodiscard]] constexpr Math::Vec2ui GetSize() const noexcept;
		/// @brief Retrieve the aspect ratio of the framebuffer.
		/// @return Aspect ratio.
		[[nodiscard]] constexpr f32 GetAspectRatio() const noexcept;
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
		u32 m_width;
		u32 m_height;
		std::reference_wrapper<TRAP::Window> m_window;
	};
}

[[nodiscard]] constexpr u32 TRAP::Events::WindowResizeEvent::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Events::WindowResizeEvent::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowResizeEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowResizeEvent::GetStaticType() noexcept
{
	return EventType::WindowResize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowResizeEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowResizeEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowResizeEvent::GetName() const
{
	return "WindowResize";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowMinimizeEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowMinimizeEvent::GetStaticType() noexcept
{
	return EventType::WindowMinimize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowMinimizeEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowMinimizeEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowMinimizeEvent::GetName() const
{
	return "WindowMinimize";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowMaximizeEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowMaximizeEvent::GetStaticType() noexcept
{
	return EventType::WindowMaximize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowMaximizeEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowMaximizeEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowMaximizeEvent::GetName() const
{
	return "WindowMaximize";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowRestoreEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowRestoreEvent::GetStaticType() noexcept
{
	return EventType::WindowRestore;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowRestoreEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowRestoreEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowRestoreEvent::GetName() const
{
	return "WindowRestore";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowCloseEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowCloseEvent::GetStaticType() noexcept
{
	return EventType::WindowClose;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowCloseEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowCloseEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowCloseEvent::GetName() const
{
	return "WindowClose";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr i32 TRAP::Events::WindowMoveEvent::GetX() const noexcept
{
	return m_x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr i32 TRAP::Events::WindowMoveEvent::GetY() const noexcept
{
	return m_y;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2i TRAP::Events::WindowMoveEvent::GetPosition() const noexcept
{
	return {m_x, m_y};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowMoveEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowMoveEvent::GetStaticType() noexcept
{
	return EventType::WindowMove;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowMoveEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowMoveEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowMoveEvent::GetName() const
{
	return "WindowMove";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowFocusEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowFocusEvent::GetStaticType() noexcept
{
	return EventType::WindowFocus;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowFocusEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowFocusEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowFocusEvent::GetName() const
{
	return "WindowFocus";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowLostFocusEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowLostFocusEvent::GetStaticType() noexcept
{
	return EventType::WindowLostFocus;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowLostFocusEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowLostFocusEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowLostFocusEvent::GetName() const
{
	return "WindowLostFocus";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowDropEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowDropEvent::GetStaticType() noexcept
{
	return EventType::WindowDrop;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowDropEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowDropEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::string> TRAP::Events::WindowDropEvent::GetPaths() const noexcept
{
	return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowDropEvent::GetName() const
{
	return "WindowDrop";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Events::WindowContentScaleEvent::GetXScale() const noexcept
{
	return m_XScale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Events::WindowContentScaleEvent::GetYScale() const noexcept
{
	return m_YScale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::Events::WindowContentScaleEvent::GetScale() const noexcept
{
	return {m_XScale, m_YScale};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::WindowContentScaleEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowContentScaleEvent::GetStaticType() noexcept
{
	return EventType::WindowContentScale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::WindowContentScaleEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::WindowContentScaleEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowContentScaleEvent::GetName() const
{
	return "WindowContentScale";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Events::FrameBufferResizeEvent::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Events::FrameBufferResizeEvent::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2ui TRAP::Events::FrameBufferResizeEvent::GetSize() const noexcept
{
	return {m_width, m_height};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Events::FrameBufferResizeEvent::GetAspectRatio() const noexcept
{
	return NumericCast<f32>(m_width) / NumericCast<f32>(m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window& TRAP::Events::FrameBufferResizeEvent::GetWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::FrameBufferResizeEvent::GetStaticType() noexcept
{
	return EventType::FrameBufferResize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::FrameBufferResizeEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::FrameBufferResizeEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::Window;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::FrameBufferResizeEvent::GetName() const
{
	return "FrameBufferResize";
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_WINDOWEVENT_H*/
