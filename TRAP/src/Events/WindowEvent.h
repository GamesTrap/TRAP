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