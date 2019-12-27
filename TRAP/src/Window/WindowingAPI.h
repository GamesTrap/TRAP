#ifndef _TRAP_WINDOWINGAPI_H_
#define _TRAP_WINDOWINGAPI_H_

#include "Utils/Singleton.h"
#include "Input/Input.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{	
	class WindowingAPI final : public Singleton
	{
	public:	
		//-------------------------------------------------------------------------------------------------------------------//
		//Forward Declarations-----------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
		struct InternalWindow;
		struct InternalMonitor;
		struct InternalCursor;
		enum class Error;
	private:
		struct VkWin32SurfaceCreateInfoKHR;
		//---------//
		//Windows//
		//---------//
#ifdef TRAP_PLATFORM_WINDOWS
		enum class Monitor_DPI_Type;
		enum class Process_DPI_Awareness;
#endif
		//-------------------------------------------------------------------------------------------------------------------//
		//Typedefs-----------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Callbacks//
		//---------//
	public:
		//The function pointer type for error callbacks.
		typedef void (*ErrorFunc)(Error error, const std::string& description);
		//The function pointer type for window position callbacks.
		typedef void (*WindowPositionFunc)(Ref<InternalWindow> window, int32_t xPos, int32_t yPos);
		//The function pointer type for window size callbacks.
		typedef void (*WindowSizeFunc)(Ref<InternalWindow>, int32_t width, int32_t height);
		//The function pointer type for window close callbacks.
		typedef void (*WindowCloseFunc)(Ref<InternalWindow>);
		//The function pointer type for window focus callbacks.
		typedef void (*WindowFocusFunc)(Ref<InternalWindow>, bool focused);
		//The function pointer type for framebuffer size callbacks.
		typedef void (*FrameBufferSizeFunc)(Ref<InternalWindow>, int32_t width, int32_t height);
		//The function pointer type for window content scale callbacks.
		typedef void (*WindowContentScaleFunc)(Ref<InternalWindow>, float xScale, float yScale);
		//The function pointer type for mouse button callbacks.
		typedef void (*MouseButtonFunc)(Ref<InternalWindow>, Input::MouseButton mouseButton, bool pressed);
		//The function pointer type for cursor position callbacks.
		typedef void (*CursorPositionFunc)(Ref<InternalWindow>, double xPos, double yPos);
		//The function pointer type for scroll callbacks.
		typedef void (*ScrollFunc)(Ref<InternalWindow>, double xOffset, double yOffset);
		//The function pointer type for keyboard key callbacks.
		typedef void (*KeyFunc)(Ref<InternalWindow>, Input::Key key, int32_t scanCode, bool pressed); //TODO parameter scanCode needed??
		//The function pointer type for Unicode character callbacks.
		typedef void (*CharFunc)(Ref<InternalWindow>, uint32_t codePoint);
		//The function pointer type for path drop callbacks.
		typedef void (*DropFunc)(Ref<InternalWindow>&, std::vector<std::string> paths);
		//The function pointer type for monitor configuration callbacks.
		typedef void (*MonitorFunc)(Ref<InternalMonitor>&, bool connected);
	private:
		//--------------//
		//OpenGL Context//
		//--------------//
		//Generic function pointer used for returning client API function pointers
		//without forcing a cast from a regular pointer.
		typedef void (*GLProcess)();
		typedef void (*MakeContextCurrentFunc)(Ref<InternalWindow> window);
		typedef void (*SwapBuffersFunc)(const Ref<InternalWindow>& window);
		typedef void (*SwapIntervalFunc)(int32_t interval);
		typedef bool (*ExtensionSupportedFunc)(const char* extension);
		typedef GLProcess(*GetProcAddressFunc)(const char* process);
		typedef void (*DestroyContextFunc)(const Ref<InternalWindow>& window);
		//--------------//
		//Vulkan Surface//
		//--------------//
		typedef VkFlags VkWin32SurfaceCreateFlagsKHR;
		typedef VkResult(*PFN_vkCreateWin32SurfaceKHR)(VkInstance, const VkWin32SurfaceCreateInfoKHR*, const VkAllocationCallbacks*, VkSurfaceKHR*);
		//-------//
		//Windows//
		//-------//
#ifdef TRAP_PLATFORM_WINDOWS
		//user32.dll function pointer typedefs
		typedef BOOL(WINAPI* PFN_SetProcessDPIAware)();
		typedef BOOL(WINAPI* PFN_ChangeWindowMessageFilterEx)(HWND, UINT, DWORD, CHANGEFILTERSTRUCT*);
		typedef BOOL(WINAPI* PFN_EnableNonClientDPIScaling)(HWND);
		typedef BOOL(WINAPI* PFN_SetProcessDPIAwarenessContext)(HANDLE);
		typedef UINT(WINAPI* PFN_GetDPIForWindow)(HWND);
		typedef BOOL(WINAPI* PFN_AdjustWindowRectExForDPI)(LPRECT, DWORD, BOOL, DWORD, UINT);

		//dwmapi.dll function pointer typedefs
		typedef HRESULT(WINAPI* PFN_DwmIsCompositionEnabled)(BOOL*);
		typedef HRESULT(WINAPI* PFN_DwmFlush)();
		typedef HRESULT(WINAPI* PFN_DwmEnableBlurBehindWindow)(HWND, const DWM_BLURBEHIND*);

		//shcore.dll function pointer typedefs
		typedef HRESULT(WINAPI* PFN_SetProcessDPIAwareness)(Process_DPI_Awareness);
		typedef HRESULT(WINAPI* PFN_GetDPIForMonitor)(HMONITOR, Monitor_DPI_Type, UINT*, UINT*);

		//ntdll.dll function pointer typedefs
		typedef LONG(WINAPI* PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);

		//WGL extension pointer typedefs
		typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int32_t interval);
		typedef BOOL(WINAPI* PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC, int32_t, int32_t, UINT, const int32_t*, int32_t*);
		typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC)();
		typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
		typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int32_t*);

		//opengl32.dll function pointer typedefs
		typedef HGLRC(WINAPI* PFN_WGLCreateContext)(HDC);
		typedef BOOL(WINAPI* PFN_WGLDeleteContext)(HGLRC);
		typedef PROC(WINAPI* PFN_WGLGetProcAddress)(LPCSTR);
		typedef HDC(WINAPI* PFN_WGLGetCurrentDC)();
		typedef HGLRC(WINAPI* PFN_WGLGetCurrentContext)();
		typedef BOOL(WINAPI* PFN_WGLMakeCurrent)(HDC, HGLRC);
		typedef BOOL(WINAPI* PFN_WGLShareLists)(HGLRC, HGLRC);
#endif
		//-------------------------------------------------------------------------------------------------------------------//
		//Enums--------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
	public:		
		enum class Error
		{
			No_Error,
			No_Current_Context,
			Invalid_Enum,
			Invalid_Value,
			Out_Of_Memory,
			API_Unavailable,
			Version_Unavailable,
			Platform_Error,
			Format_Unavailable,
			No_Window_Context,
			Cursor_Unavailable
		};
		enum class Hint
		{
			Resizable,
			Maximized,
			Visible,
			Focused
			//Stereo //Used for 3D/VR
		};
		enum class ContextAPI
		{
			None,
			OpenGL
		};
		enum class CursorMode
		{
			Normal,
			Hidden,
			Disabled
		};
		enum class CursorType
		{
			Arrow,
			Input,
			Crosshair,
			PointingHand,
			ResizeHorizontal,
			ResizeVertical,
			ResizeDiagonalTopLeftBottomRight,
			ResizeDiagonalTopRightBottomLeft,
			ResizeAll,
			NotAllowed
		};
	private:
		//-------//
		//Windows//
		//-------//
#ifdef TRAP_PLATFORM_WINDOWS
	#ifndef DPI_ENUMS_DECLARED
		enum class Process_DPI_Awareness
		{
			Process_DPI_Unaware = 0,
			Process_System_DPI_Aware = 1,
			Process_Per_Monitor_DPI_Aware = 2
		};
		enum class Monitor_DPI_Type
		{
			MDT_Effective_DPI = 0,
			MDT_Angular_DPI = 1,
			MDT_Raw_DPI = 2,
			MDT_DEFAULT = MDT_Effective_DPI
		};
	#endif /*DPI_ENUMS_DECLARED*/
#endif /*TRAP_PLATFORM_WINDOWS*/
		//-------------------------------------------------------------------------------------------------------------------//
		//Constant Expressions-----------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//-------//
		//Windows//
		//-------//
#ifdef TRAP_PLATFORM_WINDOWS
	#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
		#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((HANDLE) -4)
	#endif /*DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2*/
		//HACK: Define macros that some windows.h variants don't		
	#ifndef WM_COPYGLOBALDATA
		inline static constexpr uint32_t WM_COPYGLOBALDATA = 0x0049;
	#endif /*WM_COPYGLOBALDATA*/
		//WGL constants
		inline static constexpr uint32_t WGL_NUMBER_PIXEL_FORMATS_ARB = 0x2000;
		inline static constexpr uint32_t WGL_SUPPORT_OPENGL_ARB = 0x2010;
		inline static constexpr uint32_t WGL_DRAW_TO_WINDOW_ARB = 0x2001;
		inline static constexpr uint32_t WGL_PIXEL_TYPE_ARB = 0x2013;
		inline static constexpr uint32_t WGL_TYPE_RGBA_ARB = 0x202b;
		inline static constexpr uint32_t WGL_ACCELERATION_ARB = 0x2003;
		inline static constexpr uint32_t WGL_NO_ACCELERATION_ARB = 0x2025;
		inline static constexpr uint32_t WGL_RED_BITS_ARB = 0x2015;
		inline static constexpr uint32_t WGL_RED_SHIFT_ARB = 0x2016;
		inline static constexpr uint32_t WGL_GREEN_BITS_ARB = 0x2017;
		inline static constexpr uint32_t WGL_GREEN_SHIFT_ARB = 0x2018;
		inline static constexpr uint32_t WGL_BLUE_BITS_ARB = 0x2019;
		inline static constexpr uint32_t WGL_BLUE_SHIFT_ARB = 0x201a;
		inline static constexpr uint32_t WGL_ALPHA_BITS_ARB = 0x201b;
		inline static constexpr uint32_t WGL_ALPHA_SHIFT_ARB = 0x201c;
		inline static constexpr uint32_t WGL_ACCUM_BITS_ARB = 0x201d;
		inline static constexpr uint32_t WGL_ACCUM_RED_BITS_ARB = 0x201e;
		inline static constexpr uint32_t WGL_ACCUM_GREEN_BITS_ARB = 0x201f;
		inline static constexpr uint32_t WGL_ACCUM_BLUE_BITS_ARB = 0x2020;
		inline static constexpr uint32_t WGL_ACCUM_ALPHA_BITS_ARB = 0x2021;
		inline static constexpr uint32_t WGL_DEPTH_BITS_ARB = 0x2022;
		inline static constexpr uint32_t WGL_STENCIL_BITS_ARB = 0x2023;
		inline static constexpr uint32_t WGL_AUX_BUFFERS_ARB = 0x2024;
		inline static constexpr uint32_t WGL_STEREO_ARB = 0x2012;
		inline static constexpr uint32_t WGL_DOUBLE_BUFFER_ARB = 0x2011;
		inline static constexpr uint32_t WGL_SAMPLES_ARB = 0x2042;
		inline static constexpr uint32_t WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB = 0x20a9;
		inline static constexpr uint32_t WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB = 0x00000002;
		inline static constexpr uint32_t WGL_CONTEXT_PROFILE_MASK_ARB = 0x9126;
		inline static constexpr uint32_t WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001;
		inline static constexpr uint32_t WGL_CONTEXT_MAJOR_VERSION_ARB = 0x2091;
		inline static constexpr uint32_t WGL_CONTEXT_MINOR_VERSION_ARB = 0x2092;
		inline static constexpr uint32_t WGL_CONTEXT_FLAGS_ARB = 0x2094;
		
		inline static constexpr uint32_t ERROR_INVALID_VERSION_ARB = 0x2095;
		inline static constexpr uint32_t ERROR_INVALID_PROFILE_ARB = 0x2096;
		inline static constexpr uint32_t ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB = 0x2054;
#endif
		//-------------------------------------------------------------------------------------------------------------------//
		//Structs------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		struct VkWin32SurfaceCreateInfoKHR
		{
			VkStructureType sType;
			const void* pNext;
			VkWin32SurfaceCreateFlagsKHR flags;
			HINSTANCE hinstance;
			HWND hwnd;
		};
		//Thread local storage structure
		struct TLS
		{
#ifdef TRAP_PLATFORM_WINDOWS
			bool Allocated = false;
			DWORD Index = 0;
#endif
		};
		//Mutex structure
		struct Mutex
		{
#ifdef TRAP_PLATFORM_WINDOWS
			bool Allocated = false;
			CRITICAL_SECTION Section{};
#endif
		};
		//Per-thread error structure
		struct WindowingError
		{
			Ref<WindowingError> Next = nullptr;
			Error ErrorCode = Error::No_Error;
			std::string Description = "";
		};
		//Framebuffer configuration
		struct FrameBufferConfig
		{
			int32_t RedBits = 0;
			int32_t GreenBits = 0;
			int32_t BlueBits = 0;
			int32_t AlphaBits = 0;
			int32_t DepthBits = 0;
			int32_t StencilBits = 0;
			int32_t AccumRedBits = 0;
			int32_t AccumGreenBits = 0;
			int32_t AccumBlueBits = 0;
			int32_t AccumAlphaBits = 0;
			int32_t AuxBuffers = 0;
			bool Stereo = false;
			int32_t Samples = 0;
			bool SRGB = false;
			bool DoubleBuffer = false;
			bool Transparent = false;
			uintptr_t Handle = 0;
		};
		//Window configuration
		struct WindowConfig
		{
			uint32_t Width = 0;
			uint32_t Height = 0;
			std::string Title{};
			bool Resizable = false;
			bool Decorated = true;
			bool Visible = false;
			bool Maximized = false;
			bool Focused = false;
		};
		//Context configuration
		struct ContextConfig
		{
			ContextAPI Client = ContextAPI::None;
			Ref<InternalWindow> Share = nullptr;
		};		
		//Context structure
		struct Context
		{
			ContextAPI Client = ContextAPI::None;
			uint32_t Major = 0, Minor = 0;

			PFNGLGETSTRINGIPROC GetStringi = nullptr;
			PFNGLGETINTEGERVPROC GetIntegerv = nullptr;
			PFNGLGETSTRINGPROC  GetString = nullptr;

			MakeContextCurrentFunc MakeCurrent = nullptr;
			SwapBuffersFunc SwapBuffers = nullptr;
			SwapIntervalFunc SwapInterval = nullptr;
			ExtensionSupportedFunc ExtensionSupported = nullptr;
			GetProcAddressFunc GetProcAddress = nullptr;
			DestroyContextFunc Destroy = nullptr;

#ifdef TRAP_PLATFORM_WINDOWS
			HDC DC = nullptr;
			HGLRC Handle = nullptr;
			int32_t Interval = 0;
#endif
		};
		//Global state
		struct Data
		{
			bool Initialized = false;

			struct
			{
				FrameBufferConfig FrameBuffer{};
				WindowConfig Window{};
				ContextConfig Context{};
			} Hints{};

			Ref<WindowingError> ErrorListHead = nullptr;
			Ref<InternalWindow> WindowListHead = nullptr;
			Ref<InternalCursor> CursorListHead = nullptr;

			std::vector<Ref<InternalMonitor>> Monitors{};

			TLS ErrorSlot{};
			TLS ContextSlot{};
			Mutex ErrorLock{};

			struct
			{
				MonitorFunc Monitor = nullptr;
			} Callbacks{};

			struct
			{
				bool Available = false;
				void* Handle = nullptr;
				std::array<std::string, 2> Extensions{};
				bool KHR_Surface = false;
#if defined(TRAP_PLATFORM_WINDOWS)
				bool KHR_Win32_Surface = false;
#elif defined(TRAP_PLATFORM_LINUX_X11)
				bool KHR_XLib_Surface = false;
				bool KHR_XCB_Surface = false;
#elif defined(TRAP_PLATFORM_LINUX_WAYLAND)
				bool KHR_Wayland_Surface = false;
#endif
			} VK{};

#ifdef TRAP_PLATFORM_WINDOWS
			HWND HelperWindowHandle = nullptr;
			HDEVNOTIFY DeviceNotificationHandle = nullptr;
			DWORD ForegroundLockTimeout = 0;
			int32_t AcquiredMonitorCount = 0;
			std::string ClipboardString{};
			std::array<Input::Key, 512> KeyCodes{};
			std::array<int16_t, static_cast<uint32_t>(Input::Key::Menu) + 1> ScanCodes{};
			std::array<std::array<char, 5>, static_cast<uint32_t>(Input::Key::Menu) + 1> KeyNames{};
			//Where to place the cursor when re-enabled
			double RestoreCursorPosX = 0.0, RestoreCursorPosY = 0.0;
			//The window whose disabled cursor mode is active
			Ref<InternalWindow> DisabledCursorWindow = nullptr;
			std::vector<RAWINPUT> RawInput{};
			int32_t RawInputSize = 0;
			UINT MouseTrailSize = 0;

			struct
			{
				HINSTANCE Instance = nullptr;
				PFN_SetProcessDPIAware SetProcessDPIAware = nullptr;
				PFN_ChangeWindowMessageFilterEx ChangeWindowMessageFilterEx = nullptr;
				PFN_EnableNonClientDPIScaling EnableNonClientDPIScaling = nullptr;
				PFN_SetProcessDPIAwarenessContext SetProcessDPIAwarenessContext = nullptr;
				PFN_GetDPIForWindow GetDPIForWindow = nullptr;
				PFN_AdjustWindowRectExForDPI AdjustWindowRectExForDPI = nullptr;
			} User32;

			struct
			{
				HINSTANCE Instance = nullptr;
				PFN_DwmIsCompositionEnabled IsCompositionEnabled = nullptr;
				PFN_DwmFlush Flush = nullptr;
				PFN_DwmEnableBlurBehindWindow EnableBlurBehindWindow = nullptr;
			} DWMAPI_;

			struct
			{
				HINSTANCE Instance = nullptr;
				PFN_SetProcessDPIAwareness SetProcessDPIAwareness = nullptr;
				PFN_GetDPIForMonitor GetDPIForMonitor = nullptr;
			} SHCore;

			struct
			{
				HINSTANCE Instance = nullptr;
				PFN_RtlVerifyVersionInfo RtlVerifyVersionInfo = nullptr;
			} NTDLL;

			//WGL-specific global data
			struct
			{
				HINSTANCE Instance = nullptr;
				PFN_WGLCreateContext CreateContext = nullptr;
				PFN_WGLDeleteContext DeleteContext = nullptr;
				PFN_WGLGetProcAddress GetProcAddress = nullptr;
				PFN_WGLGetCurrentDC GetCurrentDC = nullptr;
				PFN_WGLGetCurrentContext GetCurrentContext = nullptr;
				PFN_WGLMakeCurrent MakeCurrent = nullptr;
				PFN_WGLShareLists ShareLists = nullptr;

				PFNWGLSWAPINTERVALEXTPROC SwapIntervalEXT = nullptr;
				PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB = nullptr;
				PFNWGLGETEXTENSIONSSTRINGEXTPROC GetExtensionsStringEXT = nullptr;
				PFNWGLGETEXTENSIONSSTRINGARBPROC GetExtensionsStringARB = nullptr;
				PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB = nullptr;
				bool EXT_swap_control = false;
				bool ARB_multisample = false;
				bool ARB_framebuffer_sRGB = false;
				bool EXT_framebuffer_sRGB = false;
				bool ARB_pixel_format = false;
				bool ARB_create_context = false;
				bool ARB_create_context_profile = false;
				bool ARB_context_flush_control = false;
			} WGL;
#endif
		};
	public:
		//This describes a single video mode.
		struct VideoMode
		{
			//The width, in screen coordinates, of the video mode.
			int32_t Width = 0;
			//The height, in screen coordinates, of the video mode.
			int32_t Height = 0;
			//The bit depth of the red channel of the video mode.
			int32_t RedBits = 0;
			//The bit depth of the green channel of the video mode.
			int32_t GreenBits = 0;
			//The bit depth of the blue channel of the video mode.
			int32_t BlueBits = 0;
			//The refresh rate, in Hz, of the video mode.
			int32_t RefreshRate = 0;
		};
		//Monitor structure
		struct InternalMonitor
		{
			std::string Name{};

			//The window whose video mode is current on this monitor
			Ref<InternalWindow> Window = nullptr;

			std::vector<VideoMode> Modes{};
			VideoMode CurrentMode;

#ifdef TRAP_PLATFORM_WINDOWS
			HMONITOR Handle = nullptr;
			std::wstring AdapterName{};
			std::wstring DisplayName{};
			std::string PublicAdapterName{};
			std::string PublicDisplayName{};
			bool ModesPruned = false;
			bool ModeChanged = false;
#endif
		};
		//Cursor structure
		struct InternalCursor
		{
			Ref<InternalCursor> Next = nullptr;

#ifdef TRAP_PLATFORM_WINDOWS
			HCURSOR Handle = nullptr;
#endif
		};
		//Window and Context structure
		struct InternalWindow
		{
			Ref<InternalWindow> Next = nullptr;
			
			//Window settings and state
			bool Resizable = true;
			bool Decorated = true;
			bool ShouldClose = false;
			void* UserPointer = nullptr;
			VideoMode VideoMode{};
			Ref<InternalMonitor> Monitor = nullptr;
			Ref<InternalCursor> Cursor = nullptr;

			CursorMode CursorMode = CursorMode::Normal;
			std::array<bool, 8> MouseButtons{};
			std::array<bool, 349 + 1> Keys{};
			//Virtual Cursor position when Cursor is disabled
			double VirtualCursorPosX = 0.0, VirtualCursorPosY = 0.0;
			bool RawMouseMotion = false;

			Context Context{};

			struct
			{
				WindowPositionFunc Pos = nullptr;
				WindowSizeFunc Size = nullptr;
				WindowCloseFunc Close = nullptr;
				WindowFocusFunc Focus = nullptr;
				FrameBufferSizeFunc FBSize = nullptr;
				WindowContentScaleFunc Scale = nullptr;
				MouseButtonFunc MouseButton = nullptr;
				CursorPositionFunc CursorPos = nullptr;
				ScrollFunc Scroll = nullptr;
				KeyFunc Key = nullptr;
				CharFunc Character = nullptr;
				DropFunc Drop = nullptr;
			} Callbacks;

#ifdef TRAP_PLATFORM_WINDOWS
			HWND Handle = nullptr;
			HICON BigIcon = nullptr;
			HICON SmallIcon = nullptr;

			bool CursorTracked = false;
			bool FrameAction = false;
			bool Minimized = false;
			bool Maximized = false;

			//The last received cursor position, regardless of source
			int32_t LastCursorPosX = 0, LastCursorPosY = 0;
#endif
		};		
	private:
		//-------------------------------------------------------------------------------------------------------------------//
		//Static Members-----------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		static WindowingError s_MainThreadError;
		static ErrorFunc s_ErrorCallback;
		static Data s_Data;		
		//-------------------------------------------------------------------------------------------------------------------//
		//Platform Independent Functions-------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//------//
		//Public//
		//------//
	public:
		//Initializes the Windowing API.
		static bool Init();
		//Shuts down the Windowing API.
		static void Shutdown();
		//Destroys the specified window and its context.
		static void DestroyWindow(Ref<InternalWindow> window);
		//Makes the context of the specified window current for the calling
		static void MakeContextCurrent(const Ref<InternalWindow>& window);
		//Resets all window hints to their default values.
		static void DefaultWindowHints();
		//Sets the specified window hint to the desired value.
		static void WindowHint(Hint hint, bool value);
		//Sets the specified sample count to the desired sample value.
		static void SetSamples(uint32_t samples);
		//Sets the specified window context API to the desired value
		static void SetContextAPI(ContextAPI contextAPI);	
		//Returns the name of the specified monitor.
		static std::string GetMonitorName(const Ref<InternalMonitor>& monitor);
		//Returns the primary monitor.
		static Ref<InternalMonitor> GetPrimaryMonitor();
		//Returns the currently connected monitors.
		static const std::vector<Ref<InternalMonitor>>& GetMonitors();
		//Returns the current mode of the specified monitor.
		static const VideoMode& GetVideoMode(const Ref<InternalMonitor>& monitor);
		//Returns the available video modes for the specified monitor.
		static std::vector<VideoMode> GetVideoModes(const Ref<InternalMonitor>& monitor);
		//Creates a window and its associated context.
		static Ref<InternalWindow> CreateWindow(uint32_t width, uint32_t height, const std::string& title, Ref<InternalMonitor> monitor, Ref<InternalWindow> share);
		//Sets the close flag of the specified window.
		static void SetWindowShouldClose(const Ref<InternalWindow>& window, bool value);
		//Sets the title of the specified window.
		static void SetWindowTitle(const Ref<InternalWindow>& window, const std::string& title);
		//Retrieves the content scale for the specified monitor.
		static void GetMonitorContentScale(Ref<InternalMonitor> monitor, float& xScale, float& yScale);
		//Destroys a cursor.
		static void DestroyCursor(Ref<InternalCursor> cursor);
		//Creates a custom cursor.
		static Ref<InternalCursor> CreateCursor(const Scope<Image>& image, int32_t xHotspot, int32_t yHotspot);
		//Creates a cursor with a standard shape.
		static Ref<InternalCursor> CreateStandardCursor(const CursorType& type);
		//Sets the cursor for the window.
		static void SetCursor(const Ref<InternalWindow>& window, const Ref<InternalCursor>& cursor);
		//Sets the icon for the specified window.
		static void SetWindowIcon(const Ref<InternalWindow>& window, const Scope<Image>& image);
		//Sets the position of the content area of the specified window.
		static void SetWindowPos(const Ref<InternalWindow>& window, int32_t xPos, int32_t yPos);
		//Retrieves the position of the content area of the specified window.
		static void GetWindowPos(const Ref<InternalWindow>& window, int32_t& xPos, int32_t& yPos);
		//Sets the size of the content area of the specified window.
		static void SetWindowSize(const Ref<InternalWindow>& window, int32_t width, int32_t height);
		//Retrieves the size of the content area of the specified window.
		static void GetWindowSize(const Ref<InternalWindow>& window, int32_t& width, int32_t& height);
		//Retrieves the size of the framebuffer of the specified window.
		static void GetFrameBufferSize(const Ref<InternalWindow>& window, int32_t& width, int32_t& height);
		//Retrieves the content scale for the specified window.
		static void GetWindowContentScale(const Ref<InternalWindow>& window, float& xScale, float& yScale);
		//Sets the mode, monitor, video mode and placement of a window.
		static void SetWindowMonitor(const Ref<InternalWindow>& window,
		                             Ref<InternalMonitor> monitor,
		                             int32_t xPos,
		                             int32_t yPos,
		                             int32_t width,
		                             int32_t height,
		                             int32_t refreshRate);
		//Sets the user pointer of the specified window.
		static void SetWindowUserPointer(const Ref<InternalWindow>& window, void* pointer);
		//Returns the user pointer of the specified window.
		static void* GetWindowUserPointer(const Ref<InternalWindow>& window);
		//Sets the error callback.
		static void SetErrorCallback(ErrorFunc callback);
		//Sets the monitor configuration callback.
		static void SetMonitorCallback(MonitorFunc callback);
		//Sets the position callback for the specified window.
		static void SetWindowPosCallback(const Ref<InternalWindow>& window, WindowPositionFunc callback);
		//Sets the size callback for the specified window.
		static void SetWindowSizeCallback(const Ref<InternalWindow>& window, WindowSizeFunc callback);
		//Sets the close callback for the specified window.
		static void SetWindowCloseCallback(const Ref<InternalWindow>& window, WindowCloseFunc callback);
		//Sets the focus callback for the specified window.
		static void SetWindowFocusCallback(const Ref<InternalWindow>& window, WindowFocusFunc callback);
		//Sets the framebuffer resize callback for the specified window.
		static void SetFrameBufferSizeCallback(const Ref<InternalWindow>& window, FrameBufferSizeFunc callback);
		//Sets the window content scale callback for the specified window.
		static void SetWindowContentScaleCallback(const Ref<InternalWindow>& window, WindowContentScaleFunc callback);
		//Sets the key callback.
		static void SetKeyCallback(const Ref<InternalWindow>& window, KeyFunc callback);
		//Sets the Unicode character callback.
		static void SetCharCallback(const Ref<InternalWindow>& window, CharFunc callback);
		//Sets the mouse button callback.
		static void SetMouseButtonCallback(const Ref<InternalWindow>& window, MouseButtonFunc callback);
		//Sets the cursor position callback.
		static void SetCursorPosCallback(const Ref<InternalWindow>& window, CursorPositionFunc callback);
		//Sets the scroll callback.
		static void SetScrollCallback(const Ref<InternalWindow>& window, ScrollFunc callback);
		//Sets the path drop callback.
		static void SetDropCallback(const Ref<InternalWindow>& window, DropFunc callback);
		//Gets the error callback.
		static ErrorFunc GetErrorCallback();
		//Gets the monitor configuration callback.
		static MonitorFunc GetMonitorCallback();
		//Gets the position callback for the specified window.
		static WindowPositionFunc GetWindowPosCallback(const Ref<InternalWindow>& window);
		//Gets the size callback for the specified window.
		static WindowSizeFunc GetWindowSizeCallback(const Ref<InternalWindow>& window);
		//Gets the close callback for the specified window.
		static WindowCloseFunc GetWindowCloseCallback(const Ref<InternalWindow>& window);
		//Gets the focus callback for the specified window.
		static WindowFocusFunc GetWindowFocusCallback(const Ref<InternalWindow>& window);
		//Gets the framebuffer resize callback for the specified window.
		static FrameBufferSizeFunc GetFrameBufferSizeCallback(const Ref<InternalWindow>& window);
		//Gets the window content scale callback for the specified window.
		static WindowContentScaleFunc GetWindowContentScaleCallback(const Ref<InternalWindow>& window);
		//Gets the key callback.
		static KeyFunc GetKeyCallback(const Ref<InternalWindow>& window);
		//Gets the Unicode character callback.
		static CharFunc GetCharCallback(const Ref<InternalWindow>& window);
		//Gets the mouse button callback.
		static MouseButtonFunc GetMouseButtonCallback(const Ref<InternalWindow>& window);
		//Gets the cursor position callback.
		static CursorPositionFunc GetCursorPosCallback(const Ref<InternalWindow>& window);
		//Gets the scroll callback.
		static ScrollFunc GetScrollCallback(const Ref<InternalWindow>& window);
		//Gets the path drop callback.
		static DropFunc GetDropCallback(const Ref<InternalWindow>& window);
		//Processes all pending events.
		static void PollEvents();
		//Sets the cursor mode for the specified window.
		static void SetCursorMode(const Ref<InternalWindow>& window, CursorMode mode);
		//Retrieves the cursor mode for the specified window.
		static CursorMode GetCursorMode(const Ref<InternalWindow>& window);
		//Returns whether raw mouse motion is supported.
		static bool RawMouseMotionSupported();
		//Sets the raw mouse motion mode for the specified window.
		static void SetRawMouseMotionMode(const Ref<InternalWindow>& window, bool enabled);
		//Retrvieves the raw mouse motion mode for the specified window.
		static bool GetRawMouseMotionMode(const Ref<InternalWindow>& window);
		//Returns the layout-specific name of the specified printable key.
		static const char* GetKeyName(Input::Key key, int32_t scanCode);
		//Returns the last reported state of a keyboard key for the specified window.
		static bool GetKey(const Ref<InternalWindow>& window, Input::Key key);
		//Returns the last reported state of a mouse button for the specified window.
		static bool GetMouseButton(const Ref<InternalWindow>& window, Input::MouseButton button);
		//Retrieves the position of the cursor relative to the content area of the window.
		static void GetCursorPos(const Ref<InternalWindow>& window, double& xPos, double& yPos);
		//Sets the clipboard to the specified string.
		static void SetClipboardString(const Ref<InternalWindow>& window, const std::string& string);
		//Returns the contents of the clipboard as a string.
		static std::string GetClipboardString(const Ref<InternalWindow>& window);
		//Returns the window whose context is current on the calling thread.
		static Ref<InternalWindow> GetCurrentContext();
		//Swaps the front and back buffers of the specified window.
		static void SwapBuffers(const Ref<InternalWindow>& window);
		//Sets the swap interval for the current context.
		static void SwapInterval(int32_t interval);
		//Returns whether the specified extension is available.
		static bool ExtensionSupported(const std::string& extension);
		//Returns the address of the specified function for the current context.
		static GLProcess GetProcAddress(const char* procName);
		//Returns whether the Vulkan loader and an ICD have been found.
		static bool VulkanSupported();
		//Returns the Vulkan instance extensions required by TRAP.
		static std::array<std::string, 2> GetRequiredInstanceExtensions();
		//Creates a Vulkan surface for the specified window.
		static VkResult CreateWindowSurface(VkInstance instance, const Ref<InternalWindow>& window, const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
#ifdef TRAP_PLATFORM_WINDOWS
		static HWND GetWin32Window(Ref<InternalWindow> window);
#endif
		//-------//
		//Private//
		//-------//
	private:
		//Checks whether the desired context attributes are valid
		static bool IsValidContextConfig(const ContextConfig& CTXConfig);
		//Center the cursor in the content area of the specified window
		static void CenterCursorInContentArea(const Ref<InternalWindow>& window);
		//Notifies shared code of an error
		static void InputError(Error code, const std::string& str);		
		//-------------------------------------------------------------------------------------------------------------------//
		//Platform Specific Functions----------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//Create key code translation tables
		static void CreateKeyTables();
		
		static void PlatformGetVideoMode(Ref<InternalMonitor> monitor, VideoMode& mode); 
		static void PlatformGetWindowSize(Ref<InternalWindow> window, int32_t& width, int32_t& height);
		static void PlatformSetWindowPos(Ref<InternalWindow> window, int32_t xPos, int32_t yPos);
		static void PlatformGetWindowFrameSize(Ref<InternalWindow> window,
			                                   int32_t& left, int32_t& top,
			                                   int32_t& right, int32_t& bottom);
		static void PlatformSetWindowMonitor(const Ref<InternalWindow>& window, Ref<InternalMonitor> monitor,
			                                 int32_t xPos, int32_t yPos, int32_t width, int32_t height, int32_t refreshRate);
		static std::vector<VideoMode> PlatformGetVideoModes(Ref<InternalMonitor> monitor);
		static bool PlatformInit();
		static bool PlatformCreateMutex(Mutex& mutex); 
		static bool PlatformCreateTLS(TLS& tls);
		static void PlatformSetTLS(TLS& tls, void* value);
		static void* PlatformGetTLS(TLS& tls);
		static void PlatformDestroyTLS(TLS& tls);
		static void PlatformDestroyMutex(Mutex& mutex);
		static void PlatformLockMutex(Mutex& mutex);
		static void PlatformUnlockMutex(Mutex& mutex);
		static void PlatformDestroyWindow(Ref<InternalWindow> window);
		static void PlatformShutdown();
		static void PlatformGetMonitorContentScale(Ref<InternalMonitor> monitor, float& xScale, float& yScale);
		static void PlatformGetMonitorPos(Ref<InternalMonitor> monitor, int32_t& xPos, int32_t& yPos);
		static void PlatformShowWindow(Ref<InternalWindow> window);
		static void PlatformFocusWindow(Ref<InternalWindow> window);
		static bool PlatformCreateWindow(Ref<InternalWindow>& window,
			                             const WindowConfig& WNDConfig,
			                             const ContextConfig& CTXConfig,
			                             const FrameBufferConfig& FBConfig);
		static void PlatformSetWindowTitle(Ref<InternalWindow> window, const std::string& title);
		static bool PlatformCreateCursor(Ref<InternalCursor> cursor, const Scope<Image>& image, int32_t xHotspot, int32_t yHotspot);
		static bool PlatformCreateStandardCursor(Ref<InternalCursor> cursor, const CursorType& type);
		static void PlatformDestroyCursor(Ref<InternalCursor> cursor);
		static void PlatformSetCursor(Ref<InternalWindow> window, Ref<InternalCursor> cursor);
		static void PlatformSetCursorMode(Ref<InternalWindow> window, CursorMode mode);
		static void PlatformSetCursorPos(Ref<InternalWindow> window, double xPos, double yPos);
		static void PlatformGetCursorPos(Ref<InternalWindow> window, double& xPos, double& yPos);
		static void PlatformSetWindowIcon(Ref<InternalWindow> window, const Scope<Image>& image);
		static void PlatformGetWindowPos(Ref<InternalWindow> window, int32_t& xPos, int32_t& yPos);
		static void PlatformSetWindowSize(Ref<InternalWindow> window, int32_t width, int32_t height);
		static void PlatformGetFrameBufferSize(Ref<InternalWindow> window, int32_t& width, int32_t& height);
		static void PlatformGetWindowContentScale(Ref<InternalWindow> window, float& xScale, float& yScale);
		static void PlatformPollEvents();
		static bool PlatformWindowFocused(Ref<InternalWindow> window);
		static bool PlatformRawMouseMotionSupported();
		static void PlatformSetRawMouseMotion(Ref<InternalWindow> window, bool enabled);
		static int32_t PlatformGetKeyScanCode(Input::Key key);
		static const char* PlatformGetScanCodeName(int32_t scanCode);
		static void PlatformSetClipboardString(const std::string& string);
		static std::string PlatformGetClipboardString();
		static void PlatformGetRequiredInstanceExtensions(std::array<std::string, 2>& extensions);
		static VkResult PlatformCreateWindowSurface(VkInstance instance, Ref<InternalWindow> window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		static void PlatformMinimizeWindow(Ref<InternalWindow> window);
		//-------------------------------------------------------------------------------------------------------------------//
		//Single Platform Functions------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//-------//
		//Windows//
		//-------//		
#ifdef TRAP_PLATFORM_WINDOWS
		//Replacement for IsWindowsVersionOrGreater as MinGW lacks versionhelpers.h
		static BOOL IsWindowsVersionOrGreaterWin32(WORD major, WORD minor, WORD sp);
		//Checks whether we are on at least the specified build of Windows 10
		static BOOL IsWindows10BuildOrGreaterWin32(WORD build);
		//Checks whether we are on at least Windows 10 Anniversary Update
		static BOOL IsWindows10AnniversaryUpdateOrGreaterWin32();
		//Checks whether we are on at least Windows 10 Creators Update
		static BOOL IsWindows10CreatorsUpdateOrGreaterWin32();
		//Checks whether we are on at least Windows 8.1
		static BOOL IsWindows8Point1OrGreaterWin32();
		//Checks whether we are on at least Windows 8
		static BOOL IsWindows8OrGreaterWin32();
		//Checks whether we are on at least Windows 7
		static BOOL IsWindows7OrGreaterWin32();
		//Checks whether we are on at least Windows Vista
		static BOOL IsWindowsVistaOrGreaterWin32();
		//Checks whether we are on at least Windows XP
		static BOOL IsWindowsXPOrGreaterWin32();
		//Returns an UTF-8 string version of the specified wide string
		static std::string CreateUTF8StringFromWideStringWin32(const std::wstring& wStr);
		//Returns a wide string string version of the specified UTF-8 string
		static std::wstring CreateWideStringFromUTF8StringWin32(const std::string& str);
		//Load necessary libraries (DLLs)
		static bool LoadLibraries();
		//Unload used libraries (DLLs)
		static void FreeLibraries();
		//Reports the specified error, appending information about the last Win32 error
		static void InputErrorWin32(Error error, std::string description);
		//Notifies shared code of a mouse button click event
		static void InputMouseClick(Ref<InternalWindow> window, Input::MouseButton button, bool pressed);
		//Notifies shared code that a window has lost or received input focus
		static void InputWindowFocus(Ref<InternalWindow> window, bool focused);
		//Notifies shared code of a Unicode codepoint input event
		//The 'plain' parameter determines whether to emit a regular character event
		static void InputChar(Ref<InternalWindow> window, uint32_t codePoint);
		//Notifies shared code of a cursor motion event
		//The position is specified in content area relative screen coordinates
		static void InputCursorPos(Ref<InternalWindow> window, double xPos, double yPos);
		//Notifies shared code of a scroll event
		static void InputScroll(Ref<InternalWindow> window, double xOffset, double yOffset);
		//Notifies shared code that a window framebuffer has been resized
		//The size is specified in pixels
		static void InputFrameBufferSize(Ref<InternalWindow> window, int32_t width, int32_t height); 
		//Notifies shared code that a window has been resized
		//The size is specified in screen coordinates
		static void InputWindowSize(Ref<InternalWindow> window, int32_t width, int32_t height);
		//Notifies shared code that a window has moved
		//The position is specified in content area relative screen coordinates
		static void InputWindowPos(Ref<InternalWindow> window, int32_t x, int32_t y);
		//Notifies shared code that a window content scale has changed
		//The scale is specified as the ratio between the current and default DPI
		static void InputWindowContentScale(Ref<InternalWindow> window, float xScale, float yScale);
		//Notifies shared code of files or directories dropped on a window
		static void InputDrop(Ref<InternalWindow> window, const std::vector<std::string>& paths);
		//Translates a Windows key to the corresponding TRAP key
		static Input::Key TranslateKey(WPARAM wParam, LPARAM lParam);
		//Updates key names according to the current keyboard layout
		static void UpdateKeyNamesWin32();
		//Window callback function (handles window messages)
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		//Registers the TRAP window class
		static bool RegisterWindowClassWin32();
		//Unregisters the TRAP window class
		static void UnregisterWindowClassWin32();
		//Callback for EnumDisplayMonitors in createMonitor
		static BOOL CALLBACK MonitorCallback(HMONITOR handle, HDC dc, RECT* rect, LPARAM data);
		//Create monitor from an adapter and (optionally) a display
		static Ref<InternalMonitor> CreateMonitor(DISPLAY_DEVICEW* adapter, DISPLAY_DEVICEW* display);
		//Notifies shared code of a monitor connection or disconnection
		static void InputMonitor(Ref<InternalMonitor> monitor, bool connected, uint32_t placement);
		//Poll for changes in the set of connected monitors
		static void PollMonitorsWin32();
		//Make the specified window and its video mode active on its monitor
		static void AcquireMonitor(Ref<InternalWindow> window);
		//Remove the window and restore the original video mode
		static void ReleaseMonitor(Ref<InternalWindow> window);
		static void FitToMonitor(Ref<InternalWindow> window);		
		//Lexically compare video modes, used by qsort
		static int32_t CompareVideoModes(const void* fp, const void* sp);
		//Retrieves the available modes for the specified monitor
		static bool RefreshVideoModes(Ref<InternalMonitor> monitor);
		//Chooses the video mode most closely matching the desired one
		static VideoMode* ChooseVideoMode(Ref<InternalMonitor> monitor, const VideoMode& desired);
		//Change the current video mode
		static void SetVideoModeWin32(Ref<InternalMonitor> monitor, const VideoMode& desired);
		static void GetMonitorContentScaleWin32(HMONITOR handle, float& xScale, float& yScale);
		//Splits a color depth into red, green and blue bit depths
		static void SplitBPP(int32_t bpp, int32_t& red, int32_t& green, int32_t& blue);
		//Returns the window style for the specified window
		static DWORD GetWindowStyle(Ref<InternalWindow> window);
		//Returns the extended window style for the specified window
		static DWORD GetWindowExStyle(Ref<InternalWindow> window); 
		//Creates the TRAP window
		static int32_t CreateNativeWindow(Ref<InternalWindow>& window,
			                              const WindowConfig& WNDConfig,
			                              const FrameBufferConfig& FBConfig);
		//Creates a dummy window for behind-the-scenes work
		static bool CreateHelperWindow();
		//Translate content area size to full window size according to styles and DPI
		static void GetFullWindowSize(DWORD style, DWORD exStyle,
			                          int32_t contentWidth, int32_t contentHeight,
			                          int32_t& fullWidth, int32_t& fullHeight,
			                          UINT dpi);		
		static bool InitWGL();
		//Shutdown WGL
		static void ShutdownWGL();
		static bool CreateContextWGL(const Ref<InternalWindow>& window,
			                         const ContextConfig& CTXConfig,
			                         const FrameBufferConfig& FBConfig);
		static void DestroyContextWGL(const Ref<InternalWindow>& window);
		static void MakeContextCurrentWGL(Ref<InternalWindow> window);
		static void SwapBuffersWGL(const Ref<InternalWindow>& window);
		static void SwapIntervalWGL(int32_t interval);
		static GLProcess GetProcAddressWGL(const char* procName);
		static bool ExtensionSupportedWGL(const char* extension);		
		//Searches an extension string for the specified extension
		static bool StringInExtensionString(const char* string, const char* extensions);
		//Return the value corresponding to the specified attribute
		static int32_t FindPixelFormatAttribValue(const std::vector<int32_t>& attribs,
			                                      const std::vector<int32_t>& values,
			                                      int32_t attrib);
		//Chooses the framebuffer config that best matches the desired one
		static const FrameBufferConfig* ChooseFBConfig(const FrameBufferConfig& desired,
			                                           const std::vector<FrameBufferConfig>& alternatives);
		//Returns a list of available and usable framebuffer configs
		static int32_t ChoosePixelFormat(Ref<InternalWindow> window,
			                             const ContextConfig& CTXConfig,
			                             const FrameBufferConfig& FBConfig);		
		static bool RefreshContextAttribs(const Ref<InternalWindow>& window,
			                              const ContextConfig& CTXConfig);
		//Returns whether the cursor is in the content area of the specified window
		static bool CursorInContentArea(Ref<InternalWindow> window);
		//Updates the cursor image according to its cursor mode
		static void UpdateCursorImage(Ref<InternalWindow> window);
		//Creates an RGBA icon or cursor
		static HICON CreateIcon(const Scope<Image>& image, int32_t xHot, int32_t yHot, bool icon);
		//Notifies shared code that the user wishes to close a window
		static void InputWindowCloseRequest(Ref<InternalWindow> window);
		//Notifies shared code of a physical key event
		static void InputKey(Ref<InternalWindow> window, Input::Key key, int32_t scancode, bool action);
		//Updates the cursor clip rect
		static void UpdateClipRect(Ref<InternalWindow> window);
		//Enables WM_INPUT messages for the mouse for the specified window
		static void EnableRawMouseMotion(Ref<InternalWindow> window);
		//Disables WM_INPUT messages for the mouse
		static void DisableRawMouseMotion(Ref<InternalWindow> window);
		//Exit disabled cursor mode for the specified window
		static void EnableCursor(Ref<InternalWindow> window);
		//Apply disabled cursor mode to a focused window
		static void DisableCursor(Ref<InternalWindow> window);
		static bool InitVulkan(uint32_t mode);
		static std::string GetVulkanResultString(VkResult result);
#endif
	};
}
#endif /*_TRAP_WINDOWINGAPI_H_*/