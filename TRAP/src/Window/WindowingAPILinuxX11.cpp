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

#include "TRAPPCH.h"

#include "Core/PlatformDetection.h"

#ifdef TRAP_PLATFORM_LINUX

#include "WindowingAPI.h"
#include "Application.h"
#include "Utils/Utils.h"
#include "Utils/DynamicLoading/DynamicLoading.h"

//Action for EWMH client messages
static constexpr int32_t _NET_WM_STATE_REMOVE = 0;
static constexpr int32_t _NET_WM_STATE_ADD = 1;

//Additional mouse button names for XButtonEvent
static constexpr int32_t Button6 = 6;
static constexpr int32_t Button7 = 7;

//Motif WM hints flags
static constexpr int32_t MWM_HINTS_DECORATIONS = 2;
static constexpr int32_t MWM_DECOR_ALL = 1;

static constexpr int32_t TRAP_XDND_VERSION = 5;

//Atom values
static constexpr int32_t None = 0;

static constexpr int32_t KeyPress = 2;
static constexpr int32_t KeyRelease = 3;

static constexpr uint32_t InvalidCodepoint = 0xFFFFFFFFu;

//Calculates the refresh rate, in Hz, from the specified RandR mode info
int32_t TRAP::INTERNAL::WindowingAPI::CalculateRefreshRate(const XRRModeInfo* mi)
{
	if(mi->hTotal && mi->vTotal)
	{
		return static_cast<int32_t>(TRAP::Math::Round(static_cast<double>(mi->dotClock) /
		                                              (static_cast<double>(mi->hTotal) *
													  static_cast<double>(mi->vTotal))));
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::VideoModeFromModeInfo(const XRRModeInfo* mi,
                                                                                                    const XRRCrtcInfo* ci)
{
	InternalVideoMode mode{};

	if(ci->rotation == RR_Rotate_90 || ci->rotation == RR_Rotate_270)
	{
		mode.Width = static_cast<int32_t>(mi->height);
		mode.Height = static_cast<int32_t>(mi->width);
	}
	else
	{
		mode.Width = static_cast<int32_t>(mi->width);
		mode.Height = static_cast<int32_t>(mi->height);
	}

	mode.RefreshRate = CalculateRefreshRate(mi);

	SplitBPP(DefaultDepth(s_Data.display, s_Data.Screen), mode.RedBits, mode.GreenBits, mode.BlueBits);

	return mode;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sends an EWMH or ICCCM event to the window manager
void TRAP::INTERNAL::WindowingAPI::SendEventToWM(const InternalWindow* window, const Atom type, const int64_t a,
												 const int64_t b, const int64_t c, const int64_t d, const int64_t e)
{
	XEvent event = { ClientMessage };
	event.xclient.window = window->Handle;
	event.xclient.format = 32; //Data is 32-bit longs
	event.xclient.message_type = type;
	event.xclient.data.l[0] = a;
	event.xclient.data.l[1] = b;
	event.xclient.data.l[2] = c;
	event.xclient.data.l[3] = d;
	event.xclient.data.l[4] = e;

	s_Data.XLIB.SendEvent(s_Data.display, s_Data.Root, 0, SubstructureNotifyMask | SubstructureRedirectMask,
	                      &event);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether it is a _NET_FRAME_EXTENTS event for the specified window
bool TRAP::INTERNAL::WindowingAPI::IsFrameExtentsEvent(Display*, XEvent* event, XPointer pointer)
{
	const InternalWindow* window = reinterpret_cast<InternalWindow*>(pointer);

	return event->type             == PropertyNotify   &&
	       event->xproperty.state  == PropertyNewValue &&
		   event->xproperty.window == window->Handle   &&
		   event->xproperty.atom   == s_Data.NET_FRAME_EXTENTS;
}

//-------------------------------------------------------------------------------------------------------------------//

//Wait for data to arrive on any of the specified file descriptors
bool TRAP::INTERNAL::WindowingAPI::WaitForData(pollfd* fds, const nfds_t count, double* timeout)
{
	while(true)
	{
		if(timeout)
		{
			const uint64_t base = static_cast<uint64_t>(TRAP::Application::GetTime());

#if defined(__linux__) || defined(__FreeBDS__) || defined(__OpenBSD__) || defined(__CYGWIN__)
			const time_t seconds = static_cast<time_t>(*timeout);
			const int64_t nanoseconds = static_cast<int64_t>((*timeout - static_cast<double>(seconds)) * 1e9);
			const timespec ts = {seconds, nanoseconds};
			const int32_t result = ppoll(fds, count, &ts, nullptr);
#elif defined(__NetBSD__)
			const time_t seconds = static_cast<time_t>(*timeout);
			const long nanoseconds = static_cast<long>((*timeout - seconds) * 1e9);
			const timespec ts = {seconds, nanoseconds};
			const int32_t result = pollts(fds, count, &ts, nullptr);
#else
			const int milliseconds = static_cast<int>(*timeout * 1e3);
			const int32_t result = poll(fds, count, milliseconds);
#endif

			const int32_t error = errno;

			*timeout -= (TRAP::Application::GetTime() - static_cast<float>(base)) / 60.0;

			if(result > 0)
				return true;
			if((result == -1 && error != EINTR && error != EAGAIN) || *timeout <= 0.0)
				return false;
		}
		else
		{
			const int result = poll(fds, count, -1);
			if(result > 0)
				return true;
			if(result == -1 && errno != EINTR && errno != EAGAIN)
				return false;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::WaitForX11Event(double* timeout)
{
	pollfd fd = {ConnectionNumber(s_Data.display), POLLIN, 0};

	while(!s_Data.XLIB.Pending(s_Data.display))
	{
		if(!WaitForData(&fd, 1, timeout))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::WriteEmptyEvent()
{
	while(true)
	{
		const char byte = 0;
		const ssize_t result = write(s_Data.EmptyEventPipe[1], &byte, 1);
		if(result == 1 || (result == -1 && errno != EINTR))
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DrainEmptyEvents()
{
	while(true)
	{
		std::array<char, 64> dummy{};
		const ssize_t result = read(s_Data.EmptyEventPipe[0], dummy.data(), dummy.size());
		if(result == -1 && errno != EINTR)
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::CreateEmptyEventPipe()
{
	if(pipe(s_Data.EmptyEventPipe.data()) != 0)
	{
		InputError(Error::Platform_Error, "X11: Failed to create empty event pipe: " + Utils::String::GetStrError());
		return false;
	}

	for(int& fd : s_Data.EmptyEventPipe)
	{
		const int sf = fcntl(fd, F_GETFL, 0);
		const int df = fcntl(fd, F_GETFD, 0);

		if(sf == -1 || df == -1 ||
		   fcntl(fd, F_SETFL, sf | O_NONBLOCK) == -1 ||
		   fcntl(fd, F_SETFD, df | FD_CLOEXEC) == -1)
		{
			InputError(Error::Platform_Error, "X11: Failed to set flags for empty event pipe: " + Utils::String::GetStrError());
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieve a single window property of the specified type
uint64_t TRAP::INTERNAL::WindowingAPI::GetWindowPropertyX11(::Window window, const Atom property, const Atom type,
                                                            uint8_t** value)
{
	Atom actualType{};
	int32_t actualFormat{};
	uint64_t itemCount{}, bytesAfter{};

	s_Data.XLIB.GetWindowProperty(s_Data.display, window, property, 0, std::numeric_limits<long>::max(), 0, type, &actualType,
					              &actualFormat, &itemCount, &bytesAfter, value);

	return itemCount;
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the normal hints according to the window settings
void TRAP::INTERNAL::WindowingAPI::UpdateNormalHints(InternalWindow* window, const int32_t width, const int32_t height)
{
	XSizeHints* hints = s_Data.XLIB.AllocSizeHints();

	long supplied;
	s_Data.XLIB.GetWMNormalHints(s_Data.display, window->Handle, hints, &supplied);
	hints->flags &= ~(PMinSize | PMaxSize | PAspect);

	if(!window->Monitor)
	{
		if(window->Resizable)
		{
			if(window->MinWidth != -1 && window->MinHeight != -1)
			{
				hints->flags |= PMinSize;
				hints->min_width = window->MinWidth;
				hints->min_height = window->MinHeight;
			}

			if(window->MaxWidth != -1 && window->MaxHeight != -1)
			{
				hints->flags |= PMaxSize;
				hints->max_width = window->MaxWidth;
				hints->max_height = window->MaxHeight;
			}
		}
		else
		{
			hints->flags |= (PMinSize | PMaxSize);
			hints->min_width = hints->max_width = width;
			hints->min_height = hints->max_height = height;
		}
	}

	s_Data.XLIB.SetWMNormalHints(s_Data.display, window->Handle, hints);
	s_Data.XLIB.Free(hints);
}

//-------------------------------------------------------------------------------------------------------------------//

//Waits until a VisibilityNotify event arrives for the specified window or the timeout period elapses
bool TRAP::INTERNAL::WindowingAPI::WaitForVisibilityNotify(InternalWindow* window)
{
	XEvent dummy;
	double timeout = 0.1;

	while(!s_Data.XLIB.CheckTypedWindowEvent(s_Data.display, window->Handle, VisibilityNotify, &dummy))
	{
		if(!WaitForX11Event(&timeout))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the full screen status of the window
void TRAP::INTERNAL::WindowingAPI::UpdateWindowMode(InternalWindow* window)
{
	if(window->Monitor)
	{
		if(s_Data.Xinerama.Available && s_Data.NET_WM_FULLSCREEN_MONITORS)
		{
			SendEventToWM(window,
			              s_Data.NET_WM_FULLSCREEN_MONITORS,
						  window->Monitor->Index,
						  window->Monitor->Index,
						  window->Monitor->Index,
						  window->Monitor->Index,
						  0);
		}

		if(s_Data.NET_WM_STATE && s_Data.NET_WM_STATE_FULLSCREEN)
		{
			SendEventToWM(window,
						  s_Data.NET_WM_STATE,
						  _NET_WM_STATE_ADD,
						  static_cast<int64_t>(s_Data.NET_WM_STATE_FULLSCREEN),
						  0, 1, 0);
		}
		else
		{
			//This is the butcher's way of removing window decorations
			//Setting the override-redirect attribute on a window makes the window manager ignore the
			//window completely
			//The good thing is that this makes undecorated full screen windows easy to do
			//The bad thing is that we have to do everything manually and some things (like minimize/restore)
			//won't work at all, as those are tasks usually performed by the window manager

			XSetWindowAttributes attributes;
			attributes.override_redirect = 1;
			s_Data.XLIB.ChangeWindowAttributes(s_Data.display, window->Handle, CWOverrideRedirect, &attributes);

			window->OverrideRedirect = true;
		}

		//Enable compositor bypass
		if(!window->Transparent)
		{
			const uint64_t value = 1;

			s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_BYPASS_COMPOSITOR, XA_CARDINAL,
			                           32, PropModeReplace, reinterpret_cast<const uint8_t*>(&value), 1);
		}
	}
	else
	{
		if(s_Data.Xinerama.Available && s_Data.NET_WM_FULLSCREEN_MONITORS)
			s_Data.XLIB.DeleteProperty(s_Data.display, window->Handle, s_Data.NET_WM_FULLSCREEN_MONITORS);

		if(s_Data.NET_WM_STATE && s_Data.NET_WM_STATE_FULLSCREEN)
		{
			SendEventToWM(window,
						  s_Data.NET_WM_STATE,
						  _NET_WM_STATE_REMOVE,
						  static_cast<int64_t>(s_Data.NET_WM_STATE_FULLSCREEN),
						  0, 1, 0);
		}
		else
		{
			XSetWindowAttributes attributes;
			attributes.override_redirect = 0;
			s_Data.XLIB.ChangeWindowAttributes(s_Data.display, window->Handle, CWOverrideRedirect, &attributes);

			window->OverrideRedirect = false;
		}

		//Disable compositor bypass
		if(!window->Transparent)
			s_Data.XLIB.DeleteProperty(s_Data.display, window->Handle, s_Data.NET_WM_BYPASS_COMPOSITOR);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the mode info for a RandR mode XID
const XRRModeInfo* TRAP::INTERNAL::WindowingAPI::GetModeInfo(const XRRScreenResources* sr, const RRMode id)
{
	for(int32_t i = 0; i < sr->nmode; i++)
	{
		if(sr->modes[i].id == id)
			return sr->modes + i;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieve system content scale via folklore heuristics
void TRAP::INTERNAL::WindowingAPI::GetSystemContentScale(float& xScale, float& yScale)
{
	//Start by assuming the default X11 DPI
	//NOTE: Some desktop environments (KDE) may remove the Xft.dpi field when it would be set to 96, so assume
	//      that is the case if we cannot find it
	float xDPI = 96.0f, yDPI = 96.0f;

	//NOTE: Basing the scale on Xft.dpi where available should provide the most consistent user experience
	//      (matches Qt, Gtk, etc), although not always the most accurate one
	const char* rms = s_Data.XLIB.ResourceManagerString(s_Data.display);
	if(rms)
	{
		const XrmDatabase db = s_Data.XRM.GetStringDatabase(rms);
		if(db)
		{
			XrmValue value;
			char* type = nullptr;

			if(s_Data.XRM.GetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value))
			{
				if(type && strcmp(type, "String") == 0)
					xDPI = yDPI = static_cast<float>(std::atof(value.addr));
			}

			s_Data.XRM.DestroyDatabase(db);
		}
	}

	xScale = xDPI / 96.0f;
	yScale = yDPI / 96.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

//Look for and initialize supported X11 extensions
bool TRAP::INTERNAL::WindowingAPI::InitExtensions()
{
#if defined(__CYGWIN__)
	s_Data.XI.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXi-6.so");
#elif defined (__OpenBSD__) || defined(__NetBSD__)
	s_Data.XI.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXi.so");
#else
	s_Data.XI.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXi.so.6");
#endif
	if(s_Data.XI.Handle)
	{
		s_Data.XI.QueryVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XIQueryVersion>(s_Data.XI.Handle, "XIQueryVersion");
		s_Data.XI.SelectEvents = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XISelectEvents>(s_Data.XI.Handle, "XISelectEvents");

		if(s_Data.XLIB.QueryExtension(s_Data.display,
		                              "XInputExtension",
						              &s_Data.XI.MajorOPCode,
						              &s_Data.XI.EventBase,
						              &s_Data.XI.ErrorBase))
		{
			s_Data.XI.Major = 2;
			s_Data.XI.Minor = 0;

			if(s_Data.XI.QueryVersion(s_Data.display, &s_Data.XI.Major, &s_Data.XI.Minor) == 0) //0 = Success
				s_Data.XI.Available = true;
		}
	}

#if defined(__CYGWIN__)
	s_Data.RandR.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrandr-2.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.RandR.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrandr.so");
#else
	s_Data.RandR.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrandr.so.2");
#endif
	if(s_Data.RandR.Handle)
	{
		s_Data.RandR.FreeCrtcInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRFreeCrtcInfo>(s_Data.RandR.Handle,
		                                                                                               "XRRFreeCrtcInfo");
		s_Data.RandR.FreeOutputInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRFreeOutputInfo>(s_Data.RandR.Handle,
		                                                                                                   "XRRFreeOutputInfo");
		s_Data.RandR.FreeScreenResources = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRFreeScreenResources>(s_Data.RandR.Handle,
		                                                                                                             "XRRFreeScreenResources");
		s_Data.RandR.GetCrtcInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRGetCrtcInfo>(s_Data.RandR.Handle,
		                                                                                             "XRRGetCrtcInfo");
		s_Data.RandR.GetOutputInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRGetOutputInfo>(s_Data.RandR.Handle,
		                                                                                                 "XRRGetOutputInfo");
		s_Data.RandR.GetOutputPrimary = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRGetOutputPrimary>(s_Data.RandR.Handle,
		                                                                                                       "XRRGetOutputPrimary");
		s_Data.RandR.GetScreenResourcesCurrent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRGetScreenResourcesCurrent>(s_Data.RandR.Handle,
		 																														 "XRRGetScreenResourcesCurrent");
		s_Data.RandR.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRQueryExtension>(s_Data.RandR.Handle,
		                                                                                                   "XRRQueryExtension");
		s_Data.RandR.QueryVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRQueryVersion>(s_Data.RandR.Handle,
		                                                                                               "XRRQueryVersion");
		s_Data.RandR.SelectInput = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRSelectInput>(s_Data.RandR.Handle,
		                                                                                             "XRRSelectInput");
		s_Data.RandR.SetCrtcConfig = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRSetCrtcConfig>(s_Data.RandR.Handle,
		                                                                                                 "XRRSetCrtcConfig");
		s_Data.RandR.UpdateConfiguration = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRUpdateConfiguration>(s_Data.RandR.Handle,
																													 "XRRUpdateConfiguration");

		if(s_Data.RandR.QueryExtension(s_Data.display, &s_Data.RandR.EventBase, &s_Data.RandR.ErrorBase))
		{
			if(s_Data.RandR.QueryVersion(s_Data.display, &s_Data.RandR.Major, &s_Data.RandR.Minor))
			{
				//The TRAP RandR path requires at least verison 1.3
				if(s_Data.RandR.Major > 1 || s_Data.RandR.Minor >= 3)
					s_Data.RandR.Available = true;
			}
			else
				InputError(Error::Platform_Error, "[X11] Failed to query RandR version!");
		}
	}

	if(s_Data.RandR.Available)
	{
		XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);

		if(!sr->ncrtc)
		{
			//A system without CRTCs is likely a system with borken RandR
			//Disable the RandR monitor path and fall back to core functions
			s_Data.RandR.MonitorBroken = true;
		}

		s_Data.RandR.FreeScreenResources(sr);
	}

	if(s_Data.RandR.Available && !s_Data.RandR.MonitorBroken)
		s_Data.RandR.SelectInput(s_Data.display, s_Data.Root, RROutputChangeNotifyMask);

#if defined(__CYGWIN__)
	s_Data.XCursor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXcursor-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.XCursor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXcursor.so");
#else
	s_Data.XCursor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXcursor.so.1");
#endif
	if(s_Data.XCursor.Handle)
	{
		s_Data.XCursor.ImageCreate = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorImageCreate>(s_Data.XCursor.Handle,
		                                                                                                   "XcursorImageCreate");
		s_Data.XCursor.ImageDestroy = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorImageDestroy>(s_Data.XCursor.Handle,
		                                                                                                     "XcursorImageDestroy");
		s_Data.XCursor.ImageLoadCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorImageLoadCursor>(s_Data.XCursor.Handle,
																												   "XcursorImageLoadCursor");
		s_Data.XCursor.GetTheme = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorGetTheme>(s_Data.XCursor.Handle,
													                                                 "XcursorGetTheme");
		s_Data.XCursor.GetDefaultSize = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorGetDefaultSize>(s_Data.XCursor.Handle,
		                                                                                                         "XcursorGetDefaultSize");
		s_Data.XCursor.LibraryLoadImage = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorLibraryLoadImage>(s_Data.XCursor.Handle,
																													 "XcursorLibraryLoadImage");
	}

#if defined(__CYGWIN__)
	s_Data.Xinerama.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXinerama-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.Xinerama.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXinerama.so");
#else
	s_Data.Xinerama.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXinerama.so.1");
#endif
	if(s_Data.Xinerama.Handle)
	{
		s_Data.Xinerama.IsActive = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XineramaIsActive>(s_Data.Xinerama.Handle,
		                                                                                               "XineramaIsActive");
		s_Data.Xinerama.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XineramaQueryExtension>(s_Data.Xinerama.Handle,
																												   "XineramaQueryExtension");
		s_Data.Xinerama.QueryScreens = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XineramaQueryScreens>(s_Data.Xinerama.Handle,
		                                                                                					   "XineramaQueryScreens");

		if(s_Data.Xinerama.QueryExtension(s_Data.display, &s_Data.Xinerama.Major, &s_Data.Xinerama.Minor))
		{
			if(s_Data.Xinerama.IsActive(s_Data.display))
				s_Data.Xinerama.Available = true;
		}
	}

	s_Data.XKB.Major = 1;
	s_Data.XKB.Minor = 0;
	s_Data.XKB.Available = s_Data.XKB.QueryExtension(s_Data.display,
											         &s_Data.XKB.MajorOPCode,
											         &s_Data.XKB.EventBase,
											         &s_Data.XKB.ErrorBase,
											         &s_Data.XKB.Major,
											         &s_Data.XKB.Minor);

	if(s_Data.XKB.Available)
	{
		int32_t supported = 0;

		if(s_Data.XKB.SetDetectableAutoRepeat(s_Data.display, 1, &supported))
		{
			if(supported)
				s_Data.XKB.Detectable = true;
		}

		s_Data.XKB.Group = 0;
		XkbStateRec state;
		if(s_Data.XKB.GetState(s_Data.display, XkbUseCoreKbd, &state) == 0) //0 = Success
			s_Data.XKB.Group = static_cast<uint32_t>(state.group);

		s_Data.XKB.SelectEventDetails(s_Data.display, XkbUseCoreKbd, XkbStateNotify, XkbGroupStateMask,
		                              XkbGroupStateMask);
	}

#if defined(__CYGWIN__)
	s_Data.XCB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11-xcb-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.XCB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11-xcb.so");
#else
	s_Data.XCB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11-xcb.so.1");
#endif
	if(s_Data.XCB.Handle)
		s_Data.XCB.GetXCBConnection = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetXCBConnection>(s_Data.XCB.Handle,
		                                                                            					   "XGetXCBConnection");

#if defined(__CYGWIN__)
	s_Data.XRender.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrender-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.XRender.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrender.so");
#else
	s_Data.XRender.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrender.so.1");
#endif
	if(s_Data.XRender.Handle)
	{
		s_Data.XRender.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRenderQueryExtension>(s_Data.XRender.Handle,
		                                                                                                         "XRenderQueryExtension");
		s_Data.XRender.QueryVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRenderQueryVersion>(s_Data.XRender.Handle,
		                                                                                                     "XRenderQueryVersion");
		s_Data.XRender.FindVisualFormat = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRenderFindVisualFormat>(s_Data.XRender.Handle,
																													 "XRenderFindVisualFormat");

		if(s_Data.XRender.QueryExtension(s_Data.display, &s_Data.XRender.ErrorBase, &s_Data.XRender.EventBase))
		{
			if(s_Data.XRender.QueryVersion(s_Data.display, &s_Data.XRender.Major, &s_Data.XRender.Minor))
				s_Data.XRender.Available = true;
		}
	}

#if defined(__CYGWIN__)
	s_Data.XShape.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXext-6.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.XShape.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXext.so");
#else
	s_Data.XShape.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXext.so.6");
#endif
	if(s_Data.XShape.Handle)
	{
		s_Data.XShape.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XShapeQueryExtension>(s_Data.XShape.Handle,
		                                                                                                       "XShapeQueryExtension");
		s_Data.XShape.CombineRegion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XShapeCombineRegion>(s_Data.XShape.Handle,
		                                                                                                     "XShapeCombineRegion");
		s_Data.XShape.CombineMask = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XShapeCombineMask>(s_Data.XShape.Handle,
		                                                                                                 "XShapeCombineMask");
		s_Data.XShape.QueryVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XShapeQueryVersion>(s_Data.XShape.Handle,
		                                                                                                   "XShapeQueryVersion");

		if(s_Data.XShape.QueryExtension(s_Data.display, &s_Data.XShape.ErrorBase, &s_Data.XShape.EventBase))
		{
			if(s_Data.XShape.QueryVersion(s_Data.display, &s_Data.XShape.Major, &s_Data.XShape.Minor))
				s_Data.XShape.Available = true;
		}
	}

	//Update the key code LUT
	CreateKeyTables();

	//String format atoms
	s_Data.NULL_ =       s_Data.XLIB.InternAtom(s_Data.display, "NULL", 0);
	s_Data.UTF8_STRING = s_Data.XLIB.InternAtom(s_Data.display, "UTF8_STRING", 0);
	s_Data.ATOM_PAIR =   s_Data.XLIB.InternAtom(s_Data.display, "ATOM_PAIR", 0);

	//Custom selection property atom
	s_Data.TRAP_SELECTION = s_Data.XLIB.InternAtom(s_Data.display, "TRAP_SELECTION", 0);

	//ICCCM standard clipboard atoms
	s_Data.TARGETS =   s_Data.XLIB.InternAtom(s_Data.display, "TARGETS", 0);
	s_Data.MULTIPLE =  s_Data.XLIB.InternAtom(s_Data.display, "MULTIPLE", 0);
	s_Data.PRIMARY =   s_Data.XLIB.InternAtom(s_Data.display, "PRIMARY", 0);
	s_Data.INCR =      s_Data.XLIB.InternAtom(s_Data.display, "INCR", 0);
	s_Data.CLIPBOARD = s_Data.XLIB.InternAtom(s_Data.display, "CLIPBOARD", 0);

	//Clipboard manager atoms
	s_Data.CLIPBOARD_MANAGER = s_Data.XLIB.InternAtom(s_Data.display, "CLIPBOARD_MANAGER", 0);
	s_Data.SAVE_TARGETS =      s_Data.XLIB.InternAtom(s_Data.display, "SAVE_TARGETS", 0);

	//Xdnd (drag and drop) atoms
	s_Data.XDNDAware =      s_Data.XLIB.InternAtom(s_Data.display, "XdndAware", 0);
	s_Data.XDNDEnter =      s_Data.XLIB.InternAtom(s_Data.display, "XdndEnter", 0);
	s_Data.XDNDPosition =   s_Data.XLIB.InternAtom(s_Data.display, "XdndPosition", 0);
	s_Data.XDNDStatus =     s_Data.XLIB.InternAtom(s_Data.display, "XdndStatus", 0);
	s_Data.XDNDActionCopy = s_Data.XLIB.InternAtom(s_Data.display, "XdndActionCopy", 0);
	s_Data.XDNDDrop =       s_Data.XLIB.InternAtom(s_Data.display, "XdndDrop", 0);
	s_Data.XDNDFinished =   s_Data.XLIB.InternAtom(s_Data.display, "XdndFinished", 0);
	s_Data.XDNDSelection =  s_Data.XLIB.InternAtom(s_Data.display, "XdndSelection", 0);
	s_Data.XDNDTypeList =   s_Data.XLIB.InternAtom(s_Data.display, "XdndTypeList", 0);
	s_Data.text_uri_list =  s_Data.XLIB.InternAtom(s_Data.display, "text/uri-list", 0);

	//ICCCM, EWMH and Motif window property atoms
	//These can be set safely even without WM support
	//The EWMH atoms that require WM support are handled in detectEWMH
	s_Data.WM_PROTOCOLS =             s_Data.XLIB.InternAtom(s_Data.display, "WM_PROTOCOLS", 0);
	s_Data.WM_STATE =                 s_Data.XLIB.InternAtom(s_Data.display, "WM_STATE", 0);
	s_Data.WM_DELETE_WINDOW =         s_Data.XLIB.InternAtom(s_Data.display, "WM_DELETE_WINDOW", 0);
	s_Data.NET_SUPPORTED =            s_Data.XLIB.InternAtom(s_Data.display, "_NET_SUPPORTED", 0);
	s_Data.NET_SUPPORTING_WM_CHECK =  s_Data.XLIB.InternAtom(s_Data.display, "_NET_SUPPORTING_WM_CHECK", 0);
	s_Data.NET_WM_ICON =              s_Data.XLIB.InternAtom(s_Data.display, "_NET_WM_ICON", 0);
	s_Data.NET_WM_PING =              s_Data.XLIB.InternAtom(s_Data.display, "_NET_WM_PING", 0);
	s_Data.NET_WM_PID =               s_Data.XLIB.InternAtom(s_Data.display, "_NET_WM_PID", 0);
	s_Data.NET_WM_NAME =              s_Data.XLIB.InternAtom(s_Data.display, "_NET_WM_NAME", 0);
	s_Data.NET_WM_ICON_NAME =         s_Data.XLIB.InternAtom(s_Data.display, "_NET_WM_ICON_NAME", 0);
	s_Data.NET_WM_BYPASS_COMPOSITOR = s_Data.XLIB.InternAtom(s_Data.display, "_NET_WM_BYPASS_COMPOSITOR", 0);
	s_Data.NET_WM_WINDOW_OPACITY =    s_Data.XLIB.InternAtom(s_Data.display, "_NET_WM_WINDOW_OPACITY", 0);
	s_Data.MOTIF_WM_HINTS =           s_Data.XLIB.InternAtom(s_Data.display, "_MOTIF_WM_HINTS", 0);

	//The compositing manager selection name contains the screen number
	s_Data.NET_WM_CM_Sx = s_Data.XLIB.InternAtom(s_Data.display,
	                                             std::string("_NET_WM_CM_S" + std::to_string(s_Data.Screen)).c_str(),
												 0);

	//Detect whether an EWMH-conformant window manager is running
	DetectEWMH();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Check whether the running window manager is EMWH-compliant
void TRAP::INTERNAL::WindowingAPI::DetectEWMH()
{
	//First we read the _NET_SUPPORTING_WM_CHECK property on the root window
	::Window* windowFromRoot = nullptr;
	if(!GetWindowPropertyX11(s_Data.Root, s_Data.NET_SUPPORTING_WM_CHECK, XA_WINDOW,
	                         reinterpret_cast<uint8_t**>(&windowFromRoot)))
		return;

	GrabErrorHandlerX11();

	//If it exists, it should be the XID of a top-level window
	//Then we look for the same property on that window
	::Window* windowFromChild = nullptr;
	if(!GetWindowPropertyX11(*windowFromRoot, s_Data.NET_SUPPORTING_WM_CHECK, XA_WINDOW,
	                         reinterpret_cast<uint8_t**>(&windowFromChild)))
	{
		s_Data.XLIB.Free(windowFromRoot);
		return;
	}

	ReleaseErrorHandlerX11();

	//If the property exists, it should contain the XID of the window
	if(*windowFromRoot != *windowFromChild)
	{
		s_Data.XLIB.Free(windowFromRoot);
		s_Data.XLIB.Free(windowFromChild);
		return;
	}

	s_Data.XLIB.Free(windowFromRoot);
	s_Data.XLIB.Free(windowFromChild);

	//We are now fairly sure that an EWMH-compliant WM is currently running
	//We can now start querying the WM about what features it supports by looking in the _NET_SUPPORTED property
	//on the root window
	//It should contain a list of supported EWMH protocl and state atoms

	Atom* supportedAtoms = nullptr;
	const uint64_t atomCount = GetWindowPropertyX11(s_Data.Root, s_Data.NET_SUPPORTED, XA_ATOM,
	                                                     reinterpret_cast<uint8_t**>(&supportedAtoms));

	//See which of the atoms we support that are supported by the WM

	s_Data.NET_WM_STATE = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE");
    s_Data.NET_WM_STATE_ABOVE = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_ABOVE");
    s_Data.NET_WM_STATE_FULLSCREEN = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_FULLSCREEN");
    s_Data.NET_WM_STATE_MAXIMIZED_VERT = GetAtomIfSupported(supportedAtoms, atomCount,
	                                                        "_NET_WM_STATE_MAXIMIZED_VERT");
    s_Data.NET_WM_STATE_MAXIMIZED_HORZ = GetAtomIfSupported(supportedAtoms, atomCount,
	                                                        "_NET_WM_STATE_MAXIMIZED_HORZ");
    s_Data.NET_WM_STATE_DEMANDS_ATTENTION = GetAtomIfSupported(supportedAtoms, atomCount,
	                                                           "_NET_WM_STATE_DEMANDS_ATTENTION");
    s_Data.NET_WM_FULLSCREEN_MONITORS = GetAtomIfSupported(supportedAtoms, atomCount,
	                                                       "_NET_WM_FULLSCREEN_MONITORS");
    s_Data.NET_WM_WINDOW_TYPE = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_WINDOW_TYPE");
    s_Data.NET_WM_WINDOW_TYPE_NORMAL = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_WINDOW_TYPE_NORMAL");
    s_Data.NET_WORKAREA = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_WORKAREA");
    s_Data.NET_CURRENT_DESKTOP = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_CURRENT_DESKTOP");
    s_Data.NET_ACTIVE_WINDOW = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_ACTIVE_WINDOW");
    s_Data.NET_FRAME_EXTENTS = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_FRAME_EXTENTS");
    s_Data.NET_REQUEST_FRAME_EXTENTS = GetAtomIfSupported(supportedAtoms, atomCount, "_NET_REQUEST_FRAME_EXTENTS");

	if(supportedAtoms)
		s_Data.XLIB.Free(supportedAtoms);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the X error handler callback
void TRAP::INTERNAL::WindowingAPI::GrabErrorHandlerX11()
{
	TRAP_ASSERT(s_Data.PrevErrorHandler == nullptr);

	s_Data.ErrorCode = 0; //0 = Success
	s_Data.PrevErrorHandler = s_Data.XLIB.SetErrorHandler(ErrorHandler);
}

//-------------------------------------------------------------------------------------------------------------------//

//X error handler
int32_t TRAP::INTERNAL::WindowingAPI::ErrorHandler(Display* display, XErrorEvent* event)
{
	if(s_Data.display != display)
		return 0;

	s_Data.ErrorCode = event->error_code;
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

//Clears the X error handler callback
void TRAP::INTERNAL::WindowingAPI::ReleaseErrorHandlerX11()
{
	//Synchronize to make sure all commands are processed
	s_Data.XLIB.Sync(s_Data.display, 0);
	s_Data.XLIB.SetErrorHandler(s_Data.PrevErrorHandler);
	s_Data.PrevErrorHandler = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Return the atom ID only if it is listed in the specified array
Atom TRAP::INTERNAL::WindowingAPI::GetAtomIfSupported(const Atom* supportedAtoms, const uint64_t atomCount,
                                                      const std::string_view atomName)
{
	const Atom atom = s_Data.XLIB.InternAtom(s_Data.display, atomName.data(), 0);

	for(uint64_t i = 0; i < atomCount; i++)
	{
		if(supportedAtoms[i] == atom)
			return atom;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create a blank cursor for hidden and disabled cursor modes
Cursor TRAP::INTERNAL::WindowingAPI::CreateHiddenCursor()
{
	const std::array<uint8_t, static_cast<std::size_t>(16) * 16 * 4> pixels = {0};
	return CreateCursorX11(TRAP::Image::LoadFromMemory(16, 16, TRAP::Image::ColorFormat::RGBA,
	                                                           std::vector<uint8_t>(pixels.begin(),
															   pixels.end())).get(), 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

//Check whether the IM has a usable style
bool TRAP::INTERNAL::WindowingAPI::HasUsableInputMethodStyle()
{
	bool found = false;
	XIMStyles* styles = nullptr;

	if(s_Data.XLIB.GetIMValues(s_Data.IM, XNQueryInputStyle, &styles, nullptr) != nullptr)
		return false;

	for(uint32_t i = 0; i < styles->count_styles; i++)
	{
		if(styles->supported_styles[i] == (XIMPreeditNothing | XIMStatusNothing))
		{
			found = true;
			break;
		}
	}

	s_Data.XLIB.Free(styles);
	return found;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputMethodDestroyCallback(XIM, XPointer, XPointer)
{
	s_Data.IM = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputMethodInstantiateCallback(Display*, XPointer, XPointer)
{
	if(s_Data.IM)
		return;

	s_Data.IM = s_Data.XLIB.OpenIM(s_Data.display, nullptr, nullptr, nullptr);
	if(s_Data.IM)
	{
		if(!HasUsableInputMethodStyle())
		{
			s_Data.XLIB.CloseIM(s_Data.IM);
			s_Data.IM = nullptr;
		}
	}

	if(s_Data.IM)
	{
		XIMCallback callback{};
		callback.callback = static_cast<XIMProc>(InputMethodDestroyCallback);
		callback.client_data = nullptr;
		s_Data.XLIB.SetIMValues(s_Data.IM, XNDestroyCallback, &callback, nullptr);

		for(InternalWindow* window  : s_Data.WindowList)
			CreateInputContextX11(window);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Poll for changes in the set of connected monitors
void TRAP::INTERNAL::WindowingAPI::PollMonitorsX11()
{
	if(!s_Data.RandR.Available || s_Data.RandR.MonitorBroken)
	{
		InputMonitor(CreateMonitor("Display"), true, 0);
		return;
	}

	int32_t disconnectedCount = 0, screenCount = 0;
	std::vector<InternalMonitor*> disconnected{};
	XineramaScreenInfo* screens = nullptr;
	XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);
	const RROutput primary = s_Data.RandR.GetOutputPrimary(s_Data.display, s_Data.Root);

	if(s_Data.Xinerama.Available)
		screens = s_Data.Xinerama.QueryScreens(s_Data.display, &screenCount);

	disconnectedCount = static_cast<int32_t>(s_Data.Monitors.size());
	if(disconnectedCount)
	{
		disconnected.resize(s_Data.Monitors.size());
		for(uint32_t i = 0; i < s_Data.Monitors.size(); i++)
			disconnected[i] = s_Data.Monitors[i].get();
	}

	for(int32_t i = 0; i < sr->noutput; i++)
	{
		int32_t j = 0, type = 0;

		XRROutputInfo* oi = s_Data.RandR.GetOutputInfo(s_Data.display, sr, sr->outputs[i]);
		if(oi->connection != RR_Connected || oi->crtc == 0)
		{
			s_Data.RandR.FreeOutputInfo(oi);
			continue;
		}

		XRRCrtcInfo* ci = s_Data.RandR.GetCrtcInfo(s_Data.display, sr, oi->crtc);

		for(j = 0; j < disconnectedCount; j++)
		{
			if(disconnected[j] && disconnected[j]->Output == sr->outputs[i])
			{
				disconnected[j] = nullptr;
				break;
			}
		}

		if(j < disconnectedCount)
		{
			s_Data.RandR.FreeOutputInfo(oi);
			continue;
		}

		Scope<InternalMonitor> monitor = CreateMonitor(oi->name);
		monitor->Output = sr->outputs[i];
		monitor->CRTC = oi->crtc;

		for(j = 0; j < screenCount; j++)
		{
			if(screens[j].x_org == ci->x && screens[j].y_org == ci->y &&
			   static_cast<uint32_t>(screens[j].width) == ci->width &&
			   static_cast<uint32_t>(screens[j].height) == ci->height)
			{
				monitor->Index = j;
				break;
			}
		}

		if(monitor->Output == primary)
			type = 0;
		else
			type = 1;

		InputMonitor(std::move(monitor), true, type);

		s_Data.RandR.FreeOutputInfo(oi);
		s_Data.RandR.FreeCrtcInfo(ci);
	}

	s_Data.RandR.FreeScreenResources(sr);

	if(screens)
		s_Data.XLIB.Free(screens);

	for(int32_t i = 0; i < disconnectedCount; i++)
	{
		if(disconnected[i])
			InputMonitorDisconnect(i, 0);
	}

	disconnected.clear();
}

//-------------------------------------------------------------------------------------------------------------------//


//Returns whether the event is a selection event
int32_t TRAP::INTERNAL::WindowingAPI::IsSelectionEvent(Display*, XEvent* event, XPointer)
{
	if(event->xany.window != s_Data.HelperWindowHandle)
		return 0;

	return event->type == SelectionRequest ||
	       event->type == SelectionNotify ||
		   event->type == SelectionClear;
}

//-------------------------------------------------------------------------------------------------------------------//

//Set the specified property to the selection converted to the requested target
Atom TRAP::INTERNAL::WindowingAPI::WriteTargetToProperty(const XSelectionRequestEvent* request)
{
	std::string selectionString{};
	const std::array<Atom, 2> formats{s_Data.UTF8_STRING, XA_STRING};

	if(request->selection == s_Data.PRIMARY)
		selectionString = s_Data.PrimarySelectionString;
	else
		selectionString = s_Data.ClipboardString;

	if(request->property == None)
	{
		//The requestor is a legacy client
		//We don't support legacy clients, so fail here
		return None;
	}

	if(request->target == s_Data.TARGETS)
	{
		//The list of supported targets was requested
		std::array<Atom, 4> targets{s_Data.TARGETS, s_Data.MULTIPLE, s_Data.UTF8_STRING, XA_STRING};

		s_Data.XLIB.ChangeProperty(s_Data.display, request->requestor, request->property, XA_ATOM, 32,
		                           PropModeReplace, reinterpret_cast<uint8_t*>(targets.data()),
								   targets.size());

		return request->property;
	}

	if(request->target == s_Data.MULTIPLE)
	{
		//Multiple conversions were requested

		Atom* targets = nullptr;

		const uint64_t count = GetWindowPropertyX11(request->requestor, request->property, s_Data.ATOM_PAIR,
		                                            reinterpret_cast<uint8_t**>(&targets));

		for(uint64_t i = 0; i < count; i += 2)
		{
			uint32_t j = 0;

			for(j = 0; j < formats.size(); j++)
			{
				if(targets[i] == formats[j])
					break;
			}

			if(j < formats.size())
				s_Data.XLIB.ChangeProperty(s_Data.display, request->requestor, targets[i + 1], targets[i], 8,
				                           PropModeReplace, reinterpret_cast<uint8_t*>(selectionString.data()),
										   static_cast<int>(selectionString.size()));
			else
				targets[i + 1] = None;
		}

		s_Data.XLIB.ChangeProperty(s_Data.display, request->requestor, request->property, s_Data.ATOM_PAIR, 32,
                                   PropModeReplace, reinterpret_cast<uint8_t*>(targets), static_cast<int>(count));

		s_Data.XLIB.Free(targets);

		return request->property;
	}

	if(request->target == s_Data.SAVE_TARGETS)
	{
		//The request is a check whether we support SAVE_TARGETS
		//It should be handled as a no-op side effect target

		s_Data.XLIB.ChangeProperty(s_Data.display, request->requestor, request->property, s_Data.NULL_, 32,
		                           PropModeReplace, nullptr, 0);

		return request->property;
	}

	//Conversion to a data target was requested

	for(Atom format : formats)
	{
		if(request->target == format)
		{
			//The requested target is one we support

			s_Data.XLIB.ChangeProperty(s_Data.display, request->requestor, request->property, request->target, 8,
						               PropModeReplace, reinterpret_cast<uint8_t*>(selectionString.data()),
									   static_cast<int>(selectionString.size()));

			return request->property;
		}
	}

	//The request target is not supported

	return None;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::HandleSelectionRequest(XEvent& event)
{
	const XSelectionRequestEvent* request = &event.xselectionrequest;

	XEvent reply = {SelectionNotify};
	reply.xselection.property = WriteTargetToProperty(request);
	reply.xselection.display = request->display;
	reply.xselection.requestor = request->requestor;
	reply.xselection.selection = request->selection;
	reply.xselection.target = request->target;
	reply.xselection.time = request->time;

	s_Data.XLIB.SendEvent(s_Data.display, request->requestor, 0, 0, &reply);
}

//-------------------------------------------------------------------------------------------------------------------//

//Push contents of our selection to clipboard manager
void TRAP::INTERNAL::WindowingAPI::PushSelectionToManagerX11()
{
	s_Data.XLIB.ConvertSelection(s_Data.display, s_Data.CLIPBOARD_MANAGER, s_Data.SAVE_TARGETS, None,
	                             s_Data.HelperWindowHandle, CurrentTime);

	while(true)
	{
		XEvent event;

		while(s_Data.XLIB.CheckIfEvent(s_Data.display, &event, IsSelectionEvent, nullptr))
		{
			switch(event.type)
			{
			case SelectionRequest:
				HandleSelectionRequest(event);
				break;

			case SelectionNotify:
				if(event.xselection.target == s_Data.SAVE_TARGETS)
				{
					//This means one of two things.
					//Either the selection was not owned, which means there is no clipboard manager,
					//or the transfer to the clipboard manager has completed
					//In either case, it means we are done here
					return;
				}
				break;
			}
		}

		WaitForX11Event(nullptr);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CreateInputContextX11(InternalWindow* window)
{
	XIMCallback callback;
	callback.callback = reinterpret_cast<XIMProc>(InputContextDestroyCallback);
	callback.client_data = reinterpret_cast<XPointer>(window);

	window->IC = s_Data.XLIB.CreateIC(s_Data.IM,
		                              XNInputStyle,
		                              XIMPreeditNothing | XIMStatusNothing,
		                              XNClientWindow,
		                              window->Handle,
		                              XNFocusWindow,
		                              window->Handle,
		                              XNDestroyCallback,
		                              &callback,
		                              nullptr);

	if(!window->IC)
		return;

	XWindowAttributes attribs;
	s_Data.XLIB.GetWindowAttributes(s_Data.display, window->Handle, &attribs);

	uint64_t filter = 0;
	if(s_Data.XLIB.GetICValues(window->IC, XNFilterEvents, &filter, nullptr) == nullptr)
		s_Data.XLIB.SelectInput(s_Data.display, window->Handle, static_cast<int64_t>(attribs.your_event_mask | filter));
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::IsVisualTransparentX11(Visual* visual)
{
	if(!s_Data.XRender.Available)
		return false;

	const XRenderPictFormat* pf = s_Data.XRender.FindVisualFormat(s_Data.display, visual);
	return pf && pf->direct.alphaMask;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create the X11 window (and its colormap)
bool TRAP::INTERNAL::WindowingAPI::CreateNativeWindow(InternalWindow* window, WindowConfig& WNDConfig,
                                                      Visual* visual, int32_t depth)
{
	const int32_t width = static_cast<int32_t>(WNDConfig.Width);
	const int32_t height = static_cast<int32_t>(WNDConfig.Height);

	//Create a colormap based on the visual used by the current context
	window->colormap = s_Data.XLIB.CreateColormap(s_Data.display, s_Data.Root, visual, AllocNone);

	window->Transparent = IsVisualTransparentX11(visual);

	XSetWindowAttributes wa = {};
	wa.colormap = window->colormap;
	wa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
	                PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
					ExposureMask | FocusChangeMask | VisibilityChangeMask |
					EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

	GrabErrorHandlerX11();

	window->Parent = s_Data.Root;
	window->Handle = s_Data.XLIB.CreateWindow(s_Data.display, s_Data.Root,
	                                          0, 0,
								              width, height,
								              0,
								              depth,
								              InputOutput,
								              visual,
								              CWBorderPixel | CWColormap | CWEventMask,
								              &wa);

	ReleaseErrorHandlerX11();

	if(!window->Handle)
	{
		InputErrorX11(Error::Platform_Error, "[X11] Failed to create window!");
		return false;
	}

	s_Data.XLIB.SaveContext(s_Data.display, window->Handle, s_Data.Context, reinterpret_cast<XPointer>(window));

	if(!WNDConfig.Decorated)
		PlatformSetWindowDecoratedX11(window, false);

	if(s_Data.NET_WM_STATE && !window->Monitor)
	{
		std::array<Atom, 3> states{};
		int32_t count = 0;

		if(WNDConfig.Floating)
		{
			if(s_Data.NET_WM_STATE_ABOVE)
				states[count++] = s_Data.NET_WM_STATE_ABOVE;
		}

		if(WNDConfig.Maximized)
		{
			if(s_Data.NET_WM_STATE_MAXIMIZED_VERT &&
			   s_Data.NET_WM_STATE_MAXIMIZED_HORZ)
			{
				states[count++] = s_Data.NET_WM_STATE_MAXIMIZED_VERT;
				states[count++] = s_Data.NET_WM_STATE_MAXIMIZED_HORZ;
				window->Maximized = true;
			}
		}

		if(count)
		{
			s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_STATE, XA_ATOM, 32,
							           PropModeReplace, reinterpret_cast<uint8_t*>(states.data()), count);
		}
	}

	//Declare the WM protocols supported by TRAP
	{
		std::array<Atom, 2> protocols{s_Data.WM_DELETE_WINDOW, s_Data.NET_WM_PING};

		s_Data.XLIB.SetWMProtocols(s_Data.display, window->Handle, protocols.data(), protocols.size());
	}

	//Declare our PID
	{
		int64_t pid = getpid();
		s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_PID, XA_CARDINAL, 32,
		                           PropModeReplace, reinterpret_cast<uint8_t*>(&pid), 1);
	}

	if(s_Data.NET_WM_WINDOW_TYPE && s_Data.NET_WM_WINDOW_TYPE_NORMAL)
	{
		Atom type = s_Data.NET_WM_WINDOW_TYPE_NORMAL;
		s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_WINDOW_TYPE, XA_ATOM, 32,
		                           PropModeReplace, reinterpret_cast<uint8_t*>(&type), 1);
	}

	//Set ICCCM WM_HINTS property
	{
		XWMHints* hints = s_Data.XLIB.AllocWMHints();
		if(!hints)
		{
			InputError(Error::Out_Of_Memory, "[X11] Failed to allocate WM hints!");
			return false;
		}

		hints->flags = StateHint;
		hints->initial_state = NormalState;

		s_Data.XLIB.SetWMHints(s_Data.display, window->Handle, hints);
		s_Data.XLIB.Free(hints);
	}

	//Set ICCCM WM_NORMAL_HINTS property
	{
		XSizeHints* hints = s_Data.XLIB.AllocSizeHints();
		if(!hints)
		{
			InputError(Error::Out_Of_Memory, "[X11] Failed to allocate size hints!");
			return false;
		}

		if(!WNDConfig.Resizable)
		{
			hints->flags |= (PMinSize | PMaxSize);
			hints->min_width = hints->max_width = width;
			hints->min_height = hints->max_height = height;
		}

		hints->flags |= PWinGravity;
		hints->win_gravity = StaticGravity;

		s_Data.XLIB.SetWMNormalHints(s_Data.display, window->Handle, hints);
		s_Data.XLIB.Free(hints);
	}

	//Set ICCCM WM_CLASS property
	{
		XClassHint* hint = s_Data.XLIB.AllocClassHint();
		std::string fallback = "TRAP-Application";

		if(!WNDConfig.Title.empty())
		{
			hint->res_name = WNDConfig.Title.data();
			hint->res_class = WNDConfig.Title.data();
		}
		else
		{
			hint->res_name = fallback.data();
			hint->res_class = fallback.data();
		}

		s_Data.XLIB.SetClassHint(s_Data.display, window->Handle, hint);
		s_Data.XLIB.Free(hint);
	}

	if(s_Data.IM)
		CreateInputContextX11(window);

	PlatformSetWindowTitleX11(window, WNDConfig.Title);
	PlatformGetWindowPosX11(window, window->XPos, window->YPos);
	PlatformGetWindowSizeX11(window, window->Width, window->Height);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a native cursor object from the specified image and hotspot
Cursor TRAP::INTERNAL::WindowingAPI::CreateCursorX11(const TRAP::Image* const image, int32_t const xHotSpot,
                                                     const int32_t yHotSpot)
{
	if(!s_Data.XCursor.Handle)
		return 0;

	XcursorImage* native = s_Data.XCursor.ImageCreate(static_cast<int32_t>(image->GetWidth()),
	                                                  static_cast<int32_t>(image->GetHeight()));
	if(native == nullptr)
		return 0;

	native->xhot = xHotSpot;
	native->yhot = yHotSpot;

	const uint8_t* source = static_cast<const uint8_t*>(image->GetPixelData());
	XcursorPixel* target = native->pixels;

	for(uint32_t i = 0; i < image->GetWidth() * image->GetHeight(); i++, target++, source += 4)
	{
		const uint32_t alpha = source[3];

		*target = (alpha << 24) |
		          (static_cast<uint8_t>((source[0] * alpha) / 255) << 16) |
				  (static_cast<uint8_t>((source[1] * alpha) / 255) <<  8) |
				  (static_cast<uint8_t>((source[2] * alpha) / 255) <<  0);
	}

	const Cursor cursor = s_Data.XCursor.ImageLoadCursor(s_Data.display, native);
	s_Data.XCursor.ImageDestroy(native);

	return cursor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the window is iconified
int32_t TRAP::INTERNAL::WindowingAPI::GetWindowState(const InternalWindow* window)
{
	int32_t result = WithdrawnState;
	struct State
	{
		CARD32 State{};
		::Window Icon{};
	} *state = nullptr;

	if(GetWindowPropertyX11(window->Handle, s_Data.WM_STATE, s_Data.WM_STATE,
	                        reinterpret_cast<uint8_t**>(&state)) >= 2)
		result = static_cast<int32_t>(state->State);

	if(state)
		s_Data.XLIB.Free(state);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

//Encode a Unicode code point to a UTF-8 stream
std::size_t TRAP::INTERNAL::WindowingAPI::EncodeUTF8(char* s, const uint32_t ch)
{
	std::size_t count = 0;

	if(ch < 0x80)
		s[count++] = static_cast<char>(ch);
	else if(ch < 0x800)
	{
		s[count++] = static_cast<char>((ch >> 6) | 0xC0);
		s[count++] = static_cast<char>((ch & 0x3F) | 0x80);
	}
	else if(ch < 0x10000)
	{
		s[count++] = static_cast<char>((ch >> 12) | 0xE0);
		s[count++] = static_cast<char>(((ch >> 6) & 0x3F) | 0x80);
		s[count++] = static_cast<char>((ch & 0x3F) | 0x80);
	}
	else if(ch < 0x110000)
	{
		s[count++] = static_cast<char>((ch >> 18) | 0xF0);
		s[count++] = static_cast<char>(((ch >> 12) & 0x3F) | 0x80);
		s[count++] = static_cast<char>(((ch >> 6) & 0x3F) | 0x80);
		s[count++] = static_cast<char>((ch & 0x3F) | 0x80);
	}

	return count;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::GetSelectionString(const Atom selection)
{
	std::string* selectionString = nullptr;
	const std::array<Atom, 2> targets = {s_Data.UTF8_STRING, XA_STRING};

	if(selection == s_Data.PRIMARY)
		selectionString = &s_Data.PrimarySelectionString;
	else
		selectionString = &s_Data.ClipboardString;

	if(s_Data.XLIB.GetSelectionOwner(s_Data.display, selection) == s_Data.HelperWindowHandle)
	{
		//Instead of doing a large number of X round-trips just to put this string into a window property and
		//then read it back, just return it
		return *selectionString;
	}

	selectionString->clear();

	for(const Atom& target : targets)
	{
		char* data = nullptr;
		Atom actualType = 0;
		int32_t actualFormat = 0;
		uint64_t itemCount = 0, bytesAfter = 0;
		XEvent notification, dummy;

		s_Data.XLIB.ConvertSelection(s_Data.display, selection, target, s_Data.TRAP_SELECTION,
		                             s_Data.HelperWindowHandle, CurrentTime);

		while(!s_Data.XLIB.CheckTypedWindowEvent(s_Data.display, s_Data.HelperWindowHandle, SelectionNotify,
		      &notification))
			WaitForX11Event(nullptr);

		if(notification.xselection.property == None)
			continue;

		s_Data.XLIB.CheckIfEvent(s_Data.display, &dummy, IsSelPropNewValueNotify,
		                         reinterpret_cast<XPointer>(&notification));

		s_Data.XLIB.GetWindowProperty(s_Data.display, notification.xselection.requestor,
		                              notification.xselection.property, 0, std::numeric_limits<long>::max(), 1, AnyPropertyType,
									  &actualType, &actualFormat, &itemCount, &bytesAfter,
									  reinterpret_cast<uint8_t**>(&data));

		if(actualType == s_Data.INCR)
		{
			std::string string{};

			while(true)
			{
				while(!s_Data.XLIB.CheckIfEvent(s_Data.display, &dummy, IsSelPropNewValueNotify,
				                                reinterpret_cast<XPointer>(&notification)))
					WaitForX11Event(nullptr);

				s_Data.XLIB.Free(data);
				s_Data.XLIB.GetWindowProperty(s_Data.display, notification.xselection.requestor,
				                              notification.xselection.property, 0, std::numeric_limits<long>::max(), 1, AnyPropertyType,
											  &actualType, &actualFormat, &itemCount, &bytesAfter,
											  reinterpret_cast<uint8_t**>(&data));

				if(itemCount)
					string = data;

				if(!itemCount)
				{
					if(string.empty())
					{
						if(target == XA_STRING)
						{
							*selectionString = ConvertLatin1ToUTF8(string.c_str());
							string.clear();
						}
						else
							*selectionString = string;
					}

					break;
				}
			}
		}
		else if(actualType == target)
		{
			if(target == XA_STRING)
				*selectionString = ConvertLatin1ToUTF8(data);
			else
				*selectionString = data;
		}

		s_Data.XLIB.Free(data);

		if(!selectionString->empty())
			break;
	}

	if(selectionString->empty())
		InputError(Error::Format_Unavailable, "[X11] Failed to convert selection to string");

	return *selectionString;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether it is a property event for the specified selection transfer
int32_t TRAP::INTERNAL::WindowingAPI::IsSelPropNewValueNotify(Display*, XEvent* event, XPointer pointer)
{
	const XEvent* notification = reinterpret_cast<XEvent*>(pointer);

	return event->type == PropertyNotify &&
	       event->xproperty.state == PropertyNewValue &&
		   event->xproperty.window == notification->xselection.requestor &&
		   event->xproperty.atom == notification->xselection.property;
}

//-------------------------------------------------------------------------------------------------------------------//

//Convert the specified Latin-1 string to UTF-8
std::string TRAP::INTERNAL::WindowingAPI::ConvertLatin1ToUTF8(const std::string_view source)
{
	std::size_t size = 1;
	const char* sp = nullptr;

	for(sp = source.data(); *sp; sp++)
		size += (*sp & 0x80) ? 2 : 1;

	std::string target{};
	target.resize(size);
	char* tp = target.data();

	for(sp = source.data(); *sp; sp++)
		tp += EncodeUTF8(tp, *sp);

	return target;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModeX11(const InternalMonitor* monitor)
{
	InternalVideoMode mode{};

	if(s_Data.RandR.Available && !s_Data.RandR.MonitorBroken)
	{
		XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);
		XRRCrtcInfo* ci = s_Data.RandR.GetCrtcInfo(s_Data.display, sr, monitor->CRTC);

		if(ci)
		{
			const XRRModeInfo* mi = GetModeInfo(sr, ci->mode);
			if(mi) //mi can be nullptr if the monitor has been disconnected
				mode = VideoModeFromModeInfo(mi, ci);

			s_Data.RandR.FreeCrtcInfo(ci);
		}

		s_Data.RandR.FreeScreenResources(sr);
	}
	else
	{
		mode.Width = DisplayWidth(s_Data.display, s_Data.Screen);
		mode.Height = DisplayHeight(s_Data.display, s_Data.Screen);
		mode.RefreshRate = 0;

		SplitBPP(DefaultDepth(s_Data.display, s_Data.Screen), mode.RedBits, mode.GreenBits, mode.BlueBits);
	}

	return mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSizeX11(const InternalWindow* window, int32_t& width,
                                                         int32_t& height)
{
	XWindowAttributes attribs;
	s_Data.XLIB.GetWindowAttributes(s_Data.display, window->Handle, &attribs);

	width = attribs.width;
	height = attribs.height;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPosX11(const InternalWindow* window, const int32_t xPos, const int32_t yPos)
{
	//HACK: Explicitly setting PPosition to any value causes some WMs, notably Compiz and Metacity, to honor
	//      the position of unmapped windows
	if(!PlatformWindowVisibleX11(window))
	{
		int64_t supplied = 0;
		XSizeHints* hints = s_Data.XLIB.AllocSizeHints();

		if(s_Data.XLIB.GetWMNormalHints(s_Data.display, window->Handle, hints, &supplied))
		{
			hints->flags |= PPosition;
			hints->x = hints->y = 0;

			s_Data.XLIB.SetWMNormalHints(s_Data.display, window->Handle, hints);
		}

		s_Data.XLIB.Free(hints);
	}

	s_Data.XLIB.MoveWindow(s_Data.display, window->Handle, xPos, yPos);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorX11(InternalWindow* window, InternalMonitor* monitor,
														    const int32_t xPos, const int32_t yPos, const int32_t width,
															const int32_t height, int32_t /*refreshRate*/)
{
	if(window->Monitor == monitor)
	{
		if(monitor)
		{
			if(monitor->Window == window)
				AcquireMonitor(window);
		}
		else
		{
			if(!window->Resizable)
				UpdateNormalHints(window, width, height);

			s_Data.XLIB.MoveResizeWindow(s_Data.display, window->Handle, xPos, yPos, width, height);
		}

		s_Data.XLIB.Flush(s_Data.display);
		return;
	}

	if(window->Monitor)
	{
		PlatformSetWindowDecoratedX11(window, window->Decorated);
		PlatformSetWindowFloatingX11(window, window->Floating);
		ReleaseMonitor(window);
	}
	if(window->BorderlessFullscreen)
		window->BorderlessFullscreen = false;

	window->Monitor = monitor;
	UpdateNormalHints(window, width, height);

	if(window->Monitor)
	{
		if(!PlatformWindowVisibleX11(window))
		{
			s_Data.XLIB.MapRaised(s_Data.display, window->Handle);
			WaitForVisibilityNotify(window);
		}

		UpdateWindowMode(window);
		AcquireMonitor(window);
	}
	else
	{
		UpdateWindowMode(window);
		s_Data.XLIB.MoveResizeWindow(s_Data.display, window->Handle, xPos, yPos, width, height);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderlessX11(InternalWindow* window,
                                                                      InternalMonitor* monitor)
{
	window->BorderlessFullscreen = true;
	window->Monitor = monitor;
	UpdateNormalHints(window, 0, 0);

	if(!window->Monitor)
		return;

	if(!PlatformWindowVisibleX11(window))
	{
		s_Data.XLIB.MapRaised(s_Data.display, window->Handle);
		WaitForVisibilityNotify(window);
	}

	UpdateWindowMode(window);
	AcquireMonitorBorderless(window);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModesX11(const InternalMonitor* monitor)
{
	std::vector<InternalVideoMode> result{};
	uint32_t count = 0;

	if(s_Data.RandR.Available && !s_Data.RandR.MonitorBroken)
	{
		XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);
		XRRCrtcInfo* ci = s_Data.RandR.GetCrtcInfo(s_Data.display, sr, monitor->CRTC);
		XRROutputInfo* oi = s_Data.RandR.GetOutputInfo(s_Data.display, sr, monitor->Output);

		result.reserve(oi->nmode);

		for(uint32_t i = 0; i < static_cast<uint32_t>(oi->nmode); i++)
		{
			const XRRModeInfo* mi = GetModeInfo(sr, oi->modes[i]);
			if(!static_cast<bool>((mi->modeFlags & RR_Interlace) == 0))
				continue;

			const InternalVideoMode mode = VideoModeFromModeInfo(mi, ci);
			uint32_t j = 0;

			for(j = 0; j < count; j++)
			{
				if(CompareVideoModes(&result[j], &mode) == 0)
					break;
			}

			//Skip duplicate modes
			if(j < count)
				continue;

			count++;
			result.push_back(mode);
		}

		s_Data.RandR.FreeOutputInfo(oi);
		s_Data.RandR.FreeCrtcInfo(ci);
		s_Data.RandR.FreeScreenResources(sr);
	}
	else
	{
		count = 1;
		result.push_back(PlatformGetVideoModeX11(monitor));
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformInitX11()
{
#if !defined(X_HAVE_UTF8_STRING)
	//HACK: If the current locale is "C" and the Xlib UTF-8 functions are unavailable, apply the environment's
	//      locale in the hope that it's both available and not "C"
	//      This is done because the "C" locale breaks wide character input, which is what we fall back on
	//      when UTF-8 support is missing
	if(strcmp(setLocale(LC_CTYPE, nullptr), "C") == 0)
		setLocale(LC_CTYPE, "");
#endif

	#if defined(__CYGWIN__)
		s_Data.XLIB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11-6.so");
	#elif defined(__OpenBSD__) || defined(__NetBSD__)
		s_Data.XLIB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11.so");
	#else
        s_Data.XLIB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11.so.6");
	#endif

	if(!s_Data.XLIB.Handle)
	{
		InputError(Error::Platform_Error, "[X11] Failed to load Xlib");
		return false;
	}

	s_Data.XLIB.AllocClassHint = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XAllocClassHint>(s_Data.XLIB.Handle, "XAllocClassHint");
	s_Data.XLIB.AllocSizeHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XAllocSizeHints>(s_Data.XLIB.Handle, "XAllocSizeHints");
	s_Data.XLIB.AllocWMHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XAllocWMHints>(s_Data.XLIB.Handle, "XAllocWMHints");
	s_Data.XLIB.ChangeProperty = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XChangeProperty>(s_Data.XLIB.Handle, "XChangeProperty");
	s_Data.XLIB.ChangeWindowAttributes = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XChangeWindowAttributes>(s_Data.XLIB.Handle,
																													"XChangeWindowAttributes");
	s_Data.XLIB.CheckIfEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCheckIfEvent>(s_Data.XLIB.Handle, "XCheckIfEvent");
	s_Data.XLIB.CheckTypedWindowEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCheckTypedWindowEvent>(s_Data.XLIB.Handle,
																												  "XCheckTypedWindowEvent");
	s_Data.XLIB.CloseDisplay = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCloseDisplay>(s_Data.XLIB.Handle, "XCloseDisplay");
	s_Data.XLIB.CloseIM = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCloseIM>(s_Data.XLIB.Handle, "XCloseIM");
	s_Data.XLIB.ConvertSelection = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XConvertSelection>(s_Data.XLIB.Handle,
	                                                                                                    "XConvertSelection");
	s_Data.XLIB.CreateColormap = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateColormap>(s_Data.XLIB.Handle,
	                                                                                                "XCreateColormap");
	s_Data.XLIB.CreateFontCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateFontCursor>(s_Data.XLIB.Handle,
	                                                                                                    "XCreateFontCursor");
	s_Data.XLIB.CreateIC = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateIC>(s_Data.XLIB.Handle,"XCreateIC");
	s_Data.XLIB.CreateWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateWindow>(s_Data.XLIB.Handle, "XCreateWindow");
	s_Data.XLIB.DefineCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDefineCursor>(s_Data.XLIB.Handle, "XDefineCursor");
	s_Data.XLIB.DeleteContext = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDeleteContext>(s_Data.XLIB.Handle, "XDeleteContext");
	s_Data.XLIB.DeleteProperty = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDeleteProperty>(s_Data.XLIB.Handle, "XDeleteProperty");
	s_Data.XLIB.DestroyIC = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDestroyIC>(s_Data.XLIB.Handle, "XDestroyIC");
	s_Data.XLIB.DestroyWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDestroyWindow>(s_Data.XLIB.Handle, "XDestroyWindow");
	s_Data.XLIB.DisplayKeycodes = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDisplayKeycodes>(s_Data.XLIB.Handle,
	                                                                                                  "XDisplayKeycodes");
	s_Data.XLIB.EventsQueued = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XEventsQueued>(s_Data.XLIB.Handle, "XEventsQueued");
	s_Data.XLIB.FilterEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFilterEvent>(s_Data.XLIB.Handle, "XFilterEvent");
	s_Data.XLIB.FindContext = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFindContext>(s_Data.XLIB.Handle, "XFindContext");
	s_Data.XLIB.Flush = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFlush>(s_Data.XLIB.Handle, "XFlush");
	s_Data.XLIB.Free = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFree>(s_Data.XLIB.Handle, "XFree");
	s_Data.XLIB.FreeColormap = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFreeColormap>(s_Data.XLIB.Handle, "XFreeColormap");
	s_Data.XLIB.FreeCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFreeCursor>(s_Data.XLIB.Handle, "XFreeCursor");
	s_Data.XLIB.FreeEventData = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFreeEventData>(s_Data.XLIB.Handle, "XFreeEventData");
	s_Data.XLIB.GetAtomName = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetAtomName>(s_Data.XLIB.Handle, "XGetAtomName");
	s_Data.XLIB.GetErrorText = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetErrorText>(s_Data.XLIB.Handle, "XGetErrorText");
	s_Data.XLIB.GetEventData = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetEventData>(s_Data.XLIB.Handle, "XGetEventData");
	s_Data.XLIB.GetICValues = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetICValues>(s_Data.XLIB.Handle, "XGetICValues");
	s_Data.XLIB.GetIMValues = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetIMValues>(s_Data.XLIB.Handle, "XGetIMValues");
	s_Data.XLIB.GetInputFocus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetInputFocus>(s_Data.XLIB.Handle, "XGetInputFocus");
	s_Data.XLIB.GetKeyboardMapping = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetKeyboardMapping>(s_Data.XLIB.Handle,
	                                                                                                        "XGetKeyboardMapping");
	s_Data.XLIB.GetScreenSaver = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetScreenSaver>(s_Data.XLIB.Handle, "XGetScreenSaver");
	s_Data.XLIB.GetSelectionOwner = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetSelectionOwner>(s_Data.XLIB.Handle,
	                                                                               						  "XGetSelectionOwner");
	s_Data.XLIB.GetVisualInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetVisualInfo>(s_Data.XLIB.Handle, "XGetVisualInfo");
	s_Data.XLIB.GetWMNormalHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetWMNormalHints>(s_Data.XLIB.Handle,
	                                                                             					    "XGetWMNormalHints");
	s_Data.XLIB.GetWindowAttributes = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetWindowAttributes>(s_Data.XLIB.Handle,
	                                                                                                          "XGetWindowAttributes");
	s_Data.XLIB.GetWindowProperty = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetWindowProperty>(s_Data.XLIB.Handle,
	                                                                                                      "XGetWindowProperty");
	s_Data.XLIB.GrabPointer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGrabPointer>(s_Data.XLIB.Handle, "XGrabPointer");
	s_Data.XLIB.IconifyWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XIconifyWindow>(s_Data.XLIB.Handle, "XIconifyWindow");
	s_Data.XLIB.InitThreads = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XInitThreads>(s_Data.XLIB.Handle, "XInitThreads");
	s_Data.XLIB.InternAtom = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XInternAtom>(s_Data.XLIB.Handle, "XInternAtom");
	s_Data.XLIB.LookupString = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XLookupString>(s_Data.XLIB.Handle, "XLookupString");
	s_Data.XLIB.MapRaised = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XMapRaised>(s_Data.XLIB.Handle, "XMapRaised");
	s_Data.XLIB.MapWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XMapWindow>(s_Data.XLIB.Handle, "XMapWindow");
	s_Data.XLIB.MoveResizeWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XMoveResizeWindow>(s_Data.XLIB.Handle,
	                                                                                                    "XMoveResizeWindow");
	s_Data.XLIB.MoveWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XMoveWindow>(s_Data.XLIB.Handle, "XMoveWindow");
	s_Data.XLIB.NextEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XNextEvent>(s_Data.XLIB.Handle, "XNextEvent");
	s_Data.XLIB.OpenDisplay = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XOpenDisplay>(s_Data.XLIB.Handle, "XOpenDisplay");
	s_Data.XLIB.OpenIM = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XOpenIM>(s_Data.XLIB.Handle, "XOpenIM");
	s_Data.XLIB.PeekEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XPeekEvent>(s_Data.XLIB.Handle, "XPeekEvent");
	s_Data.XLIB.Pending = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XPending>(s_Data.XLIB.Handle, "XPending");
	s_Data.XLIB.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XQueryExtension>(s_Data.XLIB.Handle, "XQueryExtension");
	s_Data.XLIB.QueryPointer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XQueryPointer>(s_Data.XLIB.Handle, "XQueryPointer");
	s_Data.XLIB.RaiseWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRaiseWindow>(s_Data.XLIB.Handle, "XRaiseWindow");
	s_Data.XLIB.RegisterIMInstantiateCallback = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRegisterIMInstantiateCallback>(s_Data.XLIB.Handle,
																																  "XRegisterIMInstantiateCallback");
	s_Data.XLIB.ResizeWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XResizeWindow>(s_Data.XLIB.Handle, "XResizeWindow");
	s_Data.XLIB.ResourceManagerString = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XResourceManagerString>(s_Data.XLIB.Handle,
																												  "XResourceManagerString");
	s_Data.XLIB.SaveContext = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSaveContext>(s_Data.XLIB.Handle, "XSaveContext");
	s_Data.XLIB.SelectInput = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSelectInput>(s_Data.XLIB.Handle, "XSelectInput");
	s_Data.XLIB.SendEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSendEvent>(s_Data.XLIB.Handle, "XSendEvent");
	s_Data.XLIB.SetClassHint = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetClassHint>(s_Data.XLIB.Handle, "XSetClassHint");
	s_Data.XLIB.SetErrorHandler = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetErrorHandler>(s_Data.XLIB.Handle,
	                                                                                                  "XSetErrorHandler");
	s_Data.XLIB.SetICFocus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetICFocus>(s_Data.XLIB.Handle, "XSetICFocus");
	s_Data.XLIB.SetIMValues = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetIMValues>(s_Data.XLIB.Handle, "XSetIMValues");
	s_Data.XLIB.SetInputFocus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetInputFocus>(s_Data.XLIB.Handle, "XSetInputFocus");
	s_Data.XLIB.SetLocaleModifiers = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetLocaleModifiers>(s_Data.XLIB.Handle,
	                                                                                                        "XSetLocaleModifiers");
	s_Data.XLIB.SetScreenSaver = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetScreenSaver>(s_Data.XLIB.Handle, "XSetScreenSaver");
	s_Data.XLIB.SetSelectionOwner = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetSelectionOwner>(s_Data.XLIB.Handle,
	                                                                                                      "XSetSelectionOwner");
	s_Data.XLIB.SetWMHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetWMHints>(s_Data.XLIB.Handle, "XSetWMHints");
	s_Data.XLIB.SetWMNormalHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetWMNormalHints>(s_Data.XLIB.Handle,
	                                                                                                    "XSetWMNormalHints");
	s_Data.XLIB.SetWMProtocols = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetWMProtocols>(s_Data.XLIB.Handle, "XSetWMProtocols");
	s_Data.XLIB.SupportsLocale = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSupportsLocale>(s_Data.XLIB.Handle, "XSupportsLocale");
	s_Data.XLIB.Sync = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSync>(s_Data.XLIB.Handle, "XSync");
	s_Data.XLIB.TranslateCoordinates = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XTranslateCoordinates>(s_Data.XLIB.Handle,
	                                                                                                            "XTranslateCoordinates");
	s_Data.XLIB.UndefineCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUndefineCursor>(s_Data.XLIB.Handle, "XUndefineCursor");
	s_Data.XLIB.UngrabPointer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUngrabPointer>(s_Data.XLIB.Handle, "XUngrabPointer");
	s_Data.XLIB.UnmapWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUnmapWindow>(s_Data.XLIB.Handle, "XUnmapWindow");
	s_Data.XLIB.UnsetICFocus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUnsetICFocus>(s_Data.XLIB.Handle, "XUnsetICFocus");
	s_Data.XLIB.VisualIDFromVisual = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XVisualIDFromVisual>(s_Data.XLIB.Handle,
	                                                                                                        "XVisualIDFromVisual");
	s_Data.XLIB.WarpPointer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XWarpPointer>(s_Data.XLIB.Handle, "XWarpPointer");
	s_Data.XLIB.UnregisterIMInstantiateCallback = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUnregisterIMInstantiateCallback>(s_Data.XLIB.Handle,
	                       																											  "XUnregisterIMInstantiateCallback");
	s_Data.XLIB.UTF8LookupString = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_Xutf8LookupString>(s_Data.XLIB.Handle,
	                                                                                                    "Xutf8LookupString");
	s_Data.XLIB.UTF8SetWMProperties = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_Xutf8SetWMProperties>(s_Data.XLIB.Handle,
	                                                                                                          "Xutf8SetWMProperties");
	s_Data.XLIB.CreateRegion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateRegion>(s_Data.XLIB.Handle, "XCreateRegion");
	s_Data.XLIB.DestroyRegion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDestroyRegion>(s_Data.XLIB.Handle, "XDestroyRegion");
	s_Data.XKB.AllocKeyboard = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbAllocKeyboard>(s_Data.XLIB.Handle, "XkbAllocKeyboard");
	s_Data.XKB.FreeKeyboard = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbFreeKeyboard>(s_Data.XLIB.Handle, "XkbFreeKeyboard");
	s_Data.XKB.FreeNames = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbFreeNames>(s_Data.XLIB.Handle, "XkbFreeNames");
	s_Data.XKB.GetMap = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbGetMap>(s_Data.XLIB.Handle, "XkbGetMap");
	s_Data.XKB.GetNames = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbGetNames>(s_Data.XLIB.Handle, "XkbGetNames");
	s_Data.XKB.GetState = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbGetState>(s_Data.XLIB.Handle, "XkbGetState");
	s_Data.XKB.KeycodeToKeysym = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbKeycodeToKeysym>(s_Data.XLIB.Handle,
	                                                                                                   "XkbKeycodeToKeysym");
	s_Data.XKB.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbQueryExtension>(s_Data.XLIB.Handle,
	                                                                                                 "XkbQueryExtension");
	s_Data.XKB.SelectEventDetails = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbSelectEventDetails>(s_Data.XLIB.Handle,
	                                                                                                         "XkbSelectEventDetails");
	s_Data.XKB.SetDetectableAutoRepeat = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbSetDetectableAutoRepeat>(s_Data.XLIB.Handle,
	 																												   "XkbSetDetectableAutoRepeat");
	s_Data.XRM.DestroyDatabase = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmDestroyDatabase>(s_Data.XLIB.Handle,
	                                                                                                   "XrmDestroyDatabase");
	s_Data.XRM.GetResource = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmGetResource>(s_Data.XLIB.Handle, "XrmGetResource");
	s_Data.XRM.GetStringDatabase = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmGetStringDatabase>(s_Data.XLIB.Handle,
	                                                                                                       "XrmGetStringDatabase");
	s_Data.XRM.Initialize = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmInitialize>(s_Data.XLIB.Handle, "XrmInitialize");
	s_Data.XRM.UniqueQuark = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmUniqueQuark>(s_Data.XLIB.Handle, "XrmUniqueQuark");
	if (s_Data.XLIB.UTF8LookupString && s_Data.XLIB.UTF8SetWMProperties)
		s_Data.XLIB.UTF8 = true;

	s_Data.XLIB.InitThreads();
	s_Data.XRM.Initialize();

	s_Data.display = s_Data.XLIB.OpenDisplay(nullptr);
	if(!s_Data.display)
	{
		const char* display = getenv("DISPLAY");
		if(display)
			InputError(Error::Platform_Error, std::string("[X11] Failed to open display: ") + display);
		else
			InputError(Error::Platform_Error, "[X11] The DISPLAY environment variable is missing!");

		return false;
	}

	s_Data.Screen = DefaultScreen(s_Data.display);
	s_Data.Root = RootWindow(s_Data.display, s_Data.Screen);
	s_Data.Context = static_cast<XContext>(s_Data.XRM.UniqueQuark()); //Same as calling XUniqueContext();

	GetSystemContentScale(s_Data.ContentScaleX, s_Data.ContentScaleY);

	if(!CreateEmptyEventPipe())
		return false;

	if(!InitExtensions())
		return false;

	s_Data.HelperWindowHandle = CreateHelperWindow();
	s_Data.HiddenCursorHandle = CreateHiddenCursor();

	if(s_Data.XLIB.SupportsLocale() && s_Data.XLIB.UTF8)
	{
		s_Data.XLIB.SetLocaleModifiers("");

		//If an IM is already present our callback will be called right away
		s_Data.XLIB.RegisterIMInstantiateCallback(s_Data.display, nullptr, nullptr, nullptr,
		                                          InputMethodInstantiateCallback, nullptr);
	}

	PollMonitorsX11();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyWindowX11(InternalWindow* window)
{
	if (s_Data.DisabledCursorWindow == window)
		s_Data.DisabledCursorWindow = nullptr;
		// EnableCursor(window);

	if(window->Monitor)
		ReleaseMonitor(window);

	if(window->IC)
	{
		s_Data.XLIB.DestroyIC(window->IC);
		window->IC = nullptr;
	}

	if(window->Handle)
	{
		s_Data.XLIB.DeleteContext(s_Data.display, window->Handle, s_Data.Context);
		s_Data.XLIB.UnmapWindow(s_Data.display, window->Handle);
		s_Data.XLIB.DestroyWindow(s_Data.display, window->Handle);
		window->Handle = 0;
	}

	if(window->colormap)
	{
		s_Data.XLIB.FreeColormap(s_Data.display, window->colormap);
		window->colormap = 0;
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShutdownX11()
{
	if(s_Data.HelperWindowHandle)
	{
		if(s_Data.XLIB.GetSelectionOwner(s_Data.display, s_Data.CLIPBOARD) == s_Data.HelperWindowHandle)
			PushSelectionToManagerX11();

		s_Data.XLIB.DestroyWindow(s_Data.display, s_Data.HelperWindowHandle);
		s_Data.HelperWindowHandle = 0;
	}

	if(s_Data.HiddenCursorHandle)
	{
		s_Data.XLIB.FreeCursor(s_Data.display, s_Data.HiddenCursorHandle);
		s_Data.HiddenCursorHandle = 0;
	}

	s_Data.XLIB.UnregisterIMInstantiateCallback(s_Data.display, nullptr, nullptr, nullptr,
	                                            InputMethodInstantiateCallback, nullptr);

	if(s_Data.IM)
	{
		s_Data.XLIB.CloseIM(s_Data.IM);
		s_Data.IM = nullptr;
	}

	if(s_Data.display)
	{
		s_Data.XLIB.CloseDisplay(s_Data.display);
		s_Data.display = nullptr;
	}

	if(s_Data.XCB.Handle)
	{
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.XCB.Handle);
		s_Data.XCB.Handle = nullptr;
	}

	if(s_Data.XCursor.Handle)
	{
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.XCursor.Handle);
		s_Data.XCursor.Handle = nullptr;
	}

	if(s_Data.RandR.Handle)
	{
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.RandR.Handle);
		s_Data.RandR.Handle = nullptr;
	}

	if(s_Data.Xinerama.Handle)
	{
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.Xinerama.Handle);
		s_Data.Xinerama.Handle = nullptr;
	}

	if(s_Data.XRender.Handle)
	{
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.XRender.Handle);
		s_Data.XRender.Handle = nullptr;
	}

	if(s_Data.XI.Handle)
	{
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.XI.Handle);
		s_Data.XI.Handle = nullptr;
	}

	if(s_Data.XLIB.Handle)
	{
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.XLIB.Handle);
		s_Data.XLIB.Handle = nullptr;
	}

	if(s_Data.EmptyEventPipe[0] || s_Data.EmptyEventPipe[1])
	{
		close(s_Data.EmptyEventPipe[0]);
		close(s_Data.EmptyEventPipe[1]);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScaleX11(const InternalMonitor*, float& xScale,
                                                                  float& yScale)
{
	xScale = s_Data.ContentScaleX;
	yScale = s_Data.ContentScaleY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPosX11(const InternalMonitor* monitor, int32_t& xPos,
                                                         int32_t& yPos)
{
	if(!s_Data.RandR.Available || s_Data.RandR.MonitorBroken)
		return;

	XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);
	XRRCrtcInfo* ci = s_Data.RandR.GetCrtcInfo(s_Data.display, sr, monitor->CRTC);

	if(ci)
	{
		xPos = ci->x;
		yPos = ci->y;

		s_Data.RandR.FreeCrtcInfo(ci);
	}
	s_Data.RandR.FreeScreenResources(sr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindowX11(InternalWindow* window)
{
	if(PlatformWindowVisibleX11(window))
		return;

	s_Data.XLIB.MapWindow(s_Data.display, window->Handle);
	WaitForVisibilityNotify(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindowX11(const InternalWindow* window)
{
	if(s_Data.NET_ACTIVE_WINDOW)
		SendEventToWM(window, s_Data.NET_ACTIVE_WINDOW, 1, 0, 0, 0, 0);
	else if (PlatformWindowVisibleX11(window))
	{
		s_Data.XLIB.RaiseWindow(s_Data.display, window->Handle);
		s_Data.XLIB.SetInputFocus(s_Data.display, window->Handle, RevertToParent, CurrentTime);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowX11(InternalWindow* window,
			                                            WindowConfig& WNDConfig)
{
	Visual* visual = nullptr;
	int32_t depth = 0;

	if(!visual)
	{
		visual = DefaultVisual(s_Data.display, s_Data.Screen);
		depth = DefaultDepth(s_Data.display, s_Data.Screen);
	}

	if(!CreateNativeWindow(window, WNDConfig, visual, depth))
		return false;

	if(WNDConfig.MousePassthrough)
		PlatformSetWindowMousePassthroughX11(window, true);

	if(window->Monitor)
	{
		PlatformShowWindowX11(window);
		UpdateWindowMode(window);
		if(window->BorderlessFullscreen)
			AcquireMonitorBorderless(window);
		else
			AcquireMonitor(window);
	}
	else
	{
		if(WNDConfig.Visible)
		{
			PlatformShowWindowX11(window);
			if(WNDConfig.Focused)
				PlatformFocusWindowX11(window);
		}
	}

	s_Data.XLIB.Flush(s_Data.display);
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowTitleX11(const InternalWindow* window, const std::string& title)
{
	if (s_Data.XLIB.UTF8)
	{
		s_Data.XLIB.UTF8SetWMProperties(s_Data.display,
			                            window->Handle,
			                            title.c_str(),
			                            title.c_str(),
			                            nullptr,
			                            0,
			                            nullptr,
			                            nullptr,
			                            nullptr);
	}

	s_Data.XLIB.ChangeProperty(s_Data.display,
					           window->Handle,
					           s_Data.NET_WM_NAME,
					           s_Data.UTF8_STRING,
					           8,
					           PropModeReplace,
					           reinterpret_cast<const uint8_t*>(title.data()),
					           static_cast<int32_t>(title.size()));

	s_Data.XLIB.ChangeProperty(s_Data.display,
	                           window->Handle,
					           s_Data.NET_WM_ICON_NAME,
					           s_Data.UTF8_STRING,
					           8,
					           PropModeReplace,
					           reinterpret_cast<const uint8_t*>(title.data()),
					           static_cast<int32_t>(title.size()));

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursorX11(InternalCursor* cursor, const Image* const image,
                                                        const int32_t xHotspot, const int32_t yHotspot)
{
	cursor->Handle = CreateCursorX11(image, xHotspot, yHotspot);

	return cursor->Handle;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursorX11(InternalCursor* cursor, const CursorType& type)
{
	if(s_Data.XCursor.Handle)
	{
		const char* theme = s_Data.XCursor.GetTheme(s_Data.display);
		if(theme)
		{
			const int32_t size = s_Data.XCursor.GetDefaultSize(s_Data.display);
			std::string name = "default";

			if(type == CursorType::Arrow)
				name = "default";
			else if(type == CursorType::Input)
				name = "text";
			else if(type == CursorType::Crosshair)
				name = "crosshair";
			else if(type == CursorType::PointingHand)
				name = "pointer";
			else if(type == CursorType::ResizeHorizontal)
				name = "ew-resize";
			else if(type == CursorType::ResizeVertical)
				name = "ns-resize";
			else if(type == CursorType::ResizeDiagonalTopRightBottomLeft)
				name = "nwse-resize";
			else if(type == CursorType::ResizeDiagonalTopLeftBottomRight)
				name = "nesw-resize";
			else if(type == CursorType::ResizeAll)
				name = "all-scroll";
			else if(type == CursorType::NotAllowed)
				name = "not-allowed";

			XcursorImage* image = s_Data.XCursor.LibraryLoadImage(name.c_str(), theme, size);
			if(image)
			{
				cursor->Handle = s_Data.XCursor.ImageLoadCursor(s_Data.display, image);
				s_Data.XCursor.ImageDestroy(image);
			}
		}
	}

	if(!cursor->Handle)
	{
		uint32_t native = 0;

		if(type == CursorType::Arrow)
			native = XC_left_ptr;
		else if(type == CursorType::Input)
			native = XC_xterm;
		else if(type == CursorType::Crosshair)
			native = XC_crosshair;
		else if(type == CursorType::PointingHand)
			native = XC_hand2;
		else if(type == CursorType::ResizeHorizontal)
			native = XC_sb_h_double_arrow;
		else if(type == CursorType::ResizeVertical)
			native = XC_sb_v_double_arrow;
		else if(type == CursorType::ResizeDiagonalTopRightBottomLeft)
			native = XC_top_left_corner;
		else if(type == CursorType::ResizeDiagonalTopLeftBottomRight)
			native = XC_top_right_corner;
		else if(type == CursorType::ResizeAll)
			native = XC_fleur;
		else if(type == CursorType::NotAllowed)
			native = XC_X_cursor;
		else
		{
			InputError(Error::Cursor_Unavailable, "[X11] Standard cursor shape unavailable!");
			return false;
		}

		cursor->Handle = s_Data.XLIB.CreateFontCursor(s_Data.display, native);
		if(!cursor->Handle)
		{
			InputError(Error::Platform_Error, "[X11] Failed to create standard cursor!");
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursorX11(InternalCursor* cursor)
{
	if(cursor->Handle)
		s_Data.XLIB.FreeCursor(s_Data.display, cursor->Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorX11(const InternalWindow* window, const InternalCursor* /*cursor*/)
{
	if(window->cursorMode != CursorMode::Normal)
		return;

	UpdateCursorImage(window);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorModeX11(InternalWindow* window, const CursorMode mode)
{
	if (PlatformWindowFocusedX11(window))
	{
		if (mode == CursorMode::Disabled)
		{
			PlatformGetCursorPosX11(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
			CenterCursorInContentArea(window);
			if (window->RawMouseMotion)
				EnableRawMouseMotion(window);
		}
		else if (s_Data.DisabledCursorWindow == window)
		{
			if (window->RawMouseMotion)
				DisableRawMouseMotion(window);
		}

		if (mode == CursorMode::Disabled || mode == CursorMode::Captured)
			CaptureCursor(window);
		else
			ReleaseCursor();

		if (mode == CursorMode::Disabled)
			s_Data.DisabledCursorWindow = window;
		else if (s_Data.DisabledCursorWindow == window)
		{
			s_Data.DisabledCursorWindow = nullptr;
			PlatformSetCursorPosX11(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
		}
	}

	UpdateCursorImage(window);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPosX11(InternalWindow* window, const double xPos, const double yPos)
{
	//Store the new position so it can be recognized later
	window->WarpCursorPosX = static_cast<int32_t>(xPos);
	window->WarpCursorPosY = static_cast<int32_t>(yPos);

	s_Data.XLIB.WarpPointer(s_Data.display, None, window->Handle, 0, 0, 0, 0, static_cast<int32_t>(xPos),
	                        static_cast<int32_t>(yPos));
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPosX11(const InternalWindow* window, double& xPos, double& yPos)
{
	::Window root = 0, child = 0;
	int32_t rootX = 0, rootY = 0, childX = 0, childY = 0;
	uint32_t mask = 0;

	s_Data.XLIB.QueryPointer(s_Data.display, window->Handle, &root, &child, &rootX, &rootY, &childX, &childY,
	                         &mask);

	xPos = childX;
	yPos = childY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIconX11(InternalWindow* window, const Image* const image)
{
	if(image)
	{
		const uint32_t longCount = 2 + image->GetWidth() * image->GetHeight();

		std::vector<uint64_t> icon{};
		icon.resize(longCount);
		uint64_t* target = icon.data();
		const std::vector<uint8_t> imgData(static_cast<const uint8_t*>(image->GetPixelData()),
		                                   static_cast<const uint8_t*>(image->GetPixelData()) +
									       image->GetPixelDataSize());

		*target++ = image->GetWidth();
		*target++ = image->GetHeight();

		for(uint32_t j = 0; j < image->GetWidth() * image->GetHeight(); j++)
		{
			*target++ = (static_cast<uint64_t>(imgData[j * 4 + 0]) << 16) |
						(static_cast<uint64_t>(imgData[j * 4 + 1]) <<  8) |
						(static_cast<uint64_t>(imgData[j * 4 + 2]) <<  0) |
						(static_cast<uint64_t>(imgData[j * 4 + 3]) << 24);
		}

		//NOTE: XChangeProperty expects 32-bit values like the image data above to be
		//      places in the 32 least significant bits of individual longs. This is true
		//      even if long is 64-bit and a WM protocol calls for "packed" data.
		//      This is because of a historical mistake that then became part of the Xlib ABI.
		//      Xlib will pack these values into a regular array of 32-bit values before
		//      sending it over the wire.
		s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_ICON, XA_CARDINAL, 32,
		                           PropModeReplace,	reinterpret_cast<uint8_t*>(icon.data()),
								   static_cast<int32_t>(longCount));
	}
	else
		s_Data.XLIB.DeleteProperty(s_Data.display, window->Handle, s_Data.NET_WM_ICON);

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPosX11(const InternalWindow* window, int32_t& xPos, int32_t& yPos)
{
	::Window dummy = 0;
	int32_t x = 0, y = 0;

	s_Data.XLIB.TranslateCoordinates(s_Data.display, window->Handle, s_Data.Root, 0, 0, &x, &y, &dummy);

	xPos = x;
	yPos = y;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeX11(InternalWindow* window, const int32_t width, const int32_t height)
{
	if(window->Monitor && window->Monitor->Window == window)
	{
		if(window->Monitor->Window->BorderlessFullscreen)
			AcquireMonitorBorderless(window);
		else
			AcquireMonitor(window);
	}
	else
	{
		if(!window->Resizable)
			UpdateNormalHints(window, width, height);

		s_Data.XLIB.ResizeWindow(s_Data.display, window->Handle, width, height);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizableX11(InternalWindow* window, bool /*enabled*/)
{
	int32_t width = 0, height = 0;

	PlatformGetWindowSizeX11(window, width, height);
	UpdateNormalHints(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecoratedX11(const InternalWindow* window, const bool enabled)
{
	struct Hints
	{
		uint64_t Flags = 0;
		uint64_t Functions = 0;
		uint64_t Decorations = 0;
		int64_t InputMode = 0;
		uint64_t status = 0;
	} hints{};

	hints.Flags = MWM_HINTS_DECORATIONS; //MWM_HINTS_DECORATIONS
	hints.Decorations = enabled ? MWM_DECOR_ALL : 0; //1 = MWM_DECOR_ALL

	s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.MOTIF_WM_HINTS, s_Data.MOTIF_WM_HINTS, 32,
	                           PropModeReplace, reinterpret_cast<uint8_t*>(&hints), sizeof(hints) / sizeof(int64_t));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloatingX11(const InternalWindow* window, const bool enabled)
{
	if(!s_Data.NET_WM_STATE || !s_Data.NET_WM_STATE_ABOVE)
		return;

	if(PlatformWindowVisibleX11(window))
	{
		const int64_t action = static_cast<int64_t>(enabled ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE);
		SendEventToWM(window, s_Data.NET_WM_STATE, action, static_cast<int64_t>(s_Data.NET_WM_STATE_ABOVE), 0, 1, 0);
	}
	else
	{
		Atom* states = nullptr;

		const uint64_t count = GetWindowPropertyX11(window->Handle, s_Data.NET_WM_STATE, XA_ATOM,
		                                                 reinterpret_cast<uint8_t**>(&states));

		//NOTE: We don't check for failure as this property may not exist yet and that's fine (and we'll create
		//      it implicitly with append)
		if(enabled)
		{
			uint64_t i = 0;

			for(i = 0; i < count; i++)
			{
				if(states[i] == s_Data.NET_WM_STATE_ABOVE)
					break;
			}

			if(i == count)
			{
				s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_STATE, XA_ATOM, 32,
										   PropModeAppend, reinterpret_cast<uint8_t*>(&s_Data.NET_WM_STATE_ABOVE), 1);
			}
		}
		else if(states)
		{
			for(uint64_t i = 0; i < count; ++i)
			{
				if(states[i] == s_Data.NET_WM_STATE_ABOVE)
				{
					states[i] = states[count - 1];
					s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_STATE, XA_ATOM, 32,
											   PropModeReplace, reinterpret_cast<uint8_t*>(states),
											   static_cast<int32_t>(count) - 1);
					break;
				}
			}
		}

		if(states)
			s_Data.XLIB.Free(states);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacityX11(const InternalWindow* window, const float opacity)
{
	const CARD32 value = static_cast<CARD32>(0xFFFFFFFFu * static_cast<double>(opacity));
	s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_WINDOW_OPACITY, XA_CARDINAL, 32,
	                           PropModeReplace, reinterpret_cast<const uint8_t*>(&value), 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthroughX11(InternalWindow* window, const bool enabled)
{
	if(!s_Data.XShape.Available)
		return;

	if (enabled)
	{
		const Region region = s_Data.XLIB.CreateRegion();
		s_Data.XShape.CombineRegion(s_Data.display, window->Handle, ShapeInput, 0, 0, region, ShapeSet);
		s_Data.XLIB.DestroyRegion(region);
	}
	else
		s_Data.XShape.CombineMask(s_Data.display, window->Handle, ShapeInput, 0, 0, None, ShapeSet);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbarX11(InternalWindow* /*window*/)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacityX11(const InternalWindow* window)
{
	float opacity = 1.0f;

	if(s_Data.XLIB.GetSelectionOwner(s_Data.display, s_Data.NET_WM_CM_Sx))
	{
		CARD32* value = nullptr;

		if(GetWindowPropertyX11(window->Handle, s_Data.NET_WM_WINDOW_OPACITY, XA_CARDINAL,
		                        reinterpret_cast<uint8_t**>(&value)))
			opacity = static_cast<float>(*value / static_cast<double>(0xFFFFFFFFu));

		if(value)
			s_Data.XLIB.Free(value);
	}

	return opacity;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSizeX11(const InternalWindow* window, int32_t& width,
                                                              int32_t& height)
{
	PlatformGetWindowSizeX11(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScaleX11(const InternalWindow*, float& xScale, float& yScale)
{
	xScale = s_Data.ContentScaleX;
	yScale = s_Data.ContentScaleY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkAreaX11(const InternalMonitor* monitor, int32_t& xPos,
                                                              int32_t& yPos, int32_t& width, int32_t& height)
{
	int32_t areaX = 0, areaY = 0, areaWidth = 0, areaHeight = 0;

	if(s_Data.RandR.Available && !s_Data.RandR.MonitorBroken)
	{
		XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);
		XRRCrtcInfo* ci = s_Data.RandR.GetCrtcInfo(s_Data.display, sr, monitor->CRTC);

		areaX = ci->x;
		areaY = ci->y;

		const XRRModeInfo* mi = GetModeInfo(sr, ci->mode);

		if(ci->rotation == RR_Rotate_90 || ci->rotation == RR_Rotate_270)
		{
			areaWidth = static_cast<int32_t>(mi->height);
			areaHeight = static_cast<int32_t>(mi->width);
		}
		else
		{
			areaWidth = static_cast<int32_t>(mi->width);
			areaHeight = static_cast<int32_t>(mi->height);
		}

		s_Data.RandR.FreeCrtcInfo(ci);
		s_Data.RandR.FreeScreenResources(sr);
	}
	else
	{
		areaWidth = DisplayWidth(s_Data.display, s_Data.Screen);
		areaHeight = DisplayHeight(s_Data.display, s_Data.Screen);
	}

	if(s_Data.NET_WORKAREA && s_Data.NET_CURRENT_DESKTOP)
	{
		Atom* extents = nullptr;
		Atom* desktop = nullptr;
		const uint64_t extentCount = GetWindowPropertyX11(s_Data.Root, s_Data.NET_WORKAREA, XA_CARDINAL,
		                                                  reinterpret_cast<uint8_t**>(&extents));

		if(GetWindowPropertyX11(s_Data.Root, s_Data.NET_CURRENT_DESKTOP, XA_CARDINAL,
		                        reinterpret_cast<uint8_t**>(&desktop)) > 0)
		{
			if(extentCount >= 4 && *desktop < extentCount / 4)
			{
				const int32_t globalX = static_cast<int32_t>(extents[*desktop * 4 + 0]);
				const int32_t globalY = static_cast<int32_t>(extents[*desktop * 4 + 1]);
				const int32_t globalWidth = static_cast<int32_t>(extents[*desktop * 4 + 2]);
				const int32_t globalHeight = static_cast<int32_t>(extents[*desktop * 4 + 3]);

				if(areaX < globalX)
				{
					areaWidth -= globalX - areaX;
					areaX = globalX;
				}

				if(areaY < globalY)
				{
					areaHeight -= globalY - areaY;
					areaY = globalY;
				}

				if(areaX + areaWidth > globalX + globalWidth)
					areaWidth = globalX - areaX + globalWidth;
				if(areaY + areaHeight > globalY + globalHeight)
					areaHeight = globalY - areaY + globalHeight;
			}
		}

		if(extents)
			s_Data.XLIB.Free(extents);
		if(desktop)
			s_Data.XLIB.Free(desktop);
	}

	xPos = areaX;
	yPos = areaY;
	width = areaWidth;
	height = areaHeight;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowVisibleX11(const InternalWindow* window)
{
	XWindowAttributes wa;
	s_Data.XLIB.GetWindowAttributes(s_Data.display, window->Handle, &wa);

	return wa.map_state == IsViewable;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximizedX11(const InternalWindow* window)
{
	Atom* states = nullptr;
	bool maximized = false;

	if(!s_Data.NET_WM_STATE || !s_Data.NET_WM_STATE_MAXIMIZED_VERT || !s_Data.NET_WM_STATE_MAXIMIZED_HORZ)
		return maximized;

	const uint64_t count = GetWindowPropertyX11(window->Handle, s_Data.NET_WM_STATE, XA_ATOM,
	                                            reinterpret_cast<uint8_t**>(&states));

	for(uint64_t i = 0; i < count; i++)
	{
		if(states[i] == s_Data.NET_WM_STATE_MAXIMIZED_VERT || states[i] == s_Data.NET_WM_STATE_MAXIMIZED_HORZ)
		{
			maximized = true;
			break;
		}
	}

	if(states)
		s_Data.XLIB.Free(states);

	return maximized;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMinimizedX11(const InternalWindow* window)
{
	return GetWindowState(window) == IconicState;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPollEventsX11()
{
	DrainEmptyEvents();

	Input::DetectControllerConnectionLinux();

	s_Data.XLIB.Pending(s_Data.display);

	while(QLength(s_Data.display))
	{
		XEvent event;
		s_Data.XLIB.NextEvent(s_Data.display, &event);
		ProcessEvent(event);
	}

	InternalWindow* window = s_Data.DisabledCursorWindow;
	if(window)
	{
		int32_t width = 0, height = 0;
		PlatformGetWindowSizeX11(window, width, height);

		if(window->LastCursorPosX != width / 2 || window->LastCursorPosY != height / 2)
			PlatformSetCursorPosX11(window, static_cast<double>(width) / 2, static_cast<double>(height) / 2);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocusedX11(const InternalWindow* window)
{
	::Window focused = 0;
	int32_t state = 0;

	s_Data.XLIB.GetInputFocus(s_Data.display, &focused, &state);

	return window->Handle == focused;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHoveredX11(const InternalWindow* window)
{
	::Window w = s_Data.Root;
	while(w)
	{
		::Window root = 0;
		int32_t rootX = 0, rootY = 0, childX = 0, childY = 0;
		uint32_t mask = 0;

		GrabErrorHandlerX11();

		const int32_t result = s_Data.XLIB.QueryPointer(s_Data.display, w, &root, &w, &rootX, &rootY, &childX,
		                                                &childY, &mask);

		ReleaseErrorHandlerX11();

		if (s_Data.ErrorCode == BadWindow)
			w = s_Data.Root;
		else if(!result)
			return false;
		else if(w == window->Handle)
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformRawMouseMotionSupportedX11()
{
	return s_Data.XI.Available;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotionX11(const InternalWindow* window, const bool enabled)
{
	if(!s_Data.XI.Available)
		return;

	if(s_Data.DisabledCursorWindow != window)
		return;

	if(enabled)
		EnableRawMouseMotion(window);
	else
		DisableRawMouseMotion(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetProgressX11(const InternalWindow* /*window*/, const ProgressState /*state*/,
													   const uint32_t /*completed*/)
{
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::INTERNAL::WindowingAPI::PlatformGetKeyScanCodeX11(const Input::Key key)
{
	return s_Data.ScanCodes[static_cast<uint32_t>(key)];
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeNameX11(const int32_t scanCode)
{
	if(!s_Data.XKB.Available)
		return nullptr;

	if(scanCode < 0 || scanCode > 0xFF || s_Data.KeyCodes[scanCode] == Input::Key::Unknown)
	{
		InputError(Error::Invalid_Value, "Invalid scancode" + std::to_string(scanCode));
		return nullptr;
	}

	const int32_t key = static_cast<int32_t>(s_Data.KeyCodes[scanCode]);
	const KeySym keySym = s_Data.XKB.KeycodeToKeysym(s_Data.display, scanCode,
											         static_cast<int32_t>(s_Data.XKB.Group), 0);
	if(keySym == NoSymbol)
		return nullptr;

	const uint32_t ch = KeySymToUnicode(keySym);
	if(ch == 0xFFFFFFFFu)
		return nullptr;

	const std::size_t count = EncodeUTF8(s_Data.KeyNames[key].data(), ch);
	if(count == 0)
		return nullptr;

	s_Data.KeyNames[key][count] = '\0';

	return s_Data.KeyNames[key].data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetClipboardStringX11(const std::string& string)
{
	s_Data.ClipboardString = string;

	s_Data.XLIB.SetSelectionOwner(s_Data.display, s_Data.CLIPBOARD, s_Data.HelperWindowHandle, CurrentTime);

	if(s_Data.XLIB.GetSelectionOwner(s_Data.display, s_Data.CLIPBOARD) != s_Data.HelperWindowHandle)
		InputError(Error::Platform_Error, "[X11] Failed to become owner of clipboard selection!");
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::PlatformGetClipboardStringX11()
{
	return GetSelectionString(s_Data.CLIPBOARD);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetRequiredInstanceExtensionsX11(std::array<std::string, 2>& extensions)
{
	if(!s_Data.VK.KHR_Surface)
		return;

	if(!s_Data.VK.KHR_XCB_Surface || !s_Data.XCB.Handle)
	{
		if(!s_Data.VK.KHR_XLib_Surface)
			return;
	}

	extensions[0] = "VK_KHR_surface";

	//TODO Wayland support should prefer wayland if available & on wayland session

	//NOTE: VK_KHR_xcb_surface is preferred due to some early ICDs exposing but not correctly implementing
	//      VK_KHR_xlib_surface
	if(s_Data.VK.KHR_XCB_Surface && s_Data.XCB.Handle)
		extensions[1] = "VK_KHR_xcb_surface";
	else
		extensions[1] = "VK_KHR_xlib_surface";
}

//-------------------------------------------------------------------------------------------------------------------//

VkResult TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowSurfaceX11(VkInstance instance, const InternalWindow* window,
																   const VkAllocationCallbacks* allocator,
																   VkSurfaceKHR& surface)
{
	if(s_Data.VK.KHR_XCB_Surface && s_Data.XCB.Handle)
	{
		xcb_connection_t* connection = s_Data.XCB.GetXCBConnection(s_Data.display);
		if(!connection)
		{
			InputError(Error::Platform_Error, "[X11] Failed to retrieve XCB connection!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		const PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(vkGetInstanceProcAddr
			(
				instance, "vkCreateXcbSurfaceKHR"
			));
		if(!vkCreateXcbSurfaceKHR)
		{
			InputError(Error::API_Unavailable, "[X11] Vulkan instance missing VK_KHR_xcb_surface extension!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		VkXcbSurfaceCreateInfoKHR sci{};
		sci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		sci.connection = connection;
		sci.window = window->Handle;

		const VkResult err = vkCreateXcbSurfaceKHR(instance, &sci, allocator, &surface);
		if(err)
			InputError(Error::Platform_Error,
			           std::string("[X11] Failed to create Vulkan XCB surface: ") + GetVulkanResultString(err));

		return err;
	}

	const PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(vkGetInstanceProcAddr
		(
			instance, "vkCreateXlibSurfaceKHR"
		));
	if(!vkCreateXlibSurfaceKHR)
	{
		InputError(Error::API_Unavailable, "[X11] Vulkan instance missing VK_KHR_xlib_surface extension");
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	VkXlibSurfaceCreateInfoKHR sci{};
	sci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	sci.dpy = s_Data.display;
	sci.window = window->Handle;

	const VkResult err = vkCreateXlibSurfaceKHR(instance, &sci, allocator, &surface);
	if(err)
		InputError(Error::Platform_Error,
					std::string("[X11] Failed to create Vulkan X11 surface: ") + GetVulkanResultString(err));

	return err;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindowX11(const InternalWindow* window)
{
	if(!s_Data.NET_WM_STATE || !s_Data.NET_WM_STATE_MAXIMIZED_VERT || !s_Data.NET_WM_STATE_MAXIMIZED_HORZ)
	   return;

	if(PlatformWindowVisibleX11(window))
	{
		SendEventToWM(window, s_Data.NET_WM_STATE, _NET_WM_STATE_ADD,
		              static_cast<int64_t>(s_Data.NET_WM_STATE_MAXIMIZED_VERT),
		              static_cast<int64_t>(s_Data.NET_WM_STATE_MAXIMIZED_HORZ), 1, 0);
	}
	else
	{
		Atom* states = nullptr;
		const uint64_t count = GetWindowPropertyX11(window->Handle, s_Data.NET_WM_STATE, XA_ATOM,
		                                            reinterpret_cast<uint8_t**>(&states));

		//NOTE: We don't check for failure as this property may not exist yet and that's fine (and we'll create it
		//      implicitly with append)
		std::array<Atom, 2> missing =
		{
			s_Data.NET_WM_STATE_MAXIMIZED_VERT,
			s_Data.NET_WM_STATE_MAXIMIZED_HORZ
		};
		uint64_t missingCount = 2;

		for(uint64_t i = 0; i < count; i++)
		{
			for(uint64_t j = 0; j < missingCount; j++)
			{
				if(states[i] == missing[j])
				{
					missing[j] = missing[missingCount - 1];
					missingCount--;
				}
			}
		}

		if(states)
			s_Data.XLIB.Free(states);

		if(!missingCount)
			return;

		s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_STATE, XA_ATOM, 32, PropModeAppend,
		                           reinterpret_cast<uint8_t*>(missing.data()), static_cast<int32_t>(missingCount));
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindowX11(const InternalWindow* window)
{
	if(window->OverrideRedirect)
	{
		//Override-redirect windows cannot be minimized or restored, as those tasks are performed by the
		//window manager
		InputError(Error::Platform_Error,
		           "[X11] Minimization of full screen windows requires a WM that supports EWMH full screen");
		return;
	}

	s_Data.XLIB.IconifyWindow(s_Data.display, window->Handle, s_Data.Screen);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttentionX11(const InternalWindow* window)
{
	if(!s_Data.NET_WM_STATE || !s_Data.NET_WM_STATE_DEMANDS_ATTENTION)
		return;

	SendEventToWM(window, s_Data.NET_WM_STATE, _NET_WM_STATE_ADD,
	              static_cast<int64_t>(s_Data.NET_WM_STATE_DEMANDS_ATTENTION), 0, 1, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowX11(const InternalWindow* window)
{
	s_Data.XLIB.UnmapWindow(s_Data.display, window->Handle);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindowX11(InternalWindow* window)
{
	if(window->OverrideRedirect)
	{
		//Override-redirect windows cannot be minimized or restored, as those tasks are performed by
		//the window manager
		InputError(Error::Platform_Error,
		           "[X11] Maximization of full screen windows requires a WM that supports EWMH full screen");
		return;
	}

	if(PlatformWindowMinimizedX11(window))
	{
		s_Data.XLIB.MapWindow(s_Data.display, window->Handle);
		WaitForVisibilityNotify(window);
	}
	else if(PlatformWindowVisibleX11(window))
	{
		if(s_Data.NET_WM_STATE && s_Data.NET_WM_STATE_MAXIMIZED_VERT && s_Data.NET_WM_STATE_MAXIMIZED_HORZ)
			SendEventToWM(window, s_Data.NET_WM_STATE, _NET_WM_STATE_REMOVE,
			              static_cast<int64_t>(s_Data.NET_WM_STATE_MAXIMIZED_VERT),
			              static_cast<int64_t>(s_Data.NET_WM_STATE_MAXIMIZED_HORZ), 1, 0);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimitsX11(InternalWindow* window, int32_t /*minWidth*/,
                                                               int32_t /*minHeight*/, int32_t /*maxWidth*/,
                                                               int32_t /*maxHeight*/)
{
	int32_t width = 0, height = 0;
	PlatformGetWindowSizeX11(window, width, height);
	UpdateNormalHints(window, width, height);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

//Enable XI2 raw mouse motion events
void TRAP::INTERNAL::WindowingAPI::EnableRawMouseMotion(const InternalWindow* /*window*/)
{
	XIEventMask em;
	std::array<uint8_t, XIMaskLen(XI_RawMotion)> mask = {0};

	em.deviceid = XIAllMasterDevices;
	em.mask_len = mask.size();
	em.mask = mask.data();
	XISetMask(mask.data(), XI_RawMotion);

	s_Data.XI.SelectEvents(s_Data.display, s_Data.Root, &em, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Disable XI2 raw mouse motion events
void TRAP::INTERNAL::WindowingAPI::DisableRawMouseMotion(const InternalWindow* /*window*/)
{
	XIEventMask em;
	std::array<uint8_t, 1> mask = {0};

	em.deviceid = XIAllMasterDevices;
	em.mask_len = mask.size();
	em.mask = mask.data();

	s_Data.XI.SelectEvents(s_Data.display, s_Data.Root, &em, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Reports the specified error, appending information about the last X Error
void TRAP::INTERNAL::WindowingAPI::InputErrorX11(const Error error, const std::string& message)
{
	std::vector<char> buffer{};
	buffer.resize(1024);
	s_Data.XLIB.GetErrorText(s_Data.display, s_Data.ErrorCode, buffer.data(), static_cast<int32_t>(buffer.size()));

	buffer.shrink_to_fit();

	InputError(error, "[X11] " + message + ": " + buffer.data());
}

//-------------------------------------------------------------------------------------------------------------------//

//Process the specified X event
void TRAP::INTERNAL::WindowingAPI::ProcessEvent(XEvent& event)
{
	int32_t keyCode = 0;
	bool filtered = false;

	//HACK: Save scanCode as some IMs clear the field in s_Data.XLIB.FilterEvent
	if(event.type == KeyPress || event.type == KeyRelease)
		keyCode = static_cast<int32_t>(event.xkey.keycode);

	filtered = s_Data.XLIB.FilterEvent(&event, 0);

	if(s_Data.RandR.Available)
	{
		if(event.type == s_Data.RandR.EventBase + RRNotify)
		{
			s_Data.RandR.UpdateConfiguration(&event);
			PollMonitorsX11();
			return;
		}
	}

	if(s_Data.XKB.Available)
	{
		if(event.type == s_Data.XKB.EventBase + XkbEventCode)
		{
			if(reinterpret_cast<XkbEvent*>(&event)->any.xkb_type == XkbStateNotify &&
			   (reinterpret_cast<XkbEvent*>(&event)->state.changed & XkbGroupStateMask))
			{
				s_Data.XKB.Group = reinterpret_cast<XkbEvent*>(&event)->state.group;
				InputKeyboardLayout();
			}

			return;
		}
	}

	if(event.type == GenericEvent)
	{
		if(s_Data.XI.Available)
		{
			InternalWindow* window = s_Data.DisabledCursorWindow;

			if(window && window->RawMouseMotion && event.xcookie.extension == s_Data.XI.MajorOPCode &&
			   s_Data.XLIB.GetEventData(s_Data.display, &event.xcookie) && event.xcookie.evtype == XI_RawMotion)
			{
				const XIRawEvent* re = reinterpret_cast<XIRawEvent*>(event.xcookie.data);
				if(re->valuators.mask_len)
				{
					const double* values = re->raw_values;
					double xPos = window->VirtualCursorPosX;
					double yPos = window->VirtualCursorPosY;

					if(XIMaskIsSet(re->valuators.mask, 0))
					{
						xPos += *values;
						values++;
					}

					if(XIMaskIsSet(re->valuators.mask, 1))
						yPos += *values;

					InputCursorPos(window, xPos, yPos);
				}
			}

			s_Data.XLIB.FreeEventData(s_Data.display, &event.xcookie);
		}

		return;
	}

	if(event.type == SelectionRequest)
	{
		HandleSelectionRequest(event);
		return;
	}

	InternalWindow* window = nullptr;
	if(s_Data.XLIB.FindContext(s_Data.display, event.xany.window, s_Data.Context,
	                           reinterpret_cast<XPointer*>(&window)) != 0)
		//This is an event for a window that has already been destroyed
		return;

	switch(event.type)
	{
	case ReparentNotify:
	{
		window->Parent = event.xreparent.parent;
		return;
	}

	case KeyPress:
	{
		const Input::Key key = TranslateKey(keyCode);

		if(window->IC)
		{
			//HACK: Do not report the key press events duplicated by XIM
			//      Duplicate key releases are filtered out implicitly by
			//      the TRAP key repeat loagic in InputKey
			//      A timestamp per key is used to handle simultaneous keys
			//NOTE: Always allow the first event for each key through
			//      (the server never sends a timestamp of zero)
			//NOTE: Timestamp difference is compared to handle wrap-around
			const Time diff = event.xkey.time - window->KeyPressTimes[keyCode];
			if(diff == event.xkey.time || (diff > 0 && diff < (static_cast<Time>(1u) << 31u)))
			{
				if(keyCode)
					InputKey(window, key, keyCode, Input::KeyState::Pressed);

				window->KeyPressTimes[keyCode] = event.xkey.time;
			}

			if(!filtered)
			{
				int32_t count = 0;
				int32_t status = 0;

				std::string buffer{};
				buffer.resize(100);
				char* chars = buffer.data();

				count = s_Data.XLIB.UTF8LookupString(window->IC, &event.xkey, buffer.data(),
				                                     static_cast<int32_t>(buffer.size()) - 1, nullptr, &status);

				if(status == XBufferOverflow)
				{
					buffer.resize(count + 1);
					chars = buffer.data();
					count = s_Data.XLIB.UTF8LookupString(window->IC, &event.xkey, chars, count, nullptr, &status);
				}

				if(status == XLookupChars || status == XLookupBoth)
				{
					const char* c = chars;
					while(c - chars < count)
						InputChar(window, DecodeUTF8(&c));
				}
			}
		}
		else
		{
			KeySym keySym = 0;
			s_Data.XLIB.LookupString(&event.xkey, nullptr, 0, &keySym, nullptr);

			InputKey(window, static_cast<Input::Key>(key), keyCode, Input::KeyState::Pressed);

			const uint32_t character = KeySymToUnicode(keySym);
			if(character != InvalidCodepoint)
				InputChar(window, character);
		}

		return;
	}

	case KeyRelease:
	{
		const Input::Key key = TranslateKey(keyCode);

		if(!s_Data.XKB.Detectable)
		{
			//HACK: Key repeat events will arrive as KeyRelease/KeyPress pairs with similar or identical time stamps
			//      The key repeat Logic in InputKey expects only key presses to repeat, so detect and discard release events
			if(s_Data.XLIB.EventsQueued(s_Data.display, QueuedAfterReading))
			{
				XEvent next;
				s_Data.XLIB.PeekEvent(s_Data.display, &next);

				if(next.type == KeyPress &&
					next.xkey.window == event.xkey.window &&
					next.xkey.keycode == static_cast<uint32_t>(keyCode))
				{
					//HACK: The time of repeat events sometimes doesn't match that of the press event, so add an epsilon
					if((next.xkey.time - event.xkey.time) < 20)
					{
						//This is very likely a server-generated key repeat event, so ignore it
						return;
					}
				}
			}
		}

		InputKey(window, key, keyCode, Input::KeyState::Released);
		return;
	}

	case ButtonPress:
	{
		if(event.xbutton.button == Button1)
			InputMouseClick(window, Input::MouseButton::Left, Input::KeyState::Pressed);
		else if(event.xbutton.button == Button2)
			InputMouseClick(window, Input::MouseButton::Middle, Input::KeyState::Pressed);
		else if(event.xbutton.button == Button3)
			InputMouseClick(window, Input::MouseButton::Right, Input::KeyState::Pressed);

		//Modern X provides scroll events as mouse button presses
		else if(event.xbutton.button == Button4)
			InputScroll(window, 0.0, 1.0);
		else if(event.xbutton.button == Button5)
			InputScroll(window, 0.0, -1.0);
		else if(event.xbutton.button == Button6)
			InputScroll(window, 1.0, 0.0);
		else if(event.xbutton.button == Button7)
			InputScroll(window, -1.0, 0.0);

		else
		{
			//Additional buttons after 7 are treated as regular buttons
			//We subtract 4 to fill the gap left by scroll input above
			InputMouseClick(window, static_cast<Input::MouseButton>(event.xbutton.button - Button1 - 4), Input::KeyState::Pressed);
		}

		return;
	}

	case ButtonRelease:
	{
		if(event.xbutton.button == Button1)
			InputMouseClick(window, Input::MouseButton::Left, Input::KeyState::Released);
		else if(event.xbutton.button == Button2)
			InputMouseClick(window, Input::MouseButton::Middle, Input::KeyState::Released);
		else if(event.xbutton.button == Button3)
			InputMouseClick(window, Input::MouseButton::Right, Input::KeyState::Released);
		else if(event.xbutton.button > Button7)
		{
			//Additional buttons after 7 are treated as regular buttons
			//We subtract 4 to fill the gap left by scroll input above
			InputMouseClick(window, static_cast<Input::MouseButton>(event.xbutton.button - Button1 - 4), Input::KeyState::Released);
		}

		return;
	}

	case EnterNotify:
	{
		//XEnterWindowEvent is XCrossingEvent
		const int32_t x = event.xcrossing.x;
		const int32_t y = event.xcrossing.y;

		//HACK: This is a workaround for WMs (KWM, Fluxbox) that otherwise ignore the defined cursor
		//      for hidden cursor mode
		if(window->cursorMode == CursorMode::Hidden)
			UpdateCursorImage(window);

		InputCursorEnter(window, true);
		InputCursorPos(window, x, y);

		window->LastCursorPosX = x;
		window->LastCursorPosY = y;
		return;
	}

	case LeaveNotify:
	{
		InputCursorEnter(window, false);
		return;
	}

	case MotionNotify:
	{
		const int32_t x = event.xmotion.x;
		const int32_t y = event.xmotion.y;

		if(x != window->WarpCursorPosX || y != window->WarpCursorPosY)
		{
			//The cursor was moved by something other than TRAP
			if(window->cursorMode == CursorMode::Disabled)
			{
				if(s_Data.DisabledCursorWindow != window)
					return;
				if(window->RawMouseMotion)
					return;

				const int32_t dx = x - window->LastCursorPosX;
				const int32_t dy = y - window->LastCursorPosY;

				InputCursorPos(window, window->VirtualCursorPosX + dx, window->VirtualCursorPosY + dy);
			}
			else
				InputCursorPos(window, x, y);
		}

		window->LastCursorPosX = x;
		window->LastCursorPosY = y;
		return;
	}

	case ConfigureNotify:
	{
		if(event.xconfigure.width != window->Width || event.xconfigure.height != window->Height)
		{
			InputFrameBufferSize(window, event.xconfigure.width, event.xconfigure.height);

			InputWindowSize(window, event.xconfigure.width, event.xconfigure.height);

			window->Width = event.xconfigure.width;
			window->Height = event.xconfigure.height;
		}

		int32_t xPos = event.xconfigure.x;
		int32_t yPos = event.xconfigure.y;

		//NOTE: ConfigureNotify events from the server are in local
		//      coordinates, so if any reparented we need to translate
		//      the position into root (screen) coordinates
		if(!event.xany.send_event && window->Parent != s_Data.Root)
		{
			GrabErrorHandlerX11();

			::Window dummy = 0;
			s_Data.XLIB.TranslateCoordinates(s_Data.display, window->Parent, s_Data.Root, xPos, yPos, &xPos, &yPos, &dummy);

			ReleaseErrorHandlerX11();
			if(s_Data.ErrorCode == BadWindow)
				return;
		}

		if(xPos != window->XPos || yPos != window->YPos)
		{
			InputWindowPos(window, xPos, yPos);
			window->XPos = xPos;
			window->YPos = yPos;
		}

		return;
	}

	case ClientMessage:
	{
		//Custom client message, probably from the window manager

		if(filtered)
			return;

		if(event.xclient.message_type == None)
			return;

		if(event.xclient.message_type == s_Data.WM_PROTOCOLS)
		{
			const Atom protocol = event.xclient.data.l[0];
			if(protocol == None)
				return;

			if(protocol == s_Data.WM_DELETE_WINDOW)
			{
				//The window manager was asked to close the window, for example by the user pressing a 'close'
				//window decoration button
				InputWindowCloseRequest(window);
			}
			else if(protocol == s_Data.NET_WM_PING)
			{
				//The window manager is pinging the application to ensure it's still responding to events
				XEvent reply = event;
				reply.xclient.window = s_Data.Root;

				s_Data.XLIB.SendEvent(s_Data.display, s_Data.Root, 0,
				                      SubstructureNotifyMask | SubstructureRedirectMask, &reply);
			}
		}
		else if(event.xclient.message_type == s_Data.XDNDEnter)
		{
			//A drag operation has entered the window
			uint64_t count = 0;
			Atom* formats = nullptr;
			const bool list = event.xclient.data.l[1] & 1;

			s_Data.XDND.Source = event.xclient.data.l[0];
			s_Data.XDND.Version = static_cast<int32_t>(event.xclient.data.l[1] >> 24);
			s_Data.XDND.Format = None;

			if(s_Data.XDND.Version > TRAP_XDND_VERSION)
				return;

			if(list)
				count = GetWindowPropertyX11(s_Data.XDND.Source, s_Data.XDNDTypeList, XA_ATOM,
				                             reinterpret_cast<uint8_t**>(&formats));
			else
			{
				count = 3;
				formats = reinterpret_cast<Atom*>(event.xclient.data.l) + 2;
			}

			for(uint32_t i = 0; i < count; i++)
			{
				if(formats[i] == s_Data.text_uri_list)
				{
					s_Data.XDND.Format = s_Data.text_uri_list;
					break;
				}
			}

			if(list && formats)
				s_Data.XLIB.Free(formats);
		}
		else if(event.xclient.message_type == s_Data.XDNDDrop)
		{
			//The drag operation has finished by dropping on the window
			Time time = CurrentTime;

			if(s_Data.XDND.Version > TRAP_XDND_VERSION)
				return;

			if(s_Data.XDND.Format)
			{
				if(s_Data.XDND.Version >= 1)
					time = event.xclient.data.l[2];

				//Request the chosen format from the source window
				s_Data.XLIB.ConvertSelection(s_Data.display, s_Data.XDNDSelection, s_Data.XDND.Format,
				                             s_Data.XDNDSelection, window->Handle, time);
			}
			else if(s_Data.XDND.Version >= 2)
			{
				XEvent reply = {ClientMessage};
				reply.xclient.window = s_Data.XDND.Source;
				reply.xclient.message_type = s_Data.XDNDFinished;
				reply.xclient.format = 32;
				reply.xclient.data.l[0] = static_cast<int64_t>(window->Handle);
				reply.xclient.data.l[1] = 0; //The drag was rejected
				reply.xclient.data.l[2] = None;

				s_Data.XLIB.SendEvent(s_Data.display, s_Data.XDND.Source, 0, NoEventMask, &reply);
				s_Data.XLIB.Flush(s_Data.display);
			}
		}
		else if(event.xclient.message_type == s_Data.XDNDPosition)
		{
			//The drag operation has moved over the window
			const int32_t xAbs = static_cast<int32_t>((event.xclient.data.l[2] >> 16) & 0xFFFF);
			const int32_t yAbs = static_cast<int32_t>((event.xclient.data.l[2]) & 0xFFFF);
			::Window dummy = 0;
			int32_t xPos = 0, yPos = 0;

			if(s_Data.XDND.Version > TRAP_XDND_VERSION)
				return;

			s_Data.XLIB.TranslateCoordinates(s_Data.display, s_Data.Root, window->Handle, xAbs, yAbs, &xPos, &yPos,
			                                 &dummy);

			InputCursorPos(window, xPos, yPos);

			XEvent reply = {ClientMessage};
			reply.xclient.window = s_Data.XDND.Source;
			reply.xclient.message_type = s_Data.XDNDStatus;
			reply.xclient.format = 32;
			reply.xclient.data.l[0] = static_cast<int64_t>(window->Handle);
			reply.xclient.data.l[2] = 0; //Specify an empty rectangle
			reply.xclient.data.l[3] = 0;

			if(s_Data.XDND.Format)
			{
				//Reply that we are ready to copy the dragged data
				reply.xclient.data.l[1] = 1; //Accept with no rectangle
				if(s_Data.XDND.Version >= 2)
					reply.xclient.data.l[4] = static_cast<int64_t>(s_Data.XDNDActionCopy);
			}

			s_Data.XLIB.SendEvent(s_Data.display, s_Data.XDND.Source, 0, NoEventMask, &reply);
			s_Data.XLIB.Flush(s_Data.display);
		}

		return;
	}

	case SelectionNotify:
	{
		if(event.xselection.property == s_Data.XDNDSelection)
		{
			//The converted data from the drag operation has arrived
			char* data = nullptr;
			const uint64_t result = GetWindowPropertyX11(event.xselection.requestor,
														 event.xselection.property,
			                                             event.xselection.target,
														 reinterpret_cast<uint8_t**>(&data));

			if(result)
			{
				int32_t count = 0;
				const std::vector<std::string> paths = ParseUriList(data, count);

				InputDrop(window, paths);
			}

			if(data)
				s_Data.XLIB.Free(data);

			if(s_Data.XDND.Version >= 2)
			{
				XEvent reply = {ClientMessage};
				reply.xclient.window = s_Data.XDND.Source;
				reply.xclient.message_type = s_Data.XDNDFinished;
				reply.xclient.format = 32;
				reply.xclient.data.l[0] = static_cast<int64_t>(window->Handle);
				reply.xclient.data.l[1] = static_cast<int64_t>(result);
				reply.xclient.data.l[2] = static_cast<int64_t>(s_Data.XDNDActionCopy);

				s_Data.XLIB.SendEvent(s_Data.display, s_Data.XDND.Source, 0, NoEventMask, &reply);
				s_Data.XLIB.Flush(s_Data.display);
			}
		}

		return;
	}

	case FocusIn:
	{
		if(event.xfocus.mode == NotifyGrab || event.xfocus.mode == NotifyUngrab)
		{
			//Ignore focus events from popup indicator windows, window menu key chords and window dragging
			return;
		}

		if (window->cursorMode == CursorMode::Disabled)
			DisableCursor(window);
		else if (window->cursorMode == CursorMode::Captured)
			CaptureCursor(window);

		if(window->IC)
			s_Data.XLIB.SetICFocus(window->IC);

		InputWindowFocus(window, true);
		return;
	}

	case FocusOut:
	{
		if(event.xfocus.mode == NotifyGrab || event.xfocus.mode == NotifyUngrab)
		{
			//Ignore focus events from popup indicator windows, window menu key chords and window dragging
			return;
		}

		if(window->cursorMode == CursorMode::Disabled)
			EnableCursor(window);
		else if (window->cursorMode == CursorMode::Captured)
			ReleaseCursor();

		if(window->IC)
			s_Data.XLIB.UnsetICFocus(window->IC);

		if(window->Monitor && !window->BorderlessFullscreen)
			PlatformMinimizeWindowX11(window);

		InputWindowFocus(window, false);
		return;
	}

	case Expose:
	{
		return;
	}

	case PropertyNotify:
	{
		if(event.xproperty.state != PropertyNewValue)
			return;

		if(event.xproperty.atom == s_Data.WM_STATE)
		{
			const int32_t state = GetWindowState(window);
			if(state != IconicState && state != NormalState)
				return;

			const bool minimized = (state == IconicState);
			if(window->Minimized != minimized)
			{
				if(window->Monitor)
				{
					if(minimized)
						ReleaseMonitor(window);
					else
					{
						if(window->BorderlessFullscreen)
							AcquireMonitorBorderless(window);
						else
							AcquireMonitor(window);
					}
				}

				window->Minimized = minimized;
				InputWindowMinimize(window, minimized);
			}
		}
		else if(event.xproperty.atom == s_Data.NET_WM_STATE)
		{
			const bool maximized = PlatformWindowMaximizedX11(window);
			if (window->Maximized != maximized)
			{
				window->Maximized = maximized;
				InputWindowMaximize(window, maximized);
			}
		}

		return;
	}

	case DestroyNotify:
	{
		return;
	}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Translates an X11 key code to a TRAP key token
TRAP::Input::Key TRAP::INTERNAL::WindowingAPI::TranslateKey(const int32_t scanCode)
{
	//Use the pre-filled LUT (see CreateKeyTables())
	if(scanCode < 0 || scanCode > 255)
		return Input::Key::Unknown;

	return s_Data.KeyCodes[scanCode];
}

//-------------------------------------------------------------------------------------------------------------------//

//Decode a Unicode code point from a UTF-8 stream
uint32_t TRAP::INTERNAL::WindowingAPI::DecodeUTF8(const char** s)
{
	uint32_t ch = 0, count = 0;
	static const std::array<uint32_t, 6> offsets =
	{
		0x00000000u, 0x00003080u, 0x000E2080u,
		0x03C82080u, 0xFA082080u, 0x82082080u
	};

	do
	{
		ch = (ch << 6) + static_cast<uint8_t>(**s);
		(*s)++;
		count++;
	} while((**s & 0xC0) == 0x80);

	TRAP_ASSERT(count <= 6, "");

	return ch - offsets[count - 1];
}

//-------------------------------------------------------------------------------------------------------------------//

//Splits and translates a text/uri-list into separate file paths
std::vector<std::string> TRAP::INTERNAL::WindowingAPI::ParseUriList(char* text, int32_t& count)
{
	const std::string prefix = "file://";
	std::vector<std::string> paths{};
	const char* line = nullptr;

	count = 0;

	while((line = std::strtok(text, "\r\n")))
	{
		text = nullptr;

		if(line[0] == '#')
			continue;

		if(std::strncmp(line, prefix.data(), prefix.size()) == 0)
		{
			line += prefix.size();
			while(*line != '/')
				line++;
		}

		count++;

		std::string path;
		path.resize(std::strlen(line) + 1);
		paths.resize(count);
		paths[count - 1] = path;
		char* pathPtr = paths[count - 1].data();

		while(*line)
		{
			if(line[0] == '%' && line[1] && line[2])
			{
				const std::array<char, 3> digits = { line[1], line[2], '\0'};
				*pathPtr = static_cast<char>(strtol(digits.data(), nullptr, 16));
				line += 2;
			}
			else
				*pathPtr = *line;

			pathPtr++;
			line++;
		}
	}

	return paths;
}

//-------------------------------------------------------------------------------------------------------------------//

//Make the specified window and its video mode active on its monitor
void TRAP::INTERNAL::WindowingAPI::AcquireMonitor(InternalWindow* window)
{
	if(s_Data.Saver.Count == 0)
	{
		//Remember old screen saver settings
		s_Data.XLIB.GetScreenSaver(s_Data.display, &s_Data.Saver.Timeout, &s_Data.Saver.Interval,
		                           &s_Data.Saver.Blanking, &s_Data.Saver.Exposure);

		//Disable screen saver
		s_Data.XLIB.SetScreenSaver(s_Data.display, 0, 0, DontPreferBlanking, DefaultExposures);
	}

	if(!window->Monitor->Window)
		s_Data.Saver.Count++;

	SetVideoModeX11(window->Monitor, window->videoMode);

	if(window->OverrideRedirect)
	{
		int32_t xPos = 0, yPos = 0;
		InternalVideoMode mode;

		//Manually position the window over its monitor
		PlatformGetMonitorPosX11(window->Monitor, xPos, yPos);
		mode = PlatformGetVideoModeX11(window->Monitor);

		s_Data.XLIB.MoveResizeWindow(s_Data.display, window->Handle, xPos, yPos, mode.Width, mode.Height);
	}

	window->Monitor->Window = window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Remove the window and restore the original video mode
void TRAP::INTERNAL::WindowingAPI::ReleaseMonitor(const InternalWindow* window)
{
	if(window->Monitor->Window != window)
		return;

	window->Monitor->Window = nullptr;
	RestoreVideoModeX11(window->Monitor);

	s_Data.Saver.Count--;

	if(s_Data.Saver.Count == 0)
	{
		//Restore old screen saver settings
		s_Data.XLIB.SetScreenSaver(s_Data.display, s_Data.Saver.Timeout, s_Data.Saver.Interval,
		                           s_Data.Saver.Blanking, s_Data.Saver.Exposure);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Set the current video mode for the specified monitor
void TRAP::INTERNAL::WindowingAPI::SetVideoModeX11(InternalMonitor* monitor, const InternalVideoMode& desired)
{
	if(!s_Data.RandR.Available || s_Data.RandR.MonitorBroken)
		return;

	InternalVideoMode current;
	RRMode native = 0;

	const InternalVideoMode* best = ChooseVideoMode(monitor, desired);
	current = PlatformGetVideoModeX11(monitor);

	XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);
	XRRCrtcInfo* ci = s_Data.RandR.GetCrtcInfo(s_Data.display, sr, monitor->CRTC);
	XRROutputInfo* oi = s_Data.RandR.GetOutputInfo(s_Data.display, sr, monitor->Output);

	for(uint32_t i = 0; i < static_cast<uint32_t>(oi->nmode); i++)
	{
		const XRRModeInfo* mi = GetModeInfo(sr, oi->modes[i]);
		if(!((mi->modeFlags & RR_Interlace) == 0))
			continue;

		const InternalVideoMode mode = VideoModeFromModeInfo(mi, ci);
		if(CompareVideoModes(best, &mode) == 0)
		{
			native = mi->id;
			break;
		}
	}

	if(native)
	{
		if(monitor->OldMode == 0)
			monitor->OldMode = ci->mode;

		s_Data.RandR.SetCrtcConfig(s_Data.display, sr, monitor->CRTC, CurrentTime, ci->x, ci->y, native,
		                           ci->rotation, ci->outputs, ci->noutput);
	}

	s_Data.RandR.FreeOutputInfo(oi);
	s_Data.RandR.FreeCrtcInfo(ci);
	s_Data.RandR.FreeScreenResources(sr);
}

//-------------------------------------------------------------------------------------------------------------------//

//Restore the saved(original) video mode for the specified monitor
void TRAP::INTERNAL::WindowingAPI::RestoreVideoModeX11(InternalMonitor* monitor)
{
	if(!s_Data.RandR.Available || s_Data.RandR.MonitorBroken)
		return;

	if(monitor->OldMode == 0)
		return;

	XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);
	XRRCrtcInfo* ci = s_Data.RandR.GetCrtcInfo(s_Data.display, sr, monitor->CRTC);

	s_Data.RandR.SetCrtcConfig(s_Data.display, sr, monitor->CRTC, CurrentTime, ci->x, ci->y, monitor->OldMode,
	                           ci->rotation, ci->outputs, ci->noutput);

	s_Data.RandR.FreeCrtcInfo(ci);
	s_Data.RandR.FreeScreenResources(sr);

	monitor->OldMode = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

//Make the specified window active on its monitor
void TRAP::INTERNAL::WindowingAPI::AcquireMonitorBorderless(InternalWindow* window)
{
	if(s_Data.Saver.Count == 0)
	{
		//Remember old screen saver settings
		s_Data.XLIB.GetScreenSaver(s_Data.display, &s_Data.Saver.Timeout, &s_Data.Saver.Interval,
		                           &s_Data.Saver.Blanking, &s_Data.Saver.Exposure);

		//Disable screen saver
		s_Data.XLIB.SetScreenSaver(s_Data.display, 0, 0, DontPreferBlanking, DefaultExposures);
	}

	if(!window->Monitor->Window)
		s_Data.Saver.Count++;

	if(window->OverrideRedirect)
	{
		int32_t xPos = 0, yPos = 0;
		InternalVideoMode mode;

		//Manually position the window over its monitor
		PlatformGetMonitorPosX11(window->Monitor, xPos, yPos);
		mode = PlatformGetVideoModeX11(window->Monitor);

		s_Data.XLIB.MoveResizeWindow(s_Data.display, window->Handle, xPos, yPos, mode.Width, mode.Height);
	}

	window->Monitor->Window = window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a dummy window for behind-the-scenes work
::Window TRAP::INTERNAL::WindowingAPI::CreateHelperWindow()
{
	XSetWindowAttributes wa;
	wa.event_mask = PropertyChangeMask;

	return s_Data.XLIB.CreateWindow(s_Data.display, s_Data.Root, 0, 0, 1, 1, 0, 0, InputOnly,
	                                DefaultVisual(s_Data.display, s_Data.Screen), CWEventMask, &wa);
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the cursor image according to its cursor mode
void TRAP::INTERNAL::WindowingAPI::UpdateCursorImage(const InternalWindow* window)
{
	if(window->cursorMode == CursorMode::Normal || window->cursorMode == CursorMode::Captured)
	{
		if(window->Cursor)
			s_Data.XLIB.DefineCursor(s_Data.display, window->Handle, window->Cursor->Handle);
		else
			s_Data.XLIB.UndefineCursor(s_Data.display, window->Handle);
	}
	else
		s_Data.XLIB.DefineCursor(s_Data.display, window->Handle, s_Data.HiddenCursorHandle);
}

//-------------------------------------------------------------------------------------------------------------------//

//Apply disabled cursor mode to a focused window
void TRAP::INTERNAL::WindowingAPI::DisableCursor(InternalWindow* window)
{
	if(window->RawMouseMotion)
		EnableRawMouseMotion(window);

	s_Data.DisabledCursorWindow = window;
	PlatformGetCursorPosX11(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
	UpdateCursorImage(window);
	CenterCursorInContentArea(window);
	CaptureCursor(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Exit disabled cursor mode for the specified window
void TRAP::INTERNAL::WindowingAPI::EnableCursor(InternalWindow* window)
{
	if(window->RawMouseMotion)
		DisableRawMouseMotion(window);

	s_Data.DisabledCursorWindow = nullptr;
	ReleaseCursor();
	PlatformSetCursorPosX11(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
	UpdateCursorImage(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Clear its handle when the input context has been destroyed
void TRAP::INTERNAL::WindowingAPI::InputContextDestroyCallback(XIC, XPointer clientData, XPointer)
{
	InternalWindow* window = reinterpret_cast<InternalWindow*>(clientData);

	window->IC = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Allocates and returns a monitor object with the specified name and dimensions
TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalMonitor> TRAP::INTERNAL::WindowingAPI::CreateMonitor(std::string name)
{
	Scope<InternalMonitor> monitor = MakeScope<InternalMonitor>();
	if(!name.empty())
		monitor->Name = std::move(name);

	return monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create key code translation tables
void TRAP::INTERNAL::WindowingAPI::CreateKeyTables()
{
	int32_t scanCode = 0, scanCodeMin = 0, scanCodeMax = 0;

	std::fill(s_Data.KeyCodes.begin(), s_Data.KeyCodes.end(), Input::Key::Unknown);
	std::fill(s_Data.ScanCodes.begin(), s_Data.ScanCodes.end(), -1);

	if(s_Data.XKB.Available)
	{
		//Use XKB to determine physical key locations independently of the current keyboard layout

		const XkbDescPtr desc = s_Data.XKB.GetMap(s_Data.display, 0, XkbUseCoreKbd);
		s_Data.XKB.GetNames(s_Data.display, XkbKeyNamesMask | XkbKeyAliasesMask, desc);

		scanCodeMin = desc->min_key_code;
		scanCodeMax = desc->max_key_code;

		struct Keys
		{
			TRAP::Input::Key Key = TRAP::Input::Key::Unknown;
			std::string Name;
		};

		const std::array<Keys, 121> KeyMap =
		{
            {
                { TRAP::Input::Key::Grave_Accent, "TLDE"}, { TRAP::Input::Key::One, "AE01"},
                { TRAP::Input::Key::Two, "AE02"}, { TRAP::Input::Key::Three, "AE03"},
                { TRAP::Input::Key::Four, "AE04"}, { TRAP::Input::Key::Five, "AE05"},
                { TRAP::Input::Key::Six, "AE06"}, { TRAP::Input::Key::Seven, "AE07"},
                { TRAP::Input::Key::Eight, "AE08"}, { TRAP::Input::Key::Nine, "AE09"},
                { TRAP::Input::Key::Zero, "AE10"}, { TRAP::Input::Key::Minus, "AE11"},
                { TRAP::Input::Key::Equal, "AE12"}, { TRAP::Input::Key::Q, "AD01"},
                { TRAP::Input::Key::W, "AD02"}, { TRAP::Input::Key::E, "AD03"},
                { TRAP::Input::Key::R, "AD04"}, { TRAP::Input::Key::T, "AD05"},
                { TRAP::Input::Key::Y, "AD06"}, { TRAP::Input::Key::U, "AD07"},
                { TRAP::Input::Key::I, "AD08"}, { TRAP::Input::Key::O, "AD09"},
                { TRAP::Input::Key::P, "AD10"}, { TRAP::Input::Key::Left_Bracket, "AD11"},
                { TRAP::Input::Key::Right_Bracket, "AD12"}, { TRAP::Input::Key::A, "AC01"},
                { TRAP::Input::Key::S, "AC02"}, { TRAP::Input::Key::D, "AC03"},
                { TRAP::Input::Key::F, "AC04"}, { TRAP::Input::Key::G, "AC05"},
                { TRAP::Input::Key::H, "AC06"}, { TRAP::Input::Key::J, "AC07"},
                { TRAP::Input::Key::K, "AC08"}, { TRAP::Input::Key::L, "AC09"},
                { TRAP::Input::Key::Semicolon, "AC10"}, { TRAP::Input::Key::Apostrophe, "AC11"},
                { TRAP::Input::Key::Z, "AB01"}, { TRAP::Input::Key::X, "AB02"},
                { TRAP::Input::Key::C, "AB03"}, { TRAP::Input::Key::V, "AB04"},
                { TRAP::Input::Key::B, "AB05"}, { TRAP::Input::Key::N, "AB06"},
                { TRAP::Input::Key::M, "AB07"}, { TRAP::Input::Key::Comma, "AB08"},
                { TRAP::Input::Key::Period, "AB09"}, { TRAP::Input::Key::Slash, "AB10"},
                { TRAP::Input::Key::Backslash, "BKSL"}, { TRAP::Input::Key::World_1, "LSGT"},
                { TRAP::Input::Key::Space, "SPCE"}, { TRAP::Input::Key::Escape, "ESC"},
                { TRAP::Input::Key::Enter, "RTRN"}, { TRAP::Input::Key::Tab, "TAB"},
                { TRAP::Input::Key::Backspace, "BKSP"}, { TRAP::Input::Key::Insert, "INS"},
                { TRAP::Input::Key::Delete, "DELE"}, { TRAP::Input::Key::Right, "RGHT"},
                { TRAP::Input::Key::Left, "LEFT"}, { TRAP::Input::Key::Down, "DOWN"},
                { TRAP::Input::Key::Up, "UP"}, { TRAP::Input::Key::Page_Up, "PGUP"},
                { TRAP::Input::Key::Page_Down, "PGDN"}, { TRAP::Input::Key::Home, "HOME"},
                { TRAP::Input::Key::End, "END"}, { TRAP::Input::Key::Caps_Lock, "CAPS"},
                { TRAP::Input::Key::Scroll_Lock, "SCLK"}, { TRAP::Input::Key::Num_Lock, "NMLK"},
                { TRAP::Input::Key::Print_Screen, "PRSC"}, { TRAP::Input::Key::Pause, "PAUS"},
                { TRAP::Input::Key::F1, "FK01"}, { TRAP::Input::Key::F2, "FK02"},
                { TRAP::Input::Key::F3, "FK03"}, { TRAP::Input::Key::F4, "FK04"},
                { TRAP::Input::Key::F5, "FK05"}, { TRAP::Input::Key::F6, "FK06"},
                { TRAP::Input::Key::F7, "FK07"}, { TRAP::Input::Key::F8, "FK08"},
                { TRAP::Input::Key::F9, "FK09"}, { TRAP::Input::Key::F10, "FK10"},
                { TRAP::Input::Key::F11, "FK11"}, { TRAP::Input::Key::F12, "FK12"},
                { TRAP::Input::Key::F13, "FK13"}, { TRAP::Input::Key::F14, "FK14"},
                { TRAP::Input::Key::F15, "FK15"}, { TRAP::Input::Key::F16, "FK16"},
                { TRAP::Input::Key::F17, "FK17"}, { TRAP::Input::Key::F18, "FK18"},
                { TRAP::Input::Key::F19, "FK19"}, { TRAP::Input::Key::F20, "FK20"},
                { TRAP::Input::Key::F21, "FK21"}, { TRAP::Input::Key::F22, "FK22"},
                { TRAP::Input::Key::F23, "FK23"}, { TRAP::Input::Key::F24, "FK24"},
                { TRAP::Input::Key::F25, "FK25"}, { TRAP::Input::Key::KP_0, "KP0"},
                { TRAP::Input::Key::KP_1, "KP1"}, { TRAP::Input::Key::KP_2, "KP2"},
                { TRAP::Input::Key::KP_3, "KP3"}, { TRAP::Input::Key::KP_4, "KP4"},
                { TRAP::Input::Key::KP_5, "KP5"}, { TRAP::Input::Key::KP_6, "KP6"},
                { TRAP::Input::Key::KP_7, "KP7"}, { TRAP::Input::Key::KP_8, "KP8"},
                { TRAP::Input::Key::KP_9, "KP9"}, { TRAP::Input::Key::KP_Decimal, "KPDL"},
                { TRAP::Input::Key::KP_Divide, "KPDV"}, { TRAP::Input::Key::KP_Multiply, "KPMU"},
                { TRAP::Input::Key::KP_Subtract, "KPSU"}, { TRAP::Input::Key::KP_Add, "KPAD"},
                { TRAP::Input::Key::KP_Enter, "KPEN"}, { TRAP::Input::Key::KP_Equal, "KPEQ"},
                { TRAP::Input::Key::Left_Shift, "LFSH"}, { TRAP::Input::Key::Left_Control, "LCTL"},
                { TRAP::Input::Key::Left_ALT, "LALT"}, { TRAP::Input::Key::Left_Super, "LWIN"},
                { TRAP::Input::Key::Right_Shift, "RTSH"}, { TRAP::Input::Key::Right_Control, "RCTL"},
                { TRAP::Input::Key::Right_ALT, "RALT"}, { TRAP::Input::Key::Right_ALT, "LVL3"},
				{ TRAP::Input::Key::Right_ALT, "MDSW"}, { TRAP::Input::Key::Right_Super, "RWIN"},
                { TRAP::Input::Key::Menu, "MENU"}
            }
		};

		//Find the X11 key code -> TRAP key code mapping
		for(scanCode = scanCodeMin; scanCode <= scanCodeMax; scanCode++)
		{
			TRAP::Input::Key key = TRAP::Input::Key::Unknown;

			//Map the key name to a TRAP key code.
			//Note: We use the US keyboard layout.
			//Because function keys aren't mapped correctly when using traditional KeySym translations,
			//they are mapped here instead.
			for(const Keys& keyMapKey : KeyMap)
			{
				if(strncmp(desc->names->keys[scanCode].name, keyMapKey.Name.c_str(), XkbKeyNameLength) == 0)
				{
					key = keyMapKey.Key;
					break;
				}
			}

			//Fall back to key aliases in case the key name did not match
			for(uint32_t i = 0; i < desc->names->num_key_aliases; i++)
			{
				if(key != TRAP::Input::Key::Unknown)
					break;

				if(strncmp(desc->names->key_aliases[i].real, desc->names->keys[scanCode].name, XkbKeyNameLength) != 0)
					continue;

				for(const Keys& keyMapKey : KeyMap)
				{
					if(strncmp(desc->names->key_aliases[i].alias, keyMapKey.Name.c_str(), XkbKeyNameLength) == 0)
					{
						key = keyMapKey.Key;
						break;
					}
				}
			}

			s_Data.KeyCodes[scanCode] = key;
		}

		s_Data.XKB.FreeNames(desc, XkbKeyNamesMask, 1);
		s_Data.XKB.FreeKeyboard(desc, 0, 1);
	}
	else
		s_Data.XLIB.DisplayKeycodes(s_Data.display, &scanCodeMin, &scanCodeMax);

	int32_t width = 0;
	KeySym* keySyms = s_Data.XLIB.GetKeyboardMapping(s_Data.display, scanCodeMin, scanCodeMax - scanCodeMin + 1,
	                                                 &width);

	for(scanCode = scanCodeMin; scanCode <= scanCodeMax; scanCode++)
	{
		//Translate the un-translated key codes using traditional X11 KeySym lookups
		if(static_cast<int32_t>(s_Data.KeyCodes[scanCode]) < 0)
		{
			const std::size_t base = static_cast<std::size_t>((scanCode - scanCodeMin)) * width;
			s_Data.KeyCodes[scanCode] = TranslateKeySyms(&keySyms[base], width);
		}

		//Store the reverse translation for faster key name lookup
		if(static_cast<int32_t>(s_Data.KeyCodes[scanCode]) > 0)
			s_Data.ScanCodes[static_cast<int32_t>(s_Data.KeyCodes[scanCode])] = static_cast<int16_t>(scanCode);
	}

	s_Data.XLIB.Free(keySyms);
}

//-------------------------------------------------------------------------------------------------------------------//

//Grabs the cursor and confines it to the window
void TRAP::INTERNAL::WindowingAPI::CaptureCursor(InternalWindow* window)
{
	s_Data.XLIB.GrabPointer(s_Data.display, window->Handle, 1,
				            ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
				            GrabModeAsync, GrabModeAsync,
				            window->Handle,
							s_Data.HiddenCursorHandle,
				            CurrentTime);
}

//-------------------------------------------------------------------------------------------------------------------//

//Ungrabs the cursor
void TRAP::INTERNAL::WindowingAPI::ReleaseCursor()
{
	s_Data.XLIB.UngrabPointer(s_Data.display, CurrentTime);
}

//-------------------------------------------------------------------------------------------------------------------//

//Translate the X11 KeySyms for a key to a TRAP key
//NOTE: This is only used as a fallback, in case the XKB method fails
//      It is layout-dependent and will fail partially on most non-US layouts
TRAP::Input::Key TRAP::INTERNAL::WindowingAPI::TranslateKeySyms(const KeySym* keySyms, const int32_t width)
{
	if(width > 1)
	{
		switch(keySyms[1])
		{
		case XK_KP_0:
			return TRAP::Input::Key::KP_0;

		case XK_KP_1:
			return TRAP::Input::Key::KP_1;

		case XK_KP_2:
			return TRAP::Input::Key::KP_2;

		case XK_KP_3:
			return TRAP::Input::Key::KP_3;

		case XK_KP_4:
			return TRAP::Input::Key::KP_4;

		case XK_KP_5:
			return TRAP::Input::Key::KP_5;

		case XK_KP_6:
			return TRAP::Input::Key::KP_6;

		case XK_KP_7:
			return TRAP::Input::Key::KP_7;

		case XK_KP_8:
			return TRAP::Input::Key::KP_8;

		case XK_KP_9:
			return TRAP::Input::Key::KP_9;

		case XK_KP_Separator:
		case XK_KP_Decimal:
			return TRAP::Input::Key::KP_Decimal;

		case XK_KP_Equal:
			return TRAP::Input::Key::KP_Equal;

		case XK_KP_Enter:
			return TRAP::Input::Key::KP_Enter;

		default:
			break;
		}
	}

	switch(keySyms[0])
	{
		case XK_Escape:
			return TRAP::Input::Key::Escape;

		case XK_Tab:
			return TRAP::Input::Key::Tab;

		case XK_Shift_L:
			return TRAP::Input::Key::Left_Shift;

		case XK_Shift_R:
			return TRAP::Input::Key::Right_Shift;

		case XK_Control_L:
			return TRAP::Input::Key::Left_Control;

		case XK_Control_R:
			return TRAP::Input::Key::Right_Control;

		case XK_Meta_L:
		case XK_Alt_L:
			return TRAP::Input::Key::Left_ALT;

		case XK_Mode_switch: //Mapped to Alt_R on many keyboards
		case XK_ISO_Level3_Shift: //AltGr on at least some machines
		case XK_Meta_R:
		case XK_Alt_R:
			return TRAP::Input::Key::Right_ALT;

		case XK_Super_L:
			return TRAP::Input::Key::Left_Super;

		case XK_Super_R:
			return TRAP::Input::Key::Right_Super;

		case XK_Menu:
			return TRAP::Input::Key::Menu;

		case XK_Num_Lock:
			return TRAP::Input::Key::Num_Lock;

		case XK_Caps_Lock:
			return TRAP::Input::Key::Caps_Lock;

		case XK_Print:
			return TRAP::Input::Key::Print_Screen;

		case XK_Scroll_Lock:
			return TRAP::Input::Key::Scroll_Lock;

		case XK_Pause:
			return TRAP::Input::Key::Pause;

		case XK_Delete:
			return TRAP::Input::Key::Delete;

		case XK_BackSpace:
			return TRAP::Input::Key::Backspace;

		case XK_Return:
			return TRAP::Input::Key::Enter;

		case XK_Home:
			return TRAP::Input::Key::Home;

		case XK_End:
			return TRAP::Input::Key::End;

		case XK_Page_Up:
			return TRAP::Input::Key::Page_Up;

		case XK_Page_Down:
			return TRAP::Input::Key::Page_Down;

		case XK_Insert:
			return TRAP::Input::Key::Insert;

		case XK_Left:
			return TRAP::Input::Key::Left;

		case XK_Right:
			return TRAP::Input::Key::Right;

		case XK_Down:
			return TRAP::Input::Key::Down;

		case XK_Up:
			return TRAP::Input::Key::Up;

		case XK_F1:
			return TRAP::Input::Key::F1;

		case XK_F2:
			return TRAP::Input::Key::F2;

		case XK_F3:
			return TRAP::Input::Key::F3;

		case XK_F4:
			return TRAP::Input::Key::F4;

		case XK_F5:
			return TRAP::Input::Key::F5;

		case XK_F6:
			return TRAP::Input::Key::F6;

		case XK_F7:
			return TRAP::Input::Key::F7;

		case XK_F8:
			return TRAP::Input::Key::F8;

		case XK_F9:
			return TRAP::Input::Key::F9;

		case XK_F10:
			return TRAP::Input::Key::F10;

		case XK_F11:
			return TRAP::Input::Key::F11;

		case XK_F12:
			return TRAP::Input::Key::F12;

		case XK_F13:
			return TRAP::Input::Key::F13;

		case XK_F14:
			return TRAP::Input::Key::F14;

		case XK_F15:
			return TRAP::Input::Key::F15;

		case XK_F16:
			return TRAP::Input::Key::F16;

		case XK_F17:
			return TRAP::Input::Key::F17;

		case XK_F18:
			return TRAP::Input::Key::F18;

		case XK_F19:
			return TRAP::Input::Key::F19;

		case XK_F20:
			return TRAP::Input::Key::F20;

		case XK_F21:
			return TRAP::Input::Key::F21;

		case XK_F22:
			return TRAP::Input::Key::F22;

		case XK_F23:
			return TRAP::Input::Key::F23;

		case XK_F24:
			return TRAP::Input::Key::F24;

		case XK_F25:
			return TRAP::Input::Key::F25;

		//Numeric keypad
		case XK_KP_Divide:
			return TRAP::Input::Key::KP_Divide;

		case XK_KP_Multiply:
			return TRAP::Input::Key::KP_Multiply;

		case XK_KP_Subtract:
			return TRAP::Input::Key::KP_Subtract;

		case XK_KP_Add:
			return TRAP::Input::Key::KP_Add;

		//These should have been detected in seconday keysym test above!
		case XK_KP_Insert:
			return TRAP::Input::Key::KP_0;

		case XK_KP_End:
			return TRAP::Input::Key::KP_1;

		case XK_KP_Down:
			return TRAP::Input::Key::KP_2;

		case XK_KP_Page_Down:
			return TRAP::Input::Key::KP_3;

		case XK_KP_Left:
			return TRAP::Input::Key::KP_4;

		case XK_KP_Right:
			return TRAP::Input::Key::KP_6;

		case XK_KP_Home:
			return TRAP::Input::Key::KP_7;

		case XK_KP_Up:
			return TRAP::Input::Key::KP_8;

		case XK_KP_Page_Up:
			return TRAP::Input::Key::KP_9;

		case XK_KP_Delete:
			return TRAP::Input::Key::KP_Decimal;

		case XK_KP_Equal:
			return TRAP::Input::Key::KP_Equal;

		case XK_KP_Enter:
			return TRAP::Input::Key::KP_Enter;

		//Last resor: Check for printable keys (should not happen if the XKB extension is available).
		//This will give a layout dependent mapping (which is wrong, and we may miss some keys,
		//especially on non-US keyboards), but it is better than nothing...
		case XK_a:
			return TRAP::Input::Key::A;

		case XK_b:
			return TRAP::Input::Key::B;

		case XK_c:
			return TRAP::Input::Key::C;

		case XK_d:
			return TRAP::Input::Key::D;

		case XK_e:
			return TRAP::Input::Key::E;

		case XK_f:
			return TRAP::Input::Key::F;

		case XK_g:
			return TRAP::Input::Key::G;

		case XK_h:
			return TRAP::Input::Key::H;

		case XK_i:
			return TRAP::Input::Key::I;

		case XK_j:
			return TRAP::Input::Key::J;

		case XK_k:
			return TRAP::Input::Key::K;

		case XK_l:
			return TRAP::Input::Key::L;

		case XK_m:
			return TRAP::Input::Key::M;

		case XK_n:
			return TRAP::Input::Key::N;

		case XK_o:
			return TRAP::Input::Key::O;

		case XK_p:
			return TRAP::Input::Key::P;

		case XK_q:
			return TRAP::Input::Key::Q;

		case XK_r:
			return TRAP::Input::Key::R;

		case XK_s:
			return TRAP::Input::Key::S;

		case XK_t:
			return TRAP::Input::Key::T;

		case XK_u:
			return TRAP::Input::Key::U;

		case XK_v:
			return TRAP::Input::Key::V;

		case XK_w:
			return TRAP::Input::Key::W;

		case XK_x:
			return TRAP::Input::Key::X;

		case XK_y:
			return TRAP::Input::Key::Y;

		case XK_z:
			return TRAP::Input::Key::Z;

		case XK_1:
			return TRAP::Input::Key::One;

		case XK_2:
			return TRAP::Input::Key::Two;

		case XK_3:
			return TRAP::Input::Key::Three;

		case XK_4:
			return TRAP::Input::Key::Four;

		case XK_5:
			return TRAP::Input::Key::Five;

		case XK_6:
			return TRAP::Input::Key::Six;

		case XK_7:
			return TRAP::Input::Key::Seven;

		case XK_8:
			return TRAP::Input::Key::Eight;

		case XK_9:
			return TRAP::Input::Key::Nine;

		case XK_0:
			return TRAP::Input::Key::Zero;

		case XK_space:
			return TRAP::Input::Key::Space;

		case XK_minus:
			return TRAP::Input::Key::Minus;

		case XK_equal:
			return TRAP::Input::Key::Equal;

		case XK_bracketleft:
			return TRAP::Input::Key::Left_Bracket;

		case XK_bracketright:
			return TRAP::Input::Key::Right_Bracket;

		case XK_backslash:
			return TRAP::Input::Key::Backslash;

		case XK_semicolon:
			return TRAP::Input::Key::Semicolon;

		case XK_apostrophe:
			return TRAP::Input::Key::Apostrophe;

		case XK_grave:
			return TRAP::Input::Key::Grave_Accent;

		case XK_comma:
			return TRAP::Input::Key::Comma;

		case XK_period:
			return TRAP::Input::Key::Period;

		case XK_slash:
			return TRAP::Input::Key::Slash;

		case XK_less:
			return TRAP::Input::Key::World_1; //At least in some layouts...

		default:
			break;
	}

	//No matching translation was found
	return TRAP::Input::Key::Unknown;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::GetLinuxKeyboardLayoutNameX11()
{
	if (!s_Data.XKB.Available)
	{
		InputError(Error::API_Unavailable, "[Input][X11] XKB extension required for keyboard layout names");
		return "";
	}

	XkbStateRec state{};
	s_Data.XKB.GetState(s_Data.display, XkbUseCoreKbd, &state);

	const XkbDescPtr desc = s_Data.XKB.AllocKeyboard();
	if (s_Data.XKB.GetNames(s_Data.display, XkbGroupNamesMask, desc) != 0) //0 = Success
	{
		s_Data.XKB.FreeKeyboard(desc, 0, 1);
		InputError(Error::Platform_Error, "[Input][X11] Failed to retrieve keyboard layout names");
		return "";
	}

	const Atom atom = desc->names->groups[state.group];
	s_Data.XKB.FreeKeyboard(desc, 0, 1);

	if (atom == 0)
	{
		InputError(Error::Platform_Error, "[Input][X11] Name missing for current keyboard layout");
		return "";
	}

	return s_Data.XLIB.GetAtomName(s_Data.display, atom);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDropX11(InternalWindow* window, const bool value)
{
	if(value)
	{
		//Announce support for Xdnd (drag and drop)
		const Atom version = 5;
		s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.XDNDAware, XA_ATOM, 32,
			                       PropModeReplace, reinterpret_cast<const uint8_t*>(&version), 1);
	}
	else
		s_Data.XLIB.DeleteProperty(s_Data.display, window->Handle, s_Data.XDNDAware);
}

#endif