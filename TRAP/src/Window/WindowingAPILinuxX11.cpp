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

#include <cstddef>

#include "TRAPPCH.h"

#include "Core/PlatformDetection.h"
#include "Utils/String/String.h"

#ifdef TRAP_PLATFORM_LINUX

#include "WindowingAPI.h"
#include "Application.h"
#include "Utils/Utils.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Utils/Time/TimeStep.h"

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

//Sends an EWMH or ICCCM event to the window manager
void TRAP::INTERNAL::WindowingAPI::SendEventToWM(const InternalWindow& window, const Atom type, const int64_t a,
												 const int64_t b, const int64_t c, const int64_t d, const int64_t e)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	XEvent event = { ClientMessage };
	event.xclient.window = window.X11.Handle;
	event.xclient.format = 32; //Data is 32-bit longs
	event.xclient.message_type = type;
	event.xclient.data.l[0] = a;
	event.xclient.data.l[1] = b;
	event.xclient.data.l[2] = c;
	event.xclient.data.l[3] = d;
	event.xclient.data.l[4] = e;

	s_Data.X11.XLIB.SendEvent(s_Data.X11.display, s_Data.X11.Root, 0,
	                          SubstructureNotifyMask | SubstructureRedirectMask, &event);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::WaitForX11Event(double* const timeout)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	pollfd fd = {ConnectionNumber(s_Data.X11.display), POLLIN, 0};

	while(s_Data.X11.XLIB.Pending(s_Data.X11.display) == 0)
	{
		if(!PollPOSIX(&fd, 1, timeout))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::WaitForAnyEvent(double* const timeout)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::array<pollfd, 3> fds
	{
		{
			{ConnectionNumber(s_Data.X11.display), POLLIN, 0},
			{std::get<0>(s_Data.EmptyEventPipe), POLLIN, 0},
			{TRAP::Input::s_linuxController.INotify, POLLIN, 0}
		}
	};

	while(s_Data.X11.XLIB.Pending(s_Data.X11.display) == 0)
	{
		if(!PollPOSIX(fds.data(), fds.size(), timeout))
			return false;

		for(std::size_t i = 1; i < fds.size(); ++i)
		{
			if((fds[i].revents & POLLIN) != 0)
				return true;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::WriteEmptyEvent()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	while(true)
	{
		const char byte = 0;
		const ssize_t result = write(std::get<1>(s_Data.EmptyEventPipe), &byte, 1);
		if(result == 1 || (result == -1 && errno != EINTR))
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DrainEmptyEvents()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	while(true)
	{
		std::array<char, 64> dummy{};
		const ssize_t result = read(std::get<0>(s_Data.EmptyEventPipe), dummy.data(), dummy.size());
		if(result == -1 && errno != EINTR)
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::SetEventFlags(int32_t& fileDescriptor)
{
	const int sf = fcntl(fileDescriptor, F_GETFL, 0);
	const int df = fcntl(fileDescriptor, F_GETFD, 0);

	if(sf == -1 || df == -1 ||
	   fcntl(fileDescriptor, F_SETFL, sf | O_NONBLOCK) == -1 ||
	   fcntl(fileDescriptor, F_SETFD, df | FD_CLOEXEC) == -1)
	{
		InputError(Error::Platform_Error, "X11: Failed to set flags for empty event pipe: " + Utils::String::GetStrError());
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::CreateEmptyEventPipe()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(pipe(s_Data.EmptyEventPipe.data()) != 0)
	{
		InputError(Error::Platform_Error, "X11: Failed to create empty event pipe: " + Utils::String::GetStrError());
		return false;
	}

	if(!std::all_of(s_Data.EmptyEventPipe.begin(), s_Data.EmptyEventPipe.end(), SetEventFlags))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieve a single window property of the specified type
[[nodiscard]] uint64_t TRAP::INTERNAL::WindowingAPI::GetWindowPropertyX11(::Window window, const Atom property, const Atom type,
                                                                          uint8_t** const value)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(value, "WindowingAPI::GetWindowPropertyX11(): value is nullptr!");

	Atom actualType{};
	int32_t actualFormat{};
	uint64_t itemCount{}, bytesAfter{};

	s_Data.X11.XLIB.GetWindowProperty(s_Data.X11.display, window, property, 0, std::numeric_limits<long>::max(),
	                                  0, type, &actualType, &actualFormat, &itemCount, &bytesAfter, value);

	return itemCount;
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the normal hints according to the window settings
void TRAP::INTERNAL::WindowingAPI::UpdateNormalHints(const InternalWindow& window, const int32_t width,
                                                     const int32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	XSizeHints* const hints = s_Data.X11.XLIB.AllocSizeHints();
	if(hints == nullptr)
	{
		InputErrorX11(Error::Platform_Error, "[X11] Failed to update normal hints (allocation of hints failed)!");
		return;
	}

	long supplied = 0;
	s_Data.X11.XLIB.GetWMNormalHints(s_Data.X11.display, window.X11.Handle, hints, &supplied);
	hints->flags &= ~(PMinSize | PMaxSize | PAspect);

	if(window.Monitor == nullptr)
	{
		if(window.Resizable)
		{
			if(window.MinWidth != -1 && window.MinHeight != -1)
			{
				hints->flags |= PMinSize;
				hints->min_width = window.MinWidth;
				hints->min_height = window.MinHeight;
			}

			if(window.MaxWidth != -1 && window.MaxHeight != -1)
			{
				hints->flags |= PMaxSize;
				hints->max_width = window.MaxWidth;
				hints->max_height = window.MaxHeight;
			}

			if(window.Numerator != -1 && window.Denominator != -1)
			{
				hints->flags |= PAspect;
				hints->min_aspect.x = window.Numerator;
				hints->max_aspect.x = window.Numerator;
				hints->min_aspect.y = window.Denominator;
				hints->max_aspect.y = window.Denominator;
			}
		}
		else
		{
			hints->flags |= (PMinSize | PMaxSize);
			hints->min_width = hints->max_width = width;
			hints->min_height = hints->max_height = height;
		}
	}

	s_Data.X11.XLIB.SetWMNormalHints(s_Data.X11.display, window.X11.Handle, hints);
	s_Data.X11.XLIB.Free(hints);
}

//-------------------------------------------------------------------------------------------------------------------//

//Waits until a VisibilityNotify event arrives for the specified window or the timeout period elapses
bool TRAP::INTERNAL::WindowingAPI::WaitForVisibilityNotify(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	XEvent dummy;
	double timeout = 0.1;

	while(s_Data.X11.XLIB.CheckTypedWindowEvent(s_Data.X11.display, window.X11.Handle, VisibilityNotify, &dummy) == 0)
	{
		if(!WaitForX11Event(&timeout))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the full screen status of the window
void TRAP::INTERNAL::WindowingAPI::UpdateWindowMode(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.Monitor != nullptr)
	{
		if(s_Data.X11.Xinerama.Available && (s_Data.X11.NET_WM_FULLSCREEN_MONITORS != 0u))
		{
			SendEventToWM(window,
			              s_Data.X11.NET_WM_FULLSCREEN_MONITORS,
						  window.Monitor->X11.Index,
						  window.Monitor->X11.Index,
						  window.Monitor->X11.Index,
						  window.Monitor->X11.Index,
						  0);
		}

		if((s_Data.X11.NET_WM_STATE != 0u) && (s_Data.X11.NET_WM_STATE_FULLSCREEN != 0u))
		{
			SendEventToWM(window,
						  s_Data.X11.NET_WM_STATE,
						  _NET_WM_STATE_ADD,
						  static_cast<int64_t>(s_Data.X11.NET_WM_STATE_FULLSCREEN),
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

			XSetWindowAttributes attributes{};
			attributes.override_redirect = 1;
			s_Data.X11.XLIB.ChangeWindowAttributes(s_Data.X11.display, window.X11.Handle, CWOverrideRedirect,
			                                       &attributes);

			window.X11.OverrideRedirect = true;
		}

		//Enable compositor bypass
		if(!window.Transparent)
		{
			const uint64_t value = 1;

			s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle,
			                               s_Data.X11.NET_WM_BYPASS_COMPOSITOR, XA_CARDINAL, 32,
										   PropModeReplace, reinterpret_cast<const uint8_t*>(&value), 1);
		}
	}
	else
	{
		if(s_Data.X11.Xinerama.Available && (s_Data.X11.NET_WM_FULLSCREEN_MONITORS != 0u))
		{
			s_Data.X11.XLIB.DeleteProperty(s_Data.X11.display, window.X11.Handle,
			                               s_Data.X11.NET_WM_FULLSCREEN_MONITORS);
		}

		if((s_Data.X11.NET_WM_STATE != 0u) && (s_Data.X11.NET_WM_STATE_FULLSCREEN != 0u))
		{
			SendEventToWM(window,
						  s_Data.X11.NET_WM_STATE,
						  _NET_WM_STATE_REMOVE,
						  static_cast<int64_t>(s_Data.X11.NET_WM_STATE_FULLSCREEN),
						  0, 1, 0);
		}
		else
		{
			XSetWindowAttributes attributes{};
			attributes.override_redirect = 0;
			s_Data.X11.XLIB.ChangeWindowAttributes(s_Data.X11.display, window.X11.Handle,
			                                       CWOverrideRedirect, &attributes);

			window.X11.OverrideRedirect = false;
		}

		//Disable compositor bypass
		if(!window.Transparent)
			s_Data.X11.XLIB.DeleteProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_BYPASS_COMPOSITOR);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieve system content scale via folklore heuristics
void TRAP::INTERNAL::WindowingAPI::GetSystemContentScale(float& xScale, float& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	//Start by assuming the default X11 DPI
	//NOTE: Some desktop environments (KDE) may remove the Xft.dpi field when it would be set to 96, so assume
	//      that is the case if we cannot find it
	float xDPI = 96.0f, yDPI = 96.0f;

	//NOTE: Basing the scale on Xft.dpi where available should provide the most consistent user experience
	//      (matches Qt, Gtk, etc), although not always the most accurate one
	const char* const rms = s_Data.X11.XLIB.ResourceManagerString(s_Data.X11.display);
	if(rms != nullptr)
	{
		const XrmDatabase db = s_Data.X11.XRM.GetStringDatabase(rms);
		if(db != nullptr)
		{
			XrmValue value;
			char* type = nullptr;

			if(s_Data.X11.XRM.GetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value) != 0)
			{
				if((type != nullptr) && strcmp(type, "String") == 0)
				{
					char* strEnd = nullptr;
					xDPI = yDPI = static_cast<float>(std::strtod(value.addr, &strEnd));
				}
			}

			s_Data.X11.XRM.DestroyDatabase(db);
		}
	}

	xScale = xDPI / 96.0f;
	yScale = yDPI / 96.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

//Look for and initialize supported X11 extensions
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::InitExtensions()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

#if defined(__CYGWIN__)
	s_Data.X11.XI.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXi-6.so");
#elif defined (__OpenBSD__) || defined(__NetBSD__)
	s_Data.X11.XI.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXi.so");
#else
	s_Data.X11.XI.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXi.so.6");
#endif
	if(s_Data.X11.XI.Handle != nullptr)
	{
		s_Data.X11.XI.QueryVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XIQueryVersion>(s_Data.X11.XI.Handle, "XIQueryVersion");
		s_Data.X11.XI.SelectEvents = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XISelectEvents>(s_Data.X11.XI.Handle, "XISelectEvents");

		if(s_Data.X11.XLIB.QueryExtension(s_Data.X11.display,
		                                  "XInputExtension",
						                  &s_Data.X11.XI.MajorOPCode,
						                  &s_Data.X11.XI.EventBase,
						                  &s_Data.X11.XI.ErrorBase) != 0)
		{
			s_Data.X11.XI.Major = 2;
			s_Data.X11.XI.Minor = 0;

			if(s_Data.X11.XI.QueryVersion(s_Data.X11.display, &s_Data.X11.XI.Major, &s_Data.X11.XI.Minor) == 0) //0 = Success
				s_Data.X11.XI.Available = true;
		}
	}

#if defined(__CYGWIN__)
	s_Data.X11.RandR.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrandr-2.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.X11.RandR.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrandr.so");
#else
	s_Data.X11.RandR.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrandr.so.2");
#endif
	if(s_Data.X11.RandR.Handle != nullptr)
	{
		s_Data.X11.RandR.FreeCrtcInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRFreeCrtcInfo>(s_Data.X11.RandR.Handle,
		                                                                                                   "XRRFreeCrtcInfo");
		s_Data.X11.RandR.FreeOutputInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRFreeOutputInfo>(s_Data.X11.RandR.Handle,
		                                                                                                       "XRRFreeOutputInfo");
		s_Data.X11.RandR.FreeScreenResources = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRFreeScreenResources>(s_Data.X11.RandR.Handle,
		                                                                                                                 "XRRFreeScreenResources");
		s_Data.X11.RandR.GetCrtcInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRGetCrtcInfo>(s_Data.X11.RandR.Handle,
		                                                                                                 "XRRGetCrtcInfo");
		s_Data.X11.RandR.GetOutputInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRGetOutputInfo>(s_Data.X11.RandR.Handle,
		                                                                                                     "XRRGetOutputInfo");
		s_Data.X11.RandR.GetOutputPrimary = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRGetOutputPrimary>(s_Data.X11.RandR.Handle,
		                                                                                                           "XRRGetOutputPrimary");
		s_Data.X11.RandR.GetScreenResourcesCurrent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRGetScreenResourcesCurrent>(s_Data.X11.RandR.Handle,
		 																														     "XRRGetScreenResourcesCurrent");
		s_Data.X11.RandR.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRQueryExtension>(s_Data.X11.RandR.Handle,
		                                                                                                       "XRRQueryExtension");
		s_Data.X11.RandR.QueryVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRQueryVersion>(s_Data.X11.RandR.Handle,
		                                                                                                   "XRRQueryVersion");
		s_Data.X11.RandR.SelectInput = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRSelectInput>(s_Data.X11.RandR.Handle,
		                                                                                                 "XRRSelectInput");
		s_Data.X11.RandR.SetCrtcConfig = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRSetCrtcConfig>(s_Data.X11.RandR.Handle,
		                                                                                                     "XRRSetCrtcConfig");
		s_Data.X11.RandR.UpdateConfiguration = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRRUpdateConfiguration>(s_Data.X11.RandR.Handle,
																													     "XRRUpdateConfiguration");

		if(s_Data.X11.RandR.QueryExtension(s_Data.X11.display, &s_Data.X11.RandR.EventBase, &s_Data.X11.RandR.ErrorBase) != 0)
		{
			if(s_Data.X11.RandR.QueryVersion(s_Data.X11.display, &s_Data.X11.RandR.Major, &s_Data.X11.RandR.Minor) != 0)
			{
				//The TRAP RandR path requires at least verison 1.3
				if(s_Data.X11.RandR.Major > 1 || s_Data.X11.RandR.Minor >= 3)
					s_Data.X11.RandR.Available = true;
			}
			else
				InputError(Error::Platform_Error, "[X11] Failed to query RandR version!");
		}
	}

	if(s_Data.X11.RandR.Available)
	{
		XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);

		if(sr->ncrtc == 0)
		{
			//A system without CRTCs is likely a system with borken RandR
			//Disable the RandR monitor path and fall back to core functions
			s_Data.X11.RandR.MonitorBroken = true;
		}

		s_Data.X11.RandR.FreeScreenResources(sr);
	}

	if(s_Data.X11.RandR.Available && !s_Data.X11.RandR.MonitorBroken)
		s_Data.X11.RandR.SelectInput(s_Data.X11.display, s_Data.X11.Root, RROutputChangeNotifyMask);

#if defined(__CYGWIN__)
	s_Data.X11.XCursor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXcursor-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.X11.XCursor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXcursor.so");
#else
	s_Data.X11.XCursor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXcursor.so.1");
#endif
	if(s_Data.X11.XCursor.Handle != nullptr)
	{
		s_Data.X11.XCursor.ImageCreate = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorImageCreate>(s_Data.X11.XCursor.Handle,
		                                                                                                       "XcursorImageCreate");
		s_Data.X11.XCursor.ImageDestroy = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorImageDestroy>(s_Data.X11.XCursor.Handle,
		                                                                                                         "XcursorImageDestroy");
		s_Data.X11.XCursor.ImageLoadCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorImageLoadCursor>(s_Data.X11.XCursor.Handle,
																												       "XcursorImageLoadCursor");
		s_Data.X11.XCursor.GetTheme = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorGetTheme>(s_Data.X11.XCursor.Handle,
													                                                     "XcursorGetTheme");
		s_Data.X11.XCursor.GetDefaultSize = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorGetDefaultSize>(s_Data.X11.XCursor.Handle,
		                                                                                                             "XcursorGetDefaultSize");
		s_Data.X11.XCursor.LibraryLoadImage = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XcursorLibraryLoadImage>(s_Data.X11.XCursor.Handle,
																													     "XcursorLibraryLoadImage");
	}

#if defined(__CYGWIN__)
	s_Data.X11.Xinerama.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXinerama-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.X11.Xinerama.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXinerama.so");
#else
	s_Data.X11.Xinerama.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXinerama.so.1");
#endif
	if(s_Data.X11.Xinerama.Handle != nullptr)
	{
		s_Data.X11.Xinerama.IsActive = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XineramaIsActive>(s_Data.X11.Xinerama.Handle,
		                                                                                                   "XineramaIsActive");
		s_Data.X11.Xinerama.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XineramaQueryExtension>(s_Data.X11.Xinerama.Handle,
																												       "XineramaQueryExtension");
		s_Data.X11.Xinerama.QueryScreens = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XineramaQueryScreens>(s_Data.X11.Xinerama.Handle,
		                                                                                					       "XineramaQueryScreens");

		if(s_Data.X11.Xinerama.QueryExtension(s_Data.X11.display, &s_Data.X11.Xinerama.Major, &s_Data.X11.Xinerama.Minor) != 0)
		{
			if(s_Data.X11.Xinerama.IsActive(s_Data.X11.display) != 0)
				s_Data.X11.Xinerama.Available = true;
		}
	}

	s_Data.X11.XKB.Major = 1;
	s_Data.X11.XKB.Minor = 0;
	s_Data.X11.XKB.Available = (s_Data.X11.XKB.QueryExtension(s_Data.X11.display,
											                  &s_Data.X11.XKB.MajorOPCode,
											                  &s_Data.X11.XKB.EventBase,
											                  &s_Data.X11.XKB.ErrorBase,
											                  &s_Data.X11.XKB.Major,
											                  &s_Data.X11.XKB.Minor) != 0);

	if(s_Data.X11.XKB.Available)
	{
		int32_t supported = 0;

		if(s_Data.X11.XKB.SetDetectableAutoRepeat(s_Data.X11.display, 1, &supported) != 0)
		{
			if(supported != 0)
				s_Data.X11.XKB.Detectable = true;
		}

		s_Data.X11.XKB.Group = 0;
		XkbStateRec state;
		if(s_Data.X11.XKB.GetState(s_Data.X11.display, XkbUseCoreKbd, &state) == 0) //0 = Success
			s_Data.X11.XKB.Group = static_cast<uint32_t>(state.group);

		s_Data.X11.XKB.SelectEventDetails(s_Data.X11.display, XkbUseCoreKbd, XkbStateNotify,
										  XkbGroupStateMask, XkbGroupStateMask);
	}

#if defined(__CYGWIN__)
	s_Data.X11.XCB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11-xcb-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.X11.XCB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11-xcb.so");
#else
	s_Data.X11.XCB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11-xcb.so.1");
#endif
	if(s_Data.X11.XCB.Handle != nullptr)
		s_Data.X11.XCB.GetXCBConnection = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetXCBConnection>(s_Data.X11.XCB.Handle,
		                                                                            					       "XGetXCBConnection");

#if defined(__CYGWIN__)
	s_Data.X11.XRender.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrender-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.X11.XRender.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrender.so");
#else
	s_Data.X11.XRender.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXrender.so.1");
#endif
	if(s_Data.X11.XRender.Handle != nullptr)
	{
		s_Data.X11.XRender.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRenderQueryExtension>(s_Data.X11.XRender.Handle,
		                                                                                                             "XRenderQueryExtension");
		s_Data.X11.XRender.QueryVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRenderQueryVersion>(s_Data.X11.XRender.Handle,
		                                                                                                         "XRenderQueryVersion");
		s_Data.X11.XRender.FindVisualFormat = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRenderFindVisualFormat>(s_Data.X11.XRender.Handle,
																													     "XRenderFindVisualFormat");

		if(s_Data.X11.XRender.QueryExtension(s_Data.X11.display, &s_Data.X11.XRender.ErrorBase, &s_Data.X11.XRender.EventBase) != 0)
		{
			if(s_Data.X11.XRender.QueryVersion(s_Data.X11.display, &s_Data.X11.XRender.Major, &s_Data.X11.XRender.Minor) != 0)
				s_Data.X11.XRender.Available = true;
		}
	}

#if defined(__CYGWIN__)
	s_Data.X11.XShape.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXext-6.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	s_Data.X11.XShape.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXext.so");
#else
	s_Data.X11.XShape.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libXext.so.6");
#endif
	if(s_Data.X11.XShape.Handle != nullptr)
	{
		s_Data.X11.XShape.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XShapeQueryExtension>(s_Data.X11.XShape.Handle,
		                                                                                                           "XShapeQueryExtension");
		s_Data.X11.XShape.CombineRegion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XShapeCombineRegion>(s_Data.X11.XShape.Handle,
		                                                                                                         "XShapeCombineRegion");
		s_Data.X11.XShape.CombineMask = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XShapeCombineMask>(s_Data.X11.XShape.Handle,
		                                                                                                     "XShapeCombineMask");
		s_Data.X11.XShape.QueryVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XShapeQueryVersion>(s_Data.X11.XShape.Handle,
		                                                                                                       "XShapeQueryVersion");

		if(s_Data.X11.XShape.QueryExtension(s_Data.X11.display, &s_Data.X11.XShape.ErrorBase, &s_Data.X11.XShape.EventBase) != 0)
		{
			if(s_Data.X11.XShape.QueryVersion(s_Data.X11.display, &s_Data.X11.XShape.Major, &s_Data.X11.XShape.Minor) != 0)
				s_Data.X11.XShape.Available = true;
		}
	}

	//Update the key code LUT
	CreateKeyTablesX11();

	//String format atoms
	s_Data.X11.NULL_ =       s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "NULL", 0);
	s_Data.X11.UTF8_STRING = s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "UTF8_STRING", 0);
	s_Data.X11.ATOM_PAIR =   s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "ATOM_PAIR", 0);

	//Custom selection property atom
	s_Data.X11.TRAP_SELECTION = s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "TRAP_SELECTION", 0);

	//ICCCM standard clipboard atoms
	s_Data.X11.TARGETS =   s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "TARGETS", 0);
	s_Data.X11.MULTIPLE =  s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "MULTIPLE", 0);
	s_Data.X11.PRIMARY =   s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "PRIMARY", 0);
	s_Data.X11.INCR =      s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "INCR", 0);
	s_Data.X11.CLIPBOARD = s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "CLIPBOARD", 0);

	//Clipboard manager atoms
	s_Data.X11.CLIPBOARD_MANAGER = s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "CLIPBOARD_MANAGER", 0);
	s_Data.X11.SAVE_TARGETS =      s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "SAVE_TARGETS", 0);

	//Xdnd (drag and drop) atoms
	s_Data.X11.XDNDAware =      s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndAware", 0);
	s_Data.X11.XDNDEnter =      s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndEnter", 0);
	s_Data.X11.XDNDPosition =   s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndPosition", 0);
	s_Data.X11.XDNDStatus =     s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndStatus", 0);
	s_Data.X11.XDNDActionCopy = s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndActionCopy", 0);
	s_Data.X11.XDNDDrop =       s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndDrop", 0);
	s_Data.X11.XDNDFinished =   s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndFinished", 0);
	s_Data.X11.XDNDSelection =  s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndSelection", 0);
	s_Data.X11.XDNDTypeList =   s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "XdndTypeList", 0);
	s_Data.X11.text_uri_list =  s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "text/uri-list", 0);

	//ICCCM, EWMH and Motif window property atoms
	//These can be set safely even without WM support
	//The EWMH atoms that require WM support are handled in detectEWMH
	s_Data.X11.WM_PROTOCOLS =             s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "WM_PROTOCOLS", 0);
	s_Data.X11.WM_STATE =                 s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "WM_STATE", 0);
	s_Data.X11.WM_DELETE_WINDOW =         s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "WM_DELETE_WINDOW", 0);
	s_Data.X11.NET_SUPPORTED =            s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_SUPPORTED", 0);
	s_Data.X11.NET_SUPPORTING_WM_CHECK =  s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_SUPPORTING_WM_CHECK", 0);
	s_Data.X11.NET_WM_ICON =              s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_WM_ICON", 0);
	s_Data.X11.NET_WM_PING =              s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_WM_PING", 0);
	s_Data.X11.NET_WM_PID =               s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_WM_PID", 0);
	s_Data.X11.NET_WM_NAME =              s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_WM_NAME", 0);
	s_Data.X11.NET_WM_ICON_NAME =         s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_WM_ICON_NAME", 0);
	s_Data.X11.NET_WM_BYPASS_COMPOSITOR = s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_WM_BYPASS_COMPOSITOR", 0);
	s_Data.X11.NET_WM_WINDOW_OPACITY =    s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_NET_WM_WINDOW_OPACITY", 0);
	s_Data.X11.MOTIF_WM_HINTS =           s_Data.X11.XLIB.InternAtom(s_Data.X11.display, "_MOTIF_WM_HINTS", 0);

	//The compositing manager selection name contains the screen number
	s_Data.X11.NET_WM_CM_Sx = s_Data.X11.XLIB.InternAtom(s_Data.X11.display,
	                                                     std::string("_NET_WM_CM_S" + std::to_string(s_Data.X11.Screen)).c_str(),
												         0);

	//Detect whether an EWMH-conformant window manager is running
	DetectEWMH();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Check whether the running window manager is EMWH-compliant
void TRAP::INTERNAL::WindowingAPI::DetectEWMH()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	//First we read the _NET_SUPPORTING_WM_CHECK property on the root window
	::Window* windowFromRoot = nullptr;
	if(GetWindowPropertyX11(s_Data.X11.Root, s_Data.X11.NET_SUPPORTING_WM_CHECK, XA_WINDOW,
	                        reinterpret_cast<uint8_t**>(&windowFromRoot)) == 0u)
		return;

	GrabErrorHandlerX11();

	//If it exists, it should be the XID of a top-level window
	//Then we look for the same property on that window
	::Window* windowFromChild = nullptr;
	if(GetWindowPropertyX11(*windowFromRoot, s_Data.X11.NET_SUPPORTING_WM_CHECK, XA_WINDOW,
	                        reinterpret_cast<uint8_t**>(&windowFromChild)) == 0u)
	{
		s_Data.X11.XLIB.Free(windowFromRoot);
		return;
	}

	ReleaseErrorHandlerX11();

	//If the property exists, it should contain the XID of the window
	if(*windowFromRoot != *windowFromChild)
	{
		s_Data.X11.XLIB.Free(windowFromRoot);
		s_Data.X11.XLIB.Free(windowFromChild);
		return;
	}

	s_Data.X11.XLIB.Free(windowFromRoot);
	s_Data.X11.XLIB.Free(windowFromChild);

	//We are now fairly sure that an EWMH-compliant WM is currently running
	//We can now start querying the WM about what features it supports by looking in the _NET_SUPPORTED property
	//on the root window
	//It should contain a list of supported EWMH protocl and state atoms

	Atom* supportedAtoms = nullptr;
	const uint64_t atomCount = GetWindowPropertyX11(s_Data.X11.Root, s_Data.X11.NET_SUPPORTED, XA_ATOM,
	                                                reinterpret_cast<uint8_t**>(&supportedAtoms));
	if(supportedAtoms == nullptr)
		return;

	const std::vector<Atom> supportedAtomsVec(supportedAtoms, supportedAtoms + atomCount);

	//See which of the atoms we support that are supported by the WM

	s_Data.X11.NET_WM_STATE                   = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_STATE").value_or(0);
    s_Data.X11.NET_WM_STATE_ABOVE             = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_STATE_ABOVE").value_or(0);
    s_Data.X11.NET_WM_STATE_FULLSCREEN        = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_STATE_FULLSCREEN").value_or(0);
    s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT    = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_STATE_MAXIMIZED_VERT").value_or(0);
    s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ    = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_STATE_MAXIMIZED_HORZ").value_or(0);
    s_Data.X11.NET_WM_STATE_DEMANDS_ATTENTION = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_STATE_DEMANDS_ATTENTION").value_or(0);
    s_Data.X11.NET_WM_FULLSCREEN_MONITORS     = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_FULLSCREEN_MONITORS").value_or(0);
    s_Data.X11.NET_WM_WINDOW_TYPE             = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_WINDOW_TYPE").value_or(0);
    s_Data.X11.NET_WM_WINDOW_TYPE_NORMAL      = GetAtomIfSupported(supportedAtomsVec, "_NET_WM_WINDOW_TYPE_NORMAL").value_or(0);
    s_Data.X11.NET_WORKAREA                   = GetAtomIfSupported(supportedAtomsVec, "_NET_WORKAREA").value_or(0);
    s_Data.X11.NET_CURRENT_DESKTOP            = GetAtomIfSupported(supportedAtomsVec, "_NET_CURRENT_DESKTOP").value_or(0);
    s_Data.X11.NET_ACTIVE_WINDOW              = GetAtomIfSupported(supportedAtomsVec, "_NET_ACTIVE_WINDOW").value_or(0);

	if(supportedAtoms != nullptr)
		s_Data.X11.XLIB.Free(supportedAtoms);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the X error handler callback
void TRAP::INTERNAL::WindowingAPI::GrabErrorHandlerX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(s_Data.X11.PrevErrorHandler == nullptr, "WindowingAPI::GrabErrorHandlerX11(): Previous error handler is not nullptr!");

	s_Data.X11.ErrorCode = 0; //0 = Success
	s_Data.X11.PrevErrorHandler = s_Data.X11.XLIB.SetErrorHandler(ErrorHandler);
}

//-------------------------------------------------------------------------------------------------------------------//

//X error handler
[[nodiscard]] int32_t TRAP::INTERNAL::WindowingAPI::ErrorHandler(Display* const display, XErrorEvent* const event)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(display, "WindowingAPI::ErrorHandler(): Display is nullptr!");
	TRAP_ASSERT(event, "WindowingAPI::ErrorHandler(): XErrorEvent is nullptr!");

	if(s_Data.X11.display != display)
		return 0;

	s_Data.X11.ErrorCode = event->error_code;
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

//Clears the X error handler callback
void TRAP::INTERNAL::WindowingAPI::ReleaseErrorHandlerX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	//Synchronize to make sure all commands are processed
	s_Data.X11.XLIB.Sync(s_Data.X11.display, 0);
	s_Data.X11.XLIB.SetErrorHandler(s_Data.X11.PrevErrorHandler);
	s_Data.X11.PrevErrorHandler = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Return the atom ID only if it is listed in the specified array
[[nodiscard]] std::optional<Atom> TRAP::INTERNAL::WindowingAPI::GetAtomIfSupported(const std::vector<Atom>& supportedAtoms,
                                                                                   const std::string_view atomName)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const Atom searchAtom = s_Data.X11.XLIB.InternAtom(s_Data.X11.display, atomName.data(), 0);

	const auto res = std::find(supportedAtoms.begin(), supportedAtoms.end(), searchAtom);
	if(res != supportedAtoms.end())
		return *res;

	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create a blank cursor for hidden and disabled cursor modes
[[nodiscard]] std::optional<Cursor> TRAP::INTERNAL::WindowingAPI::CreateHiddenCursor()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const std::array<uint8_t, static_cast<std::size_t>(16) * 16 * 4> pixels = {0};
	const Scope<Image> cursorImage = TRAP::Image::LoadFromMemory(16, 16, TRAP::Image::ColorFormat::RGBA,
	                                                             std::vector<uint8_t>(pixels.begin(), pixels.end()));
	if(!cursorImage)
		return std::nullopt;

	return CreateCursorX11(*cursorImage, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

//Check whether the IM has a usable style
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::HasUsableInputMethodStyle()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	XIMStyles* styles = nullptr;

	if(s_Data.X11.XLIB.GetIMValues(s_Data.X11.IM, XNQueryInputStyle, &styles, nullptr) != nullptr || (styles == nullptr))
		return false;

	for(uint16_t i = 0; i < styles->count_styles; i++)
	{
		if(styles->supported_styles[i] == (XIMPreeditNothing | XIMStatusNothing))
		{
			s_Data.X11.XLIB.Free(styles);
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputMethodDestroyCallback([[maybe_unused]] XIM im,
                                                              [[maybe_unused]] XPointer clientData,
															  [[maybe_unused]] XPointer callData)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_Data.X11.IM = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputMethodInstantiateCallback([[maybe_unused]] Display* display,
                                                                  [[maybe_unused]] XPointer clientData,
																  [[maybe_unused]] XPointer callData)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(s_Data.X11.IM != nullptr)
		return;

	s_Data.X11.IM = s_Data.X11.XLIB.OpenIM(s_Data.X11.display, nullptr, nullptr, nullptr);
	if(s_Data.X11.IM == nullptr)
		return;

	if(!HasUsableInputMethodStyle())
	{
		s_Data.X11.XLIB.CloseIM(s_Data.X11.IM);
		s_Data.X11.IM = nullptr;
		return;
	}

	XIMCallback callback{};
	callback.callback = static_cast<XIMProc>(InputMethodDestroyCallback);
	callback.client_data = nullptr;
	s_Data.X11.XLIB.SetIMValues(s_Data.X11.IM, XNDestroyCallback, &callback, nullptr);

	for(const Scope<InternalWindow>& window : s_Data.WindowList)
		CreateInputContextX11(*window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Poll for changes in the set of connected monitors
void TRAP::INTERNAL::WindowingAPI::PollMonitorsX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.X11.RandR.Available || s_Data.X11.RandR.MonitorBroken)
	{
		InputMonitor(CreateMonitor("Display"), true, 0);
		return;
	}

	int32_t disconnectedCount = 0, screenCount = 0;
	std::vector<InternalMonitor*> disconnected{};
	XineramaScreenInfo* screens = nullptr;
	XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
	const RROutput primary = s_Data.X11.RandR.GetOutputPrimary(s_Data.X11.display, s_Data.X11.Root);

	if(sr == nullptr)
	{
		InputErrorX11(Error::Platform_Error, "[X11] Failed to poll monitors (failed to retrieve current screen resources)!");
		return;
	}

	if(s_Data.X11.Xinerama.Available)
		screens = s_Data.X11.Xinerama.QueryScreens(s_Data.X11.display, &screenCount);

	disconnectedCount = static_cast<int32_t>(s_Data.Monitors.size());
	if(disconnectedCount != 0)
	{
		disconnected.resize(s_Data.Monitors.size());
		for(std::size_t i = 0; i < s_Data.Monitors.size(); i++)
			disconnected[i] = s_Data.Monitors[i].get();
	}

	for(int32_t i = 0; i < sr->noutput; i++)
	{
		int32_t j = 0, type = 0;

		XRROutputInfo* const oi = s_Data.X11.RandR.GetOutputInfo(s_Data.X11.display, sr, sr->outputs[i]);
		if((oi == nullptr) || oi->connection != RR_Connected || oi->crtc == 0)
		{
			if(oi != nullptr)
				s_Data.X11.RandR.FreeOutputInfo(oi);
			continue;
		}

		XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, oi->crtc);
		if(ci == nullptr)
		{
			if(oi != nullptr)
				s_Data.X11.RandR.FreeOutputInfo(oi);
			continue;
		}

		for(j = 0; j < disconnectedCount; j++)
		{
			if((disconnected[j] != nullptr) && disconnected[j]->X11.Output == sr->outputs[i])
			{
				disconnected[j] = nullptr;
				break;
			}
		}

		if(j < disconnectedCount)
		{
			s_Data.X11.RandR.FreeOutputInfo(oi);
			s_Data.X11.RandR.FreeCrtcInfo(ci);
			continue;
		}

		Scope<InternalMonitor> monitor = CreateMonitor(oi->name);
		monitor->X11.Output = sr->outputs[i];
		monitor->X11.CRTC = oi->crtc;

		if(screens != nullptr)
		{
			for(j = 0; j < screenCount; j++)
			{
				if(screens[j].x_org == ci->x && screens[j].y_org == ci->y &&
				static_cast<uint32_t>(screens[j].width) == ci->width &&
				static_cast<uint32_t>(screens[j].height) == ci->height)
				{
					monitor->X11.Index = j;
					break;
				}
			}
		}

		if(monitor->X11.Output == primary)
			type = 0;
		else
			type = 1;

		InputMonitor(std::move(monitor), true, type);

		s_Data.X11.RandR.FreeOutputInfo(oi);
		s_Data.X11.RandR.FreeCrtcInfo(ci);
	}

	if(sr != nullptr)
		s_Data.X11.RandR.FreeScreenResources(sr);

	if(screens != nullptr)
		s_Data.X11.XLIB.Free(screens);

	for(int32_t i = 0; i < disconnectedCount; i++)
	{
		if(disconnected[i] != nullptr)
			InputMonitorDisconnect(i, 0);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the event is a selection event
[[nodiscard]] int32_t TRAP::INTERNAL::WindowingAPI::IsSelectionEvent([[maybe_unused]] Display* const display,
                                                                      XEvent* const event,
																	  [[maybe_unused]] XPointer ptr)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(event, "WindowingAPI::IsSelectionEvent(): XEvent is nullptr!");

	if(event->xany.window != s_Data.X11.HelperWindowHandle)
		return 0;

	return static_cast<int32_t>(event->type == SelectionRequest ||
	                            event->type == SelectionNotify ||
		                        event->type == SelectionClear);
}

//-------------------------------------------------------------------------------------------------------------------//

//Set the specified property to the selection converted to the requested target
[[nodiscard]] Atom TRAP::INTERNAL::WindowingAPI::WriteTargetToProperty(const XSelectionRequestEvent& request)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::string selectionString{};
	const std::array<Atom, 2> formats{s_Data.X11.UTF8_STRING, XA_STRING};

	if(request.selection == s_Data.X11.PRIMARY)
		selectionString = s_Data.X11.PrimarySelectionString;
	else
		selectionString = s_Data.ClipboardString;

	if(request.property == None)
	{
		//The requestor is a legacy client
		//We don't support legacy clients, so fail here
		return None;
	}

	if(request.target == s_Data.X11.TARGETS)
	{
		//The list of supported targets was requested
		const std::array<Atom, 4> targets{s_Data.X11.TARGETS, s_Data.X11.MULTIPLE, s_Data.X11.UTF8_STRING, XA_STRING};

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, request.property, XA_ATOM, 32,
		                               PropModeReplace, reinterpret_cast<const uint8_t*>(targets.data()),
								       targets.size());

		return request.property;
	}

	if(request.target == s_Data.X11.MULTIPLE)
	{
		//Multiple conversions were requested

		Atom* targets = nullptr;
		const uint64_t count = GetWindowPropertyX11(request.requestor, request.property, s_Data.X11.ATOM_PAIR,
		                                            reinterpret_cast<uint8_t**>(&targets));

		if(targets == nullptr)
			return None;

		for(uint64_t i = 0; i < count; i += 2)
		{
			if(std::find(formats.begin(), formats.end(), targets[i]) != formats.end())
			{
				s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, targets[i + 1], targets[i], 8,
				                               PropModeReplace, reinterpret_cast<uint8_t*>(selectionString.data()),
										       static_cast<int>(selectionString.size()));
			}
			else
				targets[i + 1] = None;
		}

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, request.property, s_Data.X11.ATOM_PAIR, 32,
                                       PropModeReplace, reinterpret_cast<uint8_t*>(targets), static_cast<int>(count));

		s_Data.X11.XLIB.Free(targets);

		return request.property;
	}

	if(request.target == s_Data.X11.SAVE_TARGETS)
	{
		//The request is a check whether we support SAVE_TARGETS
		//It should be handled as a no-op side effect target

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, request.property, s_Data.X11.NULL_, 32,
		                               PropModeReplace, nullptr, 0);

		return request.property;
	}

	//Conversion to a data target was requested

	if(std::find(formats.begin(), formats.end(), request.target) != formats.end())
	{
		//The requested target is one we support

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, request.property, request.target, 8,
									   PropModeReplace, reinterpret_cast<uint8_t*>(selectionString.data()),
									   static_cast<int>(selectionString.size()));

		return request.property;
	}

	//The request target is not supported

	return None;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::HandleSelectionRequest(XEvent& event)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const XSelectionRequestEvent& request = event.xselectionrequest;

	XEvent reply = {SelectionNotify};
	reply.xselection.property = WriteTargetToProperty(request);
	reply.xselection.display = request.display;
	reply.xselection.requestor = request.requestor;
	reply.xselection.selection = request.selection;
	reply.xselection.target = request.target;
	reply.xselection.time = request.time;

	s_Data.X11.XLIB.SendEvent(s_Data.X11.display, request.requestor, 0, 0, &reply);
}

//-------------------------------------------------------------------------------------------------------------------//

//Push contents of our selection to clipboard manager
void TRAP::INTERNAL::WindowingAPI::PushSelectionToManagerX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	s_Data.X11.XLIB.ConvertSelection(s_Data.X11.display, s_Data.X11.CLIPBOARD_MANAGER, s_Data.X11.SAVE_TARGETS, None,
	                                 s_Data.X11.HelperWindowHandle, CurrentTime);

	while(true)
	{
		XEvent event{};

		while(s_Data.X11.XLIB.CheckIfEvent(s_Data.X11.display, &event, IsSelectionEvent, nullptr) != 0)
		{
			switch(event.type)
			{
			case SelectionRequest:
				HandleSelectionRequest(event);
				break;

			case SelectionNotify:
				if(event.xselection.target == s_Data.X11.SAVE_TARGETS)
				{
					//This means one of two things.
					//Either the selection was not owned, which means there is no clipboard manager,
					//or the transfer to the clipboard manager has completed
					//In either case, it means we are done here
					return;
				}
				break;

			default:
				break;
			}
		}

		WaitForX11Event(nullptr);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CreateInputContextX11(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	XIMCallback callback{};
	callback.callback = reinterpret_cast<XIMProc>(InputContextDestroyCallback);
	callback.client_data = reinterpret_cast<XPointer>(&window);

	window.X11.IC = s_Data.X11.XLIB.CreateIC(s_Data.X11.IM,
		                                     XNInputStyle,
		                                     XIMPreeditNothing | XIMStatusNothing,
		                                     XNClientWindow,
		                                     window.X11.Handle,
		                                     XNFocusWindow,
		                                     window.X11.Handle,
		                                     XNDestroyCallback,
		                                     &callback,
		                                     nullptr);

	if(window.X11.IC == nullptr)
		return;

	XWindowAttributes attribs;
	s_Data.X11.XLIB.GetWindowAttributes(s_Data.X11.display, window.X11.Handle, &attribs);

	uint64_t filter = 0;
	if(s_Data.X11.XLIB.GetICValues(window.X11.IC, XNFilterEvents, &filter, nullptr) == nullptr)
		s_Data.X11.XLIB.SelectInput(s_Data.X11.display, window.X11.Handle, static_cast<int64_t>(attribs.your_event_mask | filter));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::IsVisualTransparentX11(const Visual& visual)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.X11.XRender.Available)
		return false;

	const XRenderPictFormat* const pf = s_Data.X11.XRender.FindVisualFormat(s_Data.X11.display, &visual);
	return (pf != nullptr) && (pf->direct.alphaMask != 0);
}

//-------------------------------------------------------------------------------------------------------------------//

//Create the X11 window (and its colormap)
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::CreateNativeWindow(InternalWindow& window, WindowConfig& WNDConfig,
                                                                    Visual& visual, int32_t depth)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const int32_t width = static_cast<int32_t>(WNDConfig.Width);
	const int32_t height = static_cast<int32_t>(WNDConfig.Height);

	//Create a colormap based on the visual used by the current context
	window.X11.colormap = s_Data.X11.XLIB.CreateColormap(s_Data.X11.display, s_Data.X11.Root, &visual, AllocNone);

	window.Transparent = IsVisualTransparentX11(visual);

	XSetWindowAttributes wa{};
	wa.colormap = window.X11.colormap;
	wa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
	                PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
					ExposureMask | FocusChangeMask | VisibilityChangeMask |
					EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

	GrabErrorHandlerX11();

	window.X11.Parent = s_Data.X11.Root;
	window.X11.Handle = s_Data.X11.XLIB.CreateWindow(s_Data.X11.display, s_Data.X11.Root,
	                                                 0, 0,
								                     width, height,
								                     0,
								                     depth,
								                     InputOutput,
								                     &visual,
								                     CWBorderPixel | CWColormap | CWEventMask,
								                     &wa);

	ReleaseErrorHandlerX11();

	if(window.X11.Handle == 0u)
	{
		InputErrorX11(Error::Platform_Error, "[X11] Failed to create window!");
		return false;
	}

	s_Data.X11.XLIB.SaveContext(s_Data.X11.display, window.X11.Handle, s_Data.X11.Context, reinterpret_cast<XPointer>(&window));

	if(!WNDConfig.Decorated)
		PlatformSetWindowDecoratedX11(window, false);

	if((s_Data.X11.NET_WM_STATE != 0u) && (window.Monitor == nullptr))
	{
		std::array<Atom, 3> states{};
		int32_t count = 0;

		if(WNDConfig.Floating)
		{
			if(s_Data.X11.NET_WM_STATE_ABOVE != 0u)
				states[count++] = s_Data.X11.NET_WM_STATE_ABOVE;
		}

		if(WNDConfig.Maximized)
		{
			if((s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT != 0u) &&
			   (s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ != 0u))
			{
				states[count++] = s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT;
				states[count++] = s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ;
				window.Maximized = true;
			}
		}

		if(count != 0)
		{
			s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM, 32,
							               PropModeReplace, reinterpret_cast<const uint8_t*>(states.data()), count);
		}
	}

	//Declare the WM protocols supported by TRAP
	{
		std::array<Atom, 2> protocols{s_Data.X11.WM_DELETE_WINDOW, s_Data.X11.NET_WM_PING};

		s_Data.X11.XLIB.SetWMProtocols(s_Data.X11.display, window.X11.Handle, protocols.data(), protocols.size());
	}

	//Declare our PID
	{
		const int64_t pid = getpid();
		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_PID, XA_CARDINAL, 32,
		                               PropModeReplace, reinterpret_cast<const uint8_t*>(&pid), 1);
	}

	if((s_Data.X11.NET_WM_WINDOW_TYPE != 0u) && (s_Data.X11.NET_WM_WINDOW_TYPE_NORMAL != 0u))
	{
		const Atom type = s_Data.X11.NET_WM_WINDOW_TYPE_NORMAL;
		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_WINDOW_TYPE, XA_ATOM, 32,
		                               PropModeReplace, reinterpret_cast<const uint8_t*>(&type), 1);
	}

	//Set ICCCM WM_HINTS property
	{
		XWMHints* const hints = s_Data.X11.XLIB.AllocWMHints();
		if(hints == nullptr)
		{
			InputError(Error::Out_Of_Memory, "[X11] Failed to allocate WM hints!");
			return false;
		}

		hints->flags = StateHint;
		hints->initial_state = NormalState;

		s_Data.X11.XLIB.SetWMHints(s_Data.X11.display, window.X11.Handle, hints);
		s_Data.X11.XLIB.Free(hints);
	}

	//Set ICCCM WM_NORMAL_HINTS property
	{
		XSizeHints* const hints = s_Data.X11.XLIB.AllocSizeHints();
		if(hints == nullptr)
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

		s_Data.X11.XLIB.SetWMNormalHints(s_Data.X11.display, window.X11.Handle, hints);
		s_Data.X11.XLIB.Free(hints);
	}

	//Set ICCCM WM_CLASS property
	{
		XClassHint* const hint = s_Data.X11.XLIB.AllocClassHint();
		if(hint == nullptr)
		{
			InputError(Error::Out_Of_Memory, "[X11] Failed to allocate class hint!");
			return false;
		}
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

		s_Data.X11.XLIB.SetClassHint(s_Data.X11.display, window.X11.Handle, hint);
		s_Data.X11.XLIB.Free(hint);
	}

	if(s_Data.X11.IM != nullptr)
		CreateInputContextX11(window);

	PlatformSetWindowTitleX11(window, WNDConfig.Title);
	PlatformGetWindowPosX11(window, window.X11.XPos, window.X11.YPos);
	PlatformGetWindowSizeX11(window, window.Width, window.Height);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a native cursor object from the specified image and hotspot
[[nodiscard]] std::optional<Cursor> TRAP::INTERNAL::WindowingAPI::CreateCursorX11(const TRAP::Image& image,
                                                                                  const int32_t xHotSpot,
                                                                                  const int32_t yHotSpot)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(s_Data.X11.XCursor.Handle == nullptr)
		return std::nullopt;

	XcursorImage* const native = s_Data.X11.XCursor.ImageCreate(static_cast<int32_t>(image.GetWidth()),
	                                                            static_cast<int32_t>(image.GetHeight()));
	if(native == nullptr)
		return std::nullopt;

	native->xhot = xHotSpot;
	native->yhot = yHotSpot;

	const uint8_t* source = static_cast<const uint8_t*>(image.GetPixelData());
	XcursorPixel* target = native->pixels;

	for(uint64_t i = 0; i < static_cast<uint64_t>(image.GetWidth()) * static_cast<uint64_t>(image.GetHeight()); i++, target++, source += 4)
	{
		const uint32_t alpha = source[3];

		*target = (alpha << 24u) |
		          (static_cast<uint8_t>((source[0] * alpha) / 255u) << 16u) |
				  (static_cast<uint8_t>((source[1] * alpha) / 255u) <<  8u) |
				  (static_cast<uint8_t>((source[2] * alpha) / 255u) <<  0u);
	}

	const Cursor cursor = s_Data.X11.XCursor.ImageLoadCursor(s_Data.X11.display, native);
	s_Data.X11.XCursor.ImageDestroy(native);

	return cursor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the window is iconified
[[nodiscard]] int32_t TRAP::INTERNAL::WindowingAPI::GetWindowState(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t result = WithdrawnState;
	struct State
	{
		CARD32 State{};
		::Window Icon{};
	} *state = nullptr;

	if(GetWindowPropertyX11(window.X11.Handle, s_Data.X11.WM_STATE, s_Data.X11.WM_STATE,
	                        reinterpret_cast<uint8_t**>(&state)) >= 2 && (state != nullptr))
		result = static_cast<int32_t>(state->State);

	if(state != nullptr)
		s_Data.X11.XLIB.Free(state);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::GetSelectionString(const Atom selection)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::string& selectionString = (selection == s_Data.X11.PRIMARY) ? s_Data.X11.PrimarySelectionString : s_Data.ClipboardString;
	const std::array<Atom, 2> targets = {s_Data.X11.UTF8_STRING, XA_STRING};

	if(s_Data.X11.XLIB.GetSelectionOwner(s_Data.X11.display, selection) == s_Data.X11.HelperWindowHandle)
	{
		//Instead of doing a large number of X round-trips just to put this string into a window property and
		//then read it back, just return it
		return selectionString;
	}

	selectionString.clear();

	for(const Atom& target : targets)
	{
		char* data = nullptr;
		Atom actualType = 0;
		int32_t actualFormat = 0;
		uint64_t itemCount = 0, bytesAfter = 0;
		XEvent notification, dummy;

		s_Data.X11.XLIB.ConvertSelection(s_Data.X11.display, selection, target, s_Data.X11.TRAP_SELECTION,
		                                 s_Data.X11.HelperWindowHandle, CurrentTime);

		while(s_Data.X11.XLIB.CheckTypedWindowEvent(s_Data.X11.display, s_Data.X11.HelperWindowHandle, SelectionNotify,
		      &notification) == 0)
			WaitForX11Event(nullptr);

		if(notification.xselection.property == None)
			continue;

		s_Data.X11.XLIB.CheckIfEvent(s_Data.X11.display, &dummy, IsSelPropNewValueNotify,
		                             reinterpret_cast<XPointer>(&notification));

		s_Data.X11.XLIB.GetWindowProperty(s_Data.X11.display, notification.xselection.requestor,
		                                  notification.xselection.property, 0, std::numeric_limits<long>::max(), 1, AnyPropertyType,
									      &actualType, &actualFormat, &itemCount, &bytesAfter,
									      reinterpret_cast<uint8_t**>(&data));

		if(actualType == s_Data.X11.INCR)
		{
			std::string string{};

			while(true)
			{
				while(s_Data.X11.XLIB.CheckIfEvent(s_Data.X11.display, &dummy, IsSelPropNewValueNotify,
				                                    reinterpret_cast<XPointer>(&notification)) == 0)
					WaitForX11Event(nullptr);

				s_Data.X11.XLIB.Free(data);
				s_Data.X11.XLIB.GetWindowProperty(s_Data.X11.display, notification.xselection.requestor,
				                                  notification.xselection.property, 0, std::numeric_limits<long>::max(), 1, AnyPropertyType,
											      &actualType, &actualFormat, &itemCount, &bytesAfter,
											      reinterpret_cast<uint8_t**>(&data));

				if(itemCount != 0u)
					string = data;

				if(itemCount == 0u)
				{
					if(string.empty())
					{
						if(target == XA_STRING)
						{
							selectionString = ConvertLatin1ToUTF8(string.c_str());
							string.clear();
						}
						else
							selectionString = string;
					}

					break;
				}
			}
		}
		else if(actualType == target)
		{
			if(target == XA_STRING)
				selectionString = ConvertLatin1ToUTF8(data);
			else
				selectionString = data;
		}

		s_Data.X11.XLIB.Free(data);

		if(!selectionString.empty())
			break;
	}

	if(selectionString.empty())
		InputError(Error::Format_Unavailable, "[X11] Failed to convert selection to string");

	return selectionString;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether it is a property event for the specified selection transfer
[[nodiscard]] int32_t TRAP::INTERNAL::WindowingAPI::IsSelPropNewValueNotify([[maybe_unused]] Display* const display,
                                                                            XEvent* const event, XPointer ptr)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(event, "WindowingAPI::IsSelPropNewValueNotify(): XEvent is nullptr!");
	TRAP_ASSERT(ptr, "WindowingAPI::IsSelPropNewValueNotify(): XPointer is nullptr!");

	const XEvent* const notification = reinterpret_cast<XEvent*>(ptr);

	return static_cast<int32_t>(event->type == PropertyNotify &&
	                            event->xproperty.state == PropertyNewValue &&
		                        event->xproperty.window == notification->xselection.requestor &&
		                        event->xproperty.atom == notification->xselection.property);
}

//-------------------------------------------------------------------------------------------------------------------//

//Convert the specified Latin-1 string to UTF-8
[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::ConvertLatin1ToUTF8(const std::string_view source)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::size_t size = 1;

	for(const char* sp = source.data(); *sp != 0; sp++)
		size += (*sp & 0x80u) != 0 ? 2 : 1;

	std::string target{};
	target.reserve(size);

	for(const char* sp = source.data(); *sp != 0; sp++)
		target += Utils::String::EncodeUTF8(*sp);

	return target;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModeX11(const InternalMonitor& monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::optional<InternalVideoMode> mode{};

	if(s_Data.X11.RandR.Available && !s_Data.X11.RandR.MonitorBroken)
	{
		XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
		XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);

		if((sr != nullptr) && (ci != nullptr))
		{
			const XRRModeInfo* const mi = GetModeInfo(*sr, ci->mode);
			if(mi != nullptr) //mi can be nullptr if the monitor has been disconnected
				mode = VideoModeFromModeInfo(*mi, *ci);
		}

		if(ci != nullptr)
			s_Data.X11.RandR.FreeCrtcInfo(ci);

		if(sr != nullptr)
			s_Data.X11.RandR.FreeScreenResources(sr);
	}

	if(!mode)
	{
		mode = InternalVideoMode{};
		mode->Width = DisplayWidth(s_Data.X11.display, s_Data.X11.Screen);
		mode->Height = DisplayHeight(s_Data.X11.display, s_Data.X11.Screen);
		mode->RefreshRate = 60.0;

		SplitBPP(DefaultDepth(s_Data.X11.display, s_Data.X11.Screen), mode->RedBits, mode->GreenBits, mode->BlueBits);
	}

	return *mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSizeX11(const InternalWindow& window, int32_t& width,
                                                            int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	XWindowAttributes attribs;
	s_Data.X11.XLIB.GetWindowAttributes(s_Data.X11.display, window.X11.Handle, &attribs);

	width = attribs.width;
	height = attribs.height;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPosX11(const InternalWindow& window, const int32_t xPos,
                                                           const int32_t yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	//HACK: Explicitly setting PPosition to any value causes some WMs, notably Compiz and Metacity, to honor
	//      the position of unmapped windows
	if(!PlatformWindowVisibleX11(window))
	{
		int64_t supplied = 0;
		XSizeHints* const hints = s_Data.X11.XLIB.AllocSizeHints();

		if(s_Data.X11.XLIB.GetWMNormalHints(s_Data.X11.display, window.X11.Handle, hints, &supplied) != 0)
		{
			hints->flags |= PPosition;
			hints->x = hints->y = 0;

			s_Data.X11.XLIB.SetWMNormalHints(s_Data.X11.display, window.X11.Handle, hints);
		}

		s_Data.X11.XLIB.Free(hints);
	}

	s_Data.X11.XLIB.MoveWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorX11(InternalWindow& window,
                                                               InternalMonitor* const monitor, const int32_t xPos,
															   const int32_t yPos, const int32_t width,
															   const int32_t height,
															   [[maybe_unused]] const double refreshRate)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.Monitor == monitor)
	{
		if(monitor != nullptr)
		{
			if(monitor->Window == &window)
				AcquireMonitor(window);
		}
		else
		{
			if(!window.Resizable)
				UpdateNormalHints(window, width, height);

			s_Data.X11.XLIB.MoveResizeWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos, width, height);
		}

		s_Data.X11.XLIB.Flush(s_Data.X11.display);
		return;
	}

	if(window.Monitor != nullptr)
	{
		PlatformSetWindowDecoratedX11(window, window.Decorated);
		PlatformSetWindowFloatingX11(window, window.Floating);
		ReleaseMonitor(window);
	}
	if(window.BorderlessFullscreen)
		window.BorderlessFullscreen = false;

	window.Monitor = monitor;
	UpdateNormalHints(window, width, height);

	if(window.Monitor != nullptr)
	{
		if(!PlatformWindowVisibleX11(window))
		{
			s_Data.X11.XLIB.MapRaised(s_Data.X11.display, window.X11.Handle);
			WaitForVisibilityNotify(window);
		}

		UpdateWindowMode(window);
		AcquireMonitor(window);
	}
	else
	{
		UpdateWindowMode(window);
		s_Data.X11.XLIB.MoveResizeWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos, width, height);
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderlessX11(InternalWindow& window,
                                                                         InternalMonitor& monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	window.BorderlessFullscreen = true;
	window.Monitor = &monitor;
	UpdateNormalHints(window, 0, 0);

	if(window.Monitor == nullptr)
		return;

	if(!PlatformWindowVisibleX11(window))
	{
		s_Data.X11.XLIB.MapRaised(s_Data.X11.display, window.X11.Handle);
		WaitForVisibilityNotify(window);
	}

	UpdateWindowMode(window);
	AcquireMonitorBorderless(window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModesX11(const InternalMonitor& monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::vector<InternalVideoMode> result{};
	uint32_t count = 0;

	if(s_Data.X11.RandR.Available && !s_Data.X11.RandR.MonitorBroken)
	{
		XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
		XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);
		XRROutputInfo* const oi = s_Data.X11.RandR.GetOutputInfo(s_Data.X11.display, sr, monitor.X11.Output);

		if((sr != nullptr) && (ci != nullptr) && (oi != nullptr))
		{
			result.reserve(oi->nmode);

			for(int32_t i = 0; i < oi->nmode; i++)
			{
				const XRRModeInfo* const mi = GetModeInfo(*sr, oi->modes[i]);
				if((mi == nullptr) || !static_cast<bool>((mi->modeFlags & RR_Interlace) == 0))
					continue;

				const std::optional<InternalVideoMode> mode = VideoModeFromModeInfo(*mi, *ci);
				if(!mode)
					continue;

				const auto pos = std::distance(result.begin(), std::find(result.begin(), result.end(), *mode));

				//Skip duplicate modes
				if(pos < count)
					continue;

				count++;
				result.push_back(*mode);
			}
		}
		else
		{
			const auto mode = PlatformGetVideoModeX11(monitor);
			if(mode)
				result.push_back(*mode);
		}

		if(oi != nullptr)
			s_Data.X11.RandR.FreeOutputInfo(oi);
		if(ci != nullptr)
			s_Data.X11.RandR.FreeCrtcInfo(ci);
		if(sr != nullptr)
			s_Data.X11.RandR.FreeScreenResources(sr);
	}
	else
	{
		const auto mode = PlatformGetVideoModeX11(monitor);
		if(mode)
			result.push_back(*mode);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformInitX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

#if !defined(X_HAVE_UTF8_STRING)
	//HACK: If the current locale is "C" and the Xlib UTF-8 functions are unavailable, apply the environment's
	//      locale in the hope that it's both available and not "C"
	//      This is done because the "C" locale breaks wide character input, which is what we fall back on
	//      when UTF-8 support is missing
	if(strcmp(setLocale(LC_CTYPE, nullptr), "C") == 0)
		setLocale(LC_CTYPE, "");
#endif

	#if defined(__CYGWIN__)
		s_Data.X11.XLIB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11-6.so");
	#elif defined(__OpenBSD__) || defined(__NetBSD__)
		s_Data.X11.XLIB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11.so");
	#else
        s_Data.X11.XLIB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libX11.so.6");
	#endif

	if(s_Data.X11.XLIB.Handle == nullptr)
	{
		InputError(Error::Platform_Error, "[X11] Failed to load Xlib");
		return false;
	}

	s_Data.X11.XLIB.AllocClassHint = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XAllocClassHint>(s_Data.X11.XLIB.Handle, "XAllocClassHint");
	s_Data.X11.XLIB.AllocSizeHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XAllocSizeHints>(s_Data.X11.XLIB.Handle, "XAllocSizeHints");
	s_Data.X11.XLIB.AllocWMHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XAllocWMHints>(s_Data.X11.XLIB.Handle, "XAllocWMHints");
	s_Data.X11.XLIB.ChangeProperty = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XChangeProperty>(s_Data.X11.XLIB.Handle, "XChangeProperty");
	s_Data.X11.XLIB.ChangeWindowAttributes = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XChangeWindowAttributes>(s_Data.X11.XLIB.Handle,
																													    "XChangeWindowAttributes");
	s_Data.X11.XLIB.CheckIfEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCheckIfEvent>(s_Data.X11.XLIB.Handle, "XCheckIfEvent");
	s_Data.X11.XLIB.CheckTypedWindowEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCheckTypedWindowEvent>(s_Data.X11.XLIB.Handle,
																												      "XCheckTypedWindowEvent");
	s_Data.X11.XLIB.CloseDisplay = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCloseDisplay>(s_Data.X11.XLIB.Handle, "XCloseDisplay");
	s_Data.X11.XLIB.CloseIM = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCloseIM>(s_Data.X11.XLIB.Handle, "XCloseIM");
	s_Data.X11.XLIB.ConvertSelection = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XConvertSelection>(s_Data.X11.XLIB.Handle,
	                                                                                                        "XConvertSelection");
	s_Data.X11.XLIB.CreateColormap = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateColormap>(s_Data.X11.XLIB.Handle,
	                                                                                                    "XCreateColormap");
	s_Data.X11.XLIB.CreateFontCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateFontCursor>(s_Data.X11.XLIB.Handle,
	                                                                                                        "XCreateFontCursor");
	s_Data.X11.XLIB.CreateIC = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateIC>(s_Data.X11.XLIB.Handle,"XCreateIC");
	s_Data.X11.XLIB.CreateWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateWindow>(s_Data.X11.XLIB.Handle, "XCreateWindow");
	s_Data.X11.XLIB.DefineCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDefineCursor>(s_Data.X11.XLIB.Handle, "XDefineCursor");
	s_Data.X11.XLIB.DeleteContext = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDeleteContext>(s_Data.X11.XLIB.Handle, "XDeleteContext");
	s_Data.X11.XLIB.DeleteProperty = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDeleteProperty>(s_Data.X11.XLIB.Handle, "XDeleteProperty");
	s_Data.X11.XLIB.DestroyIC = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDestroyIC>(s_Data.X11.XLIB.Handle, "XDestroyIC");
	s_Data.X11.XLIB.DestroyWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDestroyWindow>(s_Data.X11.XLIB.Handle, "XDestroyWindow");
	s_Data.X11.XLIB.DisplayKeycodes = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDisplayKeycodes>(s_Data.X11.XLIB.Handle,
	                                                                                                      "XDisplayKeycodes");
	s_Data.X11.XLIB.EventsQueued = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XEventsQueued>(s_Data.X11.XLIB.Handle, "XEventsQueued");
	s_Data.X11.XLIB.FilterEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFilterEvent>(s_Data.X11.XLIB.Handle, "XFilterEvent");
	s_Data.X11.XLIB.FindContext = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFindContext>(s_Data.X11.XLIB.Handle, "XFindContext");
	s_Data.X11.XLIB.Flush = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFlush>(s_Data.X11.XLIB.Handle, "XFlush");
	s_Data.X11.XLIB.Free = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFree>(s_Data.X11.XLIB.Handle, "XFree");
	s_Data.X11.XLIB.FreeColormap = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFreeColormap>(s_Data.X11.XLIB.Handle, "XFreeColormap");
	s_Data.X11.XLIB.FreeCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFreeCursor>(s_Data.X11.XLIB.Handle, "XFreeCursor");
	s_Data.X11.XLIB.FreeEventData = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XFreeEventData>(s_Data.X11.XLIB.Handle, "XFreeEventData");
	s_Data.X11.XLIB.GetAtomName = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetAtomName>(s_Data.X11.XLIB.Handle, "XGetAtomName");
	s_Data.X11.XLIB.GetErrorText = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetErrorText>(s_Data.X11.XLIB.Handle, "XGetErrorText");
	s_Data.X11.XLIB.GetEventData = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetEventData>(s_Data.X11.XLIB.Handle, "XGetEventData");
	s_Data.X11.XLIB.GetICValues = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetICValues>(s_Data.X11.XLIB.Handle, "XGetICValues");
	s_Data.X11.XLIB.GetIMValues = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetIMValues>(s_Data.X11.XLIB.Handle, "XGetIMValues");
	s_Data.X11.XLIB.GetInputFocus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetInputFocus>(s_Data.X11.XLIB.Handle, "XGetInputFocus");
	s_Data.X11.XLIB.GetKeyboardMapping = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetKeyboardMapping>(s_Data.X11.XLIB.Handle,
	                                                                                                            "XGetKeyboardMapping");
	s_Data.X11.XLIB.GetScreenSaver = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetScreenSaver>(s_Data.X11.XLIB.Handle, "XGetScreenSaver");
	s_Data.X11.XLIB.GetSelectionOwner = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetSelectionOwner>(s_Data.X11.XLIB.Handle,
	                                                                               						      "XGetSelectionOwner");
	s_Data.X11.XLIB.GetVisualInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetVisualInfo>(s_Data.X11.XLIB.Handle, "XGetVisualInfo");
	s_Data.X11.XLIB.GetWMNormalHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetWMNormalHints>(s_Data.X11.XLIB.Handle,
	                                                                             					        "XGetWMNormalHints");
	s_Data.X11.XLIB.GetWindowAttributes = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetWindowAttributes>(s_Data.X11.XLIB.Handle,
	                                                                                                              "XGetWindowAttributes");
	s_Data.X11.XLIB.GetWindowProperty = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGetWindowProperty>(s_Data.X11.XLIB.Handle,
	                                                                                                          "XGetWindowProperty");
	s_Data.X11.XLIB.GrabPointer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XGrabPointer>(s_Data.X11.XLIB.Handle, "XGrabPointer");
	s_Data.X11.XLIB.IconifyWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XIconifyWindow>(s_Data.X11.XLIB.Handle, "XIconifyWindow");
	s_Data.X11.XLIB.InitThreads = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XInitThreads>(s_Data.X11.XLIB.Handle, "XInitThreads");
	s_Data.X11.XLIB.InternAtom = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XInternAtom>(s_Data.X11.XLIB.Handle, "XInternAtom");
	s_Data.X11.XLIB.LookupString = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XLookupString>(s_Data.X11.XLIB.Handle, "XLookupString");
	s_Data.X11.XLIB.MapRaised = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XMapRaised>(s_Data.X11.XLIB.Handle, "XMapRaised");
	s_Data.X11.XLIB.MapWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XMapWindow>(s_Data.X11.XLIB.Handle, "XMapWindow");
	s_Data.X11.XLIB.MoveResizeWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XMoveResizeWindow>(s_Data.X11.XLIB.Handle,
	                                                                                                        "XMoveResizeWindow");
	s_Data.X11.XLIB.MoveWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XMoveWindow>(s_Data.X11.XLIB.Handle, "XMoveWindow");
	s_Data.X11.XLIB.NextEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XNextEvent>(s_Data.X11.XLIB.Handle, "XNextEvent");
	s_Data.X11.XLIB.OpenDisplay = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XOpenDisplay>(s_Data.X11.XLIB.Handle, "XOpenDisplay");
	s_Data.X11.XLIB.OpenIM = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XOpenIM>(s_Data.X11.XLIB.Handle, "XOpenIM");
	s_Data.X11.XLIB.PeekEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XPeekEvent>(s_Data.X11.XLIB.Handle, "XPeekEvent");
	s_Data.X11.XLIB.Pending = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XPending>(s_Data.X11.XLIB.Handle, "XPending");
	s_Data.X11.XLIB.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XQueryExtension>(s_Data.X11.XLIB.Handle, "XQueryExtension");
	s_Data.X11.XLIB.QueryPointer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XQueryPointer>(s_Data.X11.XLIB.Handle, "XQueryPointer");
	s_Data.X11.XLIB.RaiseWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRaiseWindow>(s_Data.X11.XLIB.Handle, "XRaiseWindow");
	s_Data.X11.XLIB.RegisterIMInstantiateCallback = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XRegisterIMInstantiateCallback>(s_Data.X11.XLIB.Handle,
																																      "XRegisterIMInstantiateCallback");
	s_Data.X11.XLIB.ResizeWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XResizeWindow>(s_Data.X11.XLIB.Handle, "XResizeWindow");
	s_Data.X11.XLIB.ResourceManagerString = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XResourceManagerString>(s_Data.X11.XLIB.Handle,
																												      "XResourceManagerString");
	s_Data.X11.XLIB.SaveContext = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSaveContext>(s_Data.X11.XLIB.Handle, "XSaveContext");
	s_Data.X11.XLIB.SelectInput = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSelectInput>(s_Data.X11.XLIB.Handle, "XSelectInput");
	s_Data.X11.XLIB.SendEvent = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSendEvent>(s_Data.X11.XLIB.Handle, "XSendEvent");
	s_Data.X11.XLIB.SetClassHint = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetClassHint>(s_Data.X11.XLIB.Handle, "XSetClassHint");
	s_Data.X11.XLIB.SetErrorHandler = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetErrorHandler>(s_Data.X11.XLIB.Handle,
	                                                                                                      "XSetErrorHandler");
	s_Data.X11.XLIB.SetICFocus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetICFocus>(s_Data.X11.XLIB.Handle, "XSetICFocus");
	s_Data.X11.XLIB.SetIMValues = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetIMValues>(s_Data.X11.XLIB.Handle, "XSetIMValues");
	s_Data.X11.XLIB.SetInputFocus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetInputFocus>(s_Data.X11.XLIB.Handle, "XSetInputFocus");
	s_Data.X11.XLIB.SetLocaleModifiers = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetLocaleModifiers>(s_Data.X11.XLIB.Handle,
	                                                                                                            "XSetLocaleModifiers");
	s_Data.X11.XLIB.SetScreenSaver = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetScreenSaver>(s_Data.X11.XLIB.Handle, "XSetScreenSaver");
	s_Data.X11.XLIB.SetSelectionOwner = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetSelectionOwner>(s_Data.X11.XLIB.Handle,
	                                                                                                          "XSetSelectionOwner");
	s_Data.X11.XLIB.SetWMHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetWMHints>(s_Data.X11.XLIB.Handle, "XSetWMHints");
	s_Data.X11.XLIB.SetWMNormalHints = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetWMNormalHints>(s_Data.X11.XLIB.Handle,
	                                                                                                        "XSetWMNormalHints");
	s_Data.X11.XLIB.SetWMProtocols = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSetWMProtocols>(s_Data.X11.XLIB.Handle, "XSetWMProtocols");
	s_Data.X11.XLIB.SupportsLocale = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSupportsLocale>(s_Data.X11.XLIB.Handle, "XSupportsLocale");
	s_Data.X11.XLIB.Sync = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XSync>(s_Data.X11.XLIB.Handle, "XSync");
	s_Data.X11.XLIB.TranslateCoordinates = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XTranslateCoordinates>(s_Data.X11.XLIB.Handle,
	                                                                                                                "XTranslateCoordinates");
	s_Data.X11.XLIB.UndefineCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUndefineCursor>(s_Data.X11.XLIB.Handle, "XUndefineCursor");
	s_Data.X11.XLIB.UngrabPointer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUngrabPointer>(s_Data.X11.XLIB.Handle, "XUngrabPointer");
	s_Data.X11.XLIB.UnmapWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUnmapWindow>(s_Data.X11.XLIB.Handle, "XUnmapWindow");
	s_Data.X11.XLIB.UnsetICFocus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUnsetICFocus>(s_Data.X11.XLIB.Handle, "XUnsetICFocus");
	s_Data.X11.XLIB.VisualIDFromVisual = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XVisualIDFromVisual>(s_Data.X11.XLIB.Handle,
	                                                                                                            "XVisualIDFromVisual");
	s_Data.X11.XLIB.WarpPointer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XWarpPointer>(s_Data.X11.XLIB.Handle, "XWarpPointer");
	s_Data.X11.XLIB.UnregisterIMInstantiateCallback = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XUnregisterIMInstantiateCallback>(s_Data.X11.XLIB.Handle,
	                       																										    	  "XUnregisterIMInstantiateCallback");
	s_Data.X11.XLIB.UTF8LookupString = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_Xutf8LookupString>(s_Data.X11.XLIB.Handle,
	                                                                                                        "Xutf8LookupString");
	s_Data.X11.XLIB.UTF8SetWMProperties = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_Xutf8SetWMProperties>(s_Data.X11.XLIB.Handle,
	                                                                                                              "Xutf8SetWMProperties");
	s_Data.X11.XLIB.CreateRegion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XCreateRegion>(s_Data.X11.XLIB.Handle, "XCreateRegion");
	s_Data.X11.XLIB.DestroyRegion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XDestroyRegion>(s_Data.X11.XLIB.Handle, "XDestroyRegion");
	s_Data.X11.XKB.AllocKeyboard = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbAllocKeyboard>(s_Data.X11.XLIB.Handle, "XkbAllocKeyboard");
	s_Data.X11.XKB.FreeKeyboard = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbFreeKeyboard>(s_Data.X11.XLIB.Handle, "XkbFreeKeyboard");
	s_Data.X11.XKB.FreeNames = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbFreeNames>(s_Data.X11.XLIB.Handle, "XkbFreeNames");
	s_Data.X11.XKB.GetMap = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbGetMap>(s_Data.X11.XLIB.Handle, "XkbGetMap");
	s_Data.X11.XKB.GetNames = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbGetNames>(s_Data.X11.XLIB.Handle, "XkbGetNames");
	s_Data.X11.XKB.GetState = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbGetState>(s_Data.X11.XLIB.Handle, "XkbGetState");
	s_Data.X11.XKB.KeycodeToKeysym = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbKeycodeToKeysym>(s_Data.X11.XLIB.Handle,
	                                                                                                       "XkbKeycodeToKeysym");
	s_Data.X11.XKB.QueryExtension = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbQueryExtension>(s_Data.X11.XLIB.Handle,
	                                                                                                     "XkbQueryExtension");
	s_Data.X11.XKB.SelectEventDetails = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbSelectEventDetails>(s_Data.X11.XLIB.Handle,
	                                                                                                             "XkbSelectEventDetails");
	s_Data.X11.XKB.SetDetectableAutoRepeat = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XkbSetDetectableAutoRepeat>(s_Data.X11.XLIB.Handle,
	 																												       "XkbSetDetectableAutoRepeat");
	s_Data.X11.XRM.DestroyDatabase = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmDestroyDatabase>(s_Data.X11.XLIB.Handle,
	                                                                                                       "XrmDestroyDatabase");
	s_Data.X11.XRM.GetResource = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmGetResource>(s_Data.X11.XLIB.Handle, "XrmGetResource");
	s_Data.X11.XRM.GetStringDatabase = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmGetStringDatabase>(s_Data.X11.XLIB.Handle,
	                                                                                                           "XrmGetStringDatabase");
	s_Data.X11.XRM.Initialize = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmInitialize>(s_Data.X11.XLIB.Handle, "XrmInitialize");
	s_Data.X11.XRM.UniqueQuark = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_XrmUniqueQuark>(s_Data.X11.XLIB.Handle, "XrmUniqueQuark");
	if ((s_Data.X11.XLIB.UTF8LookupString != nullptr) && (s_Data.X11.XLIB.UTF8SetWMProperties != nullptr))
		s_Data.X11.XLIB.UTF8 = true;

	s_Data.X11.XLIB.InitThreads();
	s_Data.X11.XRM.Initialize();

	s_Data.X11.display = s_Data.X11.XLIB.OpenDisplay(nullptr);
	if(s_Data.X11.display == nullptr)
	{
		const char* const display = getenv("DISPLAY");
		if(display != nullptr)
			InputError(Error::Platform_Error, std::string("[X11] Failed to open display: ") + display);
		else
			InputError(Error::Platform_Error, "[X11] The DISPLAY environment variable is missing!");

		return false;
	}

	s_Data.X11.Screen = DefaultScreen(s_Data.X11.display);
	s_Data.X11.Root = RootWindow(s_Data.X11.display, s_Data.X11.Screen);
	s_Data.X11.Context = static_cast<XContext>(s_Data.X11.XRM.UniqueQuark()); //Same as calling XUniqueContext();

	GetSystemContentScale(s_Data.X11.ContentScaleX, s_Data.X11.ContentScaleY);

	if(!CreateEmptyEventPipe())
		return false;

	if(!InitExtensions())
		return false;

	s_Data.X11.HelperWindowHandle = CreateHelperWindow();
	const std::optional<Cursor> hiddenCursor = CreateHiddenCursor();
	if(hiddenCursor)
		s_Data.X11.HiddenCursorHandle = *hiddenCursor;

	if((s_Data.X11.XLIB.SupportsLocale() != 0) && s_Data.X11.XLIB.UTF8)
	{
		s_Data.X11.XLIB.SetLocaleModifiers("");

		//If an IM is already present our callback will be called right away
		s_Data.X11.XLIB.RegisterIMInstantiateCallback(s_Data.X11.display, nullptr, nullptr, nullptr,
		                                              InputMethodInstantiateCallback, nullptr);
	}

	PollMonitorsX11();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyWindowX11(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (s_Data.DisabledCursorWindow == &window)
		EnableCursor(window);

	if(window.Monitor != nullptr)
		ReleaseMonitor(window);

	if(window.X11.IC != nullptr)
	{
		s_Data.X11.XLIB.DestroyIC(window.X11.IC);
		window.X11.IC = nullptr;
	}

	if(window.X11.Handle != 0u)
	{
		s_Data.X11.XLIB.DeleteContext(s_Data.X11.display, window.X11.Handle, s_Data.X11.Context);
		s_Data.X11.XLIB.UnmapWindow(s_Data.X11.display, window.X11.Handle);
		s_Data.X11.XLIB.DestroyWindow(s_Data.X11.display, window.X11.Handle);
		window.X11.Handle = 0;
	}

	if(window.X11.colormap != 0u)
	{
		s_Data.X11.XLIB.FreeColormap(s_Data.X11.display, window.X11.colormap);
		window.X11.colormap = 0;
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShutdownX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(s_Data.X11.HelperWindowHandle != 0u)
	{
		if(s_Data.X11.XLIB.GetSelectionOwner(s_Data.X11.display, s_Data.X11.CLIPBOARD) == s_Data.X11.HelperWindowHandle)
			PushSelectionToManagerX11();

		s_Data.X11.XLIB.DestroyWindow(s_Data.X11.display, s_Data.X11.HelperWindowHandle);
	}

	if(s_Data.X11.HiddenCursorHandle != 0u)
		s_Data.X11.XLIB.FreeCursor(s_Data.X11.display, s_Data.X11.HiddenCursorHandle);

	s_Data.X11.XLIB.UnregisterIMInstantiateCallback(s_Data.X11.display, nullptr, nullptr, nullptr,
	                                                InputMethodInstantiateCallback, nullptr);

	if(s_Data.X11.IM != nullptr)
		s_Data.X11.XLIB.CloseIM(s_Data.X11.IM);

	if(s_Data.X11.display != nullptr)
		s_Data.X11.XLIB.CloseDisplay(s_Data.X11.display);

	if(s_Data.X11.XCB.Handle != nullptr)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.X11.XCB.Handle);

	if(s_Data.X11.XCursor.Handle != nullptr)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.X11.XCursor.Handle);

	if(s_Data.X11.RandR.Handle != nullptr)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.X11.RandR.Handle);

	if(s_Data.X11.Xinerama.Handle != nullptr)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.X11.Xinerama.Handle);

	if(s_Data.X11.XRender.Handle != nullptr)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.X11.XRender.Handle);

	if(s_Data.X11.XI.Handle != nullptr)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.X11.XI.Handle);

	if(s_Data.X11.XLIB.Handle != nullptr)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.X11.XLIB.Handle);

	if((std::get<0>(s_Data.EmptyEventPipe) != 0) || (std::get<1>(s_Data.EmptyEventPipe) != 0))
	{
		close(std::get<0>(s_Data.EmptyEventPipe));
		close(std::get<1>(s_Data.EmptyEventPipe));
	}

	s_Data.X11 = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScaleX11([[maybe_unused]] const InternalMonitor& monitor,
                                                                      float& xScale, float& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	xScale = s_Data.X11.ContentScaleX;
	yScale = s_Data.X11.ContentScaleY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPosX11(const InternalMonitor& monitor, int32_t& xPos,
                                                            int32_t& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.X11.RandR.Available || s_Data.X11.RandR.MonitorBroken)
		return;

	XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
	if(sr != nullptr)
	{
		XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);

		if(ci != nullptr)
		{
			xPos = ci->x;
			yPos = ci->y;

			s_Data.X11.RandR.FreeCrtcInfo(ci);
		}
		s_Data.X11.RandR.FreeScreenResources(sr);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindowX11(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(PlatformWindowVisibleX11(window))
		return;

	s_Data.X11.XLIB.MapWindow(s_Data.X11.display, window.X11.Handle);
	WaitForVisibilityNotify(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(s_Data.X11.NET_ACTIVE_WINDOW != 0u)
		SendEventToWM(window, s_Data.X11.NET_ACTIVE_WINDOW, 1, 0, 0, 0, 0);
	else if (PlatformWindowVisibleX11(window))
	{
		s_Data.X11.XLIB.RaiseWindow(s_Data.X11.display, window.X11.Handle);
		s_Data.X11.XLIB.SetInputFocus(s_Data.X11.display, window.X11.Handle, RevertToParent, CurrentTime);
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowX11(InternalWindow& window,
			                                                             WindowConfig& WNDConfig)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	Visual* const visual = DefaultVisual(s_Data.X11.display, s_Data.X11.Screen);
	const int32_t depth = DefaultDepth(s_Data.X11.display, s_Data.X11.Screen);

	if(!CreateNativeWindow(window, WNDConfig, *visual, depth))
		return false;

	if(WNDConfig.MousePassthrough)
		PlatformSetWindowMousePassthroughX11(window, true);

	if(window.Monitor != nullptr)
	{
		PlatformShowWindowX11(window);
		UpdateWindowMode(window);
		if(window.BorderlessFullscreen)
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

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowTitleX11(const InternalWindow& window, const std::string& title)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (s_Data.X11.XLIB.UTF8)
	{
		s_Data.X11.XLIB.UTF8SetWMProperties(s_Data.X11.display,
			                                window.X11.Handle,
			                                title.c_str(),
			                                title.c_str(),
			                                nullptr,
			                                0,
			                                nullptr,
			                                nullptr,
			                                nullptr);
	}

	s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display,
					               window.X11.Handle,
					               s_Data.X11.NET_WM_NAME,
					               s_Data.X11.UTF8_STRING,
					               8,
					               PropModeReplace,
					               reinterpret_cast<const uint8_t*>(title.data()),
					               static_cast<int32_t>(title.size()));

	s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display,
	                               window.X11.Handle,
					               s_Data.X11.NET_WM_ICON_NAME,
					               s_Data.X11.UTF8_STRING,
					               8,
					               PropModeReplace,
					               reinterpret_cast<const uint8_t*>(title.data()),
					               static_cast<int32_t>(title.size()));

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursorX11(InternalCursor& cursor,
                                                                         const Image& image,
																		 const int32_t xHotspot,
																		 const int32_t yHotspot)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const std::optional<Cursor> cursorHandle = CreateCursorX11(image, xHotspot, yHotspot);

	if(cursorHandle)
		cursor.X11.Handle = *cursorHandle;

	return cursor.X11.Handle != 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursorX11(InternalCursor& cursor, const CursorType& type)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(s_Data.X11.XCursor.Handle != nullptr)
	{
		const char* const theme = s_Data.X11.XCursor.GetTheme(s_Data.X11.display);
		if(theme != nullptr)
		{
			const int32_t size = s_Data.X11.XCursor.GetDefaultSize(s_Data.X11.display);
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

			XcursorImage* const image = s_Data.X11.XCursor.LibraryLoadImage(name.c_str(), theme, size);
			if(image != nullptr)
			{
				cursor.X11.Handle = s_Data.X11.XCursor.ImageLoadCursor(s_Data.X11.display, image);
				s_Data.X11.XCursor.ImageDestroy(image);
			}
		}
	}

	if(cursor.X11.Handle == 0u)
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

		cursor.X11.Handle = s_Data.X11.XLIB.CreateFontCursor(s_Data.X11.display, native);
		if(cursor.X11.Handle == 0u)
		{
			InputError(Error::Platform_Error, "[X11] Failed to create standard cursor!");
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursorX11(InternalCursor& cursor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(cursor.X11.Handle != 0u)
		s_Data.X11.XLIB.FreeCursor(s_Data.X11.display, cursor.X11.Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorX11(InternalWindow& window,
                                                        [[maybe_unused]] InternalCursor* const cursor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.cursorMode != CursorMode::Normal && window.cursorMode != CursorMode::Captured)
		return;

	UpdateCursorImage(window);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorModeX11(InternalWindow& window, const CursorMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (PlatformWindowFocusedX11(window))
	{
		if (mode == CursorMode::Disabled)
		{
			PlatformGetCursorPosX11(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
			CenterCursorInContentArea(window);
			if (window.RawMouseMotion)
				EnableRawMouseMotion(window);
		}
		else if (s_Data.DisabledCursorWindow == &window)
		{
			if (window.RawMouseMotion)
				DisableRawMouseMotion(window);
		}

		if (mode == CursorMode::Disabled || mode == CursorMode::Captured)
			CaptureCursor(window);
		else
			ReleaseCursor();

		if (mode == CursorMode::Disabled)
			s_Data.DisabledCursorWindow = &window;
		else if (s_Data.DisabledCursorWindow == &window)
		{
			s_Data.DisabledCursorWindow = nullptr;
			PlatformSetCursorPosX11(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
		}
	}

	UpdateCursorImage(window);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPosX11(InternalWindow& window, const double xPos, const double yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	//Store the new position so it can be recognized later
	window.X11.WarpCursorPosX = static_cast<int32_t>(xPos);
	window.X11.WarpCursorPosY = static_cast<int32_t>(yPos);

	s_Data.X11.XLIB.WarpPointer(s_Data.X11.display, None, window.X11.Handle, 0, 0, 0, 0, static_cast<int32_t>(xPos),
	                            static_cast<int32_t>(yPos));
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPosX11(const InternalWindow& window, double& xPos, double& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	::Window root = 0, child = 0;
	int32_t rootX = 0, rootY = 0, childX = 0, childY = 0;
	uint32_t mask = 0;

	s_Data.X11.XLIB.QueryPointer(s_Data.X11.display, window.X11.Handle, &root, &child, &rootX, &rootY, &childX, &childY,
	                             &mask);

	xPos = childX;
	yPos = childY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIconX11(InternalWindow& window, const Image* const image)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(image != nullptr)
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

		for(uint64_t j = 0; j < static_cast<uint64_t>(image->GetWidth()) * static_cast<uint64_t>(image->GetHeight()); j++)
		{
			*target++ = (static_cast<uint64_t>(imgData[j * 4 + 0]) << 16u) |
						(static_cast<uint64_t>(imgData[j * 4 + 1]) <<  8u) |
						(static_cast<uint64_t>(imgData[j * 4 + 2]) <<  0u) |
						(static_cast<uint64_t>(imgData[j * 4 + 3]) << 24u);
		}

		//NOTE: XChangeProperty expects 32-bit values like the image data above to be
		//      places in the 32 least significant bits of individual longs. This is true
		//      even if long is 64-bit and a WM protocol calls for "packed" data.
		//      This is because of a historical mistake that then became part of the Xlib ABI.
		//      Xlib will pack these values into a regular array of 32-bit values before
		//      sending it over the wire.
		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_ICON, XA_CARDINAL, 32,
		                               PropModeReplace,	reinterpret_cast<uint8_t*>(icon.data()),
								       static_cast<int32_t>(longCount));
	}
	else
		s_Data.X11.XLIB.DeleteProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_ICON);

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPosX11(const InternalWindow& window, int32_t& xPos, int32_t& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	::Window dummy = 0;
	int32_t x = 0, y = 0;

	s_Data.X11.XLIB.TranslateCoordinates(s_Data.X11.display, window.X11.Handle, s_Data.X11.Root, 0, 0, &x, &y, &dummy);

	xPos = x;
	yPos = y;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeX11(InternalWindow& window, const int32_t width, const int32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if((window.Monitor != nullptr) && window.Monitor->Window == &window)
	{
		if(window.Monitor->Window->BorderlessFullscreen)
			AcquireMonitorBorderless(window);
		else
			AcquireMonitor(window);
	}
	else
	{
		if(!window.Resizable)
			UpdateNormalHints(window, width, height);

		s_Data.X11.XLIB.ResizeWindow(s_Data.X11.display, window.X11.Handle, width, height);
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizableX11(InternalWindow& window,
                                                                 [[maybe_unused]] const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t width = 0, height = 0;

	PlatformGetWindowSizeX11(window, width, height);
	UpdateNormalHints(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecoratedX11(InternalWindow& window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

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

	s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.MOTIF_WM_HINTS, s_Data.X11.MOTIF_WM_HINTS, 32,
	                               PropModeReplace, reinterpret_cast<uint8_t*>(&hints), sizeof(hints) / sizeof(int64_t));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloatingX11(const InternalWindow& window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if((s_Data.X11.NET_WM_STATE == 0u) || (s_Data.X11.NET_WM_STATE_ABOVE == 0u))
		return;

	if(PlatformWindowVisibleX11(window))
	{
		const int64_t action = static_cast<int64_t>(enabled ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE);
		SendEventToWM(window, s_Data.X11.NET_WM_STATE, action, static_cast<int64_t>(s_Data.X11.NET_WM_STATE_ABOVE), 0, 1, 0);
	}
	else
	{
		Atom* states = nullptr;

		const uint64_t count = GetWindowPropertyX11(window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM,
		                                                 reinterpret_cast<uint8_t**>(&states));

		//NOTE: We don't check for failure as this property may not exist yet and that's fine (and we'll create
		//      it implicitly with append)
		if(enabled)
		{
			uint64_t i = 0;

			for(i = 0; i < count; i++)
			{
				if(states[i] == s_Data.X11.NET_WM_STATE_ABOVE)
					break;
			}

			if(i == count)
			{
				s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM, 32,
										       PropModeAppend, reinterpret_cast<uint8_t*>(&s_Data.X11.NET_WM_STATE_ABOVE), 1);
			}
		}
		else if(states != nullptr)
		{
			for(uint64_t i = 0; i < count; ++i)
			{
				if(states[i] == s_Data.X11.NET_WM_STATE_ABOVE)
				{
					states[i] = states[count - 1];
					s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM, 32,
											       PropModeReplace, reinterpret_cast<uint8_t*>(states),
											       static_cast<int32_t>(count) - 1);
					break;
				}
			}
		}

		if(states != nullptr)
			s_Data.X11.XLIB.Free(states);
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacityX11(const InternalWindow& window, const float opacity)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const CARD32 value = static_cast<CARD32>(0xFFFFFFFFu * static_cast<double>(opacity));
	s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_WINDOW_OPACITY, XA_CARDINAL, 32,
	                               PropModeReplace, reinterpret_cast<const uint8_t*>(&value), 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthroughX11(InternalWindow& window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.X11.XShape.Available)
		return;

	if (enabled)
	{
		const Region region = s_Data.X11.XLIB.CreateRegion();
		s_Data.X11.XShape.CombineRegion(s_Data.X11.display, window.X11.Handle, ShapeInput, 0, 0, region, ShapeSet);
		s_Data.X11.XLIB.DestroyRegion(region);
	}
	else
		s_Data.X11.XShape.CombineMask(s_Data.X11.display, window.X11.Handle, ShapeInput, 0, 0, None, ShapeSet);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbarX11([[maybe_unused]] InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    InputError(Error::Feature_Unavailable, "[X11] Platform does not support hiding windows from the taskbar");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<float> TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacityX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	float opacity = -1.0f;

	if(s_Data.X11.XLIB.GetSelectionOwner(s_Data.X11.display, s_Data.X11.NET_WM_CM_Sx) != 0u)
	{
		CARD32* value = nullptr;

		if(GetWindowPropertyX11(window.X11.Handle, s_Data.X11.NET_WM_WINDOW_OPACITY, XA_CARDINAL,
		                        reinterpret_cast<uint8_t**>(&value)) != 0u)
		{
			opacity = static_cast<float>(*value / static_cast<double>(0xFFFFFFFFu));
		}

		if(value != nullptr)
			s_Data.X11.XLIB.Free(value);
	}

	if(opacity != -1.0f)
		return opacity;

	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSizeX11(const InternalWindow& window, int32_t& width,
                                                                 int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	PlatformGetWindowSizeX11(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScaleX11([[maybe_unused]] const InternalWindow& window,
                                                                    float& xScale, float& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	xScale = s_Data.X11.ContentScaleX;
	yScale = s_Data.X11.ContentScaleY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkAreaX11(const InternalMonitor& monitor, int32_t& xPos,
                                                                 int32_t& yPos, int32_t& width, int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t areaX = 0, areaY = 0, areaWidth = 0, areaHeight = 0;

	if(s_Data.X11.RandR.Available && !s_Data.X11.RandR.MonitorBroken)
	{
		XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
		XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);

		if((sr != nullptr) && (ci != nullptr))
		{
			areaX = ci->x;
			areaY = ci->y;

			const XRRModeInfo* const mi = GetModeInfo(*sr, ci->mode);

			areaWidth = static_cast<int32_t>(mi->width);
			areaHeight = static_cast<int32_t>(mi->height);

			if(ci->rotation == RR_Rotate_90 || ci->rotation == RR_Rotate_270)
				std::swap(areaWidth, areaHeight);
		}
		else
		{
			areaWidth = DisplayWidth(s_Data.X11.display, s_Data.X11.Screen);
			areaHeight = DisplayHeight(s_Data.X11.display, s_Data.X11.Screen);
		}

		if(ci != nullptr)
			s_Data.X11.RandR.FreeCrtcInfo(ci);
		if(sr != nullptr)
			s_Data.X11.RandR.FreeScreenResources(sr);
	}
	else
	{
		areaWidth = DisplayWidth(s_Data.X11.display, s_Data.X11.Screen);
		areaHeight = DisplayHeight(s_Data.X11.display, s_Data.X11.Screen);
	}

	if((s_Data.X11.NET_WORKAREA != 0u) && (s_Data.X11.NET_CURRENT_DESKTOP != 0u))
	{
		Atom* extents = nullptr;
		Atom* desktop = nullptr;
		const uint64_t extentCount = GetWindowPropertyX11(s_Data.X11.Root, s_Data.X11.NET_WORKAREA, XA_CARDINAL,
		                                                  reinterpret_cast<uint8_t**>(&extents));

		if(GetWindowPropertyX11(s_Data.X11.Root, s_Data.X11.NET_CURRENT_DESKTOP, XA_CARDINAL,
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

		if(extents != nullptr)
			s_Data.X11.XLIB.Free(extents);
		if(desktop != nullptr)
			s_Data.X11.XLIB.Free(desktop);
	}

	xPos = areaX;
	yPos = areaY;
	width = areaWidth;
	height = areaHeight;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowVisibleX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	XWindowAttributes wa;
	s_Data.X11.XLIB.GetWindowAttributes(s_Data.X11.display, window.X11.Handle, &wa);

	return wa.map_state == IsViewable;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximizedX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	Atom* states = nullptr;
	bool maximized = false;

	if((s_Data.X11.NET_WM_STATE == 0u) || (s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT == 0u) ||
	   (s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ == 0u))
		return maximized;

	const uint64_t count = GetWindowPropertyX11(window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM,
	                                            reinterpret_cast<uint8_t**>(&states));

	for(uint64_t i = 0; i < count; i++)
	{
		if(states[i] == s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT || states[i] == s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ)
		{
			maximized = true;
			break;
		}
	}

	if(states != nullptr)
		s_Data.X11.XLIB.Free(states);

	return maximized;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMinimizedX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	return GetWindowState(window) == IconicState;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPollEventsX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	DrainEmptyEvents();

	Input::DetectControllerConnectionLinux();

	s_Data.X11.XLIB.Pending(s_Data.X11.display);

	while(QLength(s_Data.X11.display))
	{
		XEvent event;
		s_Data.X11.XLIB.NextEvent(s_Data.X11.display, &event);
		ProcessEvent(event);
	}

	InternalWindow* const window = s_Data.DisabledCursorWindow;
	if(window != nullptr)
	{
		int32_t width = 0, height = 0;
		PlatformGetWindowSizeX11(*window, width, height);

		if(window->LastCursorPosX != width / 2 || window->LastCursorPosY != height / 2)
			PlatformSetCursorPosX11(*window, static_cast<double>(width) / 2, static_cast<double>(height) / 2);
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformWaitEventsX11(double timeout)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(timeout == 0.0)
		WaitForAnyEvent(nullptr);
	else
		WaitForAnyEvent(&timeout);

	PlatformPollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPostEmptyEventX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	WriteEmptyEvent();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocusedX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	::Window focused = 0;
	int32_t state = 0;

	s_Data.X11.XLIB.GetInputFocus(s_Data.X11.display, &focused, &state);

	return window.X11.Handle == focused;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHoveredX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	::Window w = s_Data.X11.Root;
	while(w != 0u)
	{
		::Window root = 0;
		int32_t rootX = 0, rootY = 0, childX = 0, childY = 0;
		uint32_t mask = 0;

		GrabErrorHandlerX11();

		const int32_t result = s_Data.X11.XLIB.QueryPointer(s_Data.X11.display, w, &root, &w, &rootX, &rootY, &childX,
		                                                    &childY, &mask);

		ReleaseErrorHandlerX11();

		if (s_Data.X11.ErrorCode == BadWindow)
			w = s_Data.X11.Root;
		else if(result == 0)
			return false;
		else if(w == window.X11.Handle)
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformRawMouseMotionSupportedX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	return s_Data.X11.XI.Available;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotionX11(const InternalWindow& window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.X11.XI.Available)
		return;

	if(s_Data.DisabledCursorWindow != &window)
		return;

	if(enabled)
		EnableRawMouseMotion(window);
	else
		DisableRawMouseMotion(window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] int32_t TRAP::INTERNAL::WindowingAPI::PlatformGetKeyScanCodeX11(const Input::Key key)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Data.ScanCodes[static_cast<uint32_t>(key)];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const char* TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeNameX11(const int32_t scanCode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.X11.XKB.Available)
		return nullptr;

	if(scanCode < 0 || scanCode > 0xFF || s_Data.KeyCodes[scanCode] == Input::Key::Unknown)
	{
		InputError(Error::Invalid_Value, "Invalid scancode" + std::to_string(scanCode));
		return nullptr;
	}

	const int32_t key = static_cast<int32_t>(s_Data.KeyCodes[scanCode]);
	const KeySym keySym = s_Data.X11.XKB.KeycodeToKeysym(s_Data.X11.display, static_cast<KeyCode>(scanCode),
											             static_cast<int32_t>(s_Data.X11.XKB.Group), 0);
	if(keySym == NoSymbol)
		return nullptr;

	const uint32_t ch = KeySymToUnicode(static_cast<uint32_t>(keySym));
	if(ch == 0xFFFFFFFFu)
		return nullptr;

	const std::string utf8Str = Utils::String::EncodeUTF8(ch);
	if(utf8Str.empty())
		return nullptr;
	for(std::size_t i = 0; i < utf8Str.size(); ++i)
		s_Data.KeyNames[key][i] = utf8Str[i];

	s_Data.KeyNames[key][utf8Str.size()] = '\0';

	return s_Data.KeyNames[key].data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetClipboardStringX11(const std::string& string)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	s_Data.ClipboardString = string;

	s_Data.X11.XLIB.SetSelectionOwner(s_Data.X11.display, s_Data.X11.CLIPBOARD, s_Data.X11.HelperWindowHandle, CurrentTime);

	if(s_Data.X11.XLIB.GetSelectionOwner(s_Data.X11.display, s_Data.X11.CLIPBOARD) != s_Data.X11.HelperWindowHandle)
		InputError(Error::Platform_Error, "[X11] Failed to become owner of clipboard selection!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::PlatformGetClipboardStringX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	return GetSelectionString(s_Data.X11.CLIPBOARD);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetRequiredInstanceExtensionsX11(std::array<std::string, 2>& extensions)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(!s_Data.VK.KHR_Surface)
		return;

	if(!s_Data.VK.KHR_XCB_Surface || (s_Data.X11.XCB.Handle == nullptr))
	{
		if(!s_Data.VK.KHR_XLib_Surface)
			return;
	}

	std::get<0>(extensions) = "VK_KHR_surface";

	//NOTE: VK_KHR_xcb_surface is preferred due to some early ICDs exposing but not correctly implementing
	//      VK_KHR_xlib_surface
	if(s_Data.VK.KHR_XCB_Surface && (s_Data.X11.XCB.Handle != nullptr))
		std::get<1>(extensions) = "VK_KHR_xcb_surface";
	else
		std::get<1>(extensions) = "VK_KHR_xlib_surface";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkResult TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowSurfaceX11(VkInstance instance,
                                                                                    const InternalWindow& window,
																                    const VkAllocationCallbacks* const allocator,
																                    VkSurfaceKHR& surface)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(instance != VK_NULL_HANDLE, "WindowingAPI::PlatformCreateWindowSurfaceX11(): instance is VK_NULL_HANDLE!");

	if(s_Data.VK.KHR_XCB_Surface && (s_Data.X11.XCB.Handle != nullptr))
	{
		xcb_connection_t* const connection = s_Data.X11.XCB.GetXCBConnection(s_Data.X11.display);
		if(connection == nullptr)
		{
			InputError(Error::Platform_Error, "[X11] Failed to retrieve XCB connection!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		const PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(vkGetInstanceProcAddr
			(
				instance, "vkCreateXcbSurfaceKHR"
			));
		if(vkCreateXcbSurfaceKHR == nullptr)
		{
			InputError(Error::API_Unavailable, "[X11] Vulkan instance missing VK_KHR_xcb_surface extension!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		VkXcbSurfaceCreateInfoKHR sci{};
		sci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		sci.connection = connection;
		sci.window = window.X11.Handle;

		const VkResult err = vkCreateXcbSurfaceKHR(instance, &sci, allocator, &surface);
		if(err != 0)
			InputError(Error::Platform_Error,
			           std::string("[X11] Failed to create Vulkan XCB surface: ") + GetVulkanResultString(err));

		return err;
	}

	const PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(vkGetInstanceProcAddr
		(
			instance, "vkCreateXlibSurfaceKHR"
		));
	if(vkCreateXlibSurfaceKHR == nullptr)
	{
		InputError(Error::API_Unavailable, "[X11] Vulkan instance missing VK_KHR_xlib_surface extension");
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	VkXlibSurfaceCreateInfoKHR sci{};
	sci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	sci.dpy = s_Data.X11.display;
	sci.window = window.X11.Handle;

	const VkResult err = vkCreateXlibSurfaceKHR(instance, &sci, allocator, &surface);
	if(err != 0)
	{
		InputError(Error::Platform_Error,
					std::string("[X11] Failed to create Vulkan X11 surface: ") + GetVulkanResultString(err));
	}

	return err;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if((s_Data.X11.NET_WM_STATE == 0u) || (s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT == 0u) ||
	   (s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ == 0u))
	   return;

	if(PlatformWindowVisibleX11(window))
	{
		SendEventToWM(window, s_Data.X11.NET_WM_STATE, _NET_WM_STATE_ADD,
		              static_cast<int64_t>(s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT),
		              static_cast<int64_t>(s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ), 1, 0);
	}
	else
	{
		Atom* states = nullptr;
		const uint64_t count = GetWindowPropertyX11(window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM,
		                                            reinterpret_cast<uint8_t**>(&states));

		//NOTE: We don't check for failure as this property may not exist yet and that's fine (and we'll create it
		//      implicitly with append)
		std::array<Atom, 2> missing =
		{
			s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT,
			s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ
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

		if(states != nullptr)
			s_Data.X11.XLIB.Free(states);

		if(missingCount == 0u)
			return;

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM, 32, PropModeAppend,
		                               reinterpret_cast<uint8_t*>(missing.data()), static_cast<int32_t>(missingCount));
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.X11.OverrideRedirect)
	{
		//Override-redirect windows cannot be minimized or restored, as those tasks are performed by the
		//window manager
		InputError(Error::Platform_Error,
		           "[X11] Minimization of full screen windows requires a WM that supports EWMH full screen");
		return;
	}

	s_Data.X11.XLIB.IconifyWindow(s_Data.X11.display, window.X11.Handle, s_Data.X11.Screen);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttentionX11(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if((s_Data.X11.NET_WM_STATE == 0u) || (s_Data.X11.NET_WM_STATE_DEMANDS_ATTENTION == 0u))
		return;

	SendEventToWM(window, s_Data.X11.NET_WM_STATE, _NET_WM_STATE_ADD,
	              static_cast<int64_t>(s_Data.X11.NET_WM_STATE_DEMANDS_ATTENTION), 0, 1, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowX11(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	s_Data.X11.XLIB.UnmapWindow(s_Data.X11.display, window.X11.Handle);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindowX11(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.X11.OverrideRedirect)
	{
		//Override-redirect windows cannot be minimized or restored, as those tasks are performed by
		//the window manager
		InputError(Error::Platform_Error,
		           "[X11] Maximization of full screen windows requires a WM that supports EWMH full screen");
		return;
	}

	if(PlatformWindowMinimizedX11(window))
	{
		s_Data.X11.XLIB.MapWindow(s_Data.X11.display, window.X11.Handle);
		WaitForVisibilityNotify(window);
	}
	else if(PlatformWindowVisibleX11(window))
	{
		if((s_Data.X11.NET_WM_STATE != 0u) && (s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT != 0u) &&
		   (s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ != 0u))
		{
			SendEventToWM(window, s_Data.X11.NET_WM_STATE, _NET_WM_STATE_REMOVE,
			              static_cast<int64_t>(s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT),
			              static_cast<int64_t>(s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ), 1, 0);
		}
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimitsX11(InternalWindow& window,
                                                                  [[maybe_unused]] const int32_t minWidth,
                                                                  [[maybe_unused]] const int32_t minHeight,
															      [[maybe_unused]] const int32_t maxWidth,
                                                                  [[maybe_unused]] const int32_t maxHeight)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t width = 0, height = 0;
	PlatformGetWindowSizeX11(window, width, height);
	UpdateNormalHints(window, width, height);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowAspectRatioX11(InternalWindow& window,
                                                                   [[maybe_unused]] const int32_t numerator,
                                                                   [[maybe_unused]] const int32_t denominator)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t width = 0, height = 0;
	PlatformGetWindowSizeX11(window, width, height);
	UpdateNormalHints(window, width, height);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

//Enable XI2 raw mouse motion events
void TRAP::INTERNAL::WindowingAPI::EnableRawMouseMotion([[maybe_unused]] const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::array<uint8_t, XIMaskLen(XI_RawMotion)> mask{};

	XIEventMask em{};
	em.deviceid = XIAllMasterDevices;
	em.mask_len = mask.size();
	em.mask = mask.data();
	XISetMask(mask.data(), XI_RawMotion);

	s_Data.X11.XI.SelectEvents(s_Data.X11.display, s_Data.X11.Root, &em, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Disable XI2 raw mouse motion events
void TRAP::INTERNAL::WindowingAPI::DisableRawMouseMotion([[maybe_unused]] const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::array<uint8_t, 1> mask{};

	XIEventMask em{};
	em.deviceid = XIAllMasterDevices;
	em.mask_len = mask.size();
	em.mask = mask.data();

	s_Data.X11.XI.SelectEvents(s_Data.X11.display, s_Data.X11.Root, &em, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Reports the specified error, appending information about the last X Error
void TRAP::INTERNAL::WindowingAPI::InputErrorX11(const Error error, const std::string& message)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::vector<char> buffer(1024, '\0');
	s_Data.X11.XLIB.GetErrorText(s_Data.X11.display, s_Data.X11.ErrorCode, buffer.data(), static_cast<int32_t>(buffer.size()));

	buffer.erase(std::find(buffer.begin(), buffer.end(), '\0'), buffer.end());

	InputError(error, "[X11] " + message + ": " + buffer.data());
}

//-------------------------------------------------------------------------------------------------------------------//

//Process the specified X event
void TRAP::INTERNAL::WindowingAPI::ProcessEvent(XEvent& event)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t keyCode = 0;
	bool filtered = false;

	//HACK: Save scanCode as some IMs clear the field in s_Data.XLIB.FilterEvent
	if(event.type == KeyPress || event.type == KeyRelease)
		keyCode = static_cast<int32_t>(event.xkey.keycode);

	filtered = (s_Data.X11.XLIB.FilterEvent(&event, 0) != 0);

	if(s_Data.X11.RandR.Available)
	{
		if(event.type == s_Data.X11.RandR.EventBase + RRNotify)
		{
			s_Data.X11.RandR.UpdateConfiguration(&event);
			PollMonitorsX11();
			return;
		}
	}

	if(s_Data.X11.XKB.Available)
	{
		if(event.type == s_Data.X11.XKB.EventBase + XkbEventCode)
		{
			if(reinterpret_cast<XkbEvent*>(&event)->any.xkb_type == XkbStateNotify &&
			   (reinterpret_cast<XkbEvent*>(&event)->state.changed & XkbGroupStateMask))
			{
				s_Data.X11.XKB.Group = reinterpret_cast<XkbEvent*>(&event)->state.group;
				InputKeyboardLayout();
			}

			return;
		}
	}

	if(event.type == GenericEvent)
	{
		if(s_Data.X11.XI.Available)
		{
			InternalWindow* const window = s_Data.DisabledCursorWindow;

			if((window != nullptr) && window->RawMouseMotion && event.xcookie.extension == s_Data.X11.XI.MajorOPCode &&
			   (s_Data.X11.XLIB.GetEventData(s_Data.X11.display, &event.xcookie) != 0) && event.xcookie.evtype == XI_RawMotion)
			{
				const XIRawEvent* const re = reinterpret_cast<XIRawEvent*>(event.xcookie.data);
				if(re->valuators.mask_len != 0)
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

					InputCursorPos(*window, xPos, yPos);
				}
			}

			s_Data.X11.XLIB.FreeEventData(s_Data.X11.display, &event.xcookie);
		}

		return;
	}

	if(event.type == SelectionRequest)
	{
		HandleSelectionRequest(event);
		return;
	}

	InternalWindow* window = nullptr;
	if(s_Data.X11.XLIB.FindContext(s_Data.X11.display, event.xany.window, s_Data.X11.Context,
	                               reinterpret_cast<XPointer*>(&window)) != 0 || (window == nullptr))
		//This is an event for a window that has already been destroyed
		return;

	switch(event.type)
	{
	case ReparentNotify:
	{
		window->X11.Parent = event.xreparent.parent;
		return;
	}

	case KeyPress:
	{
		const Input::Key key = TranslateKey(keyCode);

		if(window->X11.IC != nullptr)
		{
			//HACK: Do not report the key press events duplicated by XIM
			//      Duplicate key releases are filtered out implicitly by
			//      the TRAP key repeat loagic in InputKey
			//      A timestamp per key is used to handle simultaneous keys
			//NOTE: Always allow the first event for each key through
			//      (the server never sends a timestamp of zero)
			//NOTE: Timestamp difference is compared to handle wrap-around
			const Time diff = event.xkey.time - window->X11.KeyPressTimes[keyCode];
			if(diff == event.xkey.time || (diff > 0 && diff < (static_cast<Time>(1u) << 31u)))
			{
				if(keyCode != 0)
					InputKey(*window, key, keyCode, Input::KeyState::Pressed);

				window->X11.KeyPressTimes[keyCode] = event.xkey.time;
			}

			if(!filtered)
			{
				int32_t count = 0;
				int32_t status = 0;

				std::string buffer(100, '\0');
				char* chars = buffer.data();

				count = s_Data.X11.XLIB.UTF8LookupString(window->X11.IC, &event.xkey, buffer.data(),
				                                         static_cast<int32_t>(buffer.size()) - 1, nullptr, &status);

				if(status == XBufferOverflow)
				{
					buffer.resize(count + 1);
					chars = buffer.data();
					count = s_Data.X11.XLIB.UTF8LookupString(window->X11.IC, &event.xkey, chars, count, nullptr, &status);
				}

				if(status == XLookupChars || status == XLookupBoth)
				{
					const char* c = chars;
					while(c - chars < count)
						InputChar(*window, DecodeUTF8(&c));
				}
			}
		}
		else
		{
			KeySym keySym = 0;
			s_Data.X11.XLIB.LookupString(&event.xkey, nullptr, 0, &keySym, nullptr);

			InputKey(*window, static_cast<Input::Key>(key), keyCode, Input::KeyState::Pressed);

			const uint32_t character = KeySymToUnicode(static_cast<uint32_t>(keySym));
			if(character != InvalidCodepoint)
				InputChar(*window, character);
		}

		return;
	}

	case KeyRelease:
	{
		const Input::Key key = TranslateKey(keyCode);

		if(!s_Data.X11.XKB.Detectable)
		{
			//HACK: Key repeat events will arrive as KeyRelease/KeyPress pairs with similar or identical time stamps
			//      The key repeat Logic in InputKey expects only key presses to repeat, so detect and discard release events
			if(s_Data.X11.XLIB.EventsQueued(s_Data.X11.display, QueuedAfterReading) != 0)
			{
				XEvent next;
				s_Data.X11.XLIB.PeekEvent(s_Data.X11.display, &next);

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

		InputKey(*window, key, keyCode, Input::KeyState::Released);
		return;
	}

	case ButtonPress:
	{
		if(event.xbutton.button == Button1)
			InputMouseClick(*window, Input::MouseButton::Left, Input::KeyState::Pressed);
		else if(event.xbutton.button == Button2)
			InputMouseClick(*window, Input::MouseButton::Middle, Input::KeyState::Pressed);
		else if(event.xbutton.button == Button3)
			InputMouseClick(*window, Input::MouseButton::Right, Input::KeyState::Pressed);

		//Modern X provides scroll events as mouse button presses
		else if(event.xbutton.button == Button4)
			InputScroll(*window, 0.0, 1.0);
		else if(event.xbutton.button == Button5)
			InputScroll(*window, 0.0, -1.0);
		else if(event.xbutton.button == Button6)
			InputScroll(*window, 1.0, 0.0);
		else if(event.xbutton.button == Button7)
			InputScroll(*window, -1.0, 0.0);
		else
		{
			//Additional buttons after 7 are treated as regular buttons
			//We subtract 4 to fill the gap left by scroll input above
			InputMouseClick(*window, static_cast<Input::MouseButton>(event.xbutton.button - Button1 - 4), Input::KeyState::Pressed);
		}

		return;
	}

	case ButtonRelease:
	{
		if(event.xbutton.button == Button1)
			InputMouseClick(*window, Input::MouseButton::Left, Input::KeyState::Released);
		else if(event.xbutton.button == Button2)
			InputMouseClick(*window, Input::MouseButton::Middle, Input::KeyState::Released);
		else if(event.xbutton.button == Button3)
			InputMouseClick(*window, Input::MouseButton::Right, Input::KeyState::Released);
		else if(event.xbutton.button > Button7)
		{
			//Additional buttons after 7 are treated as regular buttons
			//We subtract 4 to fill the gap left by scroll input above
			InputMouseClick(*window, static_cast<Input::MouseButton>(event.xbutton.button - Button1 - 4), Input::KeyState::Released);
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
			UpdateCursorImage(*window);

		InputCursorEnter(*window, true);
		InputCursorPos(*window, x, y);

		window->LastCursorPosX = x;
		window->LastCursorPosY = y;
		return;
	}

	case LeaveNotify:
	{
		InputCursorEnter(*window, false);
		return;
	}

	case MotionNotify:
	{
		const int32_t x = event.xmotion.x;
		const int32_t y = event.xmotion.y;

		if(x != window->X11.WarpCursorPosX || y != window->X11.WarpCursorPosY)
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

				InputCursorPos(*window, window->VirtualCursorPosX + dx, window->VirtualCursorPosY + dy);
			}
			else
				InputCursorPos(*window, x, y);
		}

		window->LastCursorPosX = x;
		window->LastCursorPosY = y;
		return;
	}

	case ConfigureNotify:
	{
		if(event.xconfigure.width != window->Width || event.xconfigure.height != window->Height)
		{
			InputFrameBufferSize(*window, event.xconfigure.width, event.xconfigure.height);

			InputWindowSize(*window, event.xconfigure.width, event.xconfigure.height);

			window->Width = event.xconfigure.width;
			window->Height = event.xconfigure.height;
		}

		int32_t xPos = event.xconfigure.x;
		int32_t yPos = event.xconfigure.y;

		//NOTE: ConfigureNotify events from the server are in local
		//      coordinates, so if any reparented we need to translate
		//      the position into root (screen) coordinates
		if((event.xany.send_event == 0) && window->X11.Parent != s_Data.X11.Root)
		{
			GrabErrorHandlerX11();

			::Window dummy = 0;
			s_Data.X11.XLIB.TranslateCoordinates(s_Data.X11.display, window->X11.Parent, s_Data.X11.Root, xPos, yPos, &xPos, &yPos, &dummy);

			ReleaseErrorHandlerX11();
			if(s_Data.X11.ErrorCode == BadWindow)
				return;
		}

		if(xPos != window->X11.XPos || yPos != window->X11.YPos)
		{
			InputWindowPos(*window, xPos, yPos);
			window->X11.XPos = xPos;
			window->X11.YPos = yPos;
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

		if(event.xclient.message_type == s_Data.X11.WM_PROTOCOLS)
		{
			const Atom protocol = event.xclient.data.l[0];
			if(protocol == None)
				return;

			if(protocol == s_Data.X11.WM_DELETE_WINDOW)
			{
				//The window manager was asked to close the window, for example by the user pressing a 'close'
				//window decoration button
				InputWindowCloseRequest(*window);
			}
			else if(protocol == s_Data.X11.NET_WM_PING)
			{
				//The window manager is pinging the application to ensure it's still responding to events
				XEvent reply = event;
				reply.xclient.window = s_Data.X11.Root;

				s_Data.X11.XLIB.SendEvent(s_Data.X11.display, s_Data.X11.Root, 0,
				                          SubstructureNotifyMask | SubstructureRedirectMask, &reply);
			}
		}
		else if(event.xclient.message_type == s_Data.X11.XDNDEnter)
		{
			//A drag operation has entered the window
			uint64_t count = 0;
			Atom* formats = nullptr;
			const bool list = (event.xclient.data.l[1] & 1) != 0;

			s_Data.X11.XDND.Source = event.xclient.data.l[0];
			s_Data.X11.XDND.Version = static_cast<int32_t>(event.xclient.data.l[1] >> 24u);
			s_Data.X11.XDND.Format = None;

			if(s_Data.X11.XDND.Version > TRAP_XDND_VERSION)
				return;

			if(list)
			{
				count = GetWindowPropertyX11(s_Data.X11.XDND.Source, s_Data.X11.XDNDTypeList, XA_ATOM,
				                             reinterpret_cast<uint8_t**>(&formats));
			}
			else
			{
				count = 3;
				formats = reinterpret_cast<Atom*>(event.xclient.data.l) + 2;
			}

			for(uint64_t i = 0; i < count; i++)
			{
				if(formats[i] == s_Data.X11.text_uri_list)
				{
					s_Data.X11.XDND.Format = s_Data.X11.text_uri_list;
					break;
				}
			}

			if(list && (formats != nullptr))
				s_Data.X11.XLIB.Free(formats);
		}
		else if(event.xclient.message_type == s_Data.X11.XDNDDrop)
		{
			//The drag operation has finished by dropping on the window
			Time time = CurrentTime;

			if(s_Data.X11.XDND.Version > TRAP_XDND_VERSION)
				return;

			if(s_Data.X11.XDND.Format != 0u)
			{
				if(s_Data.X11.XDND.Version >= 1)
					time = event.xclient.data.l[2];

				//Request the chosen format from the source window
				s_Data.X11.XLIB.ConvertSelection(s_Data.X11.display, s_Data.X11.XDNDSelection, s_Data.X11.XDND.Format,
				                                 s_Data.X11.XDNDSelection, window->X11.Handle, time);
			}
			else if(s_Data.X11.XDND.Version >= 2)
			{
				XEvent reply = {ClientMessage};
				reply.xclient.window = s_Data.X11.XDND.Source;
				reply.xclient.message_type = s_Data.X11.XDNDFinished;
				reply.xclient.format = 32;
				reply.xclient.data.l[0] = static_cast<int64_t>(window->X11.Handle);
				reply.xclient.data.l[1] = 0; //The drag was rejected
				reply.xclient.data.l[2] = None;

				s_Data.X11.XLIB.SendEvent(s_Data.X11.display, s_Data.X11.XDND.Source, 0, NoEventMask, &reply);
				s_Data.X11.XLIB.Flush(s_Data.X11.display);
			}
		}
		else if(event.xclient.message_type == s_Data.X11.XDNDPosition)
		{
			//The drag operation has moved over the window
			const int32_t xAbs = static_cast<int32_t>((event.xclient.data.l[2] >> 16) & 0xFFFFu);
			const int32_t yAbs = static_cast<int32_t>((event.xclient.data.l[2]) & 0xFFFFu);
			::Window dummy = 0;
			int32_t xPos = 0, yPos = 0;

			if(s_Data.X11.XDND.Version > TRAP_XDND_VERSION)
				return;

			s_Data.X11.XLIB.TranslateCoordinates(s_Data.X11.display, s_Data.X11.Root, window->X11.Handle, xAbs, yAbs, &xPos, &yPos,
			                                     &dummy);

			InputCursorPos(*window, xPos, yPos);

			XEvent reply = {ClientMessage};
			reply.xclient.window = s_Data.X11.XDND.Source;
			reply.xclient.message_type = s_Data.X11.XDNDStatus;
			reply.xclient.format = 32;
			reply.xclient.data.l[0] = static_cast<int64_t>(window->X11.Handle);
			reply.xclient.data.l[2] = 0; //Specify an empty rectangle
			reply.xclient.data.l[3] = 0;

			if(s_Data.X11.XDND.Format != 0u)
			{
				//Reply that we are ready to copy the dragged data
				reply.xclient.data.l[1] = 1; //Accept with no rectangle
				if(s_Data.X11.XDND.Version >= 2)
					reply.xclient.data.l[4] = static_cast<int64_t>(s_Data.X11.XDNDActionCopy);
			}

			s_Data.X11.XLIB.SendEvent(s_Data.X11.display, s_Data.X11.XDND.Source, 0, NoEventMask, &reply);
			s_Data.X11.XLIB.Flush(s_Data.X11.display);
		}

		return;
	}

	case SelectionNotify:
	{
		if(event.xselection.property == s_Data.X11.XDNDSelection)
		{
			//The converted data from the drag operation has arrived
			char* data = nullptr;
			const uint64_t result = GetWindowPropertyX11(event.xselection.requestor,
														 event.xselection.property,
			                                             event.xselection.target,
														 reinterpret_cast<uint8_t**>(&data));

			if(result != 0u)
			{
				const std::vector<std::string> paths = ParseUriList(data);
				if(!paths.empty())
					InputDrop(*window, paths);
			}

			if(data != nullptr)
				s_Data.X11.XLIB.Free(data);

			if(s_Data.X11.XDND.Version >= 2)
			{
				XEvent reply = {ClientMessage};
				reply.xclient.window = s_Data.X11.XDND.Source;
				reply.xclient.message_type = s_Data.X11.XDNDFinished;
				reply.xclient.format = 32;
				reply.xclient.data.l[0] = static_cast<int64_t>(window->X11.Handle);
				reply.xclient.data.l[1] = static_cast<int64_t>(result);
				reply.xclient.data.l[2] = static_cast<int64_t>(s_Data.X11.XDNDActionCopy);

				s_Data.X11.XLIB.SendEvent(s_Data.X11.display, s_Data.X11.XDND.Source, 0, NoEventMask, &reply);
				s_Data.X11.XLIB.Flush(s_Data.X11.display);
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
			DisableCursor(*window);
		else if (window->cursorMode == CursorMode::Captured)
			CaptureCursor(*window);

		if(window->X11.IC != nullptr)
			s_Data.X11.XLIB.SetICFocus(window->X11.IC);

		InputWindowFocus(*window, true);
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
			EnableCursor(*window);
		else if (window->cursorMode == CursorMode::Captured)
			ReleaseCursor();

		if(window->X11.IC != nullptr)
			s_Data.X11.XLIB.UnsetICFocus(window->X11.IC);

		if((window->Monitor != nullptr) && !window->BorderlessFullscreen)
			PlatformMinimizeWindowX11(*window);

		InputWindowFocus(*window, false);
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

		if(event.xproperty.atom == s_Data.X11.WM_STATE)
		{
			const int32_t state = GetWindowState(*window);
			if(state != IconicState && state != NormalState)
				return;

			const bool minimized = (state == IconicState);
			if(window->Minimized != minimized)
			{
				if(window->Monitor != nullptr)
				{
					if(minimized)
						ReleaseMonitor(*window);
					else
					{
						if(window->BorderlessFullscreen)
							AcquireMonitorBorderless(*window);
						else
							AcquireMonitor(*window);
					}
				}

				window->Minimized = minimized;
				InputWindowMinimize(*window, minimized);
			}
		}
		else if(event.xproperty.atom == s_Data.X11.NET_WM_STATE)
		{
			const bool maximized = PlatformWindowMaximizedX11(*window);
			if (window->Maximized != maximized)
			{
				window->Maximized = maximized;
				InputWindowMaximize(*window, maximized);
			}
		}

		return;
	}

	case DestroyNotify:
	{
		return;
	}

	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Decode a Unicode code point from a UTF-8 stream
[[nodiscard]] uint32_t TRAP::INTERNAL::WindowingAPI::DecodeUTF8(const char** const str)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(str, "WindowingAPI::DecodeUTF8(): Unicode codepoint is nullptr!");

	uint32_t ch = 0, count = 0;
	static constexpr std::array<uint32_t, 6> offsets =
	{
		0x00000000u, 0x00003080u, 0x000E2080u,
		0x03C82080u, 0xFA082080u, 0x82082080u
	};

	do
	{
		ch = (ch << 6u) + static_cast<uint8_t>(**str);
		(*str)++;
		count++;
	} while((**str & 0xC0u) == 0x80u);

	TRAP_ASSERT(count <= 6, "WindowingAPI::DecodeUTF8(): Invalid UTF-8 sequence!");

	return ch - offsets[count - 1];
}

//-------------------------------------------------------------------------------------------------------------------//

//Make the specified window and its video mode active on its monitor
void TRAP::INTERNAL::WindowingAPI::AcquireMonitor(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(s_Data.X11.Saver.Count == 0)
	{
		//Remember old screen saver settings
		s_Data.X11.XLIB.GetScreenSaver(s_Data.X11.display, &s_Data.X11.Saver.Timeout, &s_Data.X11.Saver.Interval,
		                               &s_Data.X11.Saver.Blanking, &s_Data.X11.Saver.Exposure);

		//Disable screen saver
		s_Data.X11.XLIB.SetScreenSaver(s_Data.X11.display, 0, 0, DontPreferBlanking, DefaultExposures);
	}

	if(window.Monitor->Window == nullptr)
		s_Data.X11.Saver.Count++;

	SetVideoModeX11(*window.Monitor, window.videoMode);

	if(window.X11.OverrideRedirect)
	{
		int32_t xPos = 0, yPos = 0;

		//Manually position the window over its monitor
		if(window.Monitor != nullptr)
		{
			PlatformGetMonitorPosX11(*window.Monitor, xPos, yPos);
			const std::optional<InternalVideoMode> mode = PlatformGetVideoModeX11(*window.Monitor);
			if(mode)
				s_Data.X11.XLIB.MoveResizeWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos, mode->Width, mode->Height);
		}
	}

	window.Monitor->Window = &window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Remove the window and restore the original video mode
void TRAP::INTERNAL::WindowingAPI::ReleaseMonitor(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.Monitor->Window != &window)
		return;

	window.Monitor->Window = nullptr;
	RestoreVideoModeX11(*window.Monitor);

	s_Data.X11.Saver.Count--;

	if(s_Data.X11.Saver.Count == 0)
	{
		//Restore old screen saver settings
		s_Data.X11.XLIB.SetScreenSaver(s_Data.X11.display, s_Data.X11.Saver.Timeout, s_Data.X11.Saver.Interval,
		                               s_Data.X11.Saver.Blanking, s_Data.X11.Saver.Exposure);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Set the current video mode for the specified monitor
void TRAP::INTERNAL::WindowingAPI::SetVideoModeX11(InternalMonitor& monitor, const InternalVideoMode& desired)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.X11.RandR.Available || s_Data.X11.RandR.MonitorBroken)
		return;

	RRMode native = 0;

	const std::optional<InternalVideoMode> best = ChooseVideoMode(monitor, desired);
	const std::optional<InternalVideoMode> current = PlatformGetVideoModeX11(monitor);
	if(!current || !best || static_cast<int>(CompareVideoModes(*current, *best)) == 0)
		return;

	XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
	XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);
	XRROutputInfo* const oi = s_Data.X11.RandR.GetOutputInfo(s_Data.X11.display, sr, monitor.X11.Output);

	if((sr != nullptr) && (ci != nullptr) && (oi != nullptr))
	{
		for(int32_t i = 0; i < oi->nmode; i++)
		{
			const XRRModeInfo* const mi = GetModeInfo(*sr, oi->modes[i]);
			if((mi == nullptr) || (ci == nullptr) || !((mi->modeFlags & RR_Interlace) == 0))
				continue;

			const std::optional<InternalVideoMode> mode = VideoModeFromModeInfo(*mi, *ci);
			if(best && mode && CompareVideoModes(*best, *mode))
			{
				native = mi->id;
				break;
			}
		}

		if(native != 0u)
		{
			if(monitor.X11.OldMode == 0)
				monitor.X11.OldMode = ci->mode;

			s_Data.X11.RandR.SetCrtcConfig(s_Data.X11.display, sr, monitor.X11.CRTC, CurrentTime, ci->x, ci->y, native,
										ci->rotation, ci->outputs, ci->noutput);
		}
	}

	if(oi != nullptr)
		s_Data.X11.RandR.FreeOutputInfo(oi);
	if(ci != nullptr)
		s_Data.X11.RandR.FreeCrtcInfo(ci);
	if(sr != nullptr)
		s_Data.X11.RandR.FreeScreenResources(sr);
}

//-------------------------------------------------------------------------------------------------------------------//

//Restore the saved(original) video mode for the specified monitor
void TRAP::INTERNAL::WindowingAPI::RestoreVideoModeX11(InternalMonitor& monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.X11.RandR.Available || s_Data.X11.RandR.MonitorBroken)
		return;

	if(monitor.X11.OldMode == 0)
		return;

	XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
	XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);

	if((sr != nullptr) && (ci != nullptr))
	{
		s_Data.X11.RandR.SetCrtcConfig(s_Data.X11.display, sr, monitor.X11.CRTC, CurrentTime, ci->x, ci->y, monitor.X11.OldMode,
									ci->rotation, ci->outputs, ci->noutput);

		s_Data.X11.RandR.FreeCrtcInfo(ci);
		s_Data.X11.RandR.FreeScreenResources(sr);

		monitor.X11.OldMode = 0;
	}
	else
		InputErrorX11(Error::Platform_Error, "[X11] Failed to restore video mode");
}

//-------------------------------------------------------------------------------------------------------------------//

//Make the specified window active on its monitor
void TRAP::INTERNAL::WindowingAPI::AcquireMonitorBorderless(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(s_Data.X11.Saver.Count == 0)
	{
		//Remember old screen saver settings
		s_Data.X11.XLIB.GetScreenSaver(s_Data.X11.display, &s_Data.X11.Saver.Timeout, &s_Data.X11.Saver.Interval,
		                               &s_Data.X11.Saver.Blanking, &s_Data.X11.Saver.Exposure);

		//Disable screen saver
		s_Data.X11.XLIB.SetScreenSaver(s_Data.X11.display, 0, 0, DontPreferBlanking, DefaultExposures);
	}

	if(window.Monitor->Window == nullptr)
		s_Data.X11.Saver.Count++;

	if(window.X11.OverrideRedirect)
	{
		int32_t xPos = 0, yPos = 0;

		//Manually position the window over its monitor
		if(window.Monitor != nullptr)
		{
			PlatformGetMonitorPosX11(*window.Monitor, xPos, yPos);
			const std::optional<InternalVideoMode> mode = PlatformGetVideoModeX11(*window.Monitor);
			if(mode)
				s_Data.X11.XLIB.MoveResizeWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos, mode->Width, mode->Height);
		}
	}

	window.Monitor->Window = &window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a dummy window for behind-the-scenes work
[[nodiscard]] ::Window TRAP::INTERNAL::WindowingAPI::CreateHelperWindow()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	XSetWindowAttributes wa{};
	wa.event_mask = PropertyChangeMask;

	return s_Data.X11.XLIB.CreateWindow(s_Data.X11.display, s_Data.X11.Root, 0, 0, 1, 1, 0, 0, InputOnly,
	                                    DefaultVisual(s_Data.X11.display, s_Data.X11.Screen), CWEventMask, &wa);
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the cursor image according to its cursor mode
void TRAP::INTERNAL::WindowingAPI::UpdateCursorImage(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.cursorMode == CursorMode::Normal || window.cursorMode == CursorMode::Captured)
	{
		if(window.Cursor != nullptr)
			s_Data.X11.XLIB.DefineCursor(s_Data.X11.display, window.X11.Handle, window.Cursor->X11.Handle);
		else
			s_Data.X11.XLIB.UndefineCursor(s_Data.X11.display, window.X11.Handle);
	}
	else
		s_Data.X11.XLIB.DefineCursor(s_Data.X11.display, window.X11.Handle, s_Data.X11.HiddenCursorHandle);
}

//-------------------------------------------------------------------------------------------------------------------//

//Apply disabled cursor mode to a focused window
void TRAP::INTERNAL::WindowingAPI::DisableCursor(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.RawMouseMotion)
		EnableRawMouseMotion(window);

	s_Data.DisabledCursorWindow = &window;
	PlatformGetCursorPosX11(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
	UpdateCursorImage(window);
	CenterCursorInContentArea(window);
	CaptureCursor(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Exit disabled cursor mode for the specified window
void TRAP::INTERNAL::WindowingAPI::EnableCursor(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(window.RawMouseMotion)
		DisableRawMouseMotion(window);

	s_Data.DisabledCursorWindow = nullptr;
	ReleaseCursor();
	PlatformSetCursorPosX11(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
	UpdateCursorImage(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Clear its handle when the input context has been destroyed
void TRAP::INTERNAL::WindowingAPI::InputContextDestroyCallback([[maybe_unused]] XIC ic, XPointer clientData,
                                                               [[maybe_unused]] XPointer callData)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(clientData, "WindowingAPI::InputContextDestroyCallback(): XPointer is nullptr!");

	InternalWindow* const window = reinterpret_cast<InternalWindow*>(clientData);
	if(window != nullptr)
		window->X11.IC = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Allocates and returns a monitor object with the specified name and dimensions
[[nodiscard]] TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalMonitor> TRAP::INTERNAL::WindowingAPI::CreateMonitor(std::string name)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	Scope<InternalMonitor> monitor = MakeScope<InternalMonitor>();
	if(!name.empty())
		monitor->Name = std::move(name);

	return monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create key code translation tables
void TRAP::INTERNAL::WindowingAPI::CreateKeyTablesX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t scanCode = 0, scanCodeMin = 0, scanCodeMax = 0;

	std::fill(s_Data.KeyCodes.begin(), s_Data.KeyCodes.end(), Input::Key::Unknown);
	std::fill(s_Data.ScanCodes.begin(), s_Data.ScanCodes.end(), -1);

	if(s_Data.X11.XKB.Available)
	{
		//Use XKB to determine physical key locations independently of the current keyboard layout

		const XkbDescPtr desc = s_Data.X11.XKB.GetMap(s_Data.X11.display, 0, XkbUseCoreKbd);
		s_Data.X11.XKB.GetNames(s_Data.X11.display, XkbKeyNamesMask | XkbKeyAliasesMask, desc);

		scanCodeMin = desc->min_key_code;
		scanCodeMax = desc->max_key_code;

		struct Keys
		{
			TRAP::Input::Key Key = TRAP::Input::Key::Unknown;
			std::string_view Name;
		};

		constexpr std::array<Keys, 121> KeyMap =
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
				if(strncmp(desc->names->keys[scanCode].name, keyMapKey.Name.data(), XkbKeyNameLength) == 0)
				{
					key = keyMapKey.Key;
					break;
				}
			}

			//Fall back to key aliases in case the key name did not match
			for(uint8_t i = 0; i < desc->names->num_key_aliases; i++)
			{
				if(key != TRAP::Input::Key::Unknown)
					break;

				if(strncmp(desc->names->key_aliases[i].real, desc->names->keys[scanCode].name, XkbKeyNameLength) != 0)
					continue;

				for(const Keys& keyMapKey : KeyMap)
				{
					if(strncmp(desc->names->key_aliases[i].alias, keyMapKey.Name.data(), XkbKeyNameLength) == 0)
					{
						key = keyMapKey.Key;
						break;
					}
				}
			}

			s_Data.KeyCodes[scanCode] = key;
		}

		s_Data.X11.XKB.FreeNames(desc, XkbKeyNamesMask, 1);
		s_Data.X11.XKB.FreeKeyboard(desc, 0, 1);
	}
	else
		s_Data.X11.XLIB.DisplayKeycodes(s_Data.X11.display, &scanCodeMin, &scanCodeMax);

	int32_t width = 0;
	KeySym* const keySyms = s_Data.X11.XLIB.GetKeyboardMapping(s_Data.X11.display, static_cast<uint8_t>(scanCodeMin), scanCodeMax - scanCodeMin + 1,
	                                                           &width);

	for(scanCode = scanCodeMin; scanCode <= scanCodeMax; scanCode++)
	{
		//Translate the un-translated key codes using traditional X11 KeySym lookups
		if(static_cast<int32_t>(s_Data.KeyCodes[scanCode]) < 0)
		{
			const std::size_t base = static_cast<std::size_t>((scanCode - scanCodeMin)) * width;
			const std::vector<KeySym> keySymsVec(&keySyms[base], &keySyms[base] + width);
			s_Data.KeyCodes[scanCode] = TranslateKeySyms(keySymsVec).value_or(TRAP::Input::Key::Unknown);
		}

		//Store the reverse translation for faster key name lookup
		if(static_cast<int32_t>(s_Data.KeyCodes[scanCode]) > 0)
			s_Data.ScanCodes[static_cast<int32_t>(s_Data.KeyCodes[scanCode])] = static_cast<int16_t>(scanCode);
	}

	s_Data.X11.XLIB.Free(keySyms);
}

//-------------------------------------------------------------------------------------------------------------------//

//Grabs the cursor and confines it to the window
void TRAP::INTERNAL::WindowingAPI::CaptureCursor(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	s_Data.X11.XLIB.GrabPointer(s_Data.X11.display, window.X11.Handle, 1,
				                ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
				                GrabModeAsync, GrabModeAsync,
				                window.X11.Handle,
							    None,
				                CurrentTime);
}

//-------------------------------------------------------------------------------------------------------------------//

//Ungrabs the cursor
void TRAP::INTERNAL::WindowingAPI::ReleaseCursor()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	s_Data.X11.XLIB.UngrabPointer(s_Data.X11.display, CurrentTime);
}

//-------------------------------------------------------------------------------------------------------------------//

//Translate the X11 KeySyms for a key to a TRAP key
//NOTE: This is only used as a fallback, in case the XKB method fails
//      It is layout-dependent and will fail partially on most non-US layouts
[[nodiscard]] std::optional<TRAP::Input::Key> TRAP::INTERNAL::WindowingAPI::TranslateKeySyms(const std::vector<KeySym>& keySyms)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(keySyms.empty())
		return std::nullopt;

	if(keySyms.size() > 1)
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
			[[fallthrough]];
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
			[[fallthrough]];
		case XK_Alt_L:
			return TRAP::Input::Key::Left_ALT;

		case XK_Mode_switch: //Mapped to Alt_R on many keyboards
			[[fallthrough]];
		case XK_ISO_Level3_Shift: //AltGr on at least some machines
			[[fallthrough]];
		case XK_Meta_R:
			[[fallthrough]];
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
	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<std::string> TRAP::INTERNAL::WindowingAPI::GetLinuxKeyboardLayoutNameX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (!s_Data.X11.XKB.Available)
	{
		InputError(Error::API_Unavailable, "[Input][X11] XKB extension required for keyboard layout names");
		return std::nullopt;
	}

	XkbStateRec state{};
	s_Data.X11.XKB.GetState(s_Data.X11.display, XkbUseCoreKbd, &state);

	const XkbDescPtr desc = s_Data.X11.XKB.AllocKeyboard();
	if (s_Data.X11.XKB.GetNames(s_Data.X11.display, XkbGroupNamesMask, desc) != 0) //0 = Success
	{
		s_Data.X11.XKB.FreeKeyboard(desc, 0, 1);
		InputError(Error::Platform_Error, "[Input][X11] Failed to retrieve keyboard layout names");
		return std::nullopt;
	}

	const Atom atom = desc->names->groups[state.group];
	s_Data.X11.XKB.FreeKeyboard(desc, 0, 1);

	if (atom == 0)
	{
		InputError(Error::Platform_Error, "[Input][X11] Name missing for current keyboard layout");
		return std::nullopt;
	}

	return s_Data.X11.XLIB.GetAtomName(s_Data.X11.display, atom);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDropX11(InternalWindow& window, const bool value)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(value)
	{
		//Announce support for Xdnd (drag and drop)
		const Atom version = 5;
		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.XDNDAware, XA_ATOM, 32,
			                       PropModeReplace, reinterpret_cast<const uint8_t*>(&version), 1);
	}
	else
		s_Data.X11.XLIB.DeleteProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.XDNDAware);
}

#endif