#ifndef TRAP_WINDOW_H
#define TRAP_WINDOW_H

#include "Events/Event.h"
#include "ImageLoader/Image.h"
#include "WindowingAPI.h"

namespace TRAP
{
	struct WindowProps;
	class Monitor;

	/// <summary>
	/// Class representing a desktop system based window.
	/// </summary>
	class Window final
	{
	public:
		/// <summary>
		/// Specifies the display mode of a window.
		/// </summary>
		enum class DisplayMode
		{
			Windowed,
			Fullscreen,
			Borderless
		};

		/// <summary>
		/// Same as WindowingAPI::CursorMode.
		/// Specifies the cursor mode of a window.
		/// </summary>
		using CursorMode = INTERNAL::WindowingAPI::CursorMode;

		/// <summary>
		/// Same as WindowingAPI::CursorType.
		/// Specifies the cursor type (visual appearance) of a window.
		/// </summary>
		using CursorType = INTERNAL::WindowingAPI::CursorType;

		/// <summary>
		/// Same as WindowingAPI::ProgressState.
		/// Specifies the progress state of a window.
		/// </summary>
		using ProgressState = INTERNAL::WindowingAPI::ProgressState;

		using EventCallbackFn = std::function<void(Events::Event&)>;

		/// <summary>
		/// Create a new Window.
		/// </summary>
		/// <param name="props">Properties to be applied to the new window.</param>
		/// <returns>New window object.</returns>
		explicit Window(const WindowProps& props);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Window(const Window&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Window& operator=(const Window&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Window(Window&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Window& operator=(Window&&) = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~Window();

		/// <summary>
		/// Updates all windows (polls events from OS etc.).
		/// </summary>
		static void OnUpdate();

		/// <summary>
		/// Get the amount of all active windows.
		/// </summary>
		/// <returns>Total amount of active windows.</returns>
		static uint32_t GetActiveWindows() noexcept;

		/// <summary>
		/// Get the current title of the window.
		/// </summary>
		/// <returns>Title of the window.</returns>
		std::string GetTitle() const noexcept;
		/// <summary>
		/// Get the current width of the window.
		/// </summary>
		/// <returns>Width of the window.</returns>
		uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Get the current height of the window.
		/// </summary>
		/// <returns>Height of the window.</returns>
		uint32_t GetHeight() const noexcept;
		/// <summary>
		/// Get the current width and height of the window.
		/// </summary>
		/// <returns>Vec2ui containing the width and height of the window.</returns>
		Math::Vec2ui GetSize() const noexcept;
		/// <summary>
		/// Get the current framebuffer width and height of the window.
		/// </summary>
		/// <returns>Vec2ui containing the framebuffer width and height of the window.</returns>
		Math::Vec2ui GetFrameBufferSize() const noexcept;
		/// <summary>
		/// Get the current position of the window.
		/// Retrieves the position, in screen coordinates, of the upper-left corner of the content area.
		/// </summary>
		/// <returns>Vec2i containing the position of the window.</returns>
		Math::Vec2i GetPosition() const noexcept;
		/// <summary>
		/// Get the current refresh rate of the window.
		/// </summary>
		/// <returns>Refresh rate of the window.</returns>
		uint32_t GetRefreshRate() const noexcept;
		/// <summary>
		/// Get the current display mode of the window.
		/// </summary>
		/// <returns>Display mode of the window.</returns>
		DisplayMode GetDisplayMode() const noexcept;
		/// <summary>
		/// Get the current monitor used by the window
		/// (only for display modes fullscreen and borderless).
		/// </summary>
		/// <returns>Object of monitor class used by the window.</returns>
		Monitor GetMonitor() const;
		/// <summary>
		/// Get the current cursor mode of the window.
		/// </summary>
		/// <returns>Cursor mode of the window.</returns>
		CursorMode GetCursorMode() const noexcept;
		/// <summary>
		/// Get the current raw mouse input (false = off, true = on) usage of the window.
		/// </summary>
		/// <returns>Raw mouse input status of the window.</returns>
		bool GetRawMouseInput() const noexcept;
		/// <summary>
		/// Get the ratio between the current DPI and the platforms DPI.
		/// </summary>
		/// <returns>Vec2 containing the x and y scale.</returns>
		Math::Vec2 GetContentScale() const;
		/// <summary>
		/// Get the current opacity of the window.
		/// </summary>
		/// <returns>Opacity of the window.</returns>
		float GetOpacity() const;
		/// <summary>
		/// Get whether VSync is enabled or disabled.
		/// </summary>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		bool GetVSync() const noexcept;

		/// <summary>
		/// Get the internal handle of the window.
		/// </summary>
		/// <returns>Pointer to the internal window handle.</returns>
		void* GetInternalWindow() const;

		/// <summary>
		/// Set a new title for the window.
		/// </summary>
		/// <param name="title">New window title.</param>
		void SetTitle(const std::string& title);
		/// <summary>
		/// Set a new display mode and or a new size/refresh rate for the window.
		/// </summary>
		/// <param name="mode">New display mode for the window.</param>
		/// <param name="width">New width (ignored when used with display mode borderless).</param>
		/// <param name="height">New height (ignored when used with display mode borderless).</param>
		/// <param name="refreshRate">
		/// New refresh rate (ignored when used with display modes Borderless or Windowed).
		/// </param>
		void SetDisplayMode(const DisplayMode& mode,
			                uint32_t width = 0,
			                uint32_t height = 0,
			                uint32_t refreshRate = 0);
		/// <summary>
		/// Set a new monitor for the window.
		/// </summary>
		/// <param name="monitor">Monitor object to be used from now on.</param>
		void SetMonitor(Monitor& monitor);
		/// <summary>
		/// Set the cursor mode for the window.
		/// </summary>
		/// <param name="mode">New cursor mode.</param>
		void SetCursorMode(const CursorMode& mode);
		/// <summary>
		/// Set the cursor type for the window.
		/// </summary>
		/// <param name="cursor">New cursor type.</param>
		void SetCursorType(const CursorType& cursor) const;
		/// <summary>
		/// Set the cursor to a custom image (image must be non HDR RGB/RGBA with 24/32 bits per pixel).
		/// </summary>
		/// <param name="image">Image to be used as a cursor.</param>
		/// <param name="xHotspot">X hotspot of the image.</param>
		/// <param name="yHotspot">Y hotspot of the image.</param>
		void SetCursorIcon(const Image* const image, int32_t xHotspot = 0, int32_t yHotspot = 0) const;
		/// <summary>
		/// Set if raw mouse input should be used.
		/// </summary>
		/// <param name="enabled">New status for raw mouse input.</param>
		void SetRawMouseInput(bool enabled);
		/// <summary>
		/// Sets the progress on the taskbar for the specified window.
		///
		/// Linux: This only works on KDE & Unity environments.
		///        A .desktop file must exist for the application with the same name as given to TRAP::Application.
		/// </summary>
		/// <param name="state">State of progress.</param>
		/// <param name="progress">How much has been completed. Valid values: 0 - 100.</param>
		void SetProgress(ProgressState state, uint32_t progress);
		/// <summary>
		/// Resets the window icon to the TRAP logo.
		/// </summary>
		void SetIcon() const;
		/// <summary>
		/// Set a custom logo as window icon (image must be non HDR RGBA 32 bits per pixel)
		/// (nullptr resets the icon to the TRAP logo).
		/// </summary>
		/// <param name="image">New logo to be used by the window.</param>
		void SetIcon(const Image* const image) const;
		/// <summary>
		/// Set the function where events should be reported to from the window.
		/// </summary>
		/// <param name="callback">Callback function used to report events to.</param>
		void SetEventCallback(const EventCallbackFn& callback) noexcept;
		/// <summary>
		/// Set whether resizing of the window should be allowed.
		/// </summary>
		/// <param name="enabled">Set resizable or non resizable.</param>
		void SetResizable(bool enabled) const;
		/// <summary>
		/// Limit the minimum size of the window.
		/// Setting any of the two parameters to 0 will disable the minimum size restriction.
		/// </summary>
		/// <param name="minWidth">Min width.</param>
		/// <param name="minHeight">Min height.</param>
		void SetMinimumSize(uint32_t minWidth, uint32_t minHeight);
		/// <summary>
		/// Limit the maximum size of the window.
		/// Setting any of the two parameters to 0 will disable the maximum size restriction.
		/// </summary>
		/// <param name="maxWidth">Max width.</param>
		/// <param name="maxHeight">Max height.</param>
		void SetMaximumSize(uint32_t maxWidth, uint32_t maxHeight);
		/// <summary>
		/// Set the position of the window.
		/// </summary>
		/// <param name="x">New x position.</param>
		/// <param name="y">New x position.</param>
		void SetPosition(uint32_t x, uint32_t y);
		/// <summary>
		/// Set the opacity of the window (1.0f = fully opaque, 0.0f = fully transparent).
		/// </summary>
		/// <param name="opacity">Opacity strength.</param>
		void SetOpacity(float opacity) const;
		/// <summary>
		/// Enable or disable drag and drop feature for the window.
		/// </summary>
		/// <param name="enabled">True to enable, false otherwise.</param>
		void SetDragAndDrop(bool enabled) const;
		/// <summary>
		/// Enable or disable VSync for the window.
		/// </summary>
		/// <param name="enabled">Whether to enable VSync or not.</param>
		void SetVSync(bool enabled);

		/// <summary>
		/// Query whether the window is maximized or not.
		/// </summary>
		/// <returns>Window maximization status.</returns>
		bool IsMaximized() const;
		/// <summary>
		/// Query whether the window is minimized or not.
		/// </summary>
		/// <returns>Window minimization status.</returns>
		bool IsMinimized() const;
		/// <summary>
		/// Query whether the window is resizable or not.
		/// </summary>
		/// <returns>Window resizable status.</returns>
		bool IsResizable() const;
		/// <summary>
		/// Query whether the window is visible or not.
		/// </summary>
		/// <returns>Window visibility status.</returns>
		bool IsVisible() const;
		/// <summary>
		/// Query whether the window is focused or not.
		/// </summary>
		/// <returns>Window focus status.</returns>
		bool IsFocused() const;
		/// <summary>
		/// Query whether the window is decorated or not.
		/// </summary>
		/// <returns>Window decoration status.</returns>
		bool IsDecorated() const;
		/// <summary>
		/// Query whether the mouse is hovered on the window or not.
		/// </summary>
		/// <returns>Window mouse hover status.</returns>
		bool IsHovered() const;

		/// <summary>
		/// Maximize the window.
		/// Note: This function only affects the window when using DisplayMode::Windowed.
		/// </summary>
		void Maximize() const;
		/// <summary>
		/// Minimize the window.
		/// </summary>
		void Minimize() const;
		/// <summary>
		/// Request attention/focus for the window.
		/// </summary>
		void RequestAttention() const;
		/// <summary>
		/// Focus the window.
		/// </summary>
		void Focus() const;
		/// <summary>
		/// Hide the window.
		/// </summary>
		void Hide() const;
		/// <summary>
		/// Show the window.
		/// </summary>
		void Show() const;
		/// <summary>
		/// Restore/Show the window.
		/// </summary>
		void Restore() const;

	private:
		/// <summary>
		/// Initializes a new window and also if not happened already the renderer.
		/// </summary>
		/// <param name="props">Window properties for the new window.</param>
		void Init(const WindowProps& props);
		/// <summary>
		/// Shutdown current window and if there are no other open windows also
		/// deinitializes the internal WindowingAPI.
		/// </summary>
		void Shutdown();
		/// <summary>
		/// Setup how to handle/process/dispatch incoming events from the internal WindowingAPI.
		/// </summary>
		void SetupEventCallbacks();

		Scope<INTERNAL::WindowingAPI::InternalWindow> m_window;
		INTERNAL::WindowingAPI::InternalMonitor* m_useMonitor; //Stores a reference to the monitor
		//Stores the underlying video mode being used by the OS for every monitor
		static std::unordered_map<std::size_t, INTERNAL::WindowingAPI::InternalVideoMode> s_baseVideoModes;

		/// <summary>
		/// Used when switched back from fullscreen to windowed mode.
		/// </summary>
		struct WindowedModeParams
		{
			int32_t Width = 800, Height = 600, RefreshRate;
			int32_t XPos, YPos;
		};

		struct WindowData
		{
			std::string Title;
			int32_t Width{}, Height{}, RefreshRate{};
			int32_t MinWidth = -1, MinHeight = -1;
			int32_t MaxWidth = -1, MaxHeight = -1;
			bool VSync{};
			DisplayMode displayMode{};
			uint32_t Monitor{};
			CursorMode cursorMode{};
			bool RawMouseInput{};
			std::unordered_map<uint16_t, uint32_t> KeyRepeatCounts;

			EventCallbackFn EventCallback;

			WindowedModeParams windowModeParams{};
			Window* Win = nullptr;
		} m_data;

		static uint32_t s_windows;
		static bool s_WindowingAPIInitialized;
		static std::vector<Window*> s_fullscreenWindows;

		inline static constexpr int32_t MinimumSupportedWindowWidth = 2;
		inline static constexpr int32_t MinimumSupportedWindowHeight = 2;
	};

	/// <summary>
	/// Struct containing all Window properties needed to create a new TRAP::Window.
	/// </summary>
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		uint32_t RefreshRate;
		bool VSync;
		Window::DisplayMode DisplayMode;
		uint32_t Monitor;

		/// <summary>
		/// More advanced(optional) window properties.
		/// </summary>
		struct AdvancedProps
		{
			bool Resizable = true;
			bool Maximized = false;
			bool Visible = true;
			bool Focused = true;
			bool FocusOnShow = true;
			bool Decorated = true;
			bool RawMouseInput = false;
			Window::CursorMode CursorMode = Window::CursorMode::Normal;

			/// <summary>
			/// Constructor for advanced(optional) window properties.
			/// </summary>
			/// <param name="resizable">Whether the window should be resizable.</param>
			/// <param name="maximized">Whether the window should be maximized on creation.</param>
			/// <param name="visible">Whether the window should be visible or not.</param>
			/// <param name="focused">Whether the window should be focused or not.</param>
			/// <param name="focusOnShow">Whether the window should be focused when shown or not.</param>
			/// <param name="decorated">Whether the window should be decorated or not.</param>
			/// <param name="rawMouseInput">Whether the window should use raw mouse input or not.</param>
			/// <param name="cursorMode">Cursor mode to be used by the window.</param>
			explicit AdvancedProps(bool resizable = true,
			                       bool maximized = false,
			                       bool visible = true,
			                       bool focused = true,
			                       bool focusOnShow = true,
			                       bool decorated = true,
			                       bool rawMouseInput = false,
			                       Window::CursorMode cursorMode = Window::CursorMode::Normal) noexcept;
		} Advanced{};

		/// <summary>
		/// Constructor for basic window properties.
		/// </summary>
		/// <param name="title">Title for the new window.</param>
		/// <param name="width">Width for the new window.</param>
		/// <param name="height">Height for the new window.</param>
		/// <param name="refreshRate">Refresh rate for the new window.</param>
		/// <param name="vsync">Whether to enable VSync for the new window or not.</param>
		/// <param name="displayMode">Display mode for the new window.</param>
		/// <param name="advanced">Optional advanced properties to be used for the new window.</param>
		/// <param name="monitor">Monitor to be used by the new window.</param>
		explicit WindowProps(std::string title = "TRAP™",
							 uint32_t width = 1280,
							 uint32_t height = 720,
							 uint32_t refreshRate = 60,
							 bool vsync = false,
							 Window::DisplayMode displayMode = Window::DisplayMode::Windowed,
							 AdvancedProps advanced = AdvancedProps{},
		                     uint32_t monitor = 0) noexcept;
	};
}

#endif /*TRAP_WINDOW_H*/