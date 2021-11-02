/*
Copyright (c) 2002-2006 Marcus Geelnard

Copyright (c) 2006-2019 Camilla Loewy

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
#include "ImageLoader/Image.h"
#include "Input/Input.h"

namespace TRAP::INTERNAL
{
	class WindowingAPI final
	{
	private:
		/// <summary>
		/// Private Constructor.
		/// </summary>
		WindowingAPI() = default;
		/// <summary>
		/// Private Destructor.
		/// </summary>
		~WindowingAPI() = default;

	public:
		/// <summary>
		/// Deleted Copy Constructor.
		/// </summary>
		WindowingAPI(const WindowingAPI&) = delete;
		/// <summary>
		/// Deleted Copy Assignment Operator.
		/// </summary>
		WindowingAPI& operator=(const WindowingAPI&) = delete;
		/// <summary>
		/// Deleted Move Constructor.
		/// </summary>
		WindowingAPI(WindowingAPI&&) = delete;
		/// <summary>
		/// Deleted Move Assignment Operator.
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
		struct VkXlibSurfaceCreateInfoKHR;
		struct VkXcbSurfaceCreateInfoKHR;
		//---------//
		//Windows//
		//---------//
#ifdef TRAP_PLATFORM_WINDOWS
		enum class Monitor_DPI_Type;
		enum class Process_DPI_Awareness;
#elif defined(TRAP_PLATFORM_LINUX)
		typedef struct xcb_connection_t xcb_connection_t;
		typedef XID xcb_window_t;
		typedef XID xcb_visualid_t;
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
		/// <param name="window">The window that received the event.</param>
		/// <param name="xPos">
		/// The new x-coordinate, in screen coordinates, of the upper-left corner of the content area.
		/// </param>
		/// <param name="yPos">
		/// The new y-coordinate, in screen coordinates, of the upper-left corner of the content area.
		/// </param>
		typedef void (*WindowPositionFunc)(const InternalWindow* window, int32_t xPos, int32_t yPos);
		/// <summary>
		/// The function pointer type for window size callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="width">The new width, in screen coordinates.</param>
		/// <param name="height">The new height, in screen coordinates.</param>
		typedef void (*WindowSizeFunc)(const InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// The function pointer type for window minimize/iconfiy callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="restored">
		/// True if the window was minimized/iconified, or false if it was restored.
		/// </param>
		typedef void (*WindowMinimizeFunc)(const InternalWindow* window, bool restored);
		/// <summary>
		/// The function pointer type for window maximize callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="restored">True if the window was maximized, or false if it was restored.</param>
		typedef void (*WindowMaximizeFunc)(const InternalWindow* window, bool restored);
		/// <summary>
		/// The function pointer type for window close callbacks.
		/// </summary>
		/// <param name="window">The window that the user attempted to close.</param>
		typedef void (*WindowCloseFunc)(const InternalWindow* window);
		/// <summary>
		/// The function pointer type for window focus callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="focused">True fi the window was given input focus, or false if it lost it.</param>
		typedef void (*WindowFocusFunc)(const InternalWindow* window, bool focused);
		/// <summary>
		/// The function pointer type for framebuffer size callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="width">The new width, in pixels, of the framebuffer.</param>
		/// <param name="height">The new height, in pixels, of the framebuffer.</param>
		typedef void (*FrameBufferSizeFunc)(const InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// The function pointer type for window content scale callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="xScale">The new x-axis content scale of the window.</param>
		/// <param name="yScale">The new y-axis content scale of the window.</param>
		typedef void (*WindowContentScaleFunc)(const InternalWindow* window, float xScale, float yScale);
		/// <summary>
		/// The function pointer type for mouse button callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="mouseButton">The Input::MouseButton that was pressed or released.</param>
		/// <param name="pressed">True if mouse button is pressed, false otherwise.</param>
		typedef void (*MouseButtonFunc)(const InternalWindow* window, Input::MouseButton mouseButton, bool pressed);
		/// <summary>
		/// The function pointer type for cursor position callbacks.
		/// </summary>
		/// <param name="window>The window that received the event.</param>
		/// <param name="xPos">The new cursor x-coordinate, relative to the left edge of the content area.</param>
		/// <param name="yPos">The new cursor y-coordinate, relative to the top edge of the content area.</param>
		typedef void (*CursorPositionFunc)(const InternalWindow* window, double xPos, double yPos);
		/// <summary>
		/// The function pointer type for cursor enter callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="entered">True if the cursor entered the window's content area, false if it left it.</param>
		typedef void (*CursorEnterFunc)(const InternalWindow* window, bool entered);
		/// <summary>
		/// The function pointer type for scroll callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="xOffset">The scroll offset along the x-axis.</param>
		/// <param name="yOffset">The scroll offset along the y-axis.</param>
		typedef void (*ScrollFunc)(const InternalWindow* window, double xOffset, double yOffset);
		/// <summary>
		/// The function pointer type for keyboard key callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="key">The Input::Key that was pressed or released.</param>
		/// <param name="pressed">True if key is pressed, false if it is released.</param>
		typedef void (*KeyFunc)(const InternalWindow* window, Input::Key key, bool pressed);
		/// <summary>
		/// The function pointer type for Unicode character callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="codePoint">The Unicode code point of the character.</param>
		typedef void (*CharFunc)(const InternalWindow* window, uint32_t codePoint);
		/// <summary>
		/// The function pointer type for path drop callbacks.
		/// </summary>
		/// <param name="window">The window that received the event.</param>
		/// <param name="paths">The UTF-8 encoded file and/or directory path names.</param>
		typedef void (*DropFunc)(const InternalWindow* window, std::vector<std::string> paths);
		//The function pointer type for monitor configuration callbacks.

		/// <summary>
		/// The function pointer type for monitor configuration callbacks.
		/// </summary>
		/// <param name="monitor">The monitor that was connceted or disconnceted.</param>
		/// <param name="connceted">True if monitor got connceted or false if it got disconnceted.</param>
		typedef void (*MonitorFunc)(const InternalMonitor* monitor, bool connected);
	private:
		//--------------//
		//Vulkan Surface//
		//--------------//
#ifdef TRAP_PLATFORM_WINDOWS
		typedef VkFlags VkWin32SurfaceCreateFlagsKHR;
		typedef VkResult(*PFN_vkCreateWin32SurfaceKHR)(VkInstance, const VkWin32SurfaceCreateInfoKHR*,
		                                               const VkAllocationCallbacks*, VkSurfaceKHR*);
#elif defined(TRAP_PLATFORM_LINUX)
		typedef VkResult (*PFN_vkCreateXlibSurfaceKHR)(VkInstance, const VkXlibSurfaceCreateInfoKHR*,
		                                               const VkAllocationCallbacks*, VkSurfaceKHR*);
		typedef VkBool32 (*PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)(VkPhysicalDevice, uint32_t,
		                                                                      Display*, VisualID);
		typedef VkResult (*PFN_vkCreateXcbSurfaceKHR)(VkInstance, const VkXcbSurfaceCreateInfoKHR*,
		                                              const VkAllocationCallbacks*, VkSurfaceKHR*);
		typedef VkBool32 (*PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)(VkPhysicalDevice, uint32_t,
		                                                                     xcb_connection_t*, xcb_visualid_t);
#endif
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
#elif defined(TRAP_PLATFORM_LINUX)
		//----------//
		//Linux(X11)//
		//----------//

		//RandR
		typedef void(*PFN_XRRFreeCrtcInfo)(XRRCrtcInfo*);
		typedef void(*PFN_XRRFreeOutputInfo)(XRROutputInfo*);
		typedef void(*PFN_XRRFreeScreenResources)(XRRScreenResources*);
		typedef XRRCrtcInfo*(*PFN_XRRGetCrtcInfo)(Display*, XRRScreenResources*, RRCrtc);
		typedef XRROutputInfo*(*PFN_XRRGetOutputInfo)(Display*, XRRScreenResources*, RROutput);
		typedef RROutput(*PFN_XRRGetOutputPrimary)(Display*, ::Window);
		typedef XRRScreenResources*(*PFN_XRRGetScreenResourcesCurrent)(Display*, ::Window);
		typedef int32_t(*PFN_XRRQueryExtension)(Display*, int32_t*, int32_t*);
		typedef int32_t(*PFN_XRRQueryVersion)(Display*, int32_t*, int32_t*);
		typedef void(*PFN_XRRSelectInput)(Display*, ::Window, int32_t);
		typedef int32_t(*PFN_XRRSetCrtcConfig)(Display*, XRRScreenResources*, RRCrtc, Time, int32_t, int32_t,
		                                       RRMode, Rotation, RROutput*, int32_t);
		typedef int32_t(*PFN_XRRUpdateConfiguration)(XEvent*);

		//XCursor
		typedef XcursorImage*(*PFN_XcursorImageCreate)(int32_t, int32_t);
		typedef void(*PFN_XcursorImageDestroy)(XcursorImage*);
		typedef Cursor(*PFN_XcursorImageLoadCursor)(Display*, const XcursorImage*);
		typedef char*(*PFN_XcursorGetTheme)(Display*);
		typedef int32_t(*PFN_XcursorGetDefaultSize)(Display*);
		typedef XcursorImage*(*PFN_XcursorLibraryLoadImage)(const char*, const char*, int32_t);

		//Xinerama
		typedef int32_t (*PFN_XineramaIsActive)(Display*);
		typedef int32_t(*PFN_XineramaQueryExtension)(Display*, int32_t*, int32_t*);
		typedef XineramaScreenInfo*(*PFN_XineramaQueryScreens)(Display*, int32_t*);

		//XCB
		typedef xcb_connection_t*(*PFN_XGetXCBConnection)(Display*);

		//XI
		typedef int32_t(*PFN_XIQueryVersion)(Display*, int32_t*, int32_t*);
		typedef int32_t(*PFN_XISelectEvents)(Display*, ::Window, XIEventMask*, int32_t);

		//XRender
		typedef int32_t(*PFN_XRenderQueryExtension)(Display*,int32_t*, int32_t*);
		typedef int32_t(*PFN_XRenderQueryVersion)(Display* dpy, int32_t*, int32_t*);
		typedef XRenderPictFormat*(*PFN_XRenderFindVisualFormat)(Display*, Visual const*);

		//Vulkan
		typedef VkFlags VkXlibSurfaceCreateFlagsKHR;
		typedef VkFlags VkXcbSurfaceCreateFlagsKHR;

		//XShape
		typedef int32_t (*PFN_XShapeQueryExtension)(Display*, int32_t*, int32_t*);
		typedef int32_t(*PFN_XShapeQueryVersion)(Display* dpy, int32_t*, int32_t*);
		typedef void(*PFN_XShapeCombineRegion)(Display*, ::Window, int32_t, int32_t, int32_t, Region, int32_t);
		typedef void(*PFN_XShapeCombineMask)(Display*, XID, int32_t, int32_t, int32_t, Pixmap, int32_t);

		//XLib
		typedef XClassHint* (*PFN_XAllocClassHint)();
		typedef XSizeHints* (*PFN_XAllocSizeHints)();
		typedef XWMHints* (*PFN_XAllocWMHints)();
		typedef int (*PFN_XChangeProperty)(Display*, ::Window, Atom, Atom, int, int, const unsigned char*, int);
		typedef int (*PFN_XChangeWindowAttributes)(Display*, ::Window, unsigned long, XSetWindowAttributes*);
		typedef int (*PFN_XCheckIfEvent)(Display*, XEvent*, int(*)(Display*, XEvent*, XPointer), XPointer);
		typedef int (*PFN_XCheckTypedWindowEvent)(Display*, ::Window, int, XEvent*);
		typedef int (*PFN_XCloseDisplay)(Display*);
		typedef int32_t(*PFN_XCloseIM)(XIM);
		typedef int (*PFN_XConvertSelection)(Display*, Atom, Atom, Atom, ::Window, Time);
		typedef Colormap (*PFN_XCreateColormap)(Display*, ::Window, Visual*, int);
		typedef Cursor (*PFN_XCreateFontCursor)(Display*, unsigned int);
		typedef XIC (*PFN_XCreateIC)(XIM, ...);
		typedef ::Window (*PFN_XCreateWindow)(Display*, ::Window, int, int, unsigned int, unsigned int, unsigned int, int,
		                                      unsigned int, Visual*, unsigned long, XSetWindowAttributes*);
		typedef int (*PFN_XDefineCursor)(Display*, ::Window, Cursor);
		typedef int (*PFN_XDeleteContext)(Display*, XID, XContext);
		typedef int (*PFN_XDeleteProperty)(Display*, ::Window, Atom);
		typedef void (*PFN_XDestroyIC)(XIC);
		typedef int (*PFN_XDestroyWindow)(Display*, ::Window);
		typedef int (*PFN_XDisplayKeycodes)(Display*, int*, int*);
		typedef int (*PFN_XEventsQueued)(Display*, int);
		typedef int (*PFN_XFilterEvent)(XEvent*, ::Window);
		typedef int (*PFN_XFindContext)(Display*, XID, XContext, XPointer*);
		typedef int (*PFN_XFlush)(Display*);
		typedef int (*PFN_XFree)(void*);
		typedef int (*PFN_XFreeColormap)(Display*, Colormap);
		typedef int (*PFN_XFreeCursor)(Display*, Cursor);
		typedef void (*PFN_XFreeEventData)(Display*, XGenericEventCookie*);
		typedef char* (*PFN_XGetAtomName)(Display*, Atom);
		typedef int (*PFN_XGetErrorText)(Display*, int, char*, int);
		typedef int (*PFN_XGetEventData)(Display*, XGenericEventCookie*);
		typedef char* (*PFN_XGetICValues)(XIC, ...);
		typedef char* (*PFN_XGetIMValues)(XIM, ...);
		typedef int (*PFN_XGetInputFocus)(Display*, ::Window*, int*);
		typedef KeySym* (*PFN_XGetKeyboardMapping)(Display*, KeyCode, int, int*);
		typedef int (*PFN_XGetScreenSaver)(Display*, int*, int*, int*, int*);
		typedef ::Window (*PFN_XGetSelectionOwner)(Display*, Atom);
		typedef XVisualInfo* (*PFN_XGetVisualInfo)(Display*, long, XVisualInfo*, int*);
		typedef int32_t(*PFN_XGetWMNormalHints)(Display*, ::Window, XSizeHints*, long*);
		typedef int32_t(*PFN_XGetWindowAttributes)(Display*, ::Window, XWindowAttributes*);
		typedef int (*PFN_XGetWindowProperty)(Display*, ::Window, Atom, long, long, int, Atom, Atom*, int*, unsigned long*,
		                                      unsigned long*, unsigned char**);
		typedef int (*PFN_XGrabPointer)(Display*, ::Window, int, unsigned int, int, int, ::Window, Cursor, Time);
		typedef int32_t(*PFN_XIconifyWindow)(Display*, ::Window, int);
		typedef int32_t(*PFN_XInitThreads)();
		typedef Atom (*PFN_XInternAtom)(Display*, const char*, int);
		typedef int (*PFN_XLookupString)(XKeyEvent*, char*, int, KeySym*, XComposeStatus*);
		typedef int (*PFN_XMapRaised)(Display*, ::Window);
		typedef int (*PFN_XMapWindow)(Display*, ::Window);
		typedef int (*PFN_XMoveResizeWindow)(Display*, ::Window, int, int, unsigned int, unsigned int);
		typedef int (*PFN_XMoveWindow)(Display*, ::Window, int, int);
		typedef int (*PFN_XNextEvent)(Display*, XEvent*);
		typedef Display* (*PFN_XOpenDisplay)(const char*);
		typedef XIM (*PFN_XOpenIM)(Display*, XrmDatabase*, char*, char*);
		typedef int (*PFN_XPeekEvent)(Display*, XEvent*);
		typedef int (*PFN_XPending)(Display*);
		typedef int (*PFN_XQueryExtension)(Display*, const char*, int*, int*, int*);
		typedef int (*PFN_XQueryPointer)(Display*, ::Window, ::Window*, ::Window*, int*, int*, int*, int*, unsigned int*);
		typedef int (*PFN_XRaiseWindow)(Display*, ::Window);
		typedef int (*PFN_XRegisterIMInstantiateCallback)(Display*, void*, char*, char*, XIDProc, XPointer);
		typedef int (*PFN_XResizeWindow)(Display*, ::Window, unsigned int, unsigned int);
		typedef char* (*PFN_XResourceManagerString)(Display*);
		typedef int (*PFN_XSaveContext)(Display*, XID, XContext, const char*);
		typedef int (*PFN_XSelectInput)(Display*, ::Window, long);
		typedef int32_t(*PFN_XSendEvent)(Display*, ::Window, int, long, XEvent*);
		typedef int (*PFN_XSetClassHint)(Display*, ::Window, XClassHint*);
		typedef XErrorHandler (*PFN_XSetErrorHandler)(XErrorHandler);
		typedef void (*PFN_XSetICFocus)(XIC);
		typedef char* (*PFN_XSetIMValues)(XIM, ...);
		typedef int (*PFN_XSetInputFocus)(Display*, ::Window, int, Time);
		typedef char* (*PFN_XSetLocaleModifiers)(const char*);
		typedef int (*PFN_XSetScreenSaver)(Display*, int, int, int, int);
		typedef int (*PFN_XSetSelectionOwner)(Display*, Atom, ::Window, Time);
		typedef int (*PFN_XSetWMHints)(Display*, ::Window, XWMHints*);
		typedef void (*PFN_XSetWMNormalHints)(Display*, ::Window, XSizeHints*);
		typedef int32_t(*PFN_XSetWMProtocols)(Display*, ::Window, Atom*, int);
		typedef int (*PFN_XSupportsLocale)();
		typedef int (*PFN_XSync)(Display*, int);
		typedef int (*PFN_XTranslateCoordinates)(Display*, ::Window, ::Window, int, int, int*, int*, ::Window*);
		typedef int (*PFN_XUndefineCursor)(Display*, ::Window);
		typedef int (*PFN_XUngrabPointer)(Display*, Time);
		typedef int (*PFN_XUnmapWindow)(Display*, ::Window);
		typedef void (*PFN_XUnsetICFocus)(XIC);
		typedef VisualID (*PFN_XVisualIDFromVisual)(Visual*);
		typedef int (*PFN_XWarpPointer)(Display*, ::Window, ::Window, int, int, unsigned int, unsigned int, int, int);
		typedef Region(*PFN_XCreateRegion)();
		typedef int32_t (*PFN_XDestroyRegion)(Region);

		//XKB
		typedef XkbDescPtr (*PFN_XkbAllocKeyboard)();
		typedef void (*PFN_XkbFreeKeyboard)(XkbDescPtr, unsigned int, int);
		typedef void (*PFN_XkbFreeNames)(XkbDescPtr, unsigned int, int);
		typedef XkbDescPtr (*PFN_XkbGetMap)(Display*, unsigned int, unsigned int);
		typedef int32_t(*PFN_XkbGetNames)(Display*, unsigned int, XkbDescPtr);
		typedef int32_t(*PFN_XkbGetState)(Display*, unsigned int, XkbStatePtr);
		typedef KeySym (*PFN_XkbKeycodeToKeysym)(Display*, KeyCode, int, int);
		typedef int (*PFN_XkbQueryExtension)(Display*, int*, int*, int*, int*, int*);
		typedef int (*PFN_XkbSelectEventDetails)(Display*, unsigned int, unsigned int, unsigned long, unsigned long);
		typedef int (*PFN_XkbSetDetectableAutoRepeat)(Display*, int, int*);
		typedef int (*PFN_Xutf8LookupString)(XIC, XKeyPressedEvent*, char*, int, KeySym*, int32_t*);
		typedef void (*PFN_Xutf8SetWMProperties)(Display*, ::Window, const char*, const char*, char**, int, XSizeHints*,
		                                         XWMHints*, XClassHint*);

		//XRM
		typedef void (*PFN_XrmDestroyDatabase)(XrmDatabase);
		typedef int (*PFN_XrmGetResource)(XrmDatabase, const char*, const char*, char**, XrmValue*);
		typedef XrmDatabase (*PFN_XrmGetStringDatabase)(const char*);
		typedef void (*PFN_XrmInitialize)();
		typedef XrmQuark (*PFN_XrmUniqueQuark)();
		typedef int (*PFN_XUnregisterIMInstantiateCallback)(Display*, void*, char*, char*, XIDProc, XPointer);
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
			Invalid_Enum, //One of the arguments to the function was an invalid enum value.
			Invalid_Value, //One of the arguments to the function was an invalid value.
			Out_Of_Memory, //A memory allocation failed. A bug in the WindowingAPI or the underlying operating system.
			API_Unavailable, //The WindowingAPI could not find support for the requested API on the system.
			Platform_Error, //A platform-specific error occurred that does not match any of the more specific categories.
			Format_Unavailable, //The contents of the clipboard could no be converted to the requested format.
			Cursor_Unavailable //The requested cursor is not support on the underlying operating system.
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
			Normal,
			Hidden,
			Disabled,
			Captured
		};

		/// <summary>
		/// Cursor types that can be used.
		/// </summary>
		enum class CursorType
		{
			Arrow = 0,
			Input = 1,
			Crosshair = 2,
			PointingHand = 3,
			ResizeHorizontal = 4,
			ResizeVertical = 5,
			ResizeDiagonalTopLeftBottomRight = 6,
			ResizeDiagonalTopRightBottomLeft = 7,
			ResizeAll = 8,
			NotAllowed = 9
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
			//The window the cursor is captured in
			InternalWindow* CapturedCursorWindow = nullptr;

#ifdef TRAP_PLATFORM_WINDOWS
			std::array<Input::Key, 512> KeyCodes{};
			HWND HelperWindowHandle = nullptr;
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
#elif defined(TRAP_PLATFORM_LINUX)
			std::array<Input::Key, 256> KeyCodes{};
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
			RROutput Output = 0;
			RRCrtc CRTC = 0;
			RRMode OldMode = 0;

			//Index of corresponding Xinerama screen, for EWMH full screen window placement
			int32_t Index = 0;
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
			Cursor Handle = 0;
#endif
		};

		/// <summary>
		/// Window and Context structure.
		/// </summary>
		struct InternalWindow
		{
			//Window settings and state
			bool Resizable = true;
			bool Decorated = true;
			bool Floating = false;
			bool ShouldClose = false;
			bool Visible;
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
			std::array<bool, 8> MouseButtons{};
			std::array<bool, 349 + 1> Keys{};
			//Virtual Cursor position when Cursor is disabled
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
#elif defined(TRAP_PLATFORM_LINUX)
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
		/// Initializes the Windowing API.<br>
		/// Before most WindowingAPI functions can be used, the WindowingAPI must be initialized, and before the engine
		/// terminates the WindowingAPI should be terminated in order to free any resources allocated during or after
		/// initialization.<br>
		/// <br>
		/// If this function fails, it calls WindowingAPI::Shutdown before returning.<br>
		/// If it succeeds, WindowingAPI::Shutdown should be called before the engine exits.<br>
		/// <br>
		/// Additional calls to this function after successful initialization but before terminiation will return true
		/// immediately.<br>
		/// <br>
		/// Errors: Possible errors include Error::Platform_Error.<br>
		/// Remarks:<br>
		/// X11: This function will set the LC_CTYPE category of the engine locale according to the current environment
		///      if that category is still "C". This is because the "C" locale breaks Unicode text input.<br>
		/// Threaf safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>True if Windowing API was successfully initialized, false otherwise.</returns>
		static bool Init();
		/// <summary>
		/// This function destroys all remaining windows and cursor, and frees any other allocated resources.<br>
		/// Once this function is called, you must again class WindowingAPI::Init successfully before you
		/// will be able to use most WindowingAPI functions.<br>
		/// <br>
		/// If the WindowingAPI has been successfully initialized, this function should be called before the
		/// engine exits.<br>
		/// If initialization fails, there is no need to call this function, as it is called by WindowingAPI::Init
		/// before it returns failure.<br>
		/// This function has no effect if the WindowingAPI is not initialized.<br>
		/// <br>
		/// Possible errors include Error::Platform_Error.<br>
		/// <br>
		/// Remarks: This function may be called before WindowingAPI::Init.<br>
		/// Reentrancy: This function must not be called from a callback.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		static void Shutdown();
		/// <summary>
		/// This function destroys the specified window. On calling this function, no
		/// further callbacks will be called for that window.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Reentrancy: This function must not be called from a callback.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to destroy.</param>
		static void DestroyWindow(Scope<InternalWindow> window);
		/// <summary>
		/// Resets all window hints to their default values.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		static void DefaultWindowHints();
		/// <summary>
		/// This function sets hints for the next call to WindowingAPI::CreateWindow.
		/// The hints, once set, retain their values until changed by a call to this function
		/// or WindowingAPI::DefaultWindowHints, or until the engine is terminated.<br>
		/// <br>
		/// Some hints are platform specific. These may be set on any platform but they will only
		/// affect their specific platform. Other platforms will ignore them. Setting these hints
		/// requires no platform specific headers or functions.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="hint">Hint to set.</param>
		/// <param name="value">value to set for the Hint.</param>
		static void WindowHint(Hint hint, bool value);
		/// <summary>
		/// This function returns a human-readable name, encoded as UTF-8, of the specified monitor.
		/// The name typically reflects the make and model of the monitor and is not guaranteed to be
		/// unique among the connected monitors.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">InternalMonitor to query name from.</param>
		/// <returns>Name of the provided monitor.</returns>
		static const std::string& GetMonitorName(const InternalMonitor* monitor);
		/// <summary>
		/// This function returns the primary monitor. This is usually the monitor
		/// where elements like the task bar or global menu bar are located.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.<br>
		/// Remarks: The primary monitor is always first in the vector returned by WindowingAPI::GetMonitors.
		/// </summary>
		/// <returns>
		/// Primary InternalMonitor, or nullptr if no monitors were found or if an error occurred.
		/// </returns>
		static const Scope<InternalMonitor>& GetPrimaryMonitor();
		/// <summary>
		/// This function returns a vector of handles for all currently connceted
		/// monitors. The primary monitor is always first in the returned vector.
		/// If no monitors were found, this function returns an empty vector.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>
		/// Vector of all connected monitor handles, or an empty vector if no monitors were found or
		/// if an error occurred.
		/// </returns>
		static std::vector<InternalMonitor*> GetMonitors();
		/// <summary>
		/// This function returns the current video mode of the specified monitor.
		/// If you have create a full screen window for that monitor, the return
		/// value will depend on whether that window is minimized/iconified.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">InternalMonitor to get InternalVideoMode from.</param>
		/// <returns>
		/// Current video mode of the specified monitor, or an empty one if an error occurred.
		/// </returns>
		static const InternalVideoMode& GetVideoMode(InternalMonitor* monitor);
		/// <summary>
		/// This function returns a vector of all video modes supported by the specified monitor.
		/// The returned vector is sorted in ascending order, first by color bit depth
		/// (the sum of all channel depths) and then by resolution area (the product of
		/// width and height).<br>
		/// <br>
		/// Errors: Possible errors include: Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">InternalMonitor to get all InternalVideoModes from.</param>
		/// <returns>
		/// Vector with all available video modes of the specified monitor, or an empty vector
		/// if an error occurred.
		/// </returns>
		static std::vector<InternalVideoMode> GetVideoModes(InternalMonitor* monitor);
		/// <summary>
		/// This function creates a window.
		/// Most of the options controlling how the window should be created are specified with window hints.<br>
		/// <br>
		/// The created window and framebuffer may differ from what you requested, as not all parameters and
		/// hints are hard constraints. This includes the size of the window, especially for full screen windows.
		/// To query the actual attributes of the create window, see WindowingAPI::GetWindowHint,
		/// WindowingAPI::GetWindowSize and WindowingAPI::GetFramebufferSize.<br>
		/// <br>
		/// To create a full screen window, you need to specify the monitor the window will cover.
		/// If no monitor is specified, the window will be windowed mode. Unless you have a way
		/// for the user to choose a specific monitor, it is recommended that you pick the
		/// primary monitor.<br>
		/// <br>
		/// For full screen windows, the specified size becomes the resolution of the window's desired
		/// video mode. As long as a full screen window is not minimized/iconified, the supported
		/// video mode most closely matching the desired video mode is set for the specified monitor.<br>
		/// <br>
		/// Once you have create the window, you can switch it between windowed, borderless full screen (windowed)
		/// and full screen mode with WindowingAPI::SetWindowMonitor and WindowingAPI::SetWindowMonitorBorderless.<br>
		/// <br>
		/// By default, newly created windows use the placement recommended by the window system.
		/// To create the window at a specific position, make it initially invisible using the Hint::Visibile
		/// window hint, set its position and then show it.<br>
		/// <br>
		/// As long as at least one full screen window is not minimized/iconified, the screensaver is prohibited
		/// from starting.<br>
		/// <br>
		/// Window systems put limits on window sizes. Very large or very small window dimensions
		/// may be overriden by the window system on creation. Check the actual size after creation.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum, Error::Invalid_Value,
		///         Error::Format_Unavailable and Error::Platform_Error.<br>
		/// Remarks:<br>
		///     Windows: If the executable has an icon resource named 'TRAP_ICON', it will be set as the initial
		///              icon for the window. If no such icon is present, the IDI_APPLICATION icon will be used
		///              instead. To set a different icon, see WindowingAPI::SetWindowIcon.<br>
		///     X11: Some window managers will not respect the placement of initially hidden windows.<br>
		///          Due to the asynchronous nature of X11, it may take a moment for a window to reach its
		///          requested state. This means you may not be able to query the final size, position or
		///          other attributes directly after window creation.<br>
		///          The class part of the WM_CLASS window property will by default be set to the window
		///          title passed to this function. The instance part will use the contents of the
		///          RESOURCE_NAME environment variable, if present and not empty, or fall back to the
		///          window title.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="width">Desired width for the new Window. Must be greater than zero.</param>
		/// <param name="height">Desired height for the new Window. Must be greater than zero.</param>
		/// <param name="title">UTF-8 encoded title for the new Window.</param>
		/// <param name="monitor">Optional Monitor to use for the new Window.</param>
		/// <returns>On success a new InternalWindow, or nullptr if an error occurred.</returns>
		static Scope<InternalWindow> CreateWindow(uint32_t width, uint32_t height, const std::string& title,
		                                          InternalMonitor* monitor);
		/// <summary>
		/// This function sets the value of the close flag of the specified window. This can be used
		/// to override the user's attempt to close the window, or to signal that it should be closed.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function may be called from any thread. Access is not synchronized.
		/// </summary>
		/// <param name="window">InternalWindow whose flag to change.</param>
		/// <param name="value">Whether to close the Window or not.</param>
		static void SetWindowShouldClose(InternalWindow* window, bool value);
		/// <summary>
		/// This function sets the window title, encoded as UTF-8, of the specified window.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialize and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow whose title to change.</param>
		/// <param name="title">New UTF-8 encoded title for the Window.</param>
		static void SetWindowTitle(const InternalWindow* window, std::string& title);
		/// <summary>
		/// This function retrieves the content scale for the specified monitor.
		/// The content scale is the ratio between the current DPI and the platform's default DPI.
		/// This is especially important for text and any UI elements. If the pixel dimensions
		/// of your UI scaled by this look appropriate on your machine then it should appear
		/// at a reasonable size on other machines regardless of their DPI and scaling settings.
		/// This relies on the system DPI and scaling settings being somewhat correct.<br>
		/// <br>
		/// The content scale may depend on both the monitor resolution and pixel density and
		/// on user settings. It may be very different from the raw DPI calculated from the
		/// physical size and current resolution.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">InternalMonitor.</param>
		/// <param name="xScale">Output variable for the X scale of the provided monitor.</param>
		/// <param name="yScale">Output variable for the Y scale of the provided monitor.</param>
		static void GetMonitorContentScale(const InternalMonitor* monitor, float& xScale, float& yScale);
		/// <summary>
		/// This function destroys a cursor previously created with CreateCursor. Any remaining cursors
		/// will be destroyed by WindowingAPI::Shutdown.<br>
		/// <br>
		/// If the specified cursor is current for any window, that window will be reverted to the default
		/// cursor. This does not affect the cursor mode.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Reentrancy: This function must not be called from a callback.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="cursor">InternalCursor to be destroyed.</param>
		static void DestroyCursor(Scope<InternalCursor> cursor);
		/// <summary>
		/// Creates a new custom cursor image that can be set for a window with SetCursor. The cursor can
		/// be destroyed with DestroyCursor. Any remaining cursors are destroyed by WindowingAPI::Shutdown.<br>
		/// <br>
		/// The cursor hotspot is specified in pixels, relative to the upper-left corner of the cursor image.
		/// Like all other coordinate system in the WindowingAPI, the X-axis points to the right and the
		/// Y-axis points down.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be caled from the main thread.
		/// </summary>
		/// <param name="image">Non HDR RGB 24BPP or RGBA 32 BPP Image.</param>
		/// <param name="xHotspot">Center X coordinate of the Image.</param>
		/// <param name="yHotspot">Center Y coordinate of the Image.</param>
		/// <returns>On success new InternalCursor, nullptr otherwise.</returns>
		static Scope<InternalCursor> CreateCursor(const Scope<Image>& image, int32_t xHotspot, int32_t yHotspot);
		/// <summary>
		/// Creates a cursor with a standard shape.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum and Error::Platform_Error.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="type">CursorType to get.</param>
		/// <returns>On success new InternalCursor, nullptr otherwise.</returns>
		static Scope<InternalCursor> CreateStandardCursor(const CursorType& type);
		/// <summary>
		/// This function sets the cursor image to be used when the cursor is over the content are of the
		/// specified window. The set cursor will only be visible when the CursorMode of the window is
		/// CursorMode::Normal.<br>
		/// <br>
		/// On some platforms, the set cursor may not be visible unless the window also has input focus.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the cursor for.</param>
		/// <param name="cursor">
		/// InternalCursor to set, or nullptr to switch back to the default arrow cursor.
		/// </param>
		static void SetCursor(InternalWindow* window, InternalCursor* cursor);
		/// <summary>
		/// This function sets the icon of the specified window. If no image is specified, the window
		/// reverts to its default icon.<br>
		/// <br>
		/// The image mustbe RGB 24BPP or RGBA 32BPP.<br>
		/// <br>
		/// The desired image size varies depending on platform and system settings.
		/// Good sizes include 16x16, 32x32 and 48x48.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow whose icon to set.</param>
		/// <param name="image">
		/// Image to be set as Window Icon or nullptr to revert back to the default icon.
		/// </param>
		static void SetWindowIcon(InternalWindow* window, const Scope<Image>& image);
		/// <summary>
		/// This function sets the position, in screen coordinates, of the upper-left corner of the
		/// content area of the specifed windowed mode window. If the window is a full screen window,
		/// this function does nothing.<br>
		/// <br>
		/// DO NOT USE THIS FUNCTION to move an already visible window unless you have very
		/// good reasons for doing so, as it will confuse and annoy the user.<br>
		/// <br>
		/// The window manager may put limits on what positions are allowed. The WindowingAPI
		/// cannot and should not override these limits.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the position for.</param>
		/// <param name="xPos">X position to be set.</param>
		/// <param name="yPos">Y position to be set.</param>
		static void SetWindowPos(const InternalWindow* window, int32_t xPos, int32_t yPos);
		/// <summary>
		/// This function retrieves the position, in screen coordinates, of the upper-left corner of the
		/// content area of the specified window.<br>
		/// <br>
		/// If an error occurs, xPos and yPos will be set to zero.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to get the current position from.</param>
		/// <param name="xPos">Output variable for the current X position of the InternalWindow.</param>
		/// <param name="yPos">Output variable for the current Y position of the InternalWindow.</param>
		static void GetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos);
		/// <summary>
		/// This function sets the size, in screen coordinates, of the content area of the specified window.<br>
		/// <br>
		/// For full screen windows, this function updates the resolution of its desired video mode
		/// and switches to the video mdoe closest to it.<br>
		/// <br>
		/// If you wish to update the refresh rate of the desired video mode in addition
		/// to its resolution, see WindowingAPI::SetWindowMonitor.<br>
		/// <br>
		/// The window manager may put limits on what sizes are allowed. The WindowingAPI
		/// cannot and should not override these limits.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the size for.</param>
		/// <param name="width">New width for the InternalWindow.</param>
		/// <param name="height">New height for the InternalWindow.</param>
		static void SetWindowSize(InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// This function retrieves the size, in screen coordinates, of the content area of
		/// the specified window. If you wish to retrieve the size of the framebuffer of the window
		/// in pixels, see WindowingAPI::GetFrameBufferSize.<br>
		/// <br>
		/// If an error occurs, width and height will be set to zero.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to get the size from.</param>
		/// <param name="width">Output variable for the InternalWindows current width.</param>
		/// <param name="height">Output variable for the InternalWindows current height.</param>
		static void GetWindowSize(const InternalWindow* window, int32_t& width, int32_t& height);
		/// <summary>
		/// This function retrieves the size, in pixels, of the framebuffer of the specified window.
		/// If you wish to retrieve the size of the window in screen coordinates, see
		/// WindowingAPI::GetWindowSize.<br>
		/// <br>
		/// If an error occcurs, width and height will be set to zero.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to get the framebuffer size from.</param>
		/// <param name="width">Output variable for the InternalWindows current framebuffer width.</param>
		/// <param name="height">Output variable for the InternalWindows current framebuffer height.</param>
		static void GetFrameBufferSize(const InternalWindow* window, int32_t& width, int32_t& height);
		/// <summary>
		/// This function sets the opacity of the window, including any decorations.<br>
		/// <br>
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque.<br>
		/// <br>
		/// The initial opacity value for newly created windows is one.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set opacity for.</param>
		/// <param name="opacity">Opacity ranging from 0.0f-1.0f.</param>
		static void SetWindowOpacity(const InternalWindow* window, float opacity);
		/// <summary>
		/// This function returns the opacity of the window, including any decorations.<br>
		/// <br>
		/// The opacity (or alpha) value is a positive finite number between zero and one,
		/// where zero is fully transparent and one is fully opaque. If the system does not
		/// support whole window transparency, this function always returns one.<br>
		/// <br>
		/// The initial opacity value for newly created windows is one.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to get the opacity from.</param>
		/// <returns>Opacity of the given InternalWindow.</returns>
		static float GetWindowOpacity(const InternalWindow* window);
		/// <summary>
		/// This function retrieves the content scale for the specified window.
		/// The content scale is the reatio between the current DPI and the platform's
		/// default DPI. This is especially important for text and any UI elements.
		/// If the pixel dimensions of your UI scaled by this look appropriate
		/// on your machine then it should appear at a reasonable size on other
		/// machines regardless of their DPI and scaling settings. This relies
		/// on the system DPI and scaling settings being somewhat correct.<br>
		/// <br>
		/// On systems where each monitor can have its own content scale, the window
		/// content scale will depend on which monitor the system considers the window
		/// to be on.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to get the content scale from.</param>
		/// <param name="xScale">Output variable for the InternalWindows content scale X.</param>
		/// <param name="yScale">Output variable for the InternalWindows content scale Y.</param>
		static void GetWindowContentScale(const InternalWindow* window, float& xScale, float& yScale);
		/// <summary>
		/// This function sets the value of a hint of the specified window.<br>
		/// <br>
		/// Some of these hints are ignored for full screen windows. The new value
		/// will take effect if the window is later made windowed.<br>
		/// <br>
		/// Some of these hints are ignored for windowed mode windows. The new value
		/// will take ffect if the window is later made full screen.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invaild_Enum,
		///         Error::Invalid_Value and Error::Platform_Error.<br>
		/// Remarks: Calling WindowingAPI::GetWindowHint will always return the latest value,
		///          even if that value is ignored by the current mode of the window.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the Hint for.</param>
		/// <param name="hint">Hint to be set.</param>
		/// <param name="value">Value for the Hint.</param>
		static void SetWindowHint(InternalWindow* window, Hint hint, bool value);
		/// <summary>
		/// This function returns the value of a hint of the specified window.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Enum
		/// and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to get the Hint from.</param>
		/// <param name="hint">Hint to query.</param>
		/// <returns>True if Hint is enabled, false otherwise.</returns>
		static bool GetWindowHint(const InternalWindow* window, Hint hint);
		/// <summary>
		/// This function sets the monitor that the window uses for full screen mode or,
		/// if the monitor is nullptr, makes it windowed mode.<br>
		/// <br>
		/// When setting a monitor, this function updates the width, height and
		/// refresh rate of the desired video mode and switches to the video mode
		/// closest to it. The window position is ignored when setting a monitor.<br>
		/// <br>
		/// When the monitor is nullptr, the position, width and height are used to place the
		/// window content area. The refresh rate is ignored when no monitor is specified.<br>
		/// <br>
		/// If you only wish to update the resolution of a full screen window
		/// or the size of a windowed mode window, see WindowingAPI::SetWindowSize.<br>
		/// <br>
		/// When a window transitions from full screen to windowed mode, this function
		/// restores any previous window settings such as whether it is decorated,
		/// floating, resizable, has size limits, etc.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the monitor for.</param>
		/// <param name="monitor">InternalMonitor to use or nullptr.</param>
		/// <param name="xPos">New X position for the window.</param>
		/// <param name="yPos">New Y position for the window.</param>
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
		/// This function sets the monitor that the window uses for borderless full screen mode.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the monitor for.</param>
		/// <param name="monitor">InternalMonitor to use.</param>
		static void SetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor);
		/// <summary>
		/// This function sets the user-define dpointer of the specified window.
		/// The current value is retained until the window is destroyed.
		/// The initial value is nullptr.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function may be called from any thread. Access is not synchronized.
		/// </summary>
		/// <param name="window">InternalWindow to set the user pointer for.</param>
		/// <param name="pointer">Data to be stored with the window.</param>
		static void SetWindowUserPointer(InternalWindow* window, void* pointer);
		/// <summary>
		/// This function returns the current value of the user-defined pointer
		/// of the specified window. The initial value is nullptr.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function may be called from any thread. Access is not synchronized.
		/// </summary>
		/// <param name="window">InternalWindow to get the user pointer from.</param>
		/// <returns>Data stored with the window.</returns>
		static void* GetWindowUserPointer(const InternalWindow* window);
		/// <summary>
		/// This function sets the monitor configuration callback, or removes
		/// the currently set callback. This is called when a monitor is
		/// connected to or disconnected from the system.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="callback">
		/// Function to call when a monitor event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetMonitorCallback(MonitorFunc callback);
		/// <summary>
		/// This function sets the position callback of the specified window, which is called
		/// when the window is moved. The callback is provided with the position, in screen
		/// coordinates, of the upper-left corner of the content area of the window.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a position event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowPosCallback(InternalWindow* window, WindowPositionFunc callback);
		/// <summary>
		/// This function sets the size callback of the specified window, which is
		/// called when the window is resized. The callback is provided with the
		/// size, in screen coordinates, of the content area of the window.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a size event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowSizeCallback(InternalWindow* window, WindowSizeFunc callback);
		/// <summary>
		/// This function sets the minimization/iconification callback of the specified window, which
		/// is called when the window is minimized/iconified or restored.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a minimize event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowMinimizeCallback(InternalWindow* window, WindowMinimizeFunc callback);
		/// <summary>
		/// This function sets the maximization callback of the specified window, which
		/// is called when the window is maximized or restored.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a maximize event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowMaximizeCallback(InternalWindow* window, WindowMaximizeFunc callback);
		/// <summary>
		/// This function sets the close callback of the specified window, which is called
		/// when the user attempts to close the window, for example by clicking the close widget
		/// in the title bar.<br>
		/// <br>
		/// The close flags is set before this callback is called, but you can modify it at
		/// any time with WindowingAPI::SetWindowShouldClose.<br>
		/// <br>
		/// The close callback is not triggered by WindowingAPI::DestroyWindow.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a close event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowCloseCallback(InternalWindow* window, WindowCloseFunc callback);
		/// <summary>
		/// This function sets the focus callback of the specified window, which is called
		/// when the window gains or loses input focus.<br>
		/// <br>
		/// After the focus callback is called for a window that lost input focus, synthetic
		/// key and mouse button release events will be generated for all such that had been
		/// pressed. For more information, see WindowingAPI::SetKeyCallback and
		/// WindowingAPI::SetMouseButtonCallback.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a focus event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetWindowFocusCallback(InternalWindow* window, WindowFocusFunc callback);
		/// <summary>
		/// This function sets the framebuffer resize callback of the specified window, which is
		/// called when the framebuffer of the specified window is resized.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		// Function to call when a framebuffer size event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetFrameBufferSizeCallback(InternalWindow* window, FrameBufferSizeFunc callback);
		/// <summary>
		/// This function sets the window content scale callback of the specified window, which is
		/// called when the content scale of the specified window changes.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a content scale event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetContentScaleCallback(InternalWindow* window, WindowContentScaleFunc callback);
		/// <summary>
		/// This function sets the key callback of the specified window, which is called when
		/// a key is pressed, repeated or released.<br>
		/// <br>
		/// The key functions deal with physical keys, with layout independent key tokens named
		/// after their values in the standard US keyboard layout. If oyu want to input text, use
		/// the character callback instead.<br>
		/// <br>
		/// When a window loses input focus, it will generate synthetic key release events for
		/// all pressed keys. You can tell these events from user-generated events by the fact
		/// that the synthetic ones are generated after the focus loss event has been processed,
		/// i.e. after the window focus callback has been called.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a key event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetKeyCallback(InternalWindow* window, KeyFunc callback);
		/// <summary>
		/// This function sets the character callback of the specified window, which is called
		/// when a Unicode character is input.<br>
		/// <br>
		/// The character callback is intended for Unicode text input. As it deals with characters,
		/// it is keyboard layout dependent. whereas the key callback is not. Characters
		/// do not map 1:1 to physical keys, as a key may produce zero, one or more character.
		/// If you want to know whether a specific physical key was pressed or released, see
		/// the key callback instead.<br>
		/// <br>
		/// The character callback behaves as system text input normally does and will not be called
		/// if modifier keys are held down that would prevent normal text input on that platform,
		/// for example Alt key on Windows.<br>
		/// <br>
		/// Erros: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a char event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetCharCallback(InternalWindow* window, CharFunc callback);
		/// <summary>
		/// This function sets the mouse button callback of the specified window, which is
		/// called when amouse button is pressed or released.<br>
		/// <br>
		/// When a window loses input focus, it will generate synthetic mouse button release
		/// events for all pressed mouse buttons. You cann tell these events from user-generated
		/// events by the fact that the synthetic ones are generated after the focus loss event
		/// has been processed, i.e. after the window focus callback has been called.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a mouse button event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetMouseButtonCallback(InternalWindow* window, MouseButtonFunc callback);
		/// <summary>
		/// This function sets the cursor position callback of the specified window, which is
		/// called when the cursor is moved. The callback is provided with the position,
		/// in screen coordinates, relative to the upper-left corner of the content area
		/// of the window.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a cursor position event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetCursorPosCallback(InternalWindow* window, CursorPositionFunc callback);
		/// <summary>
		/// This function sets the cursor boundary crossing callback of the specified window, which
		/// is called when the cursor enters or leaves the content area of the window.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This fucntion must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a cursor enter event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetCursorEnterCallback(InternalWindow* window, CursorEnterFunc callback);
		/// <summary>
		/// This function sets the scroll callback of the specified window, which is called when a
		/// scrolling device is used, such as a mouse wheel or scrolling area of a touchpad.<br>
		/// <br>
		/// The scroll callback receives all scrolling input, like that from a mouse wheel or a touchpad
		/// scrolling area.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
		/// <param name="callback">
		/// Function to call when a scroll event occurs, or nullptr to remove the currently set callback.
		/// </param>
		static void SetScrollCallback(InternalWindow* window, ScrollFunc callback);
		/// <summary>
		/// This function sets the path drop callback of the specified window, which is
		/// called when one or more dragged paths are dropped on the window.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the callback for.</param>
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
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a position event occurs.</returns>
		static WindowPositionFunc GetWindowPosCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the size callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a size event occurs.</returns>
		static WindowSizeFunc GetWindowSizeCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the close callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a close event occurs.</returns>
		static WindowCloseFunc GetWindowCloseCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the focus callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a focus event occurs.</returns>
		static WindowFocusFunc GetWindowFocusCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the framebuffer resize callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a framebuffer size event occurs.</returns>
		static FrameBufferSizeFunc GetFrameBufferSizeCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the window content scale callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a content scale event occurs.</returns>
		static WindowContentScaleFunc GetWindowContentScaleCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the key callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a key event occurs.</returns>
		static KeyFunc GetKeyCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the Unicode character callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a char event occurs.</returns>
		static CharFunc GetCharCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the mouse button callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a mouse button event occurs.</returns>
		static MouseButtonFunc GetMouseButtonCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the cursor position callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a cursor position event occurs.</returns>
		static CursorPositionFunc GetCursorPosCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the cursor enter callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a cursor enter event occurs.</returns>
		static CursorEnterFunc GetCursorEnterCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the scroll callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a scroll event occurs.</returns>
		static ScrollFunc GetScrollCallback(const InternalWindow* window);
		/// <summary>
		/// Gets the path drop callback for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow from which to query the callback from.</param>
		/// <returns>Function that gets called when a drop event occurs.</returns>
		static DropFunc GetDropCallback(const InternalWindow* window);
		/// <summary>
		/// This function processes only those events that are already in the event queue and then
		/// returns immediately. Processing events will cause the window and input callbacks
		/// associated with those events to be called.<br>
		/// <br>
		/// On some platforms, a window move, resize or menu operation will cause event processing
		/// to block. This is due to how event processing is designed on those platforms.<br>
		/// <br>
		/// Do not assume that callbacks you set will only be called in response to event
		/// processing functions like this one. While it is necessary to poll for events, window
		/// systems that require the WindowingAPI to register callbacks of its own can pass evnts
		/// to the WindowingAPI in response to many window system function calls. The WindowingAPI
		/// will pass those events on to the application callbacks before returning.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Reentrancy: This function must not be called from a callback.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		static void PollEvents();
		/// <summary>
		/// Sets the cursor mode for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow to set the cursor mode for.</param>
		/// <param name="mode">CursorMode to be set.</param>
		static void SetCursorMode(InternalWindow* window, CursorMode mode);
		/// <summary>
		/// Retrieves the cursor mode from the specified window.
		/// </summary>
		/// <param name="window">InternalWindow to query the cursor mode from.</param>
		/// <returns>CursorMode used by the InternalWindow.</returns>
		static CursorMode GetCursorMode(const InternalWindow* window);
		/// <summary>
		/// This function returns whether raw mouse motion is supported on the current system.<br>
		/// This status does not change after the WindowingAPI has been initialized so you
		/// only need to check this once. If you attemp to enable raw motion on a system
		/// that does not support it, Error::Platform_Error will be emitted.<br>
		/// <br>
		/// Raw mouse motion is closer to the actual motion of the mouse across a surface.
		/// It is not affected by the scaling and acceleration applied to the motion of
		/// the desktop cursor. That processing is suitable for a cursor while raw motion
		/// is better for controlling for example a 3D camera. Because of this, raw mouse
		/// motion is only provided when the cursor is disabled.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>True if raw mouse input is supported, false otherwise.</returns>
		static bool RawMouseMotionSupported();
		/// <summary>
		/// Sets the raw mouse motion mode for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow to set raw mouse input for.</param>
		/// <param name="enabled">Whether to enable or disable raw mouse input.</param>
		static void SetRawMouseMotionMode(InternalWindow* window, bool enabled);
		/// <summary>
		/// Retrieves the raw mouse motion mode for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow to query.</param>
		/// <returns>True if raw mouse motion mode is enabled, false otherwise.</returns>
		static bool GetRawMouseMotionMode(const InternalWindow* window);
		/// <summary>
		/// This function returns the name of the specified printable key, encoded as UTF-8.
		/// This is typically the character that key would produce without any modifier
		/// keys, intended for displaying key bindings to the user. For dead keys, it is
		/// typically the diacritic it would add to a character.<br>
		/// <br>
		/// DO NOT USE THIS FUNCTION for text input. You will break text input for many
		/// languages even if it happens to work for yours.<br>
		/// <br>
		/// If the key is Input::Key::Unknown, the scancode is used to identify the key,
		/// otherwise the scancode is ignored. If you specify a non-printable key, or
		/// Input::Key::Unknown and a scancode that maps to a non-printable key, this
		/// function return nullptr but does not emit an error.<br>
		/// <br>
		/// This behaviour allows you to always pass in the arguments in the key callback
		/// without modification.<br>
		/// <br>
		/// The printable keys are:<br>
		/// Input::Key::Apostrophe<br>
		/// Input::Key::Comma<br>
		/// Input::Key::Minus<br>
		/// Input::Key::Period<br>
		/// Input::Key::Slash<br>
		/// Input::Key::Semicolon<br>
		/// Input::Key::Equal<br>
		/// Input::Key::Left_Bracket<br>
		/// Input::Key::Right_Bracket<br>
		/// Input::Key::Backslash<br>
		/// Input::Key::World_1<br>
		/// Input::Key::World_2<br>
		/// Input::Key::Zero to Input::Key::Nine<br>
		/// Input::Key::A to Input::Key::Z<br>
		/// Input::Key::KP_0 to Input::Key::KP_9<br>
		/// Input::Key::KP_Decimal<br>
		/// Input::Key::KP_Divide<br>
		/// Input::Key::KP_Multiply<br>
		/// Input::Key::KP_Subtract<br>
		/// Input::Key::KP_Add<br>
		/// Input::Key::KP_Equal<br>
		/// <br>
		/// Names for printable keys depend on keyboard layout, while names for non-printable keys
		/// are the same across layouts but depend on the application language and should be localized
		/// along with other user interface text.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Remarks: The contents of the returned string may change when a keyboard layout change event is received.
		/// Pointer lifetime: The returned string is allocated and freed by the WindowingAPI.
		///                   You should not free it yourself.
		///                   It is valid until the library is terminated.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="key">Key to get layout-specific name from.</param>
		/// <param name="scanCode">Optional scan code to get layout-specific name from.</param>
		/// <returns>UTF-8 encoded, layout-specific name of the given key or nullptr.</returns>
		static const char* GetKeyName(Input::Key key, int32_t scanCode);
		/// <summary>
		/// This function returns the last state reported for the specified key to the
		/// specified window. The returned state is pressed (true) or released (false).<br>
		/// <br>
		/// The key functions deal with physical keys, with key token named after their use
		/// on the standard US keyboard layout. If you want to input text, use the Unicode
		/// character callback instead.<br>
		/// <br>
		/// DO NOT USE THIS FUNCTION to implement text input.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to query.</param>
		/// <param name="key">Key to get last reported state from.</param>
		/// <returns>Last reported state of the specified key.</returns>
		static bool GetKey(const InternalWindow* window, Input::Key key);
		/// <summary>
		/// This function returns the last state reported for the specified mouse button
		/// to the specified window. The returned state is pressed (true) or released (false).<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Invalid_Enum.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to query.</param>
		/// <param name="button">Mouse button to get last reported state from.</param>
		/// <returns>Last reported state of the specified mouse button.</returns>
		static bool GetMouseButton(const InternalWindow* window, Input::MouseButton button);
		/// <summary>
		/// This function sets the position, in screen coordinates, of the cursor relative to the
		/// upper-left corner of the content area of the specified window. The window must have
		/// input focus. If the window does not have input focus when this function is called,
		/// it fails silently.<br>
		/// <br>
		/// DO NOT USE THIS FUNCTION to implement things like camera controls. The WindowingAPI already
		/// provides the CursorMode::Disabled that hides the cursor, transparently re-centers it and
		/// provides unconstrained cursor motion. See SetCursorMode for more information.<br>
		/// <br>
		/// If the cursor mode is CursorMode::Disabled then the cursor position is unconstrained and
		/// limited only by the minimum and maximum values of a double.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set the cursor position for.</param>
		/// <param name="xPos">New X position, relative to the left edge of the content area, for the cursor.</param>
		/// <param name="yPos">New Y position, relative to the top edge of the content area, for the cursor.</param>
		static void SetCursorPos(InternalWindow* window, double xPos, double yPos);
		/// <summary>
		/// This function returns the position of the cursor, in screen coordinates, relative
		/// to the upper-left corner of the content area of the specified window.<br>
		/// <br>
		/// If the cursor is disabled (with CursorMode::Disabled) then the cursor position is unbounded and limited
		/// only by the minimum and maximum values of a double.<br>
		/// <br>
		/// The coordinate can be converted to their integer equivalents with the TRAP::Math::Floor function.
		/// Casting directly to an integer type works for positive coordinates, but fails for negative ones.<br>
		/// <br>
		/// If an error occurs, xPos and yPos will be set to zero.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow from which to get the current cursor position.</param>
		/// <param name="xPos">
		/// Output variable for the current X position, relative to the left edge of the content area, of the cursor.
		/// </param>
		/// <param name="yPos">
		/// Output variable for the current Y position, relative to the top edge of the content area, of the cursor.
		/// </param>
		static void GetCursorPos(const InternalWindow* window, double& xPos, double& yPos);
		/// <summary>
		/// Returns the position of the monitor's viewport on the virtual screen.


		/// This function returns the position, in screen coordinates, of the upper-left corner
		/// of the specified monitor.<br>
		/// <br
		/// If an error occurs, xPos and yPos will be set to zero.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">InternalMonitor to query.</param>
		/// <param name="xPos">Output variable for the X position of the monitor.</param>
		/// <param name="yPos">Output variable for the Y position of the monitor.</param>
		static void GetMonitorPos(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos);
		/// <summary>
		/// This function returns the position, in screen coordinates, of the upper-left corner
		/// of the work area of the specified monitor along with the work area size in screen
		/// coordinates. The work area is defined as the area of the monitor not occluded by
		/// the operating system task bar where present. If no task bar exists then the work
		/// area is the monitor resolution in screen coordinates.<br>
		/// <br>
		/// If an error occurs, xPos, yPos, width and height will be set to zero.<br>
		/// <br>
		/// Errors: Possible errors include: Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="monitor">InternalMonitor to query.</param>
		/// <param name="xPos">Output variable for the X position of the monitor.</param>
		/// <param name="yPos">Output variable for the Y position of the monitor.</param>
		/// <param name="width">Output variable for the width of the monitor.</param>
		/// <param name="height">Output variable for the height of the monitor.</param>
		static void GetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos,
		                               int32_t& width, int32_t& height);
		/// <summary>
		/// This function makes the specified window visible if it was previously
		/// hidden. If the window is already visible or is in full screen mode,
		/// this function does nothing.<br>
		/// <br>
		/// By default, windowed mode window are focused when shown. Set the Hint::FocusOnShow
		/// window hint to change this behaviour for all newly created windows, or change
		/// the behaviour for and existing window with WindowingAPI::SetWindowHint.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to display.</param>
		static void ShowWindow(InternalWindow* window);
		/// <summary>
		/// This function brings the specified window to front and sets input focus.
		/// The window should already be visible and not minimized/iconified.<br>
		/// <br>
		/// By default, both windowed and full screen mode windows are focused
		/// when initially created. Set the Hint::Focused hint to disable
		/// this behaviour.<br>
		/// <br>
		/// Also by default windowed mode windows are focused when shown
		/// with WindowingAPI::ShowWindow. Set the Hint::FocusOnShow hint to
		/// disable this behaviour.<br>
		/// <br>
		/// DO NOT USE THIS FUNCTION to steal focus from other applications
		/// unless you are certain that is what the user wants. Focus
		/// stealing can be extremely disruptive.<br>
		/// <br>
		/// For a less disruptive way of gettings the user's attention, see
		/// WindowingAPI::RequestWindowAttention.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to focus.</param>
		static void FocusWindow(const InternalWindow* window);
		/// <summary>
		/// This function maximizes the specified window if it was previously not
		/// maximized. If the window is already maximized, this function does nothing.<br>
		/// <br>
		/// If the specified window is a full screen window, this function does nothing.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function may only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to maximize.</param>
		static void MaximizeWindow(const InternalWindow* window);
		/// <summary>
		/// This function minimizes (iconifies) the specified window if it was previously
		/// restored. If the window is already minimized (iconified), this function does
		/// nothing.<br>
		/// <br>
		/// If the specified window is a full screen window, the original monitor resolution
		/// is restored util the window is restored.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to minimize/iconify.</param>
		static void MinimizeWindow(const InternalWindow* window);
		/// <summary>
		/// This function requests user attention to the specified window.
		/// On platforms where this is not supported, attention is requested
		/// to the application as a whole.<br>
		/// <br>
		/// Once the user has given attention, usually by focusing the window
		/// or application, the system will end the request automatically.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to request user attention for.</param>
		static void RequestWindowAttention(const InternalWindow* window);
		/// <summary>
		/// This function hides the specified window if it was previously visible. If
		/// the window is already hidden or is in full screen mode, this function
		/// does nothing.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to hide.</param>
		static void HideWindow(const InternalWindow* window);
		/// <summary>
		/// Restores the specified window.

		/// This function restores the specified window if it was previously
		/// minimized (iconified) or maximized. If the window is already restored,
		/// this function does nothing.<br>
		/// <br>
		/// If the specified window is a full screen window, the resolution chosen
		/// for the window is restored on the selected monitor.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to be restored.</param>
		static void RestoreWindow(InternalWindow* window);
		/// <summary>
		/// Sets the size limits of the specified window.<br>
		/// <br>
		/// Wayland: The size limits will not be applied until the window is actually resized, either by the
		///          user or by the compositor.

		/// This function sets the size limits of the content area of the specified window. If the
		/// window is full screen, the size limits only take effect once it is made windowed.
		/// If the window is not resizable, this function does nothing.<br>
		/// <br>
		/// The size limits are applied immediately to a windowed mode window and may cause it
		/// to be resized.<br>
		/// <br>
		/// The maximum dimensions must be greater than or equal to the minimum dimensions
		/// and all must be greater than zero.<br>
		/// <br>
		/// Use -1 to disable to minimum and/or maximum size constraints.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized, Error::Invalid_Value and
		/// Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="window">InternalWindow to set size limits for.</param>
		/// <param name="minWidth">New minimum window width.</param>
		/// <param name="minHeight">New minimum window height.</param>
		/// <param name="maxWidth">New maximum window width.</param>
		/// <param name="maxHeight">New maximum window height.</param>
		static void SetWindowSizeLimits(InternalWindow* window, int32_t minWidth, int32_t minHeight,
		                                int32_t maxWidth, int32_t maxHeight);
		/// <summary>
		/// This function sets the system clipboard to the specified, UTF-8 encoded string.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.
		/// Pointer lifetime: The specified string is copied before this function returns.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <param name="string">UTF-8 encoded string to be set for the clipboard.</param>
		static void SetClipboardString(std::string_view string);
		/// <summary>
		/// This function returns the contents of the system clipboard, if it contains or
		/// is convertible to a UTF-8 encoded string. If the clipboard is empty or if its
		/// content cannot be converted, an empty string is returned and a Error::Format_Unavailable
		/// error is generated.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::Platform_Error.<br>
		/// Thread safety: This function must only be called from the main thread.
		/// </summary>
		/// <returns>
		/// UTF-8 encoded string containing clipboard contents, or empty string if an error occurred.
		/// </returns>
		static std::string GetClipboardString();
		/// <summary>
		/// This function returns whether the Vulkan loader and any minimally function ICD
		/// have been found.<br>
		/// <br>
		/// The availability of a Vulkan loader and even an ICD does not by itself guarantee
		/// that surface creation or even instance creation is possible. For example, on
		/// Fermi systems NVIDIA will install an ICD that provides no actual Vulkan support.
		/// Call WindowingAPI::GetRequiredInstanceExtensions to check whether the extensions
		/// necessary for Vulkan surface creation are available.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized.<br>
		/// Thread safety: This function may be called from any thread.
		/// </summary>
		/// <returns>True if Vulkan API is minimally supported, false otherwise.</returns>
		static bool VulkanSupported();
		/// <summary>
		/// This function returns an array of names of Vulkan instance extensions required
		/// by the WindowingAPI for creating Vulkan surface for WindowingAPI windows.
		/// If successful, the list will always contain VK_KHR_surface, so if you don't
		/// require any additional extensions you can pass this list directly to the
		/// VkInstanceCreateInfo struct.<br>
		/// <br>
		/// If Vulkan is not available on the machine, this function return an array with
		/// empty strings and generates a Error::API_Unavailable error.
		/// Call WindowingAPI::VulkanSupported to check whether Vulkan is at least
		/// minimally available.<br>
		/// <br>
		/// If Vulkan is available but no set of extensions allowing window surface creation
		/// was found, this function returns an array with empty strings. You may still use
		/// Vulkan for off-screen rendering and compute work.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized and Error::API_Unavailable.<br>
		/// Remarks: Additional extensions may be required by future versions of the WindowingAPI.
		///          You should check if any extensions you wish to enable are already in the
		///          returned array, as it is an error to specify an extension more than once
		///          in the VkInstanceCreateInfo struct.<br>
		/// Thread safety: This function may be called from any thread.
		/// </summary>
		/// <returns>
		/// Array containing the required instance extensions, or an array with empty string
		/// if an error occurred.
		/// </returns>
		static std::array<std::string, 2> GetRequiredInstanceExtensions();
		/// <summary>
		/// Creates a Vulkan surface for the specified window.


		/// This function create a Vulkan surface for the specified window.<br>
		/// <br>
		/// If the Vulkan loader or at least one minimally functiona ICD were not found,
		/// this function return VK_ERROR_INITIALIZATION_FAILED and generates a Error::API_Unavilable error.
		/// Call WindowingAPI::VulkanSupported to check whether Vulkan is at least minimally available.<br>
		/// <br>
		/// If the required window surface creation instance extensions are not available or if the
		/// specified instance was not created with these extensions enabled, this function
		/// returns VK_ERROR_EXTENSION_NOT_PRESENT and generates a Error::API_Unavailable error.
		/// Call WindowingAPI::GetRequiredInstanceExtensions to check what instance extensions
		/// are required.<br>
		/// <br>
		/// The window surface must be destroyed before the specified Vulkan instance.
		/// It is the responsibility of the caller to destroy the window surface.
		/// The WindowingAPI does not destroy it for you.
		/// Call vkDestroySurfaceKHR to destroy the surface.<br>
		/// <br>
		/// Errors: Possible errors include Error::Not_Initialized, Error::API_Unavailable,
		///         Error::Platform_Error and Error::Invalid_Value.<br>
		/// Remarks: If an error occurs before the creation call is made, the WindowingAPI returns
		///          the Vulkan error code most appropriate for the error. Appropriate use of
		///          WindowingAPI::VulkanSupported and WindowingAPI::GetRequiredInstanceExtensions
		///          should eliminate almost all occurrences of these errors.<br>
		/// Thread safety: This function may be called from any thread. For synchronization details
		///                of Vulkan objects, see the Vulkan specification.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <param name="window">InternalWindow for which to create the surface for.</param>
		/// <param name="allocator">Optional allocator.</param>
		/// <param name="surface">Output variable for the new Vulkan Surface.</param>
		/// <returns>VK_SUCCESS if successful, or a Vulkan error code if an error occurred.</returns>
		static VkResult CreateWindowSurface(VkInstance instance, const InternalWindow* window,
		                                    const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		/// <summary>
		/// Hides the specified window from the taskbar.
		/// </summary>
		/// <param name="window">InternalWindow to hide from the taskbar.</param>
		static void HideWindowFromTaskbar(InternalWindow* window);
		/// <summary>
		/// Enable/Disable Drag and Drop feature for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow for which to set drag and drop.</param>
		/// <param name="value">Whether to enable or disable drag and drop.</param>
		static void SetDragAndDrop(InternalWindow* window, bool value);
#ifdef TRAP_PLATFORM_WINDOWS
		/// <summary>
		/// Returns the HWND handle of the InternalWindow.
		/// </summary>
		/// <param name="window">InternalWindow from which to get the HWND from.</param>
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
		/// <param name="window">InternalWindow which to center the cursor for.</param>
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
		static void CreateKeyTables();

		static InternalVideoMode PlatformGetVideoMode(const InternalMonitor* monitor);
		static void PlatformGetWindowSize(const InternalWindow* window, int32_t& width, int32_t& height);
		static void PlatformSetWindowPos(const InternalWindow* window, int32_t xPos, int32_t yPos);
		static void PlatformSetWindowMonitor(InternalWindow* window, InternalMonitor* monitor,
			                                 int32_t xPos, int32_t yPos, int32_t width, int32_t height,
											 int32_t refreshRate);
		static void PlatformSetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor);
		static std::vector<InternalVideoMode> PlatformGetVideoModes(const InternalMonitor* monitor);
		static bool PlatformInit();
		static void PlatformDestroyWindow(InternalWindow* window);
		static void PlatformShutdown();
		static void PlatformGetMonitorContentScale(const InternalMonitor* monitor, float& xScale, float& yScale);
		static void PlatformGetMonitorPos(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos);
		static void PlatformShowWindow(InternalWindow* window);
		static void PlatformFocusWindow(const InternalWindow* window);
		static bool PlatformCreateWindow(InternalWindow* window,
			                             WindowConfig& WNDConfig);
		static void PlatformSetWindowTitle(const InternalWindow* window, std::string& title);
		static bool PlatformCreateCursor(InternalCursor* cursor, const Scope<Image>& image, int32_t xHotspot,
		                                 int32_t yHotspot);
		static bool PlatformCreateStandardCursor(InternalCursor* cursor, const CursorType& type);
		static void PlatformDestroyCursor(InternalCursor* cursor);
		static void PlatformSetCursor(const InternalWindow* window, const InternalCursor* cursor);
		static void PlatformSetCursorMode(InternalWindow* window, CursorMode mode);
		static void PlatformSetCursorPos(InternalWindow* window, double xPos, double yPos);
		static void PlatformGetCursorPos(const InternalWindow* window, double& xPos, double& yPos);
		static void PlatformSetWindowIcon(InternalWindow* window, const Scope<Image>& image);
		static void PlatformGetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos);
		static void PlatformSetWindowSize(InternalWindow* window, int32_t width, int32_t height);
		static void PlatformSetWindowResizable(InternalWindow* window, bool enabled);
		static void PlatformSetWindowDecorated(const InternalWindow* window, bool enabled);
		static void PlatformSetWindowFloating(const InternalWindow* window, bool enabled);
		static void PlatformSetWindowOpacity(const InternalWindow* window, float opacity);
		static void PlatformSetWindowMousePassthrough(InternalWindow* window, bool enabled);
		static void PlatformHideWindowFromTaskbar(InternalWindow* window);
		static float PlatformGetWindowOpacity(const InternalWindow* window);
		static void PlatformGetFrameBufferSize(const InternalWindow* window, int32_t& width, int32_t& height);
		static void PlatformGetWindowContentScale(const InternalWindow* window, float& xScale, float& yScale);
		static void PlatformGetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos,
		                                       int32_t& width, int32_t& height);
		static bool PlatformWindowVisible(const InternalWindow* window);
		static bool PlatformWindowMaximized(const InternalWindow* window);
		static bool PlatformWindowMinimized(const InternalWindow* window);
		static void PlatformPollEvents();
		static bool PlatformWindowFocused(const InternalWindow* window);
		static bool PlatformWindowHovered(const InternalWindow* window);
		static bool PlatformRawMouseMotionSupported();
		static void PlatformSetRawMouseMotion(const InternalWindow* window, bool enabled);
		static int32_t PlatformGetKeyScanCode(Input::Key key);
		static const char* PlatformGetScanCodeName(int32_t scanCode);
		static void PlatformSetClipboardString(std::string_view string);
		static std::string PlatformGetClipboardString();
		static void PlatformGetRequiredInstanceExtensions(std::array<std::string, 2>& extensions);
		static VkResult PlatformCreateWindowSurface(VkInstance instance, const InternalWindow* window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		static void PlatformMaximizeWindow(const InternalWindow* window);
		static void PlatformMinimizeWindow(const InternalWindow* window);
		static void PlatformRequestWindowAttention(const InternalWindow* window);
		static void PlatformHideWindow(const InternalWindow* window);
		static void PlatformRestoreWindow(InternalWindow* window);
		static void PlatformSetWindowSizeLimits(InternalWindow* window, int32_t minWidth, int32_t minHeight,
		                                        int32_t maxWidth, int32_t maxHeight);
		static void PlatformSetDragAndDrop(InternalWindow* window, bool value);
		//-------------------------------------------------------------------------------------------------------------------//
		//Single Platform Functions------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
		/// <summary>
		/// Retrieves the available modes for the specified monitor.
		/// </summary>
		/// <param name="monitor">InternalMonitor to refresh VideoModes for.</param>
		/// <returns>True if VideoModes were successfully updated, false otherwise.</returns>
		static bool RefreshVideoModes(InternalMonitor* monitor);
		/// <summary>
		/// Initialized Vulkan API for further use.
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
		/// <param name="window">InternalWindow to make active on the given monitor.</param>
		static void AcquireMonitor(InternalWindow* window);
		/// <summary>
		/// Make the specified window active on its monitor.
		/// </summary>
		/// <param name="window">InternalWindow to make active on the given monitor.</param>
		static void AcquireMonitorBorderless(InternalWindow* window);
		/// <summary>
		/// Remove the window and restore the original video mode.
		/// </summary>
		/// <param name="window">InternalWindow which to release from current acquired monitor.</param>
		static void ReleaseMonitor(const InternalWindow* window);
		/// <summary>
		/// Lexically compare video modes, used by qsort.
		/// </summary>
		/// <param name="fp">InternalVideoMode to sort.</param>
		/// <param name="sp">InternalVideoMode to sort.</param>
		/// <returns>Index for the sorted InternalVideoMode.</returns>
		static int32_t CompareVideoModes(const void* fp, const void* sp);
		/// <summary>
		/// Searches an extension string for the specified extension.
		/// </summary>
		/// <param name="string">String to search in given extension.</param>
		/// <param name="extensions">Extension to test.</param>
		/// <returns>True if given string is inside the given extension.</returns>
		static bool StringInExtensionString(const char* string, const char* extensions);
		/// <summary>
		/// Updates the cursor image according to its cursor mode.
		/// </summary>
		/// <param name="window">InternalWindow to update.</param>
		static void UpdateCursorImage(const InternalWindow* window);
		/// <summary>
		/// Exit disabled cursor mode for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow for which to enable Cursor.</param>
		static void EnableCursor(InternalWindow* window);
		/// <summary>
		/// Apply disabled cursor mode to a focused window.
		/// </summary>
		/// <param name="window">InternalWindow for which to disable Cursor.</param>
		static void DisableCursor(InternalWindow* window);
		/// <summary>
		/// Enables raw messages for the mouse for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow for which to enable raw mouse motion.</param>
		static void EnableRawMouseMotion(const InternalWindow* window);
		/// <summary>
		/// Disables raw messages for the mouse.
		/// </summary>
		/// <param name="window">InternalWindow for which to disable raw mouse motion.</param>
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
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="xPos">X position of the cursor.</param>
		/// <param name="yPos">Y position of the cursor.</param>
		static void InputCursorPos(InternalWindow* window, double xPos, double yPos);
		/// <summary>
		/// Notifies shared code of a physical key event.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="key">Key that is pressed or released.</param>
		/// <param name="scancode">Scan code of the key which is pressed or released.</param>
		/// <param name="pressed">Whether the given Key is pressed or released.</param>
		static void InputKey(InternalWindow* window, Input::Key key, int32_t scancode, bool pressed);
		/// <summary>
		/// Notifies shared code of a Unicode codepoint input event.
		/// The 'plain' parameter determines whether to emit a regular character event.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="codePoint">Code point of the typed char.</param>
		static void InputChar(const InternalWindow* window, uint32_t codePoint);
		/// <summary>
		/// Notifies shared code of a mouse button click event.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="button">Mouse button that is pressed or released.</param>
		/// <param name="pressed">Whether the mouse button is pressed or released.</param>
		static void InputMouseClick(InternalWindow* window, Input::MouseButton button, bool pressed);
		/// <summary>
		/// Notifies shared code of a scroll event.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="xOffset">X offset of the scroll wheel.</param>
		/// <param name="yOffset">Y offset of the scroll wheel.</param>
		static void InputScroll(const InternalWindow* window, double xOffset, double yOffset);
		/// <summary>
		/// Notified shared code of a cursor enter/leave event.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="entered">Whether the cursor entered or leaved the window.</param>
		static void InputCursorEnter(InternalWindow* window, bool entered);
		/// <summary>
		/// Notifies shared code that a window framebuffer has been resized.
		/// The size is specified in pixels.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="width">New Framebuffer width.</param>
		/// <param name="height">New Framebuffer height.</param>
		static void InputFrameBufferSize(const InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// Notifies shared code that a window has been resized.
		/// The size is specified in screen coordinates.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="width">New Window width.</param>
		/// <param name="height">New Window height.</param>
		static void InputWindowSize(const InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// Notifies shared code that a window has been minimized.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="restored">Whether window was minimized or restored.</param>
		static void InputWindowMinimize(const InternalWindow* window, bool restored);
		/// <summary>
		/// Notifies shared code that a window has been maximized.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="restored">Whether window was maximized or restored.</param>
		static void InputWindowMaximize(const InternalWindow* window, bool restored);
		/// <summary>
		/// Notifies shared code that a window has moved.
		/// The position is specified in content area relative screen coordinates.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="x">X position of the window.</param>
		/// <param name="y">Y position of the window.</param>
		static void InputWindowPos(const InternalWindow* window, int32_t x, int32_t y);
		/// <summary>
		/// Notifies shared code that the user wishes to close a window.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		static void InputWindowCloseRequest(InternalWindow* window);
		/// <summary>
		/// Notifies shared code of files or directories dropped on a window.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="paths">All dropped paths.</param>
		static void InputDrop(const InternalWindow* window, const std::vector<std::string>& paths);
		/// <summary>
		/// Notifies shared code that a window has lost or received input focus.
		/// </summary>
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="focused">Whether window is focused or not.</param>
		static void InputWindowFocus(InternalWindow* window, bool focused);
		/// <summary>
		/// Notifies shared code of a keyboard layout change.
		/// </summary>
		static void InputKeyboardLayout();
		/// <summary>
		/// Chooses the video mode most closely matching the desired one.
		/// </summary>
		/// <param name="monitor">InternalMonitor to query.</param>
		/// <param name="desired">Desired InternalVideoMode.</param>
		/// <returns>Closest matching InternalVideoMode from the desired one.</returns>
		static InternalVideoMode* ChooseVideoMode(InternalMonitor* monitor, const InternalVideoMode& desired);
		/// <summary>
		/// Notifies shared code of a monitor connection or disconnection.
		/// </summary>
		/// <param name="monitor">InternalMonitor that was connected or disconnected.</param>
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
		/// <param name="window">InternalWindow for which to capture the cursor.</param>
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
		/// Replacement for IsWindowsVersionOrGreater as MinGW lacks versionhelpers.h.
		/// </summary>
		/// <param name="major">Major Windows version.</param>
		/// <param name="minor">Minor Windows version.</param>
		/// <param name="sp">Service pack.</param>
		/// <returns>Whether Windows version is given version or newer.</returns>
		static BOOL IsWindowsVersionOrGreaterWin32(WORD major, WORD minor, WORD sp);
		/// <summary>
		/// Checks whether we are on at least the specified build of Windows 10.
		/// </summary>
		/// <param name="build">Build number.</param>
		/// <returns>Whether Windows 10 version is given build or newer.</returns>
		static BOOL IsWindows10BuildOrGreaterWin32(WORD build);
		/// <summary>
		/// Checks whether we are on at least Windows 10 Anniversary Update.
		/// </summary>
		/// <returns>Whether Windows 10 Anniversary version or newer.</returns>
		static BOOL IsWindows10AnniversaryUpdateOrGreaterWin32();
		/// <summary>
		/// Checks whether we are on at least Windows 10 Creators Update.
		/// </summary>
		/// <returns>Whether Window 10 Creators version or newer.</returns>
		static BOOL IsWindows10CreatorsUpdateOrGreaterWin32();
		/// <summary>
		/// Checks whether we are on at least Windows 8.1.
		/// </summary>
		/// <returns>Whether Windows 8.1 version or newer.</returns>
		static BOOL IsWindows8Point1OrGreaterWin32();
		/// <summary>
		/// Checks whether we are on at least Windows 8.
		/// </summary>
		/// <returns>Whether Windows 8 version or newer.</returns>
		static BOOL IsWindows8OrGreaterWin32();
		/// <summary>
		/// Checks whether we are on at least Windows 7.
		/// </summary>
		/// <returns>Whether Windows 7 version or newer.</returns>
		static BOOL IsWindows7OrGreaterWin32();
		/// <summary>
		/// Checks whether we are on at least Windows Vista.
		/// </summary>
		/// <returns>Whether Windows Vista version or newer.</returns>
		static BOOL IsWindowsVistaOrGreaterWin32();
		/// <summary>
		/// Returns an UTF-8 string version of the specified wide string.
		/// </summary>
		/// <param name="wStr">Wide string representation.</param>
		/// <returns>UTF-8 string representation of the given wide string.</returns>
		static std::string CreateUTF8StringFromWideStringWin32(const std::wstring& wStr);
		/// <summary>
		/// Returns a wide string string version of the specified UTF-8 string.
		/// </summary>
		/// <param name="str">UTF-8 representation.</param>
		/// <returns>Wide string representation of the given UTF-8 string.</returns>
		static std::wstring CreateWideStringFromUTF8StringWin32(const std::string& str);
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
		/// <param name="window">InternalWindow which is meant.</param>
		/// <param name="xScale">New X content scale.</param>
		/// <param name="yScale">New Y content scale.</param>
		static void InputWindowContentScale(const InternalWindow* window, float xScale, float yScale);
		/// <summary>
		/// Updates key names according to the current keyboard layout.
		/// </summary>
		static void UpdateKeyNamesWin32();
		/// <summary>
		/// Window callback function (handles window messages).
		/// </summary>
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		/// <summary>
		/// Registers the TRAP window class.
		/// </summary>
		/// <returns>True if registration was successful, false otherwise.</returns>
		static bool RegisterWindowClassWin32();
		/// <summary>
		/// Unregisters the TRAP window class.
		/// </summary>
		static void UnregisterWindowClassWin32();
		/// <summary>
		/// Callback for EnumDisplayMonitors in CreateMonitor.
		/// </summary>
		static BOOL CALLBACK MonitorCallback(HMONITOR handle, HDC dc, RECT* rect, LPARAM data);
		/// <summary>
		/// Create monitor from an adapter and (optionally) a display.
		/// </summary>
		/// <returns>New InternalMonitor.</returns>
		static Scope<InternalMonitor> CreateMonitor(DISPLAY_DEVICEW* adapter, DISPLAY_DEVICEW* display);
		/// <summary>
		/// Poll for changes in the set of connected monitors.
		/// </summary>
		static void PollMonitorsWin32();
		/// <summary>
		/// Fit the InternalWindow to the monitor.
		/// </summary>
		/// <param name="window">InternalWindow to fit.</param>
		static void FitToMonitor(const InternalWindow* window);
		/// <summary>
		/// Change the current video mode.
		/// </summary>
		/// <param name="monitor">InternalMonitor to set InternalVideoMode for.</param>
		/// <param name="desired">Desire InternalVideoMode for the monitor.</param>
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
		/// <param name="window">InternalWindow to query.</param>
		/// <returns>Window style as a DWORD.</returns>
		static DWORD GetWindowStyle(const InternalWindow* window);
		/// <summary>
		/// Returns the extended window style for the specified window.
		/// </summary>
		/// <param name="window">InternalWindow to query.</param>
		/// <returns>Window Ex style as a DWORD.</returns>
		static DWORD GetWindowExStyle(const InternalWindow* window);
		/// <summary>
		/// Translate content area size to full window size according to styles and DPI.
		/// </summary>
		static void GetFullWindowSize(DWORD style, DWORD exStyle,
			int32_t contentWidth, int32_t contentHeight,
			int32_t& fullWidth, int32_t& fullHeight,
			UINT dpi);
		/// <summary>
		/// Returns whether the cursor is in the content area of the specified window.
		/// </summary>
		/// <param name="window">InternalWindow to check</param>
		/// <returns>True if cursor is inside the given windows content area, false otherwise.</returns>
		static bool CursorInContentArea(const InternalWindow* window);
		/// <summary>
		/// Creates an RGBA icon or cursor,
		/// </summary>
		/// <param name="image">Image to use.</param>
		/// <param name="xHot">Center X coordinate of the image.</param>
		/// <param name="yHot">Center Y coordinate of the image.</param>
		/// <param name="icon">Whether it is an icon or a cursor.</param>
		/// <returns>Handle to the icon.</returns>
		static HICON CreateIcon(const Scope<Image>& image, int32_t xHot, int32_t yHot, bool icon);
		/// <summary>
		/// Update native window styles to match attributes.
		/// </summary>
		/// <param name="window">InternalWindow to update window style for.</param>
		static void UpdateWindowStyles(const InternalWindow* window);
		/// <summary>
		/// Creates a dummy window for behind-the-scenes work.
		/// </summary>
		/// <returns>True if creation was successful, false otherwise.</returns>
		static bool CreateHelperWindow();
		/// <summary>
		/// Creates the TRAP window.
		/// </summary>
		static int32_t CreateNativeWindow(InternalWindow* window,
			                              const WindowConfig& WNDConfig);
		//----------//
		//Linux(X11)//
		//----------//
#elif defined(TRAP_PLATFORM_LINUX)
		/// <summary>
		/// Calculates the refresh rate, in Hz, from the specified RandR mode info.
		/// </summary>
		/// <returns>Refresh rate.</returns>
		static int32_t CalculateRefreshRate(const XRRModeInfo* mi);
		static InternalVideoMode VideoModeFromModeInfo(const XRRModeInfo* mi, const XRRCrtcInfo* ci);
		/// <summary>
		/// Sends an EWMH or ICCCM event to the window manager.
		/// </summary>
		static void SendEventToWM(const InternalWindow* window, Atom type, int32_t a, int32_t b, int32_t c,
		                          int32_t d, int32_t e);
		/// <summary>
		/// Returns whether it is a _NET_FRAME_EXTENTS event for the specified window.
		/// </summary>
		static int32_t IsFrameExtentsEvent(Display* display, XEvent* event, XPointer pointer);
		/// <summary>
		/// Wait for data to arrive using select.
		/// This avoids blocking other threads via the per-display XLib lock that also covers GLX functions.
		/// </summary>
		/// <param name="timeout">Time out in seconds.</param>
		static bool WaitForEvent(double* timeout);
		/// <summary>
		/// Retrieve a single window property of the specified type.
		/// </summary>
		static uint32_t GetWindowPropertyX11(::Window window, Atom property, Atom type, uint8_t** value);
		/// <summary>
		/// Updates the normal hints according to the window settings.
		/// </summary>
		static void UpdateNormalHints(InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// Waits until a VisibilityNotify event arrives for the specified window or the timeout period elapses.
		/// </summary>
		static bool WaitForVisibilityNotify(InternalWindow* window);
		/// <summary>
		/// Updates the full screen status of the window.
		/// </summary>
		static void UpdateWindowMode(InternalWindow* window);
		/// <summary>
		/// Returns the mode info for a RandR mode XID.
		/// </summary>
		static const XRRModeInfo* GetModeInfo(const XRRScreenResources* sr, RRMode id);
		/// <summary>
		/// Retrieve system content scale via folklore heuristics.
		/// </summary>
		/// <param name="xScale">Output variable for X system content scale.</param>
		/// <param name="yScale">Output variable for Y system content scale.</param>
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
		static int32_t ErrorHandler(Display* display, XErrorEvent* event);
		/// <summary>
		/// Clears the X error handler callback.
		/// </summary>
		static void ReleaseErrorHandlerX11();
		/// <summary>
		/// Check whether the specified atom is supported.
		/// </summary>
		static Atom GetAtomIfSupported(Atom* supportedAtoms, uint32_t atomCount, const char* atomName);
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
		static void InputMethodDestroyCallback(XIM im, XPointer clientData, XPointer callData);
		static void InputMethodInstantiateCallback(Display* display, XPointer clientData, XPointer callData);
		/// <summary>
		/// Poll for changes in the set of connected monitors.
		/// </summary>
		static void PollMonitorsX11();
		/// <summary>
		/// Returns whether the event is a selection event.
		/// </summary>
		static int32_t IsSelectionEvent(Display* display, XEvent* event, XPointer pointer);
		/// <summary>
		/// Set the specified property to the selection converted to the requested target.
		/// </summary>
		static Atom WriteTargetToProperty(const XSelectionRequestEvent* request);
		static void HandleSelectionRequest(XEvent& event);
		static void HandleSelectionClear(XEvent& event);
		/// <summary>
		/// Push contents of our selection to clipboard manager.
		/// </summary>
		static void PushSelectionToManagerX11();
		static void CreateInputContextX11(InternalWindow* window);
		static bool IsVisualTransparentX11(Visual* visual);
		/// <summary>
		/// Create the X11 window (and its colormap).
		/// </summary>
		static bool CreateNativeWindow(InternalWindow* window, WindowConfig& WNDConfig, Visual* visual,
		                               int32_t depth);
		/// <summary>
		/// Creates a native cursor object from the specified image and hotspot.
		/// </summary>
		/// <param name="image">Image to use.</param>
		/// <param name="xHotSpot">X center coordinate of the given image.</param>
		/// <param name="yHotSpot">Y center coordinate of the given image.</param>
		/// <returns>Newly created Cursor.</returns>
		static Cursor CreateCursorX11(const Scope<TRAP::Image>& image, int32_t xHotSpot, int32_t yHotSpot);
		/// <summary>
		/// Returns whether the window is iconified/minimized
		/// </summary>
		static int32_t GetWindowState(const InternalWindow* window);
		/// <summary>
		/// Convert XKB KeySym to Unicode.
		/// </summary>
		static int64_t KeySymToUnicode(uint32_t keySym);
		struct CodePair
		{
			uint16_t keySym;
			uint16_t UCS;
		};
		static const std::array<CodePair, 828> KeySymTab;
		/// <summary>
		/// Encode a Unicode code point to a UTF-8 stream.
		/// </summary>
		static std::size_t EncodeUTF8(char* s, uint32_t ch);
		static std::string GetSelectionString(Atom selection);
		/// <summary>
		/// Returns whether it is a property event for the specified selection transfer.
		/// </summary>
		static int32_t IsSelPropNewValueNotify(Display* display, XEvent* event, XPointer pointer);
		/// <summary>
		/// Convert the specified Latin-1 string to UTF-8.
		/// </summary>
		static std::string ConvertLatin1ToUTF8(const char* source);
		/// <summary>
		/// Reports the specified error, appending information about the last X error.
		/// </summary>
		/// <param name="error">Error code.</param>
		/// <param name="message">Description of error.</param>
		static void InputErrorX11(Error error, const char* message);
		/// <summary>
		/// Process the specified X event.
		/// </summary>
		static void ProcessEvent(XEvent& event);
		/// <summary>
		/// Translates an X11 key code to a TRAP key token.
		/// </summary>
		static Input::Key TranslateKey(int32_t scanCode);
#ifdef X_HAVE_UTF8_STRING
		/// <summary>
		/// Decode a Unicode code point from a UTF-8 stream.
		/// </summary>
		static uint32_t DecodeUTF8(const char** s);
#endif
		/// <summary>
		/// Splits and translates a text/uri-list into separate file paths.
		/// </summary>
		static std::vector<std::string> ParseUriList(char* text, int32_t& count);
		/// <summary>
		/// Set the current video mode for the specified monitor.
		/// </summary>
		static void SetVideoModeX11(InternalMonitor* monitor, const InternalVideoMode& desired);
		/// <summary>
		/// Restore the saved(original) video mode for the specified monitor.
		/// </summary>
		static void RestoreVideoModeX11(InternalMonitor* monitor);
		/// <summary>
		/// Allocates and returns a monitor object with the specified name and dimensions.
		/// </summary>
		static Scope<InternalMonitor> CreateMonitor(std::string name);
		/// <summary>
		/// Creates a dummy window for behind-the-scenes work.
		/// </summary>
		static ::Window CreateHelperWindow();
		/// <summary>
		/// Translate the X11 KeySyms for a key to a TRAP key.
		/// </summary>
		static Input::Key TranslateKeySyms(const KeySym* keySyms, int32_t width);
		/// <summary>
		/// Clear its handle when the input context has been destroyed.
		/// </summary>
		static void InputContextDestroyCallback(XIC ic, XPointer clientData, XPointer callData);
		/// <summary>
		/// Translate an X11 key code to a TRAP key.
		/// </summary>
		static std::string GetX11KeyboardLayoutName();

		friend std::string TRAP::Input::GetKeyboardLayoutName();
#endif
	};
}
#endif /*TRAP_WINDOWINGAPI_H*/