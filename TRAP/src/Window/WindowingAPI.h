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

#include "Core/PlatformDetection.h"

#include <forward_list>

#ifdef TRAP_PLATFORM_LINUX
#include "Utils/Linux.h"
#elif defined(TRAP_PLATFORM_WINDOWS)
#include "Utils/Win.h"
#endif

#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#include "ImageLoader/Image.h"
#include "Input/Input.h"

namespace TRAP::INTERNAL
{
	class WindowingAPI final
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		WindowingAPI() = delete;
		/// <summary>
		/// Destructor.
		/// </summary>
		~WindowingAPI() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		WindowingAPI(const WindowingAPI&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		WindowingAPI& operator=(const WindowingAPI&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		WindowingAPI(WindowingAPI&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		WindowingAPI& operator=(WindowingAPI&&) = delete;

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
		struct VkWaylandSurfaceCreateInfoKHR;
		struct VkXlibSurfaceCreateInfoKHR;
		struct VkXcbSurfaceCreateInfoKHR;
		//---------//
		//Windows//
		//---------//
#ifdef TRAP_PLATFORM_WINDOWS
		enum class Monitor_DPI_Type;
		enum class Process_DPI_Awareness;
#elif defined(TRAP_PLATFORM_LINUX)
		struct xcb_connection_t;
		using xcb_window_t = XID;
		using xcb_visualid_t = XID;
		struct wl_cursor_theme;
		struct wl_cursor_image;
		struct wl_cursor;
#endif
		//-------------------------------------------------------------------------------------------------------------------//
		//Typedefs-----------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Callbacks//
		//---------//
	public:
		/// <summary>
		/// The function pointer type for window position callbacks.
		/// </summary>
		/// <param name="window">The window that was moved.</param>
		/// <param name="xPos">
		/// The new x-coordinate, in screen coordinates, of the upper-left corner of the content area.
		/// </param>
		/// <param name="yPos">
		/// The new y-coordinate, in screen coordinates, of the upper-left corner of the content area.
		/// </param>
		using WindowPositionFunc = void(*)(const InternalWindow* window, int32_t xPos, int32_t yPos);
		/// <summary>
		/// The function pointer type for window size callbacks.
		/// </summary>
		/// <param name="window">The window that was resized.</param>
		/// <param name="width">The new width, in screen coordinates.</param>
		/// <param name="height">The new height, in screen coordinates.</param>
		using WindowSizeFunc = void(*)(const InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// The function pointer type for window minimize/iconfiy callbacks.
		/// </summary>
		/// <param name="window">The window that was minimized/iconified or restored.</param>
		/// <param name="restored">
		/// True if the window was minimized/iconified, or false if it was restored.
		/// </param>
		using WindowMinimizeFunc = void(*)(const InternalWindow* window, bool restored);
		/// <summary>
		/// The function pointer type for window maximize callbacks.
		/// </summary>
		/// <param name="window">The window that was maximized or restored.</param>
		/// <param name="restored">True if the window was maximized, or false if it was restored.</param>
		using WindowMaximizeFunc = void(*)(const InternalWindow* window, bool restored);
		/// <summary>
		/// The function pointer type for window close callbacks.
		/// </summary>
		/// <param name="window">The window that the user attempted to close.</param>
		using WindowCloseFunc = void(*)(const InternalWindow* window);
		/// <summary>
		/// The function pointer type for window focus callbacks.
		/// </summary>
		/// <param name="window">The window that gained or lost input focus.</param>
		/// <param name="focused">True if the window was given input focus, or false if it lost it.</param>
		using WindowFocusFunc = void(*)(const InternalWindow* window, bool focused);
		/// <summary>
		/// The function pointer type for framebuffer size callbacks.
		/// </summary>
		/// <param name="window">The window whose framebuffer was resized.</param>
		/// <param name="width">The new width, in pixels, of the framebuffer.</param>
		/// <param name="height">The new height, in pixels, of the framebuffer.</param>
		using FrameBufferSizeFunc = void(*)(const InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// The function pointer type for window content scale callbacks.
		/// </summary>
		/// <param name="window">The window whose content scale changed.</param>
		/// <param name="xScale">The new x-axis content scale of the window.</param>
		/// <param name="yScale">The new y-axis content scale of the window.</param>
		using WindowContentScaleFunc = void(*)(const InternalWindow* window, float xScale, float yScale);
		/// <summary>
		/// The function pointer type for mouse button callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="mouseButton">The mouse button that was pressed or released.</param>
		/// <param name="state">State of the mouse button (pressed or released).</param>
		using MouseButtonFunc = void(*)(const InternalWindow* window, Input::MouseButton mouseButton, TRAP::Input::KeyState state);
		/// <summary>
		/// The function pointer type for cursor position callbacks.
		/// </summary>
		/// <param name="window>The window that received the event.</param>
		/// <param name="xPos">The new cursor x-coordinate, relative to the left edge of the content area.</param>
		/// <param name="yPos">The new cursor y-coordinate, relative to the top edge of the content area.</param>
		using CursorPositionFunc = void(*)(const InternalWindow* window, double xPos, double yPos);
		/// <summary>
		/// The function pointer type for cursor enter callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="entered">True if the cursor entered the window's content area, false if it left it.</param>
		using CursorEnterFunc = void(*)(const InternalWindow* window, bool entered);
		/// <summary>
		/// The function pointer type for scroll callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="xOffset">The scroll offset along the x-axis.</param>
		/// <param name="yOffset">The scroll offset along the y-axis.</param>
		using ScrollFunc = void(*)(const InternalWindow* window, double xOffset, double yOffset);
		/// <summary>
		/// The function pointer type for keyboard key callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="key">The key that was pressed, repeated or released.</param>
		/// <param name="state">The state of the key.</param>
		using KeyFunc = void(*)(const InternalWindow* window, Input::Key key, Input::KeyState state);
		/// <summary>
		/// The function pointer type for Unicode character callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="codePoint">The Unicode code point of the character.</param>
		using CharFunc = void(*)(const InternalWindow* window, uint32_t codePoint);
		/// <summary>
		/// The function pointer type for path drop callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="paths">The UTF-8 encoded file and/or directory path names.</param>
		using DropFunc = void(*)(const InternalWindow* window, std::vector<std::string> paths);
		/// <summary>
		/// The function pointer type for monitor configuration callbacks.
		/// </summary>
		/// <param name="monitor">The monitor that was connected or disconnected.</param>
		/// <param name="connceted">True if monitor got connceted or false if it got disconnceted.</param>
		using MonitorFunc = void(*)(const InternalMonitor* monitor, bool connected);
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
		using PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR = VkBool32(*)(VkPhysicalDevice, uint32_t,
		                                                                      Display*, VisualID);
		using PFN_vkCreateXcbSurfaceKHR = VkResult(*)(VkInstance, const VkXcbSurfaceCreateInfoKHR*,
		                                              const VkAllocationCallbacks*, VkSurfaceKHR*);
		using PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR = VkBool32(*)(VkPhysicalDevice, uint32_t,
		                                                                     xcb_connection_t*, xcb_visualid_t);
		using PFN_vkCreateWaylandSurfaceKHR = VkResult(*)(VkInstance, const VkWaylandSurfaceCreateInfoKHR*,
		                                              const VkAllocationCallbacks*, VkSurfaceKHR*);
		using PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR = VkBool32(*)(VkPhysicalDevice, uint32_t,
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
		using PFN_GetSystemMetricsForDPI = int(WINAPI*)(int, UINT);

		//dwmapi.dll function pointer typedefs
		using PFN_DwmIsCompositionEnabled = HRESULT(WINAPI*)(BOOL*);
		using PFN_DwmFlush = HRESULT(WINAPI*)();
		using PFN_DwmEnableBlurBehindWindow = HRESULT(WINAPI*)(HWND, const DWM_BLURBEHIND*);
		using PFN_DwmSetWindowAttribute = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);

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
		using PFN_XRRQueryExtension = int32_t(*)(Display*, int32_t*, int32_t*);
		using PFN_XRRQueryVersion = int32_t(*)(Display*, int32_t*, int32_t*);
		using PFN_XRRSelectInput = void(*)(Display*, ::Window, int32_t);
		using PFN_XRRSetCrtcConfig = int32_t(*)(Display*, XRRScreenResources*, RRCrtc, Time, int32_t, int32_t,
		                                        RRMode, Rotation, RROutput*, int32_t);
		using PFN_XRRUpdateConfiguration = int32_t(*)(XEvent*);

		//XCursor
		using PFN_XcursorImageCreate = XcursorImage*(*)(int32_t, int32_t);
		using PFN_XcursorImageDestroy = void(*)(XcursorImage*);
		using PFN_XcursorImageLoadCursor = Cursor(*)(Display*, const XcursorImage*);
		using PFN_XcursorGetTheme = char*(*)(Display*);
		using PFN_XcursorGetDefaultSize = int32_t(*)(Display*);
		using PFN_XcursorLibraryLoadImage = XcursorImage*(*)(const char*, const char*, int32_t);

		//Xinerama
		using PFN_XineramaIsActive = int32_t (*)(Display*);
		using PFN_XineramaQueryExtension = int32_t(*)(Display*, int32_t*, int32_t*);
		using PFN_XineramaQueryScreens = XineramaScreenInfo*(*)(Display*, int32_t*);

		//XCB
		using PFN_XGetXCBConnection = xcb_connection_t*(*)(Display*);

		//XI
		using PFN_XIQueryVersion = int32_t(*)(Display*, int32_t*, int32_t*);
		using PFN_XISelectEvents = int32_t(*)(Display*, ::Window, XIEventMask*, int32_t);

		//XRender
		using PFN_XRenderQueryExtension = int32_t(*)(Display*,int32_t*, int32_t*);
		using PFN_XRenderQueryVersion = int32_t(*)(Display* dpy, int32_t*, int32_t*);
		using PFN_XRenderFindVisualFormat = XRenderPictFormat*(*)(Display*, Visual const*);

		//Vulkan
		using VkXlibSurfaceCreateFlagsKHR = VkFlags;
		using VkXcbSurfaceCreateFlagsKHR = VkFlags;
		using VkWaylandSurfaceCreateFlagsKHR = VkFlags;

		//XShape
		using PFN_XShapeQueryExtension = int32_t(*)(Display*, int32_t*, int32_t*);
		using PFN_XShapeQueryVersion = int32_t(*)(Display* dpy, int32_t*, int32_t*);
		using PFN_XShapeCombineRegion = void(*)(Display*, ::Window, int32_t, int32_t, int32_t, Region, int32_t);
		using PFN_XShapeCombineMask = void(*)(Display*, XID, int32_t, int32_t, int32_t, Pixmap, int32_t);

		//XLib
		using PFN_XAllocClassHint = XClassHint*(*)();
		using PFN_XAllocSizeHints = XSizeHints*(*)();
		using PFN_XAllocWMHints = XWMHints*(*)();
		using PFN_XChangeProperty = int(*)(Display*, ::Window, Atom, Atom, int, int, const unsigned char*, int);
		using PFN_XChangeWindowAttributes = int(*)(Display*, ::Window, unsigned long, XSetWindowAttributes*);
		using PFN_XCheckIfEvent = int(*)(Display*, XEvent*, int(*)(Display*, XEvent*, XPointer), XPointer);
		using PFN_XCheckTypedWindowEvent = int(*)(Display*, ::Window, int, XEvent*);
		using PFN_XCloseDisplay = int(*)(Display*);
		using PFN_XCloseIM = int32_t(*)(XIM);
		using PFN_XConvertSelection = int(*)(Display*, Atom, Atom, Atom, ::Window, Time);
		using PFN_XCreateColormap = Colormap(*)(Display*, ::Window, Visual*, int);
		using PFN_XCreateFontCursor = Cursor(*)(Display*, unsigned int);
		using PFN_XCreateIC = XIC(*)(XIM, ...);
		using PFN_XCreateWindow = ::Window(*)(Display*, ::Window, int, int, unsigned int, unsigned int, unsigned int, int,
		                                      unsigned int, Visual*, unsigned long, XSetWindowAttributes*);
		using PFN_XDefineCursor = int(*)(Display*, ::Window, Cursor);
		using PFN_XDeleteContext = int(*)(Display*, XID, XContext);
		using PFN_XDeleteProperty = int(*)(Display*, ::Window, Atom);
		using PFN_XDestroyIC = void(*)(XIC);
		using PFN_XDestroyWindow = int(*)(Display*, ::Window);
		using PFN_XDisplayKeycodes = int(*)(Display*, int*, int*);
		using PFN_XEventsQueued = int(*)(Display*, int);
		using PFN_XFilterEvent = int(*)(XEvent*, ::Window);
		using PFN_XFindContext = int(*)(Display*, XID, XContext, XPointer*);
		using PFN_XFlush = int(*)(Display*);
		using PFN_XFree = int(*)(void*);
		using PFN_XFreeColormap = int(*)(Display*, Colormap);
		using PFN_XFreeCursor = int(*)(Display*, Cursor);
		using PFN_XFreeEventData = void(*)(Display*, XGenericEventCookie*);
		using PFN_XGetAtomName = char*(*)(Display*, Atom);
		using PFN_XGetErrorText = int(*)(Display*, int, char*, int);
		using PFN_XGetEventData = int(*)(Display*, XGenericEventCookie*);
		using PFN_XGetICValues = char*(*)(XIC, ...);
		using PFN_XGetIMValues = char*(*)(XIM, ...);
		using PFN_XGetInputFocus = int(*)(Display*, ::Window*, int*);
		using PFN_XGetKeyboardMapping = KeySym*(*)(Display*, KeyCode, int, int*);
		using PFN_XGetScreenSaver = int(*)(Display*, int*, int*, int*, int*);
		using PFN_XGetSelectionOwner = ::Window(*)(Display*, Atom);
		using PFN_XGetVisualInfo = XVisualInfo*(*)(Display*, long, XVisualInfo*, int*);
		using PFN_XGetWMNormalHints = int32_t(*)(Display*, ::Window, XSizeHints*, long*);
		using PFN_XGetWindowAttributes = int32_t(*)(Display*, ::Window, XWindowAttributes*);
		using PFN_XGetWindowProperty = int(*)(Display*, ::Window, Atom, long, long, int, Atom, Atom*, int*, unsigned long*,
		                                      unsigned long*, unsigned char**);
		using PFN_XGrabPointer = int(*)(Display*, ::Window, int, unsigned int, int, int, ::Window, Cursor, Time);
		using PFN_XIconifyWindow = int32_t(*)(Display*, ::Window, int);
		using PFN_XInitThreads = int32_t(*)();
		using PFN_XInternAtom = Atom(*)(Display*, const char*, int);
		using PFN_XLookupString = int(*)(XKeyEvent*, char*, int, KeySym*, XComposeStatus*);
		using PFN_XMapRaised = int(*)(Display*, ::Window);
		using PFN_XMapWindow = int(*)(Display*, ::Window);
		using PFN_XMoveResizeWindow = int(*)(Display*, ::Window, int, int, unsigned int, unsigned int);
		using PFN_XMoveWindow = int(*)(Display*, ::Window, int, int);
		using PFN_XNextEvent = int(*)(Display*, XEvent*);
		using PFN_XOpenDisplay = Display*(*)(const char*);
		using PFN_XOpenIM = XIM(*)(Display*, XrmDatabase*, char*, char*);
		using PFN_XPeekEvent = int(*)(Display*, XEvent*);
		using PFN_XPending = int(*)(Display*);
		using PFN_XQueryExtension = int(*)(Display*, const char*, int*, int*, int*);
		using PFN_XQueryPointer = int(*)(Display*, ::Window, ::Window*, ::Window*, int*, int*, int*, int*, unsigned int*);
		using PFN_XRaiseWindow = int(*)(Display*, ::Window);
		using PFN_XRegisterIMInstantiateCallback = int(*)(Display*, void*, char*, char*, XIDProc, XPointer);
		using PFN_XResizeWindow = int(*)(Display*, ::Window, unsigned int, unsigned int);
		using PFN_XResourceManagerString = char*(*)(Display*);
		using PFN_XSaveContext = int(*)(Display*, XID, XContext, const char*);
		using PFN_XSelectInput = int(*)(Display*, ::Window, long);
		using PFN_XSendEvent = int32_t(*)(Display*, ::Window, int, long, XEvent*);
		using PFN_XSetClassHint = int(*)(Display*, ::Window, XClassHint*);
		using PFN_XSetErrorHandler = XErrorHandler(*)(XErrorHandler);
		using PFN_XSetICFocus = void(*)(XIC);
		using PFN_XSetIMValues = char*(*)(XIM, ...);
		using PFN_XSetInputFocus = int(*)(Display*, ::Window, int, Time);
		using PFN_XSetLocaleModifiers = char*(*)(const char*);
		using PFN_XSetScreenSaver = int(*)(Display*, int, int, int, int);
		using PFN_XSetSelectionOwner = int(*)(Display*, Atom, ::Window, Time);
		using PFN_XSetWMHints = int(*)(Display*, ::Window, XWMHints*);
		using PFN_XSetWMNormalHints = void(*)(Display*, ::Window, XSizeHints*);
		using PFN_XSetWMProtocols = int32_t(*)(Display*, ::Window, Atom*, int);
		using PFN_XSupportsLocale = int(*)();
		using PFN_XSync = int(*)(Display*, int);
		using PFN_XTranslateCoordinates = int(*)(Display*, ::Window, ::Window, int, int, int*, int*, ::Window*);
		using PFN_XUndefineCursor = int(*)(Display*, ::Window);
		using PFN_XUngrabPointer = int(*)(Display*, Time);
		using PFN_XUnmapWindow = int(*)(Display*, ::Window);
		using PFN_XUnsetICFocus = void(*)(XIC);
		using PFN_XVisualIDFromVisual = VisualID(*)(Visual*);
		using PFN_XWarpPointer = int(*)(Display*, ::Window, ::Window, int, int, unsigned int, unsigned int, int, int);
		using PFN_XCreateRegion = Region(*)();
		using PFN_XDestroyRegion = int32_t(*)(Region);

		//XKB
		using PFN_XkbAllocKeyboard = XkbDescPtr(*)();
		using PFN_XkbFreeKeyboard = void(*)(XkbDescPtr, unsigned int, int);
		using PFN_XkbFreeNames = void(*)(XkbDescPtr, unsigned int, int);
		using PFN_XkbGetMap = XkbDescPtr(*)(Display*, unsigned int, unsigned int);
		using PFN_XkbGetNames = int32_t(*)(Display*, unsigned int, XkbDescPtr);
		using PFN_XkbGetState = int32_t(*)(Display*, unsigned int, XkbStatePtr);
		using PFN_XkbKeycodeToKeysym = KeySym(*)(Display*, KeyCode, int, int);
		using PFN_XkbQueryExtension = int(*)(Display*, int*, int*, int*, int*, int*);
		using PFN_XkbSelectEventDetails = int(*)(Display*, unsigned int, unsigned int, unsigned long, unsigned long);
		using PFN_XkbSetDetectableAutoRepeat = int(*)(Display*, int, int*);
		using PFN_Xutf8LookupString = int(*)(XIC, XKeyPressedEvent*, char*, int, KeySym*, int32_t*);
		using PFN_Xutf8SetWMProperties = void(*)(Display*, ::Window, const char*, const char*, char**, int, XSizeHints*,
		                                         XWMHints*, XClassHint*);

		//XRM
		using PFN_XrmDestroyDatabase = void(*)(XrmDatabase);
		using PFN_XrmGetResource = int(*)(XrmDatabase, const char*, const char*, char**, XrmValue*);
		using PFN_XrmGetStringDatabase = XrmDatabase(*)(const char*);
		using PFN_XrmInitialize = void(*)();
		using PFN_XrmUniqueQuark = XrmQuark(*)();
		using PFN_XUnregisterIMInstantiateCallback = int(*)(Display*, void*, char*, char*, XIDProc, XPointer);

		//--------------//
		//Linux(Wayland)//
		//--------------//

		//Display
		using PFN_wl_display_flush = int(*)(wl_display*);
		using PFN_wl_display_cancel_read = void(*)(wl_display*);
		using PFN_wl_display_dispatch_pending = int(*)(wl_display*);
		using PFN_wl_display_read_events = int(*)(wl_display*);
		using PFN_wl_display_connect = wl_display*(*)(const char*);
		using PFN_wl_display_disconnect = void(*)(wl_display*);
		using PFN_wl_display_roundtrip = int(*)(wl_display*);
		using PFN_wl_display_get_fd = int(*)(wl_display*);
		using PFN_wl_display_prepare_read = int(*)(wl_display*);

		//Proxy
		using PFN_wl_proxy_marshal = void(*)(wl_proxy*, uint32_t, ...);
		using PFN_wl_proxy_add_listener = int(*)(wl_proxy*, void(**)(void), void*);
		using PFN_wl_proxy_destroy = void(*)(wl_proxy*);
		using PFN_wl_proxy_marshal_constructor = wl_proxy*(*)(wl_proxy*, uint32_t, const wl_interface*, ...);
		using PFN_wl_proxy_marshal_constructor_versioned = wl_proxy*(*)(wl_proxy*, uint32_t, const wl_interface*, uint32_t, ...);
		using PFN_wl_proxy_get_user_data = void*(*)(wl_proxy*);
		using PFN_wl_proxy_set_user_data = void*(*)(wl_proxy*, void*);
		using PFN_wl_proxy_get_version = uint32_t(*)(wl_proxy*);
		using PFN_wl_proxy_marshal_flags = wl_proxy*(*)(wl_proxy*, uint32_t, const wl_interface*, uint32_t, uint32_t, ...);

		//Cursor
		using PFN_wl_cursor_theme_load = wl_cursor_theme*(*)(const char*, int, wl_shm*);
		using PFN_wl_cursor_theme_destroy = void(*)(wl_cursor_theme*);
		using PFN_wl_cursor_theme_get_cursor = wl_cursor*(*)(wl_cursor_theme*, const char*);
		using PFN_wl_cursor_image_get_buffer = wl_buffer*(*)(wl_cursor_image*);

		//XKB
		using PFN_xkb_context_new = xkb_context*(*)(xkb_context_flags);
		using PFN_xkb_context_unref = void(*)(xkb_context*);
		using PFN_xkb_keymap_new_from_string = xkb_keymap*(*)(xkb_context*, const char*, xkb_keymap_format, xkb_keymap_compile_flags);
		using PFN_xkb_keymap_unref = void(*)(xkb_keymap*);
		using PFN_xkb_keymap_mod_get_index = xkb_mod_index_t(*)(xkb_keymap*, const char*);
		using PFN_xkb_keymap_key_repeats = int(*)(xkb_keymap*, xkb_keycode_t);
		using PFN_xkb_keymap_key_get_syms_by_level = int(*)(xkb_keymap*, xkb_keycode_t, xkb_layout_index_t, xkb_level_index_t, const xkb_keysym_t**);
		using PFN_xkb_state_new = xkb_state*(*)(xkb_keymap*);
		using PFN_xkb_state_unref = void(*)(xkb_state*);
		using PFN_xkb_state_key_get_syms = int(*)(xkb_state*, xkb_keycode_t, const xkb_keysym_t**);
		using PFN_xkb_state_update_mask = xkb_state_component(*)(xkb_state*, xkb_mod_mask_t, xkb_mod_mask_t, xkb_mod_mask_t, xkb_layout_index_t, xkb_layout_index_t, xkb_layout_index_t);
		using PFN_xkb_state_key_get_layout = xkb_layout_index_t(*)(xkb_state*, xkb_keycode_t);
		using PFN_xkb_state_mod_index_is_active = int(*)(xkb_state*, xkb_mod_index_t, xkb_state_component);

		//XKB Compose
		using PFN_xkb_compose_table_new_from_locale = xkb_compose_table*(*)(xkb_context*, const char*, xkb_compose_compile_flags);
		using PFN_xkb_compose_table_unref = void(*)(xkb_compose_table*);
		using PFN_xkb_compose_state_new = xkb_compose_state*(*)(xkb_compose_table*, xkb_compose_state_flags);
		using PFN_xkb_compose_state_unref = void(*)(xkb_compose_state*);
		using PFN_xkb_compose_state_feed = xkb_compose_feed_result(*)(xkb_compose_state*, xkb_keysym_t);
		using PFN_xkb_compose_state_get_status = xkb_compose_status(*)(xkb_compose_state*);
		using PFN_xkb_compose_state_get_one_sym = xkb_keysym_t(*)(xkb_compose_state*);
#endif
		//-------------------------------------------------------------------------------------------------------------------//
		//Enums--------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
	public:
		/// <summary>
		/// Error codes.
		/// </summary>
		enum class Error
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

		/// <summary>
		/// Hints for window creation and runtime changes.
		/// </summary>
		enum class Hint
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

		/// <summary>
		/// Cursor modes that can be used.
		/// </summary>
		enum class CursorMode
		{
			Normal,   //Makes the cursor visible and behaving normally.
			Hidden,   //Makes the cursor invisible when it is over the
			          //content area of the window but does not restrict the cursor from leaving.
			Disabled, //Hides and grabs the cursor, providing virtual and unlimited cursor movement.
			Captured  //Hides and grabs the cursor, without providing virtual cursor movement.
		};

		/// <summary>
		/// Cursor types that can be used.
		/// </summary>
		enum class CursorType
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

		/// <summary>
		/// State of progress for a window.
		/// </summary>
		enum class ProgressState
		{
			NoProgress = 0x00,
			Indeterminate = 0x01,
			Normal = 0x02,
			Error = 0x04,
			Paused = 0x08
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
#endif
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
			uint32_t width;
			uint32_t height;
			uint32_t hotspot_x;
			uint32_t hotspot_y;
			uint32_t delay;
		};

		struct wl_cursor
		{
			uint32_t image_count;
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
#endif

		/// <summary>
		/// Window configuration.
		/// </summary>
		struct WindowConfig
		{
			uint32_t Width = 0;
			uint32_t Height = 0;
			std::string Title{};
			bool Resizable = false;
			bool Visible = false;
			bool Decorated = true;
			bool Maximized = false;
			bool Focused = false;
			bool Floating = false;
			bool FocusOnShow = true;
			bool MousePassthrough = false;
		};

		/// <summary>
		/// Global state.
		/// </summary>
		struct Data
		{
			bool Initialized = false;

			struct
			{
				WindowConfig Window{};
			} Hints;

			std::forward_list<InternalWindow*> WindowList{};
			std::forward_list<InternalCursor*> CursorList{};

			std::vector<Scope<InternalMonitor>> Monitors{};

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
#elif defined(TRAP_PLATFORM_LINUX)
				bool KHR_XLib_Surface = false;
				bool KHR_XCB_Surface = false;
				bool KHR_Wayland_Surface = false;
#endif
			} VK{};

			std::string ClipboardString{};
			std::array<std::array<char, 5>, static_cast<uint32_t>(Input::Key::Menu) + 1> KeyNames{};
			std::array<int16_t, static_cast<uint32_t>(Input::Key::Menu) + 1> ScanCodes{};
			//Where to place the cursor when re-enabled
			double RestoreCursorPosX = 0.0, RestoreCursorPosY = 0.0;
			//The window whose disabled cursor mode is active
			InternalWindow* DisabledCursorWindow = nullptr;
			std::array<int, 2> EmptyEventPipe{};
			//The window the cursor is captured in
			InternalWindow* CapturedCursorWindow = nullptr;

#ifdef TRAP_PLATFORM_WINDOWS
			std::array<Input::Key, 512> KeyCodes{};
			HINSTANCE Instance = nullptr;
			HWND HelperWindowHandle = nullptr;
			ATOM HelperWindowClass{};
			ATOM MainWindowClass{};
			HDEVNOTIFY DeviceNotificationHandle = nullptr;
			int32_t AcquiredMonitorCount = 0;
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
				PFN_GetSystemMetricsForDPI GetSystemMetricsForDPI = nullptr;
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
				PFN_SetProcessDPIAwareness SetProcessDPIAwareness = nullptr;
				PFN_GetDPIForMonitor GetDPIForMonitor = nullptr;
			} SHCore;
#elif defined(TRAP_PLATFORM_LINUX)
			std::array<Input::Key, 256> KeyCodes{};

			struct x11
			{
				Display* display = nullptr;
				int32_t Screen = 0;
				::Window Root{};

				//System content scale
				float ContentScaleX = 0, ContentScaleY = 0;
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
				int32_t ErrorCode = 0;
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
				Atom NET_FRAME_EXTENTS{};
				Atom NET_REQUEST_FRAME_EXTENTS{};
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
					int32_t Major = 0;
					int32_t Minor = 0;
					int32_t EventBase = 0;
					int32_t ErrorBase = 0;
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
					int32_t EventBase = 0;
					int32_t ErrorBase = 0;
					int32_t Major = 0;
					int32_t Minor = 0;
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
					int32_t MajorOPCode = 0;
					int32_t EventBase = 0;
					int32_t ErrorBase = 0;
					int32_t Major = 0;
					int32_t Minor = 0;
					uint32_t Group = 0;
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
					int32_t Count = 0;
					int32_t Timeout = 0;
					int32_t Interval = 0;
					int32_t Blanking = 0;
					int32_t Exposure = 0;
				} Saver{};

				struct xdnd
				{
					int32_t Version = 0;
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
					int32_t Major = 0;
					int32_t Minor = 0;
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
					int32_t MajorOPCode = 0;
					int32_t EventBase = 0;
					int32_t ErrorBase = 0;
					int32_t Major = 0;
					int32_t Minor = 0;
					PFN_XIQueryVersion QueryVersion{};
					PFN_XISelectEvents SelectEvents{};
				} XI{};

				struct XRENDER
				{
					bool Available = false;
					void* Handle = nullptr;
					int32_t Major = 0;
					int32_t Minor = 0;
					int32_t EventBase = 0;
					int32_t ErrorBase = 0;
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

				std::vector<TRAPOfferWayland> Offers;

				wl_data_offer* SelectionOffer;
				wl_data_source* SelectionSource;

				wl_data_offer* DragOffer;
				InternalWindow* DragFocus;
				uint32_t DragSerial;

				int32_t CompositorVersion;
				int32_t SeatVersion;

				wl_cursor_theme* CursorTheme;
				wl_cursor_theme* CursorThemeHiDPI;
				wl_surface* CursorSurface;
				const char* CursorPreviousName;
				int32_t CursorTimerFD;
				uint32_t Serial;
				uint32_t PointerEnterSerial;

				int32_t KeyRepeatTimerFD;
				int32_t KeyRepeatRate;
				int32_t KeyRepeatDelay;
				int32_t KeyRepeatScancode;

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
					uint32_t Modifiers;

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
					PFN_xkb_state_mod_index_is_active StateModIndexIsActive;

					PFN_xkb_compose_table_new_from_locale ComposeTableNewFromLocale;
					PFN_xkb_compose_table_unref ComposeTableUnref;
					PFN_xkb_compose_state_new ComposeStateNew;
					PFN_xkb_compose_state_unref ComposeStateUnref;
					PFN_xkb_compose_state_feed ComposeStateFeed;
					PFN_xkb_compose_state_get_status ComposeStateGetStatus;
					PFN_xkb_compose_state_get_one_sym ComposeStateGetOneSym;
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
			} Wayland{};
#endif
		};
	public:
		/// <summary>
		/// This describes a single video mode.
		/// </summary>
		struct InternalVideoMode
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

		/// <summary>
		/// Monitor structure.
		/// </summary>
		struct InternalMonitor
		{
			std::string Name{};

			//The window whose video mode is current on this monitor
			InternalWindow* Window = nullptr;

			std::vector<InternalVideoMode> Modes{};
			InternalVideoMode CurrentMode{};

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
				RROutput Output = 0;
				RRCrtc CRTC = 0;
				RRMode OldMode = 0;

				//Index of corresponding Xinerama screen, for EWMH full screen window placement
				int32_t Index = 0;
			} X11;

			struct wayland
			{
				wl_output* Output;
				uint32_t Name;
				int32_t X;
				int32_t Y;
				int32_t Scale;
			} Wayland;
#endif
		};

		/// <summary>
		/// Cursor structure.
		/// </summary>
		struct InternalCursor
		{
#ifdef TRAP_PLATFORM_WINDOWS
			HCURSOR Handle = nullptr;
#elif defined(TRAP_PLATFORM_LINUX)
			struct x11
			{
				Cursor Handle = 0;
			} X11;

			struct wayland
			{
				wl_cursor* CursorWL;
				wl_cursor* CursorHiDPI;
				wl_buffer* Buffer;
				int32_t Width, Height;
				int32_t XHotspot, YHotspot;
				int32_t CurrentImage;
			};
#endif
		};

		/// <summary>
		/// Window structure.
		/// </summary>
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

			int32_t MinWidth = -1, MinHeight = -1;
			int32_t MaxWidth = -1, MaxHeight = -1;

			CursorMode cursorMode = CursorMode::Normal;
			std::array<TRAP::Input::KeyState, static_cast<uint32_t>(TRAP::Input::MouseButton::Eight) + 1> MouseButtons{};
			std::array<TRAP::Input::KeyState, static_cast<uint32_t>(TRAP::Input::Key::Menu) + 1> Keys{};
			//Virtual cursor position when cursor is disabled
			double VirtualCursorPosX = 0.0, VirtualCursorPosY = 0.0;
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
			} Callbacks;

			bool Minimized = false;
			bool Maximized = false;

			//The last received cursor position, regardless of source
			int32_t LastCursorPosX = 0, LastCursorPosY = 0;

			int32_t Width = 0, Height = 0;

#ifdef TRAP_PLATFORM_WINDOWS
			HWND Handle = nullptr;
			HICON BigIcon = nullptr;
			HICON SmallIcon = nullptr;

			bool CursorTracked = false;
			bool FrameAction = false;

			//The last received high surrogate when decoding pairs of UTF-16 messages
			WCHAR HighSurrogate{};

			ITaskbarList3* TaskbarList = nullptr;
			UINT TaskbarListMsgID = 0;
#elif defined(TRAP_PLATFORM_LINUX)
			struct x11
			{
				//X11
				Colormap colormap = 0;
				::Window Handle = 0;
				::Window Parent = 0;
				XIC IC = nullptr;
				bool OverrideRedirect = false;
				//Whether the visual supports framebuffer transparency
				bool Transparent = false;
				//Cached position and size used to filter out duplicate events
				int32_t XPos = 0, YPos = 0;
				//The last position the cursor was warped to by TRAP
				int32_t WarpCursorPosX = 0, WarpCursorPosY = 0;

				//The time of the last KeyPress event per keycode, for discarding
				//duplicate key events generated from some keys by ibus
				std::array<Time, 256> KeyPressTimes;
			} X11;

			struct wayland
			{
				//Wayland
				bool Visible;
				bool Activated;
				bool Fullscreen;
				bool Hovered;
				wl_surface* Surface;
				wl_callback* Callback;

				struct
				{
					int32_t Width, Height;
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
					uint32_t Decorationmode;
				} XDG;

				char* Title;
				char* AppID;

				int32_t Scale;
				std::vector<InternalMonitor*> Monitors;
				int32_t MonitorsCount;
				int32_t MonitorsSize;

				zwp_relative_pointer_v1* RelativePointer;
				zwp_locked_pointer_v1* LockedPointer;
				zwp_confined_pointer_v1* ConfinedPointer;

				zwp_idle_inhibitor_v1* IdleInhibitor;

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
		/// <summary>
		/// Initializes the windowing API.
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
		/// Remarks:
		/// X11: This function will set the LC_CTYPE category of the engine locale according to the current environment
		///      if that category is still "C". This is because the "C" locale breaks Unicode text input.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>True if windowing API was successfully initialized, false otherwise.</returns>
		static bool Init();
		/// <summary>
		/// This function destroys all remaining windows and cursor, and frees any other allocated resources.
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
		/// Remarks: This function may be called before WindowingAPI::Init.
		/// Reentrancy: This function must not be called from a callback.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		static void Shutdown();
		/// <summary>
		/// This function destroys the specified window. On calling this function, no
		/// further callbacks will be called for that window.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Reentrancy: This function must not be called from a callback.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to destroy.</param>
		static void DestroyWindow(Scope<InternalWindow> window);
		/// <summary>
		/// Resets all window hints to their default values.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		static void DefaultWindowHints();
		/// <summary>
		/// This function sets hints for the next call to WindowingAPI::CreateWindow.
		/// The hints, once set, retain their values until changed by a call to this function
		/// or WindowingAPI::DefaultWindowHints, or until the engine is terminated.
		///
		/// Some hints are platform specific. These may be set on any platform but they will only
		/// affect their specific platform. Other platforms will ignore them. Setting these hints
		/// requires no platform specific headers or functions.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="hint">Hint to set.</param>
		/// <param name="value">Value to set for the Hint.</param>
		static void WindowHint(Hint hint, bool value);
		/// <summary>
		/// This function returns a human-readable name, encoded as UTF-8, of the specified monitor.
		/// The name typically reflects the make and model of the monitor and is not guaranteed to be
		/// unique among the connected monitors.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">InternalMonitor to query name from.</param>
		/// <returns>Name of the provided monitor.</returns>
		static std::string GetMonitorName(const InternalMonitor* monitor);
		/// <summary>
		/// This function returns the primary monitor. This is usually the monitor
		/// where elements like the task bar or global menu bar are located.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// Remarks: The primary monitor is always first in the vector returned by WindowingAPI::GetMonitors.
		/// </summary>
		/// <returns>
		/// Primary internal monitor, or nullptr if no monitors were found or if an error occurred.
		/// </returns>
		static InternalMonitor* GetPrimaryMonitor();
		/// <summary>
		/// This function returns a vector of handles for all currently connceted
		/// monitors. The primary monitor is always first in the returned vector.
		/// If no monitors were found, this function returns an empty vector.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>
		/// Vector of all connected monitor handles, or an empty vector if no monitors were found or
		/// if an error occurred.
		/// </returns>
		static std::vector<InternalMonitor*> GetMonitors();
		/// <summary>
		/// This function returns the current video mode of the specified monitor.
		/// If you have created a full screen window for that monitor, the return
		/// value will depend on whether that window is minimized/iconified.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor to get internal video mode from.</param>
		/// <returns>
		/// Current video mode of the specified monitor, or an empty one if an error occurred.
		/// </returns>
		static InternalVideoMode GetVideoMode(InternalMonitor* monitor);
		/// <summary>
		/// This function returns a vector of all video modes supported by the specified monitor.
		/// The returned vector is sorted in ascending order, first by color bit depth
		/// (the sum of all channel depths) and then by resolution area (the product of
		/// width and height).
		///
		/// Errors: Possible errors include: Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor to get all internal video modes from.</param>
		/// <returns>
		/// Vector with all available video modes of the specified monitor, or an empty vector
		/// if an error occurred.
		/// </returns>
		static std::vector<InternalVideoMode> GetVideoModes(InternalMonitor* monitor);
		/// <summary>
		/// This function creates a window.
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
		/// Remarks:
		///     Windows: If the executable has an icon resource named 'TRAP_ICON', it will be set as the initial
		///              icon for the window. If no such icon is present, the IDI_APPLICATION icon will be used
		///              instead. To set a different icon, see WindowingAPI::SetWindowIcon.
		///     X11: Some window managers will not respect the placement of initially hidden windows.
		///          Due to the asynchronous nature of X11, it may take a moment for a window to reach its
		///          requested state. This means you may not be able to query the final size, position or
		///          other attributes directly after window creation.
		///          The class part of the WM_CLASS window property will by default be set to the window
		///          title passed to this function. The instance part will use the contents of the
		///          RESOURCE_NAME environment variable, if present and not empty, or fall back to the
		///          window title.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="width">Desired width for the new window. Must be greater than zero.</param>
		/// <param name="height">Desired height for the new window. Must be greater than zero.</param>
		/// <param name="title">UTF-8 encoded title for the new window.</param>
		/// <param name="monitor">Optional monitor to use for the new window.</param>
		/// <returns>On success a new internal window, or nullptr if an error occurred.</returns>
		static Scope<InternalWindow> CreateWindow(uint32_t width, uint32_t height, std::string title,
		                                          InternalMonitor* monitor);
		/// <summary>
		/// This function sets the value of the close flag of the specified window. This can be used
		/// to override the user's attempt to close the window, or to signal that it should be closed.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function may be called from any thread. Access is not synchronized.
		/// </summary>
		/// <param name="window">Internal window whose flag to change.</param>
		/// <param name="value">Whether to close the window or not.</param>
		static void SetWindowShouldClose(InternalWindow* window, bool value);
		/// <summary>
		/// This function sets the window title, encoded as UTF-8, of the specified window.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window whose title to change.</param>
		/// <param name="title">New UTF-8 encoded title for the window.</param>
		static void SetWindowTitle(const InternalWindow* window, const std::string& title);
		/// <summary>
		/// This function retrieves the content scale for the specified monitor.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor.</param>
		/// <param name="xScale">Output variable for the X scale of the provided monitor.</param>
		/// <param name="yScale">Output variable for the Y scale of the provided monitor.</param>
		static void GetMonitorContentScale(const InternalMonitor* monitor, float& xScale, float& yScale);
		/// <summary>
		/// This function destroys a cursor previously created with CreateCursor. Any remaining cursors
		/// will be destroyed by WindowingAPI::Shutdown.
		///
		/// If the specified cursor is current for any window, that window will be reverted to the default
		/// cursor. This does not affect the cursor mode.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Reentrancy: This function must not be called from a callback.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="cursor">Internal cursor to be destroyed.</param>
		static void DestroyCursor(Scope<InternalCursor> cursor);
		/// <summary>
		/// Creates a new custom cursor image that can be set for a window with SetCursor. The cursor can
		/// be destroyed with DestroyCursor. Any remaining cursors are destroyed by WindowingAPI::Shutdown.
		///
		/// The cursor hotspot is specified in pixels, relative to the upper-left corner of the cursor image.
		/// Like all other coordinate system in the WindowingAPI, the X-axis points to the right and the
		/// Y-axis points down.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be caled from the main thread.
		/// </summary>
		/// <param name="image">Non HDR RGB 24BPP or RGBA 32 BPP image.</param>
		/// <param name="xHotspot">Center x coordinate of the image.</param>
		/// <param name="yHotspot">Center y coordinate of the image.</param>
		/// <returns>On success new internal cursor, nullptr otherwise.</returns>
		static Scope<InternalCursor> CreateCursor(const Image* const image, int32_t xHotspot, int32_t yHotspot);
		/// <summary>
		/// Creates a cursor with a standard shape.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="type">Cursor type to get.</param>
		/// <returns>On success new internal cursor, nullptr otherwise.</returns>
		static Scope<InternalCursor> CreateStandardCursor(const CursorType& type);
		/// <summary>
		/// This function sets the cursor image to be used when the cursor is over the content are of the
		/// specified window. The set cursor will only be visible when the CursorMode of the window is
		/// CursorMode::Normal.
		///
		/// On some platforms, the set cursor may not be visible unless the window also has input focus.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the cursor for.</param>
		/// <param name="cursor">
		/// Internal cursor to set, or nullptr to switch back to the default arrow cursor.
		/// </param>
		static void SetCursor(InternalWindow* window, InternalCursor* cursor);
		/// <summary>
		/// This function sets the icon of the specified window. If no image is specified, the window
		/// reverts to its default icon.
		///
		/// The image mustbe RGB 24BPP or RGBA 32BPP.
		///
		/// The desired image size varies depending on platform and system settings.
		/// Good sizes include 16x16, 32x32 and 48x48.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window whose icon to set.</param>
		/// <param name="image">
		/// Image to be set as window icon or nullptr to revert back to the default icon.
		/// </param>
		static void SetWindowIcon(InternalWindow* window, const Image* const image);
		/// <summary>
		/// This function sets the position, in screen coordinates, of the upper-left corner of the
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the position for.</param>
		/// <param name="xPos">X position to be set.</param>
		/// <param name="yPos">Y position to be set.</param>
		static void SetWindowPos(const InternalWindow* window, int32_t xPos, int32_t yPos);
		/// <summary>
		/// This function retrieves the position, in screen coordinates, of the upper-left corner of the
		/// content area of the specified window.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the current position from.</param>
		/// <param name="xPos">Output variable for the current x position of the internal window.</param>
		/// <param name="yPos">Output variable for the current y position of the internal window.</param>
		static void GetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos);
		/// <summary>
		/// This function sets the size, in screen coordinates, of the content area of the specified window.
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
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the size for.</param>
		/// <param name="width">New width for the internal window.</param>
		/// <param name="height">New height for the internal window.</param>
		static void SetWindowSize(InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// This function retrieves the size, in screen coordinates, of the content area of
		/// the specified window. If you wish to retrieve the size of the framebuffer of the window
		/// in pixels, see WindowingAPI::GetFrameBufferSize.
		///
		/// If an error occurs, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the size from.</param>
		/// <param name="width">Output variable for the internal windows current width.</param>
		/// <param name="height">Output variable for the internal windows current height.</param>
		static void GetWindowSize(const InternalWindow* window, int32_t& width, int32_t& height);
		/// <summary>
		/// This function retrieves the size, in pixels, of the framebuffer of the specified window.
		/// If you wish to retrieve the size of the window in screen coordinates, see
		/// WindowingAPI::GetWindowSize.
		///
		/// If an error occcurs, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the framebuffer size from.</param>
		/// <param name="width">Output variable for the internal windows current framebuffer width.</param>
		/// <param name="height">Output variable for the internal windows current framebuffer height.</param>
		static void GetFrameBufferSize(const InternalWindow* window, int32_t& width, int32_t& height);
		/// <summary>
		/// This function sets the opacity of the window, including any decorations.
		///
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque.
		///
		/// The initial opacity value for newly created windows is 1.0f.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set opacity for.</param>
		/// <param name="opacity">Opacity ranging from 0.0f-1.0f.</param>
		static void SetWindowOpacity(const InternalWindow* window, float opacity);
		/// <summary>
		/// This function returns the opacity of the window, including any decorations.
		///
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque. If the system does not
		/// support whole window transparency, this function always returns one.
		///
		/// The initial opacity value for newly created windows is 1.0f.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the opacity from.</param>
		/// <returns>Opacity of the given internal window.</returns>
		static float GetWindowOpacity(const InternalWindow* window);
		/// <summary>
		/// This function retrieves the content scale for the specified window.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the content scale from.</param>
		/// <param name="xScale">Output variable for the internal windows content scale x.</param>
		/// <param name="yScale">Output variable for the internal windows content scale y.</param>
		static void GetWindowContentScale(const InternalWindow* window, float& xScale, float& yScale);
		/// <summary>
		/// This function sets the value of a hint of the specified window.
		///
		/// Some of these hints are ignored for full screen windows. The new value
		/// will take effect if the window is later made windowed.
		///
		/// Some of these hints are ignored for windowed mode windows. The new value
		/// will take effect if the window is later made full screen.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invaild_Enum,
		///         Error::Invalid_Value and Error::Platform_Error.
		/// Remarks: Calling WindowingAPI::GetWindowHint will always return the latest value,
		///          even if that value is ignored by the current mode of the window.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the hint for.</param>
		/// <param name="hint">Hint to be set.</param>
		/// <param name="value">Value for the hint.</param>
		static void SetWindowHint(InternalWindow* window, Hint hint, bool value);
		/// <summary>
		/// This function returns the value of a hint of the specified window.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum
		/// and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the hint from.</param>
		/// <param name="hint">Hint to query.</param>
		/// <returns>True if hint is enabled, false otherwise.</returns>
		static bool GetWindowHint(const InternalWindow* window, Hint hint);
		/// <summary>
		/// This function sets the monitor that the window uses for full screen mode or,
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the monitor for.</param>
		/// <param name="monitor">Internal monitor to use or nullptr.</param>
		/// <param name="xPos">New x position for the window.</param>
		/// <param name="yPos">New y position for the window.</param>
		/// <param name="width">New width for the window.</param>
		/// <param name="height">New height for the window.</param>
		/// <param name="refreshRate">New refresh rate for the window.</param>
		static void SetWindowMonitor(InternalWindow* window,
		                             InternalMonitor* monitor,
		                             int32_t xPos,
		                             int32_t yPos,
		                             int32_t width,
		                             int32_t height,
		                             int32_t refreshRate);
		/// <summary>
		/// This function sets the monitor that the window uses for borderless full screen mode.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the monitor for.</param>
		/// <param name="monitor">Internal monitor to use.</param>
		static void SetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor);
		/// <summary>
		/// This function sets the user-define dpointer of the specified window.
		/// The current value is retained until the window is destroyed.
		/// The initial value is nullptr.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function may be called from any thread. Access is not synchronized.
		/// </summary>
		/// <param name="window">Internal window to set the user pointer for.</param>
		/// <param name="pointer">Data to be stored with the window.</param>
		static void SetWindowUserPointer(InternalWindow* window, void* pointer);
		/// <summary>
		/// This function returns the current value of the user-defined pointer
		/// of the specified window. The initial value is nullptr.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function may be called from any thread. Access is not synchronized.
		/// </summary>
		/// <param name="window">Internal window to get the user pointer from.</param>
		/// <returns>Data stored with the window.</returns>
		static void* GetWindowUserPointer(const InternalWindow* window);
		/// <summary>
		/// This function sets the monitor configuration callback, or removes
		/// the currently set callback. This is called when a monitor is
		/// connected to or disconnected from the system.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="callback">
		/// Function to call when a monitor event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetMonitorCallback(MonitorFunc callback);
		/// <summary>
		/// This function sets the position callback of the specified window, which is called
		/// when the window is moved. The callback is provided with the position, in screen
		/// coordinates, of the upper-left corner of the content area of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a position event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowPosCallback(InternalWindow* window, WindowPositionFunc callback);
		/// <summary>
		/// This function sets the size callback of the specified window, which is
		/// called when the window is resized. The callback is provided with the
		/// size, in screen coordinates, of the content area of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a size event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowSizeCallback(InternalWindow* window, WindowSizeFunc callback);
		/// <summary>
		/// This function sets the minimization/iconification callback of the specified window, which
		/// is called when the window is minimized/iconified or restored.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a minimize event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowMinimizeCallback(InternalWindow* window, WindowMinimizeFunc callback);
		/// <summary>
		/// This function sets the maximization callback of the specified window, which
		/// is called when the window is maximized or restored.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a maximize event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowMaximizeCallback(InternalWindow* window, WindowMaximizeFunc callback);
		/// <summary>
		/// This function sets the close callback of the specified window, which is called
		/// when the user attempts to close the window, for example by clicking the close widget
		/// in the title bar.
		///
		/// The close flags is set before this callback is called, but you can modify it at
		/// any time with WindowingAPI::SetWindowShouldClose.
		///
		/// The close callback is not triggered by WindowingAPI::DestroyWindow.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a close event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowCloseCallback(InternalWindow* window, WindowCloseFunc callback);
		/// <summary>
		/// This function sets the focus callback of the specified window, which is called
		/// when the window gains or loses input focus.
		///
		/// After the focus callback is called for a window that lost input focus, synthetic
		/// key and mouse button release events will be generated for all such that had been
		/// pressed. For more information, see WindowingAPI::SetKeyCallback and
		/// WindowingAPI::SetMouseButtonCallback.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a focus event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowFocusCallback(InternalWindow* window, WindowFocusFunc callback);
		/// <summary>
		/// This function sets the framebuffer resize callback of the specified window, which is
		/// called when the framebuffer of the specified window is resized.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a framebuffer size event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetFrameBufferSizeCallback(InternalWindow* window, FrameBufferSizeFunc callback);
		/// <summary>
		/// This function sets the window content scale callback of the specified window, which is
		/// called when the content scale of the specified window changes.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a content scale event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetContentScaleCallback(InternalWindow* window, WindowContentScaleFunc callback);
		/// <summary>
		/// This function sets the key callback of the specified window, which is called when
		/// a key is pressed, repeated or released.
		///
		/// The key functions deal with physical keys, with layout independent key tokens named
		/// after their values in the standard US keyboard layout. If oyu want to input text, use
		/// the character callback instead.
		///
		/// When a window loses input focus, it will generate synthetic key release events for
		/// all pressed keys. You can tell these events from user-generated events by the fact
		/// that the synthetic ones are generated after the focus loss event has been processed,
		/// i.e. after the window focus callback has been called.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a key event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetKeyCallback(InternalWindow* window, KeyFunc callback);
		/// <summary>
		/// This function sets the character callback of the specified window, which is called
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a char event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetCharCallback(InternalWindow* window, CharFunc callback);
		/// <summary>
		/// This function sets the mouse button callback of the specified window, which is
		/// called when amouse button is pressed or released.
		///
		/// When a window loses input focus, it will generate synthetic mouse button release
		/// events for all pressed mouse buttons. You cann tell these events from user-generated
		/// events by the fact that the synthetic ones are generated after the focus loss event
		/// has been processed, i.e. after the window focus callback has been called.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a mouse button event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetMouseButtonCallback(InternalWindow* window, MouseButtonFunc callback);
		/// <summary>
		/// This function sets the cursor position callback of the specified window, which is
		/// called when the cursor is moved. The callback is provided with the position,
		/// in screen coordinates, relative to the upper-left corner of the content area
		/// of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a cursor position event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetCursorPosCallback(InternalWindow* window, CursorPositionFunc callback);
		/// <summary>
		/// This function sets the cursor boundary crossing callback of the specified window, which
		/// is called when the cursor enters or leaves the content area of the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This fucntion must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a cursor enter event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetCursorEnterCallback(InternalWindow* window, CursorEnterFunc callback);
		/// <summary>
		/// This function sets the scroll callback of the specified window, which is called when a
		/// scrolling device is used, such as a mouse wheel or scrolling area of a touchpad.
		///
		/// The scroll callback receives all scrolling input, like that from a mouse wheel or a touchpad
		/// scrolling area.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a scroll event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetScrollCallback(InternalWindow* window, ScrollFunc callback);
		/// <summary>
		/// This function sets the path drop callback of the specified window, which is
		/// called when one or more dragged paths are dropped on the window.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a drop even occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetDropCallback(InternalWindow* window, DropFunc callback);
		/// <summary>
		/// Gets the monitor configuration callback.
		/// </summary>
		/// <returns>Function that gets called when a monitor event occurs.</returns>
		static MonitorFunc GetMonitorCallback();
		/// <summary>
		/// Gets the position callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a position event occurs.</returns>
		static WindowPositionFunc GetWindowPosCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the size callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a size event occurs.</returns>
		static WindowSizeFunc GetWindowSizeCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the close callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a close event occurs.</returns>
		static WindowCloseFunc GetWindowCloseCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the focus callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a focus event occurs.</returns>
		static WindowFocusFunc GetWindowFocusCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the framebuffer resize callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a framebuffer size event occurs.</returns>
		static FrameBufferSizeFunc GetFrameBufferSizeCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the window content scale callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a content scale event occurs.</returns>
		static WindowContentScaleFunc GetWindowContentScaleCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the key callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a key event occurs.</returns>
		static KeyFunc GetKeyCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the Unicode character callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a char event occurs.</returns>
		static CharFunc GetCharCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the mouse button callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a mouse button event occurs.</returns>
		static MouseButtonFunc GetMouseButtonCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the cursor position callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a cursor position event occurs.</returns>
		static CursorPositionFunc GetCursorPosCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the cursor enter callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a cursor enter event occurs.</returns>
		static CursorEnterFunc GetCursorEnterCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the scroll callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a scroll event occurs.</returns>
		static ScrollFunc GetScrollCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the path drop callback for the specified window.
		/// </summary>
		/// <param name="window">Internal window from which to query the callback from.</param>
		/// <returns>Function that gets called when a drop event occurs.</returns>
		static DropFunc GetDropCallback(const InternalWindow* window);
		/// <summary>
		/// This function processes only those events that are already in the event queue and then
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
		/// Reentrancy: This function must not be called from a callback.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		static void PollEvents();
		/// <summary>
		/// This function sets a cursor mode for the specified window.
		///
		/// The mode must be one of the following CursorModes:
		/// - Normal makes the cursor visible and behaving normally.
		/// - Hidden makes the cursor invisible when it is over the content area of the
		///   window but does not restrict the cursor from leaving.
		/// - Disabled hides and grabs the cursor, providing virtual and unlimited cursor movement.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the cursor mode for.</param>
		/// <param name="mode">Cursor mode to be set.</param>
		static void SetCursorMode(InternalWindow* window, CursorMode mode);
		/// <summary>
		/// Retrieves the cursor mode from the specified window.
		/// </summary>
		/// <param name="window">Internal window to query the cursor mode from.</param>
		/// <returns>Cursor mode used by the internal window.</returns>
		static CursorMode GetCursorMode(const InternalWindow* window);
		/// <summary>
		/// This function returns whether raw mouse motion is supported on the current system.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>True if raw mouse input is supported, false otherwise.</returns>
		static bool RawMouseMotionSupported();
		/// <summary>
		/// Sets the raw mouse motion mode for the specified window.
		///
		/// If raw (unscaled and unaccelerated) mouse motion is not supported, attempting to set this
		/// will emit Error::Feature_Unavailable.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum,
		///         Error::Platform_Error and Error::Feature_Unavailable.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set raw mouse input for.</param>
		/// <param name="enabled">Whether to enable or disable raw mouse input.</param>
		static void SetRawMouseMotionMode(InternalWindow* window, bool enabled);
		/// <summary>
		/// Retrieves the raw mouse motion mode for the specified window.
		/// </summary>
		/// <param name="window">Internal window to query.</param>
		/// <returns>True if raw mouse motion mode is enabled, false otherwise.</returns>
		static bool GetRawMouseMotionMode(const InternalWindow* window);
		/// <summary>
		/// Sets the progress value and state on the taskbar for the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error and Error::Feature_Unavailable.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set progress for.</param>
		/// <param name="state">State of progress.</param>
		/// <param name="progress">How much has been completed. Valid values: 0 - 100.</param>
		static void SetProgress(const InternalWindow* window, ProgressState state, uint32_t progress);
		/// <summary>
		/// This function returns the name of the specified printable key, encoded as UTF-8.
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
		/// function return nullptr but does not emit an error.
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
		/// Remarks: The contents of the returned string may change when a keyboard layout change event is received.
		/// Pointer lifetime: The returned string is allocated and freed by the WindowingAPI.
		///                   You should not free it yourself.
		///                   It is valid until the library is terminated.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="key">Key to get layout-specific name from.</param>
		/// <param name="scanCode">Optional scan code to get layout-specific name from.</param>
		/// <returns>UTF-8 encoded, layout-specific name of the given key or nullptr.</returns>
		static const char* GetKeyName(Input::Key key, int32_t scanCode);
		/// <summary>
		/// This function returns the last state reported for the specified key to the
		/// specified window. The returned state is KeyState::Pressed or KeyState::Released.
		///
		/// The key functions deal with physical keys, with key token named after their use
		/// on the standard US keyboard layout. If you want to input text, use the Unicode
		/// character callback instead.
		///
		/// DO NOT USE THIS FUNCTION to implement text input.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to query.</param>
		/// <param name="key">Key to get last reported state from.</param>
		/// <returns>Last reported state of the specified key.</returns>
		static TRAP::Input::KeyState GetKey(const InternalWindow* window, Input::Key key);
		/// <summary>
		/// This function returns the last state reported for the specified mouse button
		/// to the specified window. The returned state is pressed (true) or released (false).
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to query.</param>
		/// <param name="button">Mouse button to get last reported state from.</param>
		/// <returns>Last reported state of the specified mouse button.</returns>
		static TRAP::Input::KeyState GetMouseButton(const InternalWindow* window, Input::MouseButton button);
		/// <summary>
		/// This function sets the position, in screen coordinates, of the cursor relative to the
		/// upper-left corner of the content area of the specified window. The window must have
		/// input focus. If the window does not have input focus when this function is called,
		/// it fails silently.
		///
		/// DO NOT USE THIS FUNCTION to implement things like camera controls. The WindowingAPI already
		/// provides the CursorMode::Disabled that hides the cursor, transparently re-centers it and
		/// provides unconstrained cursor motion. See SetCursorMode for more information.
		///
		/// If the cursor mode is CursorMode::Disabled then the cursor position is unconstrained and
		/// limited only by the minimum and maximum values of a double.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the cursor position for.</param>
		/// <param name="xPos">New x position, relative to the left edge of the content area, for the cursor.</param>
		/// <param name="yPos">New y position, relative to the top edge of the content area, for the cursor.</param>
		static void SetCursorPos(InternalWindow* window, double xPos, double yPos);
		/// <summary>
		/// This function returns the position of the cursor, in screen coordinates, relative
		/// to the upper-left corner of the content area of the specified window.
		///
		/// If the cursor is disabled (with CursorMode::Disabled) then the cursor position is unbounded and limited
		/// only by the minimum and maximum values of a double.
		///
		/// The coordinate can be converted to their integer equivalents with the TRAP::Math::Floor function.
		/// Casting directly to an integer type works for positive coordinates, but fails for negative ones.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window from which to get the current cursor position.</param>
		/// <param name="xPos">
		/// Output variable for the current x position, relative to the left edge of the content area, of the cursor.
		/// </param>
		/// <param name="yPos">
		/// Output variable for the current y position, relative to the top edge of the content area, of the cursor.
		/// </param>
		static void GetCursorPos(const InternalWindow* window, double& xPos, double& yPos);
		/// <summary>
		/// Returns the position of the monitor's viewport on the virtual screen.
		/// This function returns the position, in screen coordinates, of the upper-left corner
		/// of the specified monitor.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor to query.</param>
		/// <param name="xPos">Output variable for the x position of the monitor.</param>
		/// <param name="yPos">Output variable for the y position of the monitor.</param>
		static void GetMonitorPos(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos);
		/// <summary>
		/// This function returns the position, in screen coordinates, of the upper-left corner
		/// of the work area of the specified monitor along with the work area size in screen
		/// coordinates. The work area is defined as the area of the monitor not occluded by
		/// the operating system task bar where present. If no task bar exists then the work
		/// area is the monitor resolution in screen coordinates.
		///
		/// If an error occurs, xPos, yPos, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor to query.</param>
		/// <param name="xPos">Output variable for the x position of the monitor.</param>
		/// <param name="yPos">Output variable for the y position of the monitor.</param>
		/// <param name="width">Output variable for the width of the monitor.</param>
		/// <param name="height">Output variable for the height of the monitor.</param>
		static void GetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos,
		                               int32_t& width, int32_t& height);
		/// <summary>
		/// This function makes the specified window visible if it was previously
		/// hidden. If the window is already visible or is in full screen mode,
		/// this function does nothing.
		///
		/// By default, windowed mode window are focused when shown. Set the Hint::FocusOnShow
		/// window hint to change this behaviour for all newly created windows, or change
		/// the behaviour for and existing window with WindowingAPI::SetWindowHint.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to display.</param>
		static void ShowWindow(InternalWindow* window);
		/// <summary>
		/// This function brings the specified window to front and sets input focus.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to focus.</param>
		static void FocusWindow(const InternalWindow* window);
		/// <summary>
		/// This function maximizes the specified window if it was previously not
		/// maximized. If the window is already maximized, this function does nothing.
		///
		/// If the specified window is a full screen window, this function does nothing.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function may only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to maximize.</param>
		static void MaximizeWindow(const InternalWindow* window);
		/// <summary>
		/// This function minimizes (iconifies) the specified window if it was previously
		/// restored. If the window is already minimized (iconified), this function does
		/// nothing.
		///
		/// If the specified window is a full screen window, the original monitor resolution
		/// is restored util the window is restored.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to minimize/iconify.</param>
		static void MinimizeWindow(const InternalWindow* window);
		/// <summary>
		/// This function requests user attention to the specified window.
		/// On platforms where this is not supported, attention is requested
		/// to the application as a whole.
		///
		/// Once the user has given attention, usually by focusing the window
		/// or application, the system will end the request automatically.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to request user attention for.</param>
		static void RequestWindowAttention(const InternalWindow* window);
		/// <summary>
		/// This function hides the specified window if it was previously visible. If
		/// the window is already hidden or is in full screen mode, this function
		/// does nothing.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to hide.</param>
		static void HideWindow(const InternalWindow* window);
		/// <summary>
		/// Restores the specified window.
		/// This function restores the specified window if it was previously
		/// minimized (iconified) or maximized. If the window is already restored,
		/// this function does nothing.
		///
		/// If the specified window is a full screen window, the resolution chosen
		/// for the window is restored on the selected monitor.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to be restored.</param>
		static void RestoreWindow(InternalWindow* window);
		/// <summary>
		/// Sets the size limits of the specified window.
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
		/// Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set size limits for.</param>
		/// <param name="minWidth">New minimum window width.</param>
		/// <param name="minHeight">New minimum window height.</param>
		/// <param name="maxWidth">New maximum window width.</param>
		/// <param name="maxHeight">New maximum window height.</param>
		static void SetWindowSizeLimits(InternalWindow* window, int32_t minWidth, int32_t minHeight,
		                                int32_t maxWidth, int32_t maxHeight);
		/// <summary>
		/// This function sets the system clipboard to the specified, UTF-8 encoded string.
		///
		/// Errors: Possible errors include Error::Not_Initialized, Error::Format_Unavailable
		/// and Error::Platform_Error.
		/// Pointer lifetime: The specified string is copied before this function returns.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="string">UTF-8 encoded string to be set for the clipboard.</param>
		static void SetClipboardString(const std::string& string);
		/// <summary>
		/// This function returns the contents of the system clipboard, if it contains or
		/// is convertible to a UTF-8 encoded string. If the clipboard is empty or if its
		/// content cannot be converted, an empty string is returned and a Error::Format_Unavailable
		/// error is generated.
		///
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>
		/// UTF-8 encoded string containing clipboard contents, or empty string if an error occurred.
		/// </returns>
		static std::string GetClipboardString();
		/// <summary>
		/// This function returns whether the Vulkan loader and any minimally function ICD
		/// have been found.
		///
		/// The availability of a Vulkan loader and even an ICD does not by itself guarantee
		/// that surface creation or even instance creation is possible. Call GetRequiredInstanceExtensions
		/// to check whether the extensions necessary for Vulkan surface creation are available.
		/// You still have to check whether a queue family of a physical device supports image presentation.
		///
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function may be called from any thread.
		/// </summary>
		/// <returns>True if Vulkan API is minimally supported, false otherwise.</returns>
		static bool VulkanSupported();
		/// <summary>
		/// This function returns an array of names of Vulkan instance extensions required
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
		/// Remarks: Additional extensions may be required by future versions of the WindowingAPI.
		///          You should check if any extensions you wish to enable are already in the
		///          returned array, as it is an error to specify an extension more than once
		///          in the VkInstanceCreateInfo struct.
		/// Thread safety: This function may be called from any thread.
		/// </summary>
		/// <returns>
		/// Array containing the required instance extensions, or an array with empty string
		/// if an error occurred.
		/// </returns>
		static std::array<std::string, 2> GetRequiredInstanceExtensions();
		/// <summary>
		/// Creates a Vulkan surface for the specified window.
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
		/// Remarks: If an error occurs before the creation call is made, the WindowingAPI returns
		///          the Vulkan error code most appropriate for the error. Appropriate use of
		///          WindowingAPI::VulkanSupported and WindowingAPI::GetRequiredInstanceExtensions
		///          should eliminate almost all occurrences of these errors.
		/// Thread safety: This function may be called from any thread. For synchronization details
		///                of Vulkan objects, see the Vulkan specification.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <param name="window">Internal window for which to create the surface for.</param>
		/// <param name="allocator">Optional allocator.</param>
		/// <param name="surface">Output variable for the new Vulkan surface.</param>
		/// <returns>VK_SUCCESS if successful, or a Vulkan error code if an error occurred.</returns>
		static VkResult CreateWindowSurface(VkInstance instance, const InternalWindow* window,
		                                    const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		/// <summary>
		/// Hides the specified window from the taskbar.
		/// </summary>
		/// <param name="window">Internal window to hide from the taskbar.</param>
		static void HideWindowFromTaskbar(InternalWindow* window);
		/// <summary>
		/// Enable/Disable drag and drop feature for the specified window.
		/// </summary>
		/// <param name="window">Internal window for which to set drag and drop.</param>
		/// <param name="value">Whether to enable or disable drag and drop.</param>
		static void SetDragAndDrop(InternalWindow* window, bool value);
#ifdef TRAP_PLATFORM_WINDOWS
		/// <summary>
		/// Returns the HWND handle of the internal window.
		/// </summary>
		/// <param name="window">Internal window from which to get the HWND from.</param>
		/// <returns>HWND of the specified window.</returns>
		static HWND GetWin32Window(const InternalWindow* window);
#endif
		//-------//
		//Private//
		//-------//
	private:
		/// <summary>
		/// Center the cursor in the content area of the specified windows.
		/// </summary>
		/// <param name="window">Internal window which to center the cursor for.</param>
		static void CenterCursorInContentArea(InternalWindow* window);
		/// <summary>
		/// Notifies shared code of an error.
		/// </summary>
		/// <param name="code">Error code.</param>
		/// <param name="str">Description of the occurred error.</param>
		static void InputError(Error code, const std::string& str);
		//-------------------------------------------------------------------------------------------------------------------//
		//Platform Specific Functions----------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		/// <summary>
		/// Create key code translation tables.
		/// </summary>
		static void CreateKeyTablesWin32();
		static void CreateKeyTablesX11();
		static void CreateKeyTablesWayland();
		/// <summary>
		/// This function returns the current video mode of the specified monitor.
		/// If you have create a full screen window for that monitor, the return
		/// value will depend on whether that window is minimized/iconified.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor to get internal video mode from.</param>
		/// <returns>
		/// Current video mode of the specified monitor, or an empty one if an error occurred.
		/// </returns>
		static InternalVideoMode PlatformGetVideoMode(const InternalMonitor* monitor);
		static InternalVideoMode PlatformGetVideoModeX11(const InternalMonitor* monitor);
		static InternalVideoMode PlatformGetVideoModeWayland(const InternalMonitor* monitor);
		/// <summary>
		/// This function retrieves the size, in screen coordinates, of the content area of
		/// the specified window. If you wish to retrieve the size of the framebuffer of the window
		/// in pixels, see WindowingAPI::GetFrameBufferSize.
		///
		/// If an error occurs, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the size from.</param>
		/// <param name="width">Output variable for the internal windows current width.</param>
		/// <param name="height">Output variable for the internal windows current height.</param>
		static void PlatformGetWindowSize(const InternalWindow* window, int32_t& width, int32_t& height);
		static void PlatformGetWindowSizeX11(const InternalWindow* window, int32_t& width, int32_t& height);
		static void PlatformGetWindowSizeWayland(const InternalWindow* window, int32_t& width, int32_t& height);
		/// <summary>
		/// This function sets the position, in screen coordinates, of the upper-left corner of the
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the position for.</param>
		/// <param name="xPos">X position to be set.</param>
		/// <param name="yPos">Y position to be set.</param>
		static void PlatformSetWindowPos(const InternalWindow* window, int32_t xPos, int32_t yPos);
		static void PlatformSetWindowPosX11(const InternalWindow* window, int32_t xPos, int32_t yPos);
		static void PlatformSetWindowPosWayland(const InternalWindow* window, int32_t xPos, int32_t yPos);
		/// <summary>
		/// This function sets the monitor that the window uses for full screen mode or,
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the monitor for.</param>
		/// <param name="monitor">Internal monitor to use or nullptr.</param>
		/// <param name="xPos">New x position for the window.</param>
		/// <param name="yPos">New y position for the window.</param>
		/// <param name="width">New width for the window.</param>
		/// <param name="height">New height for the window.</param>
		/// <param name="refreshRate">New refresh rate for the window.</param>
		static void PlatformSetWindowMonitor(InternalWindow* window, InternalMonitor* monitor,
			                                 int32_t xPos, int32_t yPos, int32_t width, int32_t height,
											 int32_t refreshRate);
		static void PlatformSetWindowMonitorX11(InternalWindow* window, InternalMonitor* monitor,
			                                 int32_t xPos, int32_t yPos, int32_t width, int32_t height,
											 int32_t refreshRate);
		static void PlatformSetWindowMonitorWayland(InternalWindow* window, InternalMonitor* monitor,
			                                 int32_t xPos, int32_t yPos, int32_t width, int32_t height,
											 int32_t refreshRate);
		/// <summary>
		/// This function sets the monitor that the window uses for borderless full screen mode.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the monitor for.</param>
		/// <param name="monitor">Internal monitor to use.</param>
		static void PlatformSetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor);
		static void PlatformSetWindowMonitorBorderlessX11(InternalWindow* window, InternalMonitor* monitor);
		static void PlatformSetWindowMonitorBorderlessWayland(InternalWindow* window, InternalMonitor* monitor);
		/// <summary>
		/// This function returns a vector of all video modes supported by the specified monitor.
		/// The returned vector is sorted in ascending order, first by color bit depth
		/// (the sum of all channel depths) and then by resolution area (the product of
		/// width and height).
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor to get all internal video modes from.</param>
		/// <returns>
		/// Vector with all available video modes of the specified monitor, or an empty vector
		/// if an error occurred.
		/// </returns>
		static std::vector<InternalVideoMode> PlatformGetVideoModes(const InternalMonitor* monitor);
		static std::vector<InternalVideoMode> PlatformGetVideoModesX11(const InternalMonitor* monitor);
		static std::vector<InternalVideoMode> PlatformGetVideoModesWayland(const InternalMonitor* monitor);
		/// <summary>
		/// Initializes the windowing API.
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
		/// Remarks:
		/// X11: This function will set the LC_CTYPE category of the engine locale according to the current environment
		///      if that category is still "C". This is because the "C" locale breaks Unicode text input.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>True if windowing API was successfully initialized, false otherwise.</returns>
		static bool PlatformInit();
		static bool PlatformInitX11();
		static bool PlatformInitWayland();
		/// <summary>
		/// This function destroys the specified window. On calling this function, no
		/// further callbacks will be called for that window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Reentrancy: This function must not be called from a callback.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to destroy.</param>
		static void PlatformDestroyWindow(InternalWindow* window);
		static void PlatformDestroyWindowX11(InternalWindow* window);
		static void PlatformDestroyWindowWayland(InternalWindow* window);
		/// <summary>
		/// This function destroys all remaining windows and cursor, and frees any other allocated resources.
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
		/// Remarks: This function may be called before WindowingAPI::Init.
		/// Reentrancy: This function must not be called from a callback.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		static void PlatformShutdown();
		static void PlatformShutdownX11();
		static void PlatformShutdownWayland();
		/// <summary>
		/// This function retrieves the content scale for the specified monitor.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor.</param>
		/// <param name="xScale">Output variable for the X scale of the provided monitor.</param>
		/// <param name="yScale">Output variable for the Y scale of the provided monitor.</param>
		static void PlatformGetMonitorContentScale(const InternalMonitor* monitor, float& xScale, float& yScale);
		static void PlatformGetMonitorContentScaleX11(const InternalMonitor* monitor, float& xScale, float& yScale);
		static void PlatformGetMonitorContentScaleWayland(const InternalMonitor* monitor, float& xScale, float& yScale);
		/// <summary>
		/// Returns the position of the monitor's viewport on the virtual screen.
		/// This function returns the position, in screen coordinates, of the upper-left corner
		/// of the specified monitor.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor to query.</param>
		/// <param name="xPos">Output variable for the x position of the monitor.</param>
		/// <param name="yPos">Output variable for the y position of the monitor.</param>
		static void PlatformGetMonitorPos(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos);
		static void PlatformGetMonitorPosX11(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos);
		static void PlatformGetMonitorPosWayland(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos);
		/// <summary>
		/// This function makes the specified window visible if it was previously
		/// hidden. If the window is already visible or is in full screen mode,
		/// this function does nothing.
		///
		/// By default, windowed mode window are focused when shown. Set the Hint::FocusOnShow
		/// window hint to change this behaviour for all newly created windows, or change
		/// the behaviour for and existing window with WindowingAPI::SetWindowHint.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to display.</param>
		static void PlatformShowWindow(InternalWindow* window);
		static void PlatformShowWindowX11(InternalWindow* window);
		static void PlatformShowWindowWayland(InternalWindow* window);
		/// <summary>
		/// This function brings the specified window to front and sets input focus.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to focus.</param>
		static void PlatformFocusWindow(const InternalWindow* window);
		static void PlatformFocusWindowX11(const InternalWindow* window);
		static void PlatformFocusWindowWayland(const InternalWindow* window);
		/// <summary>
		/// This function creates a window.
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
		/// Remarks:
		///     Windows: If the executable has an icon resource named 'TRAP_ICON', it will be set as the initial
		///              icon for the window. If no such icon is present, the IDI_APPLICATION icon will be used
		///              instead. To set a different icon, see WindowingAPI::SetWindowIcon.
		///     X11: Some window managers will not respect the placement of initially hidden windows.
		///          Due to the asynchronous nature of X11, it may take a moment for a window to reach its
		///          requested state. This means you may not be able to query the final size, position or
		///          other attributes directly after window creation.
		///          The class part of the WM_CLASS window property will by default be set to the window
		///          title passed to this function. The instance part will use the contents of the
		///          RESOURCE_NAME environment variable, if present and not empty, or fall back to the
		///          window title.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to create.</param>
		/// <param name="WNDConfig">Configuration of the window to create.</param>
		/// <returns>True if the window was created successfully.</returns>
		static bool PlatformCreateWindow(InternalWindow* window,
			                             WindowConfig& WNDConfig);
		static bool PlatformCreateWindowX11(InternalWindow* window,
			                             WindowConfig& WNDConfig);
		static bool PlatformCreateWindowWayland(InternalWindow* window,
			                             WindowConfig& WNDConfig);
		/// <summary>
		/// This function sets the window title, encoded as UTF-8, of the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window whose title to change.</param>
		/// <param name="title">New UTF-8 encoded title for the window.</param>
		static void PlatformSetWindowTitle(const InternalWindow* window, const std::string& title);
		static void PlatformSetWindowTitleX11(const InternalWindow* window, const std::string& title);
		static void PlatformSetWindowTitleWayland(const InternalWindow* window, const std::string& title);
		/// <summary>
		/// Creates a new custom cursor image that can be set for a window with SetCursor. The cursor can
		/// be destroyed with DestroyCursor. Any remaining cursors are destroyed by WindowingAPI::Shutdown.
		///
		/// The cursor hotspot is specified in pixels, relative to the upper-left corner of the cursor image.
		/// Like all other coordinate system in the WindowingAPI, the X-axis points to the right and the
		/// Y-axis points down.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be caled from the main thread.
		/// </summary>
		/// <param name="cursor">Internal cursor to create.</param>
		/// <param name="image">Non HDR RGB 24BPP or RGBA 32 BPP image.</param>
		/// <param name="xHotspot">Center x coordinate of the image.</param>
		/// <param name="yHotspot">Center y coordinate of the image.</param>
		/// <returns>True if the cursor was created successfully.</returns>
		static bool PlatformCreateCursor(InternalCursor* cursor, const Image* const image, int32_t xHotspot,
		                                 int32_t yHotspot);
		static bool PlatformCreateCursorX11(InternalCursor* cursor, const Image* const image, int32_t xHotspot,
		                                 int32_t yHotspot);
		static bool PlatformCreateCursorWayland(InternalCursor* cursor, const Image* const image, int32_t xHotspot,
		                                 int32_t yHotspot);
		/// <summary>
		/// Creates a cursor with a standard shape.
		///
		/// Errors: Possible errors include Error::Invalid_Enum and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="cursor">Internal cursor to create.</param>
		/// <param name="type">Cursor type to get.</param>
		/// <returns>True if the cursor was created successfully.</returns>
		static bool PlatformCreateStandardCursor(InternalCursor* cursor, const CursorType& type);
		static bool PlatformCreateStandardCursorX11(InternalCursor* cursor, const CursorType& type);
		static bool PlatformCreateStandardCursorWayland(InternalCursor* cursor, const CursorType& type);
		/// <summary>
		/// This function destroys a cursor previously created with CreateCursor. Any remaining cursors
		/// will be destroyed by WindowingAPI::Shutdown.
		///
		/// If the specified cursor is current for any window, that window will be reverted to the default
		/// cursor. This does not affect the cursor mode.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Reentrancy: This function must not be called from a callback.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="cursor">Internal cursor to be destroyed.</param>
		static void PlatformDestroyCursor(InternalCursor* cursor);
		static void PlatformDestroyCursorX11(InternalCursor* cursor);
		static void PlatformDestroyCursorWayland(InternalCursor* cursor);
		/// <summary>
		/// This function sets the cursor image to be used when the cursor is over the content are of the
		/// specified window. The set cursor will only be visible when the CursorMode of the window is
		/// CursorMode::Normal.
		///
		/// On some platforms, the set cursor may not be visible unless the window also has input focus.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the cursor for.</param>
		/// <param name="cursor">
		/// Internal cursor to set, or nullptr to switch back to the default arrow cursor.
		/// </param>
		static void PlatformSetCursor(const InternalWindow* window, const InternalCursor* cursor);
		static void PlatformSetCursorX11(const InternalWindow* window, const InternalCursor* cursor);
		static void PlatformSetCursorWayland(const InternalWindow* window, const InternalCursor* cursor);
		/// <summary>
		/// This function sets a cursor mode for the specified window.
		///
		/// The mode must be one of the following CursorModes:
		/// - Normal makes the cursor visible and behaving normally.
		/// - Hidden makes the cursor invisible when it is over the content area of the
		///   window but does not restrict the cursor from leaving.
		/// - Disabled hides and grabs the cursor, providing virtual and unlimited cursor movement.
		///
		/// Errors: Possible errors include Error::Invalid_Enum and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the cursor mode for.</param>
		/// <param name="mode">Cursor mode to be set.</param>
		static void PlatformSetCursorMode(InternalWindow* window, CursorMode mode);
		static void PlatformSetCursorModeX11(InternalWindow* window, CursorMode mode);
		static void PlatformSetCursorModeWayland(InternalWindow* window, CursorMode mode);
		/// <summary>
		/// This function sets the position, in screen coordinates, of the cursor relative to the
		/// upper-left corner of the content area of the specified window. The window must have
		/// input focus. If the window does not have input focus when this function is called,
		/// it fails silently.
		///
		/// DO NOT USE THIS FUNCTION to implement things like camera controls. The WindowingAPI already
		/// provides the CursorMode::Disabled that hides the cursor, transparently re-centers it and
		/// provides unconstrained cursor motion. See SetCursorMode for more information.
		///
		/// If the cursor mode is CursorMode::Disabled then the cursor position is unconstrained and
		/// limited only by the minimum and maximum values of a double.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the cursor position for.</param>
		/// <param name="xPos">New x position, relative to the left edge of the content area, for the cursor.</param>
		/// <param name="yPos">New y position, relative to the top edge of the content area, for the cursor.</param>
		static void PlatformSetCursorPos(InternalWindow* window, double xPos, double yPos);
		static void PlatformSetCursorPosX11(InternalWindow* window, double xPos, double yPos);
		static void PlatformSetCursorPosWayland(InternalWindow* window, double xPos, double yPos);
		/// <summary>
		/// This function returns the position of the cursor, in screen coordinates, relative
		/// to the upper-left corner of the content area of the specified window.
		///
		/// If the cursor is disabled (with CursorMode::Disabled) then the cursor position is unbounded and limited
		/// only by the minimum and maximum values of a double.
		///
		/// The coordinate can be converted to their integer equivalents with the TRAP::Math::Floor function.
		/// Casting directly to an integer type works for positive coordinates, but fails for negative ones.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window from which to get the current cursor position.</param>
		/// <param name="xPos">
		/// Output variable for the current x position, relative to the left edge of the content area, of the cursor.
		/// </param>
		/// <param name="yPos">
		/// Output variable for the current y position, relative to the top edge of the content area, of the cursor.
		/// </param>
		static void PlatformGetCursorPos(const InternalWindow* window, double& xPos, double& yPos);
		static void PlatformGetCursorPosX11(const InternalWindow* window, double& xPos, double& yPos);
		static void PlatformGetCursorPosWayland(const InternalWindow* window, double& xPos, double& yPos);
		/// <summary>
		/// This function sets the icon of the specified window. If no image is specified, the window
		/// reverts to its default icon.
		///
		/// The image mustbe RGB 24BPP or RGBA 32BPP.
		///
		/// The desired image size varies depending on platform and system settings.
		/// Good sizes include 16x16, 32x32 and 48x48.
		///
		/// Errors: Possible errors Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window whose icon to set.</param>
		/// <param name="image">
		/// Image to be set as window icon or nullptr to revert back to the default icon.
		/// </param>
		static void PlatformSetWindowIcon(InternalWindow* window, const Image* const image);
		static void PlatformSetWindowIconX11(InternalWindow* window, const Image* const image);
		static void PlatformSetWindowIconWayland(InternalWindow* window, const Image* const image);
		/// <summary>
		/// This function retrieves the position, in screen coordinates, of the upper-left corner of the
		/// content area of the specified window.
		///
		/// If an error occurs, xPos and yPos will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the current position from.</param>
		/// <param name="xPos">Output variable for the current x position of the internal window.</param>
		/// <param name="yPos">Output variable for the current y position of the internal window.</param>
		static void PlatformGetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos);
		static void PlatformGetWindowPosX11(const InternalWindow* window, int32_t& xPos, int32_t& yPos);
		static void PlatformGetWindowPosWayland(const InternalWindow* window, int32_t& xPos, int32_t& yPos);
		/// <summary>
		/// This function sets the size, in screen coordinates, of the content area of the specified window.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the size for.</param>
		/// <param name="width">New width for the internal window.</param>
		/// <param name="height">New height for the internal window.</param>
		static void PlatformSetWindowSize(InternalWindow* window, int32_t width, int32_t height);
		static void PlatformSetWindowSizeX11(InternalWindow* window, int32_t width, int32_t height);
		static void PlatformSetWindowSizeWayland(InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// This function toggles the resizeability of the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the size for.</param>
		/// <param name="enabled">Enable or disable resizing for the internal window.</param>
		static void PlatformSetWindowResizable(InternalWindow* window, bool enabled);
		static void PlatformSetWindowResizableX11(InternalWindow* window, bool enabled);
		static void PlatformSetWindowResizableWayland(InternalWindow* window, bool enabled);
		/// <summary>
		/// This function toggles the decorations of the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the size for.</param>
		/// <param name="enabled">Enable or disable decorations for the internal window.</param>
		static void PlatformSetWindowDecorated(const InternalWindow* window, bool enabled);
		static void PlatformSetWindowDecoratedX11(const InternalWindow* window, bool enabled);
		static void PlatformSetWindowDecoratedWayland(const InternalWindow* window, bool enabled);
		/// <summary>
		/// This function toggles whether the specified window is floating.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the size for.</param>
		/// <param name="enabled">Enable or disable floating mode for the internal window.</param>
		static void PlatformSetWindowFloating(const InternalWindow* window, bool enabled);
		static void PlatformSetWindowFloatingX11(const InternalWindow* window, bool enabled);
		static void PlatformSetWindowFloatingWayland(const InternalWindow* window, bool enabled);
		/// <summary>
		/// This function sets the opacity of the window, including any decorations.
		///
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque.
		///
		/// The initial opacity value for newly created windows is 1.0f.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set opacity for.</param>
		/// <param name="opacity">Opacity ranging from 0.0f-1.0f.</param>
		static void PlatformSetWindowOpacity(const InternalWindow* window, float opacity);
		static void PlatformSetWindowOpacityX11(const InternalWindow* window, float opacity);
		static void PlatformSetWindowOpacityWayland(const InternalWindow* window, float opacity);
		/// <summary>
		/// This function toggles mouse passthrough for the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set the size for.</param>
		/// <param name="enabled">Enable or disable mouse passthrough for the internal window.</param>
		static void PlatformSetWindowMousePassthrough(InternalWindow* window, bool enabled);
		static void PlatformSetWindowMousePassthroughX11(InternalWindow* window, bool enabled);
		static void PlatformSetWindowMousePassthroughWayland(InternalWindow* window, bool enabled);
		/// <summary>
		/// Hides the specified window from the taskbar.
		/// </summary>
		/// <param name="window">Internal window to hide from the taskbar.</param>
		static void PlatformHideWindowFromTaskbar(InternalWindow* window);
		static void PlatformHideWindowFromTaskbarX11(InternalWindow* window);
		static void PlatformHideWindowFromTaskbarWayland(InternalWindow* window);
		/// <summary>
		/// This function returns the opacity of the window, including any decorations.
		///
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque. If the system does not
		/// support whole window transparency, this function always returns one.
		///
		/// The initial opacity value for newly created windows is 1.0f.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the opacity from.</param>
		/// <returns>Opacity of the given internal window.</returns>
		static float PlatformGetWindowOpacity(const InternalWindow* window);
		static float PlatformGetWindowOpacityX11(const InternalWindow* window);
		static float PlatformGetWindowOpacityWayland(const InternalWindow* window);
		/// <summary>
		/// This function retrieves the size, in pixels, of the framebuffer of the specified window.
		/// If you wish to retrieve the size of the window in screen coordinates, see
		/// WindowingAPI::GetWindowSize.
		///
		/// If an error occcurs, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the framebuffer size from.</param>
		/// <param name="width">Output variable for the internal windows current framebuffer width.</param>
		/// <param name="height">Output variable for the internal windows current framebuffer height.</param>
		static void PlatformGetFrameBufferSize(const InternalWindow* window, int32_t& width, int32_t& height);
		static void PlatformGetFrameBufferSizeX11(const InternalWindow* window, int32_t& width, int32_t& height);
		static void PlatformGetFrameBufferSizeWayland(const InternalWindow* window, int32_t& width, int32_t& height);
		/// <summary>
		/// This function retrieves the content scale for the specified window.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get the content scale from.</param>
		/// <param name="xScale">Output variable for the internal windows content scale x.</param>
		/// <param name="yScale">Output variable for the internal windows content scale y.</param>
		static void PlatformGetWindowContentScale(const InternalWindow* window, float& xScale, float& yScale);
		static void PlatformGetWindowContentScaleX11(const InternalWindow* window, float& xScale, float& yScale);
		static void PlatformGetWindowContentScaleWayland(const InternalWindow* window, float& xScale, float& yScale);
		/// <summary>
		/// This function returns the position, in screen coordinates, of the upper-left corner
		/// of the work area of the specified monitor along with the work area size in screen
		/// coordinates. The work area is defined as the area of the monitor not occluded by
		/// the operating system task bar where present. If no task bar exists then the work
		/// area is the monitor resolution in screen coordinates.
		///
		/// If an error occurs, xPos, yPos, width and height will be set to zero.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">Internal monitor to query.</param>
		/// <param name="xPos">Output variable for the x position of the monitor.</param>
		/// <param name="yPos">Output variable for the y position of the monitor.</param>
		/// <param name="width">Output variable for the width of the monitor.</param>
		/// <param name="height">Output variable for the height of the monitor.</param>
		static void PlatformGetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos,
		                                       int32_t& width, int32_t& height);
		static void PlatformGetMonitorWorkAreaX11(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos,
		                                       int32_t& width, int32_t& height);
		static void PlatformGetMonitorWorkAreaWayland(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos,
		                                       int32_t& width, int32_t& height);
		/// <summary>
		/// This function returns whether the window is visible or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get visibility state from.</param>
		/// <returns>True if the window is visible, false otherwise.</returns>
		static bool PlatformWindowVisible(const InternalWindow* window);
		static bool PlatformWindowVisibleX11(const InternalWindow* window);
		static bool PlatformWindowVisibleWayland(const InternalWindow* window);
		/// <summary>
		/// This function returns whether the window is maximized or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get maximization state from.</param>
		/// <returns>True if the window is maximized, false otherwise.</returns>
		static bool PlatformWindowMaximized(const InternalWindow* window);
		static bool PlatformWindowMaximizedX11(const InternalWindow* window);
		static bool PlatformWindowMaximizedWayland(const InternalWindow* window);
		/// <summary>
		/// This function returns whether the window is minimized or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get minimization state from.</param>
		/// <returns>True if the window is minimized, false otherwise.</returns>
		static bool PlatformWindowMinimized(const InternalWindow* window);
		static bool PlatformWindowMinimizedX11(const InternalWindow* window);
		static bool PlatformWindowMinimizedWayland(const InternalWindow* window);
		/// <summary>
		/// This function processes only those events that are already in the event queue and then
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
		/// Reentrancy: This function must not be called from a callback.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		static void PlatformPollEvents();
		static void PlatformPollEventsX11();
		static void PlatformPollEventsWayland();
		/// <summary>
		/// This function returns whether the window is focused or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get focused state from.</param>
		/// <returns>True if the window is focused, false otherwise.</returns>
		static bool PlatformWindowFocused(const InternalWindow* window);
		static bool PlatformWindowFocusedX11(const InternalWindow* window);
		static bool PlatformWindowFocusedWayland(const InternalWindow* window);
		/// <summary>
		/// This function returns whether the window is hovered or not.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to get hovered state from.</param>
		/// <returns>True if the window is hovered, false otherwise.</returns>
		static bool PlatformWindowHovered(const InternalWindow* window);
		static bool PlatformWindowHoveredX11(const InternalWindow* window);
		static bool PlatformWindowHoveredWayland(const InternalWindow* window);
		/// <summary>
		/// This function returns whether raw mouse motion is supported on the current system.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>True if raw mouse input is supported, false otherwise.</returns>
		static bool PlatformRawMouseMotionSupported();
		static bool PlatformRawMouseMotionSupportedX11();
		static bool PlatformRawMouseMotionSupportedWayland();
		/// <summary>
		/// Sets the raw mouse motion mode for the specified window.
		///
		/// If raw (unscaled and unaccelerated) mouse motion is not supported, attempting to set this
		/// will emit Error::Feature_Unavailable.
		///
		/// Errors: Possible errors include Error::Invalid_Enum,
		///         Error::Platform_Error and Error::Feature_Unavailable.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set raw mouse input for.</param>
		/// <param name="enabled">Whether to enable or disable raw mouse input.</param>
		static void PlatformSetRawMouseMotion(const InternalWindow* window, bool enabled);
		static void PlatformSetRawMouseMotionX11(const InternalWindow* window, bool enabled);
		static void PlatformSetRawMouseMotionWayland(const InternalWindow* window, bool enabled);
		/// <summary>
		/// Sets the progress value and state on the taskbar for the specified window.
		///
		/// Errors: Possible errors include Error::Platform_Error and Error::Feature_Unavailable.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set progress for.</param>
		/// <param name="state">State of progress.</param>
		/// <param name="completed">How much has been completed. Valid values: 0 - 100.</param>
		static void PlatformSetProgress(const InternalWindow* window, ProgressState state, uint32_t completed);
		static void PlatformSetProgressX11(const InternalWindow* window, ProgressState state, uint32_t completed);
		static void PlatformSetProgressWayland(const InternalWindow* window, ProgressState state, uint32_t completed);
		/// <summary>
		/// This function returns the platform-specific scancode of the specified key.
		///
		/// If the key is TRAP::Input::Key::Unknown or does not exist on the keyboard this
		/// method will return -1.
		///
		/// Errors: Possible errors include Error::Invalid_Enum, Error::Platform_Error.
		/// Thread safety: This function may be called from any thread.
		/// </summary>
		/// <param name="key">Key to get scancode for.</param>
		/// <returns>The platform-specific scancode of the specified key or -1 if an error occured.</returns>
		static int32_t PlatformGetKeyScanCode(Input::Key key);
		static int32_t PlatformGetKeyScanCodeX11(Input::Key key);
		static int32_t PlatformGetKeyScanCodeWayland(Input::Key key);
		/// <summary>
		/// This function returns a string representation for the platform-specific scancode.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function may be called from any thread.
		/// </summary>
		/// <param name="scanCode">Scancode to get string representation for.</param>
		/// <returns>String representation of scancode or nullptr if an error occured.</returns>
		static const char* PlatformGetScanCodeName(int32_t scanCode);
		static const char* PlatformGetScanCodeNameX11(int32_t scanCode);
		static const char* PlatformGetScanCodeNameWayland(int32_t scanCode);
		/// <summary>
		/// This function sets the system clipboard to the specified, UTF-8 encoded string.
		///
		/// Errors: Possible errors include Error::Format_Unavailable and Error::Platform_Error.
		/// Pointer lifetime: The specified string is copied before this function returns.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="string">UTF-8 encoded string to be set for the clipboard.</param>
		static void PlatformSetClipboardString(const std::string& string);
		static void PlatformSetClipboardStringX11(const std::string& string);
		static void PlatformSetClipboardStringWayland(const std::string& string);
		/// <summary>
		/// This function returns the contents of the system clipboard, if it contains or
		/// is convertible to a UTF-8 encoded string. If the clipboard is empty or if its
		/// content cannot be converted, an empty string is returned and a Error::Format_Unavailable
		/// error is generated.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>
		/// UTF-8 encoded string containing clipboard contents, or empty string if an error occurred.
		/// </returns>
		static std::string PlatformGetClipboardString();
		static std::string PlatformGetClipboardStringX11();
		static std::string PlatformGetClipboardStringWayland();
		/// <summary>
		/// This function returns an array of names of Vulkan instance extensions required
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
		/// Remarks: Additional extensions may be required by future versions of the WindowingAPI.
		///          You should check if any extensions you wish to enable are already in the
		///          returned array, as it is an error to specify an extension more than once
		///          in the VkInstanceCreateInfo struct.
		/// Thread safety: This function may be called from any thread.
		/// </summary>
		/// <param name="extensions">output array for the required instance extensions.</param>
		static void PlatformGetRequiredInstanceExtensions(std::array<std::string, 2>& extensions);
		static void PlatformGetRequiredInstanceExtensionsX11(std::array<std::string, 2>& extensions);
		static void PlatformGetRequiredInstanceExtensionsWayland(std::array<std::string, 2>& extensions);
		/// <summary>
		/// Creates a Vulkan surface for the specified window.
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
		/// Remarks: If an error occurs before the creation call is made, the WindowingAPI returns
		///          the Vulkan error code most appropriate for the error. Appropriate use of
		///          WindowingAPI::VulkanSupported and WindowingAPI::GetRequiredInstanceExtensions
		///          should eliminate almost all occurrences of these errors.
		/// Thread safety: This function may be called from any thread. For synchronization details
		///                of Vulkan objects, see the Vulkan specification.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <param name="window">Internal window for which to create the surface for.</param>
		/// <param name="allocator">Optional allocator.</param>
		/// <param name="surface">Output variable for the new Vulkan surface.</param>
		/// <returns>VK_SUCCESS if successful, or a Vulkan error code if an error occurred.</returns>
		static VkResult PlatformCreateWindowSurface(VkInstance instance, const InternalWindow* window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		static VkResult PlatformCreateWindowSurfaceX11(VkInstance instance, const InternalWindow* window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		static VkResult PlatformCreateWindowSurfaceWayland(VkInstance instance, const InternalWindow* window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		/// <summary>
		/// This function maximizes the specified window if it was previously not
		/// maximized. If the window is already maximized, this function does nothing.
		///
		/// If the specified window is a full screen window, this function does nothing.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function may only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to maximize.</param>
		static void PlatformMaximizeWindow(const InternalWindow* window);
		static void PlatformMaximizeWindowX11(const InternalWindow* window);
		static void PlatformMaximizeWindowWayland(const InternalWindow* window);
		/// <summary>
		/// This function minimizes (iconifies) the specified window if it was previously
		/// restored. If the window is already minimized (iconified), this function does
		/// nothing.
		///
		/// If the specified window is a full screen window, the original monitor resolution
		/// is restored util the window is restored.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to minimize/iconify.</param>
		static void PlatformMinimizeWindow(const InternalWindow* window);
		static void PlatformMinimizeWindowX11(const InternalWindow* window);
		static void PlatformMinimizeWindowWayland(const InternalWindow* window);
		/// <summary>
		/// This function requests user attention to the specified window.
		/// On platforms where this is not supported, attention is requested
		/// to the application as a whole.
		///
		/// Once the user has given attention, usually by focusing the window
		/// or application, the system will end the request automatically.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to request user attention for.</param>
		static void PlatformRequestWindowAttention(const InternalWindow* window);
		static void PlatformRequestWindowAttentionX11(const InternalWindow* window);
		static void PlatformRequestWindowAttentionWayland(const InternalWindow* window);
		/// <summary>
		/// This function hides the specified window if it was previously visible. If
		/// the window is already hidden or is in full screen mode, this function
		/// does nothing.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to hide.</param>
		static void PlatformHideWindow(const InternalWindow* window);
		static void PlatformHideWindowX11(const InternalWindow* window);
		static void PlatformHideWindowWayland(const InternalWindow* window);
		/// <summary>
		/// Restores the specified window.
		/// This function restores the specified window if it was previously
		/// minimized (iconified) or maximized. If the window is already restored,
		/// this function does nothing.
		///
		/// If the specified window is a full screen window, the resolution chosen
		/// for the window is restored on the selected monitor.
		///
		/// Errors: Possible errors include Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to be restored.</param>
		static void PlatformRestoreWindow(InternalWindow* window);
		static void PlatformRestoreWindowX11(InternalWindow* window);
		static void PlatformRestoreWindowWayland(InternalWindow* window);
		/// <summary>
		/// Sets the size limits of the specified window.
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
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">Internal window to set size limits for.</param>
		/// <param name="minWidth">New minimum window width.</param>
		/// <param name="minHeight">New minimum window height.</param>
		/// <param name="maxWidth">New maximum window width.</param>
		/// <param name="maxHeight">New maximum window height.</param>
		static void PlatformSetWindowSizeLimits(InternalWindow* window, int32_t minWidth, int32_t minHeight,
		                                        int32_t maxWidth, int32_t maxHeight);
		static void PlatformSetWindowSizeLimitsX11(InternalWindow* window, int32_t minWidth, int32_t minHeight,
		                                        int32_t maxWidth, int32_t maxHeight);
		static void PlatformSetWindowSizeLimitsWayland(InternalWindow* window, int32_t minWidth, int32_t minHeight,
		                                        int32_t maxWidth, int32_t maxHeight);
		/// <summary>
		/// Enable/Disable drag and drop feature for the specified window.
		/// </summary>
		/// <param name="window">Internal window for which to set drag and drop.</param>
		/// <param name="value">Whether to enable or disable drag and drop.</param>
		static void PlatformSetDragAndDrop(InternalWindow* window, bool value);
		static void PlatformSetDragAndDropX11(InternalWindow* window, bool value);
		static void PlatformSetDragAndDropWayland(InternalWindow* window, bool value);
		//-------------------------------------------------------------------------------------------------------------------//
		//Single Platform Functions------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
		/// <summary>
		/// Retrieves the available modes for the specified monitor.
		/// </summary>
		/// <param name="monitor">Internal monitor to refresh video modes for.</param>
		/// <returns>True if video modes were successfully updated, false otherwise.</returns>
		static bool RefreshVideoModes(InternalMonitor* monitor);
		/// <summary>
		/// Initialize Vulkan API for further use.
		/// </summary>
		/// <param name="mode">Mode to use.</param>
		/// <returns>True when successfully initialized, false otherwise.</returns>
		static bool InitVulkan(uint32_t mode);
		/// <summary>
		/// Splits a color depth into red, green and blue bit depths.
		/// </summary>
		/// <param name="bpp">Bits per pixel depth to split.</param>
		/// <param name="red">Output variable for the red channel.</param>
		/// <param name="green">Output variable for the green channel.</param>
		/// <param name="blue">Output variable for the blue channel.</param>
		static void SplitBPP(int32_t bpp, int32_t& red, int32_t& green, int32_t& blue);
		/// <summary>
		/// Make the specified window and its video mode active on its monitor.
		/// </summary>
		/// <param name="window">Internal window to make active on the given monitor.</param>
		static void AcquireMonitor(InternalWindow* window);
		/// <summary>
		/// Make the specified window active on its monitor.
		/// </summary>
		/// <param name="window">Internal window to make active on the given monitor.</param>
		static void AcquireMonitorBorderless(InternalWindow* window);
		/// <summary>
		/// Remove the window and restore the original video mode.
		/// </summary>
		/// <param name="window">Internal window which to release from current acquired monitor.</param>
		static void ReleaseMonitor(const InternalWindow* window);
		/// <summary>
		/// Lexically compare video modes, used by qsort.
		/// </summary>
		/// <param name="fp">Internal video mode to sort.</param>
		/// <param name="sp">Internal video mode to sort.</param>
		/// <returns>Index for the sorted internal video mode.</returns>
		static int32_t CompareVideoModes(const void* fp, const void* sp);
		/// <summary>
		/// Updates the cursor image according to its cursor mode.
		/// </summary>
		/// <param name="window">Internal window to update.</param>
		static void UpdateCursorImage(const InternalWindow* window);
		/// <summary>
		/// Exit disabled cursor mode for the specified window.
		/// </summary>
		/// <param name="window">Internal window for which to enable cursor.</param>
		static void EnableCursor(InternalWindow* window);
		/// <summary>
		/// Apply disabled cursor mode to a focused window.
		/// </summary>
		/// <param name="window">Internal window for which to disable cursor.</param>
		static void DisableCursor(InternalWindow* window);
		/// <summary>
		/// Enables raw messages for the mouse for the specified window.
		/// </summary>
		/// <param name="window">Internal window for which to enable raw mouse motion.</param>
		static void EnableRawMouseMotion(const InternalWindow* window);
		/// <summary>
		/// Disables raw messages for the mouse.
		/// </summary>
		/// <param name="window">Internal window for which to disable raw mouse motion.</param>
		static void DisableRawMouseMotion(const InternalWindow* window);
		/// <summary>
		/// Get a string representation of a VkResult.
		/// </summary>
		/// <param name="result">VkResult to convert.</param>
		/// <returns>Description of the given VkResult.</returns>
		static std::string GetVulkanResultString(VkResult result);
		/// <summary>
		/// Notifies shared code of a cursor motion event.
		/// The position is specified in content area relative screen coordinates.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="xPos">X position of the cursor.</param>
		/// <param name="yPos">Y position of the cursor.</param>
		static void InputCursorPos(InternalWindow* window, double xPos, double yPos);
		/// <summary>
		/// Notifies shared code of a physical key event.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="key">Key that is pressed or released.</param>
		/// <param name="scancode">Scan code of the key which is pressed or released.</param>
		/// <param name="state">State of the key (pressed, repeated or released).</param>
		static void InputKey(InternalWindow* window, Input::Key key, int32_t scancode, Input::KeyState state);
		/// <summary>
		/// Notifies shared code of a Unicode codepoint input event.
		/// The 'plain' parameter determines whether to emit a regular character event.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="codePoint">Code point of the typed char.</param>
		static void InputChar(const InternalWindow* window, uint32_t codePoint);
		/// <summary>
		/// Notifies shared code of a mouse button click event.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="button">Mouse button that is pressed or released.</param>
		/// <param name="state">Mouse button state (pressed or released).</param>
		static void InputMouseClick(InternalWindow* window, Input::MouseButton button, TRAP::Input::KeyState state);
		/// <summary>
		/// Notifies shared code of a scroll event.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="xOffset">X offset of the scroll wheel.</param>
		/// <param name="yOffset">Y offset of the scroll wheel.</param>
		static void InputScroll(const InternalWindow* window, double xOffset, double yOffset);
		/// <summary>
		/// Notified shared code of a cursor enter/leave event.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="entered">Whether the cursor entered or leaved the window.</param>
		static void InputCursorEnter(InternalWindow* window, bool entered);
		/// <summary>
		/// Notifies shared code that a window framebuffer has been resized.
		/// The size is specified in pixels.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="width">New framebuffer width.</param>
		/// <param name="height">New framebuffer height.</param>
		static void InputFrameBufferSize(const InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// Notifies shared code that a window has been resized.
		/// The size is specified in screen coordinates.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="width">New window width.</param>
		/// <param name="height">New window height.</param>
		static void InputWindowSize(const InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// Notifies shared code that a window has been minimized.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="restored">Whether window was minimized or restored.</param>
		static void InputWindowMinimize(const InternalWindow* window, bool restored);
		/// <summary>
		/// Notifies shared code that a window has been maximized.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="restored">Whether window was maximized or restored.</param>
		static void InputWindowMaximize(const InternalWindow* window, bool restored);
		/// <summary>
		/// Notifies shared code that a window has moved.
		/// The position is specified in content area relative screen coordinates.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="x">X position of the window.</param>
		/// <param name="y">Y position of the window.</param>
		static void InputWindowPos(const InternalWindow* window, int32_t x, int32_t y);
		/// <summary>
		/// Notifies shared code that the user wishes to close a window.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		static void InputWindowCloseRequest(InternalWindow* window);
		/// <summary>
		/// Notifies shared code of files or directories dropped on a window.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="paths">All dropped paths.</param>
		static void InputDrop(const InternalWindow* window, const std::vector<std::string>& paths);
		/// <summary>
		/// Notifies shared code that a window has lost or received input focus.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="focused">Whether window is focused or not.</param>
		static void InputWindowFocus(InternalWindow* window, bool focused);
		/// <summary>
		/// Notifies shared code of a keyboard layout change.
		/// </summary>
		static void InputKeyboardLayout();
		/// <summary>
		/// Chooses the video mode most closely matching the desired one.
		/// </summary>
		/// <param name="monitor">Internal monitor to query.</param>
		/// <param name="desired">Desired internal video mode.</param>
		/// <returns>Pointer to closest matching internal video mode from the desired one.</returns>
		static InternalVideoMode* ChooseVideoMode(InternalMonitor* monitor, const InternalVideoMode& desired);
		/// <summary>
		/// Notifies shared code of a monitor connection or disconnection.
		/// </summary>
		/// <param name="monitor">Internal monitor that was connected or disconnected.</param>
		/// <param name="connected">Whether the monitor was connected or disconnected.</param>
		/// <param name="placement">Placement of the monitor.</param>
		static void InputMonitor(Scope<InternalMonitor> monitor, bool connected, uint32_t placement);
		/// <summary>
		/// Notifies shared code of a monitor connection or disconnection.
		/// </summary>
		/// <param name="monitorIndex">Index of the monitor which got disconnected.</param>
		/// <param name="placement">Placement of the monitor.</param>
		static void InputMonitorDisconnect(uint32_t monitorIndex, uint32_t placement);
		/// <summary>
		/// Sets the cursor clip rect to the window content area.
		/// </summary>
		/// <param name="window">Internal window for which to capture the cursor.</param>
		static void CaptureCursor(InternalWindow* window);
		/// <summary>
		/// Disables clip cursor
		/// </summary>
		static void ReleaseCursor();

		//-------//
		//Windows//
		//-------//
#ifdef TRAP_PLATFORM_WINDOWS
		/// <summary>
		/// Load necessary libraries (DLLs).
		/// </summary>
		/// <returns>True if loading of libraries was successful, false otherwise.</returns>
		static bool LoadLibraries();
		/// <summary>
		/// Unload used libraries (DLLs).
		/// </summary>
		static void FreeLibraries();
		/// <summary>
		/// Reports the specified error, appending information about the last Win32 error.
		/// </summary>
		/// <param name="error">Error that occurred.</param>
		/// <param name="description">Description of the error.</param>
		static void InputErrorWin32(Error error, const std::string& description);
		/// <summary>
		/// Notifies shared code that a window content scale has changed.
		/// The scale is specified as the ratio between the current and default DPI.
		/// </summary>
		/// <param name="window">Internal window which is meant.</param>
		/// <param name="xScale">New x content scale.</param>
		/// <param name="yScale">New y content scale.</param>
		static void InputWindowContentScale(const InternalWindow* window, float xScale, float yScale);
		/// <summary>
		/// Updates key names according to the current keyboard layout.
		/// </summary>
		static void UpdateKeyNamesWin32();
		/// <summary>
		/// Window callback function (handles window messages).
		/// </summary>
		/// <param name="hwnd">Window handle.</param>
		/// <param name="uMsg">Message.</param>
		/// <param name="wParam">Message parameter.</param>
		/// <param name="lParam">Message parameter.</param>
		/// <returns>True if the message was handled, false otherwise.</returns>
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		/// <summary>
		/// Window callback function (handles window messages).
		/// </summary>
		/// <param name="hwnd">Window handle.</param>
		/// <param name="uMsg">Message.</param>
		/// <param name="wParam">Message parameter.</param>
		/// <param name="lParam">Message parameter.</param>
		/// <returns>True if the message was handled, false otherwise.</returns>
		static LRESULT CALLBACK HelperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		/// <summary>
		/// Callback for EnumDisplayMonitors in CreateMonitor.
		/// </summary>
		/// <param name="handle">Monitor handle.</param>
		/// <param name="dc">Monitor device context.</param>
		/// <param name="rect">Monitor rectangle.</param>
		/// <param name="data">User data.</param>
		/// <returns>True to continue enumeration, false to stop.</returns>
		static BOOL CALLBACK MonitorCallback(HMONITOR handle, HDC dc, RECT* rect, LPARAM data);
		/// <summary>
		/// Create monitor from an adapter and (optionally) a display.
		/// </summary>
		/// <param name="adapter">Adapter to create monitor from.</param>
		/// <param name="display">Display to create monitor from.</param>
		/// <returns>New Internal monitor.</returns>
		static Scope<InternalMonitor> CreateMonitor(DISPLAY_DEVICEW* adapter, DISPLAY_DEVICEW* display);
		/// <summary>
		/// Poll for changes in the set of connected monitors.
		/// </summary>
		static void PollMonitorsWin32();
		/// <summary>
		/// Fit the Internal window to the monitor.
		/// </summary>
		/// <param name="window">Internal window to fit.</param>
		static void FitToMonitor(const InternalWindow* window);
		/// <summary>
		/// Change the current video mode.
		/// </summary>
		/// <param name="monitor">Internal monitor to set internal video mode for.</param>
		/// <param name="desired">Desired internal video mode for the monitor.</param>
		static void SetVideoModeWin32(InternalMonitor* monitor, const InternalVideoMode& desired);
		/// <summary>
		/// Retrieve the content scale of the given monitor.
		/// </summary>
		/// <param name="handle">Monitor handle.</param>
		/// <param name="xScale">X monitor content scale.</param>
		/// <param name="yScale">Y monitor content scale.</param>
		static void GetMonitorContentScaleWin32(HMONITOR handle, float& xScale, float& yScale);
		/// <summary>
		/// Returns the window style for the specified window.
		/// </summary>
		/// <param name="window">Internal window to query.</param>
		/// <returns>Window style as a DWORD.</returns>
		static DWORD GetWindowStyle(const InternalWindow* window);
		/// <summary>
		/// Returns the extended window style for the specified window.
		/// </summary>
		/// <param name="window">Internal window to query.</param>
		/// <returns>Window Ex style as a DWORD.</returns>
		static DWORD GetWindowExStyle(const InternalWindow* window);
		/// <summary>
		/// Translate content area size to full window size according to styles and DPI.
		/// </summary>
		/// <param name="style">Window style.</param>
		/// <param name="exStyle">Window Ex style.</param>
		/// <param name="contentWidth">Content width.</param>
		/// <param name="contentHeight">Content height.</param>
		/// <param name="fullWidth">Full width.</param>
		/// <param name="fullHeight">Full height.</param>
		/// <param name="dpi">DPI.</param>
		static void GetFullWindowSize(DWORD style, DWORD exStyle,
									  int32_t contentWidth, int32_t contentHeight,
									  int32_t& fullWidth, int32_t& fullHeight,
									  UINT dpi);
		/// <summary>
		/// Returns whether the cursor is in the content area of the specified window.
		/// </summary>
		/// <param name="window">Internal window to check</param>
		/// <returns>True if cursor is inside the given windows content area, false otherwise.</returns>
		static bool CursorInContentArea(const InternalWindow* window);
		/// <summary>
		/// Creates an RGBA icon or cursor,
		/// </summary>
		/// <param name="image">Image to use.</param>
		/// <param name="xHot">Center x coordinate of the image.</param>
		/// <param name="yHot">Center y coordinate of the image.</param>
		/// <param name="icon">Whether it is an icon or a cursor.</param>
		/// <returns>Handle to the icon.</returns>
		static HICON CreateIcon(const Image* const image, int32_t xHot, int32_t yHot, bool icon);
		/// <summary>
		/// Update native window styles to match attributes.
		/// </summary>
		/// <param name="window">Internal window to update window style for.</param>
		static void UpdateWindowStyles(const InternalWindow* window);
		/// <summary>
		/// Creates a dummy window for behind-the-scenes work.
		/// </summary>
		/// <returns>True if creation was successful, false otherwise.</returns>
		static bool CreateHelperWindow();
		/// <summary>
		/// Creates the TRAP window.
		/// </summary>
		/// <param name="window">Internal window to create.</param>
		/// <param name="WNDConfig">Window configuration.</param>
		/// <returns>True if creation was successful, false otherwise.</returns>
		static bool CreateNativeWindow(InternalWindow* window,
			                           const WindowConfig& WNDConfig);
		/// <summary>
		/// Manually maximize the window, for when SW_MAXIMIZE cannot be used.
		/// </summary>
		/// <param name="window">Internal window to maximize manually.</param>
		static void MaximizeWindowManually(const InternalWindow* window);
		/// <summary>
		/// Retrieve the Win32 HInstance for the application.
		/// </summary>
		/// <returns>HInstance.</returns>
		static HINSTANCE GetWin32HInstance();

		friend bool TRAP::Input::InitController();
		//----------//
		//Linux(X11)//
		//----------//
#elif defined(TRAP_PLATFORM_LINUX)
		/// <summary>
		/// Calculates the refresh rate, in Hz, from the specified RandR mode info.
		/// </summary>
		/// <param name="mi">RandR mode info.</param>
		/// <returns>Refresh rate.</returns>
		static int32_t CalculateRefreshRate(const XRRModeInfo* mi);
		/// <summary>
		/// Create InternalVideoMode from RandR mode info.
		/// </summary>
		/// <param name="mi">RandR mode info.</param>
		/// <param name="ci">RandR CRTC info.</param>
		/// <returns>Newly created InternalVideoMode.</returns>
		static InternalVideoMode VideoModeFromModeInfo(const XRRModeInfo* mi, const XRRCrtcInfo* ci);
		/// <summary>
		/// Sends an EWMH or ICCCM event to the window manager.
		/// </summary>
		/// <param name="window">Internal window to send event for.</param>
		/// <param name="type">Event type.</param>
		/// <param name="a">Event data a.</param>
		/// <param name="b">Event data b.</param>
		/// <param name="c">Event data c.</param>
		/// <param name="d">Event data d.</param>
		/// <param name="e">Event data e.</param>
		static void SendEventToWM(const InternalWindow* window, Atom type, int64_t a, int64_t b, int64_t c,
		                          int64_t d, int64_t e);
		/// <summary>
		/// Returns whether it is a _NET_FRAME_EXTENTS event for the specified window.
		/// </summary>
		/// <param name="display">X11 display.</param>
		/// <param name="event">X11 event.</param>
		/// <param name="pointer">Pointer to the window.</param>
		/// <returns>True if it is a _NET_FRAME_EXTENTS event for the specified window, false otherwise.</returns>
		static bool IsFrameExtentsEvent(Display* display, XEvent* event, XPointer pointer);
		/// <summary>
		/// Wait for data to arrive on any of the specified file descriptors
		/// </summary>
		/// <param name="fds">Array of file descriptors to wait for.</param>
		/// <param name="count">Number of file descriptors to wait for.</param>
		/// <param name="timeout">Time out in seconds.</param>
		/// <returns>Number of file descriptors with data.</returns>
		static bool WaitForData(pollfd* fds, nfds_t count, double* timeout);
		/// <summary>
		/// Wait for event data to arrive on the X11 display socket.
		/// This avoids blocking other threads via the per-display Xlib lock.
		/// </summary>
		/// <param name="timeout">Time out in seconds.</param>
		/// <returns>True if data was received, false otherwise.</returns>
		static bool WaitForX11Event(double* timeout);
		/// <summary>
		/// Writes a byte to the empty event pipe
		/// </summary>
		static void WriteEmptyEvent();
		/// <summary>
		/// Drains available data from the empty event pipe
		/// </summary>
		static void DrainEmptyEvents();
		/// <summary>
		/// Create the pipe for empty events without assuming the OS has pipe2(2)
		/// </summary>
		/// <returns>True if the pipe was created, false otherwise.</returns>
		static bool CreateEmptyEventPipe();
		/// <summary>
		/// Retrieve a single window property of the specified type.
		/// </summary>
		/// <param name="window">X11 window.</param>
		/// <param name="property">Property to retrieve.</param>
		/// <param name="type">Property type.</param>
		/// <param name="value">Output property value.</param>
		/// <returns>Amount of bytes retrieved.</returns>
		static uint64_t GetWindowPropertyX11(::Window window, Atom property, Atom type, uint8_t** value);
		/// <summary>
		/// Updates the normal hints according to the window settings.
		/// </summary>
		/// <param name="window">Internal window to update hints for.</param>
		/// <param name="width">Width of the window.</param>
		/// <param name="height">Height of the window.</param>
		static void UpdateNormalHints(InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// Waits until a VisibilityNotify event arrives for the specified window or the timeout period elapses.
		/// </summary>
		/// <param name="window">Internal window to wait for.</param>
		/// <returns>True if a VisibilityNotify event was received, false otherwise.</returns>
		static bool WaitForVisibilityNotify(InternalWindow* window);
		/// <summary>
		/// Updates the full screen status of the window.
		/// </summary>
		/// <param name="window">Internal window to update window mode for.</param>
		static void UpdateWindowMode(InternalWindow* window);
		/// <summary>
		/// Returns the mode info for a RandR mode XID.
		/// </summary>
		/// <param name="sr">RandR screen resource.</param>
		/// <param name="id">RandR mode XID.</param>
		/// <returns>RandR mode info.</returns>
		static const XRRModeInfo* GetModeInfo(const XRRScreenResources* sr, RRMode id);
		/// <summary>
		/// Retrieve system content scale via folklore heuristics.
		/// </summary>
		/// <param name="xScale">Output variable for x system content scale.</param>
		/// <param name="yScale">Output variable for y system content scale.</param>
		static void GetSystemContentScale(float& xScale, float& yScale);
		/// <summary>
		/// Look for and initialize supported X11 extensions.
		/// </summary>
		/// <returns>True on success, false otherwise.</returns>
		static bool InitExtensions();
		/// <summary>
		/// Check whether the running window manager is EMWH-compliant.
		/// </summary>
		static void DetectEWMH();
		/// <summary>
		/// Sets the X error handler callback.
		/// </summary>
		static void GrabErrorHandlerX11();
		/// <summary>
		/// X error handler.
		/// </summary>
		/// <param name="display">X11 display.</param>
		/// <param name="event">X11 error event.</param>
		/// <returns>True if the error was handled, false otherwise.</returns>
		static int32_t ErrorHandler(Display* display, XErrorEvent* event);
		/// <summary>
		/// Clears the X error handler callback.
		/// </summary>
		static void ReleaseErrorHandlerX11();
		/// <summary>
		/// Check whether the specified atom is supported.
		/// </summary>
		/// <param name="supportedAtoms">List of supported atoms.</param>
		/// <param name="atomCount">Number of supported atoms.</param>
		/// <param name="atomName">Atom to check.</param>
		/// <returns>Atom on success, 0 otherwise.</returns>
		static Atom GetAtomIfSupported(const Atom* supportedAtoms, uint64_t atomCount, std::string_view atomName);
		/// <summary>
		/// Create a blank cursor for hidden and disabled cursor modes.
		/// </summary>
		/// <returns>Newly created Cursor.</returns>
		static Cursor CreateHiddenCursor();
		/// <summary>
		/// Check whether the IM has a usable style.
		/// </summary>
		/// <returns>True if an input method was found, false otherwise.</returns>
		static bool HasUsableInputMethodStyle();
		/// <summary>
		/// Input method destroy callback.
		/// </summary>
		/// <param name="im">Input method.</param>
		/// <param name="clientData">User data.</param>
		/// <param name="callData">Input method call data.</param>
		static void InputMethodDestroyCallback(XIM im, XPointer clientData, XPointer callData);
		/// <summary>
		/// Input method instantiate callback.
		/// </summary>
		/// <param name="display">X11 display.</param>
		/// <param name="clientData">User data.</param>
		/// <param name="callData">Input method call data.</param>
		static void InputMethodInstantiateCallback(Display* display, XPointer clientData, XPointer callData);
		/// <summary>
		/// Poll for changes in the set of connected monitors.
		/// </summary>
		static void PollMonitorsX11();
		/// <summary>
		/// Returns whether the event is a selection event.
		/// </summary>
		/// <param name="display">X11 display.</param>
		/// <param name="event">X11 event.</param>
		/// <param name="pointer">Pointer to the event.</param>
		/// <returns>True if the event is a selection event, false otherwise.</returns>
		static int32_t IsSelectionEvent(Display* display, XEvent* event, XPointer pointer);
		/// <summary>
		/// Set the specified property to the selection converted to the requested target.
		/// </summary>
		/// <param name="request">Selection request.</param>
		/// <returns>Atom on success, 0 otherwise.</returns>
		static Atom WriteTargetToProperty(const XSelectionRequestEvent* request);
		/// <summary>
		/// Handles a selection request.
		/// </summary>
		/// <param name="event">Selection request event.</param>
		static void HandleSelectionRequest(XEvent& event);
		/// <summary>
		/// Push contents of our selection to clipboard manager.
		/// </summary>
		static void PushSelectionToManagerX11();
		/// <summary>
		/// Create X11 input context.
		/// </summary>
		/// <param name="window">Internal window to create input context for.</param>
		static void CreateInputContextX11(InternalWindow* window);
		/// <summary>
		/// Check whether the specified visual is transparent.
		/// </summary>
		/// <param name="visual">X11 visual.</param>
		/// <returns>True if the visual is a transparent visual.</returns>
		static bool IsVisualTransparentX11(Visual* visual);
		/// <summary>
		/// Create the X11 window (and its colormap).
		/// </summary>
		/// <param name="window">Internal window to create window for.</param>
		/// <param name="WNDConfig">Window configuration.</param>
		/// <param name="visual">X11 visual.</param>
		/// <param name="depth">Window depth.</param>
		/// <returns>True on success, false otherwise.</returns>
		static bool CreateNativeWindow(InternalWindow* window, WindowConfig& WNDConfig, Visual* visual,
		                               int32_t depth);
		/// <summary>
		/// Creates a native cursor object from the specified image and hotspot.
		/// </summary>
		/// <param name="image">Image to use.</param>
		/// <param name="xHotSpot">X center coordinate of the given image.</param>
		/// <param name="yHotSpot">Y center coordinate of the given image.</param>
		/// <returns>Newly created cursor.</returns>
		static Cursor CreateCursorX11(const TRAP::Image* const image, int32_t xHotSpot, int32_t yHotSpot);
		/// <summary>
		/// Returns whether the window is iconified/minimized
		/// </summary>
		/// <param name="window">Internal window to check.</param>
		/// <returns>Window state.</returns>
		static int32_t GetWindowState(const InternalWindow* window);
		/// <summary>
		/// Convert XKB KeySym to Unicode.
		/// </summary>
		/// <param name="keysym">XKB KeySym.</param>
		/// <returns>Unicode character.</returns>
		static uint32_t KeySymToUnicode(uint32_t keySym);
		struct CodePair
		{
			uint16_t keySym;
			uint16_t UCS;
		};
		static const std::array<CodePair, 828> KeySymTab;
		/// <summary>
		/// Encode a Unicode code point to a UTF-8 stream.
		/// </summary>
		/// <param name="s">UTF-8 stream.</param>
		/// <param name="ch">Unicode code point.</param>
		/// <returns>Number of bytes written.</returns>
		static std::size_t EncodeUTF8(char* s, uint32_t ch);
		/// <summary>
		/// Return selection string from specified selection.
		/// </summary>
		/// <param name="selection">Selection to retrieve.</param>
		/// <returns>Selection string.</returns>
		static std::string GetSelectionString(Atom selection);
		/// <summary>
		/// Returns whether it is a property event for the specified selection transfer.
		/// </summary>
		/// <param name="display">X11 display.</param>
		/// <param name="event">X11 event.</param>
		/// <param name="pointer">Pointer to the event.</param>
		/// <returns>True if the event is a property new value event for the specified selection, false otherwise.</returns>
		static int32_t IsSelPropNewValueNotify(Display* display, XEvent* event, XPointer pointer);
		/// <summary>
		/// Convert the specified Latin-1 string to UTF-8.
		/// </summary>
		/// <param name="source">Latin-1 string.</param>
		/// <returns>UTF-8 string.</returns>
		static std::string ConvertLatin1ToUTF8(std::string_view source);
		/// <summary>
		/// Reports the specified error, appending information about the last X error.
		/// </summary>
		/// <param name="error">Error code.</param>
		/// <param name="message">Description of error.</param>
		static void InputErrorX11(Error error, const std::string& message);
		/// <summary>
		/// Process the specified X event.
		/// </summary>
		/// <param name="event">X event.</param>
		static void ProcessEvent(XEvent& event);
		/// <summary>
		/// Translates an X11 key code to a TRAP key token.
		/// </summary>
		/// <param name="scanCode">X11 key code.</param>
		/// <returns>Translated TRAP::Input::Key.</returns>
		static Input::Key TranslateKey(int32_t scanCode);
#ifdef X_HAVE_UTF8_STRING
		/// <summary>
		/// Decode a Unicode code point from a UTF-8 stream.
		/// </summary>
		/// <param name="s">UTF-8 stream.</param>
		/// <returns>Unicode code point.</returns>
		static uint32_t DecodeUTF8(const char** s);
#endif
		/// <summary>
		/// Splits and translates a text/uri-list into separate file paths.
		/// </summary>
		/// <param name="text">Text to split.</param>
		/// <param name="count">Number of file paths.</param>
		/// <returns>Vector of file paths.</returns>
		static std::vector<std::string> ParseUriList(char* text, int32_t& count);
		/// <summary>
		/// Set the current video mode for the specified monitor.
		/// </summary>
		/// <param name="monitor">Monitor to set video mode for.</param>
		/// <param name="desired">Desired video mode.</param>
		static void SetVideoModeX11(InternalMonitor* monitor, const InternalVideoMode& desired);
		/// <summary>
		/// Restore the saved(original) video mode for the specified monitor.
		/// </summary>
		/// <param name="monitor">Monitor to restore video mode for.</param>
		static void RestoreVideoModeX11(InternalMonitor* monitor);
		/// <summary>
		/// Allocates and returns a monitor object with the specified name.
		/// </summary>
		/// <param name="name">Monitor name.</param>
		/// <returns>Newly created monitor object.</returns>
		static Scope<InternalMonitor> CreateMonitor(std::string name);
		/// <summary>
		/// Creates a dummy window for behind-the-scenes work.
		/// </summary>
		/// <returns>Newly created X11 Window.</returns>
		static ::Window CreateHelperWindow();
		/// <summary>
		/// Translate the X11 KeySyms for a key to a TRAP key.
		/// </summary>
		/// <param name="keySyms">X11 KeySyms.</param>
		/// <param name="width">Width of KeySyms.</param>
		/// <returns>Translated TRAP::Input::Key.</returns>
		static Input::Key TranslateKeySyms(const KeySym* keySyms, int32_t width);
		/// <summary>
		/// Clear its handle when the input context has been destroyed.
		/// </summary>
		/// <param name="ic">Input context.</param>
		/// <param name="clientData">Client data.</param>
		/// <param name="callData">Call data.</param>
		static void InputContextDestroyCallback(XIC ic, XPointer clientData, XPointer callData);
		/// <summary>
		/// Retrieve the current keyboard layout name.
		/// </summary>
		/// <returns>Current keyboard layout name.</returns>
		static std::string GetLinuxKeyboardLayoutName();
		static std::string GetLinuxKeyboardLayoutNameX11();
		static std::string GetLinuxKeyboardLayoutNameWayland();

		static void RegistryHandleGlobalRemove(void* userData, wl_registry* registry, uint32_t name);
		static void RegistryHandleGlobal(void* userData, wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
		inline static constexpr wl_registry_listener RegistryListener
		{
			RegistryHandleGlobal,
			RegistryHandleGlobalRemove
		};

		static void WMBaseHandlePing(void* userData, xdg_wm_base* wmBase, uint32_t serial);
		inline static constexpr xdg_wm_base_listener WMBaseListener
		{
			WMBaseHandlePing
		};

		static void SeatHandleCapabilities(void* userData, wl_seat* seat, uint32_t caps);
		static void SeatHandleName(void* userData, wl_seat* seat, const char* name);
		inline static constexpr wl_seat_listener SeatListener
		{
			SeatHandleCapabilities,
			SeatHandleName
		};

		static void PointerHandleEnter(void* userData, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t sX, wl_fixed_t sY);
		static void PointerHandleLeave(void* userData, wl_pointer* pointer, uint32_t serial, wl_surface* surface);
		static void PointerHandleMotion(void* userData, wl_pointer* pointer, uint32_t time, wl_fixed_t sX, wl_fixed_t sY);
		static void PointerHandleButton(void* userData, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
		static void PointerHandleAxis(void* userData, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
		inline static constexpr wl_pointer_listener PointerListener
		{
			PointerHandleEnter,
			PointerHandleLeave,
			PointerHandleMotion,
			PointerHandleButton,
			PointerHandleAxis,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};

		static void KeyboardHandleKeymap(void* userData, wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size);
		static void KeyboardHandleEnter(void* userData, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys);
		static void KeyboardHandleLeave(void* userData, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface);
		static void KeyboardHandleKey(void* userData, wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t scanCode, uint32_t state);
		static void KeyboardHandleModifiers(void* userData, wl_keyboard* keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group);
#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
		static void KeyboardHandleRepeatInfo(void* userData, wl_keyboard* keyboard, int32_t rate, int32_t delay);
#endif
		inline static constexpr wl_keyboard_listener KeyboardListener
		{
			KeyboardHandleKeymap,
			KeyboardHandleEnter,
			KeyboardHandleLeave,
			KeyboardHandleKey,
			KeyboardHandleModifiers,
#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
			KeyboardHandleRepeatInfo
#endif
		};

		static void OutputHandleGeometry(void* userData, wl_output* output, int32_t x, int32_t y, int32_t physicalWidth, int32_t physicalHeight, int32_t subpixel, const char* make, const char* model, int32_t transform);
		static void OutputHandleMode(void* userData, wl_output* output, uint32_t flags, int32_t width, int32_t height, int32_t refresh);
		static void OutputHandleDone(void* userData, wl_output* output);
		static void OutputHandleScale(void* userData, wl_output* output, int32_t factor);
#ifdef WL_OUTPUT_NAME_SINCE_VERSION
		static void OutputHandleName(void* userData, wl_output* output, const char* name);
		static void OutputHandleDescription(void* userData, wl_output* output, const char* description);
#endif
		inline static constexpr wl_output_listener OutputListener
		{
			OutputHandleGeometry,
			OutputHandleMode,
			OutputHandleDone,
			OutputHandleScale,
#ifdef WL_OUTPUT_NAME_SINCE_VERSION
			OutputHandleName,
			OutputHandleDescription
#endif
		};

		static void DataDeviceHandleDataOffer(void* userData, wl_data_device* device, wl_data_offer* offer);
		static void DataDeviceHandleEnter(void* userData, wl_data_device* device, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y, wl_data_offer* offer);
		static void DataDeviceHandleLeave(void* userData, wl_data_device* device);
		static void DataDeviceHandleMotion(void* userData, wl_data_device* device, uint32_t time, wl_fixed_t x, wl_fixed_t y);
		static void DataDeviceHandleDrop(void* userData, wl_data_device* device);
		static void DataDeviceHandleSelection(void* userData, wl_data_device* device, wl_data_offer* offer);
		inline static constexpr wl_data_device_listener DataDeviceListener
		{
			DataDeviceHandleDataOffer,
			DataDeviceHandleEnter,
			DataDeviceHandleLeave,
			DataDeviceHandleMotion,
			DataDeviceHandleDrop,
			DataDeviceHandleSelection
		};

		static void DataOfferHandleOffer(void* userData, wl_data_offer* offer, const char* mimeType);
		inline static constexpr wl_data_offer_listener DataOfferListener
		{
			DataOfferHandleOffer,
			nullptr,
			nullptr
		};

		static bool LoadCursorThemeWayland();
		static std::string ReadDataOfferAsString(wl_data_offer* offer, const char* mimeType);
		static bool FlushDisplay();

		friend std::string TRAP::Input::GetKeyboardLayoutName();
#endif
	};
}
#endif /*TRAP_WINDOWINGAPI_H*/