#ifndef _TRAP_WINDOWEVENT_H_
#define _TRAP_WINDOWEVENT_H_

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
		/// <param name="window">Pointer to the affected Window.</param>
		WindowResizeEvent(uint32_t width, uint32_t height, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowResizeEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowResizeEvent(const WindowResizeEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowResizeEvent& operator=(const WindowResizeEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowResizeEvent(WindowResizeEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowResizeEvent& operator=(WindowResizeEvent&&) = default;

		/// <summary>
		/// Retrieve the new Window width.
		/// </summary>
		/// <returns>Width.</returns>
		uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve the new Window height.
		/// </summary>
		/// <returns>Height.</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowResizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit WindowMinimizeEvent(TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowMinimizeEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowMinimizeEvent(const WindowMinimizeEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowMinimizeEvent& operator=(const WindowMinimizeEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowMinimizeEvent(WindowMinimizeEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowMinimizeEvent& operator=(WindowMinimizeEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowMinimizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit WindowMaximizeEvent(TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowMaximizeEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowMaximizeEvent(const WindowMaximizeEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowMaximizeEvent& operator=(const WindowMaximizeEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowMaximizeEvent(WindowMaximizeEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowMaximizeEvent& operator=(WindowMaximizeEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;
		/// <summary>
		/// Get a string representation of the WindowMaximizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit WindowRestoreEvent(TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowRestoreEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowRestoreEvent(const WindowRestoreEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowRestoreEvent& operator=(const WindowRestoreEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowRestoreEvent(WindowRestoreEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowRestoreEvent& operator=(WindowRestoreEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowRestoreEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit WindowCloseEvent(TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowCloseEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowCloseEvent(const WindowCloseEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowCloseEvent& operator=(const WindowCloseEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowCloseEvent(WindowCloseEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowCloseEvent& operator=(WindowCloseEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowCloseEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		WindowMoveEvent(int32_t x, int32_t y, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowMoveEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowMoveEvent(const WindowMoveEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowMoveEvent& operator=(const WindowMoveEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowMoveEvent(WindowMoveEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowMoveEvent& operator=(WindowMoveEvent&&) = default;

		/// <summary>
		/// Retrieve the new Window x position.
		/// </summary>
		/// <returns>X position.</returns>
		int32_t GetX() const;
		/// <summary>
		/// Retrieve the new Window y position.
		/// </summary>
		/// <returns>Y position.</returns>
		int32_t GetY() const;
		/// <summary>
		/// Retrieve the new Window position.
		/// </summary>
		/// <returns>Position</returns>
		Math::Vec2i GetPosition() const;
		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowMoveEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit WindowFocusEvent(TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowFocusEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowFocusEvent(const WindowFocusEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowFocusEvent& operator=(const WindowFocusEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowFocusEvent(WindowFocusEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowFocusEvent& operator=(WindowFocusEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowFocusEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit WindowLostFocusEvent(TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowLostFocusEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowLostFocusEvent(const WindowLostFocusEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowLostFocusEvent& operator=(const WindowLostFocusEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowLostFocusEvent(WindowLostFocusEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowLostFocusEvent& operator=(WindowLostFocusEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowLostFocusEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		TRAP::Window* m_window;
	};

	/// <summary>
	/// Window drag n drop.
	/// </summary>
	class WindowDropEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="paths">Path(s) to the file(s) or folder(s) dropped on the Window.</param>
		/// <param name="window">Pointer to the affected Window.</param>
		explicit WindowDropEvent(std::vector<std::string> paths, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowDropEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowDropEvent(const WindowDropEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowDropEvent& operator=(const WindowDropEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowDropEvent(WindowDropEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowDropEvent& operator=(WindowDropEvent&&) = default;

		/// <summary>
		/// Retrieve the dropped paths.
		/// </summary>
		/// <returns>Vector of file or folder paths.</returns>
		const std::vector<std::string>& GetPaths() const;
		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowDropEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		explicit WindowContentScaleEvent(float xScale, float yScale, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~WindowContentScaleEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		WindowContentScaleEvent(const WindowContentScaleEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		WindowContentScaleEvent& operator=(const WindowContentScaleEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		WindowContentScaleEvent(WindowContentScaleEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		WindowContentScaleEvent& operator=(WindowContentScaleEvent&&) = default;

		/// <summary>
		/// Retrieve the new x content scale.
		/// </summary>
		/// <returns>X content scale.</returns>
		float GetXScale() const;
		/// <summary>
		/// Retrieve the new y content scale.
		/// </summary>
		/// <returns>Y content scale.</returns>
		float GetYScale() const;
		/// <summary>
		/// Retrieve the new content scale.
		/// </summary>
		/// <returns>Content scale.</returns>
		Math::Vec2 GetScale() const;
		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowContentScaleEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		/// <param name="window">Pointer to the affected Window.</param>
		FrameBufferResizeEvent(uint32_t width, uint32_t height, TRAP::Window* window);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~FrameBufferResizeEvent() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		FrameBufferResizeEvent(const FrameBufferResizeEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		FrameBufferResizeEvent& operator=(const FrameBufferResizeEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		FrameBufferResizeEvent(FrameBufferResizeEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		FrameBufferResizeEvent& operator=(FrameBufferResizeEvent&&) = default;

		/// <summary>
		/// Retrieve the new framebuffer width.
		/// </summary>
		/// <returns>Framebuffer width.</returns>
		uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve the new framebuffer height.
		/// </summary>
		/// <returns>Framebuffer height.</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve the new framebuffer size.
		/// </summary>
		/// <returns>Framebuffer size.</returns>
		Math::Vec2ui GetSize() const;
		/// <summary>
		/// Retrieve a pointer to the affected Window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the FrameBufferResizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		uint32_t m_width, m_height;
		TRAP::Window* m_window;
	};
}

#endif /*_TRAP_WINDOWEVENT_H_*/