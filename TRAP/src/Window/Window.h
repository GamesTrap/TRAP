#ifndef TRAP_WINDOW_H
#define TRAP_WINDOW_H

#ifndef TRAP_HEADLESS_MODE

#include "Window/Monitor.h"
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
		constexpr Window(const Window&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Window& operator=(const Window&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Window(Window&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Window& operator=(Window&&) noexcept = default;
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
		[[nodiscard]] static u32 GetActiveWindows() noexcept;

		/// <summary>
		/// Get the current title of the window.
		/// </summary>
		/// <returns>Title of the window.</returns>
		[[nodiscard]] constexpr std::string GetTitle() const noexcept;
		/// <summary>
		/// Get the current width of the window.
		/// </summary>
		/// <returns>Width of the window.</returns>
		[[nodiscard]] u32 GetWidth() const noexcept;
		/// <summary>
		/// Get the current height of the window.
		/// </summary>
		/// <returns>Height of the window.</returns>
		[[nodiscard]] u32 GetHeight() const noexcept;
		/// <summary>
		/// Get the current width and height of the window.
		/// </summary>
		/// <returns>Vec2ui containing the width and height of the window.</returns>
		[[nodiscard]] Math::Vec2ui GetSize() const noexcept;
		/// <summary>
		/// Get the current framebuffer width of the window.
		/// </summary>
		/// <returns>Framebuffer width of the window.</returns>
		[[nodiscard]] u32 GetFrameBufferWidth() const;
		/// <summary>
		/// Get the current framebuffer height of the window.
		/// </summary>
		/// <returns>Framebuffer height of the window.</returns>
		[[nodiscard]] u32 GetFrameBufferHeight() const;
		/// <summary>
		/// Get the current framebuffer width and height of the window.
		/// </summary>
		/// <returns>Vec2ui containing the framebuffer width and height of the window.</returns>
		[[nodiscard]] Math::Vec2ui GetFrameBufferSize() const;
		/// <summary>
		/// Get the current position of the window.
		/// Retrieves the position, in screen coordinates, of the upper-left corner of the content area.
		/// </summary>
		/// <returns>Vec2i containing the position of the window.</returns>
		[[nodiscard]] Math::Vec2i GetPosition() const;
		/// <summary>
		/// Get the current refresh rate of the window.
		/// </summary>
		/// <returns>Refresh rate of the window.</returns>
		[[nodiscard]] f64 GetRefreshRate() const noexcept;
		/// <summary>
		/// Get the current display mode of the window.
		/// </summary>
		/// <returns>Display mode of the window.</returns>
		[[nodiscard]] constexpr DisplayMode GetDisplayMode() const noexcept;
		/// <summary>
		/// Get the current monitor used by the window
		/// (only for display modes fullscreen and borderless).
		/// </summary>
		/// <returns>Object of monitor class used by the window.</returns>
		[[nodiscard]] Monitor GetMonitor() const;
		/// <summary>
		/// Get the current cursor mode of the window.
		/// </summary>
		/// <returns>Cursor mode of the window.</returns>
		[[nodiscard]] CursorMode GetCursorMode() const noexcept;
		/// <summary>
		/// Get the current raw mouse input (false = off, true = on) usage of the window.
		/// </summary>
		/// <returns>Raw mouse input status of the window.</returns>
		[[nodiscard]] bool GetRawMouseInput() const noexcept;
		/// <summary>
		/// Get the ratio between the current DPI and the platforms DPI.
		/// </summary>
		/// <returns>Vec2 containing the x and y scale.</returns>
		[[nodiscard]] Math::Vec2 GetContentScale() const;
		/// <summary>
		/// Get the current opacity of the window.
		/// </summary>
		/// <returns>Opacity of the window on success, empty optional otherwise.</returns>
		[[nodiscard]] std::optional<f32> GetOpacity() const;
		/// <summary>
		/// Get whether VSync is enabled or disabled.
		///
		/// Same as calling TRAP::Graphics::RendererAPI::GetRenderer()->GetVSync().
		/// </summary>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		[[nodiscard]] bool GetVSync() const noexcept;
		/// <summary>
		/// Get the aspect ratio of the window.
		/// </summary>
		/// <returns>Aspect ratio of window framebuffer.</returns>
		[[nodiscard]] f32 GetAspectRatio() const;

		/// <summary>
		/// Get the internal handle of the window.
		/// </summary>
		/// <returns>Pointer to the internal window handle.</returns>
		[[nodiscard]] constexpr void* GetInternalWindow() const noexcept;

		/// <summary>
		/// Set a new title for the window.
		/// </summary>
		/// <param name="title">New window title.</param>
		void SetTitle(const std::string& title);
		/// <summary>
		/// Set the window to fullscreen display mode using
		/// the monitors native video mode.
		/// </summary>
		void SetFullscreen();
		/// <summary>
		/// Set the window to fullscreen display mode using the given video mode.
		///
		/// Note: If the given video mode is not supported by the monitor
		///       then this function does nothing.
		/// </summary>
		/// <param name="videoMode">Video mode to use.</param>
		void SetFullscreen(const Monitor::VideoMode& videoMode);
		/// <summary>
		/// Set the window to fullscreen borderless display mode.
		/// </summary>
		void SetFullscreenBorderless();
		/// <summary>
		/// Set the window to windowed display mode using the previously used window size.
		/// </summary>
		void SetWindowed();
		/// <summary>
		/// Set the Windowed object
		/// </summary>
		/// <param name="width">New window width.</param>
		/// <param name="height">New window height.</param>
		void SetWindowed(u32 width, u32 height);
		/// <summary>
		/// Set a new monitor for the window.
		///
		/// Note: If the current display mode is fullscreen or fullscreen borderless,
		///       the window will use the new monitors native resolution.
		/// </summary>
		/// <param name="monitor">Monitor object to be used from now on.</param>
		void SetMonitor(const Monitor& monitor);
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
		void SetCursorIcon(const Image* image, u32 xHotspot = 0, u32 yHotspot = 0) const;
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
		/// <param name="progress">How much has been completed. Valid range [0.0-1.0].</param>
		void SetProgress(ProgressState state, f64 progress) const;
		/// <summary>
		/// Resets the window icon to the TRAP logo.
		/// </summary>
		void SetIcon() const;
		/// <summary>
		/// Set a custom logo as window icon (image must be non HDR RGBA 32 bits per pixel)
		/// (nullptr resets the icon to the TRAP logo).
		/// </summary>
		/// <param name="image">New logo to be used by the window.</param>
		void SetIcon(const Image* image) const;
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
		void SetMinimumSize(u32 minWidth, u32 minHeight);
		/// <summary>
		/// Limit the maximum size of the window.
		/// Setting any of the two parameters to 0 will disable the maximum size restriction.
		/// </summary>
		/// <param name="maxWidth">Max width.</param>
		/// <param name="maxHeight">Max height.</param>
		void SetMaximumSize(u32 maxWidth, u32 maxHeight);
		/// <summary>
		/// Set the aspect ratio of the window.
		/// This makes sure that the window size always matches the specified aspect ratio.
		///
		/// Set both parameters to 0 to disable the aspect ratio restriction.
		///
		/// Note: This only takes effect in windowed mode.
		///       This function does nothing if the window is not resizable.
		///       If you set size limits and an aspect ratio that conflict, the results are undefined.
		//        The aspect ratio is applied immediately to a windowed mode window and may cause it to be resized.
		/// </summary>
		/// <param name="numerator">Numerator of the desired aspect ratio, or 0.</param>
		/// <param name="denominator">Denominator of the desired aspect ratio, or 0.</param>
		void SetAspectRatio(u32 numerator, u32 denominator);
		/// <summary>
		/// Set the position of the window.
		/// </summary>
		/// <param name="x">New x position.</param>
		/// <param name="y">New x position.</param>
		void SetPosition(u32 x, u32 y);
		/// <summary>
		/// Set the opacity of the window (1.0f = fully opaque, 0.0f = fully transparent).
		/// </summary>
		/// <param name="opacity">Opacity strength.</param>
		void SetOpacity(f32 opacity) const;
		/// <summary>
		/// Enable or disable drag and drop feature for the window.
		/// </summary>
		/// <param name="enabled">True to enable, false otherwise.</param>
		void SetDragAndDrop(bool enabled) const;
		/// <summary>
		/// Enable or disable VSync for the window.
		///
		/// Same as calling TRAP::Graphics::RendererAPI::GetRenderer()->SetVSync().
		/// </summary>
		/// <param name="enabled">Whether to enable VSync or not.</param>
		void SetVSync(bool enabled) const;

		/// <summary>
		/// Query whether the window is maximized or not.
		/// </summary>
		/// <returns>Window maximization status.</returns>
		[[nodiscard]] bool IsMaximized() const;
		/// <summary>
		/// Query whether the window is minimized or not.
		/// </summary>
		/// <returns>Window minimization status.</returns>
		[[nodiscard]] bool IsMinimized() const;
		/// <summary>
		/// Query whether the window is resizable or not.
		/// </summary>
		/// <returns>Window resizable status.</returns>
		[[nodiscard]] bool IsResizable() const;
		/// <summary>
		/// Query whether the window is visible or not.
		/// </summary>
		/// <returns>Window visibility status.</returns>
		[[nodiscard]] bool IsVisible() const;
		/// <summary>
		/// Query whether the window is focused or not.
		/// </summary>
		/// <returns>Window focus status.</returns>
		[[nodiscard]] bool IsFocused() const;
		/// <summary>
		/// Query whether the window is decorated or not.
		/// </summary>
		/// <returns>Window decoration status.</returns>
		[[nodiscard]] bool IsDecorated() const;
		/// <summary>
		/// Query whether the mouse is hovered on the window or not.
		/// </summary>
		/// <returns>Window mouse hover status.</returns>
		[[nodiscard]] bool IsHovered() const;

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

		INTERNAL::WindowingAPI::InternalWindow* m_window; //Handle to the internal window

		/// <summary>
		/// Used when switching between fullscreen <-> windowed mode.
		/// </summary>
		struct WindowedModeParams
		{
			i32 Width = 800, Height = 600;
			i32 XPos = 0, YPos = 0;
		};

		struct WindowData
		{
			std::string Title = "TRAP™";
			i32 MinWidth = -1, MinHeight = -1;
			i32 MaxWidth = -1, MaxHeight = -1;
			DisplayMode displayMode = DisplayMode::Windowed;
			TRAP::Monitor Monitor = TRAP::Monitor::GetPrimaryMonitor();

			EventCallbackFn EventCallback;

			WindowedModeParams windowModeParams{};
			Window* Win = nullptr;
		} m_data;

		inline constinit static u32 s_windows = 0;

		inline static constexpr i32 MinimumSupportedWindowWidth = 2;
		inline static constexpr i32 MinimumSupportedWindowHeight = 2;
	};

	/// <summary>
	/// Struct containing all Window properties needed to create a new TRAP::Window.
	/// </summary>
	struct WindowProps
	{
		std::string Title;
		u32 Width;
		u32 Height;
		f64 RefreshRate;
		bool VSync;
		Window::DisplayMode DisplayMode;
		TRAP::Monitor Monitor;

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
			constexpr explicit AdvancedProps(bool resizable = true,
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
							 u32 width = 1280u,
							 u32 height = 720u,
							 f64 refreshRate = 60.0,
							 bool vsync = false,
							 Window::DisplayMode displayMode = Window::DisplayMode::Windowed,
							 AdvancedProps advanced = AdvancedProps{},
							 const TRAP::Monitor& monitor = TRAP::Monitor::GetPrimaryMonitor()) noexcept;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Window::GetTitle() const noexcept
{
	return m_data.Title;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Window::DisplayMode TRAP::Window::GetDisplayMode() const noexcept
{
	return m_data.displayMode;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr void* TRAP::Window::GetInternalWindow() const noexcept
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::WindowProps::AdvancedProps::AdvancedProps(const bool resizable,
                                                          const bool maximized,
                                                          const bool visible,
                                                          const bool focused,
                                                          const bool focusOnShow,
                                                          const bool decorated,
                                                          const bool rawMouseInput,
                                                          const Window::CursorMode cursorMode) noexcept
	: Resizable(resizable),
	  Maximized(maximized),
	  Visible(visible),
	  Focused(focused),
	  FocusOnShow(focusOnShow),
	  Decorated(decorated),
	  RawMouseInput(rawMouseInput),
	  CursorMode(cursorMode)
{
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_WINDOW_H*/