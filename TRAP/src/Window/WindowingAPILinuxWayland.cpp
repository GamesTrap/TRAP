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

//Note: Versions of wayland-scanner prior to 1.17.91 named every global array of
//      wl_interface pointers "types", making it impossible to combine several unmodified
//      private-code files into a single compilation unit
//HACK: We override this name with a macro for each file, allowing them to coexist
#define types _TRAP_wayland_types
#include "wayland-client-protocol-code.h"
#undef types

#define types _TRAP_xdg_shell_types
#include "wayland-xdg-shell-client-protocol-code.h"
#undef types

#define types _TRAP_xdg_decoration_types
#include "wayland-xdg-decoration-client-protocol-code.h"
#undef types

#define types _TRAP_viewporter_types
#include "wayland-viewporter-client-protocol-code.h"
#undef types

#define types _TRAP_relative_pointer_types
#include "wayland-relative-pointer-unstable-v1-client-protocol-code.h"
#undef types

#define types _TRAP_pointer_constraints_types
#include "wayland-pointer-constraints-unstable-v1-client-protocol-code.h"
#undef types

#define types _TRAP_idle_inhibit_types
#include "wayland-idle-inhibit-unstable-v1-client-protocol-code.h"
#undef types

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleDataOffer(void* userData, wl_data_device* device, wl_data_offer* offer)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleEnter(void* userData, wl_data_device* device, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y, wl_data_offer* offer)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleLeave(void* userData, wl_data_device* device)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleMotion(void* userData, wl_data_device* device, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleDrop(void* userData, wl_data_device* device)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleSelection(void* userData, wl_data_device* device, wl_data_offer* offer)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleGeometry(void* userData, wl_output* output, int32_t x, int32_t y, int32_t physicalWidth, int32_t physicalHeight, int32_t subpixel, const char* make, const char* model, int32_t transform)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleMode(void* userData, wl_output* output, uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleDone(void* userData, wl_output* output)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleScale(void* userData, wl_output* output, int32_t factor)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
void TRAP::INTERNAL::WindowingAPI::OutputHandleName(void* userData, wl_output* output, const char* name)
{
    //TODO
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
void TRAP::INTERNAL::WindowingAPI::OutputHandleDescription(void* userData, wl_output* output, const char* description)
{
    //TODO
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleKeymap(void* userData, wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleEnter(void* userData, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleLeave(void* userData, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleKey(void* userData, wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t scanCode, uint32_t state)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleModifiers(void* userData, wl_keyboard* keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
void TRAP::INTERNAL::WindowingAPI::KeyboardHandleRepeatInfo(void* userData, wl_keyboard* keyboard, int32_t rate, int32_t delay)
{
    //TODO
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleEnter(void* userData, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t sX, wl_fixed_t sY)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleLeave(void* userData, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleMotion(void* userData, wl_pointer* pointer, uint32_t time, wl_fixed_t sX, wl_fixed_t sY)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleButton(void* userData, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleAxis(void* userData, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SeatHandleCapabilities(void* /*userData*/, wl_seat* seat, const uint32_t caps)
{
    if((caps & WL_SEAT_CAPABILITY_POINTER) && !s_Data.Wayland.Pointer)
    {
        s_Data.Wayland.Pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(s_Data.Wayland.Pointer, &PointerListener, nullptr);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_POINTER) && s_Data.Wayland.Pointer)
    {
        wl_pointer_destroy(s_Data.Wayland.Pointer);
        s_Data.Wayland.Pointer = nullptr;
    }

    if((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !s_Data.Wayland.Keyboard)
    {
        s_Data.Wayland.Keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(s_Data.Wayland.Keyboard, &KeyboardListener, nullptr);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && s_Data.Wayland.Keyboard)
    {
        wl_keyboard_destroy(s_Data.Wayland.Keyboard);
        s_Data.Wayland.Keyboard = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SeatHandleName(void* /*userData*/, wl_seat* /*seat*/, const char* /*name*/)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::WMBaseHandlePing(void* /*userData*/, xdg_wm_base* wmBase, uint32_t serial)
{
    xdg_wm_base_pong(wmBase, serial);
}

//-------------------------------------------------------------------------------------------------------------------//

using namespace std::string_view_literals;

void TRAP::INTERNAL::WindowingAPI::RegistryHandleGlobal(void* /*userData*/, wl_registry* registry, uint32_t name,
                                                        const char* interface, uint32_t version)
{
    if(interface == "wl_compositor"sv)
    {
        s_Data.Wayland.CompositorVersion = TRAP::Math::Min(3u, version);
        s_Data.Wayland.Compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, s_Data.Wayland.CompositorVersion));
    }
    else if(interface == "wl_subcompositor"sv)
        s_Data.Wayland.SubCompositor = static_cast<wl_subcompositor*>(wl_registry_bind(registry, name, &wl_subcompositor_interface, 1));
    else if(interface == "wl_shm"sv)
        s_Data.Wayland.Shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, 1));
    else if(interface == "wl_output"sv)
    {
        if(version < 2)
        {
            InputError(Error::Platform_Error, "[Wayland] Unsupported output interface version");
            return;
        }

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
        version = TRAP::Math::Min(static_cast<int32_t>(version), WL_OUTPUT_NAME_SINCE_VERSION);
#else
        version = 2;
#endif

        wl_output* output = static_cast<wl_output*>(wl_registry_bind(s_Data.Wayland.Registry, name, &wl_output_interface, version));
        if(!output)
            return;

        //BUG Does this work or do we have a dangling pointer after this scope?!
        Scope<InternalMonitor> monitor = std::make_unique<InternalMonitor>();

        //The actual name of this output will be set in the geometry handler
        wl_output_add_listener(output, &OutputListener, &monitor);
    }
    else if(interface == "wl_seat"sv)
    {
        if(!s_Data.Wayland.Seat)
        {
            s_Data.Wayland.SeatVersion = TRAP::Math::Min(4u, version);
            s_Data.Wayland.Seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, s_Data.Wayland.SeatVersion));
            wl_seat_add_listener(s_Data.Wayland.Seat, &SeatListener, nullptr);
        }
    }
    else if(interface == "wl_data_device_manager"sv)
    {
        if(!s_Data.Wayland.DataDeviceManager)
            s_Data.Wayland.DataDeviceManager = static_cast<wl_data_device_manager*>(wl_registry_bind(registry, name, &wl_data_device_manager_interface, 1));
    }
    else if(interface == "xdg_wm_base"sv)
    {
        s_Data.Wayland.WMBase = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(s_Data.Wayland.WMBase, &WMBaseListener, nullptr);
    }
    else if(interface == "zxdg_decoration_manager_v1"sv)
        s_Data.Wayland.DecorationManager = static_cast<zxdg_decoration_manager_v1*>(wl_registry_bind(registry, name, &zxdg_decoration_manager_v1_interface, 1));
    else if(interface == "wp_viewporter"sv)
        s_Data.Wayland.Viewporter = static_cast<wp_viewporter*>(wl_registry_bind(registry, name, &wp_viewporter_interface, 1));
    else if(interface == "zwp_relative_pointer_manager_v1"sv)
        s_Data.Wayland.RelativePointerManager = static_cast<zwp_relative_pointer_manager_v1*>(wl_registry_bind(registry, name, &zwp_relative_pointer_manager_v1_interface, 1));
    else if(interface == "zwp_pointer_constraints_v1"sv)
        s_Data.Wayland.PointerConstraints = static_cast<zwp_pointer_constraints_v1*>(wl_registry_bind(registry, name, &zwp_pointer_constraints_v1_interface, 1));
    else if(interface == "zwp_idle_inhibit_manager_v1"sv)
        s_Data.Wayland.IdleInhibitManager = static_cast<zwp_idle_inhibit_manager_v1*>(wl_registry_bind(registry, name, &zwp_idle_inhibit_manager_v1_interface, 1));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::RegistryHandleGlobalRemove(void* userData, wl_registry* registry, uint32_t name)
{
    for(uint32_t i = 0; i < s_Data.Monitors.size(); ++i)
    {
        if(s_Data.Monitors[i]->Wayland.Name == name)
        {
            InputMonitorDisconnect(i, 0);
            return;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CreateKeyTablesWayland()
{
	std::fill(s_Data.KeyCodes.begin(), s_Data.KeyCodes.end(), Input::Key::Unknown);
	std::fill(s_Data.ScanCodes.begin(), s_Data.ScanCodes.end(), -1);

    s_Data.KeyCodes[KEY_GRAVE]      = Input::Key::Grave_Accent;
    s_Data.KeyCodes[KEY_1]          = Input::Key::One;
    s_Data.KeyCodes[KEY_2]          = Input::Key::Two;
    s_Data.KeyCodes[KEY_3]          = Input::Key::Three;
    s_Data.KeyCodes[KEY_4]          = Input::Key::Four;
    s_Data.KeyCodes[KEY_5]          = Input::Key::Five;
    s_Data.KeyCodes[KEY_6]          = Input::Key::Six;
    s_Data.KeyCodes[KEY_7]          = Input::Key::Seven;
    s_Data.KeyCodes[KEY_8]          = Input::Key::Eight;
    s_Data.KeyCodes[KEY_9]          = Input::Key::Nine;
    s_Data.KeyCodes[KEY_0]          = Input::Key::Zero;
    s_Data.KeyCodes[KEY_SPACE]      = Input::Key::Space;
    s_Data.KeyCodes[KEY_MINUS]      = Input::Key::Minus;
    s_Data.KeyCodes[KEY_EQUAL]      = Input::Key::Equal;
    s_Data.KeyCodes[KEY_Q]          = Input::Key::Q;
    s_Data.KeyCodes[KEY_W]          = Input::Key::W;
    s_Data.KeyCodes[KEY_E]          = Input::Key::E;
    s_Data.KeyCodes[KEY_R]          = Input::Key::R;
    s_Data.KeyCodes[KEY_T]          = Input::Key::T;
    s_Data.KeyCodes[KEY_Y]          = Input::Key::Y;
    s_Data.KeyCodes[KEY_U]          = Input::Key::U;
    s_Data.KeyCodes[KEY_I]          = Input::Key::I;
    s_Data.KeyCodes[KEY_O]          = Input::Key::O;
    s_Data.KeyCodes[KEY_P]          = Input::Key::P;
    s_Data.KeyCodes[KEY_LEFTBRACE]  = Input::Key::Left_Bracket;
    s_Data.KeyCodes[KEY_RIGHTBRACE] = Input::Key::Right_Bracket;
    s_Data.KeyCodes[KEY_A]          = Input::Key::A;
    s_Data.KeyCodes[KEY_S]          = Input::Key::S;
    s_Data.KeyCodes[KEY_D]          = Input::Key::D;
    s_Data.KeyCodes[KEY_F]          = Input::Key::F;
    s_Data.KeyCodes[KEY_G]          = Input::Key::G;
    s_Data.KeyCodes[KEY_H]          = Input::Key::H;
    s_Data.KeyCodes[KEY_J]          = Input::Key::J;
    s_Data.KeyCodes[KEY_K]          = Input::Key::K;
    s_Data.KeyCodes[KEY_L]          = Input::Key::L;
    s_Data.KeyCodes[KEY_SEMICOLON]  = Input::Key::Semicolon;
    s_Data.KeyCodes[KEY_APOSTROPHE] = Input::Key::Apostrophe;
    s_Data.KeyCodes[KEY_Z]          = Input::Key::Z;
    s_Data.KeyCodes[KEY_X]          = Input::Key::X;
    s_Data.KeyCodes[KEY_C]          = Input::Key::C;
    s_Data.KeyCodes[KEY_V]          = Input::Key::V;
    s_Data.KeyCodes[KEY_B]          = Input::Key::B;
    s_Data.KeyCodes[KEY_N]          = Input::Key::N;
    s_Data.KeyCodes[KEY_M]          = Input::Key::M;
    s_Data.KeyCodes[KEY_COMMA]      = Input::Key::Comma;
    s_Data.KeyCodes[KEY_DOT]        = Input::Key::Period;
    s_Data.KeyCodes[KEY_SLASH]      = Input::Key::Slash;
    s_Data.KeyCodes[KEY_BACKSLASH]  = Input::Key::Backslash;
    s_Data.KeyCodes[KEY_ESC]        = Input::Key::Escape;
    s_Data.KeyCodes[KEY_TAB]        = Input::Key::Tab;
    s_Data.KeyCodes[KEY_LEFTSHIFT]  = Input::Key::Left_Shift;
    s_Data.KeyCodes[KEY_RIGHTSHIFT] = Input::Key::Right_Shift;
    s_Data.KeyCodes[KEY_LEFTCTRL]   = Input::Key::Left_Control;
    s_Data.KeyCodes[KEY_RIGHTCTRL]  = Input::Key::Right_Control;
    s_Data.KeyCodes[KEY_LEFTALT]    = Input::Key::Left_ALT;
    s_Data.KeyCodes[KEY_RIGHTALT]   = Input::Key::Right_ALT;
    s_Data.KeyCodes[KEY_LEFTMETA]   = Input::Key::Left_Super;
    s_Data.KeyCodes[KEY_RIGHTMETA]  = Input::Key::Right_Super;
    s_Data.KeyCodes[KEY_COMPOSE]    = Input::Key::Menu;
    s_Data.KeyCodes[KEY_NUMLOCK]    = Input::Key::Num_Lock;
    s_Data.KeyCodes[KEY_CAPSLOCK]   = Input::Key::Caps_Lock;
    s_Data.KeyCodes[KEY_PRINT]      = Input::Key::Print_Screen;
    s_Data.KeyCodes[KEY_SCROLLLOCK] = Input::Key::Scroll_Lock;
    s_Data.KeyCodes[KEY_PAUSE]      = Input::Key::Pause;
    s_Data.KeyCodes[KEY_DELETE]     = Input::Key::Delete;
    s_Data.KeyCodes[KEY_BACKSPACE]  = Input::Key::Backspace;
    s_Data.KeyCodes[KEY_ENTER]      = Input::Key::Enter;
    s_Data.KeyCodes[KEY_HOME]       = Input::Key::Home;
    s_Data.KeyCodes[KEY_END]        = Input::Key::End;
    s_Data.KeyCodes[KEY_PAGEUP]     = Input::Key::Page_Up;
    s_Data.KeyCodes[KEY_PAGEDOWN]   = Input::Key::Page_Down;
    s_Data.KeyCodes[KEY_INSERT]     = Input::Key::Insert;
    s_Data.KeyCodes[KEY_LEFT]       = Input::Key::Left;
    s_Data.KeyCodes[KEY_RIGHT]      = Input::Key::Right;
    s_Data.KeyCodes[KEY_DOWN]       = Input::Key::Down;
    s_Data.KeyCodes[KEY_UP]         = Input::Key::Up;
    s_Data.KeyCodes[KEY_F1]         = Input::Key::F1;
    s_Data.KeyCodes[KEY_F2]         = Input::Key::F2;
    s_Data.KeyCodes[KEY_F3]         = Input::Key::F3;
    s_Data.KeyCodes[KEY_F4]         = Input::Key::F4;
    s_Data.KeyCodes[KEY_F5]         = Input::Key::F5;
    s_Data.KeyCodes[KEY_F6]         = Input::Key::F6;
    s_Data.KeyCodes[KEY_F7]         = Input::Key::F7;
    s_Data.KeyCodes[KEY_F8]         = Input::Key::F8;
    s_Data.KeyCodes[KEY_F9]         = Input::Key::F9;
    s_Data.KeyCodes[KEY_F10]        = Input::Key::F10;
    s_Data.KeyCodes[KEY_F11]        = Input::Key::F11;
    s_Data.KeyCodes[KEY_F12]        = Input::Key::F12;
    s_Data.KeyCodes[KEY_F13]        = Input::Key::F13;
    s_Data.KeyCodes[KEY_F14]        = Input::Key::F14;
    s_Data.KeyCodes[KEY_F15]        = Input::Key::F15;
    s_Data.KeyCodes[KEY_F16]        = Input::Key::F16;
    s_Data.KeyCodes[KEY_F17]        = Input::Key::F17;
    s_Data.KeyCodes[KEY_F18]        = Input::Key::F18;
    s_Data.KeyCodes[KEY_F19]        = Input::Key::F19;
    s_Data.KeyCodes[KEY_F20]        = Input::Key::F20;
    s_Data.KeyCodes[KEY_F21]        = Input::Key::F21;
    s_Data.KeyCodes[KEY_F22]        = Input::Key::F22;
    s_Data.KeyCodes[KEY_F23]        = Input::Key::F23;
    s_Data.KeyCodes[KEY_F24]        = Input::Key::F24;
    s_Data.KeyCodes[KEY_KPSLASH]    = Input::Key::KP_Divide;
    s_Data.KeyCodes[KEY_KPASTERISK] = Input::Key::KP_Multiply;
    s_Data.KeyCodes[KEY_KPMINUS]    = Input::Key::KP_Subtract;
    s_Data.KeyCodes[KEY_KPPLUS]     = Input::Key::KP_Add;
    s_Data.KeyCodes[KEY_KP0]        = Input::Key::KP_0;
    s_Data.KeyCodes[KEY_KP1]        = Input::Key::KP_1;
    s_Data.KeyCodes[KEY_KP2]        = Input::Key::KP_2;
    s_Data.KeyCodes[KEY_KP3]        = Input::Key::KP_3;
    s_Data.KeyCodes[KEY_KP4]        = Input::Key::KP_4;
    s_Data.KeyCodes[KEY_KP5]        = Input::Key::KP_5;
    s_Data.KeyCodes[KEY_KP6]        = Input::Key::KP_6;
    s_Data.KeyCodes[KEY_KP7]        = Input::Key::KP_7;
    s_Data.KeyCodes[KEY_KP8]        = Input::Key::KP_8;
    s_Data.KeyCodes[KEY_KP9]        = Input::Key::KP_9;
    s_Data.KeyCodes[KEY_KPDOT]      = Input::Key::KP_Decimal;
    s_Data.KeyCodes[KEY_KPEQUAL]    = Input::Key::KP_Equal;
    s_Data.KeyCodes[KEY_KPENTER]    = Input::Key::KP_Enter;
    s_Data.KeyCodes[KEY_102ND]      = Input::Key::World_2;

    for (int32_t scancode = 0;  scancode < 256;  scancode++)
    {
        if (s_Data.KeyCodes[scancode] != TRAP::Input::Key::Unknown)
            s_Data.ScanCodes[static_cast<int32_t>(s_Data.KeyCodes[scancode])] = static_cast<int16_t>(scancode);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::LoadCursorThemeWayland()
{
    int32_t cursorSize = 32;

    const char* sizeString = getenv("XCURSOR_SIZE");
    if(sizeString)
    {
        errno = 0;
        const long cursorSizeLong = std::stol(sizeString);
        if(errno == 0 && cursorSizeLong > 0 && cursorSizeLong < std::numeric_limits<int32_t>::max())
            cursorSize = static_cast<int32_t>(cursorSizeLong);
    }

    const char* themeName = getenv("XCURSOR_THEME");

    s_Data.Wayland.CursorTheme = s_Data.Wayland.WaylandCursor.ThemeLoad(themeName, cursorSize, s_Data.Wayland.Shm);
    if(!s_Data.Wayland.CursorTheme)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to load default cursor theme");
        return false;
    }

    //If this happens to be nullptr, we just fallback to the scale=1 version.
    s_Data.Wayland.CursorThemeHiDPI = s_Data.Wayland.WaylandCursor.ThemeLoad(themeName, cursorSize * 2, s_Data.Wayland.Shm);

    s_Data.Wayland.CursorSurface = wl_compositor_create_surface(s_Data.Wayland.Compositor);
    s_Data.Wayland.CursorTimerFD = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    return true;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//


TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModeWayland(const InternalMonitor* monitor)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSizeWayland(const InternalWindow* window, int32_t& width,
                                                                int32_t& height)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPosWayland(const InternalWindow* window, const int32_t xPos, const int32_t yPos)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorWayland(InternalWindow* window, InternalMonitor* monitor,
														           const int32_t xPos, const int32_t yPos, const int32_t width,
															       const int32_t height, const int32_t refreshRate)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderlessWayland(InternalWindow* window,
                                                                             InternalMonitor* monitor)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModesWayland(const InternalMonitor* monitor)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformInitWayland()
{
    //These must be set before any failure checks
    s_Data.Wayland.KeyRepeatTimerFD = -1;
    s_Data.Wayland.CursorTimerFD = -1;

    {
        s_Data.Wayland.WaylandClient.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libwayland-client.so.0");
        if(!s_Data.Wayland.WaylandClient.Handle)
        {
            InputError(Error::Platform_Error, "[Wayland] Failed to load libwayland-client");
            return false;
        }

        s_Data.Wayland.WaylandClient.DisplayFlush = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_flush>(s_Data.Wayland.WaylandClient.Handle, "wl_display_flush");
        s_Data.Wayland.WaylandClient.DisplayCancelRead = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_cancel_read>(s_Data.Wayland.WaylandClient.Handle, "wl_display_cancel_read");
        s_Data.Wayland.WaylandClient.DisplayDispatchPending = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_dispatch_pending>(s_Data.Wayland.WaylandClient.Handle, "wl_display_dispatch_pending");
        s_Data.Wayland.WaylandClient.DisplayReadEvents = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_read_events>(s_Data.Wayland.WaylandClient.Handle, "wl_display_read_events");
        s_Data.Wayland.WaylandClient.DisplayConnect = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_connect>(s_Data.Wayland.WaylandClient.Handle, "wl_display_connect");
        s_Data.Wayland.WaylandClient.DisplayDisconnect = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_disconnect>(s_Data.Wayland.WaylandClient.Handle, "wl_display_disconnect");
        s_Data.Wayland.WaylandClient.DisplayRoundtrip = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_roundtrip>(s_Data.Wayland.WaylandClient.Handle, "wl_display_roundtrip");
        s_Data.Wayland.WaylandClient.DisplayGetFD = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_get_fd>(s_Data.Wayland.WaylandClient.Handle, "wl_display_get_fd");
        s_Data.Wayland.WaylandClient.DisplayPrepareRead = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_display_prepare_read>(s_Data.Wayland.WaylandClient.Handle, "wl_display_prepare_read");

        s_Data.Wayland.WaylandClient.ProxyMarshal = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_marshal>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_marshal");
        s_Data.Wayland.WaylandClient.ProxyAddListener = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_add_listener>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_add_listener");
        s_Data.Wayland.WaylandClient.ProxyDestroy = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_destroy>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_destroy");
        s_Data.Wayland.WaylandClient.ProxyMarshalConstructor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_marshal_constructor>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_marshal_constructor");
        s_Data.Wayland.WaylandClient.ProxyMarshalConstructorVersioned = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_marshal_constructor_versioned>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_marshal_constructor_versioned");
        s_Data.Wayland.WaylandClient.ProxyGetUserData = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_get_user_data>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_get_user_data");
        s_Data.Wayland.WaylandClient.ProxySetUserData = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_set_user_data>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_set_user_data");
        s_Data.Wayland.WaylandClient.ProxyGetVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_get_version>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_get_version");
        s_Data.Wayland.WaylandClient.ProxyMarshalFlags = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_marshal_flags>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_marshal_flags");

        if(!s_Data.Wayland.WaylandClient.DisplayFlush ||
        !s_Data.Wayland.WaylandClient.DisplayCancelRead ||
        !s_Data.Wayland.WaylandClient.DisplayDispatchPending ||
        !s_Data.Wayland.WaylandClient.DisplayReadEvents ||
        !s_Data.Wayland.WaylandClient.DisplayConnect ||
        !s_Data.Wayland.WaylandClient.DisplayDisconnect ||
        !s_Data.Wayland.WaylandClient.DisplayRoundtrip ||
        !s_Data.Wayland.WaylandClient.DisplayGetFD ||
        !s_Data.Wayland.WaylandClient.DisplayPrepareRead ||
        !s_Data.Wayland.WaylandClient.ProxyMarshal ||
        !s_Data.Wayland.WaylandClient.ProxyAddListener ||
        !s_Data.Wayland.WaylandClient.ProxyDestroy ||
        !s_Data.Wayland.WaylandClient.ProxyMarshalConstructor ||
        !s_Data.Wayland.WaylandClient.ProxyMarshalConstructorVersioned ||
        !s_Data.Wayland.WaylandClient.ProxyGetUserData ||
        !s_Data.Wayland.WaylandClient.ProxySetUserData /*||
        !s_Data.Wayland.WaylandClient.ProxyGetVersion ||
        !s_Data.Wayland.WaylandClient.ProxyMarshalFlags*/)
        {
            TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.Wayland.WaylandClient.Handle);
            InputError(Error::Platform_Error, "[Wayland] Failed to load libwayland-client entry point");
            return false;
        }

        s_Data.Wayland.DisplayWL = s_Data.Wayland.WaylandClient.DisplayConnect(nullptr);
        if(!s_Data.Wayland.DisplayWL)
        {
            TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.Wayland.WaylandClient.Handle);
            InputError(Error::Platform_Error, "[Wayland] Failed to connect to display");
            return false;
        }
    }

    {
        s_Data.Wayland.WaylandCursor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libwayland-cursor.so.0");
        if(!s_Data.Wayland.WaylandCursor.Handle)
        {
            InputError(Error::Platform_Error, "[Wayland] Failed to load libwayland-cursor");
            return false;
        }

        s_Data.Wayland.WaylandCursor.ThemeLoad = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_cursor_theme_load>(s_Data.Wayland.WaylandCursor.Handle, "wl_cursor_theme_load");
        s_Data.Wayland.WaylandCursor.ThemeDestroy = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_cursor_theme_destroy>(s_Data.Wayland.WaylandCursor.Handle, "wl_cursor_theme_destroy");
        s_Data.Wayland.WaylandCursor.ThemeGetCursor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_cursor_theme_get_cursor>(s_Data.Wayland.WaylandCursor.Handle, "wl_cursor_theme_get_cursor");
        s_Data.Wayland.WaylandCursor.ImageGetBuffer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_cursor_image_get_buffer>(s_Data.Wayland.WaylandCursor.Handle, "wl_cursor_image_get_buffer");
    }

    {
        s_Data.Wayland.WaylandXKB.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libxkbcommon.so.0");
        if(!s_Data.Wayland.WaylandXKB.Handle)
        {
            InputError(Error::Platform_Error, "[Wayland] Failed to load libxkbcommon");
            return false;
        }

        s_Data.Wayland.WaylandXKB.ContextNew = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_context_new>(s_Data.Wayland.WaylandXKB.Handle, "xkb_context_new");
        s_Data.Wayland.WaylandXKB.ContextUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_context_unref>(s_Data.Wayland.WaylandXKB.Handle, "xkb_context_unref");
        s_Data.Wayland.WaylandXKB.KeyMapNewFromString = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_keymap_new_from_string>(s_Data.Wayland.WaylandXKB.Handle, "xkb_keymap_new_from_string");
        s_Data.Wayland.WaylandXKB.KeyMapUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_keymap_unref>(s_Data.Wayland.WaylandXKB.Handle, "xkb_keymap_unref");
        s_Data.Wayland.WaylandXKB.KeyMapModGetIndex = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_keymap_mod_get_index>(s_Data.Wayland.WaylandXKB.Handle, "xkb_keymap_mod_get_index");
        s_Data.Wayland.WaylandXKB.KeyMapKeyRepeats = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_keymap_key_repeats>(s_Data.Wayland.WaylandXKB.Handle, "xkb_keymap_key_repeats");
        s_Data.Wayland.WaylandXKB.KeyMapKeyGetSymsByLevel = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_keymap_key_get_syms_by_level>(s_Data.Wayland.WaylandXKB.Handle, "xkb_keymap_key_get_syms_by_level");
        s_Data.Wayland.WaylandXKB.StateNew = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_state_new>(s_Data.Wayland.WaylandXKB.Handle, "xkb_state_new");
        s_Data.Wayland.WaylandXKB.StateUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_state_unref>(s_Data.Wayland.WaylandXKB.Handle, "xkb_state_unref");
        s_Data.Wayland.WaylandXKB.StateKeyGetSyms = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_state_key_get_syms>(s_Data.Wayland.WaylandXKB.Handle, "xkb_state_key_get_syms");
        s_Data.Wayland.WaylandXKB.StateUpdateMask = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_state_update_mask>(s_Data.Wayland.WaylandXKB.Handle, "xkb_state_update_mask");
        s_Data.Wayland.WaylandXKB.StateKeyGetLayout = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_state_key_get_layout>(s_Data.Wayland.WaylandXKB.Handle, "xkb_state_key_get_layout");
        s_Data.Wayland.WaylandXKB.StateModIndexIsActive = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_state_mod_index_is_active>(s_Data.Wayland.WaylandXKB.Handle, "xkb_state_mod_index_is_active");
        s_Data.Wayland.WaylandXKB.ComposeTableNewFromLocale = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_table_new_from_locale>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_table_new_from_locale");
        s_Data.Wayland.WaylandXKB.ComposeTableUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_table_unref>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_table_unref");
        s_Data.Wayland.WaylandXKB.ComposeStateNew = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_new>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_new");
        s_Data.Wayland.WaylandXKB.ComposeStateUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_unref>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_unref");
        s_Data.Wayland.WaylandXKB.ComposeStateFeed = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_feed>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_feed");
        s_Data.Wayland.WaylandXKB.ComposeStateGetStatus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_get_status>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_get_status");
        s_Data.Wayland.WaylandXKB.ComposeStateGetOneSym = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_get_one_sym>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_get_one_sym");

        s_Data.Wayland.Registry = wl_display_get_registry(s_Data.Wayland.DisplayWL);
        wl_registry_add_listener(s_Data.Wayland.Registry, &RegistryListener, nullptr);

        CreateKeyTablesWayland();

        s_Data.Wayland.WaylandXKB.Context = s_Data.Wayland.WaylandXKB.ContextNew(XKB_CONTEXT_NO_FLAGS);
        if(!s_Data.Wayland.WaylandXKB.Context)
        {
            InputError(Error::Platform_Error, "[Wayland] Failed to initialize xkb context");
            return false;
        }
    }

    //Sync so we got all registry objects
    s_Data.Wayland.WaylandClient.DisplayRoundtrip(s_Data.Wayland.DisplayWL);

    //Sync so we got all initial output events
    s_Data.Wayland.WaylandClient.DisplayRoundtrip(s_Data.Wayland.DisplayWL);

#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
    if(s_Data.Wayland.SeatVersion >= WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION)
    {
        s_Data.Wayland.KeyRepeatTimerFD = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    }
#endif

    if(!s_Data.Wayland.WMBase)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to find xdg-shell in your compositor");
        return false;
    }

    if(!s_Data.Wayland.Shm)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to find wl_shm in your compositor");
        return false;
    }

    if(!LoadCursorThemeWayland())
        return false;

    if(s_Data.Wayland.Seat && s_Data.Wayland.DataDeviceManager)
    {
        s_Data.Wayland.DataDevice = wl_data_device_manager_get_data_device(s_Data.Wayland.DataDeviceManager, s_Data.Wayland.Seat);
        wl_data_device_add_listener(s_Data.Wayland.DataDevice, &DataDeviceListener, nullptr);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyWindowWayland(InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShutdownWayland()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScaleWayland(const InternalMonitor* monitor, float& xScale,
                                                                         float& yScale)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPosWayland(const InternalMonitor* monitor, int32_t& xPos,
                                                                int32_t& yPos)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindowWayland(InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindowWayland(const InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowWayland(InternalWindow* window,
			                                                   WindowConfig& WNDConfig)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowTitleWayland(const InternalWindow* window, const std::string& title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursorWayland(InternalCursor* cursor, const Image* const image,
                                                               const int32_t xHotspot, const int32_t yHotspot)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursorWayland(InternalCursor* cursor, const CursorType& type)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursorWayland(InternalCursor* cursor)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorWayland(const InternalWindow* window, const InternalCursor* cursor)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorModeWayland(InternalWindow* window, const CursorMode mode)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPosWayland(InternalWindow* window, const double xPos, const double yPos)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPosWayland(const InternalWindow* window, double& xPos, double& yPos)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIconWayland(InternalWindow* window, const Image* const image)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPosWayland(const InternalWindow* window, int32_t& xPos, int32_t& yPos)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeWayland(InternalWindow* window, const int32_t width, const int32_t height)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizableWayland(InternalWindow* window, const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecoratedWayland(const InternalWindow* window, const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloatingWayland(const InternalWindow* window, const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacityWayland(const InternalWindow* window, const float opacity)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthroughWayland(InternalWindow* window, const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbarWayland(InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacityWayland(const InternalWindow* window)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSizeWayland(const InternalWindow* window, int32_t& width,
                                                                     int32_t& height)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScaleWayland(const InternalWindow* window, float& xScale,
                                                                        float& yScale)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkAreaWayland(const InternalMonitor* monitor, int32_t& xPos,
                                                                     int32_t& yPos, int32_t& width, int32_t& height)
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowVisibleWayland(const InternalWindow* window)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximizedWayland(const InternalWindow* window)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMinimizedWayland(const InternalWindow* window)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPollEventsWayland()
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocusedWayland(const InternalWindow* window)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHoveredWayland(const InternalWindow* window)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformRawMouseMotionSupportedWayland()
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotionWayland(const InternalWindow* window, const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetProgressWayland(const InternalWindow* window, const ProgressState state,
													          const uint32_t completed)
{
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::INTERNAL::WindowingAPI::PlatformGetKeyScanCodeWayland(const Input::Key key)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeNameWayland(const int32_t scanCode)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetClipboardStringWayland(const std::string& string)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::PlatformGetClipboardStringWayland()
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetRequiredInstanceExtensionsWayland(std::array<std::string, 2>& extensions)
{
}

//-------------------------------------------------------------------------------------------------------------------//

VkResult TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowSurfaceWayland(VkInstance instance, const InternalWindow* window,
																          const VkAllocationCallbacks* allocator,
																          VkSurfaceKHR& surface)
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindowWayland(const InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindowWayland(const InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttentionWayland(const InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowWayland(const InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindowWayland(InternalWindow* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimitsWayland(InternalWindow* window, int32_t minWidth,
                                                                      int32_t minHeight, int32_t maxWidth,
                                                                      int32_t maxHeight)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::GetLinuxKeyboardLayoutNameWayland()
{
    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDropWayland(InternalWindow* window, const bool value)
{
}

#endif