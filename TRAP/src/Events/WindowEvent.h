#ifndef TRAP_WINDOWEVENT_H
#define TRAP_WINDOWEVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Event.h"
#include "Window/Window.h"

namespace TRAP::Events
{
	/// <summary>
	/// Window resized.
	/// </summary>
	class WindowResizeEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="width">New Window width.</param>
		/// <param name="height">New Window height.</param>
		/// <param name="window">Pointer to the affected window.</param>
		WindowResizeEvent(u32 width, u32 height, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowResizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowResizeEvent(const WindowResizeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowResizeEvent& operator=(const WindowResizeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowResizeEvent(WindowResizeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowResizeEvent& operator=(WindowResizeEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new window width.
		/// </summary>
		/// <returns>Width.</returns>
		[[nodiscard]] constexpr u32 GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the new window height.
		/// </summary>
		/// <returns>Height.</returns>
		[[nodiscard]] constexpr u32 GetHeight() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowResizeEvent.
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		u32 m_width, m_height;
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window minimized.
	/// </summary>
	class WindowMinimizeEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected window.</param>
		explicit WindowMinimizeEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowMinimizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowMinimizeEvent(const WindowMinimizeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMinimizeEvent& operator=(const WindowMinimizeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowMinimizeEvent(WindowMinimizeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowMinimizeEvent& operator=(WindowMinimizeEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowMinimizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr std::string ToString() const override;

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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window maximized.
	/// </summary>
	class WindowMaximizeEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected window.</param>
		explicit WindowMaximizeEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowMaximizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowMaximizeEvent(const WindowMaximizeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMaximizeEvent& operator=(const WindowMaximizeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowMaximizeEvent(WindowMaximizeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowMaximizeEvent& operator=(WindowMaximizeEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;
		/// <summary>
		/// Get a string representation of the WindowMaximizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr std::string ToString() const override;

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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window restored.
	/// </summary>
	class WindowRestoreEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected window.</param>
		explicit WindowRestoreEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowRestoreEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowRestoreEvent(const WindowRestoreEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowRestoreEvent& operator=(const WindowRestoreEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowRestoreEvent(WindowRestoreEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowRestoreEvent& operator=(WindowRestoreEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowRestoreEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr std::string ToString() const override;

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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window closed.
	/// </summary>
	class WindowCloseEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected window.</param>
		explicit WindowCloseEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowCloseEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowCloseEvent(const WindowCloseEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowCloseEvent& operator=(const WindowCloseEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowCloseEvent(WindowCloseEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowCloseEvent& operator=(WindowCloseEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowCloseEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr std::string ToString() const override;

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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window moved.
	/// </summary>
	class WindowMoveEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="x">New x position.</param>
		/// <param name="y">New y position.</param>
		/// <param name="window">Pointer to the affected window.</param>
		WindowMoveEvent(i32 x, i32 y, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowMoveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowMoveEvent(const WindowMoveEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMoveEvent& operator=(const WindowMoveEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowMoveEvent(WindowMoveEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowMoveEvent& operator=(WindowMoveEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new window x position.
		/// </summary>
		/// <returns>Window x position.</returns>
		[[nodiscard]] constexpr i32 GetX() const noexcept;
		/// <summary>
		/// Retrieve the new window y position.
		/// </summary>
		/// <returns>Window y position.</returns>
		[[nodiscard]] constexpr i32 GetY() const noexcept;
		/// <summary>
		/// Retrieve the new window position.
		/// </summary>
		/// <returns>Window position</returns>
		[[nodiscard]] constexpr Math::Vec2i GetPosition() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowMoveEvent.
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		i32 m_x, m_y;
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window focused.
	/// </summary>
	class WindowFocusEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected window.</param>
		explicit WindowFocusEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowFocusEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowFocusEvent(const WindowFocusEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowFocusEvent& operator=(const WindowFocusEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowFocusEvent(WindowFocusEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowFocusEvent& operator=(WindowFocusEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowFocusEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr std::string ToString() const override;

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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window unfocused.
	/// </summary>
	class WindowLostFocusEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="window">Pointer to the affected window.</param>
		explicit WindowLostFocusEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowLostFocusEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowLostFocusEvent(const WindowLostFocusEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowLostFocusEvent& operator=(const WindowLostFocusEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowLostFocusEvent(WindowLostFocusEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowLostFocusEvent& operator=(WindowLostFocusEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowLostFocusEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr std::string ToString() const override;

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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window drag and drop.
	/// </summary>
	class WindowDropEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="paths">Path(s) to the file(s) or folder(s) dropped onto the window.</param>
		/// <param name="window">Pointer to the affected window.</param>
		explicit WindowDropEvent(std::vector<std::string> paths, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowDropEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowDropEvent(const WindowDropEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowDropEvent& operator=(const WindowDropEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowDropEvent(WindowDropEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowDropEvent& operator=(WindowDropEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the dropped paths.
		/// </summary>
		/// <returns>Vector of file or folder paths.</returns>
		[[nodiscard]] constexpr std::vector<std::string> GetPaths() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowDropEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] constexpr std::string ToString() const override;

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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		std::vector<std::string> m_paths{};
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window content scale.
	/// </summary>
	class WindowContentScaleEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="xScale">New x content scale.</param>
		/// <param name="yScale">New y content scale.</param>
		/// <param name="window">Pointer to the affected window.</param>
		explicit WindowContentScaleEvent(f32 xScale, f32 yScale, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~WindowContentScaleEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr WindowContentScaleEvent(const WindowContentScaleEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowContentScaleEvent& operator=(const WindowContentScaleEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr WindowContentScaleEvent(WindowContentScaleEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowContentScaleEvent& operator=(WindowContentScaleEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new x content scale.
		/// </summary>
		/// <returns>Window x content scale.</returns>
		[[nodiscard]] constexpr f32 GetXScale() const noexcept;
		/// <summary>
		/// Retrieve the new y content scale.
		/// </summary>
		/// <returns>Window y content scale.</returns>
		[[nodiscard]] constexpr f32 GetYScale() const noexcept;
		/// <summary>
		/// Retrieve the new content scale.
		/// </summary>
		/// <returns>Window content scale.</returns>
		[[nodiscard]] constexpr Math::Vec2 GetScale() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowContentScaleEvent.
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
		f32 m_XScale;
		f32 m_YScale;
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window framebuffer resized.
	/// </summary>
	class FrameBufferResizeEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="width">New framebuffer width.</param>
		/// <param name="height">New framebuffer height.</param>
		/// <param name="window">Pointer to the affected window.</param>
		FrameBufferResizeEvent(u32 width, u32 height, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~FrameBufferResizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr FrameBufferResizeEvent(const FrameBufferResizeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		FrameBufferResizeEvent& operator=(const FrameBufferResizeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr FrameBufferResizeEvent(FrameBufferResizeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FrameBufferResizeEvent& operator=(FrameBufferResizeEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new framebuffer width.
		/// </summary>
		/// <returns>Framebuffer width.</returns>
		[[nodiscard]] constexpr u32 GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the new framebuffer height.
		/// </summary>
		/// <returns>Framebuffer height.</returns>
		[[nodiscard]] constexpr u32 GetHeight() const noexcept;
		/// <summary>
		/// Retrieve the new framebuffer size.
		/// </summary>
		/// <returns>Framebuffer size.</returns>
		[[nodiscard]] constexpr Math::Vec2ui GetSize() const noexcept;
		/// <summary>
		/// Retrieve the aspect ratio of the framebuffer.
		/// </summary>
		/// <returns>Aspect ratio.</returns>
		[[nodiscard]] constexpr f32 GetAspectRatio() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the FrameBufferResizeEvent.
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
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