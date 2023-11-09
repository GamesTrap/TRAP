#ifndef TRAP_WINDOW_H
#define TRAP_WINDOW_H

#ifndef TRAP_HEADLESS_MODE

#include "Window/Monitor.h"
#include "WindowingAPI.h"

namespace TRAP
{
	struct WindowProps;
	class Monitor;

	/// @brief Class representing a desktop system based window.
	/// @remark @headless This class is not available in headless mode.
	class Window final
	{
	public:
		/// @brief Specifies the display mode of a window.
		enum class DisplayMode
		{
			Windowed,
			Fullscreen,
			Borderless
		};

		/// @brief Same as WindowingAPI::CursorMode.
		/// Specifies the cursor mode of a window.
		using CursorMode = INTERNAL::WindowingAPI::CursorMode;

		/// @brief Same as WindowingAPI::CursorType.
		/// Specifies the cursor type (visual appearance) of a window.
		using CursorType = INTERNAL::WindowingAPI::CursorType;

		/// @brief Same as WindowingAPI::ProgressState.
		/// Specifies the progress state of a window.
		using ProgressState = INTERNAL::WindowingAPI::ProgressState;

		using EventCallbackFn = std::function<void(Events::Event&)>;

		/// @brief Create a new Window.
		/// @param props Properties to be applied to the new window.
		explicit Window(const WindowProps& props);
		/// @brief Copy constructor.
		consteval Window(const Window&) = delete;
		/// @brief Copy assignment operator.
		consteval Window& operator=(const Window&) = delete;
		/// @brief Move constructor.
		Window(Window&&) noexcept = default;
		/// @brief Move assignment operator.
		Window& operator=(Window&&) noexcept = default;
		/// @brief Destructor.
		~Window();

		/// @brief Updates all windows (polls events from OS etc.).
		static void OnUpdate();

		/// @brief Get the amount of all active windows.
		/// @return Total amount of active windows.
		[[nodiscard]] static u32 GetActiveWindows() noexcept;

		/// @brief Get the current title of the window.
		/// @return Title of the window.
		[[nodiscard]] constexpr std::string GetTitle() const noexcept;
		/// @brief Get the current width of the window.
		/// @return Width of the window.
		[[nodiscard]] u32 GetWidth() const noexcept;
		/// @brief Get the current height of the window.
		/// @return Height of the window.
		[[nodiscard]] u32 GetHeight() const noexcept;
		/// @brief Get the current width and height of the window.
		/// @return Vec2ui containing the width and height of the window.
		[[nodiscard]] Math::Vec2ui GetSize() const noexcept;
		/// @brief Get the current framebuffer width of the window.
		/// @return Framebuffer width of the window.
		[[nodiscard]] u32 GetFrameBufferWidth() const;
		/// @brief Get the current framebuffer height of the window.
		/// @return Framebuffer height of the window.
		[[nodiscard]] u32 GetFrameBufferHeight() const;
		/// @brief Get the current framebuffer width and height of the window.
		/// @return Vec2ui containing the framebuffer width and height of the window.
		[[nodiscard]] Math::Vec2ui GetFrameBufferSize() const;
		/// @brief Get the current position of the window.
		/// Retrieves the position, in screen coordinates, of the upper-left corner of the content area.
		/// @return Vec2i containing the position of the window.
		[[nodiscard]] Math::Vec2i GetPosition() const;
		/// @brief Get the current refresh rate of the window.
		/// @return Refresh rate of the window.
		[[nodiscard]] f64 GetRefreshRate() const noexcept;
		/// @brief Get the current display mode of the window.
		/// @return Display mode of the window.
		[[nodiscard]] constexpr DisplayMode GetDisplayMode() const noexcept;
		/// @brief Get the current monitor used by the window
		/// (only for display modes fullscreen and borderless).
		/// @return Object of monitor class used by the window.
		[[nodiscard]] Monitor GetMonitor() const;
		/// @brief Get the current cursor mode of the window.
		/// @return Cursor mode of the window.
		[[nodiscard]] CursorMode GetCursorMode() const noexcept;
		/// @brief Get the current raw mouse input (false = off, true = on) usage of the window.
		/// @return Raw mouse input status of the window.
		[[nodiscard]] bool GetRawMouseInput() const noexcept;
		/// @brief Get the ratio between the current DPI and the platforms DPI.
		/// @return Vec2 containing the x and y scale.
		[[nodiscard]] Math::Vec2 GetContentScale() const;
		/// @brief Get the current opacity of the window.
		/// @return Opacity of the window on success, empty optional otherwise.
		[[nodiscard]] std::optional<f32> GetOpacity() const;
		/// @brief Get whether VSync is enabled or disabled.
		///
		/// Same as calling TRAP::Graphics::RendererAPI::GetRenderer()->GetVSync().
		/// @return True if VSync is enabled, false otherwise.
		[[nodiscard]] bool GetVSync() const noexcept;
		/// @brief Get the aspect ratio of the window.
		/// @return Aspect ratio of window framebuffer.
		[[nodiscard]] f32 GetAspectRatio() const;

		/// @brief Get the internal handle of the window.
		/// @return Pointer to the internal window handle.
		[[nodiscard]] constexpr void* GetInternalWindow() const noexcept;

		/// @brief Set a new title for the window.
		/// @param title New window title.
		void SetTitle(const std::string& title);
		/// @brief Set the window to fullscreen display mode using
		/// the monitors native video mode.
		void SetFullscreen();
		/// @brief Set the window to fullscreen display mode using the given video mode.
		/// @param videoMode Video mode to use.
		/// @note If the given video mode is not supported by the monitor then this function does nothing.
		void SetFullscreen(const Monitor::VideoMode& videoMode);
		/// @brief Set the window to fullscreen borderless display mode.
		void SetFullscreenBorderless();
		/// @brief Set the window to windowed display mode using the previously used window size.
		void SetWindowed();
		/// @brief Set the Windowed object
		/// @param width New window width.
		/// @param height New window height.
		void SetWindowed(u32 width, u32 height);
		/// @brief Set a new monitor for the window.
		/// @param monitor Monitor object to be used from now on.
		/// @note If the current display mode is fullscreen or fullscreen borderless,
		///       the window will use the new monitors native resolution.
		void SetMonitor(const Monitor& monitor);
		/// @brief Set the cursor mode for the window.
		/// @param mode New cursor mode.
		void SetCursorMode(const CursorMode& mode);
		/// @brief Set the cursor type for the window.
		/// @param cursor New cursor type.
		void SetCursorType(const CursorType& cursor) const;
		/// @brief Set the cursor to a custom image (image must be non HDR RGB/RGBA with 24/32 bits per pixel).
		/// @param image Image to be used as a cursor.
		/// @param xHotspot X hotspot of the image.
		/// @param yHotspot Y hotspot of the image.
		void SetCursorIcon(const Image* image, u32 xHotspot = 0, u32 yHotspot = 0) const;
		/// @brief Set if raw mouse input should be used.
		/// @param enabled New status for raw mouse input.
		void SetRawMouseInput(bool enabled);
		/// @brief Sets the progress on the taskbar for the specified window.
		///
		/// Linux: This only works on KDE & Unity environments.
		///        A .desktop file must exist for the application with the same name as given to TRAP::Application.
		/// @param state State of progress.
		/// @param progress How much has been completed. Valid range [0.0-1.0].
		void SetProgress(ProgressState state, f64 progress) const;
		/// @brief Resets the window icon to the TRAP logo.
		void SetIcon() const;
		/// @brief Set a custom logo as window icon (image must be non HDR RGBA 32 bits per pixel)
		/// (nullptr resets the icon to the TRAP logo).
		/// @param image New logo to be used by the window.
		void SetIcon(const Image* image) const;
		/// @brief Set the function where events should be reported to from the window.
		/// @param callback Callback function used to report events to.
		void SetEventCallback(const EventCallbackFn& callback) noexcept;
		/// @brief Set whether resizing of the window should be allowed.
		/// @param enabled Set resizable or non resizable.
		void SetResizable(bool enabled) const;
		/// @brief Limit the minimum size of the window.
		/// Setting any of the two parameters to 0 will disable the minimum size restriction.
		/// @param minWidth Min width.
		/// @param minHeight Min height.
		void SetMinimumSize(u32 minWidth, u32 minHeight);
		/// @brief Limit the maximum size of the window.
		/// Setting any of the two parameters to 0 will disable the maximum size restriction.
		/// @param maxWidth Max width.
		/// @param maxHeight Max height.
		void SetMaximumSize(u32 maxWidth, u32 maxHeight);
		/// @brief Set the aspect ratio of the window.
		/// This makes sure that the window size always matches the specified aspect ratio.
		///
		/// Set both parameters to 0 to disable the aspect ratio restriction.
		/// @param numerator Numerator of the desired aspect ratio, or 0.
		/// @param denominator Denominator of the desired aspect ratio, or 0.
		/// @note This only takes effect in windowed mode.
		/// @note This function does nothing if the window is not resizable.
		/// @note If you set size limits and an aspect ratio that conflict, the results are undefined.
		//        The aspect ratio is applied immediately to a windowed mode window and may cause it to be resized.
		void SetAspectRatio(u32 numerator, u32 denominator);
		/// @brief Set the position of the window.
		/// @param x New x position.
		/// @param y New x position.
		void SetPosition(u32 x, u32 y);
		/// @brief Set the opacity of the window (1.0f = fully opaque, 0.0f = fully transparent).
		/// @param opacity Opacity strength.
		void SetOpacity(f32 opacity) const;
		/// @brief Enable or disable drag and drop feature for the window.
		/// @param enabled True to enable, false otherwise.
		void SetDragAndDrop(bool enabled) const;
		/// @brief Enable or disable VSync for the window.
		///
		/// Same as calling TRAP::Graphics::RendererAPI::GetRenderer()->SetVSync().
		/// @param enabled Whether to enable VSync or not.
		void SetVSync(bool enabled) const;

		/// @brief Query whether the window is maximized or not.
		/// @return Window maximization status.
		[[nodiscard]] bool IsMaximized() const;
		/// @brief Query whether the window is minimized or not.
		/// @return Window minimization status.
		[[nodiscard]] bool IsMinimized() const;
		/// @brief Query whether the window is resizable or not.
		/// @return Window resizable status.
		[[nodiscard]] bool IsResizable() const;
		/// @brief Query whether the window is visible or not.
		/// @return Window visibility status.
		[[nodiscard]] bool IsVisible() const;
		/// @brief Query whether the window is focused or not.
		/// @return Window focus status.
		[[nodiscard]] bool IsFocused() const;
		/// @brief Query whether the window is decorated or not.
		/// @return Window decoration status.
		[[nodiscard]] bool IsDecorated() const;
		/// @brief Query whether the mouse is hovered on the window or not.
		/// @return Window mouse hover status.
		[[nodiscard]] bool IsHovered() const;

		/// @brief Maximize the window.
		/// @note This function only affects the window when using DisplayMode::Windowed.
		void Maximize() const;
		/// @brief Minimize the window.
		void Minimize() const;
		/// @brief Request attention/focus for the window.
		void RequestAttention() const;
		/// @brief Focus the window.
		void Focus() const;
		/// @brief Hide the window.
		void Hide() const;
		/// @brief Show the window.
		void Show() const;
		/// @brief Restore/Show the window.
		void Restore() const;

	private:
		/// @brief Initializes a new window and also if not happened already the renderer.
		/// @param props Window properties for the new window.
		void Init(const WindowProps& props);
		/// @brief Shutdown current window and if there are no other open windows also
		/// deinitializes the internal WindowingAPI.
		void Shutdown();
		/// @brief Setup how to handle/process/dispatch incoming events from the internal WindowingAPI.
		void SetupEventCallbacks();

		INTERNAL::WindowingAPI::InternalWindow* m_window; //Handle to the internal window

		/// @brief Used when switching between fullscreen <-> windowed mode.
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

	/// @brief Struct containing all Window properties needed to create a new TRAP::Window.
	/// @remark @headless This struct is not available in headless mode.
	struct WindowProps
	{
		std::string Title;
		u32 Width;
		u32 Height;
		f64 RefreshRate;
		bool VSync;
		Window::DisplayMode DisplayMode;
		TRAP::Monitor Monitor;

		/// @brief More advanced(optional) window properties.
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

			/// @brief Constructor for advanced(optional) window properties.
			/// @param resizable Whether the window should be resizable.
			/// @param maximized Whether the window should be maximized on creation.
			/// @param visible Whether the window should be visible or not.
			/// @param focused Whether the window should be focused or not.
			/// @param focusOnShow Whether the window should be focused when shown or not.
			/// @param decorated Whether the window should be decorated or not.
			/// @param rawMouseInput Whether the window should use raw mouse input or not.
			/// @param cursorMode Cursor mode to be used by the window.
			constexpr explicit AdvancedProps(bool resizable = true,
			                                 bool maximized = false,
			                                 bool visible = true,
			                                 bool focused = true,
			                                 bool focusOnShow = true,
			                                 bool decorated = true,
			                                 bool rawMouseInput = false,
			                                 Window::CursorMode cursorMode = Window::CursorMode::Normal) noexcept;
		} Advanced{};

		/// @brief Constructor for basic window properties.
		/// @param title Title for the new window.
		/// @param width Width for the new window.
		/// @param height Height for the new window.
		/// @param refreshRate Refresh rate for the new window.
		/// @param vsync Whether to enable VSync for the new window or not.
		/// @param displayMode Display mode for the new window.
		/// @param advanced Optional advanced properties to be used for the new window.
		/// @param monitor Monitor to be used by the new window.
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

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Window::DisplayMode>
{
    static constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Window::DisplayMode displayMode,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(displayMode)
        {
        case TRAP::Window::DisplayMode::Windowed:
            enumStr = "Windowed";
            break;
        case TRAP::Window::DisplayMode::Fullscreen:
            enumStr = "Fullscreen";
            break;
        case TRAP::Window::DisplayMode::Borderless:
            enumStr = "Fullscreen (Borderless)";
            break;

        default:
            TRAP_ASSERT(false, "fmt::formatter<TRAP::Window::DisplayMod>: Missing enum value!");
            enumStr = "<MISSING ENUM VALUE>";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_WINDOW_H*/