#ifndef _TRAP_WINDOW_H_
#define _TRAP_WINDOW_H_

#include "Graphics/API/Context.h"

#include "Events/Event.h"
#include "ImageLoader/Image.h"
#include "WindowingAPI.h"
#include "Monitor.h"

namespace TRAP
{
	struct WindowProps;

	/// <summary>
	/// Interface representing a desktop system based Window
	/// </summary>
	class Window final
	{
	public:
		/// <summary>
		/// Specifies the display mode of a Window
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
		/// Specifies the cursor mode of a Window
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
		/// Specifies the cursor type (visual appearance) of a Window
		/// </summary>
		using CursorType = INTERNAL::WindowingAPI::CursorType;
		
		using EventCallbackFn = std::function<void(Events::Event&)>;

		/// <summary>
		/// Window constructor
		/// </summary>
		/// <param name="props">Properties to be applied to the new Window</param>
		/// <returns>New Window object</returns>
		explicit Window(const WindowProps& props);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = default;
		Window& operator=(Window&&) = default;
		~Window();

		/// <summary>
		/// Updates all Windows (polls events from OS etc.)
		/// </summary>
		static void OnUpdate();

		/// <summary>
		/// Select the given Window for all following rendering commands
		/// </summary>
		/// <param name="window">Window that should be used for rendering</param>
		static void Use(const Scope<Window>& window);
		/// <summary>
		/// Selects the "Main" Window for all following rendering commands
		/// </summary>
		static void Use();
		/// <summary>
		/// Get the amount of all active Windows
		/// </summary>
		/// <returns>Total amount of active Windows</returns>
		static uint32_t GetActiveWindows();

		/// <summary>
		/// Get the current title of the Window
		/// </summary>
		/// <returns>Title of the Window</returns>
		const std::string& GetTitle() const;
		/// <summary>
		/// Get the current width of the Window
		/// </summary>
		/// <returns>Width of the Window</returns>
		uint32_t GetWidth() const;
		/// <summary>
		/// Get the current height of the Window
		/// </summary>
		/// <returns>Height of the Window</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Get the current width and height of the Window
		/// </summary>
		/// <returns>Vec2ui containing the width and height of the Window</returns>
		Math::Vec2ui GetSize() const;
		/// <summary>
		/// Get the current refresh rate of the Window
		/// </summary>
		/// <returns>Refresh rate of the Window</returns>
		uint32_t GetRefreshRate() const;
		/// <summary>
		/// Get the current DisplayMode of the Window
		/// </summary>
		/// <returns>DisplayMode of the Window</returns>
		DisplayMode GetDisplayMode() const;
		/// <summary>
		/// Get the current Monitor used by the Window (only in DisplayMode Fullscreen and Borderless)
		/// </summary>
		/// <returns>Object of Monitor class used by the Window</returns>
		Monitor GetMonitor() const;
		/// <summary>
		/// Get the current VSync interval used by the Window
		/// </summary>
		/// <returns>VSync interval of the Window</returns>
		uint32_t GetVSyncInterval() const;
		/// <summary>
		/// Get the current CursorMode of the Window
		/// </summary>
		/// <returns>CursorMode of the Window</returns>
		CursorMode GetCursorMode() const;
		/// <summary>
		/// Get the current raw mouse input (false = off, true = on) usage of the Window
		/// </summary>
		/// <returns>Raw mouse input status of the Window</returns>
		bool GetRawMouseInput() const;
		/// <summary>
		/// Get the ratio between the current DPI and the platforms DPI
		/// </summary>
		/// <returns>Vec2 containing the x and y scale</returns>
		Math::Vec2 GetContentScale() const;
		/// <summary>
		/// Get the current opacity of the Window
		/// </summary>
		/// <returns>Opacity of the Window</returns>
		float GetOpacity() const;

		/// <summary>
		/// Get the internal handle of the Window
		/// </summary>
		/// <returns>Pointer to the TRAP::INTERNAL::WindowingAPI::InternalWindow handle</returns>
		void* GetInternalWindow() const;

		/// <summary>
		/// Set a new title for the Window
		/// </summary>
		/// <param name="title">New Window title</param>
		void SetTitle(const std::string& title);
		/// <summary>
		/// Set a new DisplayMode and or a new size/refresh rate for the Window
		/// </summary>
		/// <param name="mode">New DisplayMode for the Window</param>
		/// <param name="width">New width (ignored when used with DisplayMode Borderless)</param>
		/// <param name="height">New height (ignored when used with DisplayMode Borderless)</param>
		/// <param name="refreshRate">New refresh rate (ignored when used with DisplayMode Borderless, Windowed)</param>
		void SetDisplayMode(const DisplayMode& mode,
			                uint32_t width = 0,
			                uint32_t height = 0,
			                uint32_t refreshRate = 0);
		/// <summary>
		/// Set a new Monitor for the Window
		/// </summary>
		/// <param name="monitor">Monitor object to be used from now on</param>
		void SetMonitor(Monitor& monitor);
		/// <summary>
		/// Set the VSync interval (0 = No VSync)
		/// </summary>
		/// <param name="interval">New VSync interval</param>
		void SetVSyncInterval(uint32_t interval);
		/// <summary>
		/// Set the CursorMode for the Window
		/// </summary>
		/// <param name="mode">New CursorMode</param>
		void SetCursorMode(const CursorMode& mode);
		/// <summary>
		/// Set the CursorType for the Window
		/// </summary>
		/// <param name="cursor">New CursorType</param>
		void SetCursorType(const CursorType& cursor) const;
		/// <summary>
		/// Set the cursor to a custom Image (Image must be non HDR RGB/RGBA with 24/32 bits per pixel)
		/// </summary>
		/// <param name="image">Image to be used as a cursor</param>
		/// <param name="xHotspot">X hotspot of the Image</param>
		/// <param name="yHotspot">Y hotspot of the Image</param>
		void SetCursorIcon(const Scope<Image>& image, int32_t xHotspot = 0, int32_t yHotspot = 0) const;
		/// <summary>
		/// Set if raw mouse input should be used
		/// </summary>
		/// <param name="enabled">New status for raw mouse input</param>
		void SetRawMouseInput(bool enabled);
		/// <summary>
		/// Resets the Window icon to the TRAP logo
		/// </summary>
		void SetIcon() const;
		/// <summary>
		/// Set a custom logo as Window icon (Image must be non HDR RGBA 32 bits per pixel)(nullptr also resets to the TRAP logo)
		/// </summary>
		/// <param name="image">New logo to be used by the Window</param>
		void SetIcon(const Scope<Image>& image) const;
		/// <summary>
		/// Set the function where events should be reported to from the Window
		/// </summary>
		/// <param name="callback">Callback function used to report events to</param>
		void SetEventCallback(const EventCallbackFn& callback);
		/// <summary>
		/// Set whether resizing of the Window should be allowed
		/// </summary>
		/// <param name="enabled">Set resizable or non resizable</param>
		void SetResizable(bool enabled) const;
		/// <summary>
		/// Limit the minimum size of the Window
		/// </summary>
		/// <param name="minWidth">Min width</param>
		/// <param name="minHeight">Min height</param>
		void SetMinimumSize(uint32_t minWidth, uint32_t minHeight) const;
		/// <summary>
		/// Limit the maximum size of the Window
		/// </summary>
		/// <param name="maxWidth">Max width</param>
		/// <param name="maxHeight">Max height</param>
		void SetMaximumSize(uint32_t maxWidth, uint32_t maxHeight) const;
		/// <summary>
		/// Set the opacity of the Window (1.0f = Fully opaque, 0.0f = Fully transparent)
		/// </summary>
		/// <param name="opacity">Opacity strength</param>
		void SetOpacity(float opacity) const;
		/// <summary>
		/// Enable or Disable Drag and Drop feature for the Window
		/// </summary>
		/// <param name="enabled">True to enable, false otherwise</param>
		void SetDragAndDrop(bool enabled) const;

		/// <summary>
		/// Query whether the Window is maximized or not
		/// </summary>
		/// <returns>Window maximization status</returns>
		bool IsMaximized() const;
		/// <summary>
		/// Query whether the Window is minimized or not
		/// </summary>
		/// <returns>Window minimization status</returns>
		bool IsMinimized() const;
		/// <summary>
		/// Query whether the Window is resizable or not
		/// </summary>
		/// <returns>Window resizable status</returns>
		bool IsResizable() const;
		/// <summary>
		/// Query whether the Window is visible or not
		/// </summary>
		/// <returns>Window visibility status</returns>
		bool IsVisible() const;
		/// <summary>
		/// Query whether the Window is focused or not
		/// </summary>
		/// <returns>Window focus status</returns>
		bool IsFocused() const;
		/// <summary>
		/// Query whether the Window is decorated or not
		/// </summary>
		/// <returns>Window decoration status</returns>
		bool IsDecorated() const;

		/// <summary>
		/// Maximize the Window
		/// </summary>
		void Maximize() const;
		/// <summary>
		/// Minimize the Window
		/// </summary>
		void Minimize() const;
		/// <summary>
		/// Request attention/focus for the Window
		/// </summary>
		void RequestAttention() const;
		/// <summary>
		/// Focus the Window
		/// </summary>
		void Focus() const;
		/// <summary>
		/// Hide the Window
		/// </summary>
		void Hide() const;
		/// <summary>
		/// Show the Window
		/// </summary>
		void Show() const;
		/// <summary>
		/// Restore/Show the Window
		/// </summary>
		void Restore() const;

	private:
		void Init(const WindowProps& props);
		void Shutdown();
		
		Scope<INTERNAL::WindowingAPI::InternalWindow> m_window;
		INTERNAL::WindowingAPI::InternalMonitor* m_useMonitor; //Stores a reference to the monitor
		static std::unordered_map<uint32_t, INTERNAL::WindowingAPI::InternalVideoMode> s_baseVideoModes; //Stores the underlying video mode being used by the OS for every monitor
		
		struct WindowedModeParams
		{
			int32_t Width, Height, RefreshRate;
			int32_t XPos, YPos;
		} m_oldWindowedParams{};
		
		struct WindowData
		{
			std::string Title;
			int32_t Width{}, Height{}, RefreshRate{}, VSync{};
			DisplayMode displayMode{};
			uint32_t Monitor{};
			CursorMode cursorMode{};
			bool RawMouseInput{};
			std::unordered_map<uint16_t, uint32_t> KeyRepeatCounts;

			EventCallbackFn EventCallback;

			WindowedModeParams* windowModeParams = nullptr;
		} m_data;
		
		static uint32_t s_windows;
		static bool s_WindowingAPIInitialized;
		static std::vector<Window*> s_fullscreenWindows;
	};

	//Used to create new windows
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		uint32_t RefreshRate;
		Graphics::API::RenderAPI RenderAPI;
		Window::DisplayMode displayMode;
		uint32_t Monitor;

		struct Advanced
		{			
			uint32_t VSync = 0;
			bool Resizable = true;
			bool Maximized = false;
			bool Visible = true;
			bool Focused = true;
			bool FocusOnShow = true;
			bool Decorated = true;
			//bool Stereo = false;
			bool RawMouseInput = false;
			Window::CursorMode CursorMode = Window::CursorMode::Normal;

			explicit Advanced(uint32_t vSync = 0,
			                  bool resizable = true,
			                  bool maximized = false,
			                  bool visible = true,
			                  bool focused = true,
			                  bool focusOnShow = true,
			                  bool decorated = true,
			                  bool rawMouseInput = false,
			                  Window::CursorMode cursorMode = Window::CursorMode::Normal);
			
		} advanced{};

		//Sets up properties for new window(s)
		explicit WindowProps(std::string title = "TRAP Engine",
							 uint32_t width = 1280,
							 uint32_t height = 720,
							 uint32_t refreshRate = 60,
							 Window::DisplayMode displayMode = Window::DisplayMode::Windowed,
							 Advanced advanced = Advanced{},
		                     uint32_t monitor = 0);
	};
}

#endif /*_TRAP_WINDOW_H_*/