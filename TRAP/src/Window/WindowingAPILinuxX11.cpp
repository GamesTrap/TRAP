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

//Action for EWMH client messages
#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD 1
#define _NET_WM_STATE_TOGGLE 2

//Calculates the refresh rate, in Hz, from the specified RandR mode info
int32_t TRAP::INTERNAL::WindowingAPI::CalculateRefreshRate(const XRRModeInfo* mi)
{
	if(mi->hTotal && mi->vTotal)
		return static_cast<int32_t>(TRAP::Math::Round(static_cast<double>(mi->dotClock) /
		                                              (static_cast<double>(mi->hTotal) *
													  static_cast<double>(mi->vTotal))));
	else
		return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::VideoModeFromModeInfo(const XRRModeInfo* mi,
                                                                                                    const XRRCrtcInfo* ci)
{
	InternalVideoMode mode{};

	if(ci->rotation == RR_Rotate_90 || ci->rotation == RR_Rotate_270)
	{
		mode.Width = mi->height;
		mode.Height = mi->width;
	}
	else
	{
		mode.Width = mi->width;
		mode.Height = mi->height;
	}

	mode.RefreshRate = CalculateRefreshRate(mi);

	SplitBPP(DefaultDepth(s_Data.display, s_Data.Screen), mode.RedBits, mode.GreenBits, mode.BlueBits);

	return mode;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sends an EWMH or ICCCM event to the window manager
void TRAP::INTERNAL::WindowingAPI::SendEventToWM(const InternalWindow* window, Atom type, long a, long b,
                                                 long c, long d, long e)
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
int32_t TRAP::INTERNAL::WindowingAPI::IsFrameExtentsEvent(Display*, XEvent* event, XPointer pointer)
{
	InternalWindow* window = reinterpret_cast<InternalWindow*>(pointer);

	return event->type             == PropertyNotify   &&
	       event->xproperty.state  == PropertyNewValue &&
		   event->xproperty.window == window->Handle   &&
		   event->xproperty.atom   == s_Data.NET_FRAME_EXTENTS;
}

//-------------------------------------------------------------------------------------------------------------------//

//Wait for data to arrive on any of the specified file descriptors
bool TRAP::INTERNAL::WindowingAPI::WaitForData(pollfd* fds, nfds_t count, double* timeout)
{
	while(true)
	{
		if(timeout)
		{
			const uint64_t base = TRAP::Application::GetTime();

#if defined(__linux__) || defined(__FreeBDS__) || defined(__OpenBSD__) || defined(__CYGWIN__)
			const time_t seconds = static_cast<time_t>(*timeout);
			const long nanoseconds = static_cast<long>((*timeout - seconds) * 1e9);
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

			*timeout -= (TRAP::Application::GetTime() - base) / 60.0;

			if(result > 0)
				return true;
			else if(result == -1 && error != EINTR && error != EAGAIN)
				return false;
			else if(*timeout <= 0.0)
				return false;
		}
		else
		{
			const int result = poll(fds, count, -1);
			if(result > 0)
				return true;
			else if(result == -1 && errno != EINTR && errno != EAGAIN)
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
		const int32_t result = write(s_Data.EmptyEventPipe[1], &byte, 1);
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
		const int32_t result = read(s_Data.EmptyEventPipe[0], dummy.data(), dummy.size());
		if(result == -1 && errno != EINTR)
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::CreateEmptyEventPipe()
{
	if(pipe(s_Data.EmptyEventPipe.data()) != 0)
	{
		InputError(Error::Platform_Error, "X11: Failed to create empty event pipe: " + std::string(strerror(errno)));
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
			InputError(Error::Platform_Error, "X11: Failed to set flags for empty event pipe: " + std::string(strerror(errno)));
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieve a single window property of the specified type
unsigned long TRAP::INTERNAL::WindowingAPI::GetWindowPropertyX11(::Window window, Atom property, Atom type,
                                                                 uint8_t** value)
{
	Atom actualType{};
	int32_t actualFormat{};
	unsigned long itemCount{}, bytesAfter{};

	s_Data.XLIB.GetWindowProperty(s_Data.display,
					              window,
					              property,
					              0,
					              LONG_MAX,
					              0,
					              type,
					              &actualType,
					              &actualFormat,
					              &itemCount,
					              &bytesAfter,
					              value);

	return itemCount;
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the normal hints according to the window settings
void TRAP::INTERNAL::WindowingAPI::UpdateNormalHints(InternalWindow* window, int32_t width, int32_t height)
{
	XSizeHints* hints = s_Data.XLIB.AllocSizeHints();

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

	hints->flags |= PWinGravity;
	hints->win_gravity = StaticGravity;

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
						  1,
						  s_Data.NET_WM_STATE_FULLSCREEN,
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
			unsigned long value = 1;

			s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_BYPASS_COMPOSITOR, XA_CARDINAL,
			                           32, PropModeReplace, reinterpret_cast<uint8_t*>(&value), 1);
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
						  0,
						  s_Data.NET_WM_STATE_FULLSCREEN,
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
const XRRModeInfo* TRAP::INTERNAL::WindowingAPI::GetModeInfo(const XRRScreenResources* sr, RRMode id)
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
	char* rms = s_Data.XLIB.ResourceManagerString(s_Data.display);
	if(rms)
	{
		XrmDatabase db = s_Data.XRM.GetStringDatabase(rms);
		if(db)
		{
			XrmValue value;
			char* type = nullptr;

			if(s_Data.XRM.GetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value))
			{
				if(type && strcmp(type, "String") == 0)
					xDPI = yDPI = std::atof(value.addr);
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
	s_Data.XI.Handle = PlatformLoadModule("libXi-6.so");
#elif defined (__OpenBSD__) || defined(__NetBSD__)
	s_Data.XI.Handle = PlatformLoadModule("libXi.so");
#else
	s_Data.XI.Handle = PlatformLoadModule("libXi.so.6");
#endif
	if(s_Data.XI.Handle)
	{
		s_Data.XI.QueryVersion = reinterpret_cast<PFN_XIQueryVersion>(PlatformGetModuleSymbol(s_Data.XI.Handle, "XIQueryVersion"));
		s_Data.XI.SelectEvents = reinterpret_cast<PFN_XISelectEvents>(PlatformGetModuleSymbol(s_Data.XI.Handle, "XISelectEvents"));

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
	s_Data.RandR.Handle = PlatformLoadModule("libXrandr-2.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.RandR.Handle = PlatformLoadModule("libXrandr.so");
#else
	s_Data.RandR.Handle = PlatformLoadModule("libXrandr.so.2");
#endif
	if(s_Data.RandR.Handle)
	{
		s_Data.RandR.FreeCrtcInfo = reinterpret_cast<PFN_XRRFreeCrtcInfo>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                        "XRRFreeCrtcInfo"));
		s_Data.RandR.FreeOutputInfo = reinterpret_cast<PFN_XRRFreeOutputInfo>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                            "XRRFreeOutputInfo"));
		s_Data.RandR.FreeScreenResources = reinterpret_cast<PFN_XRRFreeScreenResources>(PlatformGetModuleSymbol
			(
				s_Data.RandR.Handle, "XRRFreeScreenResources"
			));
		s_Data.RandR.GetCrtcInfo = reinterpret_cast<PFN_XRRGetCrtcInfo>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                      "XRRGetCrtcInfo"));
		s_Data.RandR.GetOutputInfo = reinterpret_cast<PFN_XRRGetOutputInfo>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                          "XRRGetOutputInfo"));
		s_Data.RandR.GetOutputPrimary = reinterpret_cast<PFN_XRRGetOutputPrimary>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                                "XRRGetOutputPrimary"));
		s_Data.RandR.GetScreenResourcesCurrent = reinterpret_cast<PFN_XRRGetScreenResourcesCurrent>(PlatformGetModuleSymbol
			(
				s_Data.RandR.Handle, "XRRGetScreenResourcesCurrent"
			));
		s_Data.RandR.QueryExtension = reinterpret_cast<PFN_XRRQueryExtension>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                            "XRRQueryExtension"));
		s_Data.RandR.QueryVersion = reinterpret_cast<PFN_XRRQueryVersion>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                        "XRRQueryVersion"));
		s_Data.RandR.SelectInput = reinterpret_cast<PFN_XRRSelectInput>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                      "XRRSelectInput"));
		s_Data.RandR.SetCrtcConfig = reinterpret_cast<PFN_XRRSetCrtcConfig>(PlatformGetModuleSymbol(s_Data.RandR.Handle,
		                                                                          "XRRSetCrtcConfig"));
		s_Data.RandR.UpdateConfiguration = reinterpret_cast<PFN_XRRUpdateConfiguration>(PlatformGetModuleSymbol
			(
				s_Data.RandR.Handle, "XRRUpdateConfiguration"
			));

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
	s_Data.XCursor.Handle = PlatformLoadModule("libXcursor-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.XCursor.Handle = PlatformLoadModule("libXcursor.so");
#else
	s_Data.XCursor.Handle = PlatformLoadModule("libXcursor.so.1");
#endif
	if(s_Data.XCursor.Handle)
	{
		s_Data.XCursor.ImageCreate = reinterpret_cast<PFN_XcursorImageCreate>(PlatformGetModuleSymbol(s_Data.XCursor.Handle,
		                                                                            "XcursorImageCreate"));
		s_Data.XCursor.ImageDestroy = reinterpret_cast<PFN_XcursorImageDestroy>(PlatformGetModuleSymbol(s_Data.XCursor.Handle,
		                                                                              "XcursorImageDestroy"));
		s_Data.XCursor.ImageLoadCursor = reinterpret_cast<PFN_XcursorImageLoadCursor>(PlatformGetModuleSymbol
			(
				s_Data.XCursor.Handle, "XcursorImageLoadCursor"
			));
		s_Data.XCursor.GetTheme = reinterpret_cast<PFN_XcursorGetTheme>(PlatformGetModuleSymbol(s_Data.XCursor.Handle,
													                          "XcursorGetTheme"));
		s_Data.XCursor.GetDefaultSize = reinterpret_cast<PFN_XcursorGetDefaultSize>(PlatformGetModuleSymbol(s_Data.XCursor.Handle,
		                                                                                  "XcursorGetDefaultSize"));
		s_Data.XCursor.LibraryLoadImage = reinterpret_cast<PFN_XcursorLibraryLoadImage>(PlatformGetModuleSymbol
			(
				s_Data.XCursor.Handle, "XcursorLibraryLoadImage"
			));
	}

#if defined(__CYGWIN__)
	s_Data.Xinerama.Handle = PlatformLoadModule("libXinerama-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.Xinerama.Handle = PlatformLoadModule("libXinerama.so");
#else
	s_Data.Xinerama.Handle = PlatformLoadModule("libXinerama.so.1");
#endif
	if(s_Data.Xinerama.Handle)
	{
		s_Data.Xinerama.IsActive = reinterpret_cast<PFN_XineramaIsActive>(PlatformGetModuleSymbol(s_Data.Xinerama.Handle,
		                                                                        "XineramaIsActive"));
		s_Data.Xinerama.QueryExtension = reinterpret_cast<PFN_XineramaQueryExtension>(PlatformGetModuleSymbol
			(
				s_Data.Xinerama.Handle, "XineramaQueryExtension"
			));
		s_Data.Xinerama.QueryScreens = reinterpret_cast<PFN_XineramaQueryScreens>(PlatformGetModuleSymbol(s_Data.Xinerama.Handle,
		                                                                                "XineramaQueryScreens"));

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
		int32_t supported;

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
	s_Data.XCB.Handle = PlatformLoadModule("libX11-xcb-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.XCB.Handle = PlatformLoadModule("libX11-xcb.so");
#else
	s_Data.XCB.Handle = PlatformLoadModule("libX11-xcb.so.1");
#endif
	if(s_Data.XCB.Handle)
		s_Data.XCB.GetXCBConnection = reinterpret_cast<PFN_XGetXCBConnection>(PlatformGetModuleSymbol(s_Data.XCB.Handle,
		                                                                            "XGetXCBConnection"));

#if defined(__CYGWIN__)
	s_Data.XRender.Handle = PlatformLoadModule("libXrender-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.XRender.Handle = PlatformLoadModule("libXrender.so");
#else
	s_Data.XRender.Handle = PlatformLoadModule("libXrender.so.1");
#endif
	if(s_Data.XRender.Handle)
	{
		s_Data.XRender.QueryExtension = reinterpret_cast<PFN_XRenderQueryExtension>(PlatformGetModuleSymbol(s_Data.XRender.Handle,
		                                                                                  "XRenderQueryExtension"));
		s_Data.XRender.QueryVersion = reinterpret_cast<PFN_XRenderQueryVersion>(PlatformGetModuleSymbol(s_Data.XRender.Handle,
		                                                                              "XRenderQueryVersion"));
		s_Data.XRender.FindVisualFormat = reinterpret_cast<PFN_XRenderFindVisualFormat>(PlatformGetModuleSymbol
			(
				s_Data.XRender.Handle, "XRenderFindVisualFormat"
			));

		if(s_Data.XRender.QueryExtension(s_Data.display, &s_Data.XRender.ErrorBase, &s_Data.XRender.EventBase))
		{
			if(s_Data.XRender.QueryVersion(s_Data.display, &s_Data.XRender.Major, &s_Data.XRender.Minor))
				s_Data.XRender.Available = true;
		}
	}

#if defined(__CYGWIN__)
	s_Data.XShape.Handle = PlatformLoadModule("libXext-6.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.XShape.Handle = PlatformLoadModule("libXext.so");
#else
	s_Data.XShape.Handle = PlatformLoadModule("libXext.so.6");
#endif
	if(s_Data.XShape.Handle)
	{
		s_Data.XShape.QueryExtension = reinterpret_cast<PFN_XShapeQueryExtension>(PlatformGetModuleSymbol(s_Data.XShape.Handle,
		                                                                                "XShapeQueryExtension"));
		s_Data.XShape.CombineRegion = reinterpret_cast<PFN_XShapeCombineRegion>(PlatformGetModuleSymbol(s_Data.XShape.Handle,
		                                                                              "XShapeCombineRegion"));
		s_Data.XShape.CombineMask = reinterpret_cast<PFN_XShapeCombineMask>(PlatformGetModuleSymbol(s_Data.XShape.Handle,
		                                                                          "XShapeCombineMask"));
		s_Data.XShape.QueryVersion = reinterpret_cast<PFN_XShapeQueryVersion>(PlatformGetModuleSymbol(s_Data.XShape.Handle,
		                                                                            "XShapeQueryVersion"));

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
	const unsigned long atomCount = GetWindowPropertyX11(s_Data.Root, s_Data.NET_SUPPORTED, XA_ATOM,
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
	s_Data.ErrorCode = 0; //0 = Success
	s_Data.XLIB.SetErrorHandler(ErrorHandler);
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
	s_Data.XLIB.SetErrorHandler(nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

//Return the atom ID only if it is listed in the specified array
Atom TRAP::INTERNAL::WindowingAPI::GetAtomIfSupported(Atom* supportedAtoms, unsigned long atomCount, const char* atomName)
{
	const Atom atom = s_Data.XLIB.InternAtom(s_Data.display, atomName, 0);

	for(unsigned long i = 0; i < atomCount; i++)
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
	std::array<uint8_t, 16 * 16 * 4> pixels = {0};
	return CreateCursorX11(std::move(TRAP::Image::LoadFromMemory(16, 16, TRAP::Image::ColorFormat::RGBA,
	                                                             std::vector<uint8_t>(pixels.begin(),
																 pixels.end()))), 0, 0);
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

	s_Data.IM = s_Data.XLIB.OpenIM(s_Data.display, 0, nullptr, nullptr);
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
		callback.callback = (XIMProc)InputMethodDestroyCallback;
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

	int32_t disconnectedCount, screenCount = 0;
	std::vector<InternalMonitor*> disconnected{};
	XineramaScreenInfo* screens = nullptr;
	XRRScreenResources* sr = s_Data.RandR.GetScreenResourcesCurrent(s_Data.display, s_Data.Root);
	RROutput primary = s_Data.RandR.GetOutputPrimary(s_Data.display, s_Data.Root);

	if(s_Data.Xinerama.Available)
		screens = s_Data.Xinerama.QueryScreens(s_Data.display, &screenCount);

	disconnectedCount = s_Data.Monitors.size();
	if(disconnectedCount)
	{
		disconnected.resize(s_Data.Monitors.size());
		for(uint32_t i = 0; i < s_Data.Monitors.size(); i++)
			disconnected[i] = s_Data.Monitors[i].get();
	}

	for(int32_t i = 0; i < sr->noutput; i++)
	{
		int32_t j, type;

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

	if(request->property == 0)
	{
		//The requestor is a legacy client
		//We don't support legacy clients, so fail here
		return 0;
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

		unsigned long count = GetWindowPropertyX11(request->requestor, request->property, s_Data.ATOM_PAIR,
		                                      reinterpret_cast<uint8_t**>(&targets));

		for(unsigned long i = 0; i < count; i += 2)
		{
			uint32_t j;

			for(j = 0; j < formats.size(); j++)
				if(targets[i] == formats[j])
					break;

			if(j < formats.size())
				s_Data.XLIB.ChangeProperty(s_Data.display, request->requestor, targets[i + 1], targets[i], 8,
				                           PropModeReplace, reinterpret_cast<uint8_t*>(selectionString.data()),
										   selectionString.size());
			else
				targets[i + 1] = 0;
		}

		s_Data.XLIB.ChangeProperty(s_Data.display, request->requestor, request->property, s_Data.ATOM_PAIR, 32,
                                   PropModeReplace, reinterpret_cast<uint8_t*>(targets), count);

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

	for(uint32_t i = 0; i < formats.size(); i++)
	{
		if(request->target == formats[i])
		{
			//The requested target is one we support

			s_Data.XLIB.ChangeProperty(s_Data.display, request->requestor, request->property, request->target, 8,
						               PropModeReplace, reinterpret_cast<uint8_t*>(selectionString.data()),
									   selectionString.size());

			return request->property;
		}
	}

	//The request target is not supported

	return 0;
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

void TRAP::INTERNAL::WindowingAPI::HandleSelectionClear(XEvent& event)
{
	if(event.xselectionclear.selection == s_Data.PRIMARY)
		s_Data.PrimarySelectionString.clear();
	else
		s_Data.ClipboardString.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

//Push contents of our selection to clipboard manager
void TRAP::INTERNAL::WindowingAPI::PushSelectionToManagerX11()
{
	s_Data.XLIB.ConvertSelection(s_Data.display, s_Data.CLIPBOARD_MANAGER, s_Data.SAVE_TARGETS, 0,
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

			case SelectionClear:
				HandleSelectionClear(event);
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

	unsigned long filter = 0;
	if(s_Data.XLIB.GetICValues(window->IC, XNFilterEvents, &filter, nullptr) == nullptr)
		s_Data.XLIB.SelectInput(s_Data.display, window->Handle, attribs.your_event_mask | filter);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::IsVisualTransparentX11(Visual* visual)
{
	if(!s_Data.XRender.Available)
		return false;

	XRenderPictFormat* pf = s_Data.XRender.FindVisualFormat(s_Data.display, visual);
	return pf && pf->direct.alphaMask;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create the X11 window (and its colormap)
bool TRAP::INTERNAL::WindowingAPI::CreateNativeWindow(InternalWindow* window, WindowConfig& WNDConfig,
                                                      Visual* visual, int32_t depth)
{
	int32_t width = WNDConfig.Width;
	int32_t height = WNDConfig.Height;

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
		PlatformSetWindowDecorated(window, false);

	if(s_Data.NET_WM_STATE && !window->Monitor)
	{
		std::array<Atom, 3> states{};
		int32_t count = 0;

		if(WNDConfig.Floating)
			if(s_Data.NET_WM_STATE_ABOVE)
				states[count++] = s_Data.NET_WM_STATE_ABOVE;

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
		long pid = getpid();
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

	UpdateNormalHints(window, width, height);

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

	PlatformSetWindowTitle(window, WNDConfig.Title);
	PlatformGetWindowPos(window, window->XPos, window->YPos);
	PlatformGetWindowSize(window, window->Width, window->Height);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a native cursor object from the specified image and hotspot
Cursor TRAP::INTERNAL::WindowingAPI::CreateCursorX11(const Scope<TRAP::Image>& image, int32_t xHotSpot,
                                                     int32_t yHotSpot)
{
	if(!s_Data.XCursor.Handle)
		return 0;

	XcursorImage* native = s_Data.XCursor.ImageCreate(image->GetWidth(), image->GetHeight());
	if(native == nullptr)
		return 0;

	native->xhot = xHotSpot;
	native->yhot = yHotSpot;

	const uint8_t* source = reinterpret_cast<const uint8_t*>(image->GetPixelData());
	XcursorPixel* target = native->pixels;

	for(uint32_t i = 0; i < image->GetWidth() * image->GetHeight(); i++, target++, source += 4)
	{
		uint32_t alpha = source[3];

		*target = (alpha << 24) |
		          (static_cast<uint8_t>((source[0] * alpha) / 255) << 16) |
				  (static_cast<uint8_t>((source[1] * alpha) / 255) <<  8) |
				  (static_cast<uint8_t>((source[2] * alpha) / 255) <<  0);
	}

	Cursor cursor = s_Data.XCursor.ImageLoadCursor(s_Data.display, native);
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
		result = state->State;

	if(state)
		s_Data.XLIB.Free(state);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<TRAP::INTERNAL::WindowingAPI::CodePair, 828> TRAP::INTERNAL::WindowingAPI::KeySymTab =
{
	{
		{ 0x01a1, 0x0104 }, { 0x01a2, 0x02d8 }, { 0x01a3, 0x0141 }, { 0x01a5, 0x013d },
		{ 0x01a6, 0x015a }, { 0x01a9, 0x0160 }, { 0x01aa, 0x015e }, { 0x01ab, 0x0164 },
		{ 0x01ac, 0x0179 }, { 0x01ae, 0x017d }, { 0x01af, 0x017b }, { 0x01b1, 0x0105 },
		{ 0x01b2, 0x02db }, { 0x01b3, 0x0142 }, { 0x01b5, 0x013e }, { 0x01b6, 0x015b },
		{ 0x01b7, 0x02c7 }, { 0x01b9, 0x0161 }, { 0x01ba, 0x015f }, { 0x01bb, 0x0165 },
		{ 0x01bc, 0x017a }, { 0x01bd, 0x02dd }, { 0x01be, 0x017e }, { 0x01bf, 0x017c },
		{ 0x01c0, 0x0154 }, { 0x01c3, 0x0102 }, { 0x01c5, 0x0139 }, { 0x01c6, 0x0106 },
		{ 0x01c8, 0x010c }, { 0x01ca, 0x0118 }, { 0x01cc, 0x011a }, { 0x01cf, 0x010e },
		{ 0x01d0, 0x0110 }, { 0x01d1, 0x0143 }, { 0x01d2, 0x0147 }, { 0x01d5, 0x0150 },
		{ 0x01d8, 0x0158 }, { 0x01d9, 0x016e }, { 0x01db, 0x0170 }, { 0x01de, 0x0162 },
		{ 0x01e0, 0x0155 }, { 0x01e3, 0x0103 }, { 0x01e5, 0x013a }, { 0x01e6, 0x0107 },
		{ 0x01e8, 0x010d }, { 0x01ea, 0x0119 }, { 0x01ec, 0x011b }, { 0x01ef, 0x010f },
		{ 0x01f0, 0x0111 }, { 0x01f1, 0x0144 }, { 0x01f2, 0x0148 }, { 0x01f5, 0x0151 },
		{ 0x01f8, 0x0159 }, { 0x01f9, 0x016f }, { 0x01fb, 0x0171 }, { 0x01fe, 0x0163 },
		{ 0x01ff, 0x02d9 }, { 0x02a1, 0x0126 }, { 0x02a6, 0x0124 }, { 0x02a9, 0x0130 },
		{ 0x02ab, 0x011e }, { 0x02ac, 0x0134 }, { 0x02b1, 0x0127 }, { 0x02b6, 0x0125 },
		{ 0x02b9, 0x0131 }, { 0x02bb, 0x011f }, { 0x02bc, 0x0135 }, { 0x02c5, 0x010a },
		{ 0x02c6, 0x0108 }, { 0x02d5, 0x0120 }, { 0x02d8, 0x011c }, { 0x02dd, 0x016c },
		{ 0x02de, 0x015c }, { 0x02e5, 0x010b }, { 0x02e6, 0x0109 }, { 0x02f5, 0x0121 },
		{ 0x02f8, 0x011d }, { 0x02fd, 0x016d }, { 0x02fe, 0x015d }, { 0x03a2, 0x0138 },
		{ 0x03a3, 0x0156 }, { 0x03a5, 0x0128 }, { 0x03a6, 0x013b }, { 0x03aa, 0x0112 },
		{ 0x03ab, 0x0122 }, { 0x03ac, 0x0166 }, { 0x03b3, 0x0157 }, { 0x03b5, 0x0129 },
		{ 0x03b6, 0x013c }, { 0x03ba, 0x0113 }, { 0x03bb, 0x0123 }, { 0x03bc, 0x0167 },
		{ 0x03bd, 0x014a }, { 0x03bf, 0x014b }, { 0x03c0, 0x0100 }, { 0x03c7, 0x012e },
		{ 0x03cc, 0x0116 }, { 0x03cf, 0x012a }, { 0x03d1, 0x0145 }, { 0x03d2, 0x014c },
		{ 0x03d3, 0x0136 }, { 0x03d9, 0x0172 }, { 0x03dd, 0x0168 }, { 0x03de, 0x016a },
		{ 0x03e0, 0x0101 }, { 0x03e7, 0x012f }, { 0x03ec, 0x0117 }, { 0x03ef, 0x012b },
		{ 0x03f1, 0x0146 }, { 0x03f2, 0x014d }, { 0x03f3, 0x0137 }, { 0x03f9, 0x0173 },
		{ 0x03fd, 0x0169 }, { 0x03fe, 0x016b }, { 0x047e, 0x203e }, { 0x04a1, 0x3002 },
		{ 0x04a2, 0x300c }, { 0x04a3, 0x300d }, { 0x04a4, 0x3001 }, { 0x04a5, 0x30fb },
		{ 0x04a6, 0x30f2 }, { 0x04a7, 0x30a1 }, { 0x04a8, 0x30a3 }, { 0x04a9, 0x30a5 },
		{ 0x04aa, 0x30a7 }, { 0x04ab, 0x30a9 }, { 0x04ac, 0x30e3 }, { 0x04ad, 0x30e5 },
		{ 0x04ae, 0x30e7 }, { 0x04af, 0x30c3 }, { 0x04b0, 0x30fc }, { 0x04b1, 0x30a2 },
		{ 0x04b2, 0x30a4 }, { 0x04b3, 0x30a6 }, { 0x04b4, 0x30a8 }, { 0x04b5, 0x30aa },
		{ 0x04b6, 0x30ab }, { 0x04b7, 0x30ad }, { 0x04b8, 0x30af }, { 0x04b9, 0x30b1 },
		{ 0x04ba, 0x30b3 }, { 0x04bb, 0x30b5 }, { 0x04bc, 0x30b7 }, { 0x04bd, 0x30b9 },
		{ 0x04be, 0x30bb }, { 0x04bf, 0x30bd }, { 0x04c0, 0x30bf }, { 0x04c1, 0x30c1 },
		{ 0x04c2, 0x30c4 }, { 0x04c3, 0x30c6 }, { 0x04c4, 0x30c8 }, { 0x04c5, 0x30ca },
		{ 0x04c6, 0x30cb }, { 0x04c7, 0x30cc }, { 0x04c8, 0x30cd }, { 0x04c9, 0x30ce },
		{ 0x04ca, 0x30cf }, { 0x04cb, 0x30d2 }, { 0x04cc, 0x30d5 }, { 0x04cd, 0x30d8 },
		{ 0x04ce, 0x30db }, { 0x04cf, 0x30de }, { 0x04d0, 0x30df }, { 0x04d1, 0x30e0 },
		{ 0x04d2, 0x30e1 }, { 0x04d3, 0x30e2 }, { 0x04d4, 0x30e4 }, { 0x04d5, 0x30e6 },
		{ 0x04d6, 0x30e8 }, { 0x04d7, 0x30e9 }, { 0x04d8, 0x30ea }, { 0x04d9, 0x30eb },
		{ 0x04da, 0x30ec }, { 0x04db, 0x30ed }, { 0x04dc, 0x30ef }, { 0x04dd, 0x30f3 },
		{ 0x04de, 0x309b }, { 0x04df, 0x309c }, { 0x05ac, 0x060c }, { 0x05bb, 0x061b },
		{ 0x05bf, 0x061f }, { 0x05c1, 0x0621 }, { 0x05c2, 0x0622 }, { 0x05c3, 0x0623 },
		{ 0x05c4, 0x0624 }, { 0x05c5, 0x0625 }, { 0x05c6, 0x0626 }, { 0x05c7, 0x0627 },
		{ 0x05c8, 0x0628 }, { 0x05c9, 0x0629 }, { 0x05ca, 0x062a }, { 0x05cb, 0x062b },
		{ 0x05cc, 0x062c }, { 0x05cd, 0x062d }, { 0x05ce, 0x062e }, { 0x05cf, 0x062f },
		{ 0x05d0, 0x0630 }, { 0x05d1, 0x0631 }, { 0x05d2, 0x0632 }, { 0x05d3, 0x0633 },
		{ 0x05d4, 0x0634 }, { 0x05d5, 0x0635 }, { 0x05d6, 0x0636 }, { 0x05d7, 0x0637 },
		{ 0x05d8, 0x0638 }, { 0x05d9, 0x0639 }, { 0x05da, 0x063a }, { 0x05e0, 0x0640 },
		{ 0x05e1, 0x0641 }, { 0x05e2, 0x0642 }, { 0x05e3, 0x0643 }, { 0x05e4, 0x0644 },
		{ 0x05e5, 0x0645 }, { 0x05e6, 0x0646 }, { 0x05e7, 0x0647 }, { 0x05e8, 0x0648 },
		{ 0x05e9, 0x0649 }, { 0x05ea, 0x064a }, { 0x05eb, 0x064b }, { 0x05ec, 0x064c },
		{ 0x05ed, 0x064d }, { 0x05ee, 0x064e }, { 0x05ef, 0x064f }, { 0x05f0, 0x0650 },
		{ 0x05f1, 0x0651 }, { 0x05f2, 0x0652 }, { 0x06a1, 0x0452 }, { 0x06a2, 0x0453 },
		{ 0x06a3, 0x0451 }, { 0x06a4, 0x0454 }, { 0x06a5, 0x0455 }, { 0x06a6, 0x0456 },
		{ 0x06a7, 0x0457 }, { 0x06a8, 0x0458 }, { 0x06a9, 0x0459 }, { 0x06aa, 0x045a },
		{ 0x06ab, 0x045b }, { 0x06ac, 0x045c }, { 0x06ae, 0x045e }, { 0x06af, 0x045f },
		{ 0x06b0, 0x2116 }, { 0x06b1, 0x0402 }, { 0x06b2, 0x0403 }, { 0x06b3, 0x0401 },
		{ 0x06b4, 0x0404 }, { 0x06b5, 0x0405 }, { 0x06b6, 0x0406 }, { 0x06b7, 0x0407 },
		{ 0x06b8, 0x0408 }, { 0x06b9, 0x0409 }, { 0x06ba, 0x040a }, { 0x06bb, 0x040b },
		{ 0x06bc, 0x040c }, { 0x06be, 0x040e }, { 0x06bf, 0x040f }, { 0x06c0, 0x044e },
		{ 0x06c1, 0x0430 }, { 0x06c2, 0x0431 }, { 0x06c3, 0x0446 }, { 0x06c4, 0x0434 },
		{ 0x06c5, 0x0435 }, { 0x06c6, 0x0444 }, { 0x06c7, 0x0433 }, { 0x06c8, 0x0445 },
		{ 0x06c9, 0x0438 }, { 0x06ca, 0x0439 }, { 0x06cb, 0x043a }, { 0x06cc, 0x043b },
		{ 0x06cd, 0x043c }, { 0x06ce, 0x043d }, { 0x06cf, 0x043e }, { 0x06d0, 0x043f },
		{ 0x06d1, 0x044f }, { 0x06d2, 0x0440 }, { 0x06d3, 0x0441 }, { 0x06d4, 0x0442 },
		{ 0x06d5, 0x0443 }, { 0x06d6, 0x0436 }, { 0x06d7, 0x0432 }, { 0x06d8, 0x044c },
		{ 0x06d9, 0x044b }, { 0x06da, 0x0437 }, { 0x06db, 0x0448 }, { 0x06dc, 0x044d },
		{ 0x06dd, 0x0449 }, { 0x06de, 0x0447 }, { 0x06df, 0x044a }, { 0x06e0, 0x042e },
		{ 0x06e1, 0x0410 }, { 0x06e2, 0x0411 }, { 0x06e3, 0x0426 }, { 0x06e4, 0x0414 },
		{ 0x06e5, 0x0415 }, { 0x06e6, 0x0424 }, { 0x06e7, 0x0413 }, { 0x06e8, 0x0425 },
		{ 0x06e9, 0x0418 }, { 0x06ea, 0x0419 }, { 0x06eb, 0x041a }, { 0x06ec, 0x041b },
		{ 0x06ed, 0x041c }, { 0x06ee, 0x041d }, { 0x06ef, 0x041e }, { 0x06f0, 0x041f },
		{ 0x06f1, 0x042f }, { 0x06f2, 0x0420 }, { 0x06f3, 0x0421 }, { 0x06f4, 0x0422 },
		{ 0x06f5, 0x0423 }, { 0x06f6, 0x0416 }, { 0x06f7, 0x0412 }, { 0x06f8, 0x042c },
		{ 0x06f9, 0x042b }, { 0x06fa, 0x0417 }, { 0x06fb, 0x0428 }, { 0x06fc, 0x042d },
		{ 0x06fd, 0x0429 }, { 0x06fe, 0x0427 }, { 0x06ff, 0x042a }, { 0x07a1, 0x0386 },
		{ 0x07a2, 0x0388 }, { 0x07a3, 0x0389 }, { 0x07a4, 0x038a }, { 0x07a5, 0x03aa },
		{ 0x07a7, 0x038c }, { 0x07a8, 0x038e }, { 0x07a9, 0x03ab }, { 0x07ab, 0x038f },
		{ 0x07ae, 0x0385 }, { 0x07af, 0x2015 }, { 0x07b1, 0x03ac }, { 0x07b2, 0x03ad },
		{ 0x07b3, 0x03ae }, { 0x07b4, 0x03af }, { 0x07b5, 0x03ca }, { 0x07b6, 0x0390 },
		{ 0x07b7, 0x03cc }, { 0x07b8, 0x03cd }, { 0x07b9, 0x03cb }, { 0x07ba, 0x03b0 },
		{ 0x07bb, 0x03ce }, { 0x07c1, 0x0391 }, { 0x07c2, 0x0392 }, { 0x07c3, 0x0393 },
		{ 0x07c4, 0x0394 }, { 0x07c5, 0x0395 }, { 0x07c6, 0x0396 }, { 0x07c7, 0x0397 },
		{ 0x07c8, 0x0398 }, { 0x07c9, 0x0399 }, { 0x07ca, 0x039a }, { 0x07cb, 0x039b },
		{ 0x07cc, 0x039c }, { 0x07cd, 0x039d }, { 0x07ce, 0x039e }, { 0x07cf, 0x039f },
		{ 0x07d0, 0x03a0 }, { 0x07d1, 0x03a1 }, { 0x07d2, 0x03a3 }, { 0x07d4, 0x03a4 },
		{ 0x07d5, 0x03a5 }, { 0x07d6, 0x03a6 }, { 0x07d7, 0x03a7 }, { 0x07d8, 0x03a8 },
		{ 0x07d9, 0x03a9 }, { 0x07e1, 0x03b1 }, { 0x07e2, 0x03b2 }, { 0x07e3, 0x03b3 },
		{ 0x07e4, 0x03b4 }, { 0x07e5, 0x03b5 }, { 0x07e6, 0x03b6 }, { 0x07e7, 0x03b7 },
		{ 0x07e8, 0x03b8 }, { 0x07e9, 0x03b9 }, { 0x07ea, 0x03ba }, { 0x07eb, 0x03bb },
		{ 0x07ec, 0x03bc }, { 0x07ed, 0x03bd }, { 0x07ee, 0x03be }, { 0x07ef, 0x03bf },
		{ 0x07f0, 0x03c0 }, { 0x07f1, 0x03c1 }, { 0x07f2, 0x03c3 }, { 0x07f3, 0x03c2 },
		{ 0x07f4, 0x03c4 }, { 0x07f5, 0x03c5 }, { 0x07f6, 0x03c6 }, { 0x07f7, 0x03c7 },
		{ 0x07f8, 0x03c8 }, { 0x07f9, 0x03c9 }, { 0x08a1, 0x23b7 }, { 0x08a2, 0x250c },
		{ 0x08a3, 0x2500 }, { 0x08a4, 0x2320 }, { 0x08a5, 0x2321 }, { 0x08a6, 0x2502 },
		{ 0x08a7, 0x23a1 }, { 0x08a8, 0x23a3 }, { 0x08a9, 0x23a4 }, { 0x08aa, 0x23a6 },
		{ 0x08ab, 0x239b }, { 0x08ac, 0x239d }, { 0x08ad, 0x239e }, { 0x08ae, 0x23a0 },
		{ 0x08af, 0x23a8 }, { 0x08b0, 0x23ac }, { 0x08bc, 0x2264 }, { 0x08bd, 0x2260 },
		{ 0x08be, 0x2265 }, { 0x08bf, 0x222b }, { 0x08c0, 0x2234 }, { 0x08c1, 0x221d },
		{ 0x08c2, 0x221e }, { 0x08c5, 0x2207 }, { 0x08c8, 0x223c }, { 0x08c9, 0x2243 },
		{ 0x08cd, 0x21d4 }, { 0x08ce, 0x21d2 }, { 0x08cf, 0x2261 }, { 0x08d6, 0x221a },
		{ 0x08da, 0x2282 }, { 0x08db, 0x2283 }, { 0x08dc, 0x2229 }, { 0x08dd, 0x222a },
		{ 0x08de, 0x2227 }, { 0x08df, 0x2228 }, { 0x08ef, 0x2202 }, { 0x08f6, 0x0192 },
		{ 0x08fb, 0x2190 }, { 0x08fc, 0x2191 }, { 0x08fd, 0x2192 }, { 0x08fe, 0x2193 },
		{ 0x09e0, 0x25c6 }, { 0x09e1, 0x2592 }, { 0x09e2, 0x2409 }, { 0x09e3, 0x240c },
		{ 0x09e4, 0x240d }, { 0x09e5, 0x240a }, { 0x09e8, 0x2424 }, { 0x09e9, 0x240b },
		{ 0x09ea, 0x2518 }, { 0x09eb, 0x2510 }, { 0x09ec, 0x250c }, { 0x09ed, 0x2514 },
		{ 0x09ee, 0x253c }, { 0x09ef, 0x23ba }, { 0x09f0, 0x23bb }, { 0x09f1, 0x2500 },
		{ 0x09f2, 0x23bc }, { 0x09f3, 0x23bd }, { 0x09f4, 0x251c }, { 0x09f5, 0x2524 },
		{ 0x09f6, 0x2534 }, { 0x09f7, 0x252c }, { 0x09f8, 0x2502 }, { 0x0aa1, 0x2003 },
		{ 0x0aa2, 0x2002 }, { 0x0aa3, 0x2004 }, { 0x0aa4, 0x2005 }, { 0x0aa5, 0x2007 },
		{ 0x0aa6, 0x2008 }, { 0x0aa7, 0x2009 }, { 0x0aa8, 0x200a }, { 0x0aa9, 0x2014 },
		{ 0x0aaa, 0x2013 }, { 0x0aae, 0x2026 }, { 0x0aaf, 0x2025 }, { 0x0ab0, 0x2153 },
		{ 0x0ab1, 0x2154 }, { 0x0ab2, 0x2155 }, { 0x0ab3, 0x2156 }, { 0x0ab4, 0x2157 },
		{ 0x0ab5, 0x2158 }, { 0x0ab6, 0x2159 }, { 0x0ab7, 0x215a }, { 0x0ab8, 0x2105 },
		{ 0x0abb, 0x2012 }, { 0x0abc, 0x2329 }, { 0x0abe, 0x232a }, { 0x0ac3, 0x215b },
		{ 0x0ac4, 0x215c }, { 0x0ac5, 0x215d }, { 0x0ac6, 0x215e }, { 0x0ac9, 0x2122 },
		{ 0x0aca, 0x2613 }, { 0x0acc, 0x25c1 }, { 0x0acd, 0x25b7 }, { 0x0ace, 0x25cb },
		{ 0x0acf, 0x25af }, { 0x0ad0, 0x2018 }, { 0x0ad1, 0x2019 }, { 0x0ad2, 0x201c },
		{ 0x0ad3, 0x201d }, { 0x0ad4, 0x211e }, { 0x0ad6, 0x2032 }, { 0x0ad7, 0x2033 },
		{ 0x0ad9, 0x271d }, { 0x0adb, 0x25ac }, { 0x0adc, 0x25c0 }, { 0x0add, 0x25b6 },
		{ 0x0ade, 0x25cf }, { 0x0adf, 0x25ae }, { 0x0ae0, 0x25e6 }, { 0x0ae1, 0x25ab },
		{ 0x0ae2, 0x25ad }, { 0x0ae3, 0x25b3 }, { 0x0ae4, 0x25bd }, { 0x0ae5, 0x2606 },
		{ 0x0ae6, 0x2022 }, { 0x0ae7, 0x25aa }, { 0x0ae8, 0x25b2 }, { 0x0ae9, 0x25bc },
		{ 0x0aea, 0x261c }, { 0x0aeb, 0x261e }, { 0x0aec, 0x2663 }, { 0x0aed, 0x2666 },
		{ 0x0aee, 0x2665 }, { 0x0af0, 0x2720 }, { 0x0af1, 0x2020 }, { 0x0af2, 0x2021 },
		{ 0x0af3, 0x2713 }, { 0x0af4, 0x2717 }, { 0x0af5, 0x266f }, { 0x0af6, 0x266d },
		{ 0x0af7, 0x2642 }, { 0x0af8, 0x2640 }, { 0x0af9, 0x260e }, { 0x0afa, 0x2315 },
		{ 0x0afb, 0x2117 }, { 0x0afc, 0x2038 }, { 0x0afd, 0x201a }, { 0x0afe, 0x201e },
		{ 0x0ba3, 0x003c }, { 0x0ba6, 0x003e }, { 0x0ba8, 0x2228 }, { 0x0ba9, 0x2227 },
		{ 0x0bc0, 0x00af }, { 0x0bc2, 0x22a5 }, { 0x0bc3, 0x2229 }, { 0x0bc4, 0x230a },
		{ 0x0bc6, 0x005f }, { 0x0bca, 0x2218 }, { 0x0bcc, 0x2395 }, { 0x0bce, 0x22a4 },
		{ 0x0bcf, 0x25cb }, { 0x0bd3, 0x2308 }, { 0x0bd6, 0x222a }, { 0x0bd8, 0x2283 },
		{ 0x0bda, 0x2282 }, { 0x0bdc, 0x22a2 }, { 0x0bfc, 0x22a3 }, { 0x0cdf, 0x2017 },
		{ 0x0ce0, 0x05d0 }, { 0x0ce1, 0x05d1 }, { 0x0ce2, 0x05d2 }, { 0x0ce3, 0x05d3 },
		{ 0x0ce4, 0x05d4 }, { 0x0ce5, 0x05d5 }, { 0x0ce6, 0x05d6 }, { 0x0ce7, 0x05d7 },
		{ 0x0ce8, 0x05d8 }, { 0x0ce9, 0x05d9 }, { 0x0cea, 0x05da }, { 0x0ceb, 0x05db },
		{ 0x0cec, 0x05dc }, { 0x0ced, 0x05dd }, { 0x0cee, 0x05de }, { 0x0cef, 0x05df },
		{ 0x0cf0, 0x05e0 }, { 0x0cf1, 0x05e1 }, { 0x0cf2, 0x05e2 }, { 0x0cf3, 0x05e3 },
		{ 0x0cf4, 0x05e4 }, { 0x0cf5, 0x05e5 }, { 0x0cf6, 0x05e6 }, { 0x0cf7, 0x05e7 },
		{ 0x0cf8, 0x05e8 }, { 0x0cf9, 0x05e9 }, { 0x0cfa, 0x05ea }, { 0x0da1, 0x0e01 },
		{ 0x0da2, 0x0e02 }, { 0x0da3, 0x0e03 }, { 0x0da4, 0x0e04 }, { 0x0da5, 0x0e05 },
		{ 0x0da6, 0x0e06 }, { 0x0da7, 0x0e07 }, { 0x0da8, 0x0e08 }, { 0x0da9, 0x0e09 },
		{ 0x0daa, 0x0e0a }, { 0x0dab, 0x0e0b }, { 0x0dac, 0x0e0c }, { 0x0dad, 0x0e0d },
		{ 0x0dae, 0x0e0e }, { 0x0daf, 0x0e0f }, { 0x0db0, 0x0e10 }, { 0x0db1, 0x0e11 },
		{ 0x0db2, 0x0e12 }, { 0x0db3, 0x0e13 }, { 0x0db4, 0x0e14 }, { 0x0db5, 0x0e15 },
		{ 0x0db6, 0x0e16 }, { 0x0db7, 0x0e17 }, { 0x0db8, 0x0e18 }, { 0x0db9, 0x0e19 },
		{ 0x0dba, 0x0e1a }, { 0x0dbb, 0x0e1b }, { 0x0dbc, 0x0e1c }, { 0x0dbd, 0x0e1d },
		{ 0x0dbe, 0x0e1e }, { 0x0dbf, 0x0e1f }, { 0x0dc0, 0x0e20 }, { 0x0dc1, 0x0e21 },
		{ 0x0dc2, 0x0e22 }, { 0x0dc3, 0x0e23 }, { 0x0dc4, 0x0e24 }, { 0x0dc5, 0x0e25 },
		{ 0x0dc6, 0x0e26 }, { 0x0dc7, 0x0e27 }, { 0x0dc8, 0x0e28 }, { 0x0dc9, 0x0e29 },
		{ 0x0dca, 0x0e2a }, { 0x0dcb, 0x0e2b }, { 0x0dcc, 0x0e2c }, { 0x0dcd, 0x0e2d },
		{ 0x0dce, 0x0e2e }, { 0x0dcf, 0x0e2f }, { 0x0dd0, 0x0e30 }, { 0x0dd1, 0x0e31 },
		{ 0x0dd2, 0x0e32 }, { 0x0dd3, 0x0e33 }, { 0x0dd4, 0x0e34 }, { 0x0dd5, 0x0e35 },
		{ 0x0dd6, 0x0e36 }, { 0x0dd7, 0x0e37 }, { 0x0dd8, 0x0e38 }, { 0x0dd9, 0x0e39 },
		{ 0x0dda, 0x0e3a }, { 0x0ddf, 0x0e3f }, { 0x0de0, 0x0e40 }, { 0x0de1, 0x0e41 },
		{ 0x0de2, 0x0e42 }, { 0x0de3, 0x0e43 }, { 0x0de4, 0x0e44 }, { 0x0de5, 0x0e45 },
		{ 0x0de6, 0x0e46 }, { 0x0de7, 0x0e47 }, { 0x0de8, 0x0e48 }, { 0x0de9, 0x0e49 },
		{ 0x0dea, 0x0e4a }, { 0x0deb, 0x0e4b }, { 0x0dec, 0x0e4c }, { 0x0ded, 0x0e4d },
		{ 0x0df0, 0x0e50 }, { 0x0df1, 0x0e51 }, { 0x0df2, 0x0e52 }, { 0x0df3, 0x0e53 },
		{ 0x0df4, 0x0e54 }, { 0x0df5, 0x0e55 }, { 0x0df6, 0x0e56 }, { 0x0df7, 0x0e57 },
		{ 0x0df8, 0x0e58 }, { 0x0df9, 0x0e59 }, { 0x0ea1, 0x3131 }, { 0x0ea2, 0x3132 },
		{ 0x0ea3, 0x3133 }, { 0x0ea4, 0x3134 }, { 0x0ea5, 0x3135 }, { 0x0ea6, 0x3136 },
		{ 0x0ea7, 0x3137 }, { 0x0ea8, 0x3138 }, { 0x0ea9, 0x3139 }, { 0x0eaa, 0x313a },
		{ 0x0eab, 0x313b }, { 0x0eac, 0x313c }, { 0x0ead, 0x313d }, { 0x0eae, 0x313e },
		{ 0x0eaf, 0x313f }, { 0x0eb0, 0x3140 }, { 0x0eb1, 0x3141 }, { 0x0eb2, 0x3142 },
		{ 0x0eb3, 0x3143 }, { 0x0eb4, 0x3144 }, { 0x0eb5, 0x3145 }, { 0x0eb6, 0x3146 },
		{ 0x0eb7, 0x3147 }, { 0x0eb8, 0x3148 }, { 0x0eb9, 0x3149 }, { 0x0eba, 0x314a },
		{ 0x0ebb, 0x314b }, { 0x0ebc, 0x314c }, { 0x0ebd, 0x314d }, { 0x0ebe, 0x314e },
		{ 0x0ebf, 0x314f }, { 0x0ec0, 0x3150 }, { 0x0ec1, 0x3151 }, { 0x0ec2, 0x3152 },
		{ 0x0ec3, 0x3153 }, { 0x0ec4, 0x3154 }, { 0x0ec5, 0x3155 }, { 0x0ec6, 0x3156 },
		{ 0x0ec7, 0x3157 }, { 0x0ec8, 0x3158 }, { 0x0ec9, 0x3159 }, { 0x0eca, 0x315a },
		{ 0x0ecb, 0x315b }, { 0x0ecc, 0x315c }, { 0x0ecd, 0x315d }, { 0x0ece, 0x315e },
		{ 0x0ecf, 0x315f }, { 0x0ed0, 0x3160 }, { 0x0ed1, 0x3161 }, { 0x0ed2, 0x3162 },
		{ 0x0ed3, 0x3163 }, { 0x0ed4, 0x11a8 }, { 0x0ed5, 0x11a9 }, { 0x0ed6, 0x11aa },
		{ 0x0ed7, 0x11ab }, { 0x0ed8, 0x11ac }, { 0x0ed9, 0x11ad }, { 0x0eda, 0x11ae },
		{ 0x0edb, 0x11af }, { 0x0edc, 0x11b0 }, { 0x0edd, 0x11b1 }, { 0x0ede, 0x11b2 },
		{ 0x0edf, 0x11b3 }, { 0x0ee0, 0x11b4 }, { 0x0ee1, 0x11b5 }, { 0x0ee2, 0x11b6 },
		{ 0x0ee3, 0x11b7 }, { 0x0ee4, 0x11b8 }, { 0x0ee5, 0x11b9 }, { 0x0ee6, 0x11ba },
		{ 0x0ee7, 0x11bb }, { 0x0ee8, 0x11bc }, { 0x0ee9, 0x11bd }, { 0x0eea, 0x11be },
		{ 0x0eeb, 0x11bf }, { 0x0eec, 0x11c0 }, { 0x0eed, 0x11c1 }, { 0x0eee, 0x11c2 },
		{ 0x0eef, 0x316d }, { 0x0ef0, 0x3171 }, { 0x0ef1, 0x3178 }, { 0x0ef2, 0x317f },
		{ 0x0ef3, 0x3181 }, { 0x0ef4, 0x3184 }, { 0x0ef5, 0x3186 }, { 0x0ef6, 0x318d },
		{ 0x0ef7, 0x318e }, { 0x0ef8, 0x11eb }, { 0x0ef9, 0x11f0 }, { 0x0efa, 0x11f9 },
		{ 0x0eff, 0x20a9 }, { 0x13a4, 0x20ac }, { 0x13bc, 0x0152 }, { 0x13bd, 0x0153 },
		{ 0x13be, 0x0178 }, { 0x20ac, 0x20ac }, { 0xfe50,    '`' }, { 0xfe51, 0x00b4 },
		{ 0xfe52,    '^' }, { 0xfe53,    '~' }, { 0xfe54, 0x00af }, { 0xfe55, 0x02d8 },
		{ 0xfe56, 0x02d9 }, { 0xfe57, 0x00a8 }, { 0xfe58, 0x02da }, { 0xfe59, 0x02dd },
		{ 0xfe5a, 0x02c7 }, { 0xfe5b, 0x00b8 }, { 0xfe5c, 0x02db }, { 0xfe5d, 0x037a },
		{ 0xfe5e, 0x309b }, { 0xfe5f, 0x309c }, { 0xfe63,    '/' }, { 0xfe64, 0x02bc },
		{ 0xfe65, 0x02bd }, { 0xfe66, 0x02f5 }, { 0xfe67, 0x02f3 }, { 0xfe68, 0x02cd },
		{ 0xfe69, 0xa788 }, { 0xfe6a, 0x02f7 }, { 0xfe6e,    ',' }, { 0xfe6f, 0x00a4 },
		{ 0xfe80,    'a' }, // XK_dead_a
		{ 0xfe81,    'A' }, // XK_dead_A
		{ 0xfe82,    'e' }, // XK_dead_e
		{ 0xfe83,    'E' }, // XK_dead_E
		{ 0xfe84,    'i' }, // XK_dead_i
		{ 0xfe85,    'I' }, // XK_dead_I
		{ 0xfe86,    'o' }, // XK_dead_o
		{ 0xfe87,    'O' }, // XK_dead_O
		{ 0xfe88,    'u' }, // XK_dead_u
		{ 0xfe89,    'U' }, // XK_dead_U
		{ 0xfe8a, 0x0259 }, { 0xfe8b, 0x018f }, { 0xfe8c, 0x00b5 }, { 0xfe90,    '_' },
		{ 0xfe91, 0x02c8 }, { 0xfe92, 0x02cc },
		{ 0xff80 /*XKB_KEY_KP_Space*/,     ' ' },
		{ 0xff95 /*XKB_KEY_KP_7*/, 0x0037 },
		{ 0xff96 /*XKB_KEY_KP_4*/, 0x0034 },
		{ 0xff97 /*XKB_KEY_KP_8*/, 0x0038 },
		{ 0xff98 /*XKB_KEY_KP_6*/, 0x0036 },
		{ 0xff99 /*XKB_KEY_KP_2*/, 0x0032 },
		{ 0xff9a /*XKB_KEY_KP_9*/, 0x0039 },
		{ 0xff9b /*XKB_KEY_KP_3*/, 0x0033 },
		{ 0xff9c /*XKB_KEY_KP_1*/, 0x0031 },
		{ 0xff9d /*XKB_KEY_KP_5*/, 0x0035 },
		{ 0xff9e /*XKB_KEY_KP_0*/, 0x0030 },
		{ 0xffaa /*XKB_KEY_KP_Multiply*/,  '*' },
		{ 0xffab /*XKB_KEY_KP_Add*/,       '+' },
		{ 0xffac /*XKB_KEY_KP_Separator*/, ',' },
		{ 0xffad /*XKB_KEY_KP_Subtract*/,  '-' },
		{ 0xffae /*XKB_KEY_KP_Decimal*/,   '.' },
		{ 0xffaf /*XKB_KEY_KP_Divide*/,    '/' },
		{ 0xffb0 /*XKB_KEY_KP_0*/, 0x0030 },
		{ 0xffb1 /*XKB_KEY_KP_1*/, 0x0031 },
		{ 0xffb2 /*XKB_KEY_KP_2*/, 0x0032 },
		{ 0xffb3 /*XKB_KEY_KP_3*/, 0x0033 },
		{ 0xffb4 /*XKB_KEY_KP_4*/, 0x0034 },
		{ 0xffb5 /*XKB_KEY_KP_5*/, 0x0035 },
		{ 0xffb6 /*XKB_KEY_KP_6*/, 0x0036 },
		{ 0xffb7 /*XKB_KEY_KP_7*/, 0x0037 },
		{ 0xffb8 /*XKB_KEY_KP_8*/, 0x0038 },
		{ 0xffb9 /*XKB_KEY_KP_9*/, 0x0039 },
		{ 0xffbd /*XKB_KEY_KP_Equal*/,     '=' }
	}
};

//-------------------------------------------------------------------------------------------------------------------//

//Encode a Unicode code point to a UTF-8 stream
std::size_t TRAP::INTERNAL::WindowingAPI::EncodeUTF8(char* s, uint32_t ch)
{
	std::size_t count = 0;

	if(ch < 0x80)
		s[count++] = static_cast<uint8_t>(ch);
	else if(ch < 0x800)
	{
		s[count++] = (ch >> 6) | 0xC0;
		s[count++] = (ch & 0x3F) | 0x80;
	}
	else if(ch < 0x10000)
	{
		s[count++] = (ch >> 12) | 0xE0;
		s[count++] = ((ch >> 6) & 0x3F) | 0x80;
		s[count++] = (ch & 0x3F) | 0x80;
	}
	else if(ch < 0x110000)
	{
		s[count++] = (ch >> 18) | 0xF0;
		s[count++] = ((ch >> 12) & 0x3F) | 0x80;
		s[count++] = ((ch >> 6) & 0x3F) | 0x80;
		s[count++] = (ch & 0x3F) | 0x80;
	}

	return count;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::GetSelectionString(Atom selection)
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

	for(std::size_t i = 0; i < targets.size(); i++)
	{
		char* data;
		Atom actualType;
		int32_t actualFormat;
		unsigned long itemCount, bytesAfter;
		XEvent notification, dummy;

		s_Data.XLIB.ConvertSelection(s_Data.display, selection, targets[i], s_Data.TRAP_SELECTION,
		                             s_Data.HelperWindowHandle, CurrentTime);

		while(!s_Data.XLIB.CheckTypedWindowEvent(s_Data.display, s_Data.HelperWindowHandle, SelectionNotify,
		      &notification))
			WaitForX11Event(nullptr);

		if(notification.xselection.property == 0)
			continue;

		s_Data.XLIB.CheckIfEvent(s_Data.display, &dummy, IsSelPropNewValueNotify,
		                         reinterpret_cast<XPointer>(&notification));

		s_Data.XLIB.GetWindowProperty(s_Data.display, notification.xselection.requestor,
		                              notification.xselection.property, 0, LONG_MAX, 1, AnyPropertyType,
									  &actualType, &actualFormat, &itemCount, &bytesAfter,
									  reinterpret_cast<uint8_t**>(&data));

		if(actualType == s_Data.INCR)
		{
			std::size_t size = 1;
			std::string string{};

			while(true)
			{
				while(!s_Data.XLIB.CheckIfEvent(s_Data.display, &dummy, IsSelPropNewValueNotify,
				                                reinterpret_cast<XPointer>(&notification)))
					WaitForX11Event(nullptr);

				s_Data.XLIB.Free(data);
				s_Data.XLIB.GetWindowProperty(s_Data.display, notification.xselection.requestor,
				                              notification.xselection.property, 0, LONG_MAX, 1, AnyPropertyType,
											  &actualType, &actualFormat, &itemCount, &bytesAfter,
											  reinterpret_cast<uint8_t**>(&data));

				if(itemCount)
				{
					size += itemCount;
					string = data;
				}

				if(!itemCount)
				{
					if(targets[i] == XA_STRING)
					{
						*selectionString = ConvertLatin1ToUTF8(string.c_str());
						string.clear();
					}
					else
						*selectionString = string;

					break;
				}
			}
		}
		else if(actualType == targets[i])
		{
			if(targets[i] == XA_STRING)
				*selectionString = ConvertLatin1ToUTF8(data);
			else
				*selectionString = data;
		}

		s_Data.XLIB.Free(data);

		if(!(selectionString->empty()))
			break;
	}

	if(selectionString->empty())
		InputError(Error::Format_Unavailable, "[X11] Failed to convert selection to string");

	return *selectionString;
}

//-------------------------------------------------------------------------------------------------------------------//

//Convert XKB KeySym to Unicode
uint32_t TRAP::INTERNAL::WindowingAPI::KeySymToUnicode(uint32_t keySym)
{
	int32_t min = 0;
	int32_t max = KeySymTab.size() - 1;
	int32_t mid;

	//First check for Latin-1 characters (1:1 mapping)
	if((keySym >= 0x0020 && keySym <= 0x007E) ||
	   (keySym >= 0x00A0 && keySym <= 0x00FF))
	{
		return keySym;
	}

	//Also check for directly encoded 24-bit UCS characters
	if((keySym & 0xFF000000) == 0x01000000)
		return keySym & 0x00FFFFFF;

	//Binary search in table
	while(max >= min)
	{
		mid = (min + max) / 2;
		if(KeySymTab[mid].keySym < keySym)
			min = mid + 1;
		else if(KeySymTab[mid].keySym > keySym)
			max = mid - 1;
		else
			return KeySymTab[mid].UCS;
	}

	//No matching Unicode value found
	return 0xFFFFFFFFu;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether it is a property event for the specified selection transfer
int32_t TRAP::INTERNAL::WindowingAPI::IsSelPropNewValueNotify(Display*, XEvent* event, XPointer pointer)
{
	XEvent* notification = reinterpret_cast<XEvent*>(pointer);

	return event->type == PropertyNotify &&
	       event->xproperty.state == PropertyNewValue &&
		   event->xproperty.window == notification->xselection.requestor &&
		   event->xproperty.atom == notification->xselection.property;
}

//-------------------------------------------------------------------------------------------------------------------//

//Convert the specified Latin-1 string to UTF-8
std::string TRAP::INTERNAL::WindowingAPI::ConvertLatin1ToUTF8(const char* source)
{
	std::size_t size = 1;
	const char* sp;

	for(sp = source; *sp; sp++)
		size += (*sp & 0x80) ? 2 : 1;

	std::string target{};
	target.resize(size);
	char* tp = target.data();

	for(sp = source; *sp; sp++)
		tp += EncodeUTF8(tp, *sp);

	return target;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::PlatformGetVideoMode(const InternalMonitor* monitor)
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

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSize(const InternalWindow* window, int32_t& width,
                                                         int32_t& height)
{
	XWindowAttributes attribs;
	s_Data.XLIB.GetWindowAttributes(s_Data.display, window->Handle, &attribs);

	width = attribs.width;
	height = attribs.height;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPos(const InternalWindow* window, int32_t xPos, int32_t yPos)
{
	//HACK: Explicitly setting PPosition to any value causes some WMs, notably Compiz and Metacity, to honor
	//      the position of unmapped windows
	if(!PlatformWindowVisible(window))
	{
		long supplied;
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitor(InternalWindow* window, InternalMonitor* monitor,
														    int32_t xPos, int32_t yPos, int32_t width,
															int32_t height, int32_t)
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
		PlatformSetWindowDecorated(window, window->Decorated);
		PlatformSetWindowFloating(window, window->Floating);
		ReleaseMonitor(window);
	}
	if(window->BorderlessFullscreen)
		window->BorderlessFullscreen = false;

	window->Monitor = monitor;
	UpdateNormalHints(window, width, height);

	if(window->Monitor)
	{
		if(!PlatformWindowVisible(window))
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderless(InternalWindow* window,
                                                                      InternalMonitor* monitor)
{
	window->BorderlessFullscreen = true;
	window->Monitor = monitor;
	UpdateNormalHints(window, 0, 0);

	if(!window->Monitor)
		return;

	if(!PlatformWindowVisible(window))
	{
		s_Data.XLIB.MapRaised(s_Data.display, window->Handle);
		WaitForVisibilityNotify(window);
	}

	UpdateWindowMode(window);
	AcquireMonitorBorderless(window);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModes(const InternalMonitor* monitor)
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

			InternalVideoMode mode = VideoModeFromModeInfo(mi, ci);
			uint32_t j;

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
		result.push_back(PlatformGetVideoMode(monitor));
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformInit()
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
		s_Data.XLIB.Handle = PlatformLoadModule("libX11-6.so");
	#elif defined(__OpenBSD__) || defined(__NetBSD__)
		s_Data.XLIB.Handle = PlatformLoadModule("libX11.so");
	#else
        s_Data.XLIB.Handle = PlatformLoadModule("libX11.so.6");
	#endif

	if(!s_Data.XLIB.Handle)
	{
		InputError(Error::Platform_Error, "[X11] Failed to load Xlib");
		return false;
	}

	s_Data.XLIB.AllocClassHint = reinterpret_cast<PFN_XAllocClassHint>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XAllocClassHint"));
	s_Data.XLIB.AllocSizeHints = reinterpret_cast<PFN_XAllocSizeHints>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XAllocSizeHints"));
	s_Data.XLIB.AllocWMHints = reinterpret_cast<PFN_XAllocWMHints>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XAllocWMHints"));
	s_Data.XLIB.ChangeProperty = reinterpret_cast<PFN_XChangeProperty>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XChangeProperty"));
	s_Data.XLIB.ChangeWindowAttributes = reinterpret_cast<PFN_XChangeWindowAttributes>(PlatformGetModuleSymbol
		(
			s_Data.XLIB.Handle, "XChangeWindowAttributes"
		));
	s_Data.XLIB.CheckIfEvent = reinterpret_cast<PFN_XCheckIfEvent>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XCheckIfEvent"));
	s_Data.XLIB.CheckTypedWindowEvent = reinterpret_cast<PFN_XCheckTypedWindowEvent>(PlatformGetModuleSymbol
		(
			s_Data.XLIB.Handle, "XCheckTypedWindowEvent"
		));
	s_Data.XLIB.CloseDisplay = reinterpret_cast<PFN_XCloseDisplay>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XCloseDisplay"));
	s_Data.XLIB.CloseIM = reinterpret_cast<PFN_XCloseIM>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XCloseIM"));
	s_Data.XLIB.ConvertSelection = reinterpret_cast<PFN_XConvertSelection>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                             "XConvertSelection"));
	s_Data.XLIB.CreateColormap = reinterpret_cast<PFN_XCreateColormap>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                         "XCreateColormap"));
	s_Data.XLIB.CreateFontCursor = reinterpret_cast<PFN_XCreateFontCursor>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                             "XCreateFontCursor"));
	s_Data.XLIB.CreateIC = reinterpret_cast<PFN_XCreateIC>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,"XCreateIC"));
	s_Data.XLIB.CreateWindow = reinterpret_cast<PFN_XCreateWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XCreateWindow"));
	s_Data.XLIB.DefineCursor = reinterpret_cast<PFN_XDefineCursor>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XDefineCursor"));
	s_Data.XLIB.DeleteContext = reinterpret_cast<PFN_XDeleteContext>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XDeleteContext"));
	s_Data.XLIB.DeleteProperty = reinterpret_cast<PFN_XDeleteProperty>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XDeleteProperty"));
	s_Data.XLIB.DestroyIC = reinterpret_cast<PFN_XDestroyIC>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XDestroyIC"));
	s_Data.XLIB.DestroyWindow = reinterpret_cast<PFN_XDestroyWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XDestroyWindow"));
	s_Data.XLIB.DisplayKeycodes = reinterpret_cast<PFN_XDisplayKeycodes>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                           "XDisplayKeycodes"));
	s_Data.XLIB.EventsQueued = reinterpret_cast<PFN_XEventsQueued>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XEventsQueued"));
	s_Data.XLIB.FilterEvent = reinterpret_cast<PFN_XFilterEvent>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XFilterEvent"));
	s_Data.XLIB.FindContext = reinterpret_cast<PFN_XFindContext>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XFindContext"));
	s_Data.XLIB.Flush = reinterpret_cast<PFN_XFlush>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XFlush"));
	s_Data.XLIB.Free = reinterpret_cast<PFN_XFree>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XFree"));
	s_Data.XLIB.FreeColormap = reinterpret_cast<PFN_XFreeColormap>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XFreeColormap"));
	s_Data.XLIB.FreeCursor = reinterpret_cast<PFN_XFreeCursor>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XFreeCursor"));
	s_Data.XLIB.FreeEventData = reinterpret_cast<PFN_XFreeEventData>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XFreeEventData"));
	s_Data.XLIB.GetAtomName = reinterpret_cast<PFN_XGetAtomName>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGetAtomName"));
	s_Data.XLIB.GetErrorText = reinterpret_cast<PFN_XGetErrorText>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGetErrorText"));
	s_Data.XLIB.GetEventData = reinterpret_cast<PFN_XGetEventData>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGetEventData"));
	s_Data.XLIB.GetICValues = reinterpret_cast<PFN_XGetICValues>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGetICValues"));
	s_Data.XLIB.GetIMValues = reinterpret_cast<PFN_XGetIMValues>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGetIMValues"));
	s_Data.XLIB.GetInputFocus = reinterpret_cast<PFN_XGetInputFocus>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGetInputFocus"));
	s_Data.XLIB.GetKeyboardMapping = reinterpret_cast<PFN_XGetKeyboardMapping>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                                 "XGetKeyboardMapping"));
	s_Data.XLIB.GetScreenSaver = reinterpret_cast<PFN_XGetScreenSaver>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGetScreenSaver"));
	s_Data.XLIB.GetSelectionOwner = reinterpret_cast<PFN_XGetSelectionOwner>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                               "XGetSelectionOwner"));
	s_Data.XLIB.GetVisualInfo = reinterpret_cast<PFN_XGetVisualInfo>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGetVisualInfo"));
	s_Data.XLIB.GetWMNormalHints = reinterpret_cast<PFN_XGetWMNormalHints>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                             "XGetWMNormalHints"));
	s_Data.XLIB.GetWindowAttributes = reinterpret_cast<PFN_XGetWindowAttributes>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                                   "XGetWindowAttributes"));
	s_Data.XLIB.GetWindowProperty = reinterpret_cast<PFN_XGetWindowProperty>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                               "XGetWindowProperty"));
	s_Data.XLIB.GrabPointer = reinterpret_cast<PFN_XGrabPointer>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XGrabPointer"));
	s_Data.XLIB.IconifyWindow = reinterpret_cast<PFN_XIconifyWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XIconifyWindow"));
	s_Data.XLIB.InitThreads = reinterpret_cast<PFN_XInitThreads>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XInitThreads"));
	s_Data.XLIB.InternAtom = reinterpret_cast<PFN_XInternAtom>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XInternAtom"));
	s_Data.XLIB.LookupString = reinterpret_cast<PFN_XLookupString>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XLookupString"));
	s_Data.XLIB.MapRaised = reinterpret_cast<PFN_XMapRaised>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XMapRaised"));
	s_Data.XLIB.MapWindow = reinterpret_cast<PFN_XMapWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XMapWindow"));
	s_Data.XLIB.MoveResizeWindow = reinterpret_cast<PFN_XMoveResizeWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                             "XMoveResizeWindow"));
	s_Data.XLIB.MoveWindow = reinterpret_cast<PFN_XMoveWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XMoveWindow"));
	s_Data.XLIB.NextEvent = reinterpret_cast<PFN_XNextEvent>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XNextEvent"));
	s_Data.XLIB.OpenDisplay = reinterpret_cast<PFN_XOpenDisplay>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XOpenDisplay"));
	s_Data.XLIB.OpenIM = reinterpret_cast<PFN_XOpenIM>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XOpenIM"));
	s_Data.XLIB.PeekEvent = reinterpret_cast<PFN_XPeekEvent>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XPeekEvent"));
	s_Data.XLIB.Pending = reinterpret_cast<PFN_XPending>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XPending"));
	s_Data.XLIB.QueryExtension = reinterpret_cast<PFN_XQueryExtension>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XQueryExtension"));
	s_Data.XLIB.QueryPointer = reinterpret_cast<PFN_XQueryPointer>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XQueryPointer"));
	s_Data.XLIB.RaiseWindow = reinterpret_cast<PFN_XRaiseWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XRaiseWindow"));
	s_Data.XLIB.RegisterIMInstantiateCallback = reinterpret_cast<PFN_XRegisterIMInstantiateCallback>(PlatformGetModuleSymbol
		(
			s_Data.XLIB.Handle, "XRegisterIMInstantiateCallback"
		));
	s_Data.XLIB.ResizeWindow = reinterpret_cast<PFN_XResizeWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XResizeWindow"));
	s_Data.XLIB.ResourceManagerString = reinterpret_cast<PFN_XResourceManagerString>(PlatformGetModuleSymbol
		(
			s_Data.XLIB.Handle, "XResourceManagerString"
		));
	s_Data.XLIB.SaveContext = reinterpret_cast<PFN_XSaveContext>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSaveContext"));
	s_Data.XLIB.SelectInput = reinterpret_cast<PFN_XSelectInput>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSelectInput"));
	s_Data.XLIB.SendEvent = reinterpret_cast<PFN_XSendEvent>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSendEvent"));
	s_Data.XLIB.SetClassHint = reinterpret_cast<PFN_XSetClassHint>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSetClassHint"));
	s_Data.XLIB.SetErrorHandler = reinterpret_cast<PFN_XSetErrorHandler>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                           "XSetErrorHandler"));
	s_Data.XLIB.SetICFocus = reinterpret_cast<PFN_XSetICFocus>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSetICFocus"));
	s_Data.XLIB.SetIMValues = reinterpret_cast<PFN_XSetIMValues>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSetIMValues"));
	s_Data.XLIB.SetInputFocus = reinterpret_cast<PFN_XSetInputFocus>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSetInputFocus"));
	s_Data.XLIB.SetLocaleModifiers = reinterpret_cast<PFN_XSetLocaleModifiers>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                                 "XSetLocaleModifiers"));
	s_Data.XLIB.SetScreenSaver = reinterpret_cast<PFN_XSetScreenSaver>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSetScreenSaver"));
	s_Data.XLIB.SetSelectionOwner = reinterpret_cast<PFN_XSetSelectionOwner>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                               "XSetSelectionOwner"));
	s_Data.XLIB.SetWMHints = reinterpret_cast<PFN_XSetWMHints>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSetWMHints"));
	s_Data.XLIB.SetWMNormalHints = reinterpret_cast<PFN_XSetWMNormalHints>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                             "XSetWMNormalHints"));
	s_Data.XLIB.SetWMProtocols = reinterpret_cast<PFN_XSetWMProtocols>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSetWMProtocols"));
	s_Data.XLIB.SupportsLocale = reinterpret_cast<PFN_XSupportsLocale>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSupportsLocale"));
	s_Data.XLIB.Sync = reinterpret_cast<PFN_XSync>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XSync"));
	s_Data.XLIB.TranslateCoordinates = reinterpret_cast<PFN_XTranslateCoordinates>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                                     "XTranslateCoordinates"));
	s_Data.XLIB.UndefineCursor = reinterpret_cast<PFN_XUndefineCursor>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XUndefineCursor"));
	s_Data.XLIB.UngrabPointer = reinterpret_cast<PFN_XUngrabPointer>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XUngrabPointer"));
	s_Data.XLIB.UnmapWindow = reinterpret_cast<PFN_XUnmapWindow>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XUnmapWindow"));
	s_Data.XLIB.UnsetICFocus = reinterpret_cast<PFN_XUnsetICFocus>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XUnsetICFocus"));
	s_Data.XLIB.VisualIDFromVisual = reinterpret_cast<PFN_XVisualIDFromVisual>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                                 "XVisualIDFromVisual"));
	s_Data.XLIB.WarpPointer = reinterpret_cast<PFN_XWarpPointer>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XWarpPointer"));
	s_Data.XLIB.UnregisterIMInstantiateCallback = reinterpret_cast<PFN_XUnregisterIMInstantiateCallback>(PlatformGetModuleSymbol
		(
			s_Data.XLIB.Handle, "XUnregisterIMInstantiateCallback"
		));
	s_Data.XLIB.UTF8LookupString = reinterpret_cast<PFN_Xutf8LookupString>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                             "Xutf8LookupString"));
	s_Data.XLIB.UTF8SetWMProperties = reinterpret_cast<PFN_Xutf8SetWMProperties>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                                   "Xutf8SetWMProperties"));
	s_Data.XLIB.CreateRegion = reinterpret_cast<PFN_XCreateRegion>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XCreateRegion"));
	s_Data.XLIB.DestroyRegion = reinterpret_cast<PFN_XDestroyRegion>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XDestroyRegion"));
	s_Data.XKB.AllocKeyboard = reinterpret_cast<PFN_XkbAllocKeyboard>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XkbAllocKeyboard"));
	s_Data.XKB.FreeKeyboard = reinterpret_cast<PFN_XkbFreeKeyboard>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XkbFreeKeyboard"));
	s_Data.XKB.FreeNames = reinterpret_cast<PFN_XkbFreeNames>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XkbFreeNames"));
	s_Data.XKB.GetMap = reinterpret_cast<PFN_XkbGetMap>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XkbGetMap"));
	s_Data.XKB.GetNames = reinterpret_cast<PFN_XkbGetNames>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XkbGetNames"));
	s_Data.XKB.GetState = reinterpret_cast<PFN_XkbGetState>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XkbGetState"));
	s_Data.XKB.KeycodeToKeysym = reinterpret_cast<PFN_XkbKeycodeToKeysym>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                            "XkbKeycodeToKeysym"));
	s_Data.XKB.QueryExtension = reinterpret_cast<PFN_XkbQueryExtension>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                          "XkbQueryExtension"));
	s_Data.XKB.SelectEventDetails = reinterpret_cast<PFN_XkbSelectEventDetails>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                                  "XkbSelectEventDetails"));
	s_Data.XKB.SetDetectableAutoRepeat = reinterpret_cast<PFN_XkbSetDetectableAutoRepeat>(PlatformGetModuleSymbol
		(
			s_Data.XLIB.Handle, "XkbSetDetectableAutoRepeat"
		));
	s_Data.XRM.DestroyDatabase = reinterpret_cast<PFN_XrmDestroyDatabase>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                            "XrmDestroyDatabase"));
	s_Data.XRM.GetResource = reinterpret_cast<PFN_XrmGetResource>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XrmGetResource"));
	s_Data.XRM.GetStringDatabase = reinterpret_cast<PFN_XrmGetStringDatabase>(PlatformGetModuleSymbol(s_Data.XLIB.Handle,
	                                                                                "XrmGetStringDatabase"));
	s_Data.XRM.Initialize = reinterpret_cast<PFN_XrmInitialize>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XrmInitialize"));
	s_Data.XRM.UniqueQuark = reinterpret_cast<PFN_XrmUniqueQuark>(PlatformGetModuleSymbol(s_Data.XLIB.Handle, "XrmUniqueQuark"));
	if (s_Data.XLIB.UTF8LookupString && s_Data.XLIB.UTF8SetWMProperties)
		s_Data.XLIB.UTF8 = true;

	s_Data.XLIB.InitThreads();
	s_Data.XRM.Initialize();

	s_Data.display = s_Data.XLIB.OpenDisplay(nullptr);
	if(!s_Data.display)
	{
		const char* display = std::getenv("DISPLAY");
		if(display)
			InputError(Error::Platform_Error, std::string("[X11] Failed to open display: ") + display);
		else
			InputError(Error::Platform_Error, "[X11] The DISPLAY environment variable is missing!");

		return false;
	}

	s_Data.Screen = DefaultScreen(s_Data.display);
	s_Data.Root = RootWindow(s_Data.display, s_Data.Screen);
	s_Data.Context = XUniqueContext();

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

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyWindow(InternalWindow* window)
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

void TRAP::INTERNAL::WindowingAPI::PlatformShutdown()
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
		PlatformFreeModule(s_Data.XCB.Handle);
		s_Data.XCB.Handle = nullptr;
	}

	if(s_Data.XCursor.Handle)
	{
		PlatformFreeModule(s_Data.XCursor.Handle);
		s_Data.XCursor.Handle = nullptr;
	}

	if(s_Data.RandR.Handle)
	{
		PlatformFreeModule(s_Data.RandR.Handle);
		s_Data.RandR.Handle = nullptr;
	}

	if(s_Data.Xinerama.Handle)
	{
		PlatformFreeModule(s_Data.Xinerama.Handle);
		s_Data.Xinerama.Handle = nullptr;
	}

	if(s_Data.XRender.Handle)
	{
		PlatformFreeModule(s_Data.XRender.Handle);
		s_Data.XRender.Handle = nullptr;
	}

	if(s_Data.XI.Handle)
	{
		PlatformFreeModule(s_Data.XI.Handle);
		s_Data.XI.Handle = nullptr;
	}

	if(s_Data.XLIB.Handle)
	{
		PlatformFreeModule(s_Data.XLIB.Handle);
		s_Data.XLIB.Handle = nullptr;
	}

	if(s_Data.EmptyEventPipe[0] || s_Data.EmptyEventPipe[1])
	{
		close(s_Data.EmptyEventPipe[0]);
		close(s_Data.EmptyEventPipe[1]);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScale(const InternalMonitor*, float& xScale,
                                                                  float& yScale)
{
	xScale = s_Data.ContentScaleX;
	yScale = s_Data.ContentScaleY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPos(const InternalMonitor* monitor, int32_t& xPos,
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

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindow(InternalWindow* window)
{
	if(PlatformWindowVisible(window))
		return;

	s_Data.XLIB.MapWindow(s_Data.display, window->Handle);
	WaitForVisibilityNotify(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindow(const InternalWindow* window)
{
	if(s_Data.NET_ACTIVE_WINDOW)
		SendEventToWM(window, s_Data.NET_ACTIVE_WINDOW, 1, 0, 0, 0, 0);
	else if (PlatformWindowVisible(window))
	{
		s_Data.XLIB.RaiseWindow(s_Data.display, window->Handle);
		s_Data.XLIB.SetInputFocus(s_Data.display, window->Handle, RevertToParent, CurrentTime);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateWindow(InternalWindow* window,
			                                            WindowConfig& WNDConfig)
{
	Visual* visual = nullptr;
	int32_t depth;

	if(!visual)
	{
		visual = DefaultVisual(s_Data.display, s_Data.Screen);
		depth = DefaultDepth(s_Data.display, s_Data.Screen);
	}

	if(!CreateNativeWindow(window, WNDConfig, visual, depth))
		return false;

	if(window->Monitor)
	{
		PlatformShowWindow(window);
		UpdateWindowMode(window);
		if(window->BorderlessFullscreen)
			AcquireMonitorBorderless(window);
		else
			AcquireMonitor(window);
	}

	s_Data.XLIB.Flush(s_Data.display);
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowTitle(const InternalWindow* window, std::string& title)
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
					           (uint8_t*)title.data(),
					           title.size());

	s_Data.XLIB.ChangeProperty(s_Data.display,
	                           window->Handle,
					           s_Data.NET_WM_ICON_NAME,
					           s_Data.UTF8_STRING,
					           8,
					           PropModeReplace,
					           (uint8_t*)title.data(),
					           title.size());

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursor(InternalCursor* cursor, const Scope<Image>& image,
                                                        int32_t xHotspot, int32_t yHotspot)
{
	cursor->Handle = CreateCursorX11(image, xHotspot, yHotspot);

	return cursor->Handle;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursor(InternalCursor* cursor, const CursorType& shape)
{
	if(s_Data.XCursor.Handle)
	{
		char* theme = s_Data.XCursor.GetTheme(s_Data.display);
		if(theme)
		{
			const int32_t size = s_Data.XCursor.GetDefaultSize(s_Data.display);
			const char* name = nullptr;

			if(shape == CursorType::Arrow)
				name = "default";
			else if(shape == CursorType::Input)
				name = "text";
			else if(shape == CursorType::Crosshair)
				name = "crosshair";
			else if(shape == CursorType::PointingHand)
				name = "pointer";
			else if(shape == CursorType::ResizeHorizontal)
				name = "ew-resize";
			else if(shape == CursorType::ResizeVertical)
				name = "ns-resize";
			else if(shape == CursorType::ResizeDiagonalTopRightBottomLeft)
				name = "nwse-resize";
			else if(shape == CursorType::ResizeDiagonalTopLeftBottomRight)
				name = "nesw-resize";
			else if(shape == CursorType::ResizeAll)
				name = "all-scroll";
			else if(shape == CursorType::NotAllowed)
				name = "not-allowed";

			XcursorImage* image = s_Data.XCursor.LibraryLoadImage(name, theme, size);
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

		if(shape == CursorType::Arrow)
			native = XC_left_ptr;
		else if(shape == CursorType::Input)
			native = XC_xterm;
		else if(shape == CursorType::Crosshair)
			native = XC_crosshair;
		else if(shape == CursorType::PointingHand)
			native = XC_hand2;
		else if(shape == CursorType::ResizeHorizontal)
			native = XC_sb_h_double_arrow;
		else if(shape == CursorType::ResizeVertical)
			native = XC_sb_v_double_arrow;
		else if(shape == CursorType::ResizeDiagonalTopRightBottomLeft)
			native = XC_top_left_corner;
		else if(shape == CursorType::ResizeDiagonalTopLeftBottomRight)
			native = XC_top_right_corner;
		else if(shape == CursorType::ResizeAll)
			native = XC_fleur;
		else if(shape == CursorType::NotAllowed)
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

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursor(InternalCursor* cursor)
{
	if(cursor->Handle)
		s_Data.XLIB.FreeCursor(s_Data.display, cursor->Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursor(const InternalWindow* window, const InternalCursor*)
{
	if(window->cursorMode != CursorMode::Normal)
		return;

	UpdateCursorImage(window);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorMode(InternalWindow* window, CursorMode mode)
{
	if (PlatformWindowFocused(window))
	{
		if (mode == CursorMode::Disabled)
		{
			PlatformGetCursorPos(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
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
			PlatformSetCursorPos(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
		}
	}

	UpdateCursorImage(window);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPos(InternalWindow* window, double xPos, double yPos)
{
	//Store the new position so it can be recognized later
	window->WarpCursorPosX = static_cast<int32_t>(xPos);
	window->WarpCursorPosY = static_cast<int32_t>(yPos);

	s_Data.XLIB.WarpPointer(s_Data.display, 0, window->Handle, 0, 0, 0, 0, static_cast<int32_t>(xPos),
	                        static_cast<int32_t>(yPos));
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPos(const InternalWindow* window, double& xPos, double& yPos)
{
	::Window root, child;
	int32_t rootX, rootY, childX, childY;
	uint32_t mask;

	s_Data.XLIB.QueryPointer(s_Data.display, window->Handle, &root, &child, &rootX, &rootY, &childX, &childY,
	                         &mask);

	xPos = childX;
	yPos = childY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIcon(InternalWindow* window, const Scope<Image>& image)
{
	if(image)
	{
		uint32_t longCount = 2 + image->GetWidth() * image->GetHeight();

		std::vector<uint64_t> icon{};
		icon.resize(longCount);
		uint64_t* target = icon.data();
		std::vector<uint8_t> imgData(reinterpret_cast<const uint8_t*>(image->GetPixelData()),
		                             reinterpret_cast<const uint8_t*>(image->GetPixelData()) +
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
		                           PropModeReplace,	reinterpret_cast<uint8_t*>(icon.data()), longCount);
	}
	else
		s_Data.XLIB.DeleteProperty(s_Data.display, window->Handle, s_Data.NET_WM_ICON);

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos)
{
	::Window dummy;
	int32_t x, y;

	s_Data.XLIB.TranslateCoordinates(s_Data.display, window->Handle, s_Data.Root, 0, 0, &x, &y, &dummy);

	xPos = x;
	yPos = y;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSize(InternalWindow* window, int32_t width, int32_t height)
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizable(InternalWindow* window, bool)
{
	int32_t width, height;

	PlatformGetWindowSize(window, width, height);
	UpdateNormalHints(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecorated(const InternalWindow* window, bool enabled)
{
	struct Hints
	{
		unsigned long Flags = 0;
		unsigned long Functions = 0;
		unsigned long Decorations = 0;
		long InputMode = 0;
		unsigned long status = 0;
	} hints{};

	hints.Flags = 2; //MWM_HINTS_DECORATIONS
	hints.Decorations = enabled ? 1 : 0; //1 = MWM_DECOR_ALL

	s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.MOTIF_WM_HINTS, s_Data.MOTIF_WM_HINTS, 32,
	                           PropModeReplace, reinterpret_cast<uint8_t*>(&hints), sizeof(hints) / sizeof(long));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloating(const InternalWindow* window, bool enabled)
{
	if(!s_Data.NET_WM_STATE || !s_Data.NET_WM_STATE_ABOVE)
		return;

	if(PlatformWindowVisible(window))
	{
		const long action = enabled ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
		SendEventToWM(window, s_Data.NET_WM_STATE, action, s_Data.NET_WM_STATE_ABOVE, 0, 1, 0);
	}
	else
	{
		Atom* states = nullptr;

		const unsigned long count = GetWindowPropertyX11(window->Handle, s_Data.NET_WM_STATE, XA_ATOM,
		                                                 reinterpret_cast<uint8_t**>(&states));

		//NOTE: We don't check for failure as this property may not exist yet and that's fine (and we'll create
		//      it implicitly with append)
		if(enabled)
		{
			unsigned long i;

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
			for(unsigned long i = 0; i < count; ++i)
			{
				if(states[i] == s_Data.NET_WM_STATE_ABOVE)
				{
					states[i] = states[count - 1];
					s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_STATE, XA_ATOM, 32,
											PropModeReplace, reinterpret_cast<uint8_t*>(states), count - 1);
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacity(const InternalWindow* window, float opacity)
{
	const CARD32 value = (CARD32)(0xFFFFFFFFu * static_cast<double>(opacity));
	s_Data.XLIB.ChangeProperty(s_Data.display, window->Handle, s_Data.NET_WM_WINDOW_OPACITY, XA_CARDINAL, 32,
	                           PropModeReplace, reinterpret_cast<const uint8_t*>(&value), 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthrough(InternalWindow* window, bool enabled)
{
	if(!s_Data.XShape.Available)
		return;

	if (enabled)
	{
		Region region = s_Data.XLIB.CreateRegion();
		s_Data.XShape.CombineRegion(s_Data.display, window->Handle, 2, 0, 0, region, 0);
		s_Data.XLIB.DestroyRegion(region);
	}
	else
		s_Data.XShape.CombineMask(s_Data.display, window->Handle, 2, 0, 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbar(InternalWindow*)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacity(const InternalWindow* window)
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

void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSize(const InternalWindow* window, int32_t& width,
                                                              int32_t& height)
{
	PlatformGetWindowSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScale(const InternalWindow*, float& xScale, float& yScale)
{
	xScale = s_Data.ContentScaleX;
	yScale = s_Data.ContentScaleY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos,
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
			areaWidth = mi->height;
			areaHeight = mi->width;
		}
		else
		{
			areaWidth = mi->width;
			areaHeight = mi->height;
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
		const unsigned long extentCount = GetWindowPropertyX11(s_Data.Root, s_Data.NET_WORKAREA, XA_CARDINAL,
		                                                       reinterpret_cast<uint8_t**>(&extents));

		if(GetWindowPropertyX11(s_Data.Root, s_Data.NET_CURRENT_DESKTOP, XA_CARDINAL,
		                        reinterpret_cast<uint8_t**>(&desktop)) > 0)
		{
			if(extentCount >= 4 && *desktop < extentCount / 4)
			{
				const int32_t globalX = extents[*desktop * 4 + 0];
				const int32_t globalY = extents[*desktop * 4 + 1];
				const int32_t globalWidth = extents[*desktop * 4 + 2];
				const int32_t globalHeight = extents[*desktop * 4 + 3];

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

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowVisible(const InternalWindow* window)
{
	XWindowAttributes wa;
	s_Data.XLIB.GetWindowAttributes(s_Data.display, window->Handle, &wa);

	return wa.map_state == IsViewable;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximized(const InternalWindow* window)
{
	Atom* states;
	bool maximized = false;

	if(!s_Data.NET_WM_STATE || !s_Data.NET_WM_STATE_MAXIMIZED_VERT || !s_Data.NET_WM_STATE_MAXIMIZED_HORZ)
		return maximized;

	const unsigned long count = GetWindowPropertyX11(window->Handle, s_Data.NET_WM_STATE, XA_ATOM,
	                                                 reinterpret_cast<uint8_t**>(&states));

	for(unsigned long i = 0; i < count; i++)
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

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMinimized(const InternalWindow* window)
{
	return GetWindowState(window) == IconicState;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPollEvents()
{
	DrainEmptyEvents();

#ifdef TRAP_PLATFORM_LINUX
	Input::DetectControllerConnectionLinux();
#endif

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
		int32_t width, height;
		PlatformGetWindowSize(window, width, height);

		if(window->LastCursorPosX != width / 2 || window->LastCursorPosY != height / 2)
			PlatformSetCursorPos(window, width / 2, height / 2);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocused(const InternalWindow* window)
{
	::Window focused;
	int32_t state;

	s_Data.XLIB.GetInputFocus(s_Data.display, &focused, &state);

	return window->Handle == focused;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHovered(const InternalWindow* window)
{
	::Window w = s_Data.Root;
	while(w)
	{
		::Window root;
		int32_t rootX, rootY, childX, childY;
		uint32_t mask;

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

bool TRAP::INTERNAL::WindowingAPI::PlatformRawMouseMotionSupported()
{
	return s_Data.XI.Available;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotion(const InternalWindow* window, bool enabled)
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

int32_t TRAP::INTERNAL::WindowingAPI::PlatformGetKeyScanCode(Input::Key key)
{
	return s_Data.ScanCodes[static_cast<uint32_t>(key)];
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeName(int32_t scanCode)
{
	if(!s_Data.XKB.Available)
		return nullptr;

	if(scanCode < 0 || scanCode > 0xFF || s_Data.KeyCodes[scanCode] == Input::Key::Unknown)
	{
		InputError(Error::Invalid_Value, "Invalid scancode" + std::to_string(scanCode));
		return nullptr;
	}

	const int32_t key = static_cast<int32_t>(s_Data.KeyCodes[scanCode]);
	KeySym keySym = s_Data.XKB.KeycodeToKeysym(s_Data.display, scanCode, s_Data.XKB.Group, 0);
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetClipboardString(const std::string& string)
{
	s_Data.ClipboardString = std::string(string);

	s_Data.XLIB.SetSelectionOwner(s_Data.display, s_Data.CLIPBOARD, s_Data.HelperWindowHandle, CurrentTime);

	if(s_Data.XLIB.GetSelectionOwner(s_Data.display, s_Data.CLIPBOARD) != s_Data.HelperWindowHandle)
		InputError(Error::Platform_Error, "[X11] Failed to become owner of clipboard selection!");
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::PlatformGetClipboardString()
{
	return GetSelectionString(s_Data.CLIPBOARD);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetRequiredInstanceExtensions(std::array<std::string, 2>& extensions)
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

VkResult TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowSurface(VkInstance instance, const InternalWindow* window,
																   const VkAllocationCallbacks* allocator,
																   VkSurfaceKHR& surface)
{
	if(s_Data.VK.KHR_XCB_Surface && s_Data.XCB.Handle)
	{
		VkResult err;
		VkXcbSurfaceCreateInfoKHR sci;
		PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;

		xcb_connection_t* connection = s_Data.XCB.GetXCBConnection(s_Data.display);
		if(!connection)
		{
			InputError(Error::Platform_Error, "[X11] Failed to retrieve XCB connection!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		vkCreateXcbSurfaceKHR = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(vkGetInstanceProcAddr
			(
				instance, "vkCreateXcbSurfaceKHR"
			));
		if(!vkCreateXcbSurfaceKHR)
		{
			InputError(Error::API_Unavailable, "[X11] Vulkan instance missing VK_KHR_xcb_surface extension!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		std::memset(&sci, 0, sizeof(sci));
		sci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		sci.connection = connection;
		sci.window = window->Handle;

		err = vkCreateXcbSurfaceKHR(instance, &sci, allocator, &surface);
		if(err)
			InputError(Error::Platform_Error,
			           std::string("[X11] Failed to create Vulkan XCB surface: ") + GetVulkanResultString(err));

		return err;
	}
	else
	{
		VkResult err;
		VkXlibSurfaceCreateInfoKHR sci;
		PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;

		vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(vkGetInstanceProcAddr
			(
				instance, "vkCreateXlibSurfaceKHR"
			));
		if(!vkCreateXlibSurfaceKHR)
		{
			InputError(Error::API_Unavailable, "[X11] Vulkan instance missing VK_KHR_xlib_surface extension");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		std::memset(&sci, 0, sizeof(sci));
		sci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
		sci.dpy = s_Data.display;
		sci.window = window->Handle;

		err = vkCreateXlibSurfaceKHR(instance, &sci, allocator, &surface);
		if(err)
			InputError(Error::Platform_Error,
			           std::string("[X11] Failed to create Vulkan X11 surface: ") + GetVulkanResultString(err));

		return err;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindow(const InternalWindow* window)
{
	if(!s_Data.NET_WM_STATE || !s_Data.NET_WM_STATE_MAXIMIZED_VERT || !s_Data.NET_WM_STATE_MAXIMIZED_HORZ)
	   return;

	if(PlatformWindowVisible(window))
	{
		SendEventToWM(window, s_Data.NET_WM_STATE, 1, s_Data.NET_WM_STATE_MAXIMIZED_VERT,
		              s_Data.NET_WM_STATE_MAXIMIZED_HORZ, 1, 0);
	}
	else
	{
		Atom* states = nullptr;
		unsigned long count = GetWindowPropertyX11(window->Handle, s_Data.NET_WM_STATE, XA_ATOM,
		                                           reinterpret_cast<uint8_t**>(&states));

		//NOTE: We don't check for failure as this property may not exist yet and that's fine (and we'll create it
		//      implicitly with append)
		std::array<Atom, 2> missing =
		{
			s_Data.NET_WM_STATE_MAXIMIZED_VERT,
			s_Data.NET_WM_STATE_MAXIMIZED_HORZ
		};
		unsigned long missingCount = 2;

		for(unsigned long i = 0; i < count; i++)
		{
			for(unsigned long j = 0; j < missingCount; j++)
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
		                           reinterpret_cast<uint8_t*>(missing.data()), missingCount);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindow(const InternalWindow* window)
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

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttention(const InternalWindow* window)
{
	if(!s_Data.NET_WM_STATE || !s_Data.NET_WM_STATE_DEMANDS_ATTENTION)
		return;

	SendEventToWM(window, s_Data.NET_WM_STATE, 1, s_Data.NET_WM_STATE_DEMANDS_ATTENTION, 0, 1, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindow(const InternalWindow* window)
{
	s_Data.XLIB.UnmapWindow(s_Data.display, window->Handle);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindow(InternalWindow* window)
{
	if(window->OverrideRedirect)
	{
		//Override-redirect windows cannot be minimized or restored, as those tasks are performed by
		//the window manager
		InputError(Error::Platform_Error,
		           "[X11] Maximization of full screen windows requires a WM that supports EWMH full screen");
		return;
	}

	if(PlatformWindowMinimized(window))
	{
		s_Data.XLIB.MapWindow(s_Data.display, window->Handle);
		WaitForVisibilityNotify(window);
	}
	else if(PlatformWindowVisible(window))
	{
		if(s_Data.NET_WM_STATE && s_Data.NET_WM_STATE_MAXIMIZED_VERT && s_Data.NET_WM_STATE_MAXIMIZED_HORZ)
			SendEventToWM(window, s_Data.NET_WM_STATE, 0, s_Data.NET_WM_STATE_MAXIMIZED_VERT,
			              s_Data.NET_WM_STATE_MAXIMIZED_HORZ, 1, 0);
	}

	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimits(InternalWindow* window, int32_t, int32_t, int32_t,
                                                               int32_t)
{
	int32_t width, height;
	PlatformGetWindowSize(window, width, height);
	UpdateNormalHints(window, width, height);
	s_Data.XLIB.Flush(s_Data.display);
}

//-------------------------------------------------------------------------------------------------------------------//

//Enable XI2 raw mouse motion events
void TRAP::INTERNAL::WindowingAPI::EnableRawMouseMotion(const InternalWindow*)
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
void TRAP::INTERNAL::WindowingAPI::DisableRawMouseMotion(const InternalWindow*)
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
void TRAP::INTERNAL::WindowingAPI::InputErrorX11(Error error, const char* message)
{
	std::vector<char> buffer{};
	buffer.resize(1024);
	s_Data.XLIB.GetErrorText(s_Data.display, s_Data.ErrorCode, buffer.data(), buffer.size());

	buffer.shrink_to_fit();

	InputError(error, "[X11] " + std::string(message) + ": " + buffer.data());
}

//-------------------------------------------------------------------------------------------------------------------//

//Process the specified X event
void TRAP::INTERNAL::WindowingAPI::ProcessEvent(XEvent& event)
{
	int32_t keyCode = 0;
	bool filtered = false;

	//HACK: Save scanCode as some IMs clear the field in s_Data.XLIB.FilterEvent
	if(event.type == 2 || event.type == 3)
		keyCode = event.xkey.keycode;

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
			if(((XkbEvent*)&event)->any.xkb_type == XkbStateNotify &&
			   (((XkbEvent*)&event)->state.changed & XkbGroupStateMask))
			{
				s_Data.XKB.Group = ((XkbEvent*)&event)->state.group;
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
				XIRawEvent* re = reinterpret_cast<XIRawEvent*>(event.xcookie.data);
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

	if(event.type == SelectionClear)
	{
		HandleSelectionClear(event);
		return;
	}
	else if(event.type == SelectionRequest)
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

	case 2:
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
			Time diff = event.xkey.time - window->KeyPressTimes[keyCode];
			if(diff == event.xkey.time || (diff > 0 && diff < (static_cast<Time>(1u) << 31u)))
			{
				if(keyCode)
					InputKey(window, (Input::Key)key, keyCode, true);

				window->KeyPressTimes[keyCode] = event.xkey.time;
			}

			if(!filtered)
			{
				int32_t count;
				int32_t status;

				std::string buffer{};
				buffer.resize(100);
				char* chars = buffer.data();

				count = s_Data.XLIB.UTF8LookupString(window->IC, &event.xkey, buffer.data(), buffer.size() - 1, nullptr, &status);

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
			KeySym keySym;
			s_Data.XLIB.LookupString(&event.xkey, nullptr, 0, &keySym, nullptr);

			InputKey(window, static_cast<Input::Key>(key), keyCode, true);

			const uint32_t character = KeySymToUnicode(keySym);
			if(character != 0xFFFFFFFFu)
				InputChar(window, character);
		}

		return;
	}

	case 3:
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

				if(next.type == 2 &&
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

		InputKey(window, key, keyCode, false);
		return;
	}

	case ButtonPress:
	{
		if(event.xbutton.button == Button1)
			InputMouseClick(window, Input::MouseButton::Left, true);
		else if(event.xbutton.button == Button2)
			InputMouseClick(window, Input::MouseButton::Middle, true);
		else if(event.xbutton.button == Button3)
			InputMouseClick(window, Input::MouseButton::Right, true);

		//Modern X provides scroll events as mouse button presses
		else if(event.xbutton.button == Button4)
			InputScroll(window, 0.0, 1.0);
		else if(event.xbutton.button == Button5)
			InputScroll(window, 0.0, -1.0);
		else if(event.xbutton.button == 6)
			InputScroll(window, 1.0, 0.0);
		else if(event.xbutton.button == 7)
			InputScroll(window, -1.0, 0.0);

		else
		{
			//Additional buttons after 7 are treated as regular buttons
			//We subtract 4 to fill the gap left by scroll input above
			InputMouseClick(window, static_cast<Input::MouseButton>(event.xbutton.button - Button1 - 4), true);
		}

		return;
	}

	case ButtonRelease:
	{
		if(event.xbutton.button == Button1)
			InputMouseClick(window, Input::MouseButton::Left, false);
		else if(event.xbutton.button == Button2)
			InputMouseClick(window, Input::MouseButton::Middle, false);
		else if(event.xbutton.button == Button3)
			InputMouseClick(window, Input::MouseButton::Right, false);
		else if(event.xbutton.button > 7)
		{
			//Additional buttons after 7 are treated as regular buttons
			//We subtract 4 to fill the gap left by scroll input above
			InputMouseClick(window, static_cast<Input::MouseButton>(event.xbutton.button - Button1 - 4), false);
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

			::Window dummy;
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

		if(event.xclient.message_type == 0)
			return;

		if(event.xclient.message_type == s_Data.WM_PROTOCOLS)
		{
			const Atom protocol = event.xclient.data.l[0];
			if(protocol == 0)
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
			unsigned long count;
			Atom* formats = nullptr;
			const bool list = event.xclient.data.l[1] & 1;

			s_Data.XDND.Source = event.xclient.data.l[0];
			s_Data.XDND.Version = event.xclient.data.l[1] >> 24;
			s_Data.XDND.Format = 0;

			if(s_Data.XDND.Version > 5)
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

			if(s_Data.XDND.Version > 5)
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
				reply.xclient.data.l[0] = window->Handle;
				reply.xclient.data.l[1] = 0; //The drag was rejected
				reply.xclient.data.l[2] = 0;

				s_Data.XLIB.SendEvent(s_Data.display, s_Data.XDND.Source, 0, NoEventMask, &reply);
				s_Data.XLIB.Flush(s_Data.display);
			}
		}
		else if(event.xclient.message_type == s_Data.XDNDPosition)
		{
			//The drag operation has moved over the window
			const int32_t xAbs = (event.xclient.data.l[2] >> 16) & 0xFFFF;
			const int32_t yAbs = (event.xclient.data.l[2]) & 0xFFFF;
			::Window dummy;
			int32_t xPos, yPos;

			if(s_Data.XDND.Version > 5)
				return;

			s_Data.XLIB.TranslateCoordinates(s_Data.display, s_Data.Root, window->Handle, xAbs, yAbs, &xPos, &yPos,
			                                 &dummy);

			InputCursorPos(window, xPos, yPos);

			XEvent reply = {ClientMessage};
			reply.xclient.window = s_Data.XDND.Source;
			reply.xclient.message_type = s_Data.XDNDStatus;
			reply.xclient.format = 32;
			reply.xclient.data.l[0] = window->Handle;
			reply.xclient.data.l[2] = 0; //Specify an empty rectangle
			reply.xclient.data.l[3] = 0;

			if(s_Data.XDND.Format)
			{
				//Reply that we are ready to copy the dragged data
				reply.xclient.data.l[1] = 1; //Accept with no rectangle
				if(s_Data.XDND.Version >= 2)
					reply.xclient.data.l[4] = s_Data.XDNDActionCopy;
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
			char* data;
			const unsigned long result = GetWindowPropertyX11(event.xselection.requestor,
															  event.xselection.property,
			                                                  event.xselection.target,
														      reinterpret_cast<uint8_t**>(&data));

			if(result)
			{
				int32_t count;
				std::vector<std::string> paths = ParseUriList(data, count);

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
				reply.xclient.data.l[0] = window->Handle;
				reply.xclient.data.l[1] = result;
				reply.xclient.data.l[2] = s_Data.XDNDActionCopy;

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
			PlatformMinimizeWindow(window);

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
			const bool maximized = PlatformWindowMaximized(window);
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
TRAP::Input::Key TRAP::INTERNAL::WindowingAPI::TranslateKey(int32_t scanCode)
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
	std::string prefix = "file://";
	std::vector<std::string> paths{};
	char* line = nullptr;

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
				*pathPtr = strtol(digits.data(), nullptr, 16);
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
		int32_t xPos, yPos;
		InternalVideoMode mode;

		//Manually position the window over its monitor
		PlatformGetMonitorPos(window->Monitor, xPos, yPos);
		mode = PlatformGetVideoMode(window->Monitor);

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
	current = PlatformGetVideoMode(monitor);

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
		int32_t xPos, yPos;
		InternalVideoMode mode;

		//Manually position the window over its monitor
		PlatformGetMonitorPos(window->Monitor, xPos, yPos);
		mode = PlatformGetVideoMode(window->Monitor);

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
	PlatformGetCursorPos(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
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
	PlatformSetCursorPos(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
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
	int32_t scanCode, scanCodeMin, scanCodeMax;

	std::fill(s_Data.KeyCodes.begin(), s_Data.KeyCodes.end(), Input::Key::Unknown);
	std::fill(s_Data.ScanCodes.begin(), s_Data.ScanCodes.end(), -1);

	if(s_Data.XKB.Available)
	{
		//Use XKB to determine physical key locations independently of the current keyboard layout

		XkbDescPtr desc = s_Data.XKB.GetMap(s_Data.display, 0, XkbUseCoreKbd);
		s_Data.XKB.GetNames(s_Data.display, XkbKeyNamesMask | XkbKeyAliasesMask, desc);

		scanCodeMin = desc->min_key_code;
		scanCodeMax = desc->max_key_code;

		struct Keys
		{
			TRAP::Input::Key Key = TRAP::Input::Key::Unknown;
			std::string Name = "";
		};

		std::array<Keys, 121> KeyMap =
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
			for(Keys keyMapKey : KeyMap)
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

				for(Keys keyMapKey : KeyMap)
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

	int32_t width;
	KeySym* keySyms = s_Data.XLIB.GetKeyboardMapping(s_Data.display, scanCodeMin, scanCodeMax - scanCodeMin + 1,
	                                                 &width);

	for(scanCode = scanCodeMin; scanCode <= scanCodeMax; scanCode++)
	{
		//Translate the un-translated key codes using traditional X11 KeySym lookups
		if(static_cast<int32_t>(s_Data.KeyCodes[scanCode]) < 0)
		{
			const std::size_t base = (scanCode - scanCodeMin) * width;
			s_Data.KeyCodes[scanCode] = TranslateKeySyms(&keySyms[base], width);
		}

		//Store the reverse translation for faster key name lookup
		if(static_cast<int32_t>(s_Data.KeyCodes[scanCode]) > 0)
			s_Data.ScanCodes[static_cast<int32_t>(s_Data.KeyCodes[scanCode])] = scanCode;
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
TRAP::Input::Key TRAP::INTERNAL::WindowingAPI::TranslateKeySyms(const KeySym* keySyms, int32_t width)
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

std::string TRAP::INTERNAL::WindowingAPI::GetX11KeyboardLayoutName()
{
	if (!s_Data.XKB.Available)
	{
		InputError(Error::API_Unavailable, "[Input][X11] XKB extension required for keyboard layout names");
		return "";
	}

	XkbStateRec state{};
	s_Data.XKB.GetState(s_Data.display, XkbUseCoreKbd, &state);

	XkbDescPtr desc = s_Data.XKB.AllocKeyboard();
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDrop(InternalWindow* window, bool value)
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