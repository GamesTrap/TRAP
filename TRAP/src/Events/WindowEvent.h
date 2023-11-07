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
		/// @param window Pointer to the affected window.
		WindowResizeEvent(u32 width, u32 height, TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowResizeEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowResizeEvent(const WindowResizeEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowResizeEvent& operator=(const WindowResizeEvent&) noexcept = default;
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
		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowResizeEvent.
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
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		u32 m_width, m_height;
		TRAP::Window* m_window;
	};

	/// @brief Window minimized.
	/// @remark @headless This class is not available in headless mode.
	class WindowMinimizeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window Pointer to the affected window.
		explicit WindowMinimizeEvent(TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowMinimizeEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowMinimizeEvent(const WindowMinimizeEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowMinimizeEvent& operator=(const WindowMinimizeEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr WindowMinimizeEvent(WindowMinimizeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowMinimizeEvent& operator=(WindowMinimizeEvent&&) noexcept = default;

		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowMinimizeEvent.
		/// @return String representation.
		[[nodiscard]] constexpr std::string ToString() const override;

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

	private:
		TRAP::Window* m_window;
	};

	/// @brief Window maximized.
	/// @remark @headless This class is not available in headless mode.
	class WindowMaximizeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window Pointer to the affected window.
		explicit WindowMaximizeEvent(TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowMaximizeEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowMaximizeEvent(const WindowMaximizeEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowMaximizeEvent& operator=(const WindowMaximizeEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr WindowMaximizeEvent(WindowMaximizeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowMaximizeEvent& operator=(WindowMaximizeEvent&&) noexcept = default;

		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;
		/// @brief Get a string representation of the WindowMaximizeEvent.
		/// @return String representation.
		[[nodiscard]] constexpr std::string ToString() const override;

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

	private:
		TRAP::Window* m_window;
	};

	/// @brief Window restored.
	/// @remark @headless This class is not available in headless mode.
	class WindowRestoreEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window Pointer to the affected window.
		explicit WindowRestoreEvent(TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowRestoreEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowRestoreEvent(const WindowRestoreEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowRestoreEvent& operator=(const WindowRestoreEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr WindowRestoreEvent(WindowRestoreEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowRestoreEvent& operator=(WindowRestoreEvent&&) noexcept = default;

		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowRestoreEvent.
		/// @return String representation.
		[[nodiscard]] constexpr std::string ToString() const override;

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

	private:
		TRAP::Window* m_window;
	};

	/// @brief Window closed.
	/// @remark @headless This class is not available in headless mode.
	class WindowCloseEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window Pointer to the affected window.
		explicit WindowCloseEvent(TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowCloseEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowCloseEvent(const WindowCloseEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowCloseEvent& operator=(const WindowCloseEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr WindowCloseEvent(WindowCloseEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowCloseEvent& operator=(WindowCloseEvent&&) noexcept = default;

		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowCloseEvent.
		/// @return String representation.
		[[nodiscard]] constexpr std::string ToString() const override;

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

	private:
		TRAP::Window* m_window;
	};

	/// @brief Window moved.
	/// @remark @headless This class is not available in headless mode.
	class WindowMoveEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param x New x position.
		/// @param y New y position.
		/// @param window Pointer to the affected window.
		WindowMoveEvent(i32 x, i32 y, TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowMoveEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowMoveEvent(const WindowMoveEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowMoveEvent& operator=(const WindowMoveEvent&) noexcept = default;
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
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowMoveEvent.
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
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		i32 m_x, m_y;
		TRAP::Window* m_window;
	};

	/// @brief Window focused.
	/// @remark @headless This class is not available in headless mode.
	class WindowFocusEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window Pointer to the affected window.
		explicit WindowFocusEvent(TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowFocusEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowFocusEvent(const WindowFocusEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowFocusEvent& operator=(const WindowFocusEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr WindowFocusEvent(WindowFocusEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowFocusEvent& operator=(WindowFocusEvent&&) noexcept = default;

		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowFocusEvent.
		/// @return String representation.
		[[nodiscard]] constexpr std::string ToString() const override;

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

	private:
		TRAP::Window* m_window;
	};

	/// @brief Window unfocused.
	/// @remark @headless This class is not available in headless mode.
	class WindowLostFocusEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param window Pointer to the affected window.
		explicit WindowLostFocusEvent(TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowLostFocusEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowLostFocusEvent(const WindowLostFocusEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowLostFocusEvent& operator=(const WindowLostFocusEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr WindowLostFocusEvent(WindowLostFocusEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowLostFocusEvent& operator=(WindowLostFocusEvent&&) noexcept = default;

		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowLostFocusEvent.
		/// @return String representation.
		[[nodiscard]] constexpr std::string ToString() const override;

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

	private:
		TRAP::Window* m_window;
	};

	/// @brief Window drag and drop.
	/// @remark @headless This class is not available in headless mode.
	class WindowDropEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param paths Path(s) to the file(s) or folder(s) dropped onto the window.
		/// @param window Pointer to the affected window.
		explicit WindowDropEvent(std::vector<std::string> paths, TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowDropEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowDropEvent(const WindowDropEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowDropEvent& operator=(const WindowDropEvent&) noexcept = default;
		/// @brief Move constructor.
		constexpr WindowDropEvent(WindowDropEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		WindowDropEvent& operator=(WindowDropEvent&&) noexcept = default;

		/// @brief Retrieve the dropped paths.
		/// @return Vector of file or folder paths.
		[[nodiscard]] constexpr std::vector<std::string> GetPaths() const noexcept;
		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowDropEvent.
		/// @return String representation.
		[[nodiscard]] constexpr std::string ToString() const override;

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

	private:
		std::vector<std::string> m_paths{};
		TRAP::Window* m_window;
	};

	/// @brief Window content scale.
	/// @remark @headless This class is not available in headless mode.
	class WindowContentScaleEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param xScale New x content scale.
		/// @param yScale New y content scale.
		/// @param window Pointer to the affected window.
		explicit WindowContentScaleEvent(f32 xScale, f32 yScale, TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~WindowContentScaleEvent() override = default;
		/// @brief Copy constructor.
		constexpr WindowContentScaleEvent(const WindowContentScaleEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		WindowContentScaleEvent& operator=(const WindowContentScaleEvent&) noexcept = default;
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
		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the WindowContentScaleEvent.
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
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		f32 m_XScale;
		f32 m_YScale;
		TRAP::Window* m_window;
	};

	/// @brief Window framebuffer resized.
	/// @remark @headless This class is not available in headless mode.
	class FrameBufferResizeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param width New framebuffer width.
		/// @param height New framebuffer height.
		/// @param window Pointer to the affected window.
		FrameBufferResizeEvent(u32 width, u32 height, TRAP::Window* window);
		/// @brief Destructor.
		constexpr ~FrameBufferResizeEvent() override = default;
		/// @brief Copy constructor.
		constexpr FrameBufferResizeEvent(const FrameBufferResizeEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		FrameBufferResizeEvent& operator=(const FrameBufferResizeEvent&) noexcept = default;
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
		/// @brief Retrieve a pointer to the affected window.
		/// @return Window pointer.
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// @brief Get a string representation of the FrameBufferResizeEvent.
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
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		u32 m_width, m_height;
		TRAP::Window* m_window;
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

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowResizeEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowMinimizeEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr std::string TRAP::Events::WindowMinimizeEvent::ToString() const
{
	return std::string("WindowMinimizeEvent (\"") + m_window->GetTitle() + "\")";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowMinimizeEvent::GetName() const
{
	return "WindowMinimize";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowMaximizeEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr std::string TRAP::Events::WindowMaximizeEvent::ToString() const
{
	return std::string("WindowMaximizeEvent (\"") + m_window->GetTitle() + "\")";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowMaximizeEvent::GetName() const
{
	return "WindowMaximize";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowRestoreEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr std::string TRAP::Events::WindowRestoreEvent::ToString() const
{
	return std::string("WindowRestoreEvent (\"") + m_window->GetTitle() + "\")";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowRestoreEvent::GetName() const
{
	return "WindowRestore";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowCloseEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr std::string TRAP::Events::WindowCloseEvent::ToString() const
{
	return std::string("WindowCloseEvent (\"") + m_window->GetTitle() + "\")";
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

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowMoveEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowFocusEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr std::string TRAP::Events::WindowFocusEvent::ToString() const
{
	return std::string("WindowFocusEvent (\"") + m_window->GetTitle() + "\")";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowFocusEvent::GetName() const
{
	return "WindowFocus";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowLostFocusEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr std::string TRAP::Events::WindowLostFocusEvent::ToString() const
{
	return std::string("WindowLostFocusEvent (\"") + m_window->GetTitle() + "\")";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::WindowLostFocusEvent::GetName() const
{
	return "WindowLostFocus";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowDropEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr std::string TRAP::Events::WindowDropEvent::ToString() const
{
	std::string result = "WindowDropEvent: ";

	if(!m_paths.empty())
	{
		result += "Path(s): ";
		for (const std::string& path : m_paths)
			result += path + '\n';
		result.back() = ' ';
	}

	result += "(\"" + m_window->GetTitle() + "\")";

	return result;
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

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::WindowContentScaleEvent::GetWindow() const noexcept
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

[[nodiscard]] constexpr TRAP::Window* TRAP::Events::FrameBufferResizeEvent::GetWindow() const noexcept
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