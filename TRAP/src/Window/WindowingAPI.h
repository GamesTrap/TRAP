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

#ifndef _TRAP_WINDOWINGAPI_H_
#define _TRAP_WINDOWINGAPI_H_

#include "ImageLoader/Image.h"
#include "Input/Input.h"

namespace TRAP::INTERNAL
{	
	class WindowingAPI final
	{
	private:
		WindowingAPI() = default;
		~WindowingAPI() = default;
		
	public:
		WindowingAPI(const WindowingAPI&) = delete;
		WindowingAPI& operator=(const WindowingAPI&) = delete;
		WindowingAPI(WindowingAPI&&) = delete;
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
		//The function pointer type for error callbacks.
		typedef void (*ErrorFunc)(Error error, const std::string& description);
		//The function pointer type for window position callbacks.
		typedef void (*WindowPositionFunc)(const InternalWindow*, int32_t xPos, int32_t yPos);
		//The function pointer type for window size callbacks.
		typedef void (*WindowSizeFunc)(const InternalWindow*, int32_t width, int32_t height);
		//The function pointer type for window minimize callbacks.
		typedef void (*WindowMinimizeFunc)(const InternalWindow*, bool restored);
		//The function pointer type for window maximize callbacks.
		typedef void (*WindowMaximizeFunc)(const InternalWindow*, bool restored);
		//The function pointer type for window close callbacks.
		typedef void (*WindowCloseFunc)(const InternalWindow*);
		//The function pointer type for window focus callbacks.
		typedef void (*WindowFocusFunc)(const InternalWindow*, bool focused);
		//The function pointer type for framebuffer size callbacks.
		typedef void (*FrameBufferSizeFunc)(const InternalWindow*, int32_t width, int32_t height);
		//The function pointer type for window content scale callbacks.
		typedef void (*WindowContentScaleFunc)(const InternalWindow*, float xScale, float yScale);
		//The function pointer type for mouse button callbacks.
		typedef void (*MouseButtonFunc)(const InternalWindow*, Input::MouseButton mouseButton, bool pressed);
		//The function pointer type for cursor position callbacks.
		typedef void (*CursorPositionFunc)(const InternalWindow*, double xPos, double yPos);
		//The function pointer type for cursor enter callbacks.
		typedef void (*CursorEnterFunc)(const InternalWindow*, bool entered);
		//The function pointer type for scroll callbacks.
		typedef void (*ScrollFunc)(const InternalWindow*, double xOffset, double yOffset);
		//The function pointer type for keyboard key callbacks.
		typedef void (*KeyFunc)(const InternalWindow*, Input::Key key, bool pressed);
		//The function pointer type for Unicode character callbacks.
		typedef void (*CharFunc)(const InternalWindow*, uint32_t codePoint);
		//The function pointer type for path drop callbacks.
		typedef void (*DropFunc)(const InternalWindow*, std::vector<std::string> paths);
		//The function pointer type for monitor configuration callbacks.
		typedef void (*MonitorFunc)(const InternalMonitor*, bool connected); //TODO Implement Interface | Handle Disconnect for Main window
	private:
		//--------------//
		//OpenGL Context//
		//--------------//
		//Generic function pointer used for returning client API function pointers
		//without forcing a cast from a regular pointer.
		typedef void (*GLProcess)();
		typedef void (*MakeContextCurrentFunc)(InternalWindow* window);
		typedef void (*SwapBuffersFunc)(const InternalWindow* window);
		typedef void (*SwapIntervalFunc)(int32_t interval);
		typedef bool (*ExtensionSupportedFunc)(const char* extension);
		typedef GLProcess(*GetProcAddressFunc)(const char* process);
		typedef void (*DestroyContextFunc)(InternalWindow* window);
		//--------------//
		//Vulkan Surface//
		//--------------//
#ifdef TRAP_PLATFORM_WINDOWS
		typedef VkFlags VkWin32SurfaceCreateFlagsKHR;
		typedef VkResult(*PFN_vkCreateWin32SurfaceKHR)(VkInstance, const VkWin32SurfaceCreateInfoKHR*, const VkAllocationCallbacks*, VkSurfaceKHR*);
#elif defined(TRAP_PLATFORM_LINUX)
		typedef VkResult (APIENTRY *PFN_vkCreateXlibSurfaceKHR)(VkInstance,const VkXlibSurfaceCreateInfoKHR*,const VkAllocationCallbacks*,VkSurfaceKHR*);
		typedef VkBool32 (APIENTRY *PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)(VkPhysicalDevice,uint32_t,Display*,VisualID);
		typedef VkResult (APIENTRY *PFN_vkCreateXcbSurfaceKHR)(VkInstance,const VkXcbSurfaceCreateInfoKHR*,const VkAllocationCallbacks*,VkSurfaceKHR*);
		typedef VkBool32 (APIENTRY *PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)(VkPhysicalDevice,uint32_t,xcb_connection_t*,xcb_visualid_t);
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
#elif defined(TRAP_PLATFORM_LINUX)
		//----------//
		//Linux(X11)//
		//----------//
		//GLX
		typedef struct __GLXFBConfig* GLXFBConfig;
		typedef struct __GLXcontext* GLXContext;
		
		//RandR
		typedef void(*PFN_XRRFreeCrtcInfo)(XRRCrtcInfo*);
		typedef void(*PFN_XRRFreeOutputInfo)(XRROutputInfo*);
		typedef void(*PFN_XRRFreeScreenResources)(XRRScreenResources*);
		typedef XRRCrtcInfo*(*PFN_XRRGetCrtcInfo)(Display*, XRRScreenResources*, RRCrtc);
		typedef XRROutputInfo*(*PFN_XRRGetOutputInfo)(Display*, XRRScreenResources*, RROutput);
		typedef RROutput(*PFN_XRRGetOutputPrimary)(Display*, ::Window);
		typedef XRRScreenResources*(*PFN_XRRGetScreenResourcesCurrent)(Display*, ::Window);
		typedef int32_t(*PFN_XRRQueryExtension)(Display*, int32_t*, int32_t*);
		typedef Status(*PFN_XRRQueryVersion)(Display*, int32_t*, int32_t*);
		typedef void(*PFN_XRRSelectInput)(Display*, ::Window, int32_t);
		typedef Status(*PFN_XRRSetCrtcConfig)(Display*, XRRScreenResources*, RRCrtc, Time, int32_t, int32_t, RRMode, Rotation, RROutput*, int32_t);
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
		typedef Status(*PFN_XIQueryVersion)(Display*, int32_t*, int32_t*);
		typedef int32_t(*PFN_XISelectEvents)(Display*, ::Window, XIEventMask*, int32_t);
		
		//XRender
		typedef int32_t(*PFN_XRenderQueryExtension)(Display*,int32_t*, int32_t*);
		typedef Status(*PFN_XRenderQueryVersion)(Display* dpy, int32_t*, int32_t*);
		typedef XRenderPictFormat*(*PFN_XRenderFindVisualFormat)(Display*, Visual const*);
		
		//Vulkan
		typedef VkFlags VkXlibSurfaceCreateFlagsKHR;
		typedef VkFlags VkXcbSurfaceCreateFlagsKHR;
		
		//XShape
		typedef int32_t (*PFN_XShapeQueryExtension)(Display*, int32_t*, int32_t*);
		typedef Status (*PFN_XShapeQueryVersion)(Display* dpy, int32_t*, int32_t*);
		typedef void(*PFN_XShapeCombineRegion)(Display*, ::Window, int32_t, int32_t, int32_t, Region, int32_t);
		typedef void(*PFN_XShapeCombineMask)(Display*, XID, int32_t, int32_t, int32_t, Pixmap, int32_t);
		
		//GLX
		typedef XID GLXWindow;
		typedef XID GLXDrawable;
		typedef void(*__GLXextproc)(void);
		typedef GLXFBConfig*(*PFNGLXGETFBCONFIGSPROC)(Display*, int32_t, int32_t*);
		typedef int32_t(*PFNGLXGETFBCONFIGATTRIBPROC)(Display*, GLXFBConfig, int32_t, int32_t*);
		typedef const char*(*PFNGLXGETCLIENTSTRINGPROC)(Display*, int32_t);
		typedef int32_t (*PFNGLXQUERYEXTENSIONPROC)(Display*, int32_t*, int32_t*);
		typedef int32_t (*PFNGLXQUERYVERSIONPROC)(Display*, int32_t*, int32_t*);
		typedef void (*PFNGLXDESTROYCONTEXTPROC)(Display*, GLXContext);
		typedef int32_t (*PFNGLXMAKECURRENTPROC)(Display*, GLXDrawable, GLXContext);
		typedef void (*PFNGLXSWAPBUFFERSPROC)(Display*, GLXDrawable);
		typedef const char* (*PFNGLXQUERYEXTENSIONSSTRINGPROC)(Display*, int32_t);
		typedef GLXContext (*PFNGLXCREATENEWCONTEXTPROC)(Display*, GLXFBConfig, int32_t, GLXContext, int32_t);
		typedef __GLXextproc (* PFNGLXGETPROCADDRESSPROC)(const GLubyte* procName);
		typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display*, GLXDrawable, int32_t);
		typedef XVisualInfo* (*PFNGLXGETVISUALFROMFBCONFIGPROC)(Display*, GLXFBConfig);
		typedef GLXWindow (*PFNGLXCREATEWINDOWPROC)(Display*, GLXFBConfig, ::Window, const int32_t*);
		typedef void (*PFNGLXDESTROYWINDOWPROC)(Display*, GLXWindow);
		
		typedef int (*PFNGLXSWAPINTERVALMESAPROC)(int32_t);
		typedef int (*PFNGLXSWAPINTERVALSGIPROC)(int32_t);
		typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, int32_t, const int32_t*);

		//XLib
		typedef XClassHint* (*PFN_XAllocClassHint)();
		typedef XSizeHints* (*PFN_XAllocSizeHints)();
		typedef XWMHints* (*PFN_XAllocWMHints)();
		typedef int (*PFN_XChangeProperty)(Display*, ::Window, Atom, Atom, int, int, const unsigned char*, int);
		typedef int (*PFN_XChangeWindowAttributes)(Display*, ::Window, unsigned long, XSetWindowAttributes*);
		typedef int (*PFN_XCheckIfEvent)(Display*, XEvent*, int(*)(Display*, XEvent*, XPointer), XPointer);
		typedef int (*PFN_XCheckTypedWindowEvent)(Display*, ::Window, int, XEvent*);
		typedef int (*PFN_XCloseDisplay)(Display*);
		typedef Status (*PFN_XCloseIM)(XIM);
		typedef int (*PFN_XConvertSelection)(Display*, Atom, Atom, Atom, ::Window, Time);
		typedef Colormap (*PFN_XCreateColormap)(Display*, ::Window, Visual*, int);
		typedef Cursor (*PFN_XCreateFontCursor)(Display*, unsigned int);
		typedef XIC (*PFN_XCreateIC)(XIM, ...);
		typedef ::Window (*PFN_XCreateWindow)(Display*, ::Window, int, int, unsigned int, unsigned int, unsigned int, int, unsigned int, Visual*, unsigned long, XSetWindowAttributes*);
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
		typedef int (*PFN_XGetErrorText)(Display*, int, char*, int);
		typedef int (*PFN_XGetEventData)(Display*, XGenericEventCookie*);
		typedef char* (*PFN_XGetICValues)(XIC, ...);
		typedef char* (*PFN_XGetIMValues)(XIM, ...);
		typedef int (*PFN_XGetInputFocus)(Display*, ::Window*, int*);
		typedef KeySym* (*PFN_XGetKeyboardMapping)(Display*, KeyCode, int, int*);
		typedef int (*PFN_XGetScreenSaver)(Display*, int*, int*, int*, int*);
		typedef ::Window (*PFN_XGetSelectionOwner)(Display*, Atom);
		typedef XVisualInfo* (*PFN_XGetVisualInfo)(Display*, long, XVisualInfo*, int*);
		typedef Status (*PFN_XGetWMNormalHints)(Display*, ::Window, XSizeHints*, long*);
		typedef Status (*PFN_XGetWindowAttributes)(Display*, ::Window, XWindowAttributes*);
		typedef int (*PFN_XGetWindowProperty)(Display*, ::Window, Atom, long, long, int, Atom, Atom*, int*, unsigned long*, unsigned long*, unsigned char**);
		typedef int (*PFN_XGrabPointer)(Display*, ::Window, int, unsigned int, int, int, ::Window, Cursor, Time);
		typedef Status (*PFN_XIconifyWindow)(Display*, ::Window, int);
		typedef Status (*PFN_XInitThreads)();
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
		typedef Status (*PFN_XSendEvent)(Display*, ::Window, int, long, XEvent*);
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
		typedef Status (*PFN_XSetWMProtocols)(Display*, ::Window, Atom*, int);
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
		typedef void (*PFN_XkbFreeKeyboard)(XkbDescPtr, unsigned int, int);
		typedef void (*PFN_XkbFreeNames)(XkbDescPtr, unsigned int, int);
		typedef XkbDescPtr (*PFN_XkbGetMap)(Display*, unsigned int, unsigned int);
		typedef Status (*PFN_XkbGetNames)(Display*, unsigned int, XkbDescPtr);
		typedef Status (*PFN_XkbGetState)(Display*, unsigned int, XkbStatePtr);
		typedef KeySym (*PFN_XkbKeycodeToKeysym)(Display*, KeyCode, int, int);
		typedef int (*PFN_XkbQueryExtension)(Display*, int*, int*, int*, int*, int*);
		typedef int (*PFN_XkbSelectEventDetails)(Display*, unsigned int, unsigned int, unsigned long, unsigned long);
		typedef int (*PFN_XkbSetDetectableAutoRepeat)(Display*, int, int*);
		typedef int (*PFN_Xutf8LookupString)(XIC, XKeyPressedEvent*, char*, int, KeySym*, Status*);
		typedef void (*PFN_Xutf8SetWMProperties)(Display*, ::Window, const char*, const char*, char**, int, XSizeHints*, XWMHints*, XClassHint*);

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
			Minimized,
			Visible,
			Focused,
			FocusOnShow,
			Hovered,
			Decorated,
			Floating,
			MousePassthrough
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
			Disabled,
			Captured
		};
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
#elif defined(TRAP_PLATFORM_LINUX)
		//GLX
		inline static constexpr uint32_t GLX_VENDOR = 1;
		inline static constexpr uint32_t GLX_RENDER_TYPE = 0x8011;
		inline static constexpr uint32_t GLX_RGBA_BIT = 0x00000001;
		inline static constexpr uint32_t GLX_DRAWABLE_TYPE = 0x8010;
		inline static constexpr uint32_t GLX_WINDOW_BIT = 0x00000001;
		inline static constexpr uint32_t GLX_RED_SIZE = 8;
		inline static constexpr uint32_t GLX_GREEN_SIZE = 9;
		inline static constexpr uint32_t GLX_BLUE_SIZE = 10;
		inline static constexpr uint32_t GLX_ALPHA_SIZE = 11;
		inline static constexpr uint32_t GLX_DEPTH_SIZE = 12;
		inline static constexpr uint32_t GLX_STENCIL_SIZE = 13;
		inline static constexpr uint32_t GLX_ACCUM_RED_SIZE = 14;
		inline static constexpr uint32_t GLX_ACCUM_GREEN_SIZE = 15;
		inline static constexpr uint32_t GLX_ACCUM_BLUE_SIZE = 16;
		inline static constexpr uint32_t GLX_ACCUM_ALPHA_SIZE = 17;
		inline static constexpr uint32_t GLX_AUX_BUFFERS = 7;
		inline static constexpr uint32_t GLX_STEREO = 6;
		inline static constexpr uint32_t GLX_DOUBLEBUFFER = 5;
		inline static constexpr uint32_t GLX_SAMPLES = 0x186A1;
		inline static constexpr uint32_t GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB = 0x20B2;
		inline static constexpr uint32_t GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB = 0x00000002;
		inline static constexpr uint32_t GLX_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001;
		inline static constexpr uint32_t GLX_CONTEXT_MAJOR_VERSION_ARB = 0x2091;
		inline static constexpr uint32_t GLX_CONTEXT_MINOR_VERSION_ARB = 0x2092;
		inline static constexpr uint32_t GLX_CONTEXT_PROFILE_MASK_ARB = 0x9126;
		inline static constexpr uint32_t GLX_CONTEXT_FLAGS_ARB = 0x2094;
		inline static constexpr uint32_t GLX_RGBA_TYPE = 0x8014;
		
		inline static constexpr uint32_t Button6 = 6;
		inline static constexpr uint32_t Button7 = 7;
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
		//Thread local storage structure
		struct TLS
		{
			bool Allocated = false;
#ifdef TRAP_PLATFORM_WINDOWS			
			DWORD Index = 0;
#elif defined(TRAP_PLATFORM_LINUX)
			pthread_key_t Key{};
#endif
		};
		//Mutex structure
		struct Mutex
		{
			bool Allocated = false;
#ifdef TRAP_PLATFORM_WINDOWS			
			CRITICAL_SECTION Section{};
#elif defined(TRAP_PLATFORM_LINUX)
			pthread_mutex_t Handle{};
#endif
		};
		//Per-thread error structure
		struct WindowingError
		{
			WindowingError* Next = nullptr;
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
			bool Visible = false;
			bool Decorated = true;
			bool Maximized = false;
			bool Focused = false;
			bool Floating = false;
			bool FocusOnShow = true;
			bool MousePassthrough = false;
		};
		//Context configuration
		struct ContextConfig
		{
			ContextAPI Client = ContextAPI::None;
			const InternalWindow* Share = nullptr;
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
#elif defined(TRAP_PLATFORM_LINUX)
			GLXContext Handle = nullptr;
			GLXWindow Window = 0;
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

			Scope<WindowingError> ErrorListHead = nullptr;

			std::forward_list<InternalWindow*> WindowList{};
			std::forward_list<InternalCursor*> CursorList{};
			
			std::vector<Scope<InternalMonitor>> Monitors{};

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
#elif defined(TRAP_PLATFORM_LINUX)
				bool KHR_XLib_Surface = false;
				bool KHR_XCB_Surface = false;
#elif defined(TRAP_PLATFORM_LINUX)
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
			DWORD ForegroundLockTimeout = 0;
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
			
			struct glx
			{
				int32_t Major = 0;
				int32_t Minor = 0;
				int32_t EventBase = 0;
				int32_t ErrorBase = 0;
				
				void* Handle = nullptr;
				
				//GLX 1.3 functions
				PFNGLXGETFBCONFIGSPROC GetFBConfigs;
				PFNGLXGETFBCONFIGATTRIBPROC GetFBConfigAttrib;
				PFNGLXGETCLIENTSTRINGPROC GetClientString;
				PFNGLXQUERYEXTENSIONPROC QueryExtension;
				PFNGLXQUERYVERSIONPROC QueryVersion;
				PFNGLXDESTROYCONTEXTPROC DestroyContext;
				PFNGLXMAKECURRENTPROC MakeCurrent;
				PFNGLXSWAPBUFFERSPROC SwapBuffers;
				PFNGLXQUERYEXTENSIONSSTRINGPROC QueryExtensionsString;
				PFNGLXCREATENEWCONTEXTPROC CreateNewContext;
				PFNGLXGETVISUALFROMFBCONFIGPROC GetVisualFromFBConfig;
				PFNGLXCREATEWINDOWPROC CreateWindow;
				PFNGLXDESTROYWINDOWPROC DestroyWindow;
				
				//GLX 1.4 and extension functions
				PFNGLXGETPROCADDRESSPROC GetProcAddress;
				PFNGLXGETPROCADDRESSPROC GetProcAddressARB;
				PFNGLXSWAPINTERVALSGIPROC SwapIntervalSGI;
				PFNGLXSWAPINTERVALEXTPROC SwapIntervalEXT;
				PFNGLXSWAPINTERVALMESAPROC SwapIntervalMESA;
				PFNGLXCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB;
				bool SGI_swap_control;
				bool EXT_swap_control;
				bool MESA_swap_control;
				bool ARB_multisample;
				bool ARB_framebuffer_sRGB;
				bool EXT_framebuffer_sRGB;
				bool ARB_create_context;
				bool ARB_create_context_profile;
			} GLX{};

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
		//This describes a single video mode.
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
		//Monitor structure
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
		//Cursor structure
		struct InternalCursor
		{
#ifdef TRAP_PLATFORM_WINDOWS
			HCURSOR Handle = nullptr;
#elif defined(TRAP_PLATFORM_LINUX)
			Cursor Handle = 0;
#endif
		};
		//Window and Context structure
		struct InternalWindow
		{			
			//Window settings and state
			bool Resizable = true;
			bool Decorated = true;
			bool Floating = false;
			bool ShouldClose = false;
			bool FocusOnShow = true;
			bool MousePassthrough = false;
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

			Context context{};

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
		static void DestroyWindow(Scope<InternalWindow> window);
		//Makes the context of the specified window current for the calling
		static void MakeContextCurrent(InternalWindow* window);
		//Resets all window hints to their default values.
		static void DefaultWindowHints();
		//Sets the specified window hint to the desired value.
		static void WindowHint(Hint hint, bool value);
		//Sets the specified sample count to the desired sample value.
		static void SetSamples(uint32_t samples); //TODO Implement for OpenGL MSAA
		//Sets the specified window context API to the desired value
		static void SetContextAPI(ContextAPI contextAPI);
		//Returns the name of the specified monitor.
		static std::string GetMonitorName(const InternalMonitor* monitor);
		//Returns the primary monitor.
		static const Scope<InternalMonitor>& GetPrimaryMonitor();
		//Returns the currently connected monitors.
		static std::vector<InternalMonitor*> GetMonitors();
		//Returns the current mode of the specified monitor.
		static const InternalVideoMode& GetVideoMode(InternalMonitor* monitor);
		//Returns the available video modes for the specified monitor.
		static std::vector<InternalVideoMode> GetVideoModes(InternalMonitor* monitor);
		//Creates a window and its associated context.
		//
		//Win32:   - Window creation will fail if the Microsoft GDI software OpenGL implementation is the only one available.
		//         - If the executable has an icon resource named 'TRAP_ICON', it will be set as the initial icon for the window.
		//           If no such icon is present, the 'IDI_APPLICATION' icon will be used instead.
		//X11:     - Some window managers will not respect the placement of initially hidden windows.
		//         - Due to the asynchronous nature of X11, it may take a moment for a window to reach its requested state.
		//           This means you may not be able to query the final size, position or other attributes directly after window creation.
		//         - The class part of the 'WM_CLASS' window property will by default be set to the window title passed to this function.
		//           The instance part will use the contents of the 'RESOURCE_NAME' environment variable, if present and not empty, or
		//           fall back to the window title.
		//Wayland: - Compositors should implement the xdg-decoration protocol for the WindowingAPI to decorate the window properly.
		//           If this protocol isn't supported, or if the compositor prefers client-side decorations, a very simple fallback
		//           frame will be drawn using the wp_viewporter protocol.
		//           A compositor can still emit close, maximize or fullscreen events, using for instance a keybind mechanism.
		//           If neither of these protocls are supported, the window won't be decorated.
		//         - A full screen window will not attempt to change the mode, no matter what the requested size or refresh rate.
		//         - Screensaver inhibition requires the idle-inhibit protocol to be implemented in the user's compositor.
		static Scope<InternalWindow> CreateWindow(uint32_t width, uint32_t height, const std::string& title, InternalMonitor* monitor, const InternalWindow* share);
		//Sets the close flag of the specified window.
		static void SetWindowShouldClose(InternalWindow* window, bool value);
		//Sets the title of the specified window.
		static void SetWindowTitle(const InternalWindow* window, const std::string& title);
		//Retrieves the content scale for the specified monitor.
		static void GetMonitorContentScale(const InternalMonitor* monitor, float& xScale, float& yScale);
		//Destroys a cursor.
		static void DestroyCursor(Scope<InternalCursor> cursor);
		//Creates a custom cursor.
		static Scope<InternalCursor> CreateCursor(const Scope<Image>& image, int32_t xHotspot, int32_t yHotspot);
		//Creates a cursor with a standard shape.
		static Scope<InternalCursor> CreateStandardCursor(const CursorType& type);
		//Sets the cursor for the window.
		static void SetCursor(InternalWindow* window, InternalCursor* cursor);
		//Sets the icon for the specified window.
		//
		//Wayland: - There is no existing protocol to change an icon, the window will thus inherit the one defined in the
		//           application's desktop file.
		static void SetWindowIcon(InternalWindow* window, const Scope<Image>& image);
		//Sets the position of the content area of the specified window.
		//
		//Wayland: - There is no way for an application to set the global position of its windows.
		static void SetWindowPos(const InternalWindow* window, int32_t xPos, int32_t yPos);
		//Retrieves the position of the content area of the specified window.
		//
		//Wayland: - There is no way for an application to retrieve the global position of its windows.
		static void GetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos);
		//Sets the size of the content area of the specified window.
		//
		//Wayland: - A full screen window will not attempt to change the mode, no matter what the requested size.
		static void SetWindowSize(InternalWindow* window, int32_t width, int32_t height);
		//Retrieves the size of the content area of the specified window.
		static void GetWindowSize(const InternalWindow* window, int32_t& width, int32_t& height);
		//Retrieves the size of the framebuffer of the specified window.
		static void GetFrameBufferSize(const InternalWindow* window, int32_t& width, int32_t& height);
		//Sets the opacity of the whole window.
		static void SetWindowOpacity(const InternalWindow* window, float opacity);
		//Returns the opacity of the whole window.
		static float GetWindowOpacity(const InternalWindow* window);
		//Retrieves the content scale for the specified window.
		static void GetWindowContentScale(const InternalWindow* window, float& xScale, float& yScale);
		//Sets an attribute for the specified window.
		static void SetWindowAttrib(InternalWindow* window, Hint hint, bool value);
		//Returns an attribute of the specified window.
		static bool GetWindowAttrib(const InternalWindow* window, Hint hint);
		//Sets the mode, monitor, video mode and placement of a window.
		//
		//Wayland: - The desired window position is ignored, as there is no way for an application to set this property.
		//         - Setting the window to full screen will not attempt to change the mode, no matter what the
		//           requested size or refresh rate.
		static void SetWindowMonitor(InternalWindow* window,
		                             InternalMonitor* monitor,
		                             int32_t xPos,
		                             int32_t yPos,
		                             int32_t width,
		                             int32_t height,
		                             int32_t refreshRate);
		//Sets the mode, monitor and placement of a window.
		static void SetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor);
		//Sets the user pointer of the specified window.
		static void SetWindowUserPointer(InternalWindow* window, void* pointer);
		//Returns the user pointer of the specified window.
		static void* GetWindowUserPointer(const InternalWindow* window);
		//Sets the error callback.
		static void SetErrorCallback(ErrorFunc callback);
		//Sets the monitor configuration callback.
		static void SetMonitorCallback(MonitorFunc callback);
		//Sets the position callback for the specified window.
		//
		//Wayland: - This callback will never be called, as there is no way for an application to know its global position.
		static void SetWindowPosCallback(InternalWindow* window, WindowPositionFunc callback);
		//Sets the size callback for the specified window.
		static void SetWindowSizeCallback(InternalWindow* window, WindowSizeFunc callback);
		//Sets the minimize callback for the specified window.
		static void SetWindowMinimizeCallback(InternalWindow* window, WindowMinimizeFunc callback);
		//Sets the maximize callback for the specified window.
		static void SetWindowMaximizeCallback(InternalWindow* window, WindowMaximizeFunc callback);
		//Sets the close callback for the specified window.
		static void SetWindowCloseCallback(InternalWindow* window, WindowCloseFunc callback);
		//Sets the focus callback for the specified window.
		static void SetWindowFocusCallback(InternalWindow* window, WindowFocusFunc callback);
		//Sets the framebuffer resize callback for the specified window.
		static void SetFrameBufferSizeCallback(InternalWindow* window, FrameBufferSizeFunc callback);
		//Sets the window content scale callback for the specified window.
		static void SetContentScaleCallback(InternalWindow* window, WindowContentScaleFunc callback);
		//Sets the key callback.
		static void SetKeyCallback(InternalWindow* window, KeyFunc callback);
		//Sets the Unicode character callback.
		static void SetCharCallback(InternalWindow* window, CharFunc callback);
		//Sets the mouse button callback.
		static void SetMouseButtonCallback(InternalWindow* window, MouseButtonFunc callback);
		//Sets the cursor position callback.
		static void SetCursorPosCallback(InternalWindow* window, CursorPositionFunc callback);
		//Sets the cursor enter callback.
		static void SetCursorEnterCallback(InternalWindow* window, CursorEnterFunc callback);
		//Sets the scroll callback.
		static void SetScrollCallback(InternalWindow* window, ScrollFunc callback);
		//Sets the path drop callback.
		static void SetDropCallback(InternalWindow* window, DropFunc callback);
		//Gets the error callback.
		static ErrorFunc GetErrorCallback();
		//Gets the monitor configuration callback.
		static MonitorFunc GetMonitorCallback();
		//Gets the position callback for the specified window.
		static WindowPositionFunc GetWindowPosCallback(const InternalWindow* window);
		//Gets the size callback for the specified window.
		static WindowSizeFunc GetWindowSizeCallback(const InternalWindow* window);
		//Gets the close callback for the specified window.
		static WindowCloseFunc GetWindowCloseCallback(const InternalWindow* window);
		//Gets the focus callback for the specified window.
		static WindowFocusFunc GetWindowFocusCallback(const InternalWindow* window);
		//Gets the framebuffer resize callback for the specified window.
		static FrameBufferSizeFunc GetFrameBufferSizeCallback(const InternalWindow* window);
		//Gets the window content scale callback for the specified window.
		static WindowContentScaleFunc GetWindowContentScaleCallback(const InternalWindow* window);
		//Gets the key callback.
		static KeyFunc GetKeyCallback(const InternalWindow* window);
		//Gets the Unicode character callback.
		static CharFunc GetCharCallback(const InternalWindow* window);
		//Gets the mouse button callback.
		static MouseButtonFunc GetMouseButtonCallback(const InternalWindow* window);
		//Gets the cursor position callback.
		static CursorPositionFunc GetCursorPosCallback(const InternalWindow* window);
		//Gets the cursor enter callback.
		static CursorEnterFunc GetCursorEnterCallback(const InternalWindow* window);
		//Gets the scroll callback.
		static ScrollFunc GetScrollCallback(const InternalWindow* window);
		//Gets the path drop callback.
		static DropFunc GetDropCallback(const InternalWindow* window);
		//Processes all pending events.
		static void PollEvents();
		//Sets the cursor mode for the specified window.
		static void SetCursorMode(InternalWindow* window, CursorMode mode);
		//Retrieves the cursor mode for the specified window.
		static CursorMode GetCursorMode(const InternalWindow* window);
		//Returns whether raw mouse motion is supported.
		static bool RawMouseMotionSupported();
		//Sets the raw mouse motion mode for the specified window.
		static void SetRawMouseMotionMode(InternalWindow* window, bool enabled);
		//Retrvieves the raw mouse motion mode for the specified window.
		static bool GetRawMouseMotionMode(const InternalWindow* window);
		//Returns the layout-specific name of the specified printable key.
		static const char* GetKeyName(Input::Key key, int32_t scanCode);
		//Returns the last reported state of a keyboard key for the specified window.
		static bool GetKey(const InternalWindow* window, Input::Key key);
		//Returns the last reported state of a mouse button for the specified window.
		static bool GetMouseButton(const InternalWindow* window, Input::MouseButton button);
		//Sets the position of the cursor, relative to the content area of the window
		//
		//Wayland: - This function will only work when the cursor mode is Disabled, otherwise it will do nothing.
		static void SetCursorPos(InternalWindow* window, double xPos, double yPos);
		//Retrieves the position of the cursor relative to the content area of the window.
		static void GetCursorPos(const InternalWindow* window, double& xPos, double& yPos);
		//Returns the position of the monitor's viewport on the virtual screen.
		static void GetMonitorPos(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos);
		//Retrieves the work area of the monitor.
		static void GetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos, int32_t& width, int32_t& height);
		//Makes the specified window visible.
		static void ShowWindow(InternalWindow* window);
		//Brings the specified window to front and sets input focus.
		//
		//Wayland: - It is not possible for an application to bring its windows to front.
		static void FocusWindow(const InternalWindow* window);
		//Maximizes the specified window.
		static void MaximizeWindow(const InternalWindow* window);
		//Minimizes the specified window.
		//
		//Wayland: - Once a window is minimized, RestoreWindow won't be able to restore it.
		//           This is a design decision of the xdg-shell protocol.
		static void MinimizeWindow(const InternalWindow* window);
		//Requests user attention to the specified window.
		static void RequestWindowAttention(const InternalWindow* window);
		//Hides the specified window.
		static void HideWindow(const InternalWindow* window);
		//Restores the specified window.
		static void RestoreWindow(InternalWindow* window);
		//Sets the size limits of the specified window.
		//
		//Wayland: The size limits will not be applied until the window is actually resized, either by the user or by the compositor.
		static void SetWindowSizeLimits(InternalWindow* window, int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight);
		//Sets the clipboard to the specified string.
		static void SetClipboardString(const std::string& string);
		//Returns the contents of the clipboard as a string.
		static std::string GetClipboardString();
		//Returns the window whose context is current on the calling thread.
		static InternalWindow* GetCurrentContext();
		//Swaps the front and back buffers of the specified window.
		static void SwapBuffers(InternalWindow* window);
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
		static VkResult CreateWindowSurface(VkInstance instance, const InternalWindow* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		static void HideWindowFromTaskbar(InternalWindow* window);
#ifdef TRAP_PLATFORM_WINDOWS
		static HWND GetWin32Window(const InternalWindow* window);
#endif
		//-------//
		//Private//
		//-------//
	private:
		//Checks whether the desired context attributes are valid
		static bool IsValidContextConfig(const ContextConfig& CTXConfig);
		//Center the cursor in the content area of the specified window
		static void CenterCursorInContentArea(InternalWindow* window);
		//Notifies shared code of an error
		static void InputError(Error code, const std::string& str);		
		//-------------------------------------------------------------------------------------------------------------------//
		//Platform Specific Functions----------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//Create key code translation tables
		static void CreateKeyTables();
		
		static InternalVideoMode PlatformGetVideoMode(const InternalMonitor* monitor);
		static void PlatformGetWindowSize(const InternalWindow* window, int32_t& width, int32_t& height);
		static void PlatformSetWindowPos(const InternalWindow* window, int32_t xPos, int32_t yPos);
		static void PlatformGetWindowFrameSize(const InternalWindow* window,
			                                   int32_t& left, int32_t& top,
			                                   int32_t& right, int32_t& bottom);
		static void PlatformSetWindowMonitor(InternalWindow* window, InternalMonitor* monitor,
			                                 int32_t xPos, int32_t yPos, int32_t width, int32_t height, int32_t refreshRate);
		static void PlatformSetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor);
		static std::vector<InternalVideoMode> PlatformGetVideoModes(const InternalMonitor* monitor);
		static bool PlatformInit();
		static bool PlatformCreateMutex(Mutex& mutex); 
		static bool PlatformCreateTLS(TLS& tls);
		static void PlatformSetTLS(TLS& tls, void* value);
		static void* PlatformGetTLS(TLS& tls);
		static void PlatformDestroyTLS(TLS& tls);
		static void PlatformDestroyMutex(Mutex& mutex);
		static void PlatformLockMutex(Mutex& mutex);
		static void PlatformUnlockMutex(Mutex& mutex);
		static void PlatformDestroyWindow(InternalWindow* window);
		static void PlatformShutdown();
		static void PlatformGetMonitorContentScale(const InternalMonitor* monitor, float& xScale, float& yScale);
		static void PlatformGetMonitorPos(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos);
		static void PlatformShowWindow(InternalWindow* window);
		static void PlatformFocusWindow(const InternalWindow* window);
		static bool PlatformCreateWindow(InternalWindow* window,
			                             const WindowConfig& WNDConfig,
			                             const ContextConfig& CTXConfig,
			                             const FrameBufferConfig& FBConfig);
		static void PlatformSetWindowTitle(const InternalWindow* window, const std::string& title);
		static bool PlatformCreateCursor(InternalCursor* cursor, const Scope<Image>& image, int32_t xHotspot, int32_t yHotspot);
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
		static void PlatformGetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos, int32_t& width, int32_t& height);
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
		static void PlatformSetClipboardString(const std::string& string);
		static std::string PlatformGetClipboardString();
		static void PlatformGetRequiredInstanceExtensions(std::array<std::string, 2>& extensions);
		static VkResult PlatformCreateWindowSurface(VkInstance instance, const InternalWindow* window,
			                                        const VkAllocationCallbacks* allocator, VkSurfaceKHR& surface);
		static void PlatformMaximizeWindow(const InternalWindow* window);
		static void PlatformMinimizeWindow(const InternalWindow* window);
		static void PlatformRequestWindowAttention(const InternalWindow* window);
		static void PlatformHideWindow(const InternalWindow* window);
		static void PlatformRestoreWindow(InternalWindow* window);
		static void PlatformSetWindowSizeLimits(InternalWindow* window, int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight);
		//-------------------------------------------------------------------------------------------------------------------//
		//Single Platform Functions------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//---------//
		//Universal//
		//---------//
		//Retrieves the available modes for the specified monitor
		static bool RefreshVideoModes(InternalMonitor* monitor);
		static bool RefreshContextAttribs(InternalWindow* window,
										  const ContextConfig& CTXConfig);
		static bool InitVulkan(uint32_t mode);
		//Splits a color depth into red, green and blue bit depths
		static void SplitBPP(int32_t bpp, int32_t& red, int32_t& green, int32_t& blue);
		//Make the specified window and its video mode active on its monitor
		static void AcquireMonitor(InternalWindow* window);
		//Make the specified window active on its monitor
		static void AcquireMonitorBorderless(InternalWindow* window);
		//Remove the window and restore the original video mode
		static void ReleaseMonitor(const InternalWindow* window);
		//Lexically compare video modes, used by qsort
		static int32_t CompareVideoModes(const void* fp, const void* sp);
		//Searches an extension string for the specified extension
		static bool StringInExtensionString(const char* string, const char* extensions);
		//Chooses the framebuffer config that best matches the desired one
		static const FrameBufferConfig* ChooseFBConfig(const FrameBufferConfig& desired,
			                                           const std::vector<FrameBufferConfig>& alternatives);
		//Updates the cursor image according to its cursor mode
		static void UpdateCursorImage(const InternalWindow* window);		
		//Exit disabled cursor mode for the specified window
		static void EnableCursor(InternalWindow* window);
		//Apply disabled cursor mode to a focused window
		static void DisableCursor(InternalWindow* window);
		//Enables raw messages for the mouse for the specified window
		static void EnableRawMouseMotion(const InternalWindow* window);
		//Disables raw messages for the mouse
		static void DisableRawMouseMotion(const InternalWindow* window);
		static std::string GetVulkanResultString(VkResult result);
		//Notifies shared code of a cursor motion event
		//The position is specified in content area relative screen coordinates
		static void InputCursorPos(InternalWindow* window, double xPos, double yPos);
		//Notifies shared code of a physical key event
		static void InputKey(InternalWindow* window, Input::Key key, int32_t scancode, bool pressed);
		//Notifies shared code of a Unicode codepoint input event
		//The 'plain' parameter determines whether to emit a regular character event
		static void InputChar(const InternalWindow* window, uint32_t codePoint);
		//Notifies shared code of a mouse button click event
		static void InputMouseClick(InternalWindow* window, Input::MouseButton button, bool pressed);
		//Notifies shared code of a scroll event
		static void InputScroll(const InternalWindow* window, double xOffset, double yOffset);
		//Notified shared code of a cursor enter/leave event
		static void InputCursorEnter(InternalWindow* window, bool entered);
		//Notifies shared code that a window framebuffer has been resized
		//The size is specified in pixels
		static void InputFrameBufferSize(const InternalWindow* window, int32_t width, int32_t height);
		//Notifies shared code that a window has been resized
		//The size is specified in screen coordinates
		static void InputWindowSize(const InternalWindow* window, int32_t width, int32_t height);
		//Notifies shared code that a window has been minimized
		static void InputWindowMinimize(const InternalWindow* window, bool restored);
		//Notifies shared code that a window has been maximized
		static void InputWindowMaximize(const InternalWindow* window, bool restored);
		//Notifies shared code that a window has moved
		//The position is specified in content area relative screen coordinates
		static void InputWindowPos(const InternalWindow* window, int32_t x, int32_t y);
		//Notifies shared code that the user wishes to close a window
		static void InputWindowCloseRequest(InternalWindow* window);
		//Notifies shared code of files or directories dropped on a window
		static void InputDrop(const InternalWindow* window, const std::vector<std::string>& paths);
		//Notifies shared code that a window has lost or received input focus
		static void InputWindowFocus(InternalWindow* window, bool focused);
		//Chooses the video mode most closely matching the desired one
		static InternalVideoMode* ChooseVideoMode(InternalMonitor* monitor, const InternalVideoMode& desired);
		//Notifies shared code of a monitor connection or disconnection
		static void InputMonitor(Scope<InternalMonitor> monitor, bool connected, uint32_t placement);
		//Notifies shared code of a monitor connection or disconnection
		static void InputMonitorDisconnect(uint32_t monitorIndex, uint32_t placement);
		//Sets the cursor clip rect to the window content area
		static void CaptureCursor(InternalWindow* window);
		//Disables clip cursor
		static void ReleaseCursor();
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
		//Notifies shared code that a window content scale has changed
		//The scale is specified as the ratio between the current and default DPI
		static void InputWindowContentScale(const InternalWindow* window, float xScale, float yScale);
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
		static Scope<InternalMonitor> CreateMonitor(DISPLAY_DEVICEW* adapter, DISPLAY_DEVICEW* display);
		//Poll for changes in the set of connected monitors
		static void PollMonitorsWin32();
		static void FitToMonitor(const InternalWindow* window);
		//Change the current video mode
		static void SetVideoModeWin32(InternalMonitor* monitor, const InternalVideoMode& desired);
		static void GetMonitorContentScaleWin32(HMONITOR handle, float& xScale, float& yScale);
		//Returns the window style for the specified window
		static DWORD GetWindowStyle(const InternalWindow* window);
		//Returns the extended window style for the specified window
		static DWORD GetWindowExStyle(const InternalWindow* window);
		//Translate content area size to full window size according to styles and DPI
		static void GetFullWindowSize(DWORD style, DWORD exStyle,
			                          int32_t contentWidth, int32_t contentHeight,
			                          int32_t& fullWidth, int32_t& fullHeight,
			                          UINT dpi);		
		static bool InitWGL();
		//Shutdown WGL
		static void ShutdownWGL();
		static bool CreateContextWGL(InternalWindow* window,
			                         const ContextConfig& CTXConfig,
			                         const FrameBufferConfig& FBConfig);
		static void DestroyContextWGL( InternalWindow* window);
		static void MakeContextCurrentWGL(InternalWindow* window);
		static void SwapBuffersWGL(const InternalWindow* window);
		static void SwapIntervalWGL(int32_t interval);
		static GLProcess GetProcAddressWGL(const char* procName);
		static bool ExtensionSupportedWGL(const char* extension);		
		//Return the value corresponding to the specified attribute
		static int32_t FindPixelFormatAttribValue(const std::vector<int32_t>& attribs,
			                                      const std::vector<int32_t>& values,
			                                      int32_t attrib);
		//Returns a list of available and usable framebuffer configs
		static int32_t ChoosePixelFormat(const InternalWindow* window,
			                             const ContextConfig& CTXConfig,
			                             const FrameBufferConfig& FBConfig);		
		//Returns whether the cursor is in the content area of the specified window
		static bool CursorInContentArea(const InternalWindow* window);
		//Creates an RGBA icon or cursor
		static HICON CreateIcon(const Scope<Image>& image, int32_t xHot, int32_t yHot, bool icon);
		//Update native window styles to match attributes
		static void UpdateWindowStyles(const InternalWindow* window);
		//Creates a dummy window for behind-the-scenes work
		static bool CreateHelperWindow();
		//Creates the TRAP window
		static int32_t CreateNativeWindow(InternalWindow* window,
										  const WindowConfig& WNDConfig,
										  const FrameBufferConfig& FBConfig);
		//----------//
		//Linux(X11)//
		//----------//	
#elif defined(TRAP_PLATFORM_LINUX)
		//Calculates the refresh rate, in Hz, from the specified RandR mode info
		static int32_t CalculateRefreshRate(const XRRModeInfo* mi);
		static InternalVideoMode VideoModeFromModeInfo(const XRRModeInfo* mi, const XRRCrtcInfo* ci);
		//Sends an EWMH or ICCCM event to the window manager
		static void SendEventToWM(const InternalWindow* window, Atom type, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e);
		//Returns whether it is a _NET_FRAME_EXTENTS event for the specified window
		static int32_t IsFrameExtentsEvent(Display* display, XEvent* event, XPointer pointer);
		//Wait for data to arrive using select
		//This avoids blocking other threads via the per-display Xlib lock that also covers GLX functions
		static bool WaitForEvent(double* timeout);
		//Retrieve a single window property of the specified type
		static uint32_t GetWindowPropertyX11(::Window window, Atom property, Atom type, uint8_t** value);
		//Updates the normal hints according to the window settings
		static void UpdateNormalHints(InternalWindow* window, int32_t width, int32_t height);
		//Waits until a VisibilityNotify event arrives for the specified window or the timeout period elapses
		static bool WaitForVisibilityNotify(InternalWindow* window);
		//Updates the full screen status of the window
		static void UpdateWindowMode(InternalWindow* window);
		//Returns the mode info for a RandR mode XID
		static const XRRModeInfo* GetModeInfo(const XRRScreenResources* sr, RRMode id);
		//Retrieve system content scale via folklore heuristics
		static void GetSystemContentScale(float& xScale, float& yScale);
		//Look for and initialize supported X11 extensions
		static bool InitExtensions();
		//Check whether the running window manager is EMWH-compliant
		static void DetectEWMH();
		//Sets the X error handler callback
		static void GrabErrorHandlerX11();
		//X error handler
		static int32_t ErrorHandler(Display* display, XErrorEvent* event);
		//Clears the X error handler callback
		static void ReleaseErrorHandlerX11();
		//Check whether the specified atom is supported
		static Atom GetAtomIfSupported(Atom* supportedAtoms, uint32_t atomCount, const char* atomName);
		//Create a blank cursor for hidden and disabled cursor modes
		static Cursor CreateHiddenCursor();
		//Check whether the IM has a usable style
		static bool HasUsableInputMethodStyle();
		static void InputMethodDestroyCallback(XIM im, XPointer clientData, XPointer callData);
		static void InputMethodInstantiateCallback(Display* display, XPointer clientData, XPointer callData);
		//Poll for changes in the set of connected monitors
		static void PollMonitorsX11();
		//Returns whether the event is a selection event
		static int32_t IsSelectionEvent(Display* display, XEvent* event, XPointer pointer);
		//Set the specified property to the selection converted to the requested target
		static Atom WriteTargetToProperty(const XSelectionRequestEvent* request);
		static void HandleSelectionRequest(XEvent& event);
		static void HandleSelectionClear(XEvent& event);
		//Push contents of our selection to clipboard manager
		static void PushSelectionToManagerX11();
		static void CreateInputContextX11(InternalWindow* window);
		//Shutdown GLX
		static void ShutdownGLX();
		//Initialize GLX
		static bool InitGLX();
		static bool ExtensionSupportedGLX(const char* extension);
		//Returns the Visual and depth of the chosen GLXFBConfig
		static bool ChooseVisualGLX(const WindowConfig& WNDConfig,
		                            const ContextConfig& CTXConfig,
                                    const FrameBufferConfig& FBConfig,
                                    Visual** visual, 
							        int32_t* depth);
		static GLProcess GetProcAddressGLX(const char* procName);
		//Return the GLXFBConfig most closely matching the specified hints
		static bool ChooseGLXFBConfig(const FrameBufferConfig& desired, GLXFBConfig* result);
		//Returns the specified attribute of the specified GLXFBConfig
		static int32_t GetGLXFBConfigAttrib(GLXFBConfig fbconfig, int32_t attrib);
		static bool IsVisualTransparentX11(Visual* visual);
		//Create the X11 window (and its colormap)
		static bool CreateNativeWindow(InternalWindow* window, const WindowConfig& WNDConfig, Visual* visual, int32_t depth);
		//Creates a native cursor object from the specified image and hotspot
		static Cursor CreateCursorX11(const Scope<TRAP::Image>& image, int32_t xHotSpot, int32_t yHotSpot);
		//Returns whether the window is iconified
		static int32_t GetWindowState(const InternalWindow* window);
		//Convert XKB KeySym to Unicode
		static int64_t KeySymToUnicode(uint32_t keySym);
		struct CodePair 
		{
			uint16_t keySym;
			uint16_t UCS;
		};
		static const std::array<CodePair, 828> KeySymTab;
		//Encode a Unicode code point to a UTF-8 stream
		static std::size_t EncodeUTF8(char* s, uint32_t ch);
		static std::string GetSelectionString(Atom selection);
		//Returns whether it is a property event for the specified selection transfer
		static int32_t IsSelPropNewValueNotify(Display* display, XEvent* event, XPointer pointer);
		//Convert the specified Latin-1 string to UTF-8
		static std::string ConvertLatin1ToUTF8(const char* source);
		//Create the OpenGL context
		static bool CreateContextGLX(InternalWindow* window, const ContextConfig& CTXConfig, const FrameBufferConfig& FBConfig);
		//Reports the specified error, appending information about the last X error
		static void InputErrorX11(Error error, const char* message);
		static void MakeContextCurrentGLX(InternalWindow* window);
		static void SwapBuffersGLX(const InternalWindow* window);
		static void SwapIntervalGLX(int32_t interval);
		static void DestroyContextGLX(InternalWindow* window);
		//Process the specified X event
		static void ProcessEvent(XEvent& event);
		//Translates an X11 key code to a TRAP key token
		static Input::Key TranslateKey(int32_t scanCode);
#ifdef X_HAVE_UTF8_STRING
		//Decode a Unicode code point from a UTF-8 stream
		static uint32_t DecodeUTF8(const char** s);
#endif
		//Splits and translates a text/uri-list into separate file paths
		static std::vector<std::string> ParseUriList(char* text, int32_t& count);
		//Set the current video mode for the specified monitor
		static void SetVideoModeX11(InternalMonitor* monitor, const InternalVideoMode& desired);
		//Restore the saved(original) video mode for the specified monitor
		static void RestoreVideoModeX11(InternalMonitor* monitor);
		//Allocates and returns a monitor object with the specified name and dimensions
		static Scope<InternalMonitor> CreateMonitor(std::string name);
		//Creates a dummy window for behind-the-scenes work
		static ::Window CreateHelperWindow();
		//Translate the X11 KeySyms for a key to a TRAP key
		static Input::Key TranslateKeySyms(const KeySym* keySyms, int32_t width);
		//Clear its handle when the input context has been destroyed
		static void InputContextDestroyCallback(XIC ic, XPointer clientData, XPointer callData);
		//Translate an X11 key code to a TRAP key
#endif
	};
}
#endif /*_TRAP_WINDOWINGAPI_H_*/