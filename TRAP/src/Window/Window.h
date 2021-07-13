#ifndef _TRAP_WINDOW_H_
#define _TRAP_WINDOW_H_

#include "Events/Event.h"
#include "ImageLoader/Image.h"
#include "WindowingAPI.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP
{
	struct WindowProps;
	class Monitor;

	//TODO Also document on which platforms function execution differs

	/// <summary>
	/// Interface representing a desktop system based Window.
	/// </summary>
	class Window final
	{
	public:
		/// <summary>
		/// Specifies the display mode of a TRAP::Window.
		/// </summary>
		enum class DisplayMode
		{
			Windowed,
			Fullscreen,
			Borderless
		};

		/// <summary>
		/// Same as:
		/// enum class CursorMode
		/// {
		///	    Normal,
		///     Hidden,
		///     Disabled,
		///     Captured
		/// };
		/// Specifies the cursor mode of a TRAP::Window.
		/// </summary>
		using CursorMode = INTERNAL::WindowingAPI::CursorMode;

		/// <summary>
		/// Same as:
		/// enum class CursorType
		/// {
		///     Arrow = 0,
		///		Input = 1,
		///		Crosshair = 2,
		///		PointingHand = 3,
		///		ResizeHorizontal = 4,
		///		ResizeVertical = 5,
		///		ResizeDiagonalTopLeftBottomRight = 6,
		///		ResizeDiagonalTopRightBottomLeft = 7,
		///		ResizeAll = 8,
		///		NotAllowed = 9
		/// };
		/// Specifies the cursor type (visual appearance) of a TRAP::Window.
		/// </summary>
		using CursorType = INTERNAL::WindowingAPI::CursorType;

		using EventCallbackFn = std::function<void(Events::Event&)>;

		/// <summary>
		/// TRAP::Window Constructor.
		/// </summary>
		/// <param name="props">Properties to be applied to the new TRAP::Window.</param>
		/// <returns>New TRAP::Window object.</returns>
		explicit Window(const WindowProps& props);
		/// <summary>
		/// Deleted Copy Constructor.
		/// </summary>
		Window(const Window&) = delete;
		/// <summary>
		/// Deleted Copy Assignment Operator.
		/// </summary>
		Window& operator=(const Window&) = delete;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Window(Window&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Window& operator=(Window&&) = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~Window();

		/// <summary>
		/// Updates all TRAP::Windows (polls events from OS etc.).
		/// </summary>
		static void OnUpdate();

		/// <summary>
		/// Get the amount of all active TRAP::Windows.
		/// </summary>
		/// <returns>Total amount of active TRAP::Windows.</returns>
		static uint32_t GetActiveWindows();

		/// <summary>
		/// Get the current title of the TRAP::Window.
		/// </summary>
		/// <returns>Title of the TRAP::Window.</returns>
		const std::string& GetTitle() const;
		/// <summary>
		/// Get the current width of the TRAP::Window.
		/// </summary>
		/// <returns>Width of the TRAP::Window.</returns>
		uint32_t GetWidth() const;
		/// <summary>
		/// Get the current height of the TRAP::Window.
		/// </summary>
		/// <returns>Height of the TRAP::Window.</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Get the current width and height of the TRAP::Window.
		/// </summary>
		/// <returns>TRAP::Math::Vec2ui containing the width and height of the TRAP::Window.</returns>
		Math::Vec2ui GetSize() const;
		/// <summary>
		/// Get the current refresh rate of the TRAP::Window.
		/// </summary>
		/// <returns>Refresh rate of the TRAP::Window.</returns>
		uint32_t GetRefreshRate() const;
		/// <summary>
		/// Get the current TRAP::Window::DisplayMode of the TRAP::Window.
		/// </summary>
		/// <returns>TRAP::Window::DisplayMode of the TRAP::Window.</returns>
		DisplayMode GetDisplayMode() const;
		/// <summary>
		/// Get the current TRAP::Monitor used by the TRAP::Window
		/// (only in TRAP::Window::DisplayMode Fullscreen and Borderless).
		/// </summary>
		/// <returns>Object of TRAP::Monitor class used by the TRAP::Window.</returns>
		Monitor GetMonitor() const;
		/// <summary>
		/// Get the current TRAP::Window::CursorMode of the TRAP::Window.
		/// </summary>
		/// <returns>TRAP::Window::CursorMode of the TRAP::Window.</returns>
		CursorMode GetCursorMode() const;
		/// <summary>
		/// Get the current raw mouse input (false = off, true = on) usage of the TRAP::Window.
		/// </summary>
		/// <returns>Raw mouse input status of the TRAP::Window.</returns>
		bool GetRawMouseInput() const;
		/// <summary>
		/// Get the ratio between the current DPI and the platforms DPI.
		/// </summary>
		/// <returns>TRAP::Math::Vec2 containing the x and y scale.</returns>
		Math::Vec2 GetContentScale() const;
		/// <summary>
		/// Get the current opacity of the TRAP::Window.
		/// </summary>
		/// <returns>Opacity of the TRAP::Window.</returns>
		float GetOpacity() const;
		/// <summary>
		/// Get whether VSync is enabled or disabled.
		/// </summary>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		bool GetVSync() const;

		/// <summary>
		/// Get the internal handle of the TRAP::Window.
		/// </summary>
		/// <returns>Pointer to the TRAP::INTERNAL::WindowingAPI::InternalWindow handle.</returns>
		void* GetInternalWindow() const;

		/// <summary>
		/// Set a new title for the TRAP::Window.
		/// </summary>
		/// <param name="title">New TRAP::Window title.</param>
		void SetTitle(const std::string& title);
		/// <summary>
		/// Set a new TRAP::Window::DisplayMode and or a new size/refresh rate for the TRAP::Window.
		/// </summary>
		/// <param name="mode">New TRAP::Window::DisplayMode for the TRAP::Window.</param>
		/// <param name="width">New width (ignored when used with TRAP::Window::DisplayMode Borderless).</param>
		/// <param name="height">New height (ignored when used with TRAP::Window::DisplayMode Borderless).</param>
		/// <param name="refreshRate">
		/// New refresh rate (ignored when used with TRAP::Window::DisplayMode Borderless, Windowed).
		/// </param>
		void SetDisplayMode(const DisplayMode& mode,
			                uint32_t width = 0,
			                uint32_t height = 0,
			                uint32_t refreshRate = 0);
		/// <summary>
		/// Set a new Monitor for the TRAP::Window.
		/// </summary>
		/// <param name="monitor">TRAP::Monitor object to be used from now on.</param>
		void SetMonitor(Monitor& monitor);
		/// <summary>
		/// Set the TRAP::Window::CursorMode for the TRAP:Window.
		/// </summary>
		/// <param name="mode">New TRAP::Window::CursorMode.</param>
		void SetCursorMode(const CursorMode& mode);
		/// <summary>
		/// Set the TRAP::Window::CursorType for the TRAP::Window.
		/// </summary>
		/// <param name="cursor">New CursorType.</param>
		void SetCursorType(const CursorType& cursor) const;
		/// <summary>
		/// Set the cursor to a custom TRAP::Image (Image must be non HDR RGB/RGBA with 24/32 bits per pixel).
		/// </summary>
		/// <param name="image">TRAP::Image to be used as a cursor.</param>
		/// <param name="xHotspot">X hotspot of the TRAP::Image.</param>
		/// <param name="yHotspot">Y hotspot of the TRAP::Image.</param>
		void SetCursorIcon(const Scope<Image>& image, int32_t xHotspot = 0, int32_t yHotspot = 0) const;
		/// <summary>
		/// Set if raw mouse input should be used.
		/// </summary>
		/// <param name="enabled">New status for raw mouse input.</param>
		void SetRawMouseInput(bool enabled);
		/// <summary>
		/// Resets the TRAP::Window icon to the TRAP logo.
		/// </summary>
		void SetIcon() const;
		/// <summary>
		/// Set a custom logo as TRAP::Window icon (Image must be non HDR RGBA 32 bits per pixel)
		/// (nullptr resets the icon to the TRAP logo).
		/// </summary>
		/// <param name="image">New logo to be used by the TRAP::Window.</param>
		void SetIcon(const Scope<Image>& image) const;
		/// <summary>
		/// Set the function where events should be reported to from the TRAP::Window.
		/// </summary>
		/// <param name="callback">Callback function used to report events to.</param>
		void SetEventCallback(const EventCallbackFn& callback);
		/// <summary>
		/// Set whether resizing of the TRAP::Window should be allowed.
		/// </summary>
		/// <param name="enabled">Set resizable or non resizable.</param>
		void SetResizable(bool enabled) const;
		/// <summary>
		/// Limit the minimum size of the TRAP::Window.
		/// </summary>
		/// <param name="minWidth">Min width.</param>
		/// <param name="minHeight">Min height.</param>
		void SetMinimumSize(uint32_t minWidth, uint32_t minHeight);
		/// <summary>
		/// Limit the maximum size of the TRAP::Window.<br>
		/// Setting any of the two parameters to 0 will disable the maximum size restriction.
		/// </summary>
		/// <param name="maxWidth">Max width.</param>
		/// <param name="maxHeight">Max height.</param>
		void SetMaximumSize(uint32_t maxWidth, uint32_t maxHeight);
		/// <summary>
		/// Set the opacity of the TRAP::Window (1.0f = Fully opaque, 0.0f = Fully transparent).
		/// </summary>
		/// <param name="opacity">Opacity strength.</param>
		void SetOpacity(float opacity) const;
		/// <summary>
		/// Enable or Disable Drag and Drop feature for the TRAP::Window.
		/// </summary>
		/// <param name="enabled">True to enable, false otherwise.</param>
		void SetDragAndDrop(bool enabled) const;
		/// <summary>
		/// Enable or Disable VSync for the TRAP::Window.
		/// </summary>
		/// <param name="enabled">Whether to enable VSync or not.</param>
		void SetVSync(bool enabled);

		/// <summary>
		/// Query whether the TRAP::Window is maximized or not.
		/// </summary>
		/// <returns>TRAP::Window maximization status.</returns>
		bool IsMaximized() const;
		/// <summary>
		/// Query whether the TRAP::Window is minimized or not.
		/// </summary>
		/// <returns>TRAP::Window minimization status.</returns>
		bool IsMinimized() const;
		/// <summary>
		/// Query whether the TRAP::Window is resizable or not.
		/// </summary>
		/// <returns>TRAP::Window resizable status.</returns>
		bool IsResizable() const;
		/// <summary>
		/// Query whether the TRAP::Window is visible or not.
		/// </summary>
		/// <returns>TRAP::Window visibility status.</returns>
		bool IsVisible() const;
		/// <summary>
		/// Query whether the TRAP::Window is focused or not.
		/// </summary>
		/// <returns>TRAP::Window focus status.</returns>
		bool IsFocused() const;
		/// <summary>
		/// Query whether the TRAP::Window is decorated or not.
		/// </summary>
		/// <returns>TRAP::Window decoration status.</returns>
		bool IsDecorated() const;

		/// <summary>
		/// Maximize the TRAP::Window.
		/// </summary>
		void Maximize() const;
		/// <summary>
		/// Minimize the TRAP::Window.
		/// </summary>
		void Minimize() const;
		/// <summary>
		/// Request attention/focus for the TRAP::Window.
		/// </summary>
		void RequestAttention() const;
		/// <summary>
		/// Focus the TRAP::Window.
		/// </summary>
		void Focus() const;
		/// <summary>
		/// Hide the TRAP::Window.
		/// </summary>
		void Hide() const;
		/// <summary>
		/// Show the TRAP::Window.
		/// </summary>
		void Show() const;
		/// <summary>
		/// Restore/Show the TRAP::Window.
		/// </summary>
		void Restore() const;

	private:
		/// <summary>
		/// Initializes a new TRAP::Window and also if not happened already the Renderer.
		/// </summary>
		/// <param name="props">TRAP::WindowProps for the new TRAP::Window.</param>
		void Init(const WindowProps& props);
		/// <summary>
		/// Shutdown current TRAP::Window and if there are no other open TRAP::Windows also
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
		static std::unordered_map<uint32_t, INTERNAL::WindowingAPI::InternalVideoMode> s_baseVideoModes;

		/// <summary>
		/// Used when switched back from Fullscreen to Windowed mode.
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
			int32_t MinWidth = 800, MinHeight = 600;
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

		inline static constexpr int32_t MinimumSupportedWindowWidth = 2; //800 for games, but for tools?!
		inline static constexpr int32_t MinimumSupportedWindowHeight = 2; //600 for games, but for tools?!
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
			/// <param name="resizable">Whether the TRAP::Window should be resizable.</param>
			/// <param name="maximized">Whether the TRAP::Window should be maximized on creation.</param>
			/// <param name="visible">Whether the TRAP::Window should be visible or not.</param>
			/// <param name="focused">Whether the TRAP::Window should be focused or not.</param>
			/// <param name="focusOnShow">Whether the TRAP::Window should be focused when shown or not.</param>
			/// <param name="decorated">Whether the TRAP::Window should be decorated or not.</param>
			/// <param name="rawMouseInput">Whether the TRAP::Window should use raw mouse input or not.</param>
			/// <param name="cursorMode">TRAP::Window::CursorMode to be used by the TRAP::Window.</param>
			explicit AdvancedProps(bool resizable = true,
			                       bool maximized = false,
			                       bool visible = true,
			                       bool focused = true,
			                       bool focusOnShow = true,
			                       bool decorated = true,
			                       bool rawMouseInput = false,
			                       Window::CursorMode cursorMode = Window::CursorMode::Normal);
		} Advanced{};

		/// <summary>
		/// Constructor for basic window properties.
		/// </summary>
		/// <param name="title">Title for the new TRAP::Window.</param>
		/// <param name="width">Width for the new TRAP::Window.</param>
		/// <param name="height">Height for the new TRAP::Window.</param>
		/// <param name="refreshRate">Refresh rate for the new TRAP::Window.</param>
		/// <param name="displayMode">TRAP::Window::DisplayMode for the new TRAP::Window.</param>
		/// <param name="advanced">Optional advanced properties to be used for the new TRAP::Window.</param>
		/// <param name="monitor">Monitor to be used by the new TRAP::Window.</param>
		explicit WindowProps(std::string title = "TRAP Engine",
							 uint32_t width = 1280,
							 uint32_t height = 720,
							 uint32_t refreshRate = 60,
							 bool vsync = false,
							 Window::DisplayMode displayMode = Window::DisplayMode::Windowed,
							 AdvancedProps advanced = AdvancedProps{},
		                     uint32_t monitor = 0);
	};
}

#endif /*_TRAP_WINDOW_H_*/