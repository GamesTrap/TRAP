#ifndef TRAP_WINDOWEVENT_H
#define TRAP_WINDOWEVENT_H

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
		constexpr WindowResizeEvent(uint32_t width, uint32_t height, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowResizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowResizeEvent(const WindowResizeEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowResizeEvent& operator=(const WindowResizeEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowResizeEvent(WindowResizeEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowResizeEvent& operator=(WindowResizeEvent&&) = default;

		/// <summary>
		/// Retrieve the new window width.
		/// </summary>
		/// <returns>Width.</returns>
		constexpr uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve the new window height.
		/// </summary>
		/// <returns>Height.</returns>
		constexpr uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowResizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr WindowMinimizeEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowMinimizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowMinimizeEvent(const WindowMinimizeEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMinimizeEvent& operator=(const WindowMinimizeEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowMinimizeEvent(WindowMinimizeEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowMinimizeEvent& operator=(WindowMinimizeEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowMinimizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr WindowMaximizeEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowMaximizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowMaximizeEvent(const WindowMaximizeEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMaximizeEvent& operator=(const WindowMaximizeEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowMaximizeEvent(WindowMaximizeEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowMaximizeEvent& operator=(WindowMaximizeEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;
		/// <summary>
		/// Get a string representation of the WindowMaximizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr WindowRestoreEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowRestoreEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowRestoreEvent(const WindowRestoreEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowRestoreEvent& operator=(const WindowRestoreEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowRestoreEvent(WindowRestoreEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowRestoreEvent& operator=(WindowRestoreEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowRestoreEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr WindowCloseEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowCloseEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowCloseEvent(const WindowCloseEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowCloseEvent& operator=(const WindowCloseEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowCloseEvent(WindowCloseEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowCloseEvent& operator=(WindowCloseEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowCloseEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		constexpr WindowMoveEvent(int32_t x, int32_t y, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowMoveEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowMoveEvent(const WindowMoveEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowMoveEvent& operator=(const WindowMoveEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowMoveEvent(WindowMoveEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowMoveEvent& operator=(WindowMoveEvent&&) = default;

		/// <summary>
		/// Retrieve the new window x position.
		/// </summary>
		/// <returns>Window x position.</returns>
		constexpr int32_t GetX() const;
		/// <summary>
		/// Retrieve the new window y position.
		/// </summary>
		/// <returns>Window y position.</returns>
		constexpr int32_t GetY() const;
		/// <summary>
		/// Retrieve the new window position.
		/// </summary>
		/// <returns>Window position</returns>
		constexpr Math::Vec2i GetPosition() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowMoveEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr WindowFocusEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowFocusEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowFocusEvent(const WindowFocusEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowFocusEvent& operator=(const WindowFocusEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowFocusEvent(WindowFocusEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowFocusEvent& operator=(WindowFocusEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowFocusEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr WindowLostFocusEvent(TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowLostFocusEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowLostFocusEvent(const WindowLostFocusEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowLostFocusEvent& operator=(const WindowLostFocusEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowLostFocusEvent(WindowLostFocusEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowLostFocusEvent& operator=(WindowLostFocusEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowLostFocusEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

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
		WindowDropEvent(const WindowDropEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowDropEvent& operator=(const WindowDropEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowDropEvent(WindowDropEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowDropEvent& operator=(WindowDropEvent&&) = default;

		/// <summary>
		/// Retrieve the dropped paths.
		/// </summary>
		/// <returns>Vector of file or folder paths.</returns>
		const std::vector<std::string>& GetPaths() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowDropEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		explicit constexpr WindowContentScaleEvent(float xScale, float yScale, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowContentScaleEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowContentScaleEvent(const WindowContentScaleEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowContentScaleEvent& operator=(const WindowContentScaleEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowContentScaleEvent(WindowContentScaleEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowContentScaleEvent& operator=(WindowContentScaleEvent&&) = default;

		/// <summary>
		/// Retrieve the new x content scale.
		/// </summary>
		/// <returns>Window x content scale.</returns>
		constexpr float GetXScale() const;
		/// <summary>
		/// Retrieve the new y content scale.
		/// </summary>
		/// <returns>Window y content scale.</returns>
		constexpr float GetYScale() const;
		/// <summary>
		/// Retrieve the new content scale.
		/// </summary>
		/// <returns>Window content scale.</returns>
		constexpr Math::Vec2 GetScale() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the WindowContentScaleEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
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
		/// <param name="window">Pointer to the affected window.</param>
		constexpr FrameBufferResizeEvent(uint32_t width, uint32_t height, TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~FrameBufferResizeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		FrameBufferResizeEvent(const FrameBufferResizeEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		FrameBufferResizeEvent& operator=(const FrameBufferResizeEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		FrameBufferResizeEvent(FrameBufferResizeEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FrameBufferResizeEvent& operator=(FrameBufferResizeEvent&&) = default;

		/// <summary>
		/// Retrieve the new framebuffer width.
		/// </summary>
		/// <returns>Framebuffer width.</returns>
		constexpr uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve the new framebuffer height.
		/// </summary>
		/// <returns>Framebuffer height.</returns>
		constexpr uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve the new framebuffer size.
		/// </summary>
		/// <returns>Framebuffer size.</returns>
		constexpr Math::Vec2ui GetSize() const;
		/// <summary>
		/// Retrieve a pointer to the affected window.
		/// </summary>
		/// <returns>Window pointer.</returns>
		constexpr TRAP::Window* GetWindow() const;

		/// <summary>
		/// Get a string representation of the FrameBufferResizeEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
		uint32_t m_width, m_height;
		TRAP::Window* m_window;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowResizeEvent::WindowResizeEvent(const uint32_t width, const uint32_t height,
   															 TRAP::Window* window)
	: m_width(width), m_height(height), m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP::Events::WindowResizeEvent::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP::Events::WindowResizeEvent::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowResizeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowResizeEvent::GetStaticType()
{
	return EventType::WindowResize;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowMinimizeEvent::WindowMinimizeEvent(TRAP::Window* window)
	: m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowMinimizeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowMinimizeEvent::GetStaticType()
{
	return EventType::WindowMinimize;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowMaximizeEvent::WindowMaximizeEvent(TRAP::Window* window)
	: m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowMaximizeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowMaximizeEvent::GetStaticType()
{
	return EventType::WindowMaximize;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowRestoreEvent::WindowRestoreEvent(TRAP::Window* window)
	: m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowRestoreEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowRestoreEvent::GetStaticType()
{
	return EventType::WindowRestore;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowCloseEvent::WindowCloseEvent(TRAP::Window* window)
	: m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowCloseEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowCloseEvent::GetStaticType()
{
	return EventType::WindowClose;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowMoveEvent::WindowMoveEvent(const int32_t x, const int32_t y, TRAP::Window* window)
	: m_x(x), m_y(y), m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr int32_t TRAP::Events::WindowMoveEvent::GetX() const
{
	return m_x;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr int32_t TRAP::Events::WindowMoveEvent::GetY() const
{
	return m_y;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Math::Vec2i TRAP::Events::WindowMoveEvent::GetPosition() const
{
	return {m_x, m_y};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowMoveEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowMoveEvent::GetStaticType()
{
	return EventType::WindowMove;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowFocusEvent::WindowFocusEvent(TRAP::Window* window)
	: m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowFocusEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowFocusEvent::GetStaticType()
{
	return EventType::WindowFocus;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowLostFocusEvent::WindowLostFocusEvent(TRAP::Window* window)
	: m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowLostFocusEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowLostFocusEvent::GetStaticType()
{
	return EventType::WindowLostFocus;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowDropEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowDropEvent::GetStaticType()
{
	return EventType::WindowDrop;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::WindowContentScaleEvent::WindowContentScaleEvent(const float xScale, const float yScale,
	TRAP::Window* window)
	: m_XScale(xScale), m_YScale(yScale), m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr float TRAP::Events::WindowContentScaleEvent::GetXScale() const
{
	return m_XScale;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr float TRAP::Events::WindowContentScaleEvent::GetYScale() const
{
	return m_YScale;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Math::Vec2 TRAP::Events::WindowContentScaleEvent::GetScale() const
{
	return {m_XScale, m_YScale};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::WindowContentScaleEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::WindowContentScaleEvent::GetStaticType()
{
	return EventType::WindowContentScale;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::FrameBufferResizeEvent::FrameBufferResizeEvent(const uint32_t width, const uint32_t height,
                                                             TRAP::Window* window)
	: m_width(width), m_height(height), m_window(window)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP::Events::FrameBufferResizeEvent::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr uint32_t TRAP::Events::FrameBufferResizeEvent::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Math::Vec2ui TRAP::Events::FrameBufferResizeEvent::GetSize() const
{
	return {m_width, m_height};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Window* TRAP::Events::FrameBufferResizeEvent::GetWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::FrameBufferResizeEvent::GetStaticType()
{
	return EventType::FrameBufferResize;
}

#endif /*TRAP_WINDOWEVENT_H*/