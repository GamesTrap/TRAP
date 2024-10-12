/*
Copyright (c) 2002-2006 Marcus Geelnard

Copyright (c) 2006-2022 Camilla Loewy

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would
   be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.

Modified by: Jan "GamesTrap" Schuerkamp
*/

#ifndef TRAP_WINDOWINGAPI_H
#define TRAP_WINDOWINGAPI_H

#ifndef TRAP_HEADLESS_MODE

#include "Core/PlatformDetection.h"

#include <forward_list>
#include <optional>

#ifdef TRAP_PLATFORM_LINUX
#include "Utils/Linux.h"
#elif defined(TRAP_PLATFORM_WINDOWS)
#include "Utils/Win.h"
#endif

#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#include "ImageLoader/Image.h"
#include "Input/Input.h"
#include "Maths/Math.h"

namespace TRAP::INTERNAL
{
	/// @remark @headless This class is not available in headless mode.
	class WindowingAPI final
	{
	public:
		/// @brief Constructor.
		consteval WindowingAPI() = delete;
		/// @brief Destructor.
		constexpr ~WindowingAPI() = default;

		/// @brief Copy constructor.
		consteval WindowingAPI(const WindowingAPI&) = delete;
		/// @brief Copy assignment operator.
		consteval WindowingAPI& operator=(const WindowingAPI&) = delete;
		/// @brief Move constructor.
		consteval WindowingAPI(WindowingAPI&&) = delete;
		/// @brief Move assignment operator.
		consteval WindowingAPI& operator=(WindowingAPI&&) = delete;

		//-------------------------------------------------------------------------------------------------------------------//
		//Forward Declarations-----------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
		struct InternalWindow;
		struct InternalMonitor;
		struct InternalCursor;
		enum class Error : u8;
	private:
		struct VkWin32SurfaceCreateInfoKHR;
		struct VkWaylandSurfaceCreateInfoKHR;
		struct VkXlibSurfaceCreateInfoKHR;
		struct VkXcbSurfaceCreateInfoKHR;
		//-------//
		//Windows//
		//-------//
#ifdef TRAP_PLATFORM_WINDOWS
		enum class Monitor_DPI_Type;
		enum class Process_DPI_Awareness;
		enum class PreferredAppMode;
		//-----//
		//Linux//
		//-----//
#elif defined(TRAP_PLATFORM_LINUX)
		struct xcb_connection_t;
		using xcb_window_t = XID;
		using xcb_visualid_t = XID;

		struct wl_cursor_theme;
		struct wl_cursor_image;
		struct wl_cursor;

		struct libdecor;
		struct libdecor_frame;
		struct libdecor_state;
		struct libdecor_configuration;
		struct libdecor_interface;
		struct libdecor_frame_interface;
		enum class libdecor_capabilities;
		enum class libdecor_window_state : u32;
#endif
		//-------------------------------------------------------------------------------------------------------------------//
		//Typedefs-----------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Callbacks//
		//---------//
	public:
		/// @brief The function pointer type for window position callbacks.
		/// @param window The window that was moved.
		/// @param xPos The new x-coordinate, in screen coordinates, of the upper-left corner of the content area.
		/// @param yPos The new y-coordinate, in screen coordinates, of the upper-left corner of the content area.
		using WindowPositionFunc = void(*)(const InternalWindow& window, i32 xPos, i32 yPos);
		/// @brief The function pointer type for window size callbacks.
		/// @param window The window that was resized.
		/// @param width The new width, in screen coordinates.
		/// @param height The new height, in screen coordinates.
		using WindowSizeFunc = void(*)(const InternalWindow& window, i32 width, i32 height);
		/// @brief The function pointer type for window minimize/iconfiy callbacks.
		/// @param window The window that was minimized/iconified or restored.
		/// @param restored True if the window was minimized/iconified, or false if it was restored.
		using WindowMinimizeFunc = void(*)(const InternalWindow& window, bool restored);
		/// @brief The function pointer type for window maximize callbacks.
		/// @param window The window that was maximized or restored.
		/// @param restored True if the window was maximized, or false if it was restored.
		using WindowMaximizeFunc = void(*)(const InternalWindow& window, bool restored);
		/// @brief The function pointer type for window close callbacks.
		/// @param window The window that the user attempted to close.
		using WindowCloseFunc = void(*)(const InternalWindow& window);
		/// @brief The function pointer type for window focus callbacks.
		/// @param window The window that gained or lost input focus.
		/// @param focused True if the window was given input focus, or false if it lost it.
		using WindowFocusFunc = void(*)(const InternalWindow& window, bool focused);
		/// @brief The function pointer type for framebuffer size callbacks.
		/// @param window The window whose framebuffer was resized.
		/// @param width The new width, in pixels, of the framebuffer.
		/// @param height The new height, in pixels, of the framebuffer.
		using FrameBufferSizeFunc = void(*)(const InternalWindow& window, i32 width, i32 height);
		/// @brief The function pointer type for window content scale callbacks.
		/// @param window The window whose content scale changed.
		/// @param xScale The new x-axis content scale of the window.
		/// @param yScale The new y-axis content scale of the window.
		using WindowContentScaleFunc = void(*)(const InternalWindow& window, f32 xScale, f32 yScale);
		/// @brief The function pointer type for mouse button callbacks.
		/// @param window The window that received the event.
		/// @param mouseButton The mouse button that was pressed or released.
		/// @param state State of the mouse button (pressed or released).
		using MouseButtonFunc = void(*)(const InternalWindow& window, Input::MouseButton mouseButton, TRAP::Input::KeyState state);
		/// @brief The function pointer type for cursor position callbacks.
		/// @param window>The window that received the event.
		/// @param xPos The new cursor x-coordinate, relative to the left edge of the content area.
		/// @param yPos The new cursor y-coordinate, relative to the top edge of the content area.
		using CursorPositionFunc = void(*)(const InternalWindow& window, f64 xPos, f64 yPos);
		/// @brief The function pointer type for cursor enter callbacks.
		/// @param window The window that received the event.
		/// @param entered True if the cursor entered the window's content area, false if it left it.
		using CursorEnterFunc = void(*)(const InternalWindow& window, bool entered);
		/// @brief The function pointer type for scroll callbacks.
		/// @param window The window that received the event.
		/// @param xOffset The scroll offset along the x-axis.
		/// @param yOffset The scroll offset along the y-axis.
		using ScrollFunc = void(*)(const InternalWindow& window, f64 xOffset, f64 yOffset);
		/// @brief The function pointer type for keyboard key callbacks.
		/// @param window The window that received the event.
		/// @param key The key that was pressed, repeated or released.
		/// @param state The state of the key.
		using KeyFunc = void(*)(const InternalWindow& window, Input::Key key, Input::KeyState state);
		/// @brief The function pointer type for Unicode character callbacks.
		/// @param window The window that received the event.
		/// @param codePoint The Unicode code point of the character.
		using CharFunc = void(*)(const InternalWindow& window, u32 codePoint);
		/// @brief The function pointer type for path drop callbacks.
		/// @param window The window that received the event.
		/// @param paths The UTF-8 encoded file and/or directory path names.
		using DropFunc = void(*)(const InternalWindow& window, std::vector<std::string> paths);
		/// @brief The function pointer type for monitor configuration callbacks.
		/// @param monitor The monitor that was connected or disconnected.
		/// @param connceted True if monitor got connceted or false if it got disconnceted.
		using MonitorFunc = void(*)(const InternalMonitor& monitor, bool connected);
	private:
		//--------------//
		//Vulkan Surface//
		//--------------//
#ifdef TRAP_PLATFORM_WINDOWS
		using VkWin32SurfaceCreateFlagsKHR = VkFlags;
		using PFN_vkCreateWin32SurfaceKHR = VkResult(*)(VkInstance, const VkWin32SurfaceCreateInfoKHR*,
		                                                const VkAllocationCallbacks*, VkSurfaceKHR*);
#elif defined(TRAP_PLATFORM_LINUX)
		using PFN_vkCreateXlibSurfaceKHR = VkResult(*)(VkInstance, const VkXlibSurfaceCreateInfoKHR*,
		                                               const VkAllocationCallbacks*, VkSurfaceKHR*);
		using PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR = VkBool32(*)(VkPhysicalDevice, u32,
		                                                                      Display*, VisualID);
		using PFN_vkCreateXcbSurfaceKHR = VkResult(*)(VkInstance, const VkXcbSurfaceCreateInfoKHR*,
		                                              const VkAllocationCallbacks*, VkSurfaceKHR*);
		using PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR = VkBool32(*)(VkPhysicalDevice, u32,
		                                                                     xcb_connection_t*, xcb_visualid_t);
		using PFN_vkCreateWaylandSurfaceKHR = VkResult(*)(VkInstance, const VkWaylandSurfaceCreateInfoKHR*,
		                                              const VkAllocationCallbacks*, VkSurfaceKHR*);
		using PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR = VkBool32(*)(VkPhysicalDevice, u32,
		                                                                         wl_display*);
#endif
		//-------//
		//Windows//
		//-------//
#ifdef TRAP_PLATFORM_WINDOWS
		//user32.dll function pointer typedefs
		using PFN_SetProcessDPIAware = BOOL(WINAPI*)();
		using PFN_ChangeWindowMessageFilterEx = BOOL(WINAPI*)(HWND, UINT, DWORD, CHANGEFILTERSTRUCT*);
		using PFN_EnableNonClientDPIScaling = BOOL(WINAPI*)(HWND);
		using PFN_SetProcessDPIAwarenessContext = BOOL(WINAPI*)(HANDLE);
		using PFN_GetDPIForWindow = UINT(WINAPI*)(HWND);
		using PFN_AdjustWindowRectExForDPI = BOOL(WINAPI*)(LPRECT, DWORD, BOOL, DWORD, UINT);
		using PFN_GetSystemMetricsForDPI = i32(WINAPI*)(i32, UINT);
		using PFN_GetDisplayConfigBufferSizes = LONG(WINAPI*)(UINT32, UINT32*, UINT32*);
		using PFN_QueryDisplayConfig = LONG(WINAPI*)(UINT32, UINT32*, DISPLAYCONFIG_PATH_INFO*, UINT32*, DISPLAYCONFIG_MODE_INFO*, DISPLAYCONFIG_TOPOLOGY_ID*);
		using PFN_DisplayConfigGetDeviceInfo = LONG(WINAPI*)(DISPLAYCONFIG_DEVICE_INFO_HEADER*);

		//dwmapi.dll function pointer typedefs
		using PFN_DwmIsCompositionEnabled = HRESULT(WINAPI*)(BOOL*);
		using PFN_DwmFlush = HRESULT(WINAPI*)();
		using PFN_DwmEnableBlurBehindWindow = HRESULT(WINAPI*)(HWND, const DWM_BLURBEHIND*);
		using PFN_DwmSetWindowAttribute = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);

		//uxtheme.dll function pointer typedefs
		using PFN_ShouldAppsUseDarkMode = BOOL(WINAPI*)();
		using PFN_SetPreferredAppMode = PreferredAppMode(WINAPI*)(PreferredAppMode);

		//shcore.dll function pointer typedefs
		using PFN_SetProcessDPIAwareness = HRESULT(WINAPI*)(Process_DPI_Awareness);
		using PFN_GetDPIForMonitor = HRESULT(WINAPI*)(HMONITOR, Monitor_DPI_Type, UINT*, UINT*);
#elif defined(TRAP_PLATFORM_LINUX)
		//----------//
		//Linux(X11)//
		//----------//

		//RandR
		using PFN_XRRFreeCrtcInfo = void(*)(XRRCrtcInfo*);
		using PFN_XRRFreeOutputInfo = void(*)(XRROutputInfo*);
		using PFN_XRRFreeScreenResources = void(*)(XRRScreenResources*);
		using PFN_XRRGetCrtcInfo = XRRCrtcInfo*(*)(Display*, XRRScreenResources*, RRCrtc);
		using PFN_XRRGetOutputInfo = XRROutputInfo*(*)(Display*, XRRScreenResources*, RROutput);
		using PFN_XRRGetOutputPrimary = RROutput(*)(Display*, ::Window);
		using PFN_XRRGetScreenResourcesCurrent = XRRScreenResources*(*)(Display*, ::Window);
		using PFN_XRRQueryExtension = i32(*)(Display*, i32*, i32*);
		using PFN_XRRQueryVersion = i32(*)(Display*, i32*, i32*);
		using PFN_XRRSelectInput = void(*)(Display*, ::Window, i32);
		using PFN_XRRSetCrtcConfig = i32(*)(Display*, XRRScreenResources*, RRCrtc, Time, i32, i32,
		                                        RRMode, Rotation, RROutput*, i32);
		using PFN_XRRUpdateConfiguration = i32(*)(XEvent*);

		//XCursor
		using PFN_XcursorImageCreate = XcursorImage*(*)(i32, i32);
		using PFN_XcursorImageDestroy = void(*)(XcursorImage*);
		using PFN_XcursorImageLoadCursor = Cursor(*)(Display*, const XcursorImage*);
		using PFN_XcursorGetTheme = char*(*)(Display*);
		using PFN_XcursorGetDefaultSize = i32(*)(Display*);
		using PFN_XcursorLibraryLoadImage = XcursorImage*(*)(const char*, const char*, i32);

		//Xinerama
		using PFN_XineramaIsActive = i32 (*)(Display*);
		using PFN_XineramaQueryExtension = i32(*)(Display*, i32*, i32*);
		using PFN_XineramaQueryScreens = XineramaScreenInfo*(*)(Display*, i32*);

		//XCB
		using PFN_XGetXCBConnection = xcb_connection_t*(*)(Display*);

		//XI
		using PFN_XIQueryVersion = i32(*)(Display*, i32*, i32*);
		using PFN_XISelectEvents = i32(*)(Display*, ::Window, XIEventMask*, i32);

		//XRender
		using PFN_XRenderQueryExtension = i32(*)(Display*,i32*, i32*);
		using PFN_XRenderQueryVersion = i32(*)(Display* dpy, i32*, i32*);
		using PFN_XRenderFindVisualFormat = XRenderPictFormat*(*)(Display*, Visual const*);

		//Vulkan
		using VkXlibSurfaceCreateFlagsKHR = VkFlags;
		using VkXcbSurfaceCreateFlagsKHR = VkFlags;
		using VkWaylandSurfaceCreateFlagsKHR = VkFlags;

		//XShape
		using PFN_XShapeQueryExtension = i32(*)(Display*, i32*, i32*);
		using PFN_XShapeQueryVersion = i32(*)(Display* dpy, i32*, i32*);
		using PFN_XShapeCombineRegion = void(*)(Display*, ::Window, i32, i32, i32, Region, i32);
		using PFN_XShapeCombineMask = void(*)(Display*, XID, i32, i32, i32, Pixmap, i32);

		//XLib
		using PFN_XAllocClassHint = XClassHint*(*)();
		using PFN_XAllocSizeHints = XSizeHints*(*)();
		using PFN_XAllocWMHints = XWMHints*(*)();
		using PFN_XChangeProperty = i32(*)(Display*, ::Window, Atom, Atom, i32, i32, const unsigned char*, i32);
		using PFN_XChangeWindowAttributes = i32(*)(Display*, ::Window, u64, XSetWindowAttributes*);
		using PFN_XCheckIfEvent = i32(*)(Display*, XEvent*, i32(*)(Display*, XEvent*, XPointer), XPointer);
		using PFN_XCheckTypedWindowEvent = i32(*)(Display*, ::Window, i32, XEvent*);
		using PFN_XCloseDisplay = i32(*)(Display*);
		using PFN_XCloseIM = i32(*)(XIM);
		using PFN_XConvertSelection = i32(*)(Display*, Atom, Atom, Atom, ::Window, Time);
		using PFN_XCreateColormap = Colormap(*)(Display*, ::Window, Visual*, i32);
		using PFN_XCreateFontCursor = Cursor(*)(Display*, u32);
		using PFN_XCreateIC = XIC(*)(XIM, ...);
		using PFN_XCreateWindow = ::Window(*)(Display*, ::Window, i32, i32, u32, u32, u32, i32,
		                                      u32, Visual*, u64, XSetWindowAttributes*);
		using PFN_XDefineCursor = i32(*)(Display*, ::Window, Cursor);
		using PFN_XDeleteContext = i32(*)(Display*, XID, XContext);
		using PFN_XDeleteProperty = i32(*)(Display*, ::Window, Atom);
		using PFN_XDestroyIC = void(*)(XIC);
		using PFN_XDestroyWindow = i32(*)(Display*, ::Window);
		using PFN_XDisplayKeycodes = i32(*)(Display*, i32*, i32*);
		using PFN_XEventsQueued = i32(*)(Display*, i32);
		using PFN_XFilterEvent = i32(*)(XEvent*, ::Window);
		using PFN_XFindContext = i32(*)(Display*, XID, XContext, XPointer*);
		using PFN_XFlush = i32(*)(Display*);
		using PFN_XFree = i32(*)(void*);
		using PFN_XFreeColormap = i32(*)(Display*, Colormap);
		using PFN_XFreeCursor = i32(*)(Display*, Cursor);
		using PFN_XFreeEventData = void(*)(Display*, XGenericEventCookie*);
		using PFN_XGetAtomName = char*(*)(Display*, Atom);
		using PFN_XGetErrorText = i32(*)(Display*, i32, char*, i32);
		using PFN_XGetEventData = i32(*)(Display*, XGenericEventCookie*);
		using PFN_XGetICValues = char*(*)(XIC, ...);
		using PFN_XGetIMValues = char*(*)(XIM, ...);
		using PFN_XGetInputFocus = i32(*)(Display*, ::Window*, i32*);
		using PFN_XGetKeyboardMapping = KeySym*(*)(Display*, KeyCode, i32, i32*);
		using PFN_XGetScreenSaver = i32(*)(Display*, i32*, i32*, i32*, i32*);
		using PFN_XGetSelectionOwner = ::Window(*)(Display*, Atom);
		using PFN_XGetVisualInfo = XVisualInfo*(*)(Display*, i64, XVisualInfo*, i32*);
		using PFN_XGetWMNormalHints = i32(*)(Display*, ::Window, XSizeHints*, i64*);
		using PFN_XGetWindowAttributes = i32(*)(Display*, ::Window, XWindowAttributes*);
		using PFN_XGetWindowProperty = i32(*)(Display*, ::Window, Atom, i64, i64, i32, Atom, Atom*, i32*, u64*,
		                                      u64*, unsigned char**);
		using PFN_XGrabPointer = i32(*)(Display*, ::Window, i32, u32, i32, i32, ::Window, Cursor, Time);
		using PFN_XIconifyWindow = i32(*)(Display*, ::Window, i32);
		using PFN_XInitThreads = i32(*)();
		using PFN_XInternAtom = Atom(*)(Display*, const char*, i32);
		using PFN_XLookupString = i32(*)(XKeyEvent*, char*, i32, KeySym*, XComposeStatus*);
		using PFN_XMapRaised = i32(*)(Display*, ::Window);
		using PFN_XMapWindow = i32(*)(Display*, ::Window);
		using PFN_XMoveResizeWindow = i32(*)(Display*, ::Window, i32, i32, u32, u32);
		using PFN_XMoveWindow = i32(*)(Display*, ::Window, i32, i32);
		using PFN_XNextEvent = i32(*)(Display*, XEvent*);
		using PFN_XOpenDisplay = Display*(*)(const char*);
		using PFN_XOpenIM = XIM(*)(Display*, XrmDatabase*, char*, char*);
		using PFN_XPeekEvent = i32(*)(Display*, XEvent*);
		using PFN_XPending = i32(*)(Display*);
		using PFN_XQueryExtension = i32(*)(Display*, const char*, i32*, i32*, i32*);
		using PFN_XQueryPointer = i32(*)(Display*, ::Window, ::Window*, ::Window*, i32*, i32*, i32*, i32*, u32*);
		using PFN_XRaiseWindow = i32(*)(Display*, ::Window);
		using PFN_XRegisterIMInstantiateCallback = i32(*)(Display*, void*, char*, char*, XIDProc, XPointer);
		using PFN_XResizeWindow = i32(*)(Display*, ::Window, u32, u32);
		using PFN_XResourceManagerString = char*(*)(Display*);
		using PFN_XSaveContext = i32(*)(Display*, XID, XContext, const char*);
		using PFN_XSelectInput = i32(*)(Display*, ::Window, i64);
		using PFN_XSendEvent = i32(*)(Display*, ::Window, i32, i64, XEvent*);
		using PFN_XSetClassHint = i32(*)(Display*, ::Window, XClassHint*);
		using PFN_XSetErrorHandler = XErrorHandler(*)(XErrorHandler);
		using PFN_XSetICFocus = void(*)(XIC);
		using PFN_XSetIMValues = char*(*)(XIM, ...);
		using PFN_XSetInputFocus = i32(*)(Display*, ::Window, i32, Time);
		using PFN_XSetLocaleModifiers = char*(*)(const char*);
		using PFN_XSetScreenSaver = i32(*)(Display*, i32, i32, i32, i32);
		using PFN_XSetSelectionOwner = i32(*)(Display*, Atom, ::Window, Time);
		using PFN_XSetWMHints = i32(*)(Display*, ::Window, XWMHints*);
		using PFN_XSetWMNormalHints = void(*)(Display*, ::Window, XSizeHints*);
		using PFN_XSetWMProtocols = i32(*)(Display*, ::Window, Atom*, i32);
		using PFN_XSupportsLocale = i32(*)();
		using PFN_XSync = i32(*)(Display*, i32);
		using PFN_XTranslateCoordinates = i32(*)(Display*, ::Window, ::Window, i32, i32, i32*, i32*, ::Window*);
		using PFN_XUndefineCursor = i32(*)(Display*, ::Window);
		using PFN_XUngrabPointer = i32(*)(Display*, Time);
		using PFN_XUnmapWindow = i32(*)(Display*, ::Window);
		using PFN_XUnsetICFocus = void(*)(XIC);
		using PFN_XVisualIDFromVisual = VisualID(*)(Visual*);
		using PFN_XWarpPointer = i32(*)(Display*, ::Window, ::Window, i32, i32, u32, u32, i32, i32);
		using PFN_XCreateRegion = Region(*)();
		using PFN_XDestroyRegion = i32(*)(Region);

		//XKB
		using PFN_XkbAllocKeyboard = XkbDescPtr(*)();
		using PFN_XkbFreeKeyboard = void(*)(XkbDescPtr, u32, i32);
		using PFN_XkbFreeNames = void(*)(XkbDescPtr, u32, i32);
		using PFN_XkbGetMap = XkbDescPtr(*)(Display*, u32, u32);
		using PFN_XkbGetNames = i32(*)(Display*, u32, XkbDescPtr);
		using PFN_XkbGetState = i32(*)(Display*, u32, XkbStatePtr);
		using PFN_XkbKeycodeToKeysym = KeySym(*)(Display*, KeyCode, i32, i32);
		using PFN_XkbQueryExtension = i32(*)(Display*, i32*, i32*, i32*, i32*, i32*);
		using PFN_XkbSelectEventDetails = i32(*)(Display*, u32, u32, u64, u64);
		using PFN_XkbSetDetectableAutoRepeat = i32(*)(Display*, i32, i32*);
		using PFN_Xutf8LookupString = i32(*)(XIC, XKeyPressedEvent*, char*, i32, KeySym*, i32*);
		using PFN_Xutf8SetWMProperties = void(*)(Display*, ::Window, const char*, const char*, char**, i32, XSizeHints*,
		                                         XWMHints*, XClassHint*);

		//XRM
		using PFN_XrmDestroyDatabase = void(*)(XrmDatabase);
		using PFN_XrmGetResource = i32(*)(XrmDatabase, const char*, const char*, char**, XrmValue*);
		using PFN_XrmGetStringDatabase = XrmDatabase(*)(const char*);
		using PFN_XrmInitialize = void(*)();
		using PFN_XrmUniqueQuark = XrmQuark(*)();
		using PFN_XUnregisterIMInstantiateCallback = i32(*)(Display*, void*, char*, char*, XIDProc, XPointer);

		//--------------//
		//Linux(Wayland)//
		//--------------//

		//Display
		using PFN_wl_display_flush = i32(*)(wl_display*);
		using PFN_wl_display_cancel_read = void(*)(wl_display*);
		using PFN_wl_display_dispatch_pending = i32(*)(wl_display*);
		using PFN_wl_display_read_events = i32(*)(wl_display*);
		using PFN_wl_display_connect = wl_display*(*)(const char*);
		using PFN_wl_display_disconnect = void(*)(wl_display*);
		using PFN_wl_display_roundtrip = i32(*)(wl_display*);
		using PFN_wl_display_get_fd = i32(*)(wl_display*);
		using PFN_wl_display_prepare_read = i32(*)(wl_display*);

		//Proxy
		using PFN_wl_proxy_marshal = void(*)(wl_proxy*, u32, ...);
		using PFN_wl_proxy_add_listener = i32(*)(wl_proxy*, void(**)(void), void*);
		using PFN_wl_proxy_destroy = void(*)(wl_proxy*);
		using PFN_wl_proxy_marshal_constructor = wl_proxy*(*)(wl_proxy*, u32, const wl_interface*, ...);
		using PFN_wl_proxy_marshal_constructor_versioned = wl_proxy*(*)(wl_proxy*, u32, const wl_interface*, u32, ...);
		using PFN_wl_proxy_get_user_data = void*(*)(wl_proxy*);
		using PFN_wl_proxy_set_user_data = void(*)(wl_proxy*, void*);
		using PFN_wl_proxy_set_tag = void(*)(wl_proxy*, const char* const*);
		using PFN_wl_proxy_get_tag = const char* const*(*)(wl_proxy*);
		using PFN_wl_proxy_get_version = u32(*)(wl_proxy*);
		using PFN_wl_proxy_marshal_flags = wl_proxy*(*)(wl_proxy*, u32, const wl_interface*, u32, u32, ...);

		//Cursor
		using PFN_wl_cursor_theme_load = wl_cursor_theme*(*)(const char*, i32, wl_shm*);
		using PFN_wl_cursor_theme_destroy = void(*)(wl_cursor_theme*);
		using PFN_wl_cursor_theme_get_cursor = wl_cursor*(*)(wl_cursor_theme*, const char*);
		using PFN_wl_cursor_image_get_buffer = wl_buffer*(*)(wl_cursor_image*);

		//XKB
		using PFN_xkb_context_new = xkb_context*(*)(xkb_context_flags);
		using PFN_xkb_context_unref = void(*)(xkb_context*);
		using PFN_xkb_keymap_new_from_string = xkb_keymap*(*)(xkb_context*, const char*, xkb_keymap_format, xkb_keymap_compile_flags);
		using PFN_xkb_keymap_unref = void(*)(xkb_keymap*);
		using PFN_xkb_keymap_mod_get_index = xkb_mod_index_t(*)(xkb_keymap*, const char*);
		using PFN_xkb_keymap_key_repeats = i32(*)(xkb_keymap*, xkb_keycode_t);
		using PFN_xkb_keymap_key_get_syms_by_level = i32(*)(xkb_keymap*, xkb_keycode_t, xkb_layout_index_t, xkb_level_index_t, const xkb_keysym_t**);
		using PFN_xkb_state_new = xkb_state*(*)(xkb_keymap*);
		using PFN_xkb_state_unref = void(*)(xkb_state*);
		using PFN_xkb_state_key_get_syms = i32(*)(xkb_state*, xkb_keycode_t, const xkb_keysym_t**);
		using PFN_xkb_state_update_mask = xkb_state_component(*)(xkb_state*, xkb_mod_mask_t, xkb_mod_mask_t, xkb_mod_mask_t, xkb_layout_index_t, xkb_layout_index_t, xkb_layout_index_t);
		using PFN_xkb_keymap_layout_get_name = const char*(*)(xkb_keymap*, xkb_layout_index_t);
		using PFN_xkb_state_key_get_layout = xkb_layout_index_t(*)(xkb_state*, xkb_keycode_t);
		using PFN_xkb_state_mod_index_is_active = i32(*)(xkb_state*, xkb_mod_index_t, xkb_state_component);

		//XKB Compose
		using PFN_xkb_compose_table_new_from_locale = xkb_compose_table*(*)(xkb_context*, const char*, xkb_compose_compile_flags);
		using PFN_xkb_compose_table_unref = void(*)(xkb_compose_table*);
		using PFN_xkb_compose_state_new = xkb_compose_state*(*)(xkb_compose_table*, xkb_compose_state_flags);
		using PFN_xkb_compose_state_unref = void(*)(xkb_compose_state*);
		using PFN_xkb_compose_state_feed = xkb_compose_feed_result(*)(xkb_compose_state*, xkb_keysym_t);
		using PFN_xkb_compose_state_get_status = xkb_compose_status(*)(xkb_compose_state*);
		using PFN_xkb_compose_state_get_one_sym = xkb_keysym_t(*)(xkb_compose_state*);

		//LibDecor
		using PFN_libdecor_new = libdecor*(*)(wl_display*, const libdecor_interface*);
		using PFN_libdecor_unref = void(*)(libdecor*);
		using PFN_libdecor_get_fd = i32(*)(libdecor*);
		using PFN_libdecor_dispatch = i32(*)(libdecor*, i32);
		using PFN_libdecor_decorate = libdecor_frame*(*)(libdecor*, wl_surface*, const libdecor_frame_interface*, void*);
		using PFN_libdecor_frame_unref = void(*)(libdecor_frame*);
		using PFN_libdecor_frame_set_app_id = void(*)(libdecor_frame*, const char*);
		using PFN_libdecor_frame_set_title = void(*)(libdecor_frame*, const char*);
		using PFN_libdecor_frame_set_minimized = void(*)(libdecor_frame*);
		using PFN_libdecor_frame_set_fullscreen = void(*)(libdecor_frame*, wl_output*);
		using PFN_libdecor_frame_unset_fullscreen = void(*)(libdecor_frame*);
		using PFN_libdecor_frame_map = void(*)(libdecor_frame*);
		using PFN_libdecor_frame_commit = void(*)(libdecor_frame*, libdecor_state*, libdecor_configuration*);
		using PFN_libdecor_frame_set_min_content_size = void(*)(libdecor_frame*, i32, i32);
		using PFN_libdecor_frame_set_max_content_size = void(*)(libdecor_frame*, i32, i32);
		using PFN_libdecor_frame_set_maximized = void(*)(libdecor_frame*);
		using PFN_libdecor_frame_unset_maximized = void(*)(libdecor_frame*);
		using PFN_libdecor_frame_set_capabilities = void(*)(libdecor_frame*, libdecor_capabilities);
		using PFN_libdecor_frame_unset_capabilities = void(*)(libdecor_frame*, libdecor_capabilities);
		using PFN_libdecor_frame_set_visibility = void(*)(libdecor_frame*, bool visible);
		using PFN_libdecor_frame_get_xdg_toplevel = xdg_toplevel*(*)(libdecor_frame*);
		using PFN_libdecor_configuration_get_content_size = bool(*)(libdecor_configuration*, libdecor_frame*, i32*, i32*);
		using PFN_libdecor_configuration_get_window_state = bool(*)(libdecor_configuration*, libdecor_window_state*);
		using PFN_libdecor_state_new = libdecor_state*(*)(i32, i32);
		using PFN_libdecor_state_free = void(*)(libdecor_state*);
#endif
		//-------------------------------------------------------------------------------------------------------------------//
		//Enums--------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
	public:
		/// @brief Error codes.
		enum class Error : u8
		{
			No_Error, // No error has occurred.
			Not_Initialized, //WindowingAPI has not been initialized.
			Invalid_Enum, //One of the arguments to the function was an invalid enum value.
			Invalid_Value, //One of the arguments to the function was an invalid value.
			Out_Of_Memory, //A memory allocation failed. A bug in the WindowingAPI or the underlying operating system.
			API_Unavailable, //The WindowingAPI could not find support for the requested API on the system.
			Platform_Error, //A platform-specific error occurred that does not match any of the more specific categories.
			Format_Unavailable, //The requested format is not supported or available.
			Cursor_Unavailable, //The specified cursor shape is not available.
			Feature_Unavailable, //The requested feature is not provided by the platform.
			Feature_Unimplemented, //The requested feature is not yet implemented for the platform.
			Platform_Unavailable //Platform unavailable or no matching platform was found.
		};

		/// @brief Hints for window creation and runtime changes.
		enum class Hint : u8
		{
			Resizable,
			Maximized,
			Minimized,
			Visible,
			Focused,
			FocusOnShow,
			Hovered,
			Decorated,
			Floating,
			MousePassthrough
		};

		/// @brief Cursor modes that can be used.
		enum class CursorMode : u8
		{
			Normal,   //Makes the cursor visible and behaving normally.
			Hidden,   //Makes the cursor invisible when it is over the
			          //content area of the window but does not restrict the cursor from leaving.
			Disabled, //Hides and grabs the cursor, providing virtual and unlimited cursor movement.
			Captured  //Makes the cursor visible and confines it to the content area of the window.
		};

		/// @brief Cursor types that can be used.
		enum class CursorType : u32
		{
			Arrow = 0, //Regular arrow cursor shape.
			Input = 1, //Text input I-beasm cursor shape.
			Crosshair = 2, //Crosshair cursor shape.
			PointingHand = 3, //Pointing hand cursor shape.
			ResizeHorizontal = 4, //Horizontal resize/move arrow shape.
			ResizeVertical = 5, //Vertical resize/move arrow shape.
			ResizeDiagonalTopLeftBottomRight = 6, //Top-left to bottom-right diagonal resize/move arrow shape.
			ResizeDiagonalTopRightBottomLeft = 7, //Top-right to bottom-left diagonal resize/move arrow shape.
			ResizeAll = 8, //Omni-directional resize/move cursor shape.
			NotAllowed = 9 //Operation-not-allowed cursor shape.
		};

		/// @brief State of progress for a window.
		enum class ProgressState : u8
		{
			Disabled = 0,
			Indeterminate = 0x1,
			Normal = 0x2,
			Error = 0x4,
			Paused = 0x8
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

		enum class PreferredAppMode
		{
			Default = 0,
			AllowDark,
			ForceDark,
			ForceLight,

			COUNT
		};
#endif /*TRAP_PLATFORM_WINDOWS*/
		//-----//
		//Linux//
		//-----//
#ifdef TRAP_PLATFORM_LINUX
		enum class libdecor_error
		{
			CompositorIncompatible,
			InvalidFrameConfiguration,
		};

		enum class libdecor_window_state : u32
		{
			None = 0,
			Active = BIT(0u),
			Maximized = BIT(1u),
			Fullscreen = BIT(2u),
			TiledLeft = BIT(3u),
			TiledRight = BIT(4u),
			TiledTop = BIT(5u),
			TiledBottom = BIT(6u),
		};

		enum class libdecor_capabilities
		{
			ActionMove = BIT(0u),
			ActionResize = BIT(1u),
			ActionMinimize = BIT(2u),
			ActionFullscreen = BIT(3u),
			ActionClose = BIT(4u),
		};
#endif /*TRAP_PLATFORM_LINUX*/
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
		static constexpr u32 WM_COPYGLOBALDATA = 0x0049;
	#endif /*WM_COPYGLOBALDATA*/
#endif /*TRAP_PLATFORM_WINDOWS*/
		//-------------------------------------------------------------------------------------------------------------------//
		//Structs------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
#ifdef TRAP_PLATFORM_WINDOWS
		struct VkWin32SurfaceCreateInfoKHR
		{
			VkStructureType sType;
			const void* pNext;
			VkWin32SurfaceCreateFlagsKHR flags;
			HINSTANCE hinstance;
			HWND hwnd;
		};
#elif defined(TRAP_PLATFORM_LINUX)
		struct libdecor_interface
		{
			void (*error)(libdecor*, libdecor_error, const char*);
			void (*reserved0)();
			void (*reserved1)();
			void (*reserved2)();
			void (*reserved3)();
			void (*reserved4)();
			void (*reserved5)();
			void (*reserved6)();
			void (*reserved7)();
			void (*reserved8)();
			void (*reserved9)();
		};

		struct libdecor_frame_interface
		{
			void (*configure)(libdecor_frame*, libdecor_configuration*, void*);
			void (*close)(libdecor_frame*, void*);
			void (*commit)(libdecor_frame*, void*);
			void (*dismiss_popup)(libdecor_frame*, const char*, void*);
			void (*reserved0)();
			void (*reserved1)();
			void (*reserved2)();
			void (*reserved3)();
			void (*reserved4)();
			void (*reserved5)();
			void (*reserved6)();
			void (*reserved7)();
			void (*reserved8)();
			void (*reserved9)();
		};

		struct VkXlibSurfaceCreateInfoKHR
		{
			VkStructureType sType;
			const void* pNext;
			VkXlibSurfaceCreateFlagsKHR flags;
			Display* dpy;
			::Window window;
		};

		struct VkXcbSurfaceCreateInfoKHR
		{
			VkStructureType sType;
			const void* pNext;
			VkXcbSurfaceCreateFlagsKHR flags;
			xcb_connection_t* connection;
			xcb_window_t window;
		};

		struct VkWaylandSurfaceCreateInfoKHR
		{
			VkStructureType sType;
			const void* pNext;
			VkWaylandSurfaceCreateFlagsKHR flags;
			wl_display* display;
			wl_surface* surface;
		};

		struct wl_cursor_image
		{
			u32 width;
			u32 height;
			u32 hotspot_x;
			u32 hotspot_y;
			u32 delay;
		};

		struct wl_cursor
		{
			u32 image_count;
			wl_cursor_image** images;
			char* name;
		};

		enum class TRAPDecorationSideWayland
		{
			MainWindow,
			TopDecoration,
			LeftDecoration,
			RightDecoration,
			BottomDecoration
		};

		struct TRAPDecorationWayland
		{
			wl_surface* surface;
			wl_subsurface* subsurface;
			wp_viewport* viewport;
		};

		struct TRAPOfferWayland
		{
			wl_data_offer* offer;
			bool text_plain_utf8;
			bool text_uri_list;
		};

		struct TRAPScaleWayland
		{
			wl_output* output = nullptr;
			i32 factor = 1;
		};
#endif

		/// @brief Window configuration.
		struct WindowConfig
		{
			u32 Width = 0u;
			u32 Height = 0u;
			std::string Title{};
			bool Resizable = false;
			bool Visible = false;
			bool Decorated = true;
			bool Maximized = false;
			bool Focused = false;
			bool Floating = false;
			bool FocusOnShow = true;
			bool MousePassthrough = false;

			struct wayland
			{
				std::string AppID;
			} Wayland;
		};

		/// @brief Global state.
		struct Data
		{
			bool Initialized = false;

			struct
			{
				WindowConfig Window{};
			} Hints;

			std::forward_list<Scope<InternalWindow>> WindowList{};
			std::forward_list<Scope<InternalCursor>> CursorList{};

			std::vector<Scope<InternalMonitor>> Monitors{};

			struct
			{
				MonitorFunc Monitor = nullptr;
			} Callbacks{};

			struct
			{
				bool Available = false;
				void* Handle = nullptr;
				std::array<std::string, 2u> Extensions{};
				bool KHR_Surface = false;
#if defined(TRAP_PLATFORM_WINDOWS)
				bool KHR_Win32_Surface = false;
#elif defined(TRAP_PLATFORM_LINUX)
				bool KHR_XLib_Surface = false;
				bool KHR_XCB_Surface = false;
				bool KHR_Wayland_Surface = false;
#endif
			} VK{};

			std::string ClipboardString{};
			std::array<std::string, std::to_underlying(Input::Key::Menu) + 1u> KeyNames{};
			std::array<i16, std::to_underlying(Input::Key::Menu) + 1u> ScanCodes{};
			//Where to place the cursor when re-enabled
			f64 RestoreCursorPosX = 0.0, RestoreCursorPosY = 0.0;
			//The window whose disabled cursor mode is active
			InternalWindow* DisabledCursorWindow = nullptr;
			std::array<i32, 2u> EmptyEventPipe{};
			//The window the cursor is captured in
			InternalWindow* CapturedCursorWindow = nullptr;

#ifdef TRAP_PLATFORM_WINDOWS
			std::array<Input::Key, 512u> KeyCodes{};
			HINSTANCE Instance = nullptr;
			HWND HelperWindowHandle = nullptr;
			ATOM HelperWindowClass{};
			ATOM MainWindowClass{};
			HDEVNOTIFY DeviceNotificationHandle = nullptr;
			i32 AcquiredMonitorCount = 0;
			std::vector<RAWINPUT> RawInput{};
			i32 RawInputSize = 0;
			UINT MouseTrailSize = 0u;
			LPVOID MessageFiber;
			LPVOID MainFiber;

			STICKYKEYS UserStickyKeys{sizeof(STICKYKEYS), 0};
			TOGGLEKEYS UserToggleKeys{sizeof(TOGGLEKEYS), 0};
			FILTERKEYS UserFilterKeys{sizeof(FILTERKEYS), 0};

			struct
			{
				HINSTANCE Instance = nullptr;
				PFN_SetProcessDPIAware SetProcessDPIAware = nullptr;
				PFN_ChangeWindowMessageFilterEx ChangeWindowMessageFilterEx = nullptr;
				PFN_EnableNonClientDPIScaling EnableNonClientDPIScaling = nullptr;
				PFN_SetProcessDPIAwarenessContext SetProcessDPIAwarenessContext = nullptr;
				PFN_GetDPIForWindow GetDPIForWindow = nullptr;
				PFN_AdjustWindowRectExForDPI AdjustWindowRectExForDPI = nullptr;
				PFN_GetSystemMetricsForDPI GetSystemMetricsForDPI = nullptr;
				PFN_GetDisplayConfigBufferSizes GetDisplayConfigBufferSizes = nullptr;
				PFN_QueryDisplayConfig QueryDisplayConfig = nullptr;
				PFN_DisplayConfigGetDeviceInfo DisplayConfigGetDeviceInfo = nullptr;
			} User32;

			struct
			{
				HINSTANCE Instance = nullptr;
				PFN_DwmIsCompositionEnabled IsCompositionEnabled = nullptr;
				PFN_DwmFlush Flush = nullptr;
				PFN_DwmEnableBlurBehindWindow EnableBlurBehindWindow = nullptr;
				PFN_DwmSetWindowAttribute SetWindowAttribute = nullptr;
			} DWMAPI_;

			struct
			{
				HINSTANCE Instance = nullptr;
				bool DarkModeAvailable = false;
				PFN_ShouldAppsUseDarkMode ShouldAppsUseDarkMode = nullptr;
				PFN_SetPreferredAppMode SetPreferredAppMode = nullptr;
			} UXTheme;

			struct
			{
				HINSTANCE Instance = nullptr;
				PFN_SetProcessDPIAwareness SetProcessDPIAwareness = nullptr;
				PFN_GetDPIForMonitor GetDPIForMonitor = nullptr;
			} SHCore;
#elif defined(TRAP_PLATFORM_LINUX)
			std::array<Input::Key, 256u> KeyCodes{};

			struct x11
			{
				Display* display = nullptr;
				i32 Screen = 0;
				::Window Root{};

				//System content scale
				f32 ContentScaleX = 0.0f, ContentScaleY = 0.0f;
				//Helper window for IPC
				::Window HelperWindowHandle{};
				//Invisible cursor for hidden cursor mode
				Cursor HiddenCursorHandle{};
				//Context for mapping window XIDs to InternalWindow pointers
				XContext Context{};
				//XIM input method
				XIM IM{};
				//The previous X error handler, to be restored later
				XErrorHandler PrevErrorHandler{};
				//Most recent error code received by X error handler
				i32 ErrorCode = 0;
				//Primary selection string (while the primary selection is owned)
				std::string PrimarySelectionString{};

				//Window manager atoms
				Atom NET_SUPPORTED{};
				Atom NET_SUPPORTING_WM_CHECK{};
				Atom WM_PROTOCOLS{};
				Atom WM_STATE{};
				Atom WM_DELETE_WINDOW{};
				Atom NET_WM_NAME{};
				Atom NET_WM_ICON_NAME{};
				Atom NET_WM_ICON{};
				Atom NET_WM_PID{};
				Atom NET_WM_PING{};
				Atom NET_WM_WINDOW_TYPE{};
				Atom NET_WM_WINDOW_TYPE_NORMAL{};
				Atom NET_WM_STATE{};
				Atom NET_WM_STATE_ABOVE{};
				Atom NET_WM_STATE_FULLSCREEN{};
				Atom NET_WM_STATE_MAXIMIZED_VERT{};
				Atom NET_WM_STATE_MAXIMIZED_HORZ{};
				Atom NET_WM_STATE_DEMANDS_ATTENTION{};
				Atom NET_WM_BYPASS_COMPOSITOR{};
				Atom NET_WM_FULLSCREEN_MONITORS{};
				Atom NET_WM_WINDOW_OPACITY{};
				Atom NET_WM_CM_Sx{};
				Atom NET_WORKAREA{};
				Atom NET_CURRENT_DESKTOP{};
				Atom NET_ACTIVE_WINDOW{};
				Atom MOTIF_WM_HINTS{};

				//Xdnd (drag and drop) atoms
				Atom XDNDAware{};
				Atom XDNDEnter{};
				Atom XDNDPosition{};
				Atom XDNDStatus{};
				Atom XDNDActionCopy{};
				Atom XDNDDrop{};
				Atom XDNDFinished{};
				Atom XDNDSelection{};
				Atom XDNDTypeList{};
				Atom text_uri_list{};

				//Selection (clipboard) atoms
				Atom TARGETS{};
				Atom MULTIPLE{};
				Atom INCR{};
				Atom CLIPBOARD{};
				Atom PRIMARY{};
				Atom CLIPBOARD_MANAGER{};
				Atom SAVE_TARGETS{};
				Atom NULL_{};
				Atom UTF8_STRING{};
				Atom COMPOUND_STRING{};
				Atom ATOM_PAIR{};
				Atom TRAP_SELECTION{};

				struct XSHAPE
				{
					bool Available = false;
					void* Handle = nullptr;
					i32 Major = 0;
					i32 Minor = 0;
					i32 EventBase = 0;
					i32 ErrorBase = 0;
					PFN_XShapeQueryExtension QueryExtension;
					PFN_XShapeCombineRegion CombineRegion;
					PFN_XShapeCombineMask CombineMask;
					PFN_XShapeQueryVersion QueryVersion;
				} XShape{};

				struct RANDR
				{
					bool Available = false;
					void* Handle = nullptr;
					bool MonitorBroken = false;
					i32 EventBase = 0;
					i32 ErrorBase = 0;
					i32 Major = 0;
					i32 Minor = 0;
					PFN_XRRFreeCrtcInfo FreeCrtcInfo{};
					PFN_XRRFreeOutputInfo FreeOutputInfo{};
					PFN_XRRFreeScreenResources FreeScreenResources{};
					PFN_XRRGetCrtcInfo GetCrtcInfo{};
					PFN_XRRGetOutputInfo GetOutputInfo{};
					PFN_XRRGetOutputPrimary GetOutputPrimary{};
					PFN_XRRGetScreenResourcesCurrent GetScreenResourcesCurrent{};
					PFN_XRRQueryExtension QueryExtension{};
					PFN_XRRQueryVersion QueryVersion{};
					PFN_XRRSelectInput SelectInput{};
					PFN_XRRSetCrtcConfig SetCrtcConfig{};
					PFN_XRRUpdateConfiguration UpdateConfiguration{};
				} RandR{};

				struct xkb
				{
					bool Available = false;
					bool Detectable = false;
					i32 MajorOPCode = 0;
					i32 EventBase = 0;
					i32 ErrorBase = 0;
					i32 Major = 0;
					i32 Minor = 0;
					u32 Group = 0;
					PFN_XkbAllocKeyboard AllocKeyboard{};
					PFN_XkbFreeKeyboard FreeKeyboard{};
					PFN_XkbFreeNames FreeNames{};
					PFN_XkbGetMap GetMap{};
					PFN_XkbGetNames GetNames{};
					PFN_XkbGetState GetState{};
					PFN_XkbKeycodeToKeysym KeycodeToKeysym{};
					PFN_XkbQueryExtension QueryExtension{};
					PFN_XkbSelectEventDetails SelectEventDetails{};
					PFN_XkbSetDetectableAutoRepeat SetDetectableAutoRepeat{};
				} XKB{};

				struct SAVER
				{
					i32 Count = 0;
					i32 Timeout = 0;
					i32 Interval = 0;
					i32 Blanking = 0;
					i32 Exposure = 0;
				} Saver{};

				struct xdnd
				{
					i32 Version = 0;
					::Window Source{};
					Atom Format{};
				} XDND{};

				struct XCURSOR
				{
					void* Handle = nullptr;
					PFN_XcursorImageCreate ImageCreate{};
					PFN_XcursorImageDestroy ImageDestroy{};
					PFN_XcursorImageLoadCursor ImageLoadCursor{};
					PFN_XcursorGetTheme GetTheme{};
					PFN_XcursorGetDefaultSize GetDefaultSize{};
					PFN_XcursorLibraryLoadImage LibraryLoadImage{};
				} XCursor{};

				struct XINERAMA
				{
					bool Available = false;
					void* Handle = nullptr;
					i32 Major = 0;
					i32 Minor = 0;
					PFN_XineramaIsActive IsActive{};
					PFN_XineramaQueryExtension QueryExtension{};
					PFN_XineramaQueryScreens QueryScreens{};
				} Xinerama{};

				struct xcb
				{
					void* Handle = nullptr;
					PFN_XGetXCBConnection GetXCBConnection{};
				} XCB{};

				struct xi
				{
					bool Available = false;
					void* Handle = nullptr;
					i32 MajorOPCode = 0;
					i32 EventBase = 0;
					i32 ErrorBase = 0;
					i32 Major = 0;
					i32 Minor = 0;
					PFN_XIQueryVersion QueryVersion{};
					PFN_XISelectEvents SelectEvents{};
				} XI{};

				struct XRENDER
				{
					bool Available = false;
					void* Handle = nullptr;
					i32 Major = 0;
					i32 Minor = 0;
					i32 EventBase = 0;
					i32 ErrorBase = 0;
					PFN_XRenderQueryExtension QueryExtension{};
					PFN_XRenderQueryVersion QueryVersion{};
					PFN_XRenderFindVisualFormat FindVisualFormat{};
				} XRender{};

				struct xlib
				{
					void* Handle = nullptr;
					PFN_XAllocClassHint AllocClassHint{};
					PFN_XAllocSizeHints AllocSizeHints{};
					PFN_XAllocWMHints AllocWMHints{};
					PFN_XChangeProperty ChangeProperty{};
					PFN_XChangeWindowAttributes ChangeWindowAttributes{};
					PFN_XCheckIfEvent CheckIfEvent{};
					PFN_XCheckTypedWindowEvent CheckTypedWindowEvent;
					PFN_XCloseDisplay CloseDisplay{};
					PFN_XCloseIM CloseIM{};
					PFN_XConvertSelection ConvertSelection{};
					PFN_XCreateColormap CreateColormap{};
					PFN_XCreateFontCursor CreateFontCursor{};
					PFN_XCreateIC CreateIC{};
					PFN_XCreateWindow CreateWindow{};
					PFN_XDefineCursor DefineCursor{};
					PFN_XDeleteContext DeleteContext{};
					PFN_XDeleteProperty DeleteProperty{};
					PFN_XDestroyIC DestroyIC{};
					PFN_XDestroyWindow DestroyWindow{};
					PFN_XDisplayKeycodes DisplayKeycodes{};
					PFN_XEventsQueued EventsQueued{};
					PFN_XFilterEvent FilterEvent{};
					PFN_XFindContext FindContext{};
					PFN_XFlush Flush{};
					PFN_XFree Free{};
					PFN_XFreeColormap FreeColormap{};
					PFN_XFreeCursor FreeCursor{};
					PFN_XFreeEventData FreeEventData{};
					PFN_XGetAtomName GetAtomName{};
					PFN_XGetErrorText GetErrorText{};
					PFN_XGetEventData GetEventData{};
					PFN_XGetICValues GetICValues{};
					PFN_XGetIMValues GetIMValues{};
					PFN_XGetInputFocus GetInputFocus{};
					PFN_XGetKeyboardMapping GetKeyboardMapping{};
					PFN_XGetScreenSaver GetScreenSaver{};
					PFN_XGetSelectionOwner GetSelectionOwner{};
					PFN_XGetVisualInfo GetVisualInfo{};
					PFN_XGetWMNormalHints GetWMNormalHints{};
					PFN_XGetWindowAttributes GetWindowAttributes{};
					PFN_XGetWindowProperty GetWindowProperty{};
					PFN_XGrabPointer GrabPointer{};
					PFN_XIconifyWindow IconifyWindow{};
					PFN_XInitThreads InitThreads{};
					PFN_XInternAtom InternAtom{};
					PFN_XLookupString LookupString{};
					PFN_XMapRaised MapRaised{};
					PFN_XMapWindow MapWindow{};
					PFN_XMoveResizeWindow MoveResizeWindow{};
					PFN_XMoveWindow MoveWindow{};
					PFN_XNextEvent NextEvent{};
					PFN_XOpenDisplay OpenDisplay{};
					PFN_XOpenIM OpenIM{};
					PFN_XPeekEvent PeekEvent{};
					PFN_XPending Pending{};
					PFN_XQueryExtension QueryExtension{};
					PFN_XQueryPointer QueryPointer{};
					PFN_XRaiseWindow RaiseWindow{};
					PFN_XRegisterIMInstantiateCallback RegisterIMInstantiateCallback{};
					PFN_XResizeWindow ResizeWindow{};
					PFN_XResourceManagerString ResourceManagerString{};
					PFN_XSaveContext SaveContext{};
					PFN_XSelectInput SelectInput{};
					PFN_XSendEvent SendEvent{};
					PFN_XSetClassHint SetClassHint{};
					PFN_XSetErrorHandler SetErrorHandler{};
					PFN_XSetICFocus SetICFocus{};
					PFN_XSetIMValues SetIMValues{};
					PFN_XSetInputFocus SetInputFocus{};
					PFN_XSetLocaleModifiers SetLocaleModifiers{};
					PFN_XSetScreenSaver SetScreenSaver{};
					PFN_XSetSelectionOwner SetSelectionOwner{};
					PFN_XSetWMHints SetWMHints{};
					PFN_XSetWMNormalHints SetWMNormalHints{};
					PFN_XSetWMProtocols SetWMProtocols{};
					PFN_XSupportsLocale SupportsLocale{};
					PFN_XSync Sync{};
					PFN_XTranslateCoordinates TranslateCoordinates{};
					PFN_XUndefineCursor UndefineCursor{};
					PFN_XUngrabPointer UngrabPointer{};
					PFN_XUnmapWindow UnmapWindow{};
					PFN_XUnsetICFocus UnsetICFocus{};
					PFN_XVisualIDFromVisual VisualIDFromVisual{};
					PFN_XWarpPointer WarpPointer{};
					PFN_XUnregisterIMInstantiateCallback UnregisterIMInstantiateCallback{};
					PFN_Xutf8LookupString UTF8LookupString{};
					PFN_Xutf8SetWMProperties UTF8SetWMProperties{};
					PFN_XCreateRegion CreateRegion{};
					PFN_XDestroyRegion DestroyRegion{};

					bool UTF8 = false;
				} XLIB{};

				struct xrm
				{
					PFN_XrmDestroyDatabase DestroyDatabase{};
					PFN_XrmGetResource GetResource{};
					PFN_XrmGetStringDatabase GetStringDatabase{};
					PFN_XrmInitialize Initialize{};
					PFN_XrmUniqueQuark UniqueQuark{};
				} XRM{};
			} X11{};

			struct wayland
			{
				//Wayland
				wl_display* DisplayWL;
				wl_registry* Registry;
				wl_compositor* Compositor;
				wl_subcompositor* SubCompositor;
				wl_shm* Shm;
				wl_seat* Seat;
				wl_pointer* Pointer;
				wl_keyboard* Keyboard;
				wl_data_device_manager* DataDeviceManager;
				wl_data_device* DataDevice;
				xdg_wm_base* WMBase;
				zxdg_decoration_manager_v1* DecorationManager;
				wp_viewporter* Viewporter;
				zwp_relative_pointer_manager_v1* RelativePointerManager;
				zwp_pointer_constraints_v1* PointerConstraints;
				zwp_idle_inhibit_manager_v1* IdleInhibitManager;
				xdg_activation_v1* ActivationManager;
				wp_content_type_manager_v1* ContentTypeManager;
				wp_fractional_scale_manager_v1* FractionalScaleManager;

				std::vector<TRAPOfferWayland> Offers;

				wl_data_offer* SelectionOffer;
				wl_data_source* SelectionSource;

				wl_data_offer* DragOffer;
				InternalWindow* DragFocus;
				u32 DragSerial;

				std::string Tag;
				const char* TagCStr;

				wl_cursor_theme* CursorTheme;
				wl_cursor_theme* CursorThemeHiDPI;
				wl_surface* CursorSurface;
				std::string CursorPreviousName;
				i32 CursorTimerFD;
				u32 Serial;
				u32 PointerEnterSerial;

				i32 KeyRepeatTimerFD;
				i32 KeyRepeatRate;
				i32 KeyRepeatDelay;
				i32 KeyRepeatScancode;

				struct
				{
					void* Handle;
					xkb_context* Context;
					xkb_keymap* KeyMap;
					xkb_state* State;

					xkb_compose_state* ComposeState;

					xkb_mod_index_t ControlIndex;
					xkb_mod_index_t AltIndex;
					xkb_mod_index_t ShiftIndex;
					xkb_mod_index_t SuperIndex;
					xkb_mod_index_t CapsLockIndex;
					xkb_mod_index_t NumLockIndex;
					u32 Modifiers;
					xkb_layout_index_t Group;

					PFN_xkb_context_new ContextNew;
					PFN_xkb_context_unref ContextUnref;
					PFN_xkb_keymap_new_from_string KeyMapNewFromString;
					PFN_xkb_keymap_unref KeyMapUnref;
					PFN_xkb_keymap_mod_get_index KeyMapModGetIndex;
					PFN_xkb_keymap_key_repeats KeyMapKeyRepeats;
					PFN_xkb_keymap_key_get_syms_by_level KeyMapKeyGetSymsByLevel;
					PFN_xkb_state_new StateNew;
					PFN_xkb_state_unref StateUnref;
					PFN_xkb_state_key_get_syms StateKeyGetSyms;
					PFN_xkb_state_update_mask StateUpdateMask;
					PFN_xkb_state_key_get_layout StateKeyGetLayout;
					PFN_xkb_keymap_layout_get_name KeyMapLayoutGetName;
					PFN_xkb_state_mod_index_is_active StateModIndexIsActive;

					PFN_xkb_compose_table_new_from_locale ComposeTableNewFromLocale;
					PFN_xkb_compose_table_unref ComposeTableUnref;
					PFN_xkb_compose_state_new ComposeStateNew;
					PFN_xkb_compose_state_unref ComposeStateUnref;
					PFN_xkb_compose_state_feed ComposeStateFeed;
					PFN_xkb_compose_state_get_status ComposeStateGetStatus;
					PFN_xkb_compose_state_get_one_sym ComposeStateGetOneSym;

					std::string KeyboardLayoutName{};
				} WaylandXKB;

				InternalWindow* PointerFocus;
				InternalWindow* KeyboardFocus;

				struct
				{
					void* Handle;
					PFN_wl_display_flush DisplayFlush;
					PFN_wl_display_cancel_read DisplayCancelRead;
					PFN_wl_display_dispatch_pending DisplayDispatchPending;
					PFN_wl_display_read_events DisplayReadEvents;
					PFN_wl_display_connect DisplayConnect;
					PFN_wl_display_disconnect DisplayDisconnect;
					PFN_wl_display_roundtrip DisplayRoundtrip;
					PFN_wl_display_get_fd DisplayGetFD;
					PFN_wl_display_prepare_read DisplayPrepareRead;

					PFN_wl_proxy_marshal ProxyMarshal;
					PFN_wl_proxy_add_listener ProxyAddListener;
					PFN_wl_proxy_destroy ProxyDestroy;
					PFN_wl_proxy_marshal_constructor ProxyMarshalConstructor;
					PFN_wl_proxy_marshal_constructor_versioned ProxyMarshalConstructorVersioned;
					PFN_wl_proxy_get_user_data ProxyGetUserData;
					PFN_wl_proxy_set_user_data ProxySetUserData;
					PFN_wl_proxy_get_tag ProxyGetTag;
					PFN_wl_proxy_set_tag ProxySetTag;
					PFN_wl_proxy_get_version ProxyGetVersion;
					PFN_wl_proxy_marshal_flags ProxyMarshalFlags;
				} WaylandClient;

				struct
				{
					void* Handle;
					PFN_wl_cursor_theme_load ThemeLoad;
					PFN_wl_cursor_theme_destroy ThemeDestroy;
					PFN_wl_cursor_theme_get_cursor ThemeGetCursor;
					PFN_wl_cursor_image_get_buffer ImageGetBuffer;
				} WaylandCursor;

				struct
				{
					void* Handle;
					libdecor* Context;
					wl_callback* Callback;
					bool Ready;
					PFN_libdecor_new New;
					PFN_libdecor_unref Unref;
					PFN_libdecor_get_fd GetFD;
					PFN_libdecor_dispatch Dispatch;
					PFN_libdecor_decorate Decorate;
					PFN_libdecor_frame_unref FrameUnref;
					PFN_libdecor_frame_set_app_id FrameSetAppID;
					PFN_libdecor_frame_set_title FrameSetTitle;
					PFN_libdecor_frame_set_minimized FrameSetMinimized;
					PFN_libdecor_frame_set_fullscreen FrameSetFullscreen;
					PFN_libdecor_frame_unset_fullscreen FrameUnsetFullscreen;
					PFN_libdecor_frame_map FrameMap;
					PFN_libdecor_frame_commit FrameCommit;
					PFN_libdecor_frame_set_min_content_size FrameSetMinContentSize;
					PFN_libdecor_frame_set_max_content_size FrameSetMaxContentSize;
					PFN_libdecor_frame_set_maximized FrameSetMaximized;
					PFN_libdecor_frame_unset_maximized FrameUnsetMaximized;
					PFN_libdecor_frame_set_capabilities FrameSetCapabilities;
					PFN_libdecor_frame_unset_capabilities FrameUnsetCapabilities;
					PFN_libdecor_frame_set_visibility FrameSetVisibility;
					PFN_libdecor_frame_get_xdg_toplevel FrameGetXDGTopLevel;
					PFN_libdecor_configuration_get_content_size ConfigurationGetContentSize;
					PFN_libdecor_configuration_get_window_state ConfigurationGetWindowState;
					PFN_libdecor_state_new StateNew;
					PFN_libdecor_state_free StateFree;
				} LibDecor;
			} Wayland{};
#endif
		};
	public:
		/// @brief This describes a single video mode.
		struct InternalVideoMode
		{
			//The width, in screen coordinates, of the video mode.
			i32 Width = 0;
			//The height, in screen coordinates, of the video mode.
			i32 Height = 0;
			//The bit depth of the red channel of the video mode.
			i32 RedBits = 0;
			//The bit depth of the green channel of the video mode.
			i32 GreenBits = 0;
			//The bit depth of the blue channel of the video mode.
			i32 BlueBits = 0;
			//The refresh rate, in Hz, of the video mode.
			f64 RefreshRate = 0;

			[[nodiscard]] constexpr auto operator<=>(const InternalVideoMode& other) const noexcept = default;
		};

		/// @brief Monitor structure.
		struct InternalMonitor
		{
			std::string Name{};

			//The window whose video mode is current on this monitor
			InternalWindow* Window = nullptr;

			std::vector<InternalVideoMode> Modes{};
			InternalVideoMode CurrentMode{};
			std::optional<InternalVideoMode> NativeMode = std::nullopt;

#ifdef TRAP_PLATFORM_WINDOWS
			HMONITOR Handle = nullptr;
			std::wstring AdapterName{};
			std::wstring DisplayName{};
			std::string PublicAdapterName{};
			std::string PublicDisplayName{};
			bool ModesPruned = false;
			bool ModeChanged = false;
#elif defined(TRAP_PLATFORM_LINUX)
			struct x11
			{
				RROutput Output = 0u;
				RRCrtc CRTC = 0u;
				RRMode OldMode = 0u;

				//Index of corresponding Xinerama screen, for EWMH full screen window placement
				i32 Index = 0;
			} X11;

			struct wayland
			{
				wl_output* Output;
				u32 Name;
				i32 X;
				i32 Y;
				f32 ContentScale;
			} Wayland;
#endif
		};

		/// @brief Cursor structure.
		struct InternalCursor
		{
#ifdef TRAP_PLATFORM_WINDOWS
			HCURSOR Handle = nullptr;
#elif defined(TRAP_PLATFORM_LINUX)
			struct x11
			{
				Cursor Handle;
			} X11{};

			struct wayland
			{
				wl_cursor* CursorWL;
				wl_cursor* CursorHiDPI;
				wl_buffer* Buffer;
				i32 Width, Height;
				i32 XHotspot, YHotspot;
				i32 CurrentImage;
			} Wayland{};
#endif
		};

		/// @brief Window structure.
		struct InternalWindow
		{
			//Window settings and state
			bool Resizable = true;
			bool Decorated = true;
			bool Floating = false;
			bool ShouldClose = false;
			bool FocusOnShow = true;
			bool MousePassthrough = false;
			bool DragAndDrop = false;
			bool BorderlessFullscreen = false;
			void* UserPointer = nullptr;
			InternalVideoMode videoMode{};
			InternalMonitor* Monitor = nullptr;
			InternalCursor* Cursor = nullptr;

			i32 MinWidth = -1, MinHeight = -1;
			i32 MaxWidth = -1, MaxHeight = -1;
			i32 Numerator = -1, Denominator = -1;

			CursorMode cursorMode = CursorMode::Normal;
			std::array<TRAP::Input::KeyState, std::to_underlying(TRAP::Input::MouseButton::Eight) + 1u> MouseButtons{};
			std::array<TRAP::Input::KeyState, std::to_underlying(TRAP::Input::Key::Menu) + 1u> Keys{};
			//Virtual cursor position when cursor is disabled
			f64 VirtualCursorPosX = 0.0, VirtualCursorPosY = 0.0;
			bool RawMouseMotion = false;

			struct
			{
				WindowPositionFunc Pos = nullptr;
				WindowSizeFunc Size = nullptr;
				WindowMinimizeFunc Minimize = nullptr;
				WindowMaximizeFunc Maximize = nullptr;
				WindowCloseFunc Close = nullptr;
				WindowFocusFunc Focus = nullptr;
				FrameBufferSizeFunc FBSize = nullptr;
				WindowContentScaleFunc Scale = nullptr;
				MouseButtonFunc MouseButton = nullptr;
				CursorPositionFunc CursorPos = nullptr;
				CursorEnterFunc CursorEnter = nullptr;
				ScrollFunc Scroll = nullptr;
				KeyFunc Key = nullptr;
				CharFunc Character = nullptr;
				DropFunc Drop = nullptr;
			} Callbacks{};

			bool Minimized = false;
			bool Maximized = false;

			//The last received cursor position, regardless of source
			i32 LastCursorPosX = 0, LastCursorPosY = 0;

			i32 Width = 0, Height = 0;

#ifdef TRAP_PLATFORM_WINDOWS
			HWND Handle = nullptr;
			HICON BigIcon = nullptr;
			HICON SmallIcon = nullptr;

			bool CursorTracked = false;
			bool FrameAction = false;

			//The last received high surrogate when decoding pairs of UTF-16 messages
			WCHAR HighSurrogate{};

			ITaskbarList3* TaskbarList = nullptr;
			UINT TaskbarListMsgID = 0u;

			//If user pressed mouse button on window title bar
			UINT NCMouseButton;
			LPARAM NCMousePos;
#elif defined(TRAP_PLATFORM_LINUX)
			//Whether the visual supports framebuffer transparency
			bool Transparent = false;

			struct x11
			{
				//X11
				Colormap colormap = 0u;
				::Window Handle = 0u;
				::Window Parent = 0u;
				XIC IC = nullptr;
				bool OverrideRedirect = false;

				//Cached position and size used to filter out duplicate events
				i32 XPos = 0, YPos = 0;
				//The last position the cursor was warped to by TRAP
				i32 WarpCursorPosX = 0, WarpCursorPosY = 0;

				//The time of the last KeyPress event per keycode, for discarding
				//duplicate key events generated from some keys by ibus
				std::array<Time, 256u> KeyPressTimes{};
			} X11;

			struct wayland
			{
				//Wayland
				bool Visible;
				bool Activated;
				bool Fullscreen;
				bool EmulatedVideoModeActive = false;
				bool Hovered;
				wl_surface* Surface;
				wl_callback* Callback;

				f64 CursorPosX, CursorPosY;
				u32 PointerAxisTime = 0u;

				struct
				{
					i32 Width, Height;
					bool Maximized;
					bool Minimized;
					bool Activated;
					bool Fullscreen;
				} Pending;

				struct
				{
					xdg_surface* Surface;
					xdg_toplevel* TopLevel;
					zxdg_toplevel_decoration_v1* Decoration;
					u32 DecorationMode;
				} XDG;

				struct
				{
					libdecor_frame* Frame;
					i32 Mode;
				} LibDecor;

				std::string Title;
				std::string AppID;

				f32 ContentScale;
				std::vector<TRAPScaleWayland> Scales{};
				InternalMonitor* AssociatedMonitor = nullptr;

				zwp_relative_pointer_v1* RelativePointer;
				zwp_locked_pointer_v1* LockedPointer;
				zwp_confined_pointer_v1* ConfinedPointer;
				zwp_idle_inhibitor_v1* IdleInhibitor;
				xdg_activation_token_v1* ActivationToken;
				wp_content_type_v1* ContentType;
				wp_fractional_scale_v1* FractionalScaling;
				wp_viewport* DrawViewport;

				struct
				{
					wl_buffer* Buffer;
					TRAPDecorationWayland Top, Left, Right, Bottom;
					TRAPDecorationSideWayland Focus;
				} Decorations;
			} Wayland;
#endif
		};
	private:
		//-------------------------------------------------------------------------------------------------------------------//
		//Static Members-----------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		static Data s_Data;
		//-------------------------------------------------------------------------------------------------------------------//
		//Platform Independent Functions-------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//------//
		//Public//
		//------//
	public:
		/// @brief Initializes the windowing API.
		/// Before most WindowingAPI functions can be used, the WindowingAPI must be initialized, and before the engine
		/// terminates the WindowingAPI should be terminated in order to free any resources allocated during or after
		/// initialization.
		///
		/// If this function fails, it calls WindowingAPI::Shutdown before returning.
		/// If it succeeds, WindowingAPI::Shutdown should be called before the engine exits.
		///
		/// Additional calls to this function after successful initialization but before terminiation will return true
		/// immediately.
		///
		/// Errors: Possible errors include Error::Platform_Unavailable and Error::Platform_Error.
		/// @remark @x11 This function will set the LC_CTYPE category of the engine locale according to the current
		///              environment if that category is still "C". This is because the "C" locale breaks Unicode text input.
		/// @threadsafety This function must only be called from the main thread.
		/// @return True if windowing API was successfully initialized, false otherwise.
		[[nodiscard]] static bool Init();
		/// @brief This function destroys all remaining windows and cursor, and frees any other allocated resources.
		/// Once this function is called, you must again call WindowingAPI::Init successfully before you
		/// will be able to use most WindowingAPI functions.
		///
		/// If the WindowingAPI has been successfully initialized, this function should be called before the
		/// engine exits.
		/// If initialization fails, there is no need to call this function, as it is called by WindowingAPI::Init
		/// before it returns failure.
		/// This function has no effect if the WindowingAPI is not initialized.
		///
		/// Possible errors include Error::Platform_Error.
		///
		/// @remark This function may be called before WindowingAPI::Init.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		static void Shutdown();
		/// @brief This function destroys the specified window. On calling this function, no
		/// further callbacks will be called for that window.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to destroy.
		static void DestroyWindow(InternalWindow* window);
		/// @brief Resets all window hints to their default values.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		static void DefaultWindowHints();
		/// @brief This function sets hints for the next call to WindowingAPI::CreateWindow.
		/// The hints, once set, retain their values until changed by a call to this function
		/// or WindowingAPI::DefaultWindowHints, or until the engine is terminated.
		///
		/// Some hints are platform specific. These may be set on any platform but they will only
		/// affect their specific platform. Other platforms will ignore them. Setting these hints
		/// requires no platform specific headers or functions.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.
		/// @threadsafety This function must only be called from the main thread.
		/// @param hint Hint to set.
		/// @param value Value to set for the Hint.
		static void WindowHint(Hint hint, bool value);
		/// @brief This function returns a human-readable name, encoded as UTF-8, of the specified monitor.
		/// The name typically reflects the make and model of the monitor and is not guaranteed to be
		/// unique among the connected monitors.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor InternalMonitor to query name from.
		/// @return Name of the provided monitor.
		[[nodiscard]] static std::string GetMonitorName(const InternalMonitor& monitor);
		/// @brief This function returns the primary monitor. This is usually the monitor
		/// where elements like the task bar or global menu bar are located.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @remark The primary monitor is always first in the vector returned by WindowingAPI::GetMonitors.
		/// @return Primary internal monitor, or nullptr if no monitors were found or if an error occurred.
		[[nodiscard]] static InternalMonitor* GetPrimaryMonitor();
		/// @brief This function returns a vector of handles for all currently connceted
		/// monitors. The primary monitor is always first in the returned vector.
		/// If no monitors were found, this function returns an empty vector.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @return Vector of all connected monitor handles, or an empty vector if no monitors were found or
		/// if an error occurred.
		[[nodiscard]] static std::vector<InternalMonitor*> GetMonitors();
		/// @brief This function returns the current video mode of the specified monitor.
		/// If you have created a full screen window for that monitor, the return
		/// value will depend on whether that window is minimized/iconified.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to get internal video mode from.
		/// @return Current video mode of the specified monitor, or an empty optional if an error occurred.
		[[nodiscard]] static std::optional<InternalVideoMode> GetVideoMode(InternalMonitor& monitor);
		/// @brief This function returns the native video mode of the specified monitor.
		/// The return value depends on the operating systems monitor settings.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to get internal video mode from.
		/// @return Native video mode of the specified monitor, or an empty optional if an error occurred.
		[[nodiscard]] static std::optional<InternalVideoMode> GetNativeVideoMode(InternalMonitor& monitor);
		/// @brief This function returns a vector of all video modes supported by the specified monitor.
		/// The returned vector is sorted in ascending order, first by color bit depth
		/// (the sum of all channel depths) and then by resolution area (the product of
		/// width and height).
		///
		/// Errors: Possible errors include: Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to get all internal video modes from.
		/// @return Vector with all available video modes of the specified monitor, or an empty vector
		/// if an error occurred.
		[[nodiscard]] static std::vector<InternalVideoMode> GetVideoModes(InternalMonitor& monitor);
		/// @brief This function creates a window.
		/// Most of the options controlling how the window should be created are specified with window hints.
		///
		/// The created window may differ from what you requested, as not all parameters and
		/// hints are hard constraints. This includes the size of the window, especially for full screen windows.
		/// To query the actual attributes of the create window, see WindowingAPI::GetWindowHint,
		/// WindowingAPI::GetWindowSize and WindowingAPI::GetFramebufferSize.
		///
		/// To create a full screen window, you need to specify the monitor the window will cover.
		/// If no monitor is specified, the window will be windowed mode. Unless you have a way
		/// for the user to choose a specific monitor, it is recommended that you pick the
		/// primary monitor.
		///
		/// For full screen windows, the specified size becomes the resolution of the window's desired
		/// video mode. As long as a full screen window is not minimized/iconified, the supported
		/// video mode most closely matching the desired video mode is set for the specified monitor.
		///
		/// Once you have create the window, you can switch it between windowed, borderless full screen (windowed)
		/// and full screen mode with WindowingAPI::SetWindowMonitor and WindowingAPI::SetWindowMonitorBorderless.
		///
		/// By default, newly created windows use the placement recommended by the window system.
		/// To create the window at a specific position, make it initially invisible using the Hint::Visibile
		/// window hint, set its position and then show it.
		///
		/// As long as at least one full screen window is not minimized/iconified, the screensaver is prohibited
		/// from starting.
		///
		/// Window systems put limits on window sizes. Very large or very small window dimensions
		/// may be overriden by the window system on creation. Check the actual size after creation.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum, Error::Invalid_Value,
		///         Error::Format_Unavailable and Error::Platform_Error.
		/// @remark @win32: If the executable has an icon resource named 'TRAP_ICON', it will be set as the initial
		///                 icon for the window. If no such icon is present, the IDI_APPLICATION icon will be used
		///                 instead. To set a different icon, see WindowingAPI::SetWindowIcon.
		/// @remark @x11 Some window managers will not respect the placement of initially hidden windows.
		///              Due to the asynchronous nature of X11, it may take a moment for a window to reach its
		///              requested state. This means you may not be able to query the final size, position or
		///              other attributes directly after window creation.
		///              The class part of the WM_CLASS window property will by default be set to the window
		///              title passed to this function. The instance part will use the contents of the
		///              RESOURCE_NAME environment variable, if present and not empty, or fall back to the
		///              window title.
		/// @remark @wayland Compositors should implement the xdg-decoration protocol to decorate the window properly.
		///                  If this protocol isn't supported, or if the compositor prefers client-side decorations, a
		///                  fallback decoration will be drawn using libdecor. If this is also not available then we fallback
		///                  to a very simple frame using the wp_viewporter protocol.
		///                  A compositor can still emit close, maximize or fullscreen events, using for instance a keybind
		///                  mechanism. If neither of these protocols nor libdecor is supported, the window won't be docorated.
		///                  A full screen window will not attempt to change the mode, no matter what the requested size or refresh rate.
		///                  Screensaver inhibition requires the idle-inhibit protocol to be implemented in the user's compositor.
		/// @threadsafety This function must only be called from the main thread.
		/// @param width Desired width for the new window. Must be greater than zero.
		/// @param height Desired height for the new window. Must be greater than zero.
		/// @param title UTF-8 encoded title for the new window.
		/// @param monitor Optional monitor to use for the new window.
		/// @return On success a new internal window, or nullptr if an error occurred.
		[[nodiscard]] static InternalWindow* CreateWindow(u32 width, u32 height, std::string title,
		                                                  InternalMonitor* monitor);
		/// @brief This function sets the value of the close flag of the specified window. This can be used
		/// to override the user's attempt to close the window, or to signal that it should be closed.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafe
		/// @param window Internal window whose flag to change.
		/// @param value Whether to close the window or not.
		static void SetWindowShouldClose(InternalWindow& window, bool value);
		/// @brief This function sets the window title, encoded as UTF-8, of the specified window.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window whose title to change.
		/// @param title New UTF-8 encoded title for the window.
		static void SetWindowTitle(InternalWindow& window, const std::string& title);
		/// @brief This function retrieves the content scale for the specified monitor.
		/// The content scale is the ratio between the current DPI and the platform's default DPI.
		/// This is especially important for text and any UI elements. If the pixel dimensions
		/// of your UI scaled by this look appropriate on your machine then it should appear
		/// at a reasonable size on other machines regardless of their DPI and scaling settings.
		/// This relies on the system DPI and scaling settings being somewhat correct.
		///
		/// The content scale may depend on both the monitor resolution and pixel density and
		/// on user settings. It may be very different from the raw DPI calculated from the
		/// physical size and current resolution.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor.
		/// @param xScale Output variable for the X scale of the provided monitor.
		/// @param yScale Output variable for the Y scale of the provided monitor.
		static void GetMonitorContentScale(const InternalMonitor& monitor, f32& xScale, f32& yScale);
		/// @brief This function destroys a cursor previously created with CreateCursor. Any remaining cursors
		/// will be destroyed by WindowingAPI::Shutdown.
		///
		/// If the specified cursor is current for any window, that window will be reverted to the default
		/// cursor. This does not affect the cursor mode.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		/// @param cursor Internal cursor to be destroyed.
		static void DestroyCursor(InternalCursor* cursor);
		/// @brief Creates a new custom cursor image that can be set for a window with SetCursor. The cursor can
		/// be destroyed with DestroyCursor. Any remaining cursors are destroyed by WindowingAPI::Shutdown.
		///
		/// The cursor hotspot is specified in pixels, relative to the upper-left corner of the cursor image.
		/// Like all other coordinate system in the WindowingAPI, the X-axis points to the right and the
		/// Y-axis points down.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param image Non HDR RGB 24BPP or RGBA 32 BPP image.
		/// @param xHotspot Center x coordinate of the image.
		/// @param yHotspot Center y coordinate of the image.
		/// @return On success new internal cursor, nullptr otherwise.
		[[nodiscard]] static InternalCursor* CreateCursor(const Image& image, u32 xHotspot, u32 yHotspot);
		/// @brief Creates a cursor with a standard shape.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param type Cursor type to get.
		/// @return On success new internal cursor, nullptr otherwise.
		[[nodiscard]] static InternalCursor* CreateStandardCursor(const CursorType& type);
		/// @brief This function sets the cursor image to be used when the cursor is over the content are of the
		/// specified window. The set cursor will only be visible when the CursorMode of the window is
		/// CursorMode::Normal.
		///
		/// On some platforms, the set cursor may not be visible unless the window also has input focus.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the cursor for.
		/// @param cursor Internal cursor to set, or nullptr to switch back to the default arrow cursor.
		static void SetCursor(InternalWindow& window, InternalCursor* cursor);
		/// @brief This function sets the icon of the specified window. If no image is specified, the window
		/// reverts to its default icon.
		///
		/// The image mustbe RGB 24BPP or RGBA 32BPP.
		///
		/// The desired image size varies depending on platform and system settings.
		/// Good sizes include 16x16, 32x32 and 48x48.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland There is no existing protocol to change an icon, the window will thus inherit
		///                  the one defined in the application's desktop file. This function will emit Error::Feature_Unavailable.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window whose icon to set.
		/// @param image Image to be set as window icon or nullptr to revert back to the default icon.
		static void SetWindowIcon(InternalWindow& window, const Image* image);
		/// @brief This function sets the position, in screen coordinates, of the upper-left corner of the
		/// content area of the specifed windowed mode window. If the window is a full screen window,
		/// this function does nothing.
		///
		/// DO NOT USE THIS FUNCTION to move an already visible window unless you have very
		/// good reasons for doing so, as it will confuse and annoy the user.
		///
		/// The window manager may put limits on what positions are allowed. The WindowingAPI
		/// cannot and should not override these limits.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland There is no way for an application to set the global position of its window.
		///                  This function will emit Error::Feature_Unavailable.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the position for.
		/// @param xPos X position to be set.
		/// @param yPos Y position to be set.
		static void SetWindowPos(const InternalWindow& window, i32 xPos, i32 yPos);
		/// @brief This function retrieves the position, in screen coordinates, of the upper-left corner of the
		/// content area of the specified window.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland There is now way for an application to retrieve the global position of its windows.
		///                  This function will emit Error::Feature_Unavailable.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the current position from.
		/// @param xPos Output variable for the current x position of the internal window.
		/// @param yPos Output variable for the current y position of the internal window.
		static void GetWindowPos(const InternalWindow& window, i32& xPos, i32& yPos);
		/// @brief This function sets the size, in screen coordinates, of the content area of the specified window.
		///
		/// For full screen windows, this function updates the resolution of its desired video mode
		/// and switches to the video mode closest to it.
		///
		/// If you wish to update the refresh rate of the desired video mode in addition
		/// to its resolution, see WindowingAPI::SetWindowMonitor.
		///
		/// The window manager may put limits on what sizes are allowed. The WindowingAPI
		/// cannot and should not override these limits.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland A full screen window will not attempt to change the mode,
		///                  no matter what the requested size.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the size for.
		/// @param width New width for the internal window.
		/// @param height New height for the internal window.
		static void SetWindowSize(InternalWindow& window, i32 width, i32 height);
		/// @brief This function retrieves the size, in screen coordinates, of the content area of
		/// the specified window. If you wish to retrieve the size of the framebuffer of the window
		/// in pixels, see WindowingAPI::GetFrameBufferSize.
		///
		/// If an error occurs, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the size from.
		/// @param width Output variable for the internal windows current width.
		/// @param height Output variable for the internal windows current height.
		static void GetWindowSize(const InternalWindow& window, i32& width, i32& height);
		/// @brief This function retrieves the size, in pixels, of the framebuffer of the specified window.
		/// If you wish to retrieve the size of the window in screen coordinates, see
		/// WindowingAPI::GetWindowSize.
		///
		/// If an error occcurs, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the framebuffer size from.
		/// @param width Output variable for the internal windows current framebuffer width.
		/// @param height Output variable for the internal windows current framebuffer height.
		static void GetFrameBufferSize(const InternalWindow& window, i32& width, i32& height);
		/// @brief This function sets the opacity of the window, including any decorations.
		///
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque.
		///
		/// The initial opacity value for newly created windows is 1.0f.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland There is no way to set an opacity factor for a window.
		///                   This function will emit Error::Feature_Unavailable.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set opacity for.
		/// @param opacity Opacity ranging from 0.0f-1.0f.
		static void SetWindowOpacity(const InternalWindow& window, f32 opacity);
		/// @brief This function returns the opacity of the window, including any decorations.
		///
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque. If the system does not
		/// support whole window transparency, this function always returns one.
		///
		/// The initial opacity value for newly created windows is 1.0f.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the opacity from.
		/// @return Opacity of the given internal window on success, empty optional otherwise.
		[[nodiscard]] static std::optional<f32> GetWindowOpacity(const InternalWindow& window);
		/// @brief This function retrieves the content scale for the specified window.
		/// The content scale is the reatio between the current DPI and the platform's
		/// default DPI. This is especially important for text and any UI elements.
		/// If the pixel dimensions of your UI scaled by this look appropriate
		/// on your machine then it should appear at a reasonable size on other
		/// machines regardless of their DPI and scaling settings. This relies
		/// on the system DPI and scaling settings being somewhat correct.
		///
		/// On systems where each monitor can have its own content scale, the window
		/// content scale will depend on which monitor the system considers the window
		/// to be on.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the content scale from.
		/// @param xScale Output variable for the internal windows content scale x.
		/// @param yScale Output variable for the internal windows content scale y.
		static void GetWindowContentScale(const InternalWindow& window, f32& xScale, f32& yScale);
		/// @brief This function sets the value of a hint of the specified window.
		///
		/// Some of these hints are ignored for full screen windows. The new value
		/// will take effect if the window is later made windowed.
		///
		/// Some of these hints are ignored for windowed mode windows. The new value
		/// will take effect if the window is later made full screen.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invaild_Enum,
		///         Error::Invalid_Value and Error::Platform_Error.
		/// @remark: Calling WindowingAPI::GetWindowHint will always return the latest value,
		///          even if that value is ignored by the current mode of the window.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the hint for.
		/// @param hint Hint to be set.
		/// @param value Value for the hint.
		static void SetWindowHint(InternalWindow& window, Hint hint, bool value);
		/// @brief This function returns the value of a hint of the specified window.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum
		/// and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the hint from.
		/// @param hint Hint to query.
		/// @return True if hint is enabled, false otherwise.
		[[nodiscard]] static bool GetWindowHint(const InternalWindow& window, Hint hint);
		/// @brief This function sets the monitor that the window uses for full screen mode or,
		/// if the monitor is nullptr, makes it windowed mode.
		///
		/// When setting a monitor, this function updates the width, height and
		/// refresh rate of the desired video mode and switches to the video mode
		/// closest to it. The window position is ignored when setting a monitor.
		///
		/// When the monitor is nullptr, the position, width and height are used to place the
		/// window content area. The refresh rate is ignored when no monitor is specified.
		///
		/// If you only wish to update the resolution of a full screen window
		/// or the size of a windowed mode window, see WindowingAPI::SetWindowSize.
		///
		/// When a window transitions from full screen to windowed mode, this function
		/// restores any previous window settings such as whether it is decorated,
		/// floating, resizable, has size limits, etc.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland The desired window position is ignored, as there is no way
		///                  for an application to set this property.
		///                  Setting the window to full screen will not attempt to change the mode,
		///                  no matter what the requested size or refresh rate.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the monitor for.
		/// @param monitor Internal monitor to use or nullptr.
		/// @param xPos New x position for the window.
		/// @param yPos New y position for the window.
		/// @param width New width for the window.
		/// @param height New height for the window.
		/// @param refreshRate New refresh rate for the window.
		static void SetWindowMonitor(InternalWindow& window,
		                             InternalMonitor* monitor,
		                             i32 xPos,
		                             i32 yPos,
		                             i32 width,
		                             i32 height,
		                             f64 refreshRate);
		/// @brief This function sets the monitor that the window uses for borderless full screen mode.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the monitor for.
		/// @param monitor Internal monitor to use.
		static void SetWindowMonitorBorderless(InternalWindow& window, InternalMonitor& monitor);
		/// @brief This function sets the user-define dpointer of the specified window.
		/// The current value is retained until the window is destroyed.
		/// The initial value is nullptr.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafe
		/// @param window Internal window to set the user pointer for.
		/// @param ptr Data to be stored with the window.
		static void SetWindowUserPointer(InternalWindow& window, void* ptr);
		/// @brief This function returns the current value of the user-defined pointer
		/// of the specified window. The initial value is nullptr.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafe
		/// @param window Internal window to get the user pointer from.
		/// @return Data stored with the window.
		[[nodiscard]] static void* GetWindowUserPointer(const InternalWindow& window);
		/// @brief This function sets the monitor configuration callback, or removes
		/// the currently set callback. This is called when a monitor is
		/// connected to or disconnected from the system.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param callback Function to call when a monitor event occurs, or nullptr to remove the currently set callback.
		static void SetMonitorCallback(MonitorFunc callback);
		/// @brief This function sets the position callback of the specified window, which is called
		/// when the window is moved. The callback is provided with the position, in screen
		/// coordinates, of the upper-left corner of the content area of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @remark @wayland This callback will never by called, as there is no way for an application
		///                  to know its global position.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a position event occurs, or nullptr to remove the currently set callback.
		static void SetWindowPosCallback(InternalWindow& window, WindowPositionFunc callback);
		/// @brief This function sets the size callback of the specified window, which is
		/// called when the window is resized. The callback is provided with the
		/// size, in screen coordinates, of the content area of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a size event occurs, or nullptr to remove the currently set callback.
		static void SetWindowSizeCallback(InternalWindow& window, WindowSizeFunc callback);
		/// @brief This function sets the minimization/iconification callback of the specified window, which
		/// is called when the window is minimized/iconified or restored.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a minimize event occurs, or nullptr to remove the currently set callback.
		static void SetWindowMinimizeCallback(InternalWindow& window, WindowMinimizeFunc callback);
		/// @brief This function sets the maximization callback of the specified window, which
		/// is called when the window is maximized or restored.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a maximize event occurs, or nullptr to remove the currently set callback.
		static void SetWindowMaximizeCallback(InternalWindow& window, WindowMaximizeFunc callback);
		/// @brief This function sets the close callback of the specified window, which is called
		/// when the user attempts to close the window, for example by clicking the close widget
		/// in the title bar.
		///
		/// The close flags is set before this callback is called, but you can modify it at
		/// any time with WindowingAPI::SetWindowShouldClose.
		///
		/// The close callback is not triggered by WindowingAPI::DestroyWindow.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a close event occurs, or nullptr to remove the currently set callback.
		static void SetWindowCloseCallback(InternalWindow& window, WindowCloseFunc callback);
		/// @brief This function sets the focus callback of the specified window, which is called
		/// when the window gains or loses input focus.
		///
		/// After the focus callback is called for a window that lost input focus, synthetic
		/// key and mouse button release events will be generated for all such that had been
		/// pressed. For more information, see WindowingAPI::SetKeyCallback and
		/// WindowingAPI::SetMouseButtonCallback.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a focus event occurs, or nullptr to remove the currently set callback.
		static void SetWindowFocusCallback(InternalWindow& window, WindowFocusFunc callback);
		/// @brief This function sets the framebuffer resize callback of the specified window, which is
		/// called when the framebuffer of the specified window is resized.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a framebuffer size event occurs, or nullptr to remove the currently set callback.
		static void SetFrameBufferSizeCallback(InternalWindow& window, FrameBufferSizeFunc callback);
		/// @brief This function sets the window content scale callback of the specified window, which is
		/// called when the content scale of the specified window changes.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a content scale event occurs, or nullptr to remove the currently set callback.
		static void SetContentScaleCallback(InternalWindow& window, WindowContentScaleFunc callback);
		/// @brief This function sets the key callback of the specified window, which is called when
		/// a key is pressed, repeated or released.
		///
		/// The key functions deal with physical keys, with layout independent key tokens named
		/// after their values in the standard US keyboard layout. If oyu want to input text, use
		/// the character callback instead.
		///
		/// When a window loses input focus, it will generate synthetic key release events for
		/// all pressed named keys. You can tell these events from user-generated events by the fact
		/// that the synthetic ones are generated after the focus loss event has been processed,
		/// i.e. after the window focus callback has been called.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a key event occurs, or nullptr to remove the currently set callback.
		static void SetKeyCallback(InternalWindow& window, KeyFunc callback);
		/// @brief This function sets the character callback of the specified window, which is called
		/// when a Unicode character is input.
		///
		/// The character callback is intended for Unicode text input. As it deals with characters,
		/// it is keyboard layout dependent. whereas the key callback is not. Characters
		/// do not map 1:1 to physical keys, as a key may produce zero, one or more character.
		/// If you want to know whether a specific physical key was pressed or released, see
		/// the key callback instead.
		///
		/// The character callback behaves as system text input normally does and will not be called
		/// if modifier keys are held down that would prevent normal text input on that platform,
		/// for example Alt key on Windows.
		///
		/// Erros: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a char event occurs, or nullptr to remove the currently set callback.
		static void SetCharCallback(InternalWindow& window, CharFunc callback);
		/// @brief This function sets the mouse button callback of the specified window, which is
		/// called when amouse button is pressed or released.
		///
		/// When a window loses input focus, it will generate synthetic mouse button release
		/// events for all pressed mouse buttons. You cann tell these events from user-generated
		/// events by the fact that the synthetic ones are generated after the focus loss event
		/// has been processed, i.e. after the window focus callback has been called.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a mouse button event occurs, or nullptr to remove the currently set callback.
		static void SetMouseButtonCallback(InternalWindow& window, MouseButtonFunc callback);
		/// @brief This function sets the cursor position callback of the specified window, which is
		/// called when the cursor is moved. The callback is provided with the position,
		/// in screen coordinates, relative to the upper-left corner of the content area
		/// of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a cursor position event occurs, or nullptr to remove the currently set callback.
		static void SetCursorPosCallback(InternalWindow& window, CursorPositionFunc callback);
		/// @brief This function sets the cursor boundary crossing callback of the specified window, which
		/// is called when the cursor enters or leaves the content area of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This fucntion must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a cursor enter event occurs, or nullptr to remove the currently set callback.
		static void SetCursorEnterCallback(InternalWindow& window, CursorEnterFunc callback);
		/// @brief This function sets the scroll callback of the specified window, which is called when a
		/// scrolling device is used, such as a mouse wheel or scrolling area of a touchpad.
		///
		/// The scroll callback receives all scrolling input, like that from a mouse wheel or a touchpad
		/// scrolling area.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a scroll event occurs, or nullptr to remove the currently set callback.
		static void SetScrollCallback(InternalWindow& window, ScrollFunc callback);
		/// @brief This function sets the path drop callback of the specified window, which is
		/// called when one or more dragged paths are dropped on the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @remark @wayland File drop is currently unimplemented.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the callback for.
		/// @param callback Function to call when a drop even occurs, or nullptr to remove the currently set callback.
		static void SetDropCallback(InternalWindow& window, DropFunc callback);
		/// @brief Gets the monitor configuration callback.
		/// @return Function that gets called when a monitor event occurs.
		[[nodiscard]] static MonitorFunc GetMonitorCallback();
		/// @brief Gets the position callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a position event occurs.
		[[nodiscard]] static constexpr WindowPositionFunc GetWindowPosCallback(const InternalWindow& window);
		/// @brief Gets the size callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a size event occurs.
		[[nodiscard]] static constexpr WindowSizeFunc GetWindowSizeCallback(const InternalWindow& window);
		/// @brief Gets the close callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a close event occurs.
		[[nodiscard]] static constexpr WindowCloseFunc GetWindowCloseCallback(const InternalWindow& window);
		/// @brief Gets the focus callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a focus event occurs.
		[[nodiscard]] static constexpr WindowFocusFunc GetWindowFocusCallback(const InternalWindow& window);
		/// @brief Gets the framebuffer resize callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a framebuffer size event occurs.
		[[nodiscard]] static constexpr FrameBufferSizeFunc GetFrameBufferSizeCallback(const InternalWindow& window);
		/// @brief Gets the window content scale callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a content scale event occurs.
		[[nodiscard]] static constexpr WindowContentScaleFunc GetWindowContentScaleCallback(const InternalWindow& window);
		/// @brief Gets the key callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a key event occurs.
		[[nodiscard]] static constexpr KeyFunc GetKeyCallback(const InternalWindow& window);
		/// @brief Gets the Unicode character callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a char event occurs.
		[[nodiscard]] static constexpr CharFunc GetCharCallback(const InternalWindow& window);
		/// @brief Gets the mouse button callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a mouse button event occurs.
		[[nodiscard]] static constexpr MouseButtonFunc GetMouseButtonCallback(const InternalWindow& window);
		/// @brief Gets the cursor position callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a cursor position event occurs.
		[[nodiscard]] static constexpr CursorPositionFunc GetCursorPosCallback(const InternalWindow& window);
		/// @brief Gets the cursor enter callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a cursor enter event occurs.
		[[nodiscard]] static constexpr CursorEnterFunc GetCursorEnterCallback(const InternalWindow& window);
		/// @brief Gets the scroll callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a scroll event occurs.
		[[nodiscard]] static constexpr ScrollFunc GetScrollCallback(const InternalWindow& window);
		/// @brief Gets the path drop callback for the specified window.
		/// @param window Internal window from which to query the callback from.
		/// @return Function that gets called when a drop event occurs.
		[[nodiscard]] static constexpr DropFunc GetDropCallback(const InternalWindow& window);
		/// @brief This function processes only those events that are already in the event queue and then
		/// returns immediately. Processing events will cause the window and input callbacks
		/// associated with those events to be called.
		///
		/// On some platforms, a window move, resize or menu operation will cause event processing
		/// to block. This is due to how event processing is designed on those platforms.
		///
		/// Do not assume that callbacks you set will only be called in response to event
		/// processing functions like this one. While it is necessary to poll for events, window
		/// systems that require the WindowingAPI to register callbacks of its own can pass evnts
		/// to the WindowingAPI in response to many window system function calls. The WindowingAPI
		/// will pass those events on to the application callbacks before returning.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		static void PollEvents();
		/// @brief This function puts the calling thread to sleep until at least one event is available
		/// in the event queue, or until the specified timeout is reached.
		/// Once one or more events are available, it behaves exactly like
		/// PollEvents, i.e. the events in the queue are processed and the function then returns
		/// immediately. Processing events will cause the window and input callbacks
		/// associated with those events to be called.
		///
		/// Since not all events are associated with callbacks, this function may return
		/// without a callback having been called even if you are monitoring all callbacks.
		///
		/// On some platforms, a window move, resize or menu operation will cause event
		/// processing to block. This is due to how event processing is designed on
		/// those platforms.
		///
		/// Do not assume that callbacks you set will only be called in response to event
		/// processing functions like this one. While it is neccessary to poll for events,
		/// window systems that require the WindowingAPI to register callbacks of its own
		/// can pass events to the WindowingAPI in response to many window system function
		/// calls. The WindowingAPI will pass those events on to the application callbacks
		/// before returning.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value
		///         and Error::Platform_Error.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		/// @param timeout Optional: Maximum amount of time, in seconds, to wait.
		static void WaitEvents(f64 timeout = 0.0);
		/// @brief This function posts an empty event from the current thread to the event queue,
		/// causing WaitEvents to return.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafe
		static void PostEmptyEvent();
		/// @brief This function sets a cursor mode for the specified window.
		///
		/// The mode must be one of the following CursorModes:
		/// - Normal makes the cursor visible and behaving normally.
		/// - Hidden makes the cursor invisible when it is over the content area of the
		///   window but does not restrict the cursor from leaving.
		/// - Disabled hides and grabs the cursor, providing virtual and unlimited cursor movement.
		/// - Captured makes the cursor visible and confines it to the content area of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the cursor mode for.
		/// @param mode Cursor mode to be set.
		static void SetCursorMode(InternalWindow& window, CursorMode mode);
		/// @brief Retrieves the cursor mode from the specified window.
		/// @param window Internal window to query the cursor mode from.
		/// @return Cursor mode used by the internal window.
		[[nodiscard]] static CursorMode GetCursorMode(const InternalWindow& window);
		/// @brief This function returns whether raw mouse motion is supported on the current system.
		/// This status does not change after the WindowingAPI has been initialized so you
		/// only need to check this once. If you attemp to enable raw motion on a system
		/// that does not support it, Error::Platform_Error will be emitted.
		///
		/// Raw mouse motion is closer to the actual motion of the mouse across a surface.
		/// It is not affected by the scaling and acceleration applied to the motion of
		/// the desktop cursor. That processing is suitable for a cursor while raw motion
		/// is better for controlling for example a 3D camera. Because of this, raw mouse
		/// motion is only provided when the cursor is disabled.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafety This function must only be called from the main thread.
		/// @return True if raw mouse input is supported, false otherwise.
		[[nodiscard]] static bool RawMouseMotionSupported();
		/// @brief Sets the raw mouse motion mode for the specified window.
		///
		/// If raw (unscaled and unaccelerated) mouse motion is not supported, attempting to set this
		/// will emit Error::Feature_Unavailable.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum,
		///         Error::Platform_Error and Error::Feature_Unavailable.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set raw mouse input for.
		/// @param enabled Whether to enable or disable raw mouse input.
		static void SetRawMouseMotionMode(InternalWindow& window, bool enabled);
		/// @brief Retrieves the raw mouse motion mode for the specified window.
		/// @param window Internal window to query.
		/// @return True if raw mouse motion mode is enabled, false otherwise.
		[[nodiscard]] static bool GetRawMouseMotionMode(const InternalWindow& window);
		/// @brief Sets the dock or taskbar progress indicator of the specified window.
		///
		/// Linux: This only works on KDE & Unity environments.
		///        A .desktop file must exist for the application with the same name as given to TRAP::Application.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value,
		///         Error::Invalid_Enum, Error::Platform_Error, Error::Feature_Unimplemented
		//          and Error::Feature_Unavailable.
		/// @remark @x11 @wayland Requires a valid application desktop file with the same name
		///                       as the compiled executable. Due to limitations in the Unity Launcher API
		///                       ProgressState::Indeterminate, ProgressState::Error and ProgressState::Paused
		///                       have the same behaviour as ProgressState::Normal.
		///                       The Unity Launcher API is only known to be supported on Unity and
		///                       KDE desktop environments; on other desktop environments this
		///                       function may do nothing.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set progress for.
		/// @param state State of progress to be displayed in the dock or taskbar.
		/// @param progress The amount of completed progress to set.
		/// Valid range is [0.0-1.0].
		/// This parameter is ignored if state is ProgressState::Disabled
		static void SetWindowProgressIndicator(const InternalWindow& window, ProgressState state, f64 progress);
		/// @brief This function returns the name of the specified printable key, encoded as UTF-8.
		/// This is typically the character that key would produce without any modifier
		/// keys, intended for displaying key bindings to the user. For dead keys, it is
		/// typically the diacritic it would add to a character.
		///
		/// DO NOT USE THIS FUNCTION for text input. You will break text input for many
		/// languages even if it happens to work for yours.
		///
		/// If the key is Input::Key::Unknown, the scancode is used to identify the key,
		/// otherwise the scancode is ignored. If you specify a non-printable key, or
		/// Input::Key::Unknown and a scancode that maps to a non-printable key, this
		/// function return nullopt but does not emit an error.
		///
		/// This behaviour allows you to always pass in the arguments in the key callback
		/// without modification.
		///
		/// The printable keys are:
		/// Input::Key::Apostrophe
		/// Input::Key::Comma
		/// Input::Key::Minus
		/// Input::Key::Period
		/// Input::Key::Slash
		/// Input::Key::Semicolon
		/// Input::Key::Equal
		/// Input::Key::Left_Bracket
		/// Input::Key::Right_Bracket
		/// Input::Key::Backslash
		/// Input::Key::World_1
		/// Input::Key::World_2
		/// Input::Key::Zero to Input::Key::Nine
		/// Input::Key::A to Input::Key::Z
		/// Input::Key::KP_0 to Input::Key::KP_9
		/// Input::Key::KP_Decimal
		/// Input::Key::KP_Divide
		/// Input::Key::KP_Multiply
		/// Input::Key::KP_Subtract
		/// Input::Key::KP_Add
		/// Input::Key::KP_Equal
		///
		/// Names for printable keys depend on keyboard layout, while names for non-printable keys
		/// are the same across layouts but depend on the application language and should be localized
		/// along with other user interface text.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark: The contents of the returned string may change when a keyboard layout change event is received.
		/// Pointer lifetime: The returned string is allocated and freed by the WindowingAPI.
		///                   You should not free it yourself.
		///                   It is valid until the library is terminated.
		/// @threadsafety This function must only be called from the main thread.
		/// @param key Key to get layout-specific name from.
		/// @param scanCode Optional scan code to get layout-specific name from.
		/// @return UTF-8 encoded, layout-specific name of the given key or nullptr.
		[[nodiscard]] static std::optional<std::string> GetKeyName(Input::Key key, i32 scanCode);
		/// @brief This function returns the last state reported for the specified key to the
		/// specified window. The returned state is KeyState::Pressed or KeyState::Released.
		///
		/// The key functions deal with physical keys, with key token named after their use
		/// on the standard US keyboard layout. If you want to input text, use the Unicode
		/// character callback instead.
		///
		/// DO NOT USE THIS FUNCTION to implement text input.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to query.
		/// @param key Key to get last reported state from.
		/// @return Last reported state of the specified key.
		[[nodiscard]] static TRAP::Input::KeyState GetKey(const InternalWindow& window, Input::Key key);
		/// @brief This function returns the last state reported for the specified mouse button
		/// to the specified window. The returned state is pressed (true) or released (false).
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to query.
		/// @param button Mouse button to get last reported state from.
		/// @return Last reported state of the specified mouse button.
		[[nodiscard]] static TRAP::Input::KeyState GetMouseButton(const InternalWindow& window, Input::MouseButton button);
		/// @brief This function sets the position, in screen coordinates, of the cursor relative to the
		/// upper-left corner of the content area of the specified window. The window must have
		/// input focus. If the window does not have input focus when this function is called,
		/// it fails silently.
		///
		/// DO NOT USE THIS FUNCTION to implement things like camera controls. The WindowingAPI already
		/// provides the CursorMode::Disabled that hides the cursor, transparently re-centers it and
		/// provides unconstrained cursor motion. See SetCursorMode for more information.
		///
		/// If the cursor mode is CursorMode::Disabled then the cursor position is unconstrained and
		/// limited only by the minimum and maximum values of a f64.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland This function will only work when the cursor mode is CursorMode::Disabled, otherwise
		///                  it will emit Error::Feature_Unavailable.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the cursor position for.
		/// @param xPos New x position, relative to the left edge of the content area, for the cursor.
		/// @param yPos New y position, relative to the top edge of the content area, for the cursor.
		static void SetCursorPos(InternalWindow& window, f64 xPos, f64 yPos);
		/// @brief This function returns the position of the cursor, in screen coordinates, relative
		/// to the upper-left corner of the content area of the specified window.
		///
		/// If the cursor is disabled (with CursorMode::Disabled) then the cursor position is unbounded and limited
		/// only by the minimum and maximum values of a f64.
		///
		/// The coordinate can be converted to their integer equivalents with the TRAP::Math::Floor function.
		/// Casting directly to an integer type works for positive coordinates, but fails for negative ones.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window from which to get the current cursor position.
		/// @param xPos
		/// Output variable for the current x position, relative to the left edge of the content area, of the cursor.
		/// @param yPos
		/// Output variable for the current y position, relative to the top edge of the content area, of the cursor.
		static void GetCursorPos(const InternalWindow& window, f64& xPos, f64& yPos);
		/// @brief Returns the position of the monitor's viewport on the virtual screen.
		/// This function returns the position, in screen coordinates, of the upper-left corner
		/// of the specified monitor.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to query.
		/// @param xPos Output variable for the x position of the monitor.
		/// @param yPos Output variable for the y position of the monitor.
		static void GetMonitorPos(const InternalMonitor& monitor, i32& xPos, i32& yPos);
		/// @brief This function returns the position, in screen coordinates, of the upper-left corner
		/// of the work area of the specified monitor along with the work area size in screen
		/// coordinates. The work area is defined as the area of the monitor not occluded by
		/// the operating system task bar where present. If no task bar exists then the work
		/// area is the monitor resolution in screen coordinates.
		///
		/// If an error occurs, xPos, yPos, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to query.
		/// @param xPos Output variable for the x position of the monitor.
		/// @param yPos Output variable for the y position of the monitor.
		/// @param width Output variable for the width of the monitor.
		/// @param height Output variable for the height of the monitor.
		static void GetMonitorWorkArea(const InternalMonitor& monitor, i32& xPos, i32& yPos,
		                               i32& width, i32& height);
		/// @brief This function makes the specified window visible if it was previously
		/// hidden. If the window is already visible or is in full screen mode,
		/// this function does nothing.
		///
		/// By default, windowed mode window are focused when shown. Set the Hint::FocusOnShow
		/// window hint to change this behaviour for all newly created windows, or change
		/// the behaviour for and existing window with WindowingAPI::SetWindowHint.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland Because Wayland wants every frame of the desktop to be complete, this function
		///                  does not immediately make the window visible. Instead it will become visible the
		///                  next time the window framebuffer is updated after this call.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to display.
		static void ShowWindow(InternalWindow& window);
		/// @brief This function brings the specified window to front and sets input focus.
		/// The window should already be visible and not minimized/iconified.
		///
		/// By default, both windowed and full screen mode windows are focused
		/// when initially created. Set the Hint::Focused hint to disable
		/// this behaviour.
		///
		/// Also by default windowed mode windows are focused when shown
		/// with WindowingAPI::ShowWindow. Set the Hint::FocusOnShow hint to
		/// disable this behaviour.
		///
		/// DO NOT USE THIS FUNCTION to steal focus from other applications
		/// unless you are certain that is what the user wants. Focus
		/// stealing can be extremely disruptive.
		///
		/// For a less disruptive way of gettings the user's attention, see
		/// WindowingAPI::RequestWindowAttention.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland It is not possible for an application to set the input focus.
		///                  This function will emit Error::Feature_Unavailable.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to focus.
		static void FocusWindow(const InternalWindow& window);
		/// @brief This function maximizes the specified window if it was previously not
		/// maximized. If the window is already maximized, this function does nothing.
		///
		/// If the specified window is a full screen window, this function does nothing.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function may only be called from the main thread.
		/// @param window Internal window to maximize.
		static void MaximizeWindow(InternalWindow& window);
		/// @brief This function minimizes (iconifies) the specified window if it was previously
		/// restored. If the window is already minimized (iconified), this function does
		/// nothing.
		///
		/// If the specified window is a full screen window, the original monitor resolution
		/// is restored util the window is restored.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @remark @wayland Once a window is minimized, RestoreWindow won't be able to restore it.
		///                  This is a design decision of the xdg-shell protocol.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to minimize/iconify.
		static void MinimizeWindow(const InternalWindow& window);
		/// @brief This function requests user attention to the specified window.
		/// On platforms where this is not supported, attention is requested
		/// to the application as a whole.
		///
		/// Once the user has given attention, usually by focusing the window
		/// or application, the system will end the request automatically.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to request user attention for.
		static void RequestWindowAttention(InternalWindow& window);
		/// @brief This function hides the specified window if it was previously visible. If
		/// the window is already hidden or is in full screen mode, this function
		/// does nothing.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to hide.
		static void HideWindow(InternalWindow& window);
		/// @brief Restores the specified window.
		/// This function restores the specified window if it was previously
		/// minimized (iconified) or maximized. If the window is already restored,
		/// this function does nothing.
		///
		/// If the specified window is a full screen window, the resolution chosen
		/// for the window is restored on the selected monitor.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to be restored.
		static void RestoreWindow(InternalWindow& window);
		/// @brief Sets the size limits of the specified window.
		///
		/// Wayland: The size limits will not be applied until the window is actually resized, either by the
		///          user or by the compositor.
		/// This function sets the size limits of the content area of the specified window. If the
		/// window is full screen, the size limits only take effect once it is made windowed.
		/// If the window is not resizable, this function does nothing.
		///
		/// The size limits are applied immediately to a windowed mode window and may cause it
		/// to be resized.
		///
		/// The maximum dimensions must be greater than or equal to the minimum dimensions
		/// and all must be greater than zero.
		///
		/// Use -1 to disable to minimum and/or maximum size constraints.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value and
		///         Error::Platform_Error.
		/// @remark @wayland The size limits will not be applied until the window is actually resized,
		///                  either by the user or by the compositor.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set size limits for.
		/// @param minWidth New minimum window width.
		/// @param minHeight New minimum window height.
		/// @param maxWidth New maximum window width.
		/// @param maxHeight New maximum window height.
		static void SetWindowSizeLimits(InternalWindow& window, i32 minWidth, i32 minHeight,
		                                i32 maxWidth, i32 maxHeight);
		/// @brief Sets the aspect ratio of the specified window.
		///
		/// Wayland: The aspect ratio will not be applied until the window is actually resized, either
		///          by the user or by the compositor.
		///
		/// This function sets the required aspect ratio of the content area of the specified window.
		/// If the window if full screen, the aspect ratio only takes effect once it is made windowed.
		/// If the window is not resizable, this function does nothing.
		///
		/// The aspect ratio is specified as a numerator and a denominator and both values must be
		/// greater than 0. For example, the common 16:9 aspect ratio is specified as 16 and 9, respectively.
		///
		/// If the numerator and denominator are set to -1 then the aspect ratio limit gets disabled.
		///
		/// The aspect ratio is applied immediately to a windowed mode window and may cause it to be resized.
		///
		/// @note If you set the size limits and an aspect ratio that conflict, the results are undefined.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value and
		///         Error::Platform_Error.
		/// @remark @wayland The aspect ratio will not be applied until the window is actually resized,
		///                  either by the user or by the compositor.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set aspect ratio for.
		/// @param numerator Numerator of the desired aspect ratio, or -1.
		/// @param denominator Denominator of the desired aspect ratio, or -1.
		static void SetWindowAspectRatio(InternalWindow& window, i32 numerator, i32 denominator);
		/// @brief This function sets the system clipboard to the specified, UTF-8 encoded string.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Format_Unavailable
		/// and Error::Platform_Error.
		/// Pointer lifetime: The specified string is copied before this function returns.
		/// @threadsafety This function must only be called from the main thread.
		/// @param string UTF-8 encoded string to be set for the clipboard.
		static void SetClipboardString(const std::string& string);
		/// @brief This function returns the contents of the system clipboard, if it contains or
		/// is convertible to a UTF-8 encoded string. If the clipboard is empty or if its
		/// content cannot be converted, an empty string is returned and a Error::Format_Unavailable
		/// error is generated.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @return UTF-8 encoded string containing clipboard contents, or empty string if an error occurred.
		[[nodiscard]] static std::string GetClipboardString();
		/// @brief This function returns whether the Vulkan loader and any minimally function ICD
		/// have been found.
		///
		/// The availability of a Vulkan loader and even an ICD does not by itself guarantee
		/// that surface creation or even instance creation is possible. Call GetRequiredInstanceExtensions
		/// to check whether the extensions necessary for Vulkan surface creation are available.
		/// You still have to check whether a queue family of a physical device supports image presentation.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// @threadsafe
		/// @return True if Vulkan API is minimally supported, false otherwise.
		[[nodiscard]] static bool VulkanSupported();
		/// @brief This function returns an array of names of Vulkan instance extensions required
		/// by the WindowingAPI for creating Vulkan surface for WindowingAPI windows.
		/// If successful, the list will always contain VK_KHR_surface, so if you don't
		/// require any additional extensions you can pass this list directly to the
		/// VkInstanceCreateInfo struct.
		///
		/// If Vulkan is not available on the machine, this function return an array with
		/// empty strings and generates a Error::API_Unavailable error.
		/// Call WindowingAPI::VulkanSupported to check whether Vulkan is at least
		/// minimally available.
		///
		/// If Vulkan is available but no set of extensions allowing window surface creation
		/// was found, this function returns an array with empty strings. You may still use
		/// Vulkan for off-screen rendering and compute work.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::API_Unavailable.
		/// @remark: Additional extensions may be required by future versions of the WindowingAPI.
		///          You should check if any extensions you wish to enable are already in the
		///          returned array, as it is an error to specify an extension more than once
		///          in the VkInstanceCreateInfo struct.
		/// @threadsafe
		/// @return Array containing the required instance extensions, or an array with empty string
		/// if an error occurred.
		[[nodiscard]] static std::array<std::string, 2u> GetRequiredInstanceExtensions();
		/// @brief Creates a Vulkan surface for the specified window.
		/// This function create a Vulkan surface for the specified window.
		///
		/// If the Vulkan loader or at least one minimally functiona ICD were not found,
		/// this function return VK_ERROR_INITIALIZATION_FAILED and generates a Error::API_Unavilable error.
		/// Call WindowingAPI::VulkanSupported to check whether Vulkan is at least minimally available.
		///
		/// If the required window surface creation instance extensions are not available or if the
		/// specified instance was not created with these extensions enabled, this function
		/// returns VK_ERROR_EXTENSION_NOT_PRESENT and generates a Error::API_Unavailable error.
		/// Call WindowingAPI::GetRequiredInstanceExtensions to check what instance extensions
		/// are required.
		///
		/// The window surface must be destroyed before the specified Vulkan instance.
		/// It is the responsibility of the caller to destroy the window surface.
		/// The WindowingAPI does not destroy it for you.
		/// Call vkDestroySurfaceKHR to destroy the surface.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::API_Unavailable,
		///         Error::Platform_Error and Error::Invalid_Value.
		/// @remark: If an error occurs before the creation call is made, the WindowingAPI returns
		///          the Vulkan error code most appropriate for the error. Appropriate use of
		///          WindowingAPI::VulkanSupported and WindowingAPI::GetRequiredInstanceExtensions
		///          should eliminate almost all occurrences of these errors.
		/// @threadsafe
		/// @param instance Vulkan instance.
		/// @param window Internal window for which to create the surface for.
		/// @param allocator Optional allocator.
		/// @param surface Output variable for the new Vulkan surface.
		/// @return VK_SUCCESS if successful, or a Vulkan error code if an error occurred.
		[[nodiscard]] static VkResult CreateWindowSurface(VkInstance instance, const InternalWindow& window,
		                                                  const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		/// @brief Hides the specified window from the taskbar.
		/// @param window Internal window to hide from the taskbar.
		static void HideWindowFromTaskbar(InternalWindow& window);
		/// @brief Enable/Disable drag and drop feature for the specified window.
		/// @param window Internal window for which to set drag and drop.
		/// @param value Whether to enable or disable drag and drop.
		static void SetDragAndDrop(InternalWindow& window, bool value);
#ifdef TRAP_PLATFORM_WINDOWS
		/// @brief Returns the HWND handle of the internal window.
		/// @param window Internal window from which to get the HWND from.
		/// @return HWND of the specified window.
		[[nodiscard]] static HWND GetWin32Window(const InternalWindow& window);
#endif /*TRAP_PLATFORM_WINDOWS*/
		//-------//
		//Private//
		//-------//
	private:
		/// @brief Center the cursor in the content area of the specified windows.
		/// @param window Internal window which to center the cursor for.
		static void CenterCursorInContentArea(InternalWindow& window);
		/// @brief Notifies shared code of an error.
		/// @param code Error code.
		/// @param str Description of the occurred error.
		static constexpr void InputError(Error code, std::string_view str);
		//-------------------------------------------------------------------------------------------------------------------//
		//Platform Specific Functions----------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		/// @brief Create key code translation tables.
		static void CreateKeyTablesWin32();
		static void CreateKeyTablesX11();
		static void CreateKeyTablesWayland();
		/// @brief This function returns the current video mode of the specified monitor.
		/// If you have create a full screen window for that monitor, the return
		/// value will depend on whether that window is minimized/iconified.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to get internal video mode from.
		/// @return Current video mode of the specified monitor, or an empty optional if an error occurred.
		[[nodiscard]] static std::optional<InternalVideoMode> PlatformGetVideoMode(const InternalMonitor& monitor);
		[[nodiscard]] static std::optional<InternalVideoMode> PlatformGetVideoModeX11(const InternalMonitor& monitor);
		[[nodiscard]] static constexpr std::optional<InternalVideoMode> PlatformGetVideoModeWayland(const InternalMonitor& monitor);
		/// @brief This function retrieves the size, in screen coordinates, of the content area of
		/// the specified window. If you wish to retrieve the size of the framebuffer of the window
		/// in pixels, see WindowingAPI::GetFrameBufferSize.
		///
		/// If an error occurs, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the size from.
		/// @param width Output variable for the internal windows current width.
		/// @param height Output variable for the internal windows current height.
		static void PlatformGetWindowSize(const InternalWindow& window, i32& width, i32& height);
		static void PlatformGetWindowSizeX11(const InternalWindow& window, i32& width, i32& height);
		static constexpr void PlatformGetWindowSizeWayland(const InternalWindow& window, i32& width, i32& height);
		/// @brief This function sets the position, in screen coordinates, of the upper-left corner of the
		/// content area of the specifed windowed mode window. If the window is a full screen window,
		/// this function does nothing.
		///
		/// DO NOT USE THIS FUNCTION to move an already visible window unless you have very
		/// good reasons for doing so, as it will confuse and annoy the user.
		///
		/// The window manager may put limits on what positions are allowed. The WindowingAPI
		/// cannot and should not override these limits.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the position for.
		/// @param xPos X position to be set.
		/// @param yPos Y position to be set.
		static void PlatformSetWindowPos(const InternalWindow& window, i32 xPos, i32 yPos);
		static void PlatformSetWindowPosX11(const InternalWindow& window, i32 xPos, i32 yPos);
		static constexpr void PlatformSetWindowPosWayland(const InternalWindow& window, i32 xPos, i32 yPos);
		/// @brief This function sets the monitor that the window uses for full screen mode or,
		/// if the monitor is nullptr, makes it windowed mode.
		///
		/// When setting a monitor, this function updates the width, height and
		/// refresh rate of the desired video mode and switches to the video mode
		/// closest to it. The window position is ignored when setting a monitor.
		///
		/// When the monitor is nullptr, the position, width and height are used to place the
		/// window content area. The refresh rate is ignored when no monitor is specified.
		///
		/// If you only wish to update the resolution of a full screen window
		/// or the size of a windowed mode window, see WindowingAPI::SetWindowSize.
		///
		/// When a window transitions from full screen to windowed mode, this function
		/// restores any previous window settings such as whether it is decorated,
		/// floating, resizable, has size limits, etc.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the monitor for.
		/// @param monitor Internal monitor to use or nullptr.
		/// @param xPos New x position for the window.
		/// @param yPos New y position for the window.
		/// @param width New width for the window.
		/// @param height New height for the window.
		/// @param refreshRate New refresh rate for the window.
		static void PlatformSetWindowMonitor(InternalWindow& window, InternalMonitor* monitor,
			                                 i32 xPos, i32 yPos, i32 width, i32 height,
											 f64 refreshRate);
		static void PlatformSetWindowMonitorX11(InternalWindow& window, InternalMonitor* monitor,
			                                    i32 xPos, i32 yPos, i32 width, i32 height,
											    f64 refreshRate);
		static void PlatformSetWindowMonitorWayland(InternalWindow& window, InternalMonitor* monitor,
			                                        i32 xPos, i32 yPos, i32 width, i32 height,
											        f64 refreshRate);
		/// @brief This function sets the monitor that the window uses for borderless full screen mode.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the monitor for.
		/// @param monitor Internal monitor to use.
		static void PlatformSetWindowMonitorBorderless(InternalWindow& window, InternalMonitor& monitor);
		static void PlatformSetWindowMonitorBorderlessX11(InternalWindow& window, InternalMonitor& monitor);
		static void PlatformSetWindowMonitorBorderlessWayland(InternalWindow& window, InternalMonitor& monitor);
		/// @brief This function returns a vector of all video modes supported by the specified monitor.
		/// The returned vector is sorted in ascending order, first by color bit depth
		/// (the sum of all channel depths) and then by resolution area (the product of
		/// width and height).
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to get all internal video modes from.
		/// @return Vector with all available video modes of the specified monitor, or an empty vector
		/// if an error occurred.
		[[nodiscard]] static std::vector<InternalVideoMode> PlatformGetVideoModes(const InternalMonitor& monitor);
		[[nodiscard]] static std::vector<InternalVideoMode> PlatformGetVideoModesX11(const InternalMonitor& monitor);
		[[nodiscard]] static constexpr std::vector<InternalVideoMode> PlatformGetVideoModesWayland(const InternalMonitor& monitor);
		/// @brief Initializes the windowing API.
		/// Before most WindowingAPI functions can be used, the WindowingAPI must be initialized, and before the engine
		/// terminates the WindowingAPI should be terminated in order to free any resources allocated during or after
		/// initialization.
		///
		/// If this function fails, it calls WindowingAPI::Shutdown before returning.
		/// If it succeeds, WindowingAPI::Shutdown should be called before the engine exits.
		///
		/// Additional calls to this function after successful initialization but before terminiation will return true
		/// immediately.
		///
		/// Errors: Possible errors include Error::Platform_Unavailable and Error::Platform_Error.
		/// @remark @x11 This function will set the LC_CTYPE category of the engine locale according
		///              to the current environment if that category is still "C". This is because
		///              the "C" locale breaks Unicode text input.
		/// @threadsafety This function must only be called from the main thread.
		/// @return True if windowing API was successfully initialized, false otherwise.
		[[nodiscard]] static bool PlatformInit();
		[[nodiscard]] static bool PlatformInitX11();
		[[nodiscard]] static bool PlatformInitWayland();
		/// @brief This function destroys the specified window. On calling this function, no
		/// further callbacks will be called for that window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to destroy.
		static void PlatformDestroyWindow(InternalWindow& window);
		static void PlatformDestroyWindowX11(InternalWindow& window);
		static void PlatformDestroyWindowWayland(InternalWindow& window);
		/// @brief This function destroys all remaining windows and cursor, and frees any other allocated resources.
		/// Once this function is called, you must again call WindowingAPI::Init successfully before you
		/// will be able to use most WindowingAPI functions.
		///
		/// If the WindowingAPI has been successfully initialized, this function should be called before the
		/// engine exits.
		/// If initialization fails, there is no need to call this function, as it is called by WindowingAPI::Init
		/// before it returns failure.
		/// This function has no effect if the WindowingAPI is not initialized.
		///
		/// Possible errors include Error::Platform_Error.
		///
		/// @remark: This function may be called before WindowingAPI::Init.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		static void PlatformShutdown();
		static void PlatformShutdownX11();
		static void PlatformShutdownWayland();
		/// @brief This function retrieves the content scale for the specified monitor.
		/// The content scale is the ratio between the current DPI and the platform's default DPI.
		/// This is especially important for text and any UI elements. If the pixel dimensions
		/// of your UI scaled by this look appropriate on your machine then it should appear
		/// at a reasonable size on other machines regardless of their DPI and scaling settings.
		/// This relies on the system DPI and scaling settings being somewhat correct.
		///
		/// The content scale may depend on both the monitor resolution and pixel density and
		/// on user settings. It may be very different from the raw DPI calculated from the
		/// physical size and current resolution.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor.
		/// @param xScale Output variable for the X scale of the provided monitor.
		/// @param yScale Output variable for the Y scale of the provided monitor.
		static void PlatformGetMonitorContentScale(const InternalMonitor& monitor, f32& xScale, f32& yScale);
		static void PlatformGetMonitorContentScaleX11(const InternalMonitor& monitor, f32& xScale, f32& yScale);
		static constexpr void PlatformGetMonitorContentScaleWayland(const InternalMonitor& monitor, f32& xScale, f32& yScale);
		/// @brief Returns the position of the monitor's viewport on the virtual screen.
		/// This function returns the position, in screen coordinates, of the upper-left corner
		/// of the specified monitor.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to query.
		/// @param xPos Output variable for the x position of the monitor.
		/// @param yPos Output variable for the y position of the monitor.
		static void PlatformGetMonitorPos(const InternalMonitor& monitor, i32& xPos, i32& yPos);
		static void PlatformGetMonitorPosX11(const InternalMonitor& monitor, i32& xPos, i32& yPos);
		static constexpr void PlatformGetMonitorPosWayland(const InternalMonitor& monitor, i32& xPos, i32& yPos);
		/// @brief This function makes the specified window visible if it was previously
		/// hidden. If the window is already visible or is in full screen mode,
		/// this function does nothing.
		///
		/// By default, windowed mode window are focused when shown. Set the Hint::FocusOnShow
		/// window hint to change this behaviour for all newly created windows, or change
		/// the behaviour for and existing window with WindowingAPI::SetWindowHint.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to display.
		static void PlatformShowWindow(InternalWindow& window);
		static void PlatformShowWindowX11(const InternalWindow& window);
		static void PlatformShowWindowWayland(InternalWindow& window);
		/// @brief This function brings the specified window to front and sets input focus.
		/// The window should already be visible and not minimized/iconified.
		///
		/// By default, both windowed and full screen mode windows are focused
		/// when initially created. Set the Hint::Focused hint to disable
		/// this behaviour.
		///
		/// Also by default windowed mode windows are focused when shown
		/// with WindowingAPI::ShowWindow. Set the Hint::FocusOnShow hint to
		/// disable this behaviour.
		///
		/// DO NOT USE THIS FUNCTION to steal focus from other applications
		/// unless you are certain that is what the user wants. Focus
		/// stealing can be extremely disruptive.
		///
		/// For a less disruptive way of gettings the user's attention, see
		/// WindowingAPI::RequestWindowAttention.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to focus.
		static void PlatformFocusWindow(const InternalWindow& window);
		static void PlatformFocusWindowX11(const InternalWindow& window);
		static constexpr void PlatformFocusWindowWayland(const InternalWindow& window);
		/// @brief This function creates a window.
		/// Most of the options controlling how the window should be created are specified with window hints.
		///
		/// The created window may differ from what you requested, as not all parameters and
		/// hints are hard constraints. This includes the size of the window, especially for full screen windows.
		/// To query the actual attributes of the create window, see WindowingAPI::GetWindowHint,
		/// WindowingAPI::GetWindowSize and WindowingAPI::GetFramebufferSize.
		///
		/// To create a full screen window, you need to specify the monitor the window will cover.
		/// If no monitor is specified, the window will be windowed mode. Unless you have a way
		/// for the user to choose a specific monitor, it is recommended that you pick the
		/// primary monitor.
		///
		/// For full screen windows, the specified size becomes the resolution of the window's desired
		/// video mode. As long as a full screen window is not minimized/iconified, the supported
		/// video mode most closely matching the desired video mode is set for the specified monitor.
		///
		/// Once you have create the window, you can switch it between windowed, borderless full screen (windowed)
		/// and full screen mode with WindowingAPI::SetWindowMonitor and WindowingAPI::SetWindowMonitorBorderless.
		///
		/// By default, newly created windows use the placement recommended by the window system.
		/// To create the window at a specific position, make it initially invisible using the Hint::Visibile
		/// window hint, set its position and then show it.
		///
		/// As long as at least one full screen window is not minimized/iconified, the screensaver is prohibited
		/// from starting.
		///
		/// Window systems put limits on window sizes. Very large or very small window dimensions
		/// may be overriden by the window system on creation. Check the actual size after creation.
		///
		/// Errors: Possible errors include Error::Invalid_Enum, Error::Invalid_Value,
		///         Error::Format_Unavailable and Error::Platform_Error.
		/// @remark @win32 If the executable has an icon resource named 'TRAP_ICON', it will be set as the initial
		///                icon for the window. If no such icon is present, the IDI_APPLICATION icon will be used
		///                instead. To set a different icon, see WindowingAPI::SetWindowIcon.
		/// @remark @x11 Some window managers will not respect the placement of initially hidden windows.
		///              Due to the asynchronous nature of X11, it may take a moment for a window to reach its
		///              requested state. This means you may not be able to query the final size, position or
		///              other attributes directly after window creation.
		///              The class part of the WM_CLASS window property will by default be set to the window
		///              title passed to this function. The instance part will use the contents of the
		///              RESOURCE_NAME environment variable, if present and not empty, or fall back to the
		///              window title.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to create.
		/// @param WNDConfig Configuration of the window to create.
		/// @return True if the window was created successfully.
		[[nodiscard]] static bool PlatformCreateWindow(InternalWindow& window,
			                                           WindowConfig& WNDConfig);
		[[nodiscard]] static bool PlatformCreateWindowX11(InternalWindow& window,
			                                              WindowConfig& WNDConfig);
		[[nodiscard]] static bool PlatformCreateWindowWayland(InternalWindow& window,
			                                                  const WindowConfig& WNDConfig);
		/// @brief This function sets the window title, encoded as UTF-8, of the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window whose title to change.
		/// @param title New UTF-8 encoded title for the window.
		static void PlatformSetWindowTitle(InternalWindow& window, const std::string& title);
		static void PlatformSetWindowTitleX11(const InternalWindow& window, const std::string& title);
		static void PlatformSetWindowTitleWayland(InternalWindow& window, const std::string& title);
		/// @brief Creates a new custom cursor image that can be set for a window with SetCursor. The cursor can
		/// be destroyed with DestroyCursor. Any remaining cursors are destroyed by WindowingAPI::Shutdown.
		///
		/// The cursor hotspot is specified in pixels, relative to the upper-left corner of the cursor image.
		/// Like all other coordinate system in the WindowingAPI, the X-axis points to the right and the
		/// Y-axis points down.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param cursor Internal cursor to create.
		/// @param image Non HDR RGB 24BPP or RGBA 32 BPP image.
		/// @param xHotspot Center x coordinate of the image.
		/// @param yHotspot Center y coordinate of the image.
		/// @return True if the cursor was created successfully.
		[[nodiscard]] static bool PlatformCreateCursor(InternalCursor& cursor, const Image& image, u32 xHotspot,
		                                               u32 yHotspot);
		[[nodiscard]] static bool PlatformCreateCursorX11(InternalCursor& cursor, const Image& image, u32 xHotspot,
		                                                  u32 yHotspot);
		[[nodiscard]] static bool PlatformCreateCursorWayland(InternalCursor& cursor, const Image& image, u32 xHotspot,
		                                                      u32 yHotspot);
		/// @brief Creates a cursor with a standard shape.
		///
		/// Errors: Possible errors include Error::Invalid_Enum and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param cursor Internal cursor to create.
		/// @param type Cursor type to get.
		/// @return True if the cursor was created successfully.
		[[nodiscard]] static bool PlatformCreateStandardCursor(InternalCursor& cursor, const CursorType& type);
		[[nodiscard]] static bool PlatformCreateStandardCursorX11(InternalCursor& cursor, const CursorType& type);
		[[nodiscard]] static bool PlatformCreateStandardCursorWayland(InternalCursor& cursor, const CursorType& type);
		/// @brief This function destroys a cursor previously created with CreateCursor. Any remaining cursors
		/// will be destroyed by WindowingAPI::Shutdown.
		///
		/// If the specified cursor is current for any window, that window will be reverted to the default
		/// cursor. This does not affect the cursor mode.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		/// @param cursor Internal cursor to be destroyed.
		static void PlatformDestroyCursor(InternalCursor& cursor);
		static void PlatformDestroyCursorX11(const InternalCursor& cursor);
		static void PlatformDestroyCursorWayland(InternalCursor& cursor);
		/// @brief This function sets the cursor image to be used when the cursor is over the content are of the
		/// specified window. The set cursor will only be visible when the CursorMode of the window is
		/// CursorMode::Normal.
		///
		/// On some platforms, the set cursor may not be visible unless the window also has input focus.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the cursor for.
		/// @param cursor Internal cursor to set, or nullptr to switch back to the default arrow cursor.
		static void PlatformSetCursor(InternalWindow& window, InternalCursor* cursor);
		static void PlatformSetCursorX11(const InternalWindow& window, const InternalCursor* cursor);
		static void PlatformSetCursorWayland(InternalWindow& window, InternalCursor* cursor);
		/// @brief This function sets a cursor mode for the specified window.
		///
		/// The mode must be one of the following CursorModes:
		/// - Normal makes the cursor visible and behaving normally.
		/// - Hidden makes the cursor invisible when it is over the content area of the
		///   window but does not restrict the cursor from leaving.
		/// - Disabled hides and grabs the cursor, providing virtual and unlimited cursor movement.
		/// - Captured makes the cursor visible and confines it to the content area of the window.
		///
		/// Errors: Possible errors include Error::Invalid_Enum and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the cursor mode for.
		/// @param mode Cursor mode to be set.
		static void PlatformSetCursorMode(InternalWindow& window, CursorMode mode);
		static void PlatformSetCursorModeX11(InternalWindow& window, CursorMode mode);
		static void PlatformSetCursorModeWayland(InternalWindow& window, CursorMode mode);
		/// @brief This function sets the position, in screen coordinates, of the cursor relative to the
		/// upper-left corner of the content area of the specified window. The window must have
		/// input focus. If the window does not have input focus when this function is called,
		/// it fails silently.
		///
		/// DO NOT USE THIS FUNCTION to implement things like camera controls. The WindowingAPI already
		/// provides the CursorMode::Disabled that hides the cursor, transparently re-centers it and
		/// provides unconstrained cursor motion. See SetCursorMode for more information.
		///
		/// If the cursor mode is CursorMode::Disabled then the cursor position is unconstrained and
		/// limited only by the minimum and maximum values of a f64.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the cursor position for.
		/// @param xPos New x position, relative to the left edge of the content area, for the cursor.
		/// @param yPos New y position, relative to the top edge of the content area, for the cursor.
		static void PlatformSetCursorPos(InternalWindow& window, f64 xPos, f64 yPos);
		static void PlatformSetCursorPosX11(InternalWindow& window, f64 xPos, f64 yPos);
		static constexpr void PlatformSetCursorPosWayland(InternalWindow& window, f64 xPos, f64 yPos);
		/// @brief This function returns the position of the cursor, in screen coordinates, relative
		/// to the upper-left corner of the content area of the specified window.
		///
		/// If the cursor is disabled (with CursorMode::Disabled) then the cursor position is unbounded and limited
		/// only by the minimum and maximum values of a f64.
		///
		/// The coordinate can be converted to their integer equivalents with the TRAP::Math::Floor function.
		/// Casting directly to an integer type works for positive coordinates, but fails for negative ones.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window from which to get the current cursor position.
		/// @param xPos
		/// Output variable for the current x position, relative to the left edge of the content area, of the cursor.
		/// @param yPos
		/// Output variable for the current y position, relative to the top edge of the content area, of the cursor.
		static void PlatformGetCursorPos(const InternalWindow& window, f64& xPos, f64& yPos);
		static void PlatformGetCursorPosX11(const InternalWindow& window, f64& xPos, f64& yPos);
		static constexpr void PlatformGetCursorPosWayland(const InternalWindow& window, f64& xPos, f64& yPos);
		/// @brief This function sets the icon of the specified window. If no image is specified, the window
		/// reverts to its default icon.
		///
		/// The image mustbe RGB 24BPP or RGBA 32BPP.
		///
		/// The desired image size varies depending on platform and system settings.
		/// Good sizes include 16x16, 32x32 and 48x48.
		///
		/// Errors: Possible errors Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window whose icon to set.
		/// @param image Image to be set as window icon or nullptr to revert back to the default icon.
		static void PlatformSetWindowIcon(InternalWindow& window, const Image* image);
		static void PlatformSetWindowIconX11(const InternalWindow& window, const Image* image);
		static constexpr void PlatformSetWindowIconWayland(InternalWindow& window, const Image* image);
		/// @brief This function retrieves the position, in screen coordinates, of the upper-left corner of the
		/// content area of the specified window.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the current position from.
		/// @param xPos Output variable for the current x position of the internal window.
		/// @param yPos Output variable for the current y position of the internal window.
		static void PlatformGetWindowPos(const InternalWindow& window, i32& xPos, i32& yPos);
		static void PlatformGetWindowPosX11(const InternalWindow& window, i32& xPos, i32& yPos);
		static constexpr void PlatformGetWindowPosWayland(const InternalWindow& window, i32& xPos, i32& yPos);
		/// @brief This function sets the size, in screen coordinates, of the content area of the specified window.
		///
		/// For full screen windows, this function updates the resolution of its desired video mode
		/// and switches to the video mdoe closest to it.
		///
		/// If you wish to update the refresh rate of the desired video mode in addition
		/// to its resolution, see WindowingAPI::SetWindowMonitor.
		///
		/// The window manager may put limits on what sizes are allowed. The WindowingAPI
		/// cannot and should not override these limits.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the size for.
		/// @param width New width for the internal window.
		/// @param height New height for the internal window.
		static void PlatformSetWindowSize(InternalWindow& window, i32 width, i32 height);
		static void PlatformSetWindowSizeX11(InternalWindow& window, i32 width, i32 height);
		static void PlatformSetWindowSizeWayland(InternalWindow& window, i32 width, i32 height);
		/// @brief This function toggles the resizeability of the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the size for.
		/// @param enabled Enable or disable resizing for the internal window.
		static void PlatformSetWindowResizable(InternalWindow& window, bool enabled);
		static void PlatformSetWindowResizableX11(const InternalWindow& window, bool enabled);
		static void PlatformSetWindowResizableWayland(InternalWindow& window, bool enabled);
		/// @brief This function toggles the decorations of the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the size for.
		/// @param enabled Enable or disable decorations for the internal window.
		static void PlatformSetWindowDecorated(InternalWindow& window, bool enabled);
		static void PlatformSetWindowDecoratedX11(const InternalWindow& window, bool enabled);
		static void PlatformSetWindowDecoratedWayland(InternalWindow& window, bool enabled);
		/// @brief This function toggles whether the specified window is floating.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the size for.
		/// @param enabled Enable or disable floating mode for the internal window.
		static void PlatformSetWindowFloating(const InternalWindow& window, bool enabled);
		static void PlatformSetWindowFloatingX11(const InternalWindow& window, bool enabled);
		static constexpr void PlatformSetWindowFloatingWayland(const InternalWindow& window, bool enabled);
		/// @brief This function sets the opacity of the window, including any decorations.
		///
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque.
		///
		/// The initial opacity value for newly created windows is 1.0f.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set opacity for.
		/// @param opacity Opacity ranging from 0.0f-1.0f.
		static void PlatformSetWindowOpacity(const InternalWindow& window, f32 opacity);
		static void PlatformSetWindowOpacityX11(const InternalWindow& window, f32 opacity);
		static constexpr void PlatformSetWindowOpacityWayland(const InternalWindow& window, f32 opacity);
		/// @brief This function toggles mouse passthrough for the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set the size for.
		/// @param enabled Enable or disable mouse passthrough for the internal window.
		static void PlatformSetWindowMousePassthrough(InternalWindow& window, bool enabled);
		static void PlatformSetWindowMousePassthroughX11(const InternalWindow& window, bool enabled);
		static void PlatformSetWindowMousePassthroughWayland(InternalWindow& window, bool enabled);
		/// @brief Hides the specified window from the taskbar.
		/// @param window Internal window to hide from the taskbar.
		static void PlatformHideWindowFromTaskbar(InternalWindow& window);
		static constexpr void PlatformHideWindowFromTaskbarX11(const InternalWindow& window);
		static constexpr void PlatformHideWindowFromTaskbarWayland(InternalWindow& window);
		/// @brief This function returns the opacity of the window, including any decorations.
		///
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque. If the system does not
		/// support whole window transparency, this function always returns one.
		///
		/// The initial opacity value for newly created windows is 1.0f.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the opacity from.
		/// @return Opacity of the given internal window on success, empty optional otherwise.
		[[nodiscard]] static std::optional<f32> PlatformGetWindowOpacity(const InternalWindow& window);
		[[nodiscard]] static std::optional<f32> PlatformGetWindowOpacityX11(const InternalWindow& window);
		[[nodiscard]] static constexpr std::optional<f32> PlatformGetWindowOpacityWayland(const InternalWindow& window);
		/// @brief This function retrieves the size, in pixels, of the framebuffer of the specified window.
		/// If you wish to retrieve the size of the window in screen coordinates, see
		/// WindowingAPI::GetWindowSize.
		///
		/// If an error occcurs, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the framebuffer size from.
		/// @param width Output variable for the internal windows current framebuffer width.
		/// @param height Output variable for the internal windows current framebuffer height.
		static void PlatformGetFrameBufferSize(const InternalWindow& window, i32& width, i32& height);
		static void PlatformGetFrameBufferSizeX11(const InternalWindow& window, i32& width, i32& height);
		static constexpr void PlatformGetFrameBufferSizeWayland(const InternalWindow& window, i32& width, i32& height);
		/// @brief This function retrieves the content scale for the specified window.
		/// The content scale is the reatio between the current DPI and the platform's
		/// default DPI. This is especially important for text and any UI elements.
		/// If the pixel dimensions of your UI scaled by this look appropriate
		/// on your machine then it should appear at a reasonable size on other
		/// machines regardless of their DPI and scaling settings. This relies
		/// on the system DPI and scaling settings being somewhat correct.
		///
		/// On systems where each monitor can have its own content scale, the window
		/// content scale will depend on which monitor the system considers the window
		/// to be on.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get the content scale from.
		/// @param xScale Output variable for the internal windows content scale x.
		/// @param yScale Output variable for the internal windows content scale y.
		static void PlatformGetWindowContentScale(const InternalWindow& window, f32& xScale, f32& yScale);
		static void PlatformGetWindowContentScaleX11(const InternalWindow& window, f32& xScale, f32& yScale);
		static constexpr void PlatformGetWindowContentScaleWayland(const InternalWindow& window, f32& xScale, f32& yScale);
		/// @brief This function returns the position, in screen coordinates, of the upper-left corner
		/// of the work area of the specified monitor along with the work area size in screen
		/// coordinates. The work area is defined as the area of the monitor not occluded by
		/// the operating system task bar where present. If no task bar exists then the work
		/// area is the monitor resolution in screen coordinates.
		///
		/// If an error occurs, xPos, yPos, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param monitor Internal monitor to query.
		/// @param xPos Output variable for the x position of the monitor.
		/// @param yPos Output variable for the y position of the monitor.
		/// @param width Output variable for the width of the monitor.
		/// @param height Output variable for the height of the monitor.
		static void PlatformGetMonitorWorkArea(const InternalMonitor& monitor, i32& xPos, i32& yPos,
		                                       i32& width, i32& height);
		static void PlatformGetMonitorWorkAreaX11(const InternalMonitor& monitor, i32& xPos, i32& yPos,
		                                       i32& width, i32& height);
		static constexpr void PlatformGetMonitorWorkAreaWayland(const InternalMonitor& monitor, i32& xPos, i32& yPos,
		                                                        i32& width, i32& height);
		/// @brief This function returns whether the window is visible or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get visibility state from.
		/// @return True if the window is visible, false otherwise.
		[[nodiscard]] static bool PlatformWindowVisible(const InternalWindow& window);
		[[nodiscard]] static bool PlatformWindowVisibleX11(const InternalWindow& window);
		[[nodiscard]] static constexpr bool PlatformWindowVisibleWayland(const InternalWindow& window);
		/// @brief This function returns whether the window is maximized or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get maximization state from.
		/// @return True if the window is maximized, false otherwise.
		[[nodiscard]] static bool PlatformWindowMaximized(const InternalWindow& window);
		[[nodiscard]] static bool PlatformWindowMaximizedX11(const InternalWindow& window);
		[[nodiscard]] static constexpr bool PlatformWindowMaximizedWayland(const InternalWindow& window);
		/// @brief This function returns whether the window is minimized or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get minimization state from.
		/// @return True if the window is minimized, false otherwise.
		[[nodiscard]] static bool PlatformWindowMinimized(const InternalWindow& window);
		[[nodiscard]] static bool PlatformWindowMinimizedX11(const InternalWindow& window);
		[[nodiscard]] static constexpr bool PlatformWindowMinimizedWayland(const InternalWindow& window);
		/// @brief This function processes only those events that are already in the event queue and then
		/// returns immediately. Processing events will cause the window and input callbacks
		/// associated with those events to be called.
		///
		/// On some platforms, a window move, resize or menu operation will cause event processing
		/// to block. This is due to how event processing is designed on those platforms.
		///
		/// Do not assume that callbacks you set will only be called in response to event
		/// processing functions like this one. While it is necessary to poll for events, window
		/// systems that require the WindowingAPI to register callbacks of its own can pass evnts
		/// to the WindowingAPI in response to many window system function calls. The WindowingAPI
		/// will pass those events on to the application callbacks before returning.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		static void PlatformPollEvents();
		static void PlatformPollEventsX11();
		static void PlatformPollEventsWayland();
		/// @brief This function puts the calling thread to sleep until at least one event is available
		/// in the event queue, or until the specified timeout is reached.
		/// Once one or more events are available, it behaves exactly like
		/// PollEvents, i.e. the events in the queue are processed and the function then returns
		/// immediately. Processing events will cause the window and input callbacks
		/// associated with those events to be called.
		///
		/// Since not all events are associated with callbacks, this function may return
		/// without a callback having been called even if you are monitoring all callbacks.
		///
		/// On some platforms, a window move, resize or menu operation will cause event
		/// processing to block. This is due to how event processing is designed on
		/// those platforms.
		///
		/// Do not assume that callbacks you set will only be called in response to event
		/// processing functions like this one. While it is neccessary to poll for events,
		/// window systems that require the WindowingAPI to register callbacks of its own
		/// can pass events to the WindowingAPI in response to many window system function
		/// calls. The WindowingAPI will pass those events on to the application callbacks
		/// before returning.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value
		///         and Error::Platform_Error.
		/// @reentrancy This function must not be called from a callback.
		/// @threadsafety This function must only be called from the main thread.
		/// @param timeout Maximum amount of time, in seconds, to wait.
		static void PlatformWaitEvents(f64 timeout);
		static void PlatformWaitEventsX11(f64 timeout);
		static void PlatformWaitEventsWayland(f64 timeout);
		/// @brief This function posts an empty event from the current thread to the event queue,
		/// causing WaitEvents to return.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// @threadsafe
		static void PlatformPostEmptyEvent();
		static void PlatformPostEmptyEventX11();
		static void PlatformPostEmptyEventWayland();
		/// @brief This function returns whether the window is focused or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get focused state from.
		/// @return True if the window is focused, false otherwise.
		[[nodiscard]] static bool PlatformWindowFocused(const InternalWindow& window);
		[[nodiscard]] static bool PlatformWindowFocusedX11(const InternalWindow& window);
		[[nodiscard]] static bool PlatformWindowFocusedWayland(const InternalWindow& window);
		/// @brief This function returns whether the window is hovered or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to get hovered state from.
		/// @return True if the window is hovered, false otherwise.
		[[nodiscard]] static bool PlatformWindowHovered(const InternalWindow& window);
		[[nodiscard]] static bool PlatformWindowHoveredX11(const InternalWindow& window);
		[[nodiscard]] static constexpr bool PlatformWindowHoveredWayland(const InternalWindow& window);
		/// @brief This function returns whether raw mouse motion is supported on the current system.
		/// This status does not change after the WindowingAPI has been initialized so you
		/// only need to check this once. If you attemp to enable raw motion on a system
		/// that does not support it, Error::Platform_Error will be emitted.
		///
		/// Raw mouse motion is closer to the actual motion of the mouse across a surface.
		/// It is not affected by the scaling and acceleration applied to the motion of
		/// the desktop cursor. That processing is suitable for a cursor while raw motion
		/// is better for controlling for example a 3D camera. Because of this, raw mouse
		/// motion is only provided when the cursor is disabled.
		///
		/// @threadsafety This function must only be called from the main thread.
		/// @return True if raw mouse input is supported, false otherwise.
		[[nodiscard]] static bool PlatformRawMouseMotionSupported();
		[[nodiscard]] static bool PlatformRawMouseMotionSupportedX11();
		[[nodiscard]] static constexpr bool PlatformRawMouseMotionSupportedWayland();
		/// @brief Sets the raw mouse motion mode for the specified window.
		///
		/// If raw (unscaled and unaccelerated) mouse motion is not supported, attempting to set this
		/// will emit Error::Feature_Unavailable.
		///
		/// Errors: Possible errors include Error::Invalid_Enum,
		///         Error::Platform_Error and Error::Feature_Unavailable.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set raw mouse input for.
		/// @param enabled Whether to enable or disable raw mouse input.
		static void PlatformSetRawMouseMotion(const InternalWindow& window, bool enabled);
		static void PlatformSetRawMouseMotionX11(const InternalWindow& window, bool enabled);
		static constexpr void PlatformSetRawMouseMotionWayland(const InternalWindow& window, bool enabled);
		/// @brief Sets the dock or taskbar progress indicator of the specified window.
		///
		/// Linux: This only works on KDE & Unity environments.
		///        A .desktop file must exist for the application with the same name as given to TRAP::Application.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value,
		///         Error::Invalid_Enum, Error::Platform_Error, Error::Feature_Unimplemented
		//          and Error::Feature_Unavailable.
		/// @remark @x11 @wayland Requires a valid application desktop file with the same name
		///                       as the compiled executable. Due to limitations in the Unity Launcher API
		///                       ProgressState::Indeterminate, ProgressState::Error and ProgressState::Paused
		///                       have the same behaviour as ProgressState::Normal.
		///                       The Unity Launcher API is only known to be supported on Unity and
		///                       KDE desktop environments; on other desktop environments this
		///                       function may do nothing.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set progress for.
		/// @param state State of progress to be displayed in the dock or taskbar.
		/// @param progress The amount of completed progress to set.
		/// Valid range is [0.0-1.0].
		/// This parameter is ignored if state is ProgressState::Disabled
		static void PlatformSetWindowProgressIndicator(const InternalWindow& window, ProgressState state, f64 progress);
		/// @brief This function returns the platform-specific scancode of the specified key.
		///
		/// If the key is TRAP::Input::Key::Unknown or does not exist on the keyboard this
		/// method will return -1.
		///
		/// Errors: Possible errors include Error::Invalid_Enum, Error::Platform_Error.
		/// @threadsafe
		/// @param key Key to get scancode for.
		/// @return The platform-specific scancode of the specified key or -1 if an error occured.
		[[nodiscard]] static i32 PlatformGetKeyScanCode(Input::Key key);
		[[nodiscard]] static i32 PlatformGetKeyScanCodeX11(Input::Key key);
		[[nodiscard]] static i32 PlatformGetKeyScanCodeWayland(Input::Key key);
		/// @brief This function returns a string representation for the platform-specific scancode.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafe
		/// @param scanCode Scancode to get string representation for.
		/// @return String representation of scancode or nullptr if an error occured.
		[[nodiscard]] static std::optional<std::string> PlatformGetScanCodeName(i32 scanCode);
		[[nodiscard]] static std::optional<std::string> PlatformGetScanCodeNameX11(i32 scanCode);
		[[nodiscard]] static std::optional<std::string> PlatformGetScanCodeNameWayland(i32 scanCode);
		/// @brief This function sets the system clipboard to the specified, UTF-8 encoded string.
		///
		/// Errors: Possible errors include Error::Format_Unavailable and Error::Platform_Error.
		/// Pointer lifetime: The specified string is copied before this function returns.
		/// @threadsafety This function must only be called from the main thread.
		/// @param string UTF-8 encoded string to be set for the clipboard.
		static void PlatformSetClipboardString(const std::string& string);
		static void PlatformSetClipboardStringX11(const std::string& string);
		static void PlatformSetClipboardStringWayland(const std::string& string);
		/// @brief This function returns the contents of the system clipboard, if it contains or
		/// is convertible to a UTF-8 encoded string. If the clipboard is empty or if its
		/// content cannot be converted, an empty string is returned and a Error::Format_Unavailable
		/// error is generated.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @return UTF-8 encoded string containing clipboard contents, or empty string if an error occurred.
		[[nodiscard]] static std::string PlatformGetClipboardString();
		[[nodiscard]] static std::string PlatformGetClipboardStringX11();
		[[nodiscard]] static std::string PlatformGetClipboardStringWayland();
		/// @brief This function returns an array of names of Vulkan instance extensions required
		/// by the WindowingAPI for creating Vulkan surface for WindowingAPI windows.
		/// If successful, the list will always contain VK_KHR_surface, so if you don't
		/// require any additional extensions you can pass this list directly to the
		/// VkInstanceCreateInfo struct.
		///
		/// If Vulkan is not available on the machine, this function return an array with
		/// empty strings and generates a Error::API_Unavailable error.
		/// Call WindowingAPI::VulkanSupported to check whether Vulkan is at least
		/// minimally available.
		///
		/// If Vulkan is available but no set of extensions allowing window surface creation
		/// was found, this function returns an array with empty strings. You may still use
		/// Vulkan for off-screen rendering and compute work.
		///
		/// Errors: Possible errors include Error::API_Unavailable.
		/// @remark: Additional extensions may be required by future versions of the WindowingAPI.
		///          You should check if any extensions you wish to enable are already in the
		///          returned array, as it is an error to specify an extension more than once
		///          in the VkInstanceCreateInfo struct.
		/// @threadsafe
		/// @param extensions output array for the required instance extensions.
		static void PlatformGetRequiredInstanceExtensions(std::array<std::string, 2u>& extensions);
		static void PlatformGetRequiredInstanceExtensionsX11(std::array<std::string, 2u>& extensions);
		static void PlatformGetRequiredInstanceExtensionsWayland(std::array<std::string, 2u>& extensions);
		/// @brief Creates a Vulkan surface for the specified window.
		/// This function create a Vulkan surface for the specified window.
		///
		/// If the Vulkan loader or at least one minimally functiona ICD were not found,
		/// this function return VK_ERROR_INITIALIZATION_FAILED and generates a Error::API_Unavilable error.
		/// Call WindowingAPI::VulkanSupported to check whether Vulkan is at least minimally available.
		///
		/// If the required window surface creation instance extensions are not available or if the
		/// specified instance was not created with these extensions enabled, this function
		/// returns VK_ERROR_EXTENSION_NOT_PRESENT and generates a Error::API_Unavailable error.
		/// Call WindowingAPI::GetRequiredInstanceExtensions to check what instance extensions
		/// are required.
		///
		/// The window surface must be destroyed before the specified Vulkan instance.
		/// It is the responsibility of the caller to destroy the window surface.
		/// The WindowingAPI does not destroy it for you.
		/// Call vkDestroySurfaceKHR to destroy the surface.
		///
		/// Errors: Possible errors include Error::API_Unavailable, Error::Platform_Error and Error::Invalid_Value.
		/// @remark: If an error occurs before the creation call is made, the WindowingAPI returns
		///          the Vulkan error code most appropriate for the error. Appropriate use of
		///          WindowingAPI::VulkanSupported and WindowingAPI::GetRequiredInstanceExtensions
		///          should eliminate almost all occurrences of these errors.
		/// @threadsafe
		/// @param instance Vulkan instance.
		/// @param window Internal window for which to create the surface for.
		/// @param allocator Optional allocator.
		/// @param surface Output variable for the new Vulkan surface.
		/// @return VK_SUCCESS if successful, or a Vulkan error code if an error occurred.
		static VkResult PlatformCreateWindowSurface(VkInstance instance, const InternalWindow& window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		static VkResult PlatformCreateWindowSurfaceX11(VkInstance instance, const InternalWindow& window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		static VkResult PlatformCreateWindowSurfaceWayland(VkInstance instance, const InternalWindow& window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		/// @brief This function maximizes the specified window if it was previously not
		/// maximized. If the window is already maximized, this function does nothing.
		///
		/// If the specified window is a full screen window, this function does nothing.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function may only be called from the main thread.
		/// @param window Internal window to maximize.
		static void PlatformMaximizeWindow(InternalWindow& window);
		static void PlatformMaximizeWindowX11(const InternalWindow& window);
		static void PlatformMaximizeWindowWayland(InternalWindow& window);
		/// @brief This function minimizes (iconifies) the specified window if it was previously
		/// restored. If the window is already minimized (iconified), this function does
		/// nothing.
		///
		/// If the specified window is a full screen window, the original monitor resolution
		/// is restored util the window is restored.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to minimize/iconify.
		static void PlatformMinimizeWindow(const InternalWindow& window);
		static void PlatformMinimizeWindowX11(const InternalWindow& window);
		static void PlatformMinimizeWindowWayland(const InternalWindow& window);
		/// @brief This function requests user attention to the specified window.
		/// On platforms where this is not supported, attention is requested
		/// to the application as a whole.
		///
		/// Once the user has given attention, usually by focusing the window
		/// or application, the system will end the request automatically.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to request user attention for.
		static void PlatformRequestWindowAttention(InternalWindow& window);
		static void PlatformRequestWindowAttentionX11(const InternalWindow& window);
		static void PlatformRequestWindowAttentionWayland(InternalWindow& window);
		/// @brief This function hides the specified window if it was previously visible. If
		/// the window is already hidden or is in full screen mode, this function
		/// does nothing.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to hide.
		static void PlatformHideWindow(InternalWindow& window);
		static void PlatformHideWindowX11(const InternalWindow& window);
		static void PlatformHideWindowWayland(InternalWindow& window);
		/// @brief Restores the specified window.
		/// This function restores the specified window if it was previously
		/// minimized (iconified) or maximized. If the window is already restored,
		/// this function does nothing.
		///
		/// If the specified window is a full screen window, the resolution chosen
		/// for the window is restored on the selected monitor.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to be restored.
		static void PlatformRestoreWindow(InternalWindow& window);
		static void PlatformRestoreWindowX11(const InternalWindow& window);
		static void PlatformRestoreWindowWayland(InternalWindow& window);
		/// @brief Sets the size limits of the specified window.
		///
		/// Wayland: The size limits will not be applied until the window is actually resized, either by the
		///          user or by the compositor.
		/// This function sets the size limits of the content area of the specified window. If the
		/// window is full screen, the size limits only take effect once it is made windowed.
		/// If the window is not resizable, this function does nothing.
		///
		/// The size limits are applied immediately to a windowed mode window and may cause it
		/// to be resized.
		///
		/// The maximum dimensions must be greater than or equal to the minimum dimensions
		/// and all must be greater than zero.
		///
		/// Use -1 to disable to minimum and/or maximum size constraints.
		///
		/// Errors: Possible errors include Error::Invalid_Value and Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set size limits for.
		/// @param minWidth New minimum window width.
		/// @param minHeight New minimum window height.
		/// @param maxWidth New maximum window width.
		/// @param maxHeight New maximum window height.
		static void PlatformSetWindowSizeLimits(InternalWindow& window, i32 minWidth, i32 minHeight,
		                                        i32 maxWidth, i32 maxHeight);
		static void PlatformSetWindowSizeLimitsX11(const InternalWindow& window, i32 minWidth, i32 minHeight,
		                                        i32 maxWidth, i32 maxHeight);
		static void PlatformSetWindowSizeLimitsWayland(InternalWindow& window, i32 minWidth, i32 minHeight,
		                                        i32 maxWidth, i32 maxHeight);
		/// @brief Sets the aspect ratio of the specified window.
		///
		/// Wayland: The aspect ratio will not be applied until the window is actually resized, either
		///          by the user or by the compositor.
		///
		/// This function sets the required aspect ratio of the content area of the specified window.
		/// If the window if full screen, the aspect ratio only takes effect once it is made windowed.
		/// If the window is not resizable, this function does nothing.
		///
		/// The aspect ratio is specified as a numerator and a denominator and both values must be
		/// greater than 0. For example, the common 16:9 aspect ratio is specified as 16 and 9, respectively.
		///
		/// If the numerator and denominator are set to -1 then the aspect ratio limit gets disabled.
		///
		/// The aspect ratio is applied immediately to a windowed mode window and may cause it to be resized.
		///
		/// @note If you set the size limits and an aspect ratio that conflict, the results are undefined.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value and
		///         Error::Platform_Error.
		/// @threadsafety This function must only be called from the main thread.
		/// @param window Internal window to set aspect ratio for.
		/// @param numerator Numerator of the desired aspect ratio, or -1.
		/// @param denominator Denominator of the desired aspect ratio, or -1.
		static void PlatformSetWindowAspectRatio(InternalWindow& window, i32 numerator, i32 denominator);
		static void PlatformSetWindowAspectRatioX11(const InternalWindow& window, i32 numerator, i32 denominator);
		static void PlatformSetWindowAspectRatioWayland(InternalWindow& window, i32 numerator, i32 denominator);
		/// @brief Enable/Disable drag and drop feature for the specified window.
		/// @param window Internal window for which to set drag and drop.
		/// @param value Whether to enable or disable drag and drop.
		static void PlatformSetDragAndDrop(InternalWindow& window, bool value);
		static void PlatformSetDragAndDropX11(const InternalWindow& window, bool value);
		static constexpr void PlatformSetDragAndDropWayland(InternalWindow& window, bool value);
		//-------------------------------------------------------------------------------------------------------------------//
		//Single Platform Functions------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
		/// @brief Retrieves the available modes for the specified monitor.
		/// @param monitor Internal monitor to refresh video modes for.
		/// @return True if video modes were successfully updated, false otherwise.
		[[nodiscard]] static bool RefreshVideoModes(InternalMonitor& monitor);
		/// @brief Initialize Vulkan API for further use.
		/// @param mode Mode to use.
		/// @return True when successfully initialized, false otherwise.
		[[nodiscard]] static bool InitVulkan(u32 mode);
		/// @brief Splits a color depth into red, green and blue bit depths.
		/// @param bpp Bits per pixel depth to split.
		/// @param red Output variable for the red channel.
		/// @param green Output variable for the green channel.
		/// @param blue Output variable for the blue channel.
		static constexpr void SplitBPP(i32 bpp, i32& red, i32& green, i32& blue);
		/// @brief Make the specified window and its video mode active on its monitor.
		/// @param window Internal window to make active on the given monitor.
		static void AcquireMonitor(InternalWindow& window);
		/// @brief Make the specified window active on its monitor.
		/// @param window Internal window to make active on the given monitor.
		static void AcquireMonitorBorderless(InternalWindow& window);
		/// @brief Remove the window and restore the original video mode.
		/// @param window Internal window which to release from current acquired monitor.
		static void ReleaseMonitor(const InternalWindow& window);
		/// @brief Lexically compare video modes, used for sorting.
		/// @param fm Internal video mode to sort.
		/// @param sm Internal video mode to sort.
		/// @return Index for the sorted internal video mode.
		[[nodiscard]] static constexpr bool CompareVideoModes(const InternalVideoMode& fm, const InternalVideoMode& sm);
		/// @brief Updates the cursor image according to its cursor mode.
		/// @param window Internal window to update.
		static void UpdateCursorImage(const InternalWindow& window);
		/// @brief Exit disabled cursor mode for the specified window.
		/// @param window Internal window for which to enable cursor.
		static void EnableCursor(InternalWindow& window);
		/// @brief Apply disabled cursor mode to a focused window.
		/// @param window Internal window for which to disable cursor.
		static void DisableCursor(InternalWindow& window);
		/// @brief Enables raw messages for the mouse for the specified window.
		/// @param window Internal window for which to enable raw mouse motion.
		static void EnableRawMouseMotion(const InternalWindow& window);
		/// @brief Disables raw messages for the mouse.
		/// @param window Internal window for which to disable raw mouse motion.
		static void DisableRawMouseMotion(const InternalWindow& window);
		/// @brief Get a string representation of a VkResult.
		/// @param result VkResult to convert.
		/// @return Description of the given VkResult.
		[[nodiscard]] static constexpr std::string GetVulkanResultString(VkResult result);
		/// @brief Notifies shared code of a cursor motion event.
		/// The position is specified in content area relative screen coordinates.
		/// @param window Internal window which is meant.
		/// @param xPos X position of the cursor.
		/// @param yPos Y position of the cursor.
		static void InputCursorPos(InternalWindow& window, f64 xPos, f64 yPos);
		/// @brief Notifies shared code of a physical key event.
		/// @param window Internal window which is meant.
		/// @param key Key that is pressed or released.
		/// @param scancode Scan code of the key which is pressed or released.
		/// @param state State of the key (pressed, repeated or released).
		static void InputKey(InternalWindow& window, Input::Key key, i32 scancode, Input::KeyState state);
		/// @brief Notifies shared code of a Unicode codepoint input event.
		/// The 'plain' parameter determines whether to emit a regular character event.
		/// @param window Internal window which is meant.
		/// @param codePoint Code point of the typed char.
		static constexpr void InputChar(const InternalWindow& window, u32 codePoint);
		/// @brief Notifies shared code of a mouse button click event.
		/// @param window Internal window which is meant.
		/// @param button Mouse button that is pressed or released.
		/// @param state Mouse button state (pressed or released).
		static void InputMouseClick(InternalWindow& window, Input::MouseButton button, TRAP::Input::KeyState state);
		/// @brief Notifies shared code of a scroll event.
		/// @param window Internal window which is meant.
		/// @param xOffset X offset of the scroll wheel.
		/// @param yOffset Y offset of the scroll wheel.
		static void InputScroll(const InternalWindow& window, f64 xOffset, f64 yOffset);
		/// @brief Notified shared code of a cursor enter/leave event.
		/// @param window Internal window which is meant.
		/// @param entered Whether the cursor entered or leaved the window.
		static constexpr void InputCursorEnter(const InternalWindow& window, bool entered);
		/// @brief Notifies shared code that a window framebuffer has been resized.
		/// The size is specified in pixels.
		/// @param window Internal window which is meant.
		/// @param width New framebuffer width.
		/// @param height New framebuffer height.
		static void InputFrameBufferSize(const InternalWindow& window, i32 width, i32 height);
		/// @brief Notifies shared code that a window has been resized.
		/// The size is specified in screen coordinates.
		/// @param window Internal window which is meant.
		/// @param width New window width.
		/// @param height New window height.
		static void InputWindowSize(const InternalWindow& window, i32 width, i32 height);
		/// @brief Notifies shared code that a window has been minimized.
		/// @param window Internal window which is meant.
		/// @param restored Whether window was minimized or restored.
		static constexpr void InputWindowMinimize(const InternalWindow& window, bool restored);
		/// @brief Notifies shared code that a window has been maximized.
		/// @param window Internal window which is meant.
		/// @param restored Whether window was maximized or restored.
		static constexpr void InputWindowMaximize(const InternalWindow& window, bool restored);
		/// @brief Notifies shared code that a window has moved.
		/// The position is specified in content area relative screen coordinates.
		/// @param window Internal window which is meant.
		/// @param xPos X position of the window.
		/// @param yPos Y position of the window.
		static constexpr void InputWindowPos(const InternalWindow& window, i32 xPos, i32 yPos);
		/// @brief Notifies shared code that the user wishes to close a window.
		/// @param window Internal window which is meant.
		static constexpr void InputWindowCloseRequest(InternalWindow& window);
		/// @brief Notifies shared code of files or directories dropped on a window.
		/// @param window Internal window which is meant.
		/// @param paths All dropped paths.
		static constexpr void InputDrop(const InternalWindow& window, const std::vector<std::string>& paths);
		/// @brief Notifies shared code that a window has lost or received input focus.
		/// @param window Internal window which is meant.
		/// @param focused Whether window is focused or not.
		static void InputWindowFocus(InternalWindow& window, bool focused);
		/// @brief Notifies shared code of a keyboard layout change.
		static void InputKeyboardLayout();
		/// @brief Chooses the video mode most closely matching the desired one.
		/// @param monitor Internal monitor to query.
		/// @param desired Desired internal video mode.
		/// @return Pointer to closest matching internal video mode from the desired one or empty optional on error.
		[[nodiscard]] static std::optional<InternalVideoMode> ChooseVideoMode(InternalMonitor& monitor, const InternalVideoMode& desired);
		/// @brief Notifies shared code of a monitor connection or disconnection.
		/// @param monitor Internal monitor that was connected or disconnected.
		/// @param connected Whether the monitor was connected or disconnected.
		/// @param placement Placement of the monitor.
		static void InputMonitor(Scope<InternalMonitor> monitor, bool connected, u32 placement);
		/// @brief Notifies shared code of a monitor connection or disconnection.
		/// @param monitorIndex Index of the monitor which got disconnected.
		/// @param placement Placement of the monitor.
		static void InputMonitorDisconnect(u32 monitorIndex, u32 placement);
		/// @brief Sets the cursor clip rect to the window content area.
		/// @param window Internal window for which to capture the cursor.
		static void CaptureCursor(InternalWindow& window);
		/// @brief Disables clip cursor
		static void ReleaseCursor();
		/// @brief Notifies shared code that a window content scale has changed.
		/// The scale is specified as the ratio between the current and default DPI.
		/// @param window Internal window which is meant.
		/// @param xScale New x content scale.
		/// @param yScale New y content scale.
		static void InputWindowContentScale(const InternalWindow& window, f32 xScale, f32 yScale);

		//-------//
		//Windows//
		//-------//
#ifdef TRAP_PLATFORM_WINDOWS
		/// @brief Load necessary libraries (DLLs).
		/// @return True if loading of libraries was successful, false otherwise.
		[[nodiscard]] static bool LoadLibraries();
		/// @brief Unload used libraries (DLLs).
		static void FreeLibraries();
		/// @brief Reports the specified error, appending information about the last Win32 error.
		/// @param error Error that occurred.
		/// @param description Description of the error.
		static void InputErrorWin32(Error error, const std::string& description);
		/// @brief Updates key names according to the current keyboard layout.
		static void UpdateKeyNamesWin32();
		/// @brief Window callback function (handles window messages).
		/// @param hwnd Window handle.
		/// @param uMsg Message.
		/// @param wParam Message parameter.
		/// @param lParam Message parameter.
		/// @return True if the message was handled, false otherwise.
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		/// @brief Window callback function (handles window messages).
		/// @param hwnd Window handle.
		/// @param uMsg Message.
		/// @param wParam Message parameter.
		/// @param lParam Message parameter.
		/// @return True if the message was handled, false otherwise.
		static LRESULT CALLBACK HelperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		/// @brief Window message loop using fiber to allow for window updates while resizing/moving a window.
		/// This function only polls one time before giving control back to the main thread/fiber.
		/// The message pumping is simplified to only honor window close requests.
		/// @param lpFiberParameter User data pointer.
		static void CALLBACK MessageFiberProc(LPVOID lpFiberParameter);
		/// @brief Callback for EnumDisplayMonitors in CreateMonitor.
		/// @param handle Monitor handle.
		/// @param dc Monitor device context.
		/// @param rect Monitor rectangle.
		/// @param data User data.
		/// @return True to continue enumeration, false to stop.
		static BOOL CALLBACK MonitorCallback(HMONITOR handle, HDC dc, RECT* rect, LPARAM data);
		/// @brief Create monitor from an adapter and (optionally) a display.
		/// @param adapter Adapter to create monitor from.
		/// @param display Display to create monitor from.
		/// @return New Internal monitor.
		[[nodiscard]] static Scope<InternalMonitor> CreateMonitor(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW* display);
		/// @brief Poll for changes in the set of connected monitors.
		static void PollMonitorsWin32();
		/// @brief Fit the Internal window to the monitor.
		/// @param window Internal window to fit.
		static void FitToMonitor(const InternalWindow& window);
		/// @brief Change the current video mode.
		/// @param monitor Internal monitor to set internal video mode for.
		/// @param desired Desired internal video mode for the monitor.
		static void SetVideoModeWin32(InternalMonitor& monitor, const InternalVideoMode& desired);
		/// @brief Retrieve the content scale of the given monitor.
		/// @param handle Monitor handle.
		/// @param xScale X monitor content scale.
		/// @param yScale Y monitor content scale.
		static void GetMonitorContentScaleWin32(HMONITOR handle, f32& xScale, f32& yScale);
		/// @brief Update the theme of the given window.
		/// @param hWnd Window to update theme for.
		static void UpdateTheme(HWND hWnd);
		/// @brief Returns the window style for the specified window.
		/// @param window Internal window to query.
		/// @return Window style as a DWORD.
		[[nodiscard]] static constexpr DWORD GetWindowStyle(const InternalWindow& window);
		/// @brief Returns the extended window style for the specified window.
		/// @param window Internal window to query.
		/// @return Window Ex style as a DWORD.
		[[nodiscard]] static constexpr DWORD GetWindowExStyle(const InternalWindow& window);
		/// @brief Returns whether the cursor is in the content area of the specified window.
		/// @param window Internal window to check
		/// @return True if cursor is inside the given windows content area, false otherwise.
		[[nodiscard]] static bool CursorInContentArea(const InternalWindow& window);
		/// @brief Creates an RGBA icon or cursor,
		/// @param image Image to use.
		/// @param xHot Center x coordinate of the image.
		/// @param yHot Center y coordinate of the image.
		/// @param icon Whether it is an icon or a cursor.
		/// @return Handle to the icon.
		[[nodiscard]] static HICON CreateIcon(const Image& image, u32 xHot, u32 yHot, bool icon);
		/// @brief Enfore the content area aspect ratio based on which edge is being dragged.
		/// @param window Internal window to enforce aspect ratio for.
		/// @param edge Window edge being dragged.
		/// @param area Content area.
		static void ApplyAspectRatio(const InternalWindow& window, i32 edge, RECT& area);
		/// @brief Update native window styles to match attributes.
		/// @param window Internal window to update window style for.
		static void UpdateWindowStyles(const InternalWindow& window);
		/// @brief Creates a dummy window for behind-the-scenes work.
		/// @return True if creation was successful, false otherwise.
		[[nodiscard]] static bool CreateHelperWindow();
		/// @brief Creates the TRAP window.
		/// @param window Internal window to create.
		/// @param WNDConfig Window configuration.
		/// @return True if creation was successful, false otherwise.
		[[nodiscard]] static bool CreateNativeWindow(InternalWindow& window,
			                                         const WindowConfig& WNDConfig);
		/// @brief Manually maximize the window, for when SW_MAXIMIZE cannot be used.
		/// @param window Internal window to maximize manually.
		static void MaximizeWindowManually(const InternalWindow& window);
		/// @brief Retrieve the Win32 HInstance for the application.
		/// @return HInstance.
		[[nodiscard]] static HINSTANCE GetWin32HInstance();
		/// @brief Set the Accessibility Shortcut Keys state
		/// @param allowKeys Disable or enable (restore) accessibility keys.
		static void SetAccessibilityShortcutKeys(const bool allowKeys);
		/// @brief Retrieve a more accurate name for the monitor.
		/// @param deviceName Monitor name from MONITORINFOEXW.
		/// @return More accurate monitor name on success, empty optional otherwise.
		static std::optional<std::string> GetAccurateMonitorName(const std::wstring_view deviceName);

		friend bool TRAP::Input::InitController();
		//------------------//
		//Linux(X11/Wayland)//
		//------------------//
#elif defined(TRAP_PLATFORM_LINUX)
		static void UpdateTaskbarProgressDBusPOSIX(bool progressVisible, f64 progressValue);

		/// @brief Calculates the refresh rate, in Hz, from the specified RandR mode info.
		/// @param mi RandR mode info.
		/// @return Refresh rate on success, empty optional otherwise.
		[[nodiscard]] static constexpr std::optional<f64> CalculateRefreshRate(const XRRModeInfo& mi);
		/// @brief Create InternalVideoMode from RandR mode info.
		/// @param mi RandR mode info.
		/// @param ci RandR CRTC info.
		/// @return Newly created InternalVideoMode on success, empty optional otherwise.
		[[nodiscard]] static constexpr std::optional<InternalVideoMode> VideoModeFromModeInfo(const XRRModeInfo& mi, const XRRCrtcInfo& ci);
		/// @brief Sends an EWMH or ICCCM event to the window manager.
		/// @param window Internal window to send event for.
		/// @param type Event type.
		/// @param a Event data a.
		/// @param b Event data b.
		/// @param c Event data c.
		/// @param d Event data d.
		/// @param e Event data e.
		static void SendEventToWM(const InternalWindow& window, Atom type, i64 a, i64 b, i64 c,
		                          i64 d, i64 e);
		/// @brief Wait for data to arrive on any of the specified file descriptors
		/// @param fds Array of file descriptors to wait for.
		/// @param timeout Time out in seconds.
		/// @return Number of file descriptors with data.
		[[nodiscard]] static bool PollPOSIX(std::span<pollfd> fds, f64* timeout);
		/// @brief Wait for event data to arrive on the X11 display socket.
		/// This avoids blocking other threads via the per-display Xlib lock.
		/// @param timeout Time out in seconds.
		/// @return True if data was received, false otherwise.
		static bool WaitForX11Event(f64* timeout);
		/// @brief Wait for event data to arrive on any event file descriptor
		/// This avoids blocking other threads via the per-display Xlib lock
		/// that also covers GLX functions.
		/// @param timeout Optional: Max time in seconds to wait for.
		/// @return True on success, false otherwise.
		static bool WaitForAnyEvent(f64* timeout);
		/// @brief Writes a byte to the empty event pipe
		static void WriteEmptyEvent();
		/// @brief Drains available data from the empty event pipe
		static void DrainEmptyEvents();
		/// @brief Set event flags for file descriptor.
		/// @param fileDescriptor File descriptor to set flags for.
		/// @return True on success, false otherwise.
		[[nodiscard]] static bool SetEventFlags(i32 fileDescriptor);
		/// @brief Create the pipe for empty events without assuming the OS has pipe2(2)
		/// @return True if the pipe was created, false otherwise.
		[[nodiscard]] static bool CreateEmptyEventPipe();
		/// @brief Retrieve a single window property of the specified type.
		/// @param window X11 window.
		/// @param property Property to retrieve.
		/// @param type Property type.
		/// @param value Output property value.
		/// @return Amount of bytes retrieved.
		[[nodiscard]] static u64 GetWindowPropertyX11(::Window window, Atom property, Atom type, u8** value);
		/// @brief Updates the normal hints according to the window settings.
		/// @param window Internal window to update hints for.
		/// @param width Width of the window.
		/// @param height Height of the window.
		static void UpdateNormalHints(const InternalWindow& window, i32 width, i32 height);
		/// @brief Waits until a VisibilityNotify event arrives for the specified window or the timeout period elapses.
		/// @param window Internal window to wait for.
		/// @return True if a VisibilityNotify event was received, false otherwise.
		static bool WaitForVisibilityNotify(const InternalWindow& window);
		/// @brief Updates the full screen status of the window.
		/// @param window Internal window to update window mode for.
		static void UpdateWindowMode(InternalWindow& window);
		/// @brief Returns the mode info for a RandR mode XID.
		/// @param sr RandR screen resource.
		/// @param id RandR mode XID.
		/// @return RandR mode info.
		[[nodiscard]] static constexpr const XRRModeInfo* GetModeInfo(const XRRScreenResources& sr, RRMode id);
		/// @brief Retrieve system content scale via folklore heuristics.
		/// @param xScale Output variable for x system content scale.
		/// @param yScale Output variable for y system content scale.
		static void GetSystemContentScale(f32& xScale, f32& yScale);
		/// @brief Look for and initialize supported X11 extensions.
		/// @return True on success, false otherwise.
		[[nodiscard]] static bool InitExtensions();
		/// @brief Check whether the running window manager is EMWH-compliant.
		static void DetectEWMH();
		/// @brief Sets the X error handler callback.
		static void GrabErrorHandlerX11();
		/// @brief X error handler.
		/// @param display X11 display.
		/// @param event X11 error event.
		/// @return True if the error was handled, false otherwise.
		[[nodiscard]] static i32 ErrorHandler(Display* display, XErrorEvent* event);
		/// @brief Clears the X error handler callback.
		static void ReleaseErrorHandlerX11();
		/// @brief Check whether the specified atom is supported.
		/// @param supportedAtoms List of supported atoms.
		/// @param atomName Atom to check.
		/// @return Atom on success, empty optional otherwise.
		[[nodiscard]] static std::optional<Atom> GetAtomIfSupported(const std::span<const Atom> supportedAtoms, const std::string& atomName);
		/// @brief Create a blank cursor for hidden and disabled cursor modes.
		/// @return Newly created Cursor on success, empty optional otherwise.
		[[nodiscard]] static std::optional<Cursor> CreateHiddenCursor();
		/// @brief Check whether the IM has a usable style.
		/// @return True if an input method was found, false otherwise.
		[[nodiscard]] static bool HasUsableInputMethodStyle();
		/// @brief Input method destroy callback.
		/// @param im Input method.
		/// @param clientData User data.
		/// @param callData Input method call data.
		static void InputMethodDestroyCallback(XIM im, XPointer clientData, XPointer callData);
		/// @brief Input method instantiate callback.
		/// @param display X11 display.
		/// @param clientData User data.
		/// @param callData Input method call data.
		static void InputMethodInstantiateCallback(Display* display, XPointer clientData, XPointer callData);
		/// @brief Poll for changes in the set of connected monitors.
		static void PollMonitorsX11();
		/// @brief Returns whether the event is a selection event.
		/// @param display X11 display.
		/// @param event X11 event.
		/// @param ptr Pointer to the event.
		/// @return True if the event is a selection event, false otherwise.
		[[nodiscard]] static i32 IsSelectionEvent(Display* display, XEvent* event, XPointer ptr);
		/// @brief Set the specified property to the selection converted to the requested target.
		/// @param request Selection request.
		/// @return Atom on success, None Atom otherwise.
		[[nodiscard]] static Atom WriteTargetToProperty(const XSelectionRequestEvent& request);
		/// @brief Handles a selection request.
		/// @param event Selection request event.
		static void HandleSelectionRequest(const XEvent& event);
		/// @brief Push contents of our selection to clipboard manager.
		static void PushSelectionToManagerX11();
		/// @brief Create X11 input context.
		/// @param window Internal window to create input context for.
		static void CreateInputContextX11(InternalWindow& window);
		/// @brief Check whether the specified visual is transparent.
		/// @param visual X11 visual.
		/// @return True if the visual is a transparent visual.
		[[nodiscard]] static bool IsVisualTransparentX11(const Visual& visual);
		/// @brief Create the X11 window (and its colormap).
		/// @param window Internal window to create window for.
		/// @param WNDConfig Window configuration.
		/// @param visual X11 visual.
		/// @param depth Window depth.
		/// @return True on success, false otherwise.
		[[nodiscard]] static bool CreateNativeWindow(InternalWindow& window, WindowConfig& WNDConfig, Visual& visual,
		                                             i32 depth);
		/// @brief Creates a native cursor object from the specified image and hotspot.
		/// @param image Image to use.
		/// @param xHotSpot X center coordinate of the given image.
		/// @param yHotSpot Y center coordinate of the given image.
		/// @return Newly created cursor on success, empty optional otherwise.
		[[nodiscard]] static std::optional<Cursor> CreateCursorX11(const TRAP::Image& image, u32 xHotSpot, u32 yHotSpot);
		/// @brief Returns whether the window is iconified/minimized
		/// @param window Internal window to check.
		/// @return Window state.
		[[nodiscard]] static i32 GetWindowState(const InternalWindow& window);
		/// @brief Convert XKB KeySym to Unicode.
		/// @param keysym XKB KeySym.
		/// @return Unicode character on success, empty optional otherwise.
		[[nodiscard]] static constexpr std::optional<u32> KeySymToUnicode(u32 keySym);
		/// @brief Return selection string from specified selection.
		/// @param selection Selection to retrieve.
		/// @return Selection string.
		[[nodiscard]] static std::string GetSelectionString(Atom selection);
		/// @brief Returns whether it is a property event for the specified selection transfer.
		/// @param display X11 display.
		/// @param event X11 event.
		/// @param ptr Pointer to the event.
		/// @return True if the event is a property new value event for the specified selection, false otherwise.
		[[nodiscard]] static i32 IsSelPropNewValueNotify(Display* display, XEvent* event, XPointer ptr);
		/// @brief Reports the specified error, appending information about the last X error.
		/// @param error Error code.
		/// @param message Description of error.
		static void InputErrorX11(Error error, const std::string& message);
		/// @brief Process the specified X event.
		/// @param event X event.
		static void ProcessEvent(XEvent& event);
		/// @brief Translates an X11 key code to a TRAP key token.
		/// @param scanCode X11 key code.
		/// @return Translated TRAP::Input::Key.
		[[nodiscard]] static constexpr Input::Key TranslateKey(i32 scanCode);
#ifdef X_HAVE_UTF8_STRING
		/// @brief Decode a Unicode code point from a UTF-8 stream.
		/// @param s UTF-8 stream.
		/// @return Unicode code point.
		[[nodiscard]] static u32 DecodeUTF8(const char** str);
#endif /*X_HAVE_UTF8_STRING*/
		/// @brief Splits and translates a text/uri-list into separate file paths.
		/// @param text Text to split.
		/// @return Vector of file paths.
		[[nodiscard]] static std::vector<std::string> ParseUriList(std::string_view text);
		/// @brief Set the current video mode for the specified monitor.
		/// @param monitor Monitor to set video mode for.
		/// @param desired Desired video mode.
		static void SetVideoModeX11(InternalMonitor& monitor, const InternalVideoMode& desired);
		/// @brief Restore the saved(original) video mode for the specified monitor.
		/// @param monitor Monitor to restore video mode for.
		static void RestoreVideoModeX11(InternalMonitor& monitor);
		/// @brief Allocates and returns a monitor object with the specified name.
		/// @param name Monitor name.
		/// @return Newly created monitor object.
		[[nodiscard]] static Scope<InternalMonitor> CreateMonitor(std::string name);
		/// @brief Creates a dummy window for behind-the-scenes work.
		/// @return Newly created X11 Window.
		[[nodiscard]] static ::Window CreateHelperWindow();
		/// @brief Clear its handle when the input context has been destroyed.
		/// @param ic Input context.
		/// @param clientData Client data.
		/// @param callData Call data.
		static void InputContextDestroyCallback(XIC ic, XPointer clientData, XPointer callData);
		/// @brief Retrieve the current keyboard layout name.
		/// @return Current keyboard layout name on success, empty optional otherwise.
		[[nodiscard]] static std::optional<std::string> GetLinuxKeyboardLayoutName();
		[[nodiscard]] static std::optional<std::string> GetLinuxKeyboardLayoutNameX11();
		[[nodiscard]] static std::optional<std::string> GetLinuxKeyboardLayoutNameWayland();

		/// @brief Callback function for Wayland registry notifying the removal of global objects.
		/// @param userData Pointer to user provided data.
		/// @param registry Affected registry object.
		/// @param name Unique numeric name of the object.
		static void RegistryHandleGlobalRemove(void* userData, wl_registry* registry, u32 name);
		/// @brief Callback function for Wayland registry notifying the creation of global objects.
		/// @param userData Pointer to user provided data.
		/// @param registry Affected registry object.
		/// @param name Unique numeric name of the object.
		/// @param interface Interface implemented by the object.
		/// @param version Interface version.
		static void RegistryHandleGlobal(void* userData, wl_registry* registry, u32 name, const char* interface, u32 version);
		static constexpr wl_registry_listener RegistryListener
		{
			.global = RegistryHandleGlobal,
			.global_remove = RegistryHandleGlobalRemove
		};

		/// @brief Callback function for LibDecor errors.
		/// @param context Handle to the affected LibDecor context.
		/// @param error LibDecor error code.
		/// @param message Description of the error.
		static void LibDecorHandleError(libdecor* context, libdecor_error error, const char* message);
		static constexpr libdecor_interface LibDecorInterface
		{
			.error = LibDecorHandleError,
			.reserved0 = nullptr,
			.reserved1 = nullptr,
			.reserved2 = nullptr,
			.reserved3 = nullptr,
			.reserved4 = nullptr,
			.reserved5 = nullptr,
			.reserved6 = nullptr,
			.reserved7 = nullptr,
			.reserved8 = nullptr,
			.reserved9 = nullptr
		};

		static void LibDecorReadyCallback(void* userData, wl_callback* callback, u32 time);
		static constexpr wl_callback_listener LibDecorReadyListener =
		{
			.done = LibDecorReadyCallback
		};

		/// @brief Callback function for Wayland to check if the client is still alive.
		/// @param userData Pointer to user provided data.
		/// @param wmBase Affected xdg_wm_base object.
		/// @param serial Serial to pass back to the compositor via "pong" request.
		static void WMBaseHandlePing(void* userData, xdg_wm_base* wmBase, u32 serial);
		static constexpr xdg_wm_base_listener WMBaseListener
		{
			.ping = WMBaseHandlePing
		};

		/// @brief Callback function for Wayland to retrieve the capabilities of
		/// a "seat" (a group of keyboards, pointers and touch devices).
		/// @param userData Pointer to user provided data.
		/// @param seat Affected wl_seat object.
		/// @param caps Capabilities of the wl_seat object.
		static void SeatHandleCapabilities(void* userData, wl_seat* seat, u32 caps);
		/// @brief Callback function for Wayland to retrieve the unique identifier of
		/// a "seat" (a group of keyboards, pointers and touch devices).
		/// @param userData Pointer to user provided data.
		/// @param seat Affected wl_seat object.
		/// @param name Unique identifier of the wl_seat object.
		static constexpr void SeatHandleName(void* userData, wl_seat* seat, const char* name);
		static constexpr wl_seat_listener SeatListener
		{
			.capabilities = SeatHandleCapabilities,
			.name = SeatHandleName
		};

		/// @brief Callback function for Wayland notifying that the pointer
		/// is focused on a certain surface.
		/// @param userData Pointer to user provided data.
		/// @param pointer Affected wl_pointer.
		/// @param serial Unique identifier of the pointer enter event.
		/// @param surface Surface that got focused.
		/// @param sXPos X position of the pointer on the surface.
		/// @param sYPos Y position of the pointer on the surface.
		static void PointerHandleEnter(void* userData, wl_pointer* pointer, u32 serial, wl_surface* surface, wl_fixed_t sXPos, wl_fixed_t sYPos);
		/// @brief Callback function for Wayland notifying that the pointer
		/// is unfocused on a certain surface.
		/// @param userData Pointer to user provided data.
		/// @param pointer Affected wl_pointer.
		/// @param serial Unique identifier of the pointer leave event.
		/// @param surface Surface that got unfocused.
		static void PointerHandleLeave(void* userData, wl_pointer* pointer, u32 serial, wl_surface* surface);
		/// @brief Callback function for Wayland notifying that the pointer
		/// has changed position.
		/// @param userData Pointer to user provided data.
		/// @param pointer Affected wl_pointer.
		/// @param time Timestamp of the event.
		/// @param sXPos New X position on the surface.
		/// @param sYPos New Y position on the surface.
		static void PointerHandleMotion(void* userData, wl_pointer* pointer, u32 time, wl_fixed_t sXPos, wl_fixed_t sYPos);
		/// @brief Callback function for Wayland notifying that the pointer
		/// button has been clicked or released.
		/// @param userData Pointer to user provided data.
		/// @param pointer Affected wl_pointer.
		/// @param serial Unique identifier of the pointer button event.
		/// @param time Timestammp of the event.
		/// @param button Affected button.
		/// @param state State of the affected button
		static void PointerHandleButton(void* userData, wl_pointer* pointer, u32 serial, u32 time, u32 button, u32 state);
		/// @brief Callback function for Wayland notifying that the pointer
		/// axis has changed.
		/// @param userData Pointer to user provided data.
		/// @param pointer Affected wl_pointer.
		/// @param time Timestamp of the event.
		/// @param axis Affected axis
		/// @param value Amount of relative movement on the axis.
		static void PointerHandleAxis(void* userData, wl_pointer* pointer, u32 time, u32 axis, wl_fixed_t value);
		static constexpr wl_pointer_listener PointerListener
		{
			.enter = PointerHandleEnter,
			.leave = PointerHandleLeave,
			.motion = PointerHandleMotion,
			.button = PointerHandleButton,
			.axis = PointerHandleAxis,
			.frame = nullptr,
			.axis_source = nullptr,
			.axis_stop = nullptr,
			.axis_discrete = nullptr,
			.axis_value120 = nullptr,
			.axis_relative_direction = nullptr
		};

		/// @brief Callback function for Wayland notifying about the keyboard mapping.
		/// @param userData Pointer to user provided data.
		/// @param keyboard Affected wl_keyboard.
		/// @param format Format of the keyboard mapping.
		/// @param fd Keyboard map file descriptor.
		/// @param size Keyboard map size in bytes.
		static void KeyboardHandleKeymap(void* userData, wl_keyboard* keyboard, u32 format, i32 fd, u32 size);
		/// @brief Callback function for Wayland notifying that the keyboard has
		/// been focused on a certain surface.
		/// @param userData Pointer to user provided data.
		/// @param keyboard Affected wl_keyboard.
		/// @param serial Unique identifier of the keyboard enter event.
		/// @param surface Surface that got focused.
		/// @param keys Currently pressed keys.
		static void KeyboardHandleEnter(void* userData, wl_keyboard* keyboard, u32 serial, wl_surface* surface, wl_array* keys);
		/// @brief Callback function for Wayland notifying that the keyboard has
		/// been unfocused on a certain surface.
		/// @param userData Pointer to user provided data.
		/// @param keyboard Affected wl_keyboard.
		/// @param serial Unique identifier of the keyboard leave event.
		/// @param surface Surface that got unfocused.
		static void KeyboardHandleLeave(void* userData, wl_keyboard* keyboard, u32 serial, wl_surface* surface);
		/// @brief Callback function for Wayland notifying that
		/// a key on the keyboard has changed its state.
		/// @param userData Pointer to user provided data.
		/// @param keyboard Affected wl_keyboard.
		/// @param serial Unique identifier of the keyboard key event.
		/// @param time Timestamp of the event.
		/// @param scanCode Affected key.
		/// @param state State of the key.
		static void KeyboardHandleKey(void* userData, wl_keyboard* keyboard, u32 serial, u32 time, u32 scanCode, u32 state);
		/// @brief Callback function for Wayland notifying that
		/// the keyboard modifiers have changed.
		/// @param userData Pointer to user provided data.
		/// @param keyboard Affected wl_keyboard.
		/// @param serial Unique identifier of the keyboard modifiers event.
		/// @param modsDepressed Depressed modifiers.
		/// @param modsLatched Latched modifiers.
		/// @param modsLocked Locked modifiers.
		/// @param group Keyboard layout.
		static void KeyboardHandleModifiers(void* userData, wl_keyboard* keyboard, u32 serial, u32 modsDepressed, u32 modsLatched, u32 modsLocked, u32 group);
#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
		/// @brief Callback function for Wayland notifying about
		/// the keyboard repeat rate and delay.
		/// @param userData Pointer to user provided data.
		/// @param keyboard Affected wl_keyboard.
		/// @param rate Rate of repeating keys in characters per second.
		/// @param delay Delay in milliseconds since key down until repeating starts.
		static void KeyboardHandleRepeatInfo(void* userData, wl_keyboard* keyboard, i32 rate, i32 delay);
#endif /*WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION*/
		static constexpr wl_keyboard_listener KeyboardListener
		{
			.keymap = KeyboardHandleKeymap,
			.enter = KeyboardHandleEnter,
			.leave = KeyboardHandleLeave,
			.key = KeyboardHandleKey,
			.modifiers = KeyboardHandleModifiers,
#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
			.repeat_info = KeyboardHandleRepeatInfo
#endif /*WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION*/
		};

		/// @brief This functions adds a set of custom video modes to the monitor.
		/// This is needed for arbitrary fullscreen video modes as Wayland
		/// doesn't offer a way of querying the monitors available video modes.
		/// @param monitor Monitor to add video modes to.
		static void AddEmulatedVideoModes(InternalMonitor& monitor);

		/// @brief Callback function for Wayland notifying about an
		/// output handles geometry.
		/// @param userData Pointer to user provided data.
		/// @param output Affected output.
		/// @param xPos X position of the monitor within the global compositor space.
		/// @param yPos Y position of the monitor within the global compositor space.
		/// @param physicalWidth Width in millimeters of the output.
		/// @param physicalHeight Height in millimeters of the output.
		/// @param subpixel Subpixel orientationof the output.
		/// @param make Description of the manufacturer.
		/// @param model Description of the model.
		/// @param transform Transform that maps framebuffer to output.
		static void OutputHandleGeometry(void* userData, wl_output* output, i32 xPos, i32 yPos, i32 physicalWidth, i32 physicalHeight, i32 subpixel, const char* make, const char* model, i32 transform);
		/// @brief Callback function for Wayland notifying about an available video mode
		/// for the output.
		/// @param userData Pointer to user provided data.
		/// @param output Affected output.
		/// @param flags Bitfield of mode flags.
		/// @param width Width of the mode.
		/// @param height Height of the mode.
		/// @param refresh Refresh rate of the mode.
		static void OutputHandleMode(void* userData, wl_output* output, u32 flags, i32 width, i32 height, i32 refresh);
		/// @brief Callback function for Wayland notifying that the
		/// output has finished sending events.
		/// @param userData Pointer to user provided data.
		/// @param output Affected output.
		static void OutputHandleDone(void* userData, wl_output* output);
		/// @brief Callback function for Wayland notifying that the
		/// output has a new scaling factor.
		/// @param userData Pointer to user provided data.
		/// @param output Affected output.
		/// @param factor New scaling factor.
		static void OutputHandleScale(void* userData, wl_output* output, i32 factor);
#ifdef WL_OUTPUT_NAME_SINCE_VERSION
		/// @brief Callback function for Wayland notifying about the name of the output.
		/// @param userData Pointer to user provided data.
		/// @param output Affected output.
		/// @param name Name of the output.
		static void OutputHandleName(void* userData, wl_output* output, const char* name);
		/// @brief Callback function for Wayland notifying about the description of the output.
		/// @param userData Pointer to user provided data.
		/// @param output Affected output.
		/// @param description Description of the output.
		static constexpr void OutputHandleDescription(void* userData, wl_output* output, const char* description);
#endif /*WL_OUTPUT_NAME_SINCE_VERSION*/
		static constexpr wl_output_listener OutputListener
		{
			.geometry = OutputHandleGeometry,
			.mode = OutputHandleMode,
			.done = OutputHandleDone,
			.scale = OutputHandleScale,
#ifdef WL_OUTPUT_NAME_SINCE_VERSION
			.name = OutputHandleName,
			.description = OutputHandleDescription
#endif /*WL_OUTPUT_NAME_SINCE_VERSION*/
		};

		/// @brief Callback function for Wayland initiating a new data offer.
		/// @param userData Pointer to user provided data.
		/// @param device Wayland data device object.
		/// @param offer Wayland data offer object.
		static void DataDeviceHandleDataOffer(void* userData, wl_data_device* device, wl_data_offer* offer);
		/// @brief Callback function for Wayland initiating a new drag-and-drop session.
		/// @param userData Pointer to user provided data.
		/// @param device Wayland data device object.
		/// @param serial Unique identifier of the data enter event.
		/// @param surface Affected wl_surface.
		/// @param xPos X position on the surface.
		/// @param yPos Y position on the surface.
		/// @param offer Wayland data offer object.
		static void DataDeviceHandleEnter(void* userData, wl_data_device* device, u32 serial, wl_surface* surface, wl_fixed_t xPos, wl_fixed_t yPos, wl_data_offer* offer);
		/// @brief Callback function for Wayland ending a drag-and-drop session.
		/// @param userData Pointer to user provided data.
		/// @param device Wayland data device object.
		static void DataDeviceHandleLeave(void* userData, wl_data_device* device);
		/// @brief Callback function for Wayland notifying about motion in a drag-and-drop session.
		/// @param userData Pointer to user provided data.
		/// @param device Wayland data device object.
		/// @param time Timestamp of the event.
		/// @param xPos New X position on the surface.
		/// @param yPos New Y position on the surface.
		static constexpr void DataDeviceHandleMotion(void* userData, wl_data_device* device, u32 time, wl_fixed_t xPos, wl_fixed_t yPos);
		/// @brief Callback function for Wayland notifying about a successful drag-and-drop operation.
		/// @param userData Pointer to user provided data.
		/// @param device Wayland data device object.
		static void DataDeviceHandleDrop(void* userData, wl_data_device* device);
		/// @brief Callback function for Wayland notifying about a new drag-and-drop selection.
		/// @param userData Pointer to user provided data.
		/// @param device Wayland data device object.
		/// @param offer Wayland data offer object.
		static void DataDeviceHandleSelection(void* userData, wl_data_device* device, wl_data_offer* offer);
		static constexpr wl_data_device_listener DataDeviceListener
		{
			.data_offer = DataDeviceHandleDataOffer,
			.enter = DataDeviceHandleEnter,
			.leave = DataDeviceHandleLeave,
			.motion = DataDeviceHandleMotion,
			.drop = DataDeviceHandleDrop,
			.selection = DataDeviceHandleSelection
		};

		/// @brief Callback function for Wayland notifying about the mime type of a data offer.
		/// @param userData Pointer to user provided data.
		/// @param offer Wayland data offer object.
		/// @param mimeType Offered mime type.
		static void DataOfferHandleOffer(void* userData, wl_data_offer* offer, const char* mimeType);
		static constexpr wl_data_offer_listener DataOfferListener
		{
			.offer = DataOfferHandleOffer,
			.source_actions = nullptr,
			.action = nullptr
		};

		/// @brief Callback function for Wayland suggesting a surface change.
		/// @param userData Pointer to user provided data.
		/// @param surface Affected surface.
		/// @param serial Unique identifier of the XDG surface configure event.
		static void XDGSurfaceHandleConfigure(void* userData, xdg_surface* surface, u32 serial);
		static constexpr xdg_surface_listener XDGSurfaceListener
		{
			.configure = XDGSurfaceHandleConfigure
		};

		/// @brief Callback function for Wayland suggesting a surface change
		/// for the toplevel surface.
		/// @param userData Pointer to user provided data.
		/// @param topLevel Affected toplevel surface.
		/// @param width Suggested width.
		/// @param height Suggested height.
		/// @param states How to interpret the width and height arguments.
		static void XDGTopLevelHandleConfigure(void* userData, xdg_toplevel* topLevel, i32 width, i32 height, wl_array* states);
		/// @brief Callback function for Wayland notifying that the toplevel surface wants to be closed.
		/// @param userData Pointer to user provided data.
		/// @param topLevel Affected toplevel surface.
		static void XDGTopLevelHandleClose(void* userData, xdg_toplevel* topLevel);
		static constexpr xdg_toplevel_listener XDGTopLevelListener
		{
			.configure = XDGTopLevelHandleConfigure,
			.close = XDGTopLevelHandleClose,
			.configure_bounds = nullptr,
			.wm_capabilities = nullptr
		};

		/// @brief Callback function for Wayland notifying that the XDG activation is done.
		/// @param userData Pointer to user provided data.
		/// @param activationToken XDG activation token object.
		/// @param token Exported activation token.
		static void XDGActivationHandleDone(void* userData, xdg_activation_token_v1* activationToken, const char* token);
		static constexpr xdg_activation_token_v1_listener XDGActivationListener
		{
			.done = XDGActivationHandleDone
		};

		/// @brief Callback function for Wayland suggesting to change the decoration mode.
		/// @param userData Pointer to user provided data.
		/// @param decoration XDG toplevel decoration object.
		/// @param mode Decoration mode.
		static void XDGDecorationHandleConfigure(void* userData, zxdg_toplevel_decoration_v1* decoration, u32 mode);
		static constexpr zxdg_toplevel_decoration_v1_listener XDGDecorationListener
		{
			.configure = XDGDecorationHandleConfigure
		};

		/// @brief Callback function for Wayland notifying that an output entered a surface.
		/// @param userData Pointer to user provided data.
		/// @param surface Affected surface.
		/// @param output Output that entered the surface.
		static void SurfaceHandleEnter(void* userData, wl_surface* surface, wl_output* output);
		/// @brief Callback function for Wayland notifying that an output left a surface.
		/// @param userData Pointer to user provided data.
		/// @param surface Affected surface.
		/// @param output Output that left the surface.
		static void SurfaceHandleLeave(void* userData, wl_surface* surface, wl_output* output);
		static constexpr wl_surface_listener SurfaceListener
		{
			.enter = SurfaceHandleEnter,
			.leave = SurfaceHandleLeave,
			.preferred_buffer_scale = nullptr,
			.preferred_buffer_transform = nullptr
		};

		/// @brief Callback function for Wayland notifying that
		/// pointer confinement has been activated.
		/// @param userData Pointer to user provided data.
		/// @param confinedPointer ZWP confined pointer object.
		static constexpr void ConfinedPointerHandleConfined(void* userData, zwp_confined_pointer_v1* confinedPointer);
		/// @brief Callback function for Wayland notifying that
		/// pointer confinement has been deactivated.
		/// @param userData Pointer to user provided data.
		/// @param confinedPointer ZWP confined pointer object.
		static constexpr void ConfinedPointerHandleUnconfined(void* userData, zwp_confined_pointer_v1* confinedPointer);
		static constexpr zwp_confined_pointer_v1_listener ConfinedPointerListener
		{
			.confined = ConfinedPointerHandleConfined,
			.unconfined = ConfinedPointerHandleUnconfined
		};

		/// @brief Callback function for Wayland notifying that the relative pointer moved.
		/// @param userData Pointer to user provided data.
		/// @param pointer Affected pointer.
		/// @param timeHi High 32 bits of a 64 bit timestamp.
		/// @param timeLo Low 32 bits of a 64 bit timestamp.
		/// @param dx X component of the motion vector.
		/// @param dy Y component of the motion vector.
		/// @param dxUnaccel X component of the unaccelerated motion vector.
		/// @param dyUnaccel Y component of the unaccelerated motion vector.
		static void RelativePointerHandleRelativeMotion(void* userData, zwp_relative_pointer_v1* pointer, u32 timeHi,
		                                                u32 timeLo, wl_fixed_t dx, wl_fixed_t dy, wl_fixed_t dxUnaccel, wl_fixed_t dyUnaccel);
		static constexpr zwp_relative_pointer_v1_listener RelativePointerListener
		{
			.relative_motion = RelativePointerHandleRelativeMotion
		};

		/// @brief Callback function for Wayland notifying that pointer has been locked.
		/// @param userData Pointer to user provided data.
		/// @param lockedPointer ZWP locked pointer object.
		static constexpr void LockedPointerHandleLocked(void* userData, zwp_locked_pointer_v1* lockedPointer);
		/// @brief Callback function for Wayland notifying that pointer has been unlocked.
		/// @param userData Pointer to user provided data.
		/// @param lockedPointer ZWP locked pointer object.
		static constexpr void LockedPointerHandleUnlocked(void* userData, zwp_locked_pointer_v1* lockedPointer);
		static constexpr zwp_locked_pointer_v1_listener LockedPointerListener
		{
			.locked = LockedPointerHandleLocked,
			.unlocked = LockedPointerHandleUnlocked
		};

		/// @brief Callback function for Wayland called when an offered mime type got accepted.
		/// @param userData Pointer to user provided data.
		/// @param source Wayland data source object.
		/// @param mimeType Accepted mime type.
		static void DataSourceHandleTarget(void* userData, wl_data_source* source, const char* mimeType);
		/// @brief Callback function for Wayland notifying about a data request.
		/// @param userData Pointer to user provided data.
		/// @param source Wayland data source object.
		/// @param mimeType Mime type.
		/// @param fd File descriptor for data transfer.
		static void DataSourceHandleSend(void* userData, wl_data_source* source, const char* mimeType, i32 fd);
		/// @brief Callback function for Wayland notifying that a data request got cancelled.
		/// @param userData Pointer to user provided data.
		/// @param source Wayland data source object.
		static void DataSourceHandleCancelled(void* userData, wl_data_source* source);
		static constexpr wl_data_source_listener DataSourceListener
		{
			.target = DataSourceHandleTarget,
			.send = DataSourceHandleSend,
			.cancelled = DataSourceHandleCancelled,
			.dnd_drop_performed = nullptr,
			.dnd_finished = nullptr,
			.action = nullptr
		};

		/// @brief Callback function for Wayland notifying that a new preferred scale exists.
		/// @param userData Pointer to user provided data.
		/// @param fractionalScale
		/// @param preferredScale_8_24 New preferred scale numerator of a fraction with a denominator of 120.
		static void FractionalScaleHandleScaleFactor(void* userData, wp_fractional_scale_v1* fractionalScale, u32 preferredScale_8_24);
		static constexpr wp_fractional_scale_v1_listener FractionalScaleListener =
		{
			.preferred_scale = FractionalScaleHandleScaleFactor
		};

		/// @brief Callback function for Wayland notifying that a new configuration was received.
		/// @param frame LibDecor frame object.
		/// @param config LibDecor configuration object.
		/// @param userData Pointer to user provided data.
		static void LibDecorFrameHandleConfigure(libdecor_frame* frame, libdecor_configuration* config, void* userData);
		/// @brief Callback function for Wayland notifying that window
		/// was requested to be closed by compositor.
		/// @param frame LibDecor frame object.
		/// @param userData Pointer to user provided data.
		static void LibDecorFrameHandleClose(libdecor_frame* frame, void* userData);
		/// @brief Callback function for Wayland notifying about a
		/// request to commit the main surface.
		/// @param frame LibDecor frame object.
		/// @param userData Pointer to user provided data.
		static void LibDecorFrameHandleCommit(libdecor_frame* frame, void* userData);
		/// @brief Callback function for Wayland notifying that any mapped popup
		/// that has grab on the given seat should be dismissed.
		/// @param frame LibDecor frame object.
		/// @param seatName Seat to dismiss popups for.
		/// @param userData Pointer to user provided data.
		static constexpr void LibDecorFrameHandleDismissPopup(libdecor_frame* frame, const char* seatName, void* userData);
		static constexpr libdecor_frame_interface LibDecorFrameInterface
		{
			.configure = LibDecorFrameHandleConfigure,
			.close = LibDecorFrameHandleClose,
			.commit = LibDecorFrameHandleCommit,
			.dismiss_popup = LibDecorFrameHandleDismissPopup,
			.reserved0 = nullptr,
			.reserved1 = nullptr,
			.reserved2 = nullptr,
			.reserved3 = nullptr,
			.reserved4 = nullptr,
			.reserved5 = nullptr,
			.reserved6 = nullptr,
			.reserved7 = nullptr,
			.reserved8 = nullptr,
			.reserved9 = nullptr
		};

		/// @brief This function tries to load the cursor's size and theme.
		/// @return True on success, false otherwise.
		[[nodiscard]] static bool LoadCursorThemeWayland();
		/// @brief Read a Wayland data offer as a string.
		/// @param offer Wayland data offer object.
		/// @param mimeType Mime type of data to receive.
		/// @return Read data on success, empty optional otherwise
		[[nodiscard]] static std::optional<std::string> ReadDataOfferAsString(wl_data_offer& offer, const std::string& mimeType);
		/// @brief Flush the display till it succeeds.
		/// @return True on successful flush, false otherwise.
		static bool FlushDisplay();
		/// @brief Set the given cursor for the window.
		/// @param window Window to set cursor on.
		/// @param name Name of the cursor to set.
		static void SetCursorWayland(const InternalWindow& window, const std::string& name);
		/// @brief Process Wayland text input (calls InputChar()) on the given window.
		/// @param window Window to input text on.
		/// @param scanCode Wayland key.
		static void InputTextWayland(const InternalWindow& window, u32 scanCode);
		/// @brief Compose an XKB key symbol.
		/// @param sym XKB key symbol to compose.
		/// @return Composed XBK key symbol.
		[[nodiscard]] static xkb_keysym_t ComposeSymbol(xkb_keysym_t sym);
		/// @brief Set the draw surface viewport for the window.
		/// This scale the surface from the source resolution the the destination resolution.
		/// @param window Window to set surface viewport for.
		/// @param srcWidth Source width.
		/// @param srcHeight Source height.
		/// @param dstWidth Destination width.
		/// @param dstHeight Destination height.
		static void SetDrawSurfaceViewportWayland(InternalWindow& window, i32 srcWidth, i32 srcHeight, i32 dstWidth, i32 dstHeight);
		/// @brief Unset the draw surface viewport for the window.
		/// @param window Window to unset surface viewport for.
		static void UnsetDrawSurfaceViewport(InternalWindow& window);
		/// @brief Retrieve whether the given window needs a surface viewport or not.
		/// @param window Window to check for.
		/// @return True if the window needs a surface viewport.
		[[nodiscard]] static bool WindowNeedsViewport(const InternalWindow& window);
		/// @brief Update the content scaling of the given window.
		/// @param window Window to update content scaling for.
		static void UpdateContentScaleWayland(InternalWindow& window);
		/// @brief Resize the given window to the current framebuffer size.
		/// @param window Window to resize.
		static void ResizeWindowWayland(InternalWindow& window);
		/// @brief Make the content area (surface) of the given window opaque.
		/// @param window Window to make content area opaque for.
		static void SetContentAreaOpaqueWayland(const InternalWindow& window);
		/// @brief Release the acquired monitor from the given window.
		/// This disables fullscreen for the window if a monitor is acquired.
		/// @param window Window to release monitor from.
		static void ReleaseMonitorWayland(InternalWindow& window);
		/// @brief Whether the enable or disable the idle (screen saver) inhibitor.
		/// @param window Window to set the idle inhibitor for.
		/// @param enable Whether to enable or disable the idle inhibitor.
		static void SetIdleInhibitorWayland(InternalWindow& window, bool enable);
		/// @brief Create the fallback window decoration.
		/// This is used when both XDG server side decorations and libdecor are unavailable.
		/// @param window Window to create decorations for.
		static void CreateFallbackDecorationsWayland(InternalWindow& window);
		/// @brief Create a shared memory buffer with the contents of the given image.
		/// @param image Image to copy into the shared memory buffer.
		/// @return Newly created shared memory buffer on success, nullptr otherwise.
		[[nodiscard]] static wl_buffer* CreateShmBufferWayland(const Image& image);
		/// @brief Create an anonymous file handle with given size in bytes.
		/// @param size Size in bytes for the file handle.
		/// @return File descriptor on success, empty optional otherwise.
		[[nodiscard]] static std::optional<i32> CreateAnonymousFileWayland(off_t size);
		/// @brief Create a unique temporary file
		/// @param tmpName Template filename.
		/// @return File descriptor on success, empty optional otherwise.
		[[nodiscard]] static std::optional<i32> CreateTmpFileCloexec(std::string tmpName);
		/// @brief Create a fallback decoration for the given window.
		/// @param window Window to create decoration for.
		/// @param decoration Data of the decoration.
		/// @param parent Parent surface of the decoration.
		/// @param buffer Buffer to set as content of the decoration surface.
		/// @param xPos X position for the decoration surface.
		/// @param yPos Y position for the decoration surface.
		/// @param width Width for the decoration surface.
		/// @param height Height for the decoration surface.
		static void CreateFallbackDecorationWayland(InternalWindow& window, TRAPDecorationWayland& decoration, wl_surface& parent, wl_buffer& buffer, i32 xPos, i32 yPos, i32 width, i32 height);
		/// @brief Acquire monitor on the given window.
		/// This also enables fullscreen mode for the window.
		/// @param window Window to acquire monitor on.
		static void AcquireMonitorWayland(InternalWindow& window);
		/// @brief Destroy the fallback window decorations on the given window.
		/// @param window Window to destroy fallback decorations for.
		static void DestroyFallbackDecorationsWayland(InternalWindow& window);
		/// @brief Destroy a fallback decoration.
		/// @param decoration Decoration to destroy.
		static void DestroyFallbackDecorationWayland(TRAPDecorationWayland& decoration);
		/// @brief Destroy shell objects on the given window.
		/// This destroys the following objects:
		/// - Fallback window decorations
		/// - LibDecor frame
		/// - XDG decorations
		/// - XDG TopLevel
		/// - XDG Surface
		/// @param window Window to destroy shell objects for.
		static void DestroyShellObjectsWayland(InternalWindow& window);
		/// @brief Create window decorations using LibDecor for the given window.
		/// @param window Window to create decorations for.
		/// @return True on successful window decoration creation, false otherwise.
		[[nodiscard]] static bool CreateLibDecorFrame(InternalWindow& window);
		/// @brief Create XDG shell objects for the given window.
		/// @param window Window to create XDG shell objects for.
		/// @return True on success, false otherwise.
		[[nodiscard]] static bool CreateXDGShellObjectsWayland(InternalWindow& window);
		/// @brief Create shell objects for the given window.
		/// @param window Window to create shell objects for.
		/// @return True on success, false otherwise.
		static bool CreateShellObjectsWayland(InternalWindow& window);
		/// @brief Create the surface for the given window.
		/// @param window Window to create surface for.
		/// @param WNDConfig WindowConfig to use for the surface.
		/// @return True on success, false otherwise.
		[[nodiscard]] static bool CreateNativeSurfaceWayland(InternalWindow& window, const WindowConfig& WNDConfig);
		/// @brief Confine the pointer/cursor to the limits of the given window.
		/// @param window Window to confine pointer to.
		static void ConfinePointerWayland(InternalWindow& window);
		/// @brief Unconfine the pointer/cursor from the limits of the given window.
		/// @param window Window to unconfine pointer from.
		static void UnconfinePointerWayland(InternalWindow& window);
		/// @brief Lock the pointer/cursor to the given window.
		/// @param window Window to lock pointer to.
		static void LockPointerWayland(InternalWindow& window);
		/// @brief Unlock the pointer/cursor from the given window.
		/// @param window Window to unlock pointer from.
		static void UnlockPointerWayland(InternalWindow& window);
		/// @brief Set a custom image as mouse cursor for the given window.
		/// @param window Window to set mouse cursor image on.
		/// @param cursorWayland Data of the cursor to set.
		static void SetCursorImageWayland(const InternalWindow& window, InternalCursor::wayland& cursorWayland);
		/// @brief Wayland event pump.
		/// @param timeout Amount of time to wait for events before timing out.
		/// Using nullptr disabled the timeout functionality.
		static void HandleEventsWayland(f64* timeout);
		/// @brief Increment the cursor image counter of the given window.
		/// This is needed for animated cursor to display the animation.
		/// @param window Window to increment cursor image counter on.
		static void IncrementCursorImageWayland(const InternalWindow& window);
		/// @brief Sets the dock or taskbar progress indicator via DBUS.
		///
		/// This only works on KDE & Unity environments.
		/// A .desktop file must exist for the application with the same name as given to TRAP::Application.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value,
		///         Error::Invalid_Enum, Error::Platform_Error, Error::Feature_Unimplemented
		//          and Error::Feature_Unavailable.
		/// @remark @x11 @wayland Requires a valid application desktop file with the same name
		///                       as the compiled executable. Due to limitations in the Unity Launcher API
		///                       ProgressState::Indeterminate, ProgressState::Error and ProgressState::Paused
		///                       have the same behaviour as ProgressState::Normal.
		///                       The Unity Launcher API is only known to be supported on Unity and
		///                       KDE desktop environments; on other desktop environments this
		///                       function may do nothing.
		/// @threadsafety This function must only be called from the main thread.
		/// @param state State of progress to be displayed in the dock or taskbar.
		/// @param progress The amount of completed progress to set.
		/// Valid range is [0.0-1.0].
		/// This parameter is ignored if state is ProgressState::Disabled
		static void SetProgressIndicator(ProgressState state, f64 progress);

		friend std::optional<std::string> TRAP::Input::GetKeyboardLayoutName();

		struct CodePair
		{
			u16 keySym;
			u16 UCS;
		};

		static constexpr std::array<CodePair, 828u> KeySymTab
		{
			{
				{ 0x01a1u, 0x0104u }, { 0x01a2u, 0x02d8u }, { 0x01a3u, 0x0141u }, { 0x01a5u, 0x013du },
				{ 0x01a6u, 0x015au }, { 0x01a9u, 0x0160u }, { 0x01aau, 0x015eu }, { 0x01abu, 0x0164u },
				{ 0x01acu, 0x0179u }, { 0x01aeu, 0x017du }, { 0x01afu, 0x017bu }, { 0x01b1u, 0x0105u },
				{ 0x01b2u, 0x02dbu }, { 0x01b3u, 0x0142u }, { 0x01b5u, 0x013eu }, { 0x01b6u, 0x015bu },
				{ 0x01b7u, 0x02c7u }, { 0x01b9u, 0x0161u }, { 0x01bau, 0x015fu }, { 0x01bbu, 0x0165u },
				{ 0x01bcu, 0x017au }, { 0x01bdu, 0x02ddu }, { 0x01beu, 0x017eu }, { 0x01bfu, 0x017cu },
				{ 0x01c0u, 0x0154u }, { 0x01c3u, 0x0102u }, { 0x01c5u, 0x0139u }, { 0x01c6u, 0x0106u },
				{ 0x01c8u, 0x010cu }, { 0x01cau, 0x0118u }, { 0x01ccu, 0x011au }, { 0x01cfu, 0x010eu },
				{ 0x01d0u, 0x0110u }, { 0x01d1u, 0x0143u }, { 0x01d2u, 0x0147u }, { 0x01d5u, 0x0150u },
				{ 0x01d8u, 0x0158u }, { 0x01d9u, 0x016eu }, { 0x01dbu, 0x0170u }, { 0x01deu, 0x0162u },
				{ 0x01e0u, 0x0155u }, { 0x01e3u, 0x0103u }, { 0x01e5u, 0x013au }, { 0x01e6u, 0x0107u },
				{ 0x01e8u, 0x010du }, { 0x01eau, 0x0119u }, { 0x01ecu, 0x011bu }, { 0x01efu, 0x010fu },
				{ 0x01f0u, 0x0111u }, { 0x01f1u, 0x0144u }, { 0x01f2u, 0x0148u }, { 0x01f5u, 0x0151u },
				{ 0x01f8u, 0x0159u }, { 0x01f9u, 0x016fu }, { 0x01fbu, 0x0171u }, { 0x01feu, 0x0163u },
				{ 0x01ffu, 0x02d9u }, { 0x02a1u, 0x0126u }, { 0x02a6u, 0x0124u }, { 0x02a9u, 0x0130u },
				{ 0x02abu, 0x011eu }, { 0x02acu, 0x0134u }, { 0x02b1u, 0x0127u }, { 0x02b6u, 0x0125u },
				{ 0x02b9u, 0x0131u }, { 0x02bbu, 0x011fu }, { 0x02bcu, 0x0135u }, { 0x02c5u, 0x010au },
				{ 0x02c6u, 0x0108u }, { 0x02d5u, 0x0120u }, { 0x02d8u, 0x011cu }, { 0x02ddu, 0x016cu },
				{ 0x02deu, 0x015cu }, { 0x02e5u, 0x010bu }, { 0x02e6u, 0x0109u }, { 0x02f5u, 0x0121u },
				{ 0x02f8u, 0x011du }, { 0x02fdu, 0x016du }, { 0x02feu, 0x015du }, { 0x03a2u, 0x0138u },
				{ 0x03a3u, 0x0156u }, { 0x03a5u, 0x0128u }, { 0x03a6u, 0x013bu }, { 0x03aau, 0x0112u },
				{ 0x03abu, 0x0122u }, { 0x03acu, 0x0166u }, { 0x03b3u, 0x0157u }, { 0x03b5u, 0x0129u },
				{ 0x03b6u, 0x013cu }, { 0x03bau, 0x0113u }, { 0x03bbu, 0x0123u }, { 0x03bcu, 0x0167u },
				{ 0x03bdu, 0x014au }, { 0x03bfu, 0x014bu }, { 0x03c0u, 0x0100u }, { 0x03c7u, 0x012eu },
				{ 0x03ccu, 0x0116u }, { 0x03cfu, 0x012au }, { 0x03d1u, 0x0145u }, { 0x03d2u, 0x014cu },
				{ 0x03d3u, 0x0136u }, { 0x03d9u, 0x0172u }, { 0x03ddu, 0x0168u }, { 0x03deu, 0x016au },
				{ 0x03e0u, 0x0101u }, { 0x03e7u, 0x012fu }, { 0x03ecu, 0x0117u }, { 0x03efu, 0x012bu },
				{ 0x03f1u, 0x0146u }, { 0x03f2u, 0x014du }, { 0x03f3u, 0x0137u }, { 0x03f9u, 0x0173u },
				{ 0x03fdu, 0x0169u }, { 0x03feu, 0x016bu }, { 0x047eu, 0x203eu }, { 0x04a1u, 0x3002u },
				{ 0x04a2u, 0x300cu }, { 0x04a3u, 0x300du }, { 0x04a4u, 0x3001u }, { 0x04a5u, 0x30fbu },
				{ 0x04a6u, 0x30f2u }, { 0x04a7u, 0x30a1u }, { 0x04a8u, 0x30a3u }, { 0x04a9u, 0x30a5u },
				{ 0x04aau, 0x30a7u }, { 0x04abu, 0x30a9u }, { 0x04acu, 0x30e3u }, { 0x04adu, 0x30e5u },
				{ 0x04aeu, 0x30e7u }, { 0x04afu, 0x30c3u }, { 0x04b0u, 0x30fcu }, { 0x04b1u, 0x30a2u },
				{ 0x04b2u, 0x30a4u }, { 0x04b3u, 0x30a6u }, { 0x04b4u, 0x30a8u }, { 0x04b5u, 0x30aau },
				{ 0x04b6u, 0x30abu }, { 0x04b7u, 0x30adu }, { 0x04b8u, 0x30afu }, { 0x04b9u, 0x30b1u },
				{ 0x04bau, 0x30b3u }, { 0x04bbu, 0x30b5u }, { 0x04bcu, 0x30b7u }, { 0x04bdu, 0x30b9u },
				{ 0x04beu, 0x30bbu }, { 0x04bfu, 0x30bdu }, { 0x04c0u, 0x30bfu }, { 0x04c1u, 0x30c1u },
				{ 0x04c2u, 0x30c4u }, { 0x04c3u, 0x30c6u }, { 0x04c4u, 0x30c8u }, { 0x04c5u, 0x30cau },
				{ 0x04c6u, 0x30cbu }, { 0x04c7u, 0x30ccu }, { 0x04c8u, 0x30cdu }, { 0x04c9u, 0x30ceu },
				{ 0x04cau, 0x30cfu }, { 0x04cbu, 0x30d2u }, { 0x04ccu, 0x30d5u }, { 0x04cdu, 0x30d8u },
				{ 0x04ceu, 0x30dbu }, { 0x04cfu, 0x30deu }, { 0x04d0u, 0x30dfu }, { 0x04d1u, 0x30e0u },
				{ 0x04d2u, 0x30e1u }, { 0x04d3u, 0x30e2u }, { 0x04d4u, 0x30e4u }, { 0x04d5u, 0x30e6u },
				{ 0x04d6u, 0x30e8u }, { 0x04d7u, 0x30e9u }, { 0x04d8u, 0x30eau }, { 0x04d9u, 0x30ebu },
				{ 0x04dau, 0x30ecu }, { 0x04dbu, 0x30edu }, { 0x04dcu, 0x30efu }, { 0x04ddu, 0x30f3u },
				{ 0x04deu, 0x309bu }, { 0x04dfu, 0x309cu }, { 0x05acu, 0x060cu }, { 0x05bbu, 0x061bu },
				{ 0x05bfu, 0x061fu }, { 0x05c1u, 0x0621u }, { 0x05c2u, 0x0622u }, { 0x05c3u, 0x0623u },
				{ 0x05c4u, 0x0624u }, { 0x05c5u, 0x0625u }, { 0x05c6u, 0x0626u }, { 0x05c7u, 0x0627u },
				{ 0x05c8u, 0x0628u }, { 0x05c9u, 0x0629u }, { 0x05cau, 0x062au }, { 0x05cbu, 0x062bu },
				{ 0x05ccu, 0x062cu }, { 0x05cdu, 0x062du }, { 0x05ceu, 0x062eu }, { 0x05cfu, 0x062fu },
				{ 0x05d0u, 0x0630u }, { 0x05d1u, 0x0631u }, { 0x05d2u, 0x0632u }, { 0x05d3u, 0x0633u },
				{ 0x05d4u, 0x0634u }, { 0x05d5u, 0x0635u }, { 0x05d6u, 0x0636u }, { 0x05d7u, 0x0637u },
				{ 0x05d8u, 0x0638u }, { 0x05d9u, 0x0639u }, { 0x05dau, 0x063au }, { 0x05e0u, 0x0640u },
				{ 0x05e1u, 0x0641u }, { 0x05e2u, 0x0642u }, { 0x05e3u, 0x0643u }, { 0x05e4u, 0x0644u },
				{ 0x05e5u, 0x0645u }, { 0x05e6u, 0x0646u }, { 0x05e7u, 0x0647u }, { 0x05e8u, 0x0648u },
				{ 0x05e9u, 0x0649u }, { 0x05eau, 0x064au }, { 0x05ebu, 0x064bu }, { 0x05ecu, 0x064cu },
				{ 0x05edu, 0x064du }, { 0x05eeu, 0x064eu }, { 0x05efu, 0x064fu }, { 0x05f0u, 0x0650u },
				{ 0x05f1u, 0x0651u }, { 0x05f2u, 0x0652u }, { 0x06a1u, 0x0452u }, { 0x06a2u, 0x0453u },
				{ 0x06a3u, 0x0451u }, { 0x06a4u, 0x0454u }, { 0x06a5u, 0x0455u }, { 0x06a6u, 0x0456u },
				{ 0x06a7u, 0x0457u }, { 0x06a8u, 0x0458u }, { 0x06a9u, 0x0459u }, { 0x06aau, 0x045au },
				{ 0x06abu, 0x045bu }, { 0x06acu, 0x045cu }, { 0x06aeu, 0x045eu }, { 0x06afu, 0x045fu },
				{ 0x06b0u, 0x2116u }, { 0x06b1u, 0x0402u }, { 0x06b2u, 0x0403u }, { 0x06b3u, 0x0401u },
				{ 0x06b4u, 0x0404u }, { 0x06b5u, 0x0405u }, { 0x06b6u, 0x0406u }, { 0x06b7u, 0x0407u },
				{ 0x06b8u, 0x0408u }, { 0x06b9u, 0x0409u }, { 0x06bau, 0x040au }, { 0x06bbu, 0x040bu },
				{ 0x06bcu, 0x040cu }, { 0x06beu, 0x040eu }, { 0x06bfu, 0x040fu }, { 0x06c0u, 0x044eu },
				{ 0x06c1u, 0x0430u }, { 0x06c2u, 0x0431u }, { 0x06c3u, 0x0446u }, { 0x06c4u, 0x0434u },
				{ 0x06c5u, 0x0435u }, { 0x06c6u, 0x0444u }, { 0x06c7u, 0x0433u }, { 0x06c8u, 0x0445u },
				{ 0x06c9u, 0x0438u }, { 0x06cau, 0x0439u }, { 0x06cbu, 0x043au }, { 0x06ccu, 0x043bu },
				{ 0x06cdu, 0x043cu }, { 0x06ceu, 0x043du }, { 0x06cfu, 0x043eu }, { 0x06d0u, 0x043fu },
				{ 0x06d1u, 0x044fu }, { 0x06d2u, 0x0440u }, { 0x06d3u, 0x0441u }, { 0x06d4u, 0x0442u },
				{ 0x06d5u, 0x0443u }, { 0x06d6u, 0x0436u }, { 0x06d7u, 0x0432u }, { 0x06d8u, 0x044cu },
				{ 0x06d9u, 0x044bu }, { 0x06dau, 0x0437u }, { 0x06dbu, 0x0448u }, { 0x06dcu, 0x044du },
				{ 0x06ddu, 0x0449u }, { 0x06deu, 0x0447u }, { 0x06dfu, 0x044au }, { 0x06e0u, 0x042eu },
				{ 0x06e1u, 0x0410u }, { 0x06e2u, 0x0411u }, { 0x06e3u, 0x0426u }, { 0x06e4u, 0x0414u },
				{ 0x06e5u, 0x0415u }, { 0x06e6u, 0x0424u }, { 0x06e7u, 0x0413u }, { 0x06e8u, 0x0425u },
				{ 0x06e9u, 0x0418u }, { 0x06eau, 0x0419u }, { 0x06ebu, 0x041au }, { 0x06ecu, 0x041bu },
				{ 0x06edu, 0x041cu }, { 0x06eeu, 0x041du }, { 0x06efu, 0x041eu }, { 0x06f0u, 0x041fu },
				{ 0x06f1u, 0x042fu }, { 0x06f2u, 0x0420u }, { 0x06f3u, 0x0421u }, { 0x06f4u, 0x0422u },
				{ 0x06f5u, 0x0423u }, { 0x06f6u, 0x0416u }, { 0x06f7u, 0x0412u }, { 0x06f8u, 0x042cu },
				{ 0x06f9u, 0x042bu }, { 0x06fau, 0x0417u }, { 0x06fbu, 0x0428u }, { 0x06fcu, 0x042du },
				{ 0x06fdu, 0x0429u }, { 0x06feu, 0x0427u }, { 0x06ffu, 0x042au }, { 0x07a1u, 0x0386u },
				{ 0x07a2u, 0x0388u }, { 0x07a3u, 0x0389u }, { 0x07a4u, 0x038au }, { 0x07a5u, 0x03aau },
				{ 0x07a7u, 0x038cu }, { 0x07a8u, 0x038eu }, { 0x07a9u, 0x03abu }, { 0x07abu, 0x038fu },
				{ 0x07aeu, 0x0385u }, { 0x07afu, 0x2015u }, { 0x07b1u, 0x03acu }, { 0x07b2u, 0x03adu },
				{ 0x07b3u, 0x03aeu }, { 0x07b4u, 0x03afu }, { 0x07b5u, 0x03cau }, { 0x07b6u, 0x0390u },
				{ 0x07b7u, 0x03ccu }, { 0x07b8u, 0x03cdu }, { 0x07b9u, 0x03cbu }, { 0x07bau, 0x03b0u },
				{ 0x07bbu, 0x03ceu }, { 0x07c1u, 0x0391u }, { 0x07c2u, 0x0392u }, { 0x07c3u, 0x0393u },
				{ 0x07c4u, 0x0394u }, { 0x07c5u, 0x0395u }, { 0x07c6u, 0x0396u }, { 0x07c7u, 0x0397u },
				{ 0x07c8u, 0x0398u }, { 0x07c9u, 0x0399u }, { 0x07cau, 0x039au }, { 0x07cbu, 0x039bu },
				{ 0x07ccu, 0x039cu }, { 0x07cdu, 0x039du }, { 0x07ceu, 0x039eu }, { 0x07cfu, 0x039fu },
				{ 0x07d0u, 0x03a0u }, { 0x07d1u, 0x03a1u }, { 0x07d2u, 0x03a3u }, { 0x07d4u, 0x03a4u },
				{ 0x07d5u, 0x03a5u }, { 0x07d6u, 0x03a6u }, { 0x07d7u, 0x03a7u }, { 0x07d8u, 0x03a8u },
				{ 0x07d9u, 0x03a9u }, { 0x07e1u, 0x03b1u }, { 0x07e2u, 0x03b2u }, { 0x07e3u, 0x03b3u },
				{ 0x07e4u, 0x03b4u }, { 0x07e5u, 0x03b5u }, { 0x07e6u, 0x03b6u }, { 0x07e7u, 0x03b7u },
				{ 0x07e8u, 0x03b8u }, { 0x07e9u, 0x03b9u }, { 0x07eau, 0x03bau }, { 0x07ebu, 0x03bbu },
				{ 0x07ecu, 0x03bcu }, { 0x07edu, 0x03bdu }, { 0x07eeu, 0x03beu }, { 0x07efu, 0x03bfu },
				{ 0x07f0u, 0x03c0u }, { 0x07f1u, 0x03c1u }, { 0x07f2u, 0x03c3u }, { 0x07f3u, 0x03c2u },
				{ 0x07f4u, 0x03c4u }, { 0x07f5u, 0x03c5u }, { 0x07f6u, 0x03c6u }, { 0x07f7u, 0x03c7u },
				{ 0x07f8u, 0x03c8u }, { 0x07f9u, 0x03c9u }, { 0x08a1u, 0x23b7u }, { 0x08a2u, 0x250cu },
				{ 0x08a3u, 0x2500u }, { 0x08a4u, 0x2320u }, { 0x08a5u, 0x2321u }, { 0x08a6u, 0x2502u },
				{ 0x08a7u, 0x23a1u }, { 0x08a8u, 0x23a3u }, { 0x08a9u, 0x23a4u }, { 0x08aau, 0x23a6u },
				{ 0x08abu, 0x239bu }, { 0x08acu, 0x239du }, { 0x08adu, 0x239eu }, { 0x08aeu, 0x23a0u },
				{ 0x08afu, 0x23a8u }, { 0x08b0u, 0x23acu }, { 0x08bcu, 0x2264u }, { 0x08bdu, 0x2260u },
				{ 0x08beu, 0x2265u }, { 0x08bfu, 0x222bu }, { 0x08c0u, 0x2234u }, { 0x08c1u, 0x221du },
				{ 0x08c2u, 0x221eu }, { 0x08c5u, 0x2207u }, { 0x08c8u, 0x223cu }, { 0x08c9u, 0x2243u },
				{ 0x08cdu, 0x21d4u }, { 0x08ceu, 0x21d2u }, { 0x08cfu, 0x2261u }, { 0x08d6u, 0x221au },
				{ 0x08dau, 0x2282u }, { 0x08dbu, 0x2283u }, { 0x08dcu, 0x2229u }, { 0x08ddu, 0x222au },
				{ 0x08deu, 0x2227u }, { 0x08dfu, 0x2228u }, { 0x08efu, 0x2202u }, { 0x08f6u, 0x0192u },
				{ 0x08fbu, 0x2190u }, { 0x08fcu, 0x2191u }, { 0x08fdu, 0x2192u }, { 0x08feu, 0x2193u },
				{ 0x09e0u, 0x25c6u }, { 0x09e1u, 0x2592u }, { 0x09e2u, 0x2409u }, { 0x09e3u, 0x240cu },
				{ 0x09e4u, 0x240du }, { 0x09e5u, 0x240au }, { 0x09e8u, 0x2424u }, { 0x09e9u, 0x240bu },
				{ 0x09eau, 0x2518u }, { 0x09ebu, 0x2510u }, { 0x09ecu, 0x250cu }, { 0x09edu, 0x2514u },
				{ 0x09eeu, 0x253cu }, { 0x09efu, 0x23bau }, { 0x09f0u, 0x23bbu }, { 0x09f1u, 0x2500u },
				{ 0x09f2u, 0x23bcu }, { 0x09f3u, 0x23bdu }, { 0x09f4u, 0x251cu }, { 0x09f5u, 0x2524u },
				{ 0x09f6u, 0x2534u }, { 0x09f7u, 0x252cu }, { 0x09f8u, 0x2502u }, { 0x0aa1u, 0x2003u },
				{ 0x0aa2u, 0x2002u }, { 0x0aa3u, 0x2004u }, { 0x0aa4u, 0x2005u }, { 0x0aa5u, 0x2007u },
				{ 0x0aa6u, 0x2008u }, { 0x0aa7u, 0x2009u }, { 0x0aa8u, 0x200au }, { 0x0aa9u, 0x2014u },
				{ 0x0aaau, 0x2013u }, { 0x0aaeu, 0x2026u }, { 0x0aafu, 0x2025u }, { 0x0ab0u, 0x2153u },
				{ 0x0ab1u, 0x2154u }, { 0x0ab2u, 0x2155u }, { 0x0ab3u, 0x2156u }, { 0x0ab4u, 0x2157u },
				{ 0x0ab5u, 0x2158u }, { 0x0ab6u, 0x2159u }, { 0x0ab7u, 0x215au }, { 0x0ab8u, 0x2105u },
				{ 0x0abbu, 0x2012u }, { 0x0abcu, 0x2329u }, { 0x0abeu, 0x232au }, { 0x0ac3u, 0x215bu },
				{ 0x0ac4u, 0x215cu }, { 0x0ac5u, 0x215du }, { 0x0ac6u, 0x215eu }, { 0x0ac9u, 0x2122u },
				{ 0x0acau, 0x2613u }, { 0x0accu, 0x25c1u }, { 0x0acdu, 0x25b7u }, { 0x0aceu, 0x25cbu },
				{ 0x0acfu, 0x25afu }, { 0x0ad0u, 0x2018u }, { 0x0ad1u, 0x2019u }, { 0x0ad2u, 0x201cu },
				{ 0x0ad3u, 0x201du }, { 0x0ad4u, 0x211eu }, { 0x0ad6u, 0x2032u }, { 0x0ad7u, 0x2033u },
				{ 0x0ad9u, 0x271du }, { 0x0adbu, 0x25acu }, { 0x0adcu, 0x25c0u }, { 0x0addu, 0x25b6u },
				{ 0x0adeu, 0x25cfu }, { 0x0adfu, 0x25aeu }, { 0x0ae0u, 0x25e6u }, { 0x0ae1u, 0x25abu },
				{ 0x0ae2u, 0x25adu }, { 0x0ae3u, 0x25b3u }, { 0x0ae4u, 0x25bdu }, { 0x0ae5u, 0x2606u },
				{ 0x0ae6u, 0x2022u }, { 0x0ae7u, 0x25aau }, { 0x0ae8u, 0x25b2u }, { 0x0ae9u, 0x25bcu },
				{ 0x0aeau, 0x261cu }, { 0x0aebu, 0x261eu }, { 0x0aecu, 0x2663u }, { 0x0aedu, 0x2666u },
				{ 0x0aeeu, 0x2665u }, { 0x0af0u, 0x2720u }, { 0x0af1u, 0x2020u }, { 0x0af2u, 0x2021u },
				{ 0x0af3u, 0x2713u }, { 0x0af4u, 0x2717u }, { 0x0af5u, 0x266fu }, { 0x0af6u, 0x266du },
				{ 0x0af7u, 0x2642u }, { 0x0af8u, 0x2640u }, { 0x0af9u, 0x260eu }, { 0x0afau, 0x2315u },
				{ 0x0afbu, 0x2117u }, { 0x0afcu, 0x2038u }, { 0x0afdu, 0x201au }, { 0x0afeu, 0x201eu },
				{ 0x0ba3u, 0x003cu }, { 0x0ba6u, 0x003eu }, { 0x0ba8u, 0x2228u }, { 0x0ba9u, 0x2227u },
				{ 0x0bc0u, 0x00afu }, { 0x0bc2u, 0x22a5u }, { 0x0bc3u, 0x2229u }, { 0x0bc4u, 0x230au },
				{ 0x0bc6u, 0x005fu }, { 0x0bcau, 0x2218u }, { 0x0bccu, 0x2395u }, { 0x0bceu, 0x22a4u },
				{ 0x0bcfu, 0x25cbu }, { 0x0bd3u, 0x2308u }, { 0x0bd6u, 0x222au }, { 0x0bd8u, 0x2283u },
				{ 0x0bdau, 0x2282u }, { 0x0bdcu, 0x22a2u }, { 0x0bfcu, 0x22a3u }, { 0x0cdfu, 0x2017u },
				{ 0x0ce0u, 0x05d0u }, { 0x0ce1u, 0x05d1u }, { 0x0ce2u, 0x05d2u }, { 0x0ce3u, 0x05d3u },
				{ 0x0ce4u, 0x05d4u }, { 0x0ce5u, 0x05d5u }, { 0x0ce6u, 0x05d6u }, { 0x0ce7u, 0x05d7u },
				{ 0x0ce8u, 0x05d8u }, { 0x0ce9u, 0x05d9u }, { 0x0ceau, 0x05dau }, { 0x0cebu, 0x05dbu },
				{ 0x0cecu, 0x05dcu }, { 0x0cedu, 0x05ddu }, { 0x0ceeu, 0x05deu }, { 0x0cefu, 0x05dfu },
				{ 0x0cf0u, 0x05e0u }, { 0x0cf1u, 0x05e1u }, { 0x0cf2u, 0x05e2u }, { 0x0cf3u, 0x05e3u },
				{ 0x0cf4u, 0x05e4u }, { 0x0cf5u, 0x05e5u }, { 0x0cf6u, 0x05e6u }, { 0x0cf7u, 0x05e7u },
				{ 0x0cf8u, 0x05e8u }, { 0x0cf9u, 0x05e9u }, { 0x0cfau, 0x05eau }, { 0x0da1u, 0x0e01u },
				{ 0x0da2u, 0x0e02u }, { 0x0da3u, 0x0e03u }, { 0x0da4u, 0x0e04u }, { 0x0da5u, 0x0e05u },
				{ 0x0da6u, 0x0e06u }, { 0x0da7u, 0x0e07u }, { 0x0da8u, 0x0e08u }, { 0x0da9u, 0x0e09u },
				{ 0x0daau, 0x0e0au }, { 0x0dabu, 0x0e0bu }, { 0x0dacu, 0x0e0cu }, { 0x0dadu, 0x0e0du },
				{ 0x0daeu, 0x0e0eu }, { 0x0dafu, 0x0e0fu }, { 0x0db0u, 0x0e10u }, { 0x0db1u, 0x0e11u },
				{ 0x0db2u, 0x0e12u }, { 0x0db3u, 0x0e13u }, { 0x0db4u, 0x0e14u }, { 0x0db5u, 0x0e15u },
				{ 0x0db6u, 0x0e16u }, { 0x0db7u, 0x0e17u }, { 0x0db8u, 0x0e18u }, { 0x0db9u, 0x0e19u },
				{ 0x0dbau, 0x0e1au }, { 0x0dbbu, 0x0e1bu }, { 0x0dbcu, 0x0e1cu }, { 0x0dbdu, 0x0e1du },
				{ 0x0dbeu, 0x0e1eu }, { 0x0dbfu, 0x0e1fu }, { 0x0dc0u, 0x0e20u }, { 0x0dc1u, 0x0e21u },
				{ 0x0dc2u, 0x0e22u }, { 0x0dc3u, 0x0e23u }, { 0x0dc4u, 0x0e24u }, { 0x0dc5u, 0x0e25u },
				{ 0x0dc6u, 0x0e26u }, { 0x0dc7u, 0x0e27u }, { 0x0dc8u, 0x0e28u }, { 0x0dc9u, 0x0e29u },
				{ 0x0dcau, 0x0e2au }, { 0x0dcbu, 0x0e2bu }, { 0x0dccu, 0x0e2cu }, { 0x0dcdu, 0x0e2du },
				{ 0x0dceu, 0x0e2eu }, { 0x0dcfu, 0x0e2fu }, { 0x0dd0u, 0x0e30u }, { 0x0dd1u, 0x0e31u },
				{ 0x0dd2u, 0x0e32u }, { 0x0dd3u, 0x0e33u }, { 0x0dd4u, 0x0e34u }, { 0x0dd5u, 0x0e35u },
				{ 0x0dd6u, 0x0e36u }, { 0x0dd7u, 0x0e37u }, { 0x0dd8u, 0x0e38u }, { 0x0dd9u, 0x0e39u },
				{ 0x0ddau, 0x0e3au }, { 0x0ddfu, 0x0e3fu }, { 0x0de0u, 0x0e40u }, { 0x0de1u, 0x0e41u },
				{ 0x0de2u, 0x0e42u }, { 0x0de3u, 0x0e43u }, { 0x0de4u, 0x0e44u }, { 0x0de5u, 0x0e45u },
				{ 0x0de6u, 0x0e46u }, { 0x0de7u, 0x0e47u }, { 0x0de8u, 0x0e48u }, { 0x0de9u, 0x0e49u },
				{ 0x0deau, 0x0e4au }, { 0x0debu, 0x0e4bu }, { 0x0decu, 0x0e4cu }, { 0x0dedu, 0x0e4du },
				{ 0x0df0u, 0x0e50u }, { 0x0df1u, 0x0e51u }, { 0x0df2u, 0x0e52u }, { 0x0df3u, 0x0e53u },
				{ 0x0df4u, 0x0e54u }, { 0x0df5u, 0x0e55u }, { 0x0df6u, 0x0e56u }, { 0x0df7u, 0x0e57u },
				{ 0x0df8u, 0x0e58u }, { 0x0df9u, 0x0e59u }, { 0x0ea1u, 0x3131u }, { 0x0ea2u, 0x3132u },
				{ 0x0ea3u, 0x3133u }, { 0x0ea4u, 0x3134u }, { 0x0ea5u, 0x3135u }, { 0x0ea6u, 0x3136u },
				{ 0x0ea7u, 0x3137u }, { 0x0ea8u, 0x3138u }, { 0x0ea9u, 0x3139u }, { 0x0eaau, 0x313au },
				{ 0x0eabu, 0x313bu }, { 0x0eacu, 0x313cu }, { 0x0eadu, 0x313du }, { 0x0eaeu, 0x313eu },
				{ 0x0eafu, 0x313fu }, { 0x0eb0u, 0x3140u }, { 0x0eb1u, 0x3141u }, { 0x0eb2u, 0x3142u },
				{ 0x0eb3u, 0x3143u }, { 0x0eb4u, 0x3144u }, { 0x0eb5u, 0x3145u }, { 0x0eb6u, 0x3146u },
				{ 0x0eb7u, 0x3147u }, { 0x0eb8u, 0x3148u }, { 0x0eb9u, 0x3149u }, { 0x0ebau, 0x314au },
				{ 0x0ebbu, 0x314bu }, { 0x0ebcu, 0x314cu }, { 0x0ebdu, 0x314du }, { 0x0ebeu, 0x314eu },
				{ 0x0ebfu, 0x314fu }, { 0x0ec0u, 0x3150u }, { 0x0ec1u, 0x3151u }, { 0x0ec2u, 0x3152u },
				{ 0x0ec3u, 0x3153u }, { 0x0ec4u, 0x3154u }, { 0x0ec5u, 0x3155u }, { 0x0ec6u, 0x3156u },
				{ 0x0ec7u, 0x3157u }, { 0x0ec8u, 0x3158u }, { 0x0ec9u, 0x3159u }, { 0x0ecau, 0x315au },
				{ 0x0ecbu, 0x315bu }, { 0x0eccu, 0x315cu }, { 0x0ecdu, 0x315du }, { 0x0eceu, 0x315eu },
				{ 0x0ecfu, 0x315fu }, { 0x0ed0u, 0x3160u }, { 0x0ed1u, 0x3161u }, { 0x0ed2u, 0x3162u },
				{ 0x0ed3u, 0x3163u }, { 0x0ed4u, 0x11a8u }, { 0x0ed5u, 0x11a9u }, { 0x0ed6u, 0x11aau },
				{ 0x0ed7u, 0x11abu }, { 0x0ed8u, 0x11acu }, { 0x0ed9u, 0x11adu }, { 0x0edau, 0x11aeu },
				{ 0x0edbu, 0x11afu }, { 0x0edcu, 0x11b0u }, { 0x0eddu, 0x11b1u }, { 0x0edeu, 0x11b2u },
				{ 0x0edfu, 0x11b3u }, { 0x0ee0u, 0x11b4u }, { 0x0ee1u, 0x11b5u }, { 0x0ee2u, 0x11b6u },
				{ 0x0ee3u, 0x11b7u }, { 0x0ee4u, 0x11b8u }, { 0x0ee5u, 0x11b9u }, { 0x0ee6u, 0x11bau },
				{ 0x0ee7u, 0x11bbu }, { 0x0ee8u, 0x11bcu }, { 0x0ee9u, 0x11bdu }, { 0x0eeau, 0x11beu },
				{ 0x0eebu, 0x11bfu }, { 0x0eecu, 0x11c0u }, { 0x0eedu, 0x11c1u }, { 0x0eeeu, 0x11c2u },
				{ 0x0eefu, 0x316du }, { 0x0ef0u, 0x3171u }, { 0x0ef1u, 0x3178u }, { 0x0ef2u, 0x317fu },
				{ 0x0ef3u, 0x3181u }, { 0x0ef4u, 0x3184u }, { 0x0ef5u, 0x3186u }, { 0x0ef6u, 0x318du },
				{ 0x0ef7u, 0x318eu }, { 0x0ef8u, 0x11ebu }, { 0x0ef9u, 0x11f0u }, { 0x0efau, 0x11f9u },
				{ 0x0effu, 0x20a9u }, { 0x13a4u, 0x20acu }, { 0x13bcu, 0x0152u }, { 0x13bdu, 0x0153u },
				{ 0x13beu, 0x0178u }, { 0x20acu, 0x20acu }, { 0xfe50u,    '`' }, { 0xfe51u, 0x00b4u },
				{ 0xfe52u,    '^' }, { 0xfe53u,    '~' }, { 0xfe54u, 0x00afu }, { 0xfe55u, 0x02d8u },
				{ 0xfe56u, 0x02d9u }, { 0xfe57u, 0x00a8u }, { 0xfe58u, 0x02dau }, { 0xfe59u, 0x02ddu },
				{ 0xfe5au, 0x02c7u }, { 0xfe5bu, 0x00b8u }, { 0xfe5cu, 0x02dbu }, { 0xfe5du, 0x037au },
				{ 0xfe5eu, 0x309bu }, { 0xfe5fu, 0x309cu }, { 0xfe63u,    '/' }, { 0xfe64u, 0x02bcu },
				{ 0xfe65u, 0x02bdu }, { 0xfe66u, 0x02f5u }, { 0xfe67u, 0x02f3u }, { 0xfe68u, 0x02cdu },
				{ 0xfe69u, 0xa788u }, { 0xfe6au, 0x02f7u }, { 0xfe6eu,    ',' }, { 0xfe6fu, 0x00a4u },
				{ 0xfe80u,    'a' }, // XK_dead_a
				{ 0xfe81u,    'A' }, // XK_dead_A
				{ 0xfe82u,    'e' }, // XK_dead_e
				{ 0xfe83u,    'E' }, // XK_dead_E
				{ 0xfe84u,    'i' }, // XK_dead_i
				{ 0xfe85u,    'I' }, // XK_dead_I
				{ 0xfe86u,    'o' }, // XK_dead_o
				{ 0xfe87u,    'O' }, // XK_dead_O
				{ 0xfe88u,    'u' }, // XK_dead_u
				{ 0xfe89u,    'U' }, // XK_dead_U
				{ 0xfe8au, 0x0259u }, { 0xfe8bu, 0x018fu }, { 0xfe8cu, 0x00b5u }, { 0xfe90u,    '_' },
				{ 0xfe91u, 0x02c8u }, { 0xfe92u, 0x02ccu },
				{ 0xff80u /*XKB_KEY_KP_Space*/,     ' ' },
				{ 0xff95u /*XKB_KEY_KP_7*/, 0x0037u },
				{ 0xff96u /*XKB_KEY_KP_4*/, 0x0034u },
				{ 0xff97u /*XKB_KEY_KP_8*/, 0x0038u },
				{ 0xff98u /*XKB_KEY_KP_6*/, 0x0036u },
				{ 0xff99u /*XKB_KEY_KP_2*/, 0x0032u },
				{ 0xff9au /*XKB_KEY_KP_9*/, 0x0039u },
				{ 0xff9bu /*XKB_KEY_KP_3*/, 0x0033u },
				{ 0xff9cu /*XKB_KEY_KP_1*/, 0x0031u },
				{ 0xff9du /*XKB_KEY_KP_5*/, 0x0035u },
				{ 0xff9eu /*XKB_KEY_KP_0*/, 0x0030u },
				{ 0xffaau /*XKB_KEY_KP_Multiply*/,  '*' },
				{ 0xffabu /*XKB_KEY_KP_Add*/,       '+' },
				{ 0xffacu /*XKB_KEY_KP_Separator*/, ',' },
				{ 0xffadu /*XKB_KEY_KP_Subtract*/,  '-' },
				{ 0xffaeu /*XKB_KEY_KP_Decimal*/,   '.' },
				{ 0xffafu /*XKB_KEY_KP_Divide*/,    '/' },
				{ 0xffb0u /*XKB_KEY_KP_0*/, 0x0030u },
				{ 0xffb1u /*XKB_KEY_KP_1*/, 0x0031u },
				{ 0xffb2u /*XKB_KEY_KP_2*/, 0x0032u },
				{ 0xffb3u /*XKB_KEY_KP_3*/, 0x0033u },
				{ 0xffb4u /*XKB_KEY_KP_4*/, 0x0034u },
				{ 0xffb5u /*XKB_KEY_KP_5*/, 0x0035u },
				{ 0xffb6u /*XKB_KEY_KP_6*/, 0x0036u },
				{ 0xffb7u /*XKB_KEY_KP_7*/, 0x0037u },
				{ 0xffb8u /*XKB_KEY_KP_8*/, 0x0038u },
				{ 0xffb9u /*XKB_KEY_KP_9*/, 0x0039u },
				{ 0xffbdu /*XKB_KEY_KP_Equal*/,     '=' }
			}
		};
#endif
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//Universal----------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::INTERNAL::WindowingAPI::CompareVideoModes(const InternalVideoMode& fm, const InternalVideoMode& sm)
{
	const i32 fbpp = fm.RedBits + fm.GreenBits + fm.BlueBits;
	const i32 sbpp = sm.RedBits + sm.GreenBits + sm.BlueBits;
	const i32 farea = fm.Width * fm.Height;
	const i32 sarea = sm.Width * sm.Height;

	//First sort on color bits per pixel
	if (fbpp != sbpp)
		return fbpp < sbpp;

	//Then sort on screen area
	if (farea != sarea)
		return farea < sarea;

	//Then sort on width
	if (fm.Width != sm.Width)
		return fm.Width < sm.Width;

	//Lastly sort on refresh rate
	return fm.RefreshRate < sm.RefreshRate;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::SplitBPP(i32 bpp, i32& red, i32& green, i32& blue)
{
	//We assume that by 32 the user really meant 24
	if (bpp == 32)
		bpp = 24;

	//Convert "bits per pixel" to red, green & blue sizes

	red = green = blue = bpp / 3;
	const i32 delta = bpp - (red * 3);
	if (delta >= 1)
		green = green + 1;

	if (delta == 2)
		red = red + 1;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the position callback for the specified window.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::WindowPositionFunc TRAP::INTERNAL::WindowingAPI::GetWindowPosCallback(const InternalWindow& window)
{
	return window.Callbacks.Pos;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the size callback for the specified window.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::WindowSizeFunc TRAP::INTERNAL::WindowingAPI::GetWindowSizeCallback(const InternalWindow& window)
{
	return window.Callbacks.Size;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the close callback for the specified window.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::WindowCloseFunc TRAP::INTERNAL::WindowingAPI::GetWindowCloseCallback(const InternalWindow& window)
{
	return window.Callbacks.Close;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the focus callback for the specified window.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::WindowFocusFunc TRAP::INTERNAL::WindowingAPI::GetWindowFocusCallback(const InternalWindow& window)
{
	return window.Callbacks.Focus;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the framebuffer resize callback for the specified window.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::FrameBufferSizeFunc TRAP::INTERNAL::WindowingAPI::GetFrameBufferSizeCallback(const InternalWindow& window)
{
	return window.Callbacks.FBSize;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the window content scale callback for the specified window.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::WindowContentScaleFunc TRAP::INTERNAL::WindowingAPI::GetWindowContentScaleCallback(const InternalWindow& window)
{
	return window.Callbacks.Scale;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the key callback.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::KeyFunc TRAP::INTERNAL::WindowingAPI::GetKeyCallback(const InternalWindow& window)
{
	return window.Callbacks.Key;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the Unicode character callback.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::CharFunc TRAP::INTERNAL::WindowingAPI::GetCharCallback(const InternalWindow& window)
{
	return window.Callbacks.Character;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the mouse button callback.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::MouseButtonFunc TRAP::INTERNAL::WindowingAPI::GetMouseButtonCallback(const InternalWindow& window)
{
	return window.Callbacks.MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the cursor position callback.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::CursorPositionFunc TRAP::INTERNAL::WindowingAPI::GetCursorPosCallback(const InternalWindow& window)
{
	return window.Callbacks.CursorPos;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the cursor enter callback.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::CursorEnterFunc TRAP::INTERNAL::WindowingAPI::GetCursorEnterCallback(const InternalWindow& window)
{
	return window.Callbacks.CursorEnter;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the scroll callback.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::ScrollFunc TRAP::INTERNAL::WindowingAPI::GetScrollCallback(const InternalWindow& window)
{
	return window.Callbacks.Scroll;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the path drop callback.
[[nodiscard]] constexpr TRAP::INTERNAL::WindowingAPI::DropFunc TRAP::INTERNAL::WindowingAPI::GetDropCallback(const InternalWindow& window)
{
	return window.Callbacks.Drop;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a Unicode codepoint input event
//The 'plain' parameter determines whether to emit a regular character event
constexpr void TRAP::INTERNAL::WindowingAPI::InputChar(const InternalWindow& window, const u32 codePoint)
{
	if (codePoint < 32 || (codePoint > 126 && codePoint < 160))
		return;

	if (window.Callbacks.Character != nullptr)
		window.Callbacks.Character(window, codePoint);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a cursor enter/leave event
constexpr void TRAP::INTERNAL::WindowingAPI::InputCursorEnter(const InternalWindow& window, const bool entered)
{
	if (window.Callbacks.CursorEnter != nullptr)
		window.Callbacks.CursorEnter(window, entered);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been minimized
constexpr void TRAP::INTERNAL::WindowingAPI::InputWindowMinimize(const InternalWindow& window, const bool restored)
{
	if (window.Callbacks.Minimize != nullptr)
		window.Callbacks.Minimize(window, !restored);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been maximized
constexpr void TRAP::INTERNAL::WindowingAPI::InputWindowMaximize(const InternalWindow& window, const bool restored)
{
	if (window.Callbacks.Maximize != nullptr)
		window.Callbacks.Maximize(window, !restored);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has moved
//The position is specified in content area relative screen coordinates
constexpr void TRAP::INTERNAL::WindowingAPI::InputWindowPos(const InternalWindow& window, const i32 xPos, const i32 yPos)
{
	if (window.Callbacks.Pos != nullptr)
		window.Callbacks.Pos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that the user wishes to close a window
constexpr void TRAP::INTERNAL::WindowingAPI::InputWindowCloseRequest(InternalWindow& window)
{
	window.ShouldClose = true;

	if (window.Callbacks.Close != nullptr)
		window.Callbacks.Close(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of files or directories dropped on a window
constexpr void TRAP::INTERNAL::WindowingAPI::InputDrop(const InternalWindow& window, const std::vector<std::string>& paths)
{
	if (window.Callbacks.Drop != nullptr)
		window.Callbacks.Drop(window, paths);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::INTERNAL::WindowingAPI::GetVulkanResultString(const VkResult result)
{
	switch (result)
	{
	case VK_SUCCESS:
		return "Success";

	case VK_NOT_READY:
		return "A fence or query has not yet completed";

	case VK_TIMEOUT:
		return "A wait operation has not completed in the specified time";

	case VK_EVENT_SET:
		return "An event is signaled";

	case VK_EVENT_RESET:
		return "An event is unsignaled";

	case VK_INCOMPLETE:
		return "A return array was too small for the result";

	case VK_SUBOPTIMAL_KHR:
		return "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully";

	case VK_THREAD_IDLE_KHR:
		return "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call";

	case VK_THREAD_DONE_KHR:
		return "A deferred operation is not complete but there is no work remaining to assign to additional threads";

	case VK_OPERATION_DEFERRED_KHR:
		return "A deferred operation was requested and at least some of the work was deferred";

	case VK_OPERATION_NOT_DEFERRED_KHR:
		return "A deferred operation was requested and no work was deferred";

	case VK_PIPELINE_COMPILE_REQUIRED_EXT:
		return "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed";

	case VK_ERROR_OUT_OF_HOST_MEMORY:
		return "A host memory allocation has failed";

	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		return "A device memory allocation has failed";

	case VK_ERROR_INITIALIZATION_FAILED:
		return "Initialization of an object could not be completed for implementation-specific reasons";

	case VK_ERROR_DEVICE_LOST:
		return "The logical or physical device has been lost";

	case VK_ERROR_MEMORY_MAP_FAILED:
		return "Mapping of a memory object has failed";

	case VK_ERROR_LAYER_NOT_PRESENT:
		return "A requested layer is not present or could not be loaded";

	case VK_ERROR_EXTENSION_NOT_PRESENT:
		return "A requested extension is not supported";

	case VK_ERROR_FEATURE_NOT_PRESENT:
		return "A requested feature is not supported";

	case VK_ERROR_INCOMPATIBLE_DRIVER:
		return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons";

	case VK_ERROR_TOO_MANY_OBJECTS:
		return "Too many objects of the type have already been created";

	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		return "A requested format is not supported on this device";

	case VK_ERROR_FRAGMENTED_POOL:
		return "A pool allocation has failed due to fragmentation of the pool's memory";

	case VK_ERROR_SURFACE_LOST_KHR:
		return "A surface is no longer available";

	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";

	case VK_ERROR_OUT_OF_DATE_KHR:
		return "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail";

	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		return "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image";

	case VK_ERROR_INVALID_SHADER_NV:
		return "One or more shaders failed to compile or link";

	case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
		return "A pool memory allocation has failed";

	case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
		return "A synchronization primitive returned an invalid external handle";

	case VK_ERROR_FRAGMENTATION_EXT:
		return "A descriptor pool creation has failed due to fragmentation";

	case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
		return "A buffer creation failed because the requested address is not available";

	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
		return "A swapchain created with the full screen exclusive mode cannot be used to create another full screen exclusive swapchain";

	default:
		return "Unknown Vulkan error";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::InputError(const Error code, const std::string_view str)
{
	std::string description = "[WindowingAPI]";

	if (!str.empty())
		description += str;
	else
	{
		if (code == Error::Invalid_Enum)
			description += " Invalid argument for enum parameter";
		else if (code == Error::Invalid_Value)
			description += " Invalid value for parameter";
		else if (code == Error::Out_Of_Memory)
			description += " Out of memory";
		else if (code == Error::API_Unavailable)
			description += " The requested API is unavailable";
		else if (code == Error::Platform_Error)
			description += " A platform-specific error occurred";
		else if (code == Error::Format_Unavailable)
			description += " The requested format is unavailable";
		else if (code == Error::Cursor_Unavailable)
			description += " The specified cursor shape is unavailable";
		else
			description += " UNKNOWN WINDOWING ERROR";
	}

	TP_ERROR(description, " Code(", std::to_underlying(code), ")");
}

//-------------------------------------------------------------------------------------------------------------------//
//Linux (X11 & Wayland)----------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_LINUX

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbarX11([[maybe_unused]] const InternalWindow& window)
{
    InputError(Error::Feature_Unavailable, "[X11] Platform does not support hiding windows from the taskbar");
}

//-------------------------------------------------------------------------------------------------------------------//

//Translates an X11 or Wayland key code to a TRAP key token
[[nodiscard]] constexpr TRAP::Input::Key TRAP::INTERNAL::WindowingAPI::TranslateKey(const i32 scanCode)
{
	//Use the pre-filled LUT (see CreateKeyTables())
	if(scanCode < 0 || std::cmp_greater_equal(scanCode, s_Data.KeyCodes.size()))
		return Input::Key::Unknown;

	return s_Data.KeyCodes[NumericCast<u32>(scanCode)];
}

//Calculates the refresh rate, in Hz, from the specified RandR mode info
[[nodiscard]] constexpr std::optional<f64> TRAP::INTERNAL::WindowingAPI::CalculateRefreshRate(const XRRModeInfo& mi)
{
	if((mi.hTotal != 0u) && (mi.vTotal != 0u))
		return NumericCast<f64>(mi.dotClock) / (NumericCast<f64>(mi.hTotal) * NumericCast<f64>(mi.vTotal));

	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::optional<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::VideoModeFromModeInfo(const XRRModeInfo& mi,
                                                                                                                                                  const XRRCrtcInfo& ci)
{
	InternalVideoMode mode{};

	mode.Width = NumericCast<i32>(mi.width);
	mode.Height = NumericCast<i32>(mi.height);

	if(ci.rotation == RR_Rotate_90 || ci.rotation == RR_Rotate_270)
		std::swap(mode.Width, mode.Height);

	const std::optional<f64> refreshRate = CalculateRefreshRate(mi);
	if(refreshRate)
		mode.RefreshRate = *refreshRate;
	else
		return std::nullopt;

	SplitBPP(DefaultDepth(s_Data.X11.display, s_Data.X11.Screen), mode.RedBits, mode.GreenBits, mode.BlueBits);

	return mode;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the mode info for a RandR mode XID
[[nodiscard]] constexpr const XRRModeInfo* TRAP::INTERNAL::WindowingAPI::GetModeInfo(const XRRScreenResources& sr, const RRMode id)
{
	for(i32 i = 0; i < sr.nmode; i++)
	{
		if(sr.modes[i].id == id)
			return sr.modes + i;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::LockedPointerHandleLocked([[maybe_unused]] void* const userData,
                                                                              [[maybe_unused]] zwp_locked_pointer_v1* const lockedPointer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::LockedPointerHandleUnlocked([[maybe_unused]] void* const userData,
                                                                                [[maybe_unused]] zwp_locked_pointer_v1* const lockedPointer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::ConfinedPointerHandleConfined([[maybe_unused]] void* const userData,
                                                                                  [[maybe_unused]] zwp_confined_pointer_v1* const confinedPointer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::ConfinedPointerHandleUnconfined([[maybe_unused]] void* const userData,
                                                                                    [[maybe_unused]] zwp_confined_pointer_v1* const confinedPointer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleMotion([[maybe_unused]] void* const userData,
                                                                          [[maybe_unused]] wl_data_device* const device,
                                                                          [[maybe_unused]] const u32 time,
                                                                          [[maybe_unused]] const wl_fixed_t xPos,
                                                                          [[maybe_unused]] const wl_fixed_t yPos)
{
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
constexpr void TRAP::INTERNAL::WindowingAPI::OutputHandleDescription([[maybe_unused]] void* const userData,
                                                                            [[maybe_unused]] wl_output* const output,
                                                                            [[maybe_unused]] const char* const description)
{
}
#endif /*WL_OUTPUT_NAME_SINCE_VERSION*/

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::SeatHandleName([[maybe_unused]] void* const userData,
                                                                   [[maybe_unused]] wl_seat* const seat,
                                                                   [[maybe_unused]] const char* const name)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::LibDecorFrameHandleDismissPopup([[maybe_unused]] libdecor_frame* const frame,
                                                                                    [[maybe_unused]] const char* const seatName,
                                                                                    [[maybe_unused]] void* const userData)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr std::optional<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModeWayland(const InternalMonitor& monitor)
{
    return monitor.CurrentMode;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSizeWayland(const InternalWindow& window,
                                                                                 i32& width, i32& height)
{
	width = window.Width;
	height = window.Height;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScaleWayland(const InternalMonitor& monitor,
                                                                                          f32& xScale, f32& yScale)
{
	xScale = monitor.Wayland.ContentScale;
	yScale = monitor.Wayland.ContentScale;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPosWayland(const InternalMonitor& monitor,
                                                                                 i32& xPos, i32& yPos)
{
    xPos = monitor.Wayland.X;
    yPos = monitor.Wayland.Y;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPosWayland(const InternalWindow& window,
                                                                                f64& xPos, f64& yPos)
{
    xPos = window.Wayland.CursorPosX;
    yPos = window.Wayland.CursorPosY;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr std::optional<f32> TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacityWayland([[maybe_unused]] const InternalWindow& window)
{
    return 1.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSizeWayland(const InternalWindow& window,
                                                                                      i32& width, i32& height)
{
	width = NumericCast<i32>(TRAP::Math::Round(NumericCast<f32>(window.Width) * window.Wayland.ContentScale));
	height = NumericCast<i32>(TRAP::Math::Round(NumericCast<f32>(window.Height) * window.Wayland.ContentScale));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScaleWayland(const InternalWindow& window,
                                                                                         f32& xScale, f32& yScale)
{
	xScale = window.Wayland.ContentScale;
	yScale = window.Wayland.ContentScale;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkAreaWayland(const InternalMonitor& monitor,
                                                                                      i32& xPos, i32& yPos,
                                                                                      i32& width, i32& height)
{
    xPos = monitor.Wayland.X;
    yPos = monitor.Wayland.Y;
    width = monitor.CurrentMode.Width;
    height = monitor.CurrentMode.Height;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::INTERNAL::WindowingAPI::PlatformWindowVisibleWayland(const InternalWindow& window)
{
    return window.Wayland.Visible;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximizedWayland(const InternalWindow& window)
{
    return window.Maximized;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMinimizedWayland([[maybe_unused]] const InternalWindow& window)
{
    //xdg-shell doesn't give any way to request whether a surface is iconified/minimized.
    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHoveredWayland(const InternalWindow& window)
{
    return window.Wayland.Hovered;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::INTERNAL::WindowingAPI::PlatformRawMouseMotionSupportedWayland()
{
    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotionWayland([[maybe_unused]] const InternalWindow& window,
                                                                                     [[maybe_unused]] const bool enabled)
{
    //This is handled in RelativePointerHandleRelativeMotion
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPosWayland([[maybe_unused]] const InternalWindow& window,
                                                                         [[maybe_unused]] const i32 xPos,
                                                                         [[maybe_unused]] const i32 yPos)
{
    //A Wayland client can't set its position, so just warn

    InputError(Error::Feature_Unavailable, "[Wayland] The platform doesn't support setting the window position");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModesWayland(const InternalMonitor& monitor)
{
    return monitor.Modes;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindowWayland([[maybe_unused]] const InternalWindow& window)
{
    InputError(Error::Feature_Unavailable, "[Wayland] The platform does not support setting the input focus");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPosWayland([[maybe_unused]] InternalWindow& window,
                                                                         [[maybe_unused]] const f64 xPos,
                                                                         [[maybe_unused]] const f64 yPos)
{
    InputError(Error::Feature_Unavailable, "[Wayland] The platform does not support setting the cursor position");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIconWayland([[maybe_unused]] InternalWindow& window,
                                                                          [[maybe_unused]] const Image* const image)
{
    InputError(Error::Feature_Unavailable, "[Wayland] The platform does not support setting the window icon, use a .desktop file instead");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPosWayland([[maybe_unused]] const InternalWindow& window,
                                                                         [[maybe_unused]] i32& xPos,
                                                                         [[maybe_unused]] i32& yPos)
{
    //A Wayland client is not aware of its position, so just warn and leave it as (0, 0)

    InputError(Error::Feature_Unavailable, "[Wayland] The platform does not provide the window position");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloatingWayland([[maybe_unused]] const InternalWindow& window,
                                                                              [[maybe_unused]] const bool enabled)
{
    InputError(Error::Feature_Unavailable, "[Wayland] Platform does not support making a window floating");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacityWayland([[maybe_unused]] const InternalWindow& window,
                                                                             [[maybe_unused]] const f32 opacity)
{
    InputError(Error::Feature_Unavailable, "[Wayland] Platform does not support setting the window opacity");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbarWayland([[maybe_unused]] InternalWindow& window)
{
    InputError(Error::Feature_Unavailable, "[Wayland] Platform does not support hiding windows from the taskbar");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDropWayland([[maybe_unused]] InternalWindow& window,
                                                                           [[maybe_unused]] const bool value)
{
    InputError(Error::Feature_Unavailable, "[Wayland] Platform does not support toggling drag and drop. Drag and drop is enabled by default.");
}

//-------------------------------------------------------------------------------------------------------------------//

//Convert XKB KeySym to Unicode
[[nodiscard]] constexpr std::optional<u32> TRAP::INTERNAL::WindowingAPI::KeySymToUnicode(const u32 keySym)
{
	//First check for Latin-1 characters (1:1 mapping)
	if((keySym >= 0x0020u && keySym <= 0x007Eu) ||
	   (keySym >= 0x00A0u && keySym <= 0x00FFu))
	{
		return keySym;
	}

	//Also check for directly encoded 24-bit UCS characters
	if((keySym & 0xFF000000u) == 0x01000000u)
		return keySym & 0x00FFFFFFu;

	if(std::ranges::binary_search(KeySymTab, CodePair{static_cast<u16>(keySym), 0u},
	                      [](const CodePair& lhs, const CodePair& rhs){ return lhs.keySym < rhs.keySym; }))
	{
		const auto *const it = std::ranges::lower_bound(KeySymTab, CodePair{static_cast<u16>(keySym), 0u},
		                                                [](const CodePair& lhs, const CodePair& rhs){ return lhs.keySym < rhs.keySym; });
		return it->UCS;
	}

	//No matching Unicode value found
	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//
//Windows------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

#elif defined(TRAP_PLATFORM_WINDOWS)

//Returns the window style for the specified window
[[nodiscard]] constexpr DWORD TRAP::INTERNAL::WindowingAPI::GetWindowStyle(const InternalWindow& window)
{
	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (window.Monitor)
		style |= WS_POPUP;
	else
	{
		style |= WS_SYSMENU | WS_MINIMIZEBOX;

		if (window.Decorated)
		{
			style |= WS_CAPTION;

			if (window.Resizable)
				style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
		}
		else
			style |= WS_POPUP;
	}

	return style;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the extended window style for the specified window
[[nodiscard]] constexpr DWORD TRAP::INTERNAL::WindowingAPI::GetWindowExStyle(const InternalWindow& window)
{
	DWORD style = WS_EX_APPWINDOW;

	if (window.Monitor || window.Floating)
		style |= WS_EX_TOPMOST;

	return style;
}

#endif

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_WINDOWINGAPI_H*/
