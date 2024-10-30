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

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"
#include "Utils/DynamicLoading/DynamicLoading.h"

namespace
{
	//Action for EWMH client messages
	constexpr i32 _NET_WM_STATE_REMOVE = 0;
	constexpr i32 _NET_WM_STATE_ADD = 1;

	//Additional mouse button names for XButtonEvent
	constexpr i32 Button6 = 6;
	constexpr i32 Button7 = 7;

	//Motif WM hints flags
	constexpr i32 MWM_HINTS_DECORATIONS = 2;
	constexpr i32 MWM_DECOR_ALL = 1;

	constexpr i32 TRAP_XDND_VERSION = 5;

	//Atom values
	constexpr i32 None = 0;

	constexpr i32 KeyPress = 2;
	constexpr i32 KeyRelease = 3;


	//-------------------------------------------------------------------------------------------------------------------//

	//Convert the specified Latin-1 string to UTF-8
	[[nodiscard]] constexpr std::string ConvertLatin1ToUTF8(const std::string_view source)
	{
		usize size = 1u;
		for(const char c : source)
			size += (NumericCast<u8>(c) & 0x80u) != 0u ? 2u : 1u;

		std::string target{};
		target.reserve(size);

		for(const char c : source)
			target += TRAP::Utils::String::EncodeUTF8(NumericCast<u32>(c));

		return target;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	//Translate the X11 KeySyms for a key to a TRAP key
	//NOTE: This is only used as a fallback, in case the XKB method fails
	//      It is layout-dependent and will fail partially on most non-US layouts
	[[nodiscard]] constexpr std::optional<TRAP::Input::Key> TranslateKeySyms(const std::vector<KeySym>& keySyms)
	{
		if(keySyms.empty())
			return std::nullopt;

		if(keySyms.size() > 1u)
		{
			switch(keySyms[1u])
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

		switch(keySyms[0u])
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
}

//Sends an EWMH or ICCCM event to the window manager
void TRAP::INTERNAL::WindowingAPI::SendEventToWM(const InternalWindow& window, const Atom type, const i64 a,
												 const i64 b, const i64 c, const i64 d, const i64 e)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

bool TRAP::INTERNAL::WindowingAPI::WaitForX11Event(f64* const timeout)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	pollfd fd
	{
		.fd = ConnectionNumber(s_Data.X11.display),
		.events = POLLIN,
		.revents = 0
	};

	while(s_Data.X11.XLIB.Pending(s_Data.X11.display) == 0)
	{
		if(!PollPOSIX({&fd, 1u}, timeout))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::WaitForAnyEvent(f64* const timeout)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	std::array<pollfd, 3u> fds
	{
		{
			pollfd{.fd = ConnectionNumber(s_Data.X11.display), .events = POLLIN, .revents = 0},
			pollfd{.fd = std::get<0u>(s_Data.EmptyEventPipe), .events = POLLIN, .revents = 0},
			pollfd{.fd = TRAP::Input::s_linuxController.INotify, .events = POLLIN, .revents = 0}
		}
	};

	while(s_Data.X11.XLIB.Pending(s_Data.X11.display) == 0)
	{
		if(!PollPOSIX(fds, timeout))
			return false;

		for(usize i = 1u; i < fds.size(); ++i)
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	while(true)
	{
		const char byte = 0;
		const ssize_t result = write(std::get<1u>(s_Data.EmptyEventPipe), &byte, 1u);
		if(result == 1 || (result == -1 && errno != EINTR))
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DrainEmptyEvents()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	while(true)
	{
		std::array<char, 64u> dummy{};
		const ssize_t result = read(std::get<0u>(s_Data.EmptyEventPipe), dummy.data(), dummy.size());
		if(result == -1 && errno != EINTR)
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::SetEventFlags(const i32 fileDescriptor)
{
	const i32 sf = fcntl(fileDescriptor, F_GETFL, 0);
	const i32 df = fcntl(fileDescriptor, F_GETFD, 0);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(pipe(s_Data.EmptyEventPipe.data()) != 0)
	{
		InputError(Error::Platform_Error, "X11: Failed to create empty event pipe: " + Utils::String::GetStrError());
		return false;
	}

	return std::ranges::all_of(s_Data.EmptyEventPipe, SetEventFlags);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieve a single window property of the specified type
[[nodiscard]] u64 TRAP::INTERNAL::WindowingAPI::GetWindowPropertyX11(::Window window, const Atom property, const Atom type,
                                                                          u8** const value)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	TRAP_ASSERT(value, "WindowingAPI::GetWindowPropertyX11(): value is nullptr!");

	Atom actualType{};
	i32 actualFormat{};
	u64 itemCount{}, bytesAfter{};

	s_Data.X11.XLIB.GetWindowProperty(s_Data.X11.display, window, property, 0, std::numeric_limits<i64>::max(),
	                                  0, type, &actualType, &actualFormat, &itemCount, &bytesAfter, value);

	return itemCount;
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the normal hints according to the window settings
void TRAP::INTERNAL::WindowingAPI::UpdateNormalHints(const InternalWindow& window, const i32 width,
                                                     const i32 height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	XSizeHints* const hints = s_Data.X11.XLIB.AllocSizeHints();
	if(hints == nullptr)
	{
		InputErrorX11(Error::Platform_Error, "[X11] Failed to update normal hints (allocation of hints failed)!");
		return;
	}

	i64 supplied = 0;
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	XEvent dummy;
	f64 timeout = 0.1;

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
						  static_cast<i64>(s_Data.X11.NET_WM_STATE_FULLSCREEN),
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
			static constexpr u64 value = 1u;

			s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle,
			                               s_Data.X11.NET_WM_BYPASS_COMPOSITOR, XA_CARDINAL, 32,
										   PropModeReplace, reinterpret_cast<const u8*>(&value), 1);
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
						  static_cast<i64>(s_Data.X11.NET_WM_STATE_FULLSCREEN),
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

using namespace std::string_view_literals;

//Retrieve system content scale via folklore heuristics
void TRAP::INTERNAL::WindowingAPI::GetSystemContentScale(f32& xScale, f32& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	//Start by assuming the default X11 DPI
	//NOTE: Some desktop environments (KDE) may remove the Xft.dpi field when it would be set to 96, so assume
	//      that is the case if we cannot find it
	f32 xDPI = 96.0f, yDPI = 96.0f;

	//NOTE: Basing the scale on Xft.dpi where available should provide the most consistent user experience
	//      (matches Qt, Gtk, etc), although not always the most accurate one
	const char* const rms = s_Data.X11.XLIB.ResourceManagerString(s_Data.X11.display);
	if(rms != nullptr)
	{
		XrmDatabase db = s_Data.X11.XRM.GetStringDatabase(rms);
		if(db != nullptr)
		{
			XrmValue value;
			char* type = nullptr;

			if(s_Data.X11.XRM.GetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value) != 0)
			{
				if((type != nullptr) && "String"sv == type)
				{
					xDPI = yDPI = NumericCast<f32>(std::stod(value.addr));
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
		i32 supported = 0;

		if(s_Data.X11.XKB.SetDetectableAutoRepeat(s_Data.X11.display, 1, &supported) != 0)
		{
			if(supported != 0)
				s_Data.X11.XKB.Detectable = true;
		}

		s_Data.X11.XKB.Group = 0;
		XkbStateRec state;
		if(s_Data.X11.XKB.GetState(s_Data.X11.display, XkbUseCoreKbd, &state) == 0) //0 = Success
			s_Data.X11.XKB.Group = NumericCast<u32>(state.group);

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
	                                                     fmt::format("_NET_WM_CM_S{}", s_Data.X11.Screen).c_str(),
														 0);

	//Detect whether an EWMH-conformant window manager is running
	DetectEWMH();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Check whether the running window manager is EMWH-compliant
void TRAP::INTERNAL::WindowingAPI::DetectEWMH()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	//First we read the _NET_SUPPORTING_WM_CHECK property on the root window
	::Window* windowFromRoot = nullptr;
	if(GetWindowPropertyX11(s_Data.X11.Root, s_Data.X11.NET_SUPPORTING_WM_CHECK, XA_WINDOW,
	                        reinterpret_cast<u8**>(&windowFromRoot)) == 0u)
		return;

	GrabErrorHandlerX11();

	//If it exists, it should be the XID of a top-level window
	//Then we look for the same property on that window
	::Window* windowFromChild = nullptr;
	if(GetWindowPropertyX11(*windowFromRoot, s_Data.X11.NET_SUPPORTING_WM_CHECK, XA_WINDOW,
	                        reinterpret_cast<u8**>(&windowFromChild)) == 0u)
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
	const u64 atomCount = GetWindowPropertyX11(s_Data.X11.Root, s_Data.X11.NET_SUPPORTED, XA_ATOM,
	                                                reinterpret_cast<u8**>(&supportedAtoms));
	if(supportedAtoms == nullptr)
		return;

	const std::span<const Atom> supportedAtomsSpan(supportedAtoms, atomCount);

	//See which of the atoms we support that are supported by the WM

	s_Data.X11.NET_WM_STATE                   = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_STATE").value_or(0u);
    s_Data.X11.NET_WM_STATE_ABOVE             = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_STATE_ABOVE").value_or(0u);
    s_Data.X11.NET_WM_STATE_FULLSCREEN        = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_STATE_FULLSCREEN").value_or(0u);
    s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT    = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_STATE_MAXIMIZED_VERT").value_or(0u);
    s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ    = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_STATE_MAXIMIZED_HORZ").value_or(0u);
    s_Data.X11.NET_WM_STATE_DEMANDS_ATTENTION = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_STATE_DEMANDS_ATTENTION").value_or(0u);
    s_Data.X11.NET_WM_FULLSCREEN_MONITORS     = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_FULLSCREEN_MONITORS").value_or(0u);
    s_Data.X11.NET_WM_WINDOW_TYPE             = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_WINDOW_TYPE").value_or(0u);
    s_Data.X11.NET_WM_WINDOW_TYPE_NORMAL      = GetAtomIfSupported(supportedAtomsSpan, "_NET_WM_WINDOW_TYPE_NORMAL").value_or(0u);
    s_Data.X11.NET_WORKAREA                   = GetAtomIfSupported(supportedAtomsSpan, "_NET_WORKAREA").value_or(0u);
    s_Data.X11.NET_CURRENT_DESKTOP            = GetAtomIfSupported(supportedAtomsSpan, "_NET_CURRENT_DESKTOP").value_or(0u);
    s_Data.X11.NET_ACTIVE_WINDOW              = GetAtomIfSupported(supportedAtomsSpan, "_NET_ACTIVE_WINDOW").value_or(0u);

	if(supportedAtoms != nullptr)
		s_Data.X11.XLIB.Free(supportedAtoms);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the X error handler callback
void TRAP::INTERNAL::WindowingAPI::GrabErrorHandlerX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	TRAP_ASSERT(s_Data.X11.PrevErrorHandler == nullptr, "WindowingAPI::GrabErrorHandlerX11(): Previous error handler is not nullptr!");

	s_Data.X11.ErrorCode = 0; //0 = Success
	s_Data.X11.PrevErrorHandler = s_Data.X11.XLIB.SetErrorHandler(ErrorHandler);
}

//-------------------------------------------------------------------------------------------------------------------//

//X error handler
[[nodiscard]] i32 TRAP::INTERNAL::WindowingAPI::ErrorHandler(Display* const display, XErrorEvent* const event)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	//Synchronize to make sure all commands are processed
	s_Data.X11.XLIB.Sync(s_Data.X11.display, 0);
	s_Data.X11.XLIB.SetErrorHandler(s_Data.X11.PrevErrorHandler);
	s_Data.X11.PrevErrorHandler = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Return the atom ID only if it is listed in the specified array
[[nodiscard]] std::optional<Atom> TRAP::INTERNAL::WindowingAPI::GetAtomIfSupported(const std::span<const Atom> supportedAtoms,
                                                                                   const std::string& atomName)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	const Atom searchAtom = s_Data.X11.XLIB.InternAtom(s_Data.X11.display, atomName.c_str(), 0);

	const auto res = std::ranges::find(supportedAtoms, searchAtom);
	if(res != supportedAtoms.end())
		return *res;

	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create a blank cursor for hidden and disabled cursor modes
[[nodiscard]] std::optional<Cursor> TRAP::INTERNAL::WindowingAPI::CreateHiddenCursor()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	static const std::vector<u8> pixels(NumericCast<usize>(16u) * 16u * 4u, 0u);
	const Scope<Image> cursorImage = TRAP::Image::LoadFromMemory(16u, 16u, TRAP::Image::ColorFormat::RGBA,
	                                                             pixels);
	if(!cursorImage)
		return std::nullopt;

	return CreateCursorX11(*cursorImage, 0u, 0u);
}

//-------------------------------------------------------------------------------------------------------------------//

//Check whether the IM has a usable style
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::HasUsableInputMethodStyle()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	XIMStyles* styles = nullptr;

	if(s_Data.X11.XLIB.GetIMValues(s_Data.X11.IM, XNQueryInputStyle, &styles, nullptr) != nullptr || (styles == nullptr))
		return false;

	for(u16 i = 0u; i < styles->count_styles; ++i)
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	s_Data.X11.IM = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputMethodInstantiateCallback([[maybe_unused]] Display* display,
                                                                  [[maybe_unused]] XPointer clientData,
																  [[maybe_unused]] XPointer callData)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

	static constexpr XIMCallback callback
	{
		.client_data = nullptr,
		.callback = static_cast<XIMProc>(InputMethodDestroyCallback)
	};
	s_Data.X11.XLIB.SetIMValues(s_Data.X11.IM, XNDestroyCallback, &callback, nullptr);

	for(const Scope<InternalWindow>& window : s_Data.WindowList)
		CreateInputContextX11(*window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Poll for changes in the set of connected monitors
void TRAP::INTERNAL::WindowingAPI::PollMonitorsX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(!s_Data.X11.RandR.Available || s_Data.X11.RandR.MonitorBroken)
	{
		InputMonitor(CreateMonitor("Display"), true, 0u);
		return;
	}

	XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
	const RROutput primary = s_Data.X11.RandR.GetOutputPrimary(s_Data.X11.display, s_Data.X11.Root);

	if(sr == nullptr)
	{
		InputErrorX11(Error::Platform_Error, "[X11] Failed to poll monitors (failed to retrieve current screen resources)!");
		return;
	}

	i32 screenCount = 0;
	XineramaScreenInfo* screens = nullptr;
	if(s_Data.X11.Xinerama.Available)
		screens = s_Data.X11.Xinerama.QueryScreens(s_Data.X11.display, &screenCount);

	const usize disconnectedCount = s_Data.Monitors.size();
	std::vector<InternalMonitor*> disconnected{};
	if(disconnectedCount != 0u)
	{
		disconnected.resize(s_Data.Monitors.size());
		for(usize i = 0u; i < s_Data.Monitors.size(); ++i)
			disconnected[i] = s_Data.Monitors[i].get();
	}

	for(i32 i = 0; i < sr->noutput; ++i)
	{
		XRROutputInfo* const oi = s_Data.X11.RandR.GetOutputInfo(s_Data.X11.display, sr, sr->outputs[i]);
		if((oi == nullptr) || oi->connection != RR_Connected || oi->crtc == 0u)
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

		usize j = 0u;
		for(j = 0u; j < disconnectedCount; ++j)
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
			for(j = 0u; std::cmp_less(j, screenCount); ++j)
			{
				if(screens[j].x_org == ci->x && screens[j].y_org == ci->y &&
				   std::cmp_equal(screens[j].width, ci->width) &&
				   std::cmp_equal(screens[j].height, ci->height))
				{
					monitor->X11.Index = NumericCast<i32>(j);
					break;
				}
			}
		}

		u32 type = 0u;
		if(monitor->X11.Output == primary)
			type = 0u;
		else
			type = 1u;

		InputMonitor(std::move(monitor), true, type);

		s_Data.X11.RandR.FreeOutputInfo(oi);
		s_Data.X11.RandR.FreeCrtcInfo(ci);
	}

	if(sr != nullptr)
		s_Data.X11.RandR.FreeScreenResources(sr);

	if(screens != nullptr)
		s_Data.X11.XLIB.Free(screens);

	for(usize i = 0u; i < disconnectedCount; ++i)
	{
		if(disconnected[i] != nullptr)
			InputMonitorDisconnect(NumericCast<u32>(i), 0u);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the event is a selection event
[[nodiscard]] i32 TRAP::INTERNAL::WindowingAPI::IsSelectionEvent([[maybe_unused]] Display* const display,
                                                                      XEvent* const event,
																	  [[maybe_unused]] XPointer ptr)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	TRAP_ASSERT(event, "WindowingAPI::IsSelectionEvent(): XEvent is nullptr!");

	if(event->xany.window != s_Data.X11.HelperWindowHandle)
		return 0;

	return NumericCast<i32>(event->type == SelectionRequest ||
	                            event->type == SelectionNotify ||
		                        event->type == SelectionClear);
}

//-------------------------------------------------------------------------------------------------------------------//

//Set the specified property to the selection converted to the requested target
[[nodiscard]] Atom TRAP::INTERNAL::WindowingAPI::WriteTargetToProperty(const XSelectionRequestEvent& request)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	std::string selectionString{};
	const std::array<Atom, 2u> formats{s_Data.X11.UTF8_STRING, XA_STRING};

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
		const std::array<Atom, 4u> targets{s_Data.X11.TARGETS, s_Data.X11.MULTIPLE, s_Data.X11.UTF8_STRING, XA_STRING};

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, request.property, XA_ATOM, 32,
		                               PropModeReplace, reinterpret_cast<const u8*>(targets.data()),
								       targets.size());

		return request.property;
	}

	if(request.target == s_Data.X11.MULTIPLE)
	{
		//Multiple conversions were requested

		Atom* targets = nullptr;
		const u64 count = GetWindowPropertyX11(request.requestor, request.property, s_Data.X11.ATOM_PAIR,
		                                            reinterpret_cast<u8**>(&targets));

		if(targets == nullptr)
			return None;

		for(u64 i = 0u; i < count; i += 2u)
		{
			if(std::ranges::contains(formats, targets[i]))
			{
				s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, targets[i + 1u], targets[i], 8,
				                               PropModeReplace, reinterpret_cast<u8*>(selectionString.data()),
										       NumericCast<i32>(selectionString.size()));
			}
			else
				targets[i + 1u] = None;
		}

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, request.property, s_Data.X11.ATOM_PAIR, 32,
                                       PropModeReplace, reinterpret_cast<u8*>(targets), NumericCast<i32>(count));

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

	if(std::ranges::contains(formats, request.target))
	{
		//The requested target is one we support

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, request.requestor, request.property, request.target, 8,
									   PropModeReplace, reinterpret_cast<u8*>(selectionString.data()),
									   NumericCast<i32>(selectionString.size()));

		return request.property;
	}

	//The request target is not supported

	return None;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::HandleSelectionRequest(const XEvent& event)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	const XIMCallback callback
	{
		.client_data = reinterpret_cast<XPointer>(&window),
		.callback = reinterpret_cast<XIMProc>(InputContextDestroyCallback),
	};

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

	u64 filter = 0u;
	if(s_Data.X11.XLIB.GetICValues(window.X11.IC, XNFilterEvents, &filter, nullptr) == nullptr)
		s_Data.X11.XLIB.SelectInput(s_Data.X11.display, window.X11.Handle,
		                            NumericCast<i64>(NumericCast<u64>(attribs.your_event_mask) | filter));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::IsVisualTransparentX11(const Visual& visual)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(!s_Data.X11.XRender.Available)
		return false;

	const XRenderPictFormat* const pf = s_Data.X11.XRender.FindVisualFormat(s_Data.X11.display, &visual);
	return (pf != nullptr) && (pf->direct.alphaMask != 0);
}

//-------------------------------------------------------------------------------------------------------------------//

//Create the X11 window (and its colormap)
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::CreateNativeWindow(InternalWindow& window, WindowConfig& WNDConfig,
                                                                    Visual& visual, i32 depth)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	const u32 width = WNDConfig.Width;
	const u32 height = WNDConfig.Height;

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
								                     0u,
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
		std::array<Atom, 3u> states{};
		u32 count = 0;

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

		if(count != 0u)
		{
			s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM, 32,
							               PropModeReplace, reinterpret_cast<const u8*>(states.data()),
										   NumericCast<i32>(count));
		}
	}

	//Declare the WM protocols supported by TRAP
	{
		std::array<Atom, 2u> protocols{s_Data.X11.WM_DELETE_WINDOW, s_Data.X11.NET_WM_PING};

		s_Data.X11.XLIB.SetWMProtocols(s_Data.X11.display, window.X11.Handle, protocols.data(), protocols.size());
	}

	//Declare our PID
	{
		const i64 pid = getpid();
		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_PID, XA_CARDINAL, 32,
		                               PropModeReplace, reinterpret_cast<const u8*>(&pid), 1);
	}

	if((s_Data.X11.NET_WM_WINDOW_TYPE != 0u) && (s_Data.X11.NET_WM_WINDOW_TYPE_NORMAL != 0u))
	{
		const Atom type = s_Data.X11.NET_WM_WINDOW_TYPE_NORMAL;
		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_WINDOW_TYPE, XA_ATOM, 32,
		                               PropModeReplace, reinterpret_cast<const u8*>(&type), 1);
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
			hints->min_width = hints->max_width = NumericCast<i32>(width);
			hints->min_height = hints->max_height = NumericCast<i32>(height);
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
                                                                                  const u32 xHotSpot,
                                                                                  const u32 yHotSpot)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(s_Data.X11.XCursor.Handle == nullptr)
		return std::nullopt;

	XcursorImage* const native = s_Data.X11.XCursor.ImageCreate(NumericCast<i32>(image.GetWidth()),
	                                                            NumericCast<i32>(image.GetHeight()));
	if(native == nullptr)
		return std::nullopt;

	native->xhot = xHotSpot;
	native->yhot = yHotSpot;

	const u8* source = image.GetPixelData().data();
	XcursorPixel* target = native->pixels;

	for(u64 i = 0u; i < NumericCast<u64>(image.GetWidth()) * NumericCast<u64>(image.GetHeight()); ++i, ++target, source += 4u)
	{
		const u32 alpha = source[3u];

		*target = (alpha << 24u) |
		          NumericCast<u8>(((source[0u] * alpha) / 255u) << 16u) |
				  NumericCast<u8>(((source[1u] * alpha) / 255u) <<  8u) |
				  NumericCast<u8>(((source[2u] * alpha) / 255u) <<  0u);
	}

	const Cursor cursor = s_Data.X11.XCursor.ImageLoadCursor(s_Data.X11.display, native);
	s_Data.X11.XCursor.ImageDestroy(native);

	return cursor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the window is iconified
[[nodiscard]] i32 TRAP::INTERNAL::WindowingAPI::GetWindowState(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	i32 result = WithdrawnState;
	struct State
	{
		CARD32 State{};
		::Window Icon{};
	} *state = nullptr;

	if(GetWindowPropertyX11(window.X11.Handle, s_Data.X11.WM_STATE, s_Data.X11.WM_STATE,
	                        reinterpret_cast<u8**>(&state)) >= 2 && (state != nullptr))
		result = NumericCast<i32>(state->State);

	if(state != nullptr)
		s_Data.X11.XLIB.Free(state);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::GetSelectionString(const Atom selection)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	std::string& selectionString = (selection == s_Data.X11.PRIMARY) ? s_Data.X11.PrimarySelectionString : s_Data.ClipboardString;
	const std::array<Atom, 2u> targets = {s_Data.X11.UTF8_STRING, XA_STRING};

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
		Atom actualType = 0u;
		i32 actualFormat = 0;
		u64 itemCount = 0u, bytesAfter = 0u;
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
		                                  notification.xselection.property, 0, std::numeric_limits<i64>::max(), 1, AnyPropertyType,
									      &actualType, &actualFormat, &itemCount, &bytesAfter,
									      reinterpret_cast<u8**>(&data));

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
				                                  notification.xselection.property, 0, std::numeric_limits<i64>::max(), 1, AnyPropertyType,
											      &actualType, &actualFormat, &itemCount, &bytesAfter,
											      reinterpret_cast<u8**>(&data));

				if(itemCount != 0u)
					string = data;

				if(itemCount == 0u)
				{
					if(string.empty())
					{
						if(target == XA_STRING)
						{
							selectionString = ConvertLatin1ToUTF8(string);
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
[[nodiscard]] i32 TRAP::INTERNAL::WindowingAPI::IsSelPropNewValueNotify([[maybe_unused]] Display* const display,
                                                                            XEvent* const event, XPointer const ptr)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	TRAP_ASSERT(event, "WindowingAPI::IsSelPropNewValueNotify(): XEvent is nullptr!");
	TRAP_ASSERT(ptr, "WindowingAPI::IsSelPropNewValueNotify(): XPointer is nullptr!");

	const XEvent* const notification = reinterpret_cast<const XEvent*>(ptr);

	return NumericCast<i32>(event->type == PropertyNotify &&
	                            event->xproperty.state == PropertyNewValue &&
		                        event->xproperty.window == notification->xselection.requestor &&
		                        event->xproperty.atom == notification->xselection.property);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModeX11(const InternalMonitor& monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
		mode = InternalVideoMode
		{
			.Width = DisplayWidth(s_Data.X11.display, s_Data.X11.Screen),
			.Height = DisplayHeight(s_Data.X11.display, s_Data.X11.Screen),
			.RefreshRate = 60.0
		};

		SplitBPP(DefaultDepth(s_Data.X11.display, s_Data.X11.Screen), mode->RedBits, mode->GreenBits, mode->BlueBits);
	}

	return *mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSizeX11(const InternalWindow& window, i32& width,
                                                            i32& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	XWindowAttributes attribs;
	s_Data.X11.XLIB.GetWindowAttributes(s_Data.X11.display, window.X11.Handle, &attribs);

	width = attribs.width;
	height = attribs.height;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPosX11(const InternalWindow& window, const i32 xPos,
                                                           const i32 yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	//HACK: Explicitly setting PPosition to any value causes some WMs, notably Compiz and Metacity, to honor
	//      the position of unmapped windows
	if(!PlatformWindowVisibleX11(window))
	{
		i64 supplied = 0;
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
                                                               InternalMonitor* const monitor, const i32 xPos,
															   const i32 yPos, const i32 width,
															   const i32 height,
															   [[maybe_unused]] const f64 refreshRate)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

			s_Data.X11.XLIB.MoveResizeWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos,
			                                 NumericCast<u32>(width), NumericCast<u32>(height));
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
		s_Data.X11.XLIB.MoveResizeWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos,
		                                 NumericCast<u32>(width), NumericCast<u32>(height));
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderlessX11(InternalWindow& window,
                                                                         InternalMonitor& monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(window.Monitor != nullptr)
		ReleaseMonitor(window);

	window.BorderlessFullscreen = true;
	window.Monitor = &monitor;
	UpdateNormalHints(window, 0, 0);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	std::vector<InternalVideoMode> result{};

	if(s_Data.X11.RandR.Available && !s_Data.X11.RandR.MonitorBroken)
	{
		XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
		XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);
		XRROutputInfo* const oi = s_Data.X11.RandR.GetOutputInfo(s_Data.X11.display, sr, monitor.X11.Output);

		if((sr != nullptr) && (ci != nullptr) && (oi != nullptr))
		{
			result.reserve(NumericCast<u32>(oi->nmode));

			u32 count = 0u;
			for(i32 i = 0; i < oi->nmode; ++i)
			{
				const XRRModeInfo* const mi = GetModeInfo(*sr, oi->modes[i]);
				if((mi == nullptr) || (mi->modeFlags & RR_Interlace) != 0)
					continue;

				const std::optional<InternalVideoMode> mode = VideoModeFromModeInfo(*mi, *ci);
				if(!mode)
					continue;

				const auto pos = std::distance(result.begin(), std::ranges::find(result, *mode));

				//Skip duplicate modes
				if(pos < count)
					continue;

				++count;
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

#if !defined(X_HAVE_UTF8_STRING)
	//HACK: If the current locale is "C" and the Xlib UTF-8 functions are unavailable, apply the environment's
	//      locale in the hope that it's both available and not "C"
	//      This is done because the "C" locale breaks wide character input, which is what we fall back on
	//      when UTF-8 support is missing
	if("C"sv == setLocale(LC_CTYPE, nullptr))
		setLocale(LC_CTYPE, "");
#endif /*!X_HAVE_UTF8_STRING*/

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
			InputError(Error::Platform_Error, fmt::format("[X11] Failed to open display: {}", display));
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
		window.X11.Handle = 0u;
	}

	if(window.X11.colormap != 0u)
	{
		s_Data.X11.XLIB.FreeColormap(s_Data.X11.display, window.X11.colormap);
		window.X11.colormap = 0u;
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShutdownX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

	if((std::get<0u>(s_Data.EmptyEventPipe) != 0) || (std::get<1u>(s_Data.EmptyEventPipe) != 0))
	{
		close(std::get<0u>(s_Data.EmptyEventPipe));
		close(std::get<1u>(s_Data.EmptyEventPipe));
	}

	s_Data.X11 = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScaleX11([[maybe_unused]] const InternalMonitor& monitor,
                                                                      f32& xScale, f32& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	xScale = s_Data.X11.ContentScaleX;
	yScale = s_Data.X11.ContentScaleY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPosX11(const InternalMonitor& monitor, i32& xPos,
                                                            i32& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(PlatformWindowVisibleX11(window))
		return;

	s_Data.X11.XLIB.MapWindow(s_Data.X11.display, window.X11.Handle);
	WaitForVisibilityNotify(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	Visual* const visual = DefaultVisual(s_Data.X11.display, s_Data.X11.Screen);
	const i32 depth = DefaultDepth(s_Data.X11.display, s_Data.X11.Screen);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
					               reinterpret_cast<const u8*>(title.data()),
					               NumericCast<i32>(title.size()));

	s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display,
	                               window.X11.Handle,
					               s_Data.X11.NET_WM_ICON_NAME,
					               s_Data.X11.UTF8_STRING,
					               8,
					               PropModeReplace,
					               reinterpret_cast<const u8*>(title.data()),
					               NumericCast<i32>(title.size()));

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursorX11(InternalCursor& cursor,
                                                                         const Image& image,
																		 const u32 xHotspot,
																		 const u32 yHotspot)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	const std::optional<Cursor> cursorHandle = CreateCursorX11(image, xHotspot, yHotspot);

	if(cursorHandle)
		cursor.X11.Handle = *cursorHandle;

	return cursor.X11.Handle != 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursorX11(InternalCursor& cursor, const CursorType& type)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(s_Data.X11.XCursor.Handle != nullptr)
	{
		const char* const theme = s_Data.X11.XCursor.GetTheme(s_Data.X11.display);
		if(theme != nullptr)
		{
			const i32 size = s_Data.X11.XCursor.GetDefaultSize(s_Data.X11.display);
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
		u32 native = 0u;

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

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursorX11(const InternalCursor& cursor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(cursor.X11.Handle != 0u)
		s_Data.X11.XLIB.FreeCursor(s_Data.X11.display, cursor.X11.Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorX11(const InternalWindow& window,
                                                        [[maybe_unused]] const InternalCursor* const cursor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(window.cursorMode != CursorMode::Normal && window.cursorMode != CursorMode::Captured)
		return;

	UpdateCursorImage(window);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorModeX11(InternalWindow& window, const CursorMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPosX11(InternalWindow& window, const f64 xPos, const f64 yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	//Store the new position so it can be recognized later
	window.X11.WarpCursorPosX = NumericCast<i32>(xPos);
	window.X11.WarpCursorPosY = NumericCast<i32>(yPos);

	s_Data.X11.XLIB.WarpPointer(s_Data.X11.display, None, window.X11.Handle, 0, 0, 0u, 0u, NumericCast<i32>(xPos),
	                            NumericCast<i32>(yPos));
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPosX11(const InternalWindow& window, f64& xPos, f64& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	::Window root = 0u, child = 0u;
	i32 rootX = 0, rootY = 0, childX = 0, childY = 0;
	u32 mask = 0u;

	s_Data.X11.XLIB.QueryPointer(s_Data.X11.display, window.X11.Handle, &root, &child, &rootX, &rootY, &childX, &childY,
	                             &mask);

	xPos = childX;
	yPos = childY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIconX11(const InternalWindow& window, const Image* const image)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(image != nullptr)
	{
		const u32 longCount = 2u + (image->GetWidth() * image->GetHeight());

		std::vector<u64> icon(longCount, 0u);
		auto targetIt = icon.begin();
		const std::span imgData = image->GetPixelData();

		*targetIt++ = image->GetWidth();
		*targetIt++ = image->GetHeight();

		for(u64 j = 0u; j < NumericCast<u64>(image->GetWidth()) * NumericCast<u64>(image->GetHeight()); ++j)
		{
			*targetIt++ = (static_cast<u64>(imgData[(j * 4u) + 0u]) << 16u) |
						  (static_cast<u64>(imgData[(j * 4u) + 1u]) <<  8u) |
						  (static_cast<u64>(imgData[(j * 4u) + 2u]) <<  0u) |
						  (static_cast<u64>(imgData[(j * 4u) + 3u]) << 24u);
		}

		//NOTE: XChangeProperty expects 32-bit values like the image data above to be
		//      places in the 32 least significant bits of individual longs. This is true
		//      even if i64(long) is 64-bit and a WM protocol calls for "packed" data.
		//      This is because of a historical mistake that then became part of the Xlib ABI.
		//      Xlib will pack these values into a regular array of 32-bit values before
		//      sending it over the wire.
		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_ICON, XA_CARDINAL, 32,
		                               PropModeReplace,	reinterpret_cast<u8*>(icon.data()),
								       NumericCast<i32>(longCount));
	}
	else
		s_Data.X11.XLIB.DeleteProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_ICON);

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPosX11(const InternalWindow& window, i32& xPos, i32& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	::Window dummy = 0u;
	i32 x = 0, y = 0;

	s_Data.X11.XLIB.TranslateCoordinates(s_Data.X11.display, window.X11.Handle, s_Data.X11.Root, 0, 0, &x, &y, &dummy);

	xPos = x;
	yPos = y;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeX11(InternalWindow& window, const i32 width, const i32 height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

		s_Data.X11.XLIB.ResizeWindow(s_Data.X11.display, window.X11.Handle,
		                             NumericCast<u32>(width), NumericCast<u32>(height));
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizableX11(const InternalWindow& window,
                                                                 [[maybe_unused]] const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	i32 width = 0, height = 0;

	PlatformGetWindowSizeX11(window, width, height);
	UpdateNormalHints(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecoratedX11(const InternalWindow& window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	struct Hints
	{
		u64 Flags = 0u;
		u64 Functions = 0u;
		u64 Decorations = 0u;
		i64 InputMode = 0;
		u64 status = 0u;
	} hints{};

	hints.Flags = MWM_HINTS_DECORATIONS; //MWM_HINTS_DECORATIONS
	hints.Decorations = enabled ? MWM_DECOR_ALL : 0; //1 = MWM_DECOR_ALL

	s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.MOTIF_WM_HINTS, s_Data.X11.MOTIF_WM_HINTS, 32,
	                               PropModeReplace, reinterpret_cast<u8*>(&hints), sizeof(hints) / sizeof(i64));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloatingX11(const InternalWindow& window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if((s_Data.X11.NET_WM_STATE == 0u) || (s_Data.X11.NET_WM_STATE_ABOVE == 0u))
		return;

	if(PlatformWindowVisibleX11(window))
	{
		const i64 action = NumericCast<i64>(enabled ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE);
		SendEventToWM(window, s_Data.X11.NET_WM_STATE, action, static_cast<i64>(s_Data.X11.NET_WM_STATE_ABOVE), 0, 1, 0);
	}
	else
	{
		Atom* states = nullptr;

		const u64 count = GetWindowPropertyX11(window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM,
		                                                 reinterpret_cast<u8**>(&states));

		//NOTE: We don't check for failure as this property may not exist yet and that's fine (and we'll create
		//      it implicitly with append)
		if(enabled)
		{
			u64 i = 0u;

			for(i = 0u; i < count; ++i)
			{
				if(states[i] == s_Data.X11.NET_WM_STATE_ABOVE)
					break;
			}

			if(i == count)
			{
				s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM, 32,
										       PropModeAppend, reinterpret_cast<u8*>(&s_Data.X11.NET_WM_STATE_ABOVE), 1);
			}
		}
		else if(states != nullptr)
		{
			for(u64 i = 0u; i < count; ++i)
			{
				if(states[i] == s_Data.X11.NET_WM_STATE_ABOVE)
				{
					states[i] = states[count - 1];
					s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM, 32,
											       PropModeReplace, reinterpret_cast<u8*>(states),
											       NumericCast<i32>(count) - 1);
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacityX11(const InternalWindow& window, const f32 opacity)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	const CARD32 value = static_cast<CARD32>(0xFFFFFFFFu * NumericCast<f64>(opacity));
	s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_WINDOW_OPACITY, XA_CARDINAL, 32,
	                               PropModeReplace, reinterpret_cast<const u8*>(&value), 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthroughX11(const InternalWindow& window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(!s_Data.X11.XShape.Available)
		return;

	if (enabled)
	{
		Region region = s_Data.X11.XLIB.CreateRegion();
		s_Data.X11.XShape.CombineRegion(s_Data.X11.display, window.X11.Handle, ShapeInput, 0, 0, region, ShapeSet);
		s_Data.X11.XLIB.DestroyRegion(region);
	}
	else
		s_Data.X11.XShape.CombineMask(s_Data.X11.display, window.X11.Handle, ShapeInput, 0, 0, None, ShapeSet);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<f32> TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacityX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	std::optional<f32> opacity = std::nullopt;

	if(s_Data.X11.XLIB.GetSelectionOwner(s_Data.X11.display, s_Data.X11.NET_WM_CM_Sx) != 0u)
	{
		CARD32* value = nullptr;

		if(GetWindowPropertyX11(window.X11.Handle, s_Data.X11.NET_WM_WINDOW_OPACITY, XA_CARDINAL,
		                        reinterpret_cast<u8**>(&value)) != 0u)
		{
			opacity = NumericCast<f32>(*value / NumericCast<f64>(0xFFFFFFFFu));
		}

		if(value != nullptr)
			s_Data.X11.XLIB.Free(value);
	}

	return opacity;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSizeX11(const InternalWindow& window, i32& width,
                                                                 i32& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	PlatformGetWindowSizeX11(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScaleX11([[maybe_unused]] const InternalWindow& window,
                                                                    f32& xScale, f32& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	xScale = s_Data.X11.ContentScaleX;
	yScale = s_Data.X11.ContentScaleY;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkAreaX11(const InternalMonitor& monitor, i32& xPos,
                                                                 i32& yPos, i32& width, i32& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	i32 areaX = 0, areaY = 0, areaWidth = 0, areaHeight = 0;

	if(s_Data.X11.RandR.Available && !s_Data.X11.RandR.MonitorBroken)
	{
		XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
		XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);

		if((sr != nullptr) && (ci != nullptr))
		{
			areaX = ci->x;
			areaY = ci->y;

			const XRRModeInfo* const mi = GetModeInfo(*sr, ci->mode);

			areaWidth = NumericCast<i32>(mi->width);
			areaHeight = NumericCast<i32>(mi->height);

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
		const u64 extentCount = GetWindowPropertyX11(s_Data.X11.Root, s_Data.X11.NET_WORKAREA, XA_CARDINAL,
		                                                  reinterpret_cast<u8**>(&extents));

		if(GetWindowPropertyX11(s_Data.X11.Root, s_Data.X11.NET_CURRENT_DESKTOP, XA_CARDINAL,
		                        reinterpret_cast<u8**>(&desktop)) > 0)
		{
			if(extentCount >= 4u && *desktop < extentCount / 4u)
			{
				const i32 globalX = static_cast<i32>(extents[*desktop * 4u + 0u]);
				const i32 globalY = static_cast<i32>(extents[*desktop * 4u + 1u]);
				const i32 globalWidth = static_cast<i32>(extents[*desktop * 4u + 2u]);
				const i32 globalHeight = static_cast<i32>(extents[*desktop * 4u + 3u]);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	XWindowAttributes wa;
	s_Data.X11.XLIB.GetWindowAttributes(s_Data.X11.display, window.X11.Handle, &wa);

	return wa.map_state == IsViewable;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximizedX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	Atom* states = nullptr;
	bool maximized = false;

	if((s_Data.X11.NET_WM_STATE == 0u) || (s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT == 0u) ||
	   (s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ == 0u))
		return maximized;

	const u64 count = GetWindowPropertyX11(window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM,
	                                            reinterpret_cast<u8**>(&states));

	for(u64 i = 0u; i < count; ++i)
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	return GetWindowState(window) == IconicState;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPollEventsX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
		i32 width = 0, height = 0;
		PlatformGetWindowSizeX11(*window, width, height);

		if(window->LastCursorPosX != width / 2 || window->LastCursorPosY != height / 2)
			PlatformSetCursorPosX11(*window, NumericCast<f64>(width) / 2, NumericCast<f64>(height) / 2);
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformWaitEventsX11(f64 timeout)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(timeout == 0.0)
		WaitForAnyEvent(nullptr);
	else
		WaitForAnyEvent(&timeout);

	PlatformPollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPostEmptyEventX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	WriteEmptyEvent();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocusedX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	::Window focused = 0u;
	i32 state = 0;

	s_Data.X11.XLIB.GetInputFocus(s_Data.X11.display, &focused, &state);

	return window.X11.Handle == focused;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHoveredX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	::Window w = s_Data.X11.Root;
	while(w != 0u)
	{
		::Window root = 0u;
		i32 rootX = 0, rootY = 0, childX = 0, childY = 0;
		u32 mask = 0u;

		GrabErrorHandlerX11();

		const i32 result = s_Data.X11.XLIB.QueryPointer(s_Data.X11.display, w, &root, &w, &rootX, &rootY, &childX,
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	return s_Data.X11.XI.Available;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotionX11(const InternalWindow& window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

[[nodiscard]] i32 TRAP::INTERNAL::WindowingAPI::PlatformGetKeyScanCodeX11(const Input::Key key)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return s_Data.ScanCodes[NumericCast<usize>(std::to_underlying(key))];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<std::string> TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeNameX11(const i32 scanCode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(!s_Data.X11.XKB.Available)
		return std::nullopt;

	if(scanCode < 0 || scanCode > 0xFF || s_Data.KeyCodes[NumericCast<u32>(scanCode)] == Input::Key::Unknown)
	{
		InputError(Error::Invalid_Value, fmt::format("Invalid scancode {}", scanCode));
		return std::nullopt;
	}

	const TRAP::Input::Key key = s_Data.KeyCodes[NumericCast<u32>(scanCode)];
	const KeySym keySym = s_Data.X11.XKB.KeycodeToKeysym(s_Data.X11.display, static_cast<KeyCode>(scanCode),
											             NumericCast<i32>(s_Data.X11.XKB.Group), 0);
	if(keySym == NoSymbol)
		return std::nullopt;

	const std::optional<u32> ch = KeySymToUnicode(static_cast<u32>(keySym));
	if(!ch)
		return std::nullopt;

	const std::string utf8Str = Utils::String::EncodeUTF8(*ch);
	if(utf8Str.empty())
		return std::nullopt;


	std::string& keyStr = s_Data.KeyNames[NumericCast<u32>(std::to_underlying(key))];
	keyStr = utf8Str;

	return keyStr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetClipboardStringX11(const std::string& string)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	s_Data.ClipboardString = string;

	s_Data.X11.XLIB.SetSelectionOwner(s_Data.X11.display, s_Data.X11.CLIPBOARD, s_Data.X11.HelperWindowHandle, CurrentTime);

	if(s_Data.X11.XLIB.GetSelectionOwner(s_Data.X11.display, s_Data.X11.CLIPBOARD) != s_Data.X11.HelperWindowHandle)
		InputError(Error::Platform_Error, "[X11] Failed to become owner of clipboard selection!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::PlatformGetClipboardStringX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	return GetSelectionString(s_Data.X11.CLIPBOARD);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetRequiredInstanceExtensionsX11(std::array<std::string, 2u>& extensions)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

		const VkXcbSurfaceCreateInfoKHR sci
		{
			.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.connection = connection,
			.window = window.X11.Handle
		};

		const VkResult err = vkCreateXcbSurfaceKHR(instance, &sci, allocator, &surface);
		if(err != 0)
			InputError(Error::Platform_Error, fmt::format("[X11] Failed to create Vulkan XCB surface: {}", GetVulkanResultString(err)));

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

	const VkXlibSurfaceCreateInfoKHR sci
	{
		.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.dpy = s_Data.X11.display,
		.window = window.X11.Handle
	};

	const VkResult err = vkCreateXlibSurfaceKHR(instance, &sci, allocator, &surface);
	if(err != 0)
	{
		InputError(Error::Platform_Error, fmt::format("[X11] Failed to create Vulkan X11 surface: {}", GetVulkanResultString(err)));
	}

	return err;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if((s_Data.X11.NET_WM_STATE == 0u) || (s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT == 0u) ||
	   (s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ == 0u))
	   return;

	if(PlatformWindowVisibleX11(window))
	{
		SendEventToWM(window, s_Data.X11.NET_WM_STATE, _NET_WM_STATE_ADD,
		              static_cast<i64>(s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT),
		              static_cast<i64>(s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ), 1, 0);
	}
	else
	{
		Atom* states = nullptr;
		const u64 count = GetWindowPropertyX11(window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM,
		                                            reinterpret_cast<u8**>(&states));

		//NOTE: We don't check for failure as this property may not exist yet and that's fine (and we'll create it
		//      implicitly with append)
		std::array<Atom, 2u> missing =
		{
			s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT,
			s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ
		};
		u64 missingCount = 2u;

		for(u64 i = 0u; i < count; ++i)
		{
			for(u64 j = 0u; j < missingCount; ++j)
			{
				if(states[i] == missing[j])
				{
					missing[j] = missing[missingCount - 1u];
					--missingCount;
				}
			}
		}

		if(states != nullptr)
			s_Data.X11.XLIB.Free(states);

		if(missingCount == 0u)
			return;

		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.NET_WM_STATE, XA_ATOM, 32, PropModeAppend,
		                               reinterpret_cast<u8*>(missing.data()), NumericCast<i32>(missingCount));
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttentionX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if((s_Data.X11.NET_WM_STATE == 0u) || (s_Data.X11.NET_WM_STATE_DEMANDS_ATTENTION == 0u))
		return;

	SendEventToWM(window, s_Data.X11.NET_WM_STATE, _NET_WM_STATE_ADD,
	              static_cast<i64>(s_Data.X11.NET_WM_STATE_DEMANDS_ATTENTION), 0, 1, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	s_Data.X11.XLIB.UnmapWindow(s_Data.X11.display, window.X11.Handle);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindowX11(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
			              static_cast<i64>(s_Data.X11.NET_WM_STATE_MAXIMIZED_VERT),
			              static_cast<i64>(s_Data.X11.NET_WM_STATE_MAXIMIZED_HORZ), 1, 0);
		}
	}

	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimitsX11(const InternalWindow& window,
                                                                  [[maybe_unused]] const i32 minWidth,
                                                                  [[maybe_unused]] const i32 minHeight,
															      [[maybe_unused]] const i32 maxWidth,
                                                                  [[maybe_unused]] const i32 maxHeight)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	i32 width = 0, height = 0;
	PlatformGetWindowSizeX11(window, width, height);
	UpdateNormalHints(window, width, height);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowAspectRatioX11(const InternalWindow& window,
                                                                   [[maybe_unused]] const i32 numerator,
                                                                   [[maybe_unused]] const i32 denominator)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	i32 width = 0, height = 0;
	PlatformGetWindowSizeX11(window, width, height);
	UpdateNormalHints(window, width, height);
	s_Data.X11.XLIB.Flush(s_Data.X11.display);
}

//-------------------------------------------------------------------------------------------------------------------//

//Enable XI2 raw mouse motion events
void TRAP::INTERNAL::WindowingAPI::EnableRawMouseMotion([[maybe_unused]] const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	std::array<u8, XIMaskLen(XI_RawMotion)> mask{};

	XIEventMask em
	{
		.deviceid = XIAllMasterDevices,
		.mask_len = mask.size(),
		.mask = mask.data()
	};
	XISetMask(mask.data(), XI_RawMotion);

	s_Data.X11.XI.SelectEvents(s_Data.X11.display, s_Data.X11.Root, &em, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Disable XI2 raw mouse motion events
void TRAP::INTERNAL::WindowingAPI::DisableRawMouseMotion([[maybe_unused]] const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	std::array<u8, 1u> mask{};

	XIEventMask em
	{
		.deviceid = XIAllMasterDevices,
		.mask_len = mask.size(),
		.mask = mask.data()
	};

	s_Data.X11.XI.SelectEvents(s_Data.X11.display, s_Data.X11.Root, &em, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Reports the specified error, appending information about the last X Error
void TRAP::INTERNAL::WindowingAPI::InputErrorX11(const Error error, const std::string& message)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	std::string buffer(1024u, '\0');
	s_Data.X11.XLIB.GetErrorText(s_Data.X11.display, s_Data.X11.ErrorCode, buffer.data(), NumericCast<i32>(buffer.size()));

	std::erase(buffer, '\0');

	InputError(error, fmt::format("[X11] {}: {}", message, buffer));
}

//-------------------------------------------------------------------------------------------------------------------//

//Process the specified X event
void TRAP::INTERNAL::WindowingAPI::ProcessEvent(XEvent& event)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	//HACK: Save scanCode as some IMs clear the field in s_Data.XLIB.FilterEvent
	i32 keyCode = 0;
	if(event.type == KeyPress || event.type == KeyRelease)
		keyCode = NumericCast<i32>(event.xkey.keycode);

	const bool filtered = (s_Data.X11.XLIB.FilterEvent(&event, 0) != 0);

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
				s_Data.X11.XKB.Group = NumericCast<u32>(reinterpret_cast<XkbEvent*>(&event)->state.group);
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
					const f64* values = re->raw_values;
					f64 xPos = window->VirtualCursorPosX;
					f64 yPos = window->VirtualCursorPosY;

					if(XIMaskIsSet(re->valuators.mask, 0))
					{
						xPos += *values;
						++values;
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
			const Time diff = event.xkey.time - window->X11.KeyPressTimes[NumericCast<u32>(keyCode)];
			if(diff == event.xkey.time || (diff > 0u && diff < (static_cast<Time>(1u) << 31u)))
			{
				if(keyCode != 0)
					InputKey(*window, key, keyCode, Input::KeyState::Pressed);

				window->X11.KeyPressTimes[NumericCast<u32>(keyCode)] = event.xkey.time;
			}

			if(!filtered)
			{
				i32 status = 0;

				std::string buffer(100, '\0');
				char* chars = buffer.data();

				i32 count = s_Data.X11.XLIB.UTF8LookupString(window->X11.IC, &event.xkey, buffer.data(),
				                                         NumericCast<i32>(buffer.size()) - 1, nullptr, &status);

				if(status == XBufferOverflow)
				{
					buffer.resize(NumericCast<u32>(count) + 1u);
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

			InputKey(*window, key, keyCode, Input::KeyState::Pressed);

			const std::optional<u32> character = KeySymToUnicode(static_cast<u32>(keySym));
			if(character)
				InputChar(*window, *character);
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
					std::cmp_equal(next.xkey.keycode, keyCode))
				{
					//HACK: The time of repeat events sometimes doesn't match that of the press event, so add an epsilon
					if((next.xkey.time - event.xkey.time) < 20u)
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
		const i32 x = event.xcrossing.x;
		const i32 y = event.xcrossing.y;

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
		const i32 x = event.xmotion.x;
		const i32 y = event.xmotion.y;

		if(x != window->X11.WarpCursorPosX || y != window->X11.WarpCursorPosY)
		{
			//The cursor was moved by something other than TRAP
			if(window->cursorMode == CursorMode::Disabled)
			{
				if(s_Data.DisabledCursorWindow != window)
					return;
				if(window->RawMouseMotion)
					return;

				const i32 dx = x - window->LastCursorPosX;
				const i32 dy = y - window->LastCursorPosY;

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

		i32 xPos = event.xconfigure.x;
		i32 yPos = event.xconfigure.y;

		//NOTE: ConfigureNotify events from the server are in local
		//      coordinates, so if any reparented we need to translate
		//      the position into root (screen) coordinates
		if((event.xany.send_event == 0) && window->X11.Parent != s_Data.X11.Root)
		{
			GrabErrorHandlerX11();

			::Window dummy = 0u;
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
			const Atom protocol = static_cast<Atom>(event.xclient.data.l[0u]);
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
			u64 count = 0u;
			Atom* formats = nullptr;
			const bool list = (event.xclient.data.l[1u] & 1) != 0;

			s_Data.X11.XDND.Source = static_cast<::Window>(event.xclient.data.l[0u]);
			s_Data.X11.XDND.Version = NumericCast<i32>(event.xclient.data.l[1u] >> 24u);
			s_Data.X11.XDND.Format = None;

			if(s_Data.X11.XDND.Version > TRAP_XDND_VERSION)
				return;

			if(list)
			{
				count = GetWindowPropertyX11(s_Data.X11.XDND.Source, s_Data.X11.XDNDTypeList, XA_ATOM,
				                             reinterpret_cast<u8**>(&formats));
			}
			else
			{
				count = 3u;
				formats = reinterpret_cast<Atom*>(event.xclient.data.l) + 2u;
			}

			for(u64 i = 0u; i < count; ++i)
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
					time = static_cast<Time>(event.xclient.data.l[2u]);

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
				reply.xclient.data.l[0u] = static_cast<i64>(window->X11.Handle);
				reply.xclient.data.l[1u] = 0; //The drag was rejected
				reply.xclient.data.l[2u] = None;

				s_Data.X11.XLIB.SendEvent(s_Data.X11.display, s_Data.X11.XDND.Source, 0, NoEventMask, &reply);
				s_Data.X11.XLIB.Flush(s_Data.X11.display);
			}
		}
		else if(event.xclient.message_type == s_Data.X11.XDNDPosition)
		{
			//The drag operation has moved over the window
			const i32 xAbs = NumericCast<i32>((event.xclient.data.l[2u] >> 16) & 0xFFFFu);
			const i32 yAbs = NumericCast<i32>((event.xclient.data.l[2u]) & 0xFFFFu);
			::Window dummy = 0u;
			i32 xPos = 0, yPos = 0;

			if(s_Data.X11.XDND.Version > TRAP_XDND_VERSION)
				return;

			s_Data.X11.XLIB.TranslateCoordinates(s_Data.X11.display, s_Data.X11.Root, window->X11.Handle, xAbs, yAbs, &xPos, &yPos,
			                                     &dummy);

			InputCursorPos(*window, xPos, yPos);

			XEvent reply = {ClientMessage};
			reply.xclient.window = s_Data.X11.XDND.Source;
			reply.xclient.message_type = s_Data.X11.XDNDStatus;
			reply.xclient.format = 32;
			reply.xclient.data.l[0u] = static_cast<i64>(window->X11.Handle);
			reply.xclient.data.l[2u] = 0; //Specify an empty rectangle
			reply.xclient.data.l[3u] = 0;

			if(s_Data.X11.XDND.Format != 0u)
			{
				//Reply that we are ready to copy the dragged data
				reply.xclient.data.l[1u] = 1; //Accept with no rectangle
				if(s_Data.X11.XDND.Version >= 2)
					reply.xclient.data.l[4u] = static_cast<i64>(s_Data.X11.XDNDActionCopy);
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
			const u64 result = GetWindowPropertyX11(event.xselection.requestor,
														 event.xselection.property,
			                                             event.xselection.target,
														 reinterpret_cast<u8**>(&data));

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
				reply.xclient.data.l[0u] = static_cast<i64>(window->X11.Handle);
				reply.xclient.data.l[1u] = static_cast<i64>(result);
				reply.xclient.data.l[2u] = static_cast<i64>(s_Data.X11.XDNDActionCopy);

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
			const i32 state = GetWindowState(*window);
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
[[nodiscard]] u32 TRAP::INTERNAL::WindowingAPI::DecodeUTF8(const char** const str)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	TRAP_ASSERT(str, "WindowingAPI::DecodeUTF8(): Unicode codepoint is nullptr!");

	u32 ch = 0u, count = 0u;
	static constexpr std::array<u32, 6u> offsets =
	{
		0x00000000u, 0x00003080u, 0x000E2080u,
		0x03C82080u, 0xFA082080u, 0x82082080u
	};

	do
	{
		ch = (ch << 6u) + static_cast<u8>(**str);
		++(*str);
		++count;
	} while((NumericCast<u8>(**str) & 0xC0u) == 0x80u);

	TRAP_ASSERT(count <= 6u, "WindowingAPI::DecodeUTF8(): Invalid UTF-8 sequence!");

	return ch - offsets[count - 1u];
}

//-------------------------------------------------------------------------------------------------------------------//

//Make the specified window and its video mode active on its monitor
void TRAP::INTERNAL::WindowingAPI::AcquireMonitor(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(s_Data.X11.Saver.Count == 0)
	{
		//Remember old screen saver settings
		s_Data.X11.XLIB.GetScreenSaver(s_Data.X11.display, &s_Data.X11.Saver.Timeout, &s_Data.X11.Saver.Interval,
		                               &s_Data.X11.Saver.Blanking, &s_Data.X11.Saver.Exposure);

		//Disable screen saver
		s_Data.X11.XLIB.SetScreenSaver(s_Data.X11.display, 0, 0, DontPreferBlanking, DefaultExposures);
	}

	if(window.Monitor->Window == nullptr)
		++s_Data.X11.Saver.Count;

	SetVideoModeX11(*window.Monitor, window.videoMode);

	if(window.X11.OverrideRedirect)
	{
		i32 xPos = 0, yPos = 0;

		//Manually position the window over its monitor
		if(window.Monitor != nullptr)
		{
			PlatformGetMonitorPosX11(*window.Monitor, xPos, yPos);
			const std::optional<InternalVideoMode> mode = PlatformGetVideoModeX11(*window.Monitor);
			if(mode)
				s_Data.X11.XLIB.MoveResizeWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos,
				                                 NumericCast<u32>(mode->Width),
												 NumericCast<u32>(mode->Height));
		}
	}

	window.Monitor->Window = &window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Remove the window and restore the original video mode
void TRAP::INTERNAL::WindowingAPI::ReleaseMonitor(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(window.Monitor->Window != &window)
		return;

	window.Monitor->Window = nullptr;
	RestoreVideoModeX11(*window.Monitor);

	--s_Data.X11.Saver.Count;

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(!s_Data.X11.RandR.Available || s_Data.X11.RandR.MonitorBroken)
		return;

	RRMode native = 0u;

	const std::optional<InternalVideoMode> best = ChooseVideoMode(monitor, desired);
	const std::optional<InternalVideoMode> current = PlatformGetVideoModeX11(monitor);
	if(!current || !best || !CompareVideoModes(*current, *best))
		return;

	XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
	XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);
	XRROutputInfo* const oi = s_Data.X11.RandR.GetOutputInfo(s_Data.X11.display, sr, monitor.X11.Output);

	if((sr != nullptr) && (ci != nullptr) && (oi != nullptr))
	{
		for(i32 i = 0; i < oi->nmode; ++i)
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
			if(monitor.X11.OldMode == 0u)
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(!s_Data.X11.RandR.Available || s_Data.X11.RandR.MonitorBroken)
		return;

	if(monitor.X11.OldMode == 0u)
		return;

	XRRScreenResources* const sr = s_Data.X11.RandR.GetScreenResourcesCurrent(s_Data.X11.display, s_Data.X11.Root);
	XRRCrtcInfo* const ci = s_Data.X11.RandR.GetCrtcInfo(s_Data.X11.display, sr, monitor.X11.CRTC);

	if((sr != nullptr) && (ci != nullptr))
	{
		s_Data.X11.RandR.SetCrtcConfig(s_Data.X11.display, sr, monitor.X11.CRTC, CurrentTime, ci->x, ci->y, monitor.X11.OldMode,
									ci->rotation, ci->outputs, ci->noutput);

		s_Data.X11.RandR.FreeCrtcInfo(ci);
		s_Data.X11.RandR.FreeScreenResources(sr);

		monitor.X11.OldMode = 0u;
	}
	else
		InputErrorX11(Error::Platform_Error, "[X11] Failed to restore video mode");
}

//-------------------------------------------------------------------------------------------------------------------//

//Make the specified window active on its monitor
void TRAP::INTERNAL::WindowingAPI::AcquireMonitorBorderless(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(s_Data.X11.Saver.Count == 0)
	{
		//Remember old screen saver settings
		s_Data.X11.XLIB.GetScreenSaver(s_Data.X11.display, &s_Data.X11.Saver.Timeout, &s_Data.X11.Saver.Interval,
		                               &s_Data.X11.Saver.Blanking, &s_Data.X11.Saver.Exposure);

		//Disable screen saver
		s_Data.X11.XLIB.SetScreenSaver(s_Data.X11.display, 0, 0, DontPreferBlanking, DefaultExposures);
	}

	if(window.Monitor->Window == nullptr)
		++s_Data.X11.Saver.Count;

	if(window.X11.OverrideRedirect)
	{
		i32 xPos = 0, yPos = 0;

		//Manually position the window over its monitor
		if(window.Monitor != nullptr)
		{
			PlatformGetMonitorPosX11(*window.Monitor, xPos, yPos);
			const std::optional<InternalVideoMode> mode = window.Monitor->NativeMode;
			if(mode)
				s_Data.X11.XLIB.MoveResizeWindow(s_Data.X11.display, window.X11.Handle, xPos, yPos,
				                                 NumericCast<u32>(mode->Width),
												 NumericCast<u32>(mode->Height));
		}
	}

	window.Monitor->Window = &window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a dummy window for behind-the-scenes work
[[nodiscard]] ::Window TRAP::INTERNAL::WindowingAPI::CreateHelperWindow()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	XSetWindowAttributes wa{};
	wa.event_mask = PropertyChangeMask;

	return s_Data.X11.XLIB.CreateWindow(s_Data.X11.display, s_Data.X11.Root, 0, 0, 1, 1, 0, 0, InputOnly,
	                                    DefaultVisual(s_Data.X11.display, s_Data.X11.Screen), CWEventMask, &wa);
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the cursor image according to its cursor mode
void TRAP::INTERNAL::WindowingAPI::UpdateCursorImage(const InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	TRAP_ASSERT(clientData, "WindowingAPI::InputContextDestroyCallback(): XPointer is nullptr!");

	InternalWindow* const window = reinterpret_cast<InternalWindow*>(clientData);
	if(window != nullptr)
		window->X11.IC = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Allocates and returns a monitor object with the specified name and dimensions
[[nodiscard]] TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalMonitor> TRAP::INTERNAL::WindowingAPI::CreateMonitor(std::string name)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None &&
	                                              (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	Scope<InternalMonitor> monitor = MakeScope<InternalMonitor>();
	if(!name.empty())
		monitor->Name = std::move(name);

	return monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create key code translation tables
void TRAP::INTERNAL::WindowingAPI::CreateKeyTablesX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	i32 scanCode = 0, scanCodeMin = 0, scanCodeMax = 0;

	std::ranges::fill(s_Data.KeyCodes, Input::Key::Unknown);
	std::ranges::fill(s_Data.ScanCodes, -1);

	if(s_Data.X11.XKB.Available)
	{
		//Use XKB to determine physical key locations independently of the current keyboard layout

		XkbDescPtr desc = s_Data.X11.XKB.GetMap(s_Data.X11.display, 0u, XkbUseCoreKbd);
		s_Data.X11.XKB.GetNames(s_Data.X11.display, XkbKeyNamesMask | XkbKeyAliasesMask, desc);

		scanCodeMin = desc->min_key_code;
		scanCodeMax = desc->max_key_code;

		struct Keys
		{
			TRAP::Input::Key Key = TRAP::Input::Key::Unknown;
			std::string Name;
		};

		static const std::array<Keys, 121u> KeyMap =
		{
            {
                { .Key=TRAP::Input::Key::Grave_Accent, .Name="TLDE"}, { .Key=TRAP::Input::Key::One, .Name="AE01"},
                { .Key=TRAP::Input::Key::Two, .Name="AE02"}, { .Key=TRAP::Input::Key::Three, .Name="AE03"},
                { .Key=TRAP::Input::Key::Four, .Name="AE04"}, { .Key=TRAP::Input::Key::Five, .Name="AE05"},
                { .Key=TRAP::Input::Key::Six, .Name="AE06"}, { .Key=TRAP::Input::Key::Seven, .Name="AE07"},
                { .Key=TRAP::Input::Key::Eight, .Name="AE08"}, { .Key=TRAP::Input::Key::Nine, .Name="AE09"},
                { .Key=TRAP::Input::Key::Zero, .Name="AE10"}, { .Key=TRAP::Input::Key::Minus, .Name="AE11"},
                { .Key=TRAP::Input::Key::Equal, .Name="AE12"}, { .Key=TRAP::Input::Key::Q, .Name="AD01"},
                { .Key=TRAP::Input::Key::W, .Name="AD02"}, { .Key=TRAP::Input::Key::E, .Name="AD03"},
                { .Key=TRAP::Input::Key::R, .Name="AD04"}, { .Key=TRAP::Input::Key::T, .Name="AD05"},
                { .Key=TRAP::Input::Key::Y, .Name="AD06"}, { .Key=TRAP::Input::Key::U, .Name="AD07"},
                { .Key=TRAP::Input::Key::I, .Name="AD08"}, { .Key=TRAP::Input::Key::O, .Name="AD09"},
                { .Key=TRAP::Input::Key::P, .Name="AD10"}, { .Key=TRAP::Input::Key::Left_Bracket, .Name="AD11"},
                { .Key=TRAP::Input::Key::Right_Bracket, .Name="AD12"}, { .Key=TRAP::Input::Key::A, .Name="AC01"},
                { .Key=TRAP::Input::Key::S, .Name="AC02"}, { .Key=TRAP::Input::Key::D, .Name="AC03"},
                { .Key=TRAP::Input::Key::F, .Name="AC04"}, { .Key=TRAP::Input::Key::G, .Name="AC05"},
                { .Key=TRAP::Input::Key::H, .Name="AC06"}, { .Key=TRAP::Input::Key::J, .Name="AC07"},
                { .Key=TRAP::Input::Key::K, .Name="AC08"}, { .Key=TRAP::Input::Key::L, .Name="AC09"},
                { .Key=TRAP::Input::Key::Semicolon, .Name="AC10"}, { .Key=TRAP::Input::Key::Apostrophe, .Name="AC11"},
                { .Key=TRAP::Input::Key::Z, .Name="AB01"}, { .Key=TRAP::Input::Key::X, .Name="AB02"},
                { .Key=TRAP::Input::Key::C, .Name="AB03"}, { .Key=TRAP::Input::Key::V, .Name="AB04"},
                { .Key=TRAP::Input::Key::B, .Name="AB05"}, { .Key=TRAP::Input::Key::N, .Name="AB06"},
                { .Key=TRAP::Input::Key::M, .Name="AB07"}, { .Key=TRAP::Input::Key::Comma, .Name="AB08"},
                { .Key=TRAP::Input::Key::Period, .Name="AB09"}, { .Key=TRAP::Input::Key::Slash, .Name="AB10"},
                { .Key=TRAP::Input::Key::Backslash, .Name="BKSL"}, { .Key=TRAP::Input::Key::World_1, .Name="LSGT"},
                { .Key=TRAP::Input::Key::Space, .Name="SPCE"}, { .Key=TRAP::Input::Key::Escape, .Name="ESC"},
                { .Key=TRAP::Input::Key::Enter, .Name="RTRN"}, { .Key=TRAP::Input::Key::Tab, .Name="TAB"},
                { .Key=TRAP::Input::Key::Backspace, .Name="BKSP"}, { .Key=TRAP::Input::Key::Insert, .Name="INS"},
                { .Key=TRAP::Input::Key::Delete, .Name="DELE"}, { .Key=TRAP::Input::Key::Right, .Name="RGHT"},
                { .Key=TRAP::Input::Key::Left, .Name="LEFT"}, { .Key=TRAP::Input::Key::Down, .Name="DOWN"},
                { .Key=TRAP::Input::Key::Up, .Name="UP"}, { .Key=TRAP::Input::Key::Page_Up, .Name="PGUP"},
                { .Key=TRAP::Input::Key::Page_Down, .Name="PGDN"}, { .Key=TRAP::Input::Key::Home, .Name="HOME"},
                { .Key=TRAP::Input::Key::End, .Name="END"}, { .Key=TRAP::Input::Key::Caps_Lock, .Name="CAPS"},
                { .Key=TRAP::Input::Key::Scroll_Lock, .Name="SCLK"}, { .Key=TRAP::Input::Key::Num_Lock, .Name="NMLK"},
                { .Key=TRAP::Input::Key::Print_Screen, .Name="PRSC"}, { .Key=TRAP::Input::Key::Pause, .Name="PAUS"},
                { .Key=TRAP::Input::Key::F1, .Name="FK01"}, { .Key=TRAP::Input::Key::F2, .Name="FK02"},
                { .Key=TRAP::Input::Key::F3, .Name="FK03"}, { .Key=TRAP::Input::Key::F4, .Name="FK04"},
                { .Key=TRAP::Input::Key::F5, .Name="FK05"}, { .Key=TRAP::Input::Key::F6, .Name="FK06"},
                { .Key=TRAP::Input::Key::F7, .Name="FK07"}, { .Key=TRAP::Input::Key::F8, .Name="FK08"},
                { .Key=TRAP::Input::Key::F9, .Name="FK09"}, { .Key=TRAP::Input::Key::F10, .Name="FK10"},
                { .Key=TRAP::Input::Key::F11, .Name="FK11"}, { .Key=TRAP::Input::Key::F12, .Name="FK12"},
                { .Key=TRAP::Input::Key::F13, .Name="FK13"}, { .Key=TRAP::Input::Key::F14, .Name="FK14"},
                { .Key=TRAP::Input::Key::F15, .Name="FK15"}, { .Key=TRAP::Input::Key::F16, .Name="FK16"},
                { .Key=TRAP::Input::Key::F17, .Name="FK17"}, { .Key=TRAP::Input::Key::F18, .Name="FK18"},
                { .Key=TRAP::Input::Key::F19, .Name="FK19"}, { .Key=TRAP::Input::Key::F20, .Name="FK20"},
                { .Key=TRAP::Input::Key::F21, .Name="FK21"}, { .Key=TRAP::Input::Key::F22, .Name="FK22"},
                { .Key=TRAP::Input::Key::F23, .Name="FK23"}, { .Key=TRAP::Input::Key::F24, .Name="FK24"},
                { .Key=TRAP::Input::Key::F25, .Name="FK25"}, { .Key=TRAP::Input::Key::KP_0, .Name="KP0"},
                { .Key=TRAP::Input::Key::KP_1, .Name="KP1"}, { .Key=TRAP::Input::Key::KP_2, .Name="KP2"},
                { .Key=TRAP::Input::Key::KP_3, .Name="KP3"}, { .Key=TRAP::Input::Key::KP_4, .Name="KP4"},
                { .Key=TRAP::Input::Key::KP_5, .Name="KP5"}, { .Key=TRAP::Input::Key::KP_6, .Name="KP6"},
                { .Key=TRAP::Input::Key::KP_7, .Name="KP7"}, { .Key=TRAP::Input::Key::KP_8, .Name="KP8"},
                { .Key=TRAP::Input::Key::KP_9, .Name="KP9"}, { .Key=TRAP::Input::Key::KP_Decimal, .Name="KPDL"},
                { .Key=TRAP::Input::Key::KP_Divide, .Name="KPDV"}, { .Key=TRAP::Input::Key::KP_Multiply, .Name="KPMU"},
                { .Key=TRAP::Input::Key::KP_Subtract, .Name="KPSU"}, { .Key=TRAP::Input::Key::KP_Add, .Name="KPAD"},
                { .Key=TRAP::Input::Key::KP_Enter, .Name="KPEN"}, { .Key=TRAP::Input::Key::KP_Equal, .Name="KPEQ"},
                { .Key=TRAP::Input::Key::Left_Shift, .Name="LFSH"}, { .Key=TRAP::Input::Key::Left_Control, .Name="LCTL"},
                { .Key=TRAP::Input::Key::Left_ALT, .Name="LALT"}, { .Key=TRAP::Input::Key::Left_Super, .Name="LWIN"},
                { .Key=TRAP::Input::Key::Right_Shift, .Name="RTSH"}, { .Key=TRAP::Input::Key::Right_Control, .Name="RCTL"},
                { .Key=TRAP::Input::Key::Right_ALT, .Name="RALT"}, { .Key=TRAP::Input::Key::Right_ALT, .Name="LVL3"},
				{ .Key=TRAP::Input::Key::Right_ALT, .Name="MDSW"}, { .Key=TRAP::Input::Key::Right_Super, .Name="RWIN"},
                { .Key=TRAP::Input::Key::Menu, .Name="MENU"}
            }
		};

		//Find the X11 key code -> TRAP key code mapping
		for(scanCode = scanCodeMin; scanCode <= scanCodeMax; ++scanCode)
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
			for(u8 i = 0u; i < desc->names->num_key_aliases; ++i)
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

			s_Data.KeyCodes[NumericCast<u32>(scanCode)] = key;
		}

		s_Data.X11.XKB.FreeNames(desc, XkbKeyNamesMask, 1);
		s_Data.X11.XKB.FreeKeyboard(desc, 0u, 1);
	}
	else
		s_Data.X11.XLIB.DisplayKeycodes(s_Data.X11.display, &scanCodeMin, &scanCodeMax);

	i32 width = 0;
	KeySym* const keySyms = s_Data.X11.XLIB.GetKeyboardMapping(s_Data.X11.display, NumericCast<u8>(scanCodeMin), scanCodeMax - scanCodeMin + 1,
	                                                           &width);

	for(scanCode = scanCodeMin; scanCode <= scanCodeMax; ++scanCode)
	{
		//Translate the un-translated key codes using traditional X11 KeySym lookups
		if(std::to_underlying(s_Data.KeyCodes[NumericCast<u32>(scanCode)]) < 0)
		{
			const usize base = NumericCast<usize>((scanCode - scanCodeMin)) * NumericCast<usize>(width);
			const std::vector<KeySym> keySymsVec(&keySyms[base], &keySyms[base] + width);
			s_Data.KeyCodes[NumericCast<u32>(scanCode)] = TranslateKeySyms(keySymsVec).value_or(TRAP::Input::Key::Unknown);
		}

		//Store the reverse translation for faster key name lookup
		if(std::to_underlying(s_Data.KeyCodes[NumericCast<u32>(scanCode)]) > 0)
			s_Data.ScanCodes[NumericCast<u32>(std::to_underlying(s_Data.KeyCodes[NumericCast<u32>(scanCode)]))] = NumericCast<i16>(scanCode);
	}

	s_Data.X11.XLIB.Free(keySyms);
}

//-------------------------------------------------------------------------------------------------------------------//

//Grabs the cursor and confines it to the window
void TRAP::INTERNAL::WindowingAPI::CaptureCursor(InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	s_Data.X11.XLIB.UngrabPointer(s_Data.X11.display, CurrentTime);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<std::string> TRAP::INTERNAL::WindowingAPI::GetLinuxKeyboardLayoutNameX11()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if (!s_Data.X11.XKB.Available)
	{
		InputError(Error::API_Unavailable, "[Input][X11] XKB extension required for keyboard layout names");
		return std::nullopt;
	}

	XkbStateRec state{};
	s_Data.X11.XKB.GetState(s_Data.X11.display, XkbUseCoreKbd, &state);

	XkbDescPtr desc = s_Data.X11.XKB.AllocKeyboard();
	if (s_Data.X11.XKB.GetNames(s_Data.X11.display, XkbGroupNamesMask, desc) != 0) //0 = Success
	{
		s_Data.X11.XKB.FreeKeyboard(desc, 0u, 1);
		InputError(Error::Platform_Error, "[Input][X11] Failed to retrieve keyboard layout names");
		return std::nullopt;
	}

	const Atom atom = desc->names->groups[state.group];
	s_Data.X11.XKB.FreeKeyboard(desc, 0u, 1);

	if (atom == 0u)
	{
		InputError(Error::Platform_Error, "[Input][X11] Name missing for current keyboard layout");
		return std::nullopt;
	}

	return s_Data.X11.XLIB.GetAtomName(s_Data.X11.display, atom);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDropX11(const InternalWindow& window, const bool value)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::WindowingAPI) != ProfileSystems::None);

	if(value)
	{
		//Announce support for Xdnd (drag and drop)
		static constexpr Atom version = 5u;
		s_Data.X11.XLIB.ChangeProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.XDNDAware, XA_ATOM, 32,
			                       PropModeReplace, reinterpret_cast<const u8*>(&version), 1);
	}
	else
		s_Data.X11.XLIB.DeleteProperty(s_Data.X11.display, window.X11.Handle, s_Data.X11.XDNDAware);
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_PLATFORM_LINUX*/
