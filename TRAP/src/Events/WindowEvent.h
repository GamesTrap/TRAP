#ifndef TRAP_WINDOWEVENT_H
#define TRAP_WINDOWEVENT_H

#ifndef TRAP_HEADLESS_MODE

#include "Event.h"
#include "Maths/Vec2.h"

namespace TRAP
{
	class Window;
}

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
		WindowResizeEvent(uint32_t width, uint32_t height, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowResizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowResizeEvent(const WindowResizeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowResizeEvent& operator=(const WindowResizeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowResizeEvent(WindowResizeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowResizeEvent& operator=(WindowResizeEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new window width.
		/// </summary>
		/// <returns>Width.</returns>
		[[nodiscard]] constexpr uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the new window height.
		/// </summary>
		/// <returns>Height.</returns>
		[[nodiscard]] constexpr uint32_t GetHeight() const noexcept;
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
		[[nodiscard]] EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

	private:
		uint32_t m_width, m_height;
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
		~WindowMinimizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowMinimizeEvent(const WindowMinimizeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMinimizeEvent& operator=(const WindowMinimizeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowMinimizeEvent(WindowMinimizeEvent&&) noexcept = default;
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

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
		~WindowMaximizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowMaximizeEvent(const WindowMaximizeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMaximizeEvent& operator=(const WindowMaximizeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowMaximizeEvent(WindowMaximizeEvent&&) noexcept = default;
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

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
		~WindowRestoreEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowRestoreEvent(const WindowRestoreEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowRestoreEvent& operator=(const WindowRestoreEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowRestoreEvent(WindowRestoreEvent&&) noexcept = default;
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

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
		~WindowCloseEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowCloseEvent(const WindowCloseEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowCloseEvent& operator=(const WindowCloseEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowCloseEvent(WindowCloseEvent&&) noexcept = default;
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

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
		WindowMoveEvent(int32_t x, int32_t y, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowMoveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowMoveEvent(const WindowMoveEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMoveEvent& operator=(const WindowMoveEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowMoveEvent(WindowMoveEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowMoveEvent& operator=(WindowMoveEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new window x position.
		/// </summary>
		/// <returns>Window x position.</returns>
		[[nodiscard]] constexpr int32_t GetX() const noexcept;
		/// <summary>
		/// Retrieve the new window y position.
		/// </summary>
		/// <returns>Window y position.</returns>
		[[nodiscard]] constexpr int32_t GetY() const noexcept;
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
		[[nodiscard]] EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

	private:
		int32_t m_x, m_y;
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
		~WindowFocusEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowFocusEvent(const WindowFocusEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowFocusEvent& operator=(const WindowFocusEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowFocusEvent(WindowFocusEvent&&) noexcept = default;
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

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
		~WindowLostFocusEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowLostFocusEvent(const WindowLostFocusEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowLostFocusEvent& operator=(const WindowLostFocusEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowLostFocusEvent(WindowLostFocusEvent&&) noexcept = default;
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

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
		~WindowDropEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowDropEvent(const WindowDropEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowDropEvent& operator=(const WindowDropEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowDropEvent(WindowDropEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowDropEvent& operator=(WindowDropEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the dropped paths.
		/// </summary>
		/// <returns>Vector of file or folder paths.</returns>
		[[nodiscard]] const std::vector<std::string>& GetPaths() const noexcept;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		[[nodiscard]] constexpr TRAP::Window* GetWindow() const noexcept;

		/// <summary>
		/// Get a string representation of the WindowDropEvent.
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
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

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
		explicit WindowContentScaleEvent(float xScale, float yScale, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowContentScaleEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowContentScaleEvent(const WindowContentScaleEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowContentScaleEvent& operator=(const WindowContentScaleEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowContentScaleEvent(WindowContentScaleEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowContentScaleEvent& operator=(WindowContentScaleEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new x content scale.
		/// </summary>
		/// <returns>Window x content scale.</returns>
		[[nodiscard]] constexpr float GetXScale() const noexcept;
		/// <summary>
		/// Retrieve the new y content scale.
		/// </summary>
		/// <returns>Window y content scale.</returns>
		[[nodiscard]] constexpr float GetYScale() const noexcept;
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
		[[nodiscard]] EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

	private:
		float m_XScale;
		float m_YScale;
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
		FrameBufferResizeEvent(uint32_t width, uint32_t height, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~FrameBufferResizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		FrameBufferResizeEvent(const FrameBufferResizeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		FrameBufferResizeEvent& operator=(const FrameBufferResizeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		FrameBufferResizeEvent(FrameBufferResizeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FrameBufferResizeEvent& operator=(FrameBufferResizeEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the new framebuffer width.
		/// </summary>
		/// <returns>Framebuffer width.</returns>
		[[nodiscard]] constexpr uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the new framebuffer height.
		/// </summary>
		/// <returns>Framebuffer height.</returns>
		[[nodiscard]] constexpr uint32_t GetHeight() const noexcept;
		/// <summary>
		/// Retrieve the new framebuffer size.
		/// </summary>
		/// <returns>Framebuffer size.</returns>
		[[nodiscard]] constexpr Math::Vec2ui GetSize() const noexcept;
		/// <summary>
		/// Retrieve the aspect ratio of the framebuffer.
		/// </summary>
		/// <returns>Aspect ratio.</returns>
		[[nodiscard]] constexpr float GetAspectRatio() const noexcept;
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
		[[nodiscard]] EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

	private:
		uint32_t m_width, m_height;
		TRAP::Window* m_window;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Events::WindowResizeEvent::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Events::WindowResizeEvent::GetHeight() const noexcept
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
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr int32_t TRAP::Events::WindowMoveEvent::GetX() const noexcept
{
	return m_x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr int32_t TRAP::Events::WindowMoveEvent::GetY() const noexcept
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
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Events::WindowContentScaleEvent::GetXScale() const noexcept
{
	return m_XScale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Events::WindowContentScaleEvent::GetYScale() const noexcept
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
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Events::FrameBufferResizeEvent::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Events::FrameBufferResizeEvent::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2ui TRAP::Events::FrameBufferResizeEvent::GetSize() const noexcept
{
	return {m_width, m_height};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Events::FrameBufferResizeEvent::GetAspectRatio() const noexcept
{
	return NumericCast<float>(m_width) / NumericCast<float>(m_height);
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

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_WINDOWEVENT_H*/