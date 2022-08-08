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

#include "Input/Input.h"
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

using namespace std::string_view_literals;

static constexpr int32_t TRAP_BORDER_SIZE = 4;
static constexpr int32_t TRAP_CAPTION_HEIGHT = 24;

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataOfferHandleOffer(void* /*userData*/, wl_data_offer* offer, const char* mimeType)
{
    for(uint32_t i = 0; i < s_Data.Wayland.Offers.size(); ++i)
    {
        if(s_Data.Wayland.Offers[i].offer == offer)
        {
            if(mimeType == "text/plain;charset=utf-8"sv)
                s_Data.Wayland.Offers[i].text_plain_utf8 = true;
            else if(mimeType == "text/uri-list"sv)
                s_Data.Wayland.Offers[i].text_uri_list = true;

            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleDataOffer(void* /*userData*/, wl_data_device* /*device*/,
                                                             wl_data_offer* offer)
{
    s_Data.Wayland.Offers.push_back(TRAPOfferWayland{offer, false, false});
    wl_data_offer_add_listener(offer, &DataOfferListener, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleEnter(void* /*userData*/, wl_data_device* /*device*/,
                                                         uint32_t serial, wl_surface* surface, wl_fixed_t /*x*/,
                                                         wl_fixed_t /*y*/, wl_data_offer* offer)
{
    if(s_Data.Wayland.DragOffer)
    {
        wl_data_offer_destroy(s_Data.Wayland.DragOffer);
        s_Data.Wayland.DragOffer = nullptr;
        s_Data.Wayland.DragFocus = nullptr;
    }

    for(uint32_t i = 0; i < s_Data.Wayland.Offers.size(); ++i)
    {
        if(s_Data.Wayland.Offers[i].offer == offer)
        {
            InternalWindow* window = nullptr;

            if(surface)
                window = static_cast<InternalWindow*>(wl_surface_get_user_data(surface));

            if(window && s_Data.Wayland.Offers[i].text_uri_list)
            {
                s_Data.Wayland.DragOffer = offer;
                s_Data.Wayland.DragFocus = window;
                s_Data.Wayland.DragSerial = serial;
            }

            s_Data.Wayland.Offers.erase(s_Data.Wayland.Offers.begin() + i);
            break;
        }
    }

    if(s_Data.Wayland.DragOffer)
        wl_data_offer_accept(offer, serial, "text/uri-list");
    else
    {
        wl_data_offer_accept(offer, serial, nullptr);
        wl_data_offer_destroy(offer);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleLeave(void* /*userData*/, wl_data_device* /*device*/)
{
    if(s_Data.Wayland.DragOffer)
    {
        wl_data_offer_destroy(s_Data.Wayland.DragOffer);
        s_Data.Wayland.DragOffer = nullptr;
        s_Data.Wayland.DragFocus = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleMotion(void* /*userData*/, wl_data_device* /*device*/,
                                                          uint32_t /*time*/, wl_fixed_t /*x*/, wl_fixed_t /*y*/)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleDrop(void* /*userData*/, wl_data_device* /*device*/)
{
    if(!s_Data.Wayland.DragOffer)
        return;

    std::string string = ReadDataOfferAsString(s_Data.Wayland.DragOffer, "text/uri-list");
    if(!string.empty())
    {
        int32_t count = 0;
        const std::vector<std::string> paths = ParseUriList(string.data(), count);
        if(!paths.empty())
            InputDrop(s_Data.Wayland.DragFocus, paths);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleSelection(void* /*userData*/, wl_data_device* /*device*/,
                                                             wl_data_offer* offer)
{
    if(s_Data.Wayland.SelectionOffer)
    {
        wl_data_offer_destroy(s_Data.Wayland.SelectionOffer);
        s_Data.Wayland.SelectionOffer = nullptr;
    }

    for(uint32_t i = 0; i < s_Data.Wayland.Offers.size(); ++i)
    {
        if(s_Data.Wayland.Offers[i].offer == offer)
        {
            if(s_Data.Wayland.Offers[i].text_plain_utf8)
                s_Data.Wayland.SelectionOffer = offer;
            else
                wl_data_offer_destroy(offer);

            s_Data.Wayland.Offers.erase(s_Data.Wayland.Offers.begin() + i);
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleGeometry(void* userData, wl_output* /*output*/, int32_t x, int32_t y,
                                                        int32_t /*physicalWidth*/, int32_t /*physicalHeight*/,
                                                        int32_t /*subpixel*/, const char* make, const char* model,
                                                        int32_t /*transform*/)
{
    InternalMonitor* monitor = static_cast<InternalMonitor*>(userData);

    monitor->Wayland.X = x;
    monitor->Wayland.Y = y;

    const std::string name = std::string(make) + " " + std::string(model);
    if(monitor->Name.empty())
        monitor->Name = name;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleMode(void* userData, wl_output* /*output*/, uint32_t flags,
                                                    int32_t width, int32_t height, int32_t refresh)
{
    InternalMonitor* monitor = static_cast<InternalMonitor*>(userData);

    InternalVideoMode mode{};
    mode.Width = width;
    mode.Height = height;
    mode.RedBits = 8;
    mode.GreenBits = 8;
    mode.BlueBits = 8;
    mode.RefreshRate = static_cast<int32_t>(TRAP::Math::Round(refresh / 1000.0));

    monitor->Modes.push_back(mode);

    if(flags & WL_OUTPUT_MODE_CURRENT)
        monitor->CurrentMode = monitor->Modes.back();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleDone(void* userData, wl_output* /*output*/)
{
    InternalMonitor* monitor = static_cast<InternalMonitor*>(userData);

    for(const Scope<InternalMonitor>& mon : s_Data.Monitors)
    {
        if(mon.get() == monitor)
            return;
    }

    InputMonitor( TRAP::Scope<InternalMonitor>(monitor), true, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleScale(void* userData, wl_output* /*output*/, int32_t factor)
{
    InternalMonitor* monitor = static_cast<InternalMonitor*>(userData);

    monitor->Wayland.Scale = factor;

    uint32_t index = 0;
    for(InternalWindow* window : s_Data.WindowList)
    {
        if(window->Wayland.Monitors[index] == monitor)
        {
            UpdateContentScaleWayland(window);
            break;
        }

        index++;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
void TRAP::INTERNAL::WindowingAPI::OutputHandleName(void* userData, wl_output* /*output*/, const char* name)
{
    InternalMonitor* monitor = static_cast<InternalMonitor*>(userData);

    monitor->Name = name;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
void TRAP::INTERNAL::WindowingAPI::OutputHandleDescription(void* /*userData*/, wl_output* /*output*/,
                                                           const char* /*description*/)
{
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleKeymap(void* /*userData*/, wl_keyboard* /*keyboard*/, uint32_t format,
                                                        int32_t fd, uint32_t size)
{
    if(format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
    {
        close(fd);
        return;
    }

    char* mapStr = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
    if(mapStr == MAP_FAILED)
    {
        close(fd);
        return;
    }

    xkb_keymap* keymap = s_Data.Wayland.WaylandXKB.KeyMapNewFromString(s_Data.Wayland.WaylandXKB.Context, mapStr, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(mapStr, size);
    close(fd);

    if(!keymap)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to compile keymap");
        return;
    }

    xkb_state* state = s_Data.Wayland.WaylandXKB.StateNew(keymap);
    if(!state)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create XKB state");
        s_Data.Wayland.WaylandXKB.KeyMapUnref(keymap);
        return;
    }

    //Look up the preferred locale, falling back to "C" as default.
    const char* locale = getenv("LC_ALL");
    if(!locale)
        locale = getenv("LC_CTYPE");
    if(!locale)
        locale = getenv("LANG");
    if(!locale)
        locale = "C";

    xkb_compose_table* composeTable = s_Data.Wayland.WaylandXKB.ComposeTableNewFromLocale(s_Data.Wayland.WaylandXKB.Context, locale, XKB_COMPOSE_COMPILE_NO_FLAGS);

    if(composeTable)
    {
        xkb_compose_state* composeState = s_Data.Wayland.WaylandXKB.ComposeStateNew(composeTable, XKB_COMPOSE_STATE_NO_FLAGS);
        s_Data.Wayland.WaylandXKB.ComposeTableUnref(composeTable);
        if(composeState)
            s_Data.Wayland.WaylandXKB.ComposeState = composeState;
        else
            InputError(Error::Platform_Error, "[Wayland] Failed to create XKB compose state");
    }
    else
        InputError(Error::Platform_Error, "[Wayland] Failed to create XKB compose table");

    s_Data.Wayland.WaylandXKB.KeyMapUnref(s_Data.Wayland.WaylandXKB.KeyMap);
    s_Data.Wayland.WaylandXKB.StateUnref(s_Data.Wayland.WaylandXKB.State);
    s_Data.Wayland.WaylandXKB.KeyMap = keymap;
    s_Data.Wayland.WaylandXKB.State = state;

    s_Data.Wayland.WaylandXKB.ControlIndex = s_Data.Wayland.WaylandXKB.KeyMapModGetIndex(s_Data.Wayland.WaylandXKB.KeyMap, "Control");
    s_Data.Wayland.WaylandXKB.AltIndex = s_Data.Wayland.WaylandXKB.KeyMapModGetIndex(s_Data.Wayland.WaylandXKB.KeyMap, "Mod1");
    s_Data.Wayland.WaylandXKB.ShiftIndex = s_Data.Wayland.WaylandXKB.KeyMapModGetIndex(s_Data.Wayland.WaylandXKB.KeyMap, "Shift");
    s_Data.Wayland.WaylandXKB.SuperIndex = s_Data.Wayland.WaylandXKB.KeyMapModGetIndex(s_Data.Wayland.WaylandXKB.KeyMap, "Mod4");
    s_Data.Wayland.WaylandXKB.CapsLockIndex = s_Data.Wayland.WaylandXKB.KeyMapModGetIndex(s_Data.Wayland.WaylandXKB.KeyMap, "Lock");
    s_Data.Wayland.WaylandXKB.NumLockIndex = s_Data.Wayland.WaylandXKB.KeyMapModGetIndex(s_Data.Wayland.WaylandXKB.KeyMap, "Mod2");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleEnter(void* /*userData*/, wl_keyboard* /*keyboard*/, uint32_t serial,
                                                       wl_surface* surface, wl_array* /*keys*/)
{
    //Happens in the case we just destroyed the surface.
    if(!surface)
        return;

    InternalWindow* window = static_cast<InternalWindow*>(wl_surface_get_user_data(surface));
    if(!window)
    {
        TRAPDecorationSideWayland unused;
        window = FindWindowFromDecorationSurface(surface, unused);
        if(!window)
            return;
    }

    s_Data.Wayland.Serial = serial;
    s_Data.Wayland.KeyboardFocus = window;
    InputWindowFocus(window, true);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleLeave(void* /*userData*/, wl_keyboard* /*keyboard*/, uint32_t serial,
                                                       wl_surface* /*surface*/)
{
    InternalWindow* window = s_Data.Wayland.KeyboardFocus;

    if(!window)
        return;

    itimerspec timer{};
    timerfd_settime(s_Data.Wayland.KeyRepeatTimerFD, 0, &timer, nullptr);

    s_Data.Wayland.Serial = serial;
    s_Data.Wayland.KeyboardFocus = nullptr;
    InputWindowFocus(window, false);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleKey(void* /*userData*/, wl_keyboard* /*keyboard*/, uint32_t serial,
                                                     uint32_t /*time*/, uint32_t scanCode, uint32_t state)
{
    InternalWindow* window = s_Data.Wayland.KeyboardFocus;
    if(!window)
        return;

    const Input::Key key = TranslateKey(scanCode);
    const Input::KeyState action = state == WL_KEYBOARD_KEY_STATE_PRESSED ? Input::KeyState::Pressed :
                                                                            Input::KeyState::Released;

    s_Data.Wayland.Serial = serial;

    itimerspec timer{};

    if(action == Input::KeyState::Pressed)
    {
        const xkb_keycode_t keycode = scanCode + 8;

        if(s_Data.Wayland.WaylandXKB.KeyMapKeyRepeats(s_Data.Wayland.WaylandXKB.KeyMap, keycode) &&
           s_Data.Wayland.KeyRepeatScancode > 0)
        {
            s_Data.Wayland.KeyRepeatScancode = scanCode;
            if(s_Data.Wayland.KeyRepeatRate > 1)
                timer.it_interval.tv_nsec = 1000000000 / s_Data.Wayland.KeyRepeatRate;
            else
                timer.it_interval.tv_sec = 1;

            timer.it_value.tv_sec = s_Data.Wayland.KeyRepeatDelay / 1000;
            timer.it_value.tv_nsec = (s_Data.Wayland.KeyRepeatDelay % 1000) * 1000000;
        }
    }

    timerfd_settime(s_Data.Wayland.KeyRepeatTimerFD, 0, &timer, nullptr);

    InputKey(window, key, scanCode, action);

    if(action == Input::KeyState::Pressed)
        InputTextWayland(window, scanCode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleModifiers(void* /*userData*/, wl_keyboard* /*keyboard*/,
                                                           uint32_t /*serial*/, uint32_t /*modsDepressed*/,
                                                           uint32_t /*modsLatched*/, uint32_t /*modsLocked*/,
                                                           uint32_t /*group*/)
{
    //TODO Only used for setting modifier bits which we dont use anyway
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
void TRAP::INTERNAL::WindowingAPI::KeyboardHandleRepeatInfo(void* /*userData*/, wl_keyboard* keyboard, int32_t rate,
                                                            int32_t delay)
{
    if(keyboard != s_Data.Wayland.Keyboard)
        return;

    s_Data.Wayland.KeyRepeatRate = rate;
    s_Data.Wayland.KeyRepeatDelay = delay;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleEnter(void* /*userData*/, wl_pointer* /*pointer*/, uint32_t serial,
                                                      wl_surface* surface, wl_fixed_t /*sX*/, wl_fixed_t /*sY*/)
{
    //Happens in the case we just destroyed the surface.
    if(!surface)
        return;

    TRAPDecorationSideWayland focus = TRAPDecorationSideWayland::MainWindow;
    InternalWindow* window = static_cast<InternalWindow*>(wl_surface_get_user_data(surface));
    if(!window)
    {
        window = FindWindowFromDecorationSurface(surface, focus);
        if(!window)
            return;
    }

    window->Wayland.Decorations.Focus = focus;
    s_Data.Wayland.Serial = serial;
    s_Data.Wayland.PointerEnterSerial = serial;
    s_Data.Wayland.PointerFocus = window;

    window->Wayland.Hovered = true;

    PlatformSetCursor(window, window->Cursor);
    InputCursorEnter(window, true);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleLeave(void* /*userData*/, wl_pointer* /*pointer*/, uint32_t serial,
                                                      wl_surface* /*surface*/)
{
    InternalWindow* window = s_Data.Wayland.PointerFocus;

    if(!window)
        return;

    window->Wayland.Hovered = false;

    s_Data.Wayland.Serial = serial;
    s_Data.Wayland.PointerFocus = nullptr;
    s_Data.Wayland.CursorPreviousName = "";

    InputCursorEnter(window, false);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleMotion(void* /*userData*/, wl_pointer* /*pointer*/, uint32_t /*time*/,
                                                       wl_fixed_t sX, wl_fixed_t sY)
{
    InternalWindow* window = s_Data.Wayland.PointerFocus;
    std::string cursorName = nullptr;

    if(!window)
        return;

    if(window->cursorMode == CursorMode::Disabled)
        return;

    const double x = wl_fixed_to_double(sX);
    const double y = wl_fixed_to_double(sY);
    window->Wayland.CursorPosX = x;
    window->Wayland.CursorPosY = y;

    switch(window->Wayland.Decorations.Focus)
    {
    case TRAPDecorationSideWayland::MainWindow:
        s_Data.Wayland.CursorPreviousName = nullptr;
        InputCursorPos(window, x, y);
        break;

    case TRAPDecorationSideWayland::TopDecoration:
        if(y < TRAP_BORDER_SIZE)
            cursorName = "n-resize";
        else
            cursorName = "left_ptr";
        break;

    case TRAPDecorationSideWayland::LeftDecoration:
        if(y < TRAP_BORDER_SIZE)
            cursorName = "nw-resize";
        else
            cursorName = "w-resize";
        break;

    case TRAPDecorationSideWayland::RightDecoration:
        if(y < TRAP_BORDER_SIZE)
            cursorName = "ne-resize";
        else
            cursorName = "e-resize";
        break;

    case TRAPDecorationSideWayland::BottomDecoration:
        if(x < TRAP_BORDER_SIZE)
            cursorName = "sw-resize";
        else if(x > window->Width + TRAP_BORDER_SIZE)
            cursorName = "se-resize";
        else
            cursorName = "s-resize";
        break;

    default:
        TRAP_ASSERT(false);
    }

    if(s_Data.Wayland.CursorPreviousName != cursorName)
        SetCursorWayland(window, cursorName);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleButton(void* /*userData*/, wl_pointer* /*pointer*/, uint32_t serial,
                                                       uint32_t /*time*/, uint32_t button, uint32_t state)
{
    InternalWindow* window = s_Data.Wayland.PointerFocus;
    uint32_t edges = XDG_TOPLEVEL_RESIZE_EDGE_NONE;

    if(!window)
        return;
    if(button == BTN_LEFT)
    {
        switch(window->Wayland.Decorations.Focus)
        {
        case TRAPDecorationSideWayland::MainWindow:
            break;
        case TRAPDecorationSideWayland::TopDecoration:
            if(window->Wayland.CursorPosY < TRAP_BORDER_SIZE)
                edges = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
            else
                xdg_toplevel_move(window->Wayland.XDG.TopLevel, s_Data.Wayland.Seat, serial);
            break;
        case TRAPDecorationSideWayland::LeftDecoration:
            if(window->Wayland.CursorPosY < TRAP_BORDER_SIZE)
                edges = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
            else
                edges = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
            break;
        case TRAPDecorationSideWayland::RightDecoration:
            if(window->Wayland.CursorPosY < TRAP_BORDER_SIZE)
                edges = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
            else
                edges = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
            break;
        case TRAPDecorationSideWayland::BottomDecoration:
            if(window->Wayland.CursorPosX < TRAP_BORDER_SIZE)
                edges = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
            else if(window->Wayland.CursorPosX > window->Width + TRAP_BORDER_SIZE)
                edges = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
            else
                edges = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
            break;

        default:
            TRAP_ASSERT(false);
        }

        if(edges != XDG_TOPLEVEL_RESIZE_EDGE_NONE)
        {
            xdg_toplevel_resize(window->Wayland.XDG.TopLevel, s_Data.Wayland.Seat, serial, edges);
            return;
        }
    }
    else if(button == BTN_RIGHT)
    {
        if(window->Wayland.Decorations.Focus != TRAPDecorationSideWayland::MainWindow &&
           window->Wayland.XDG.TopLevel)
        {
            xdg_toplevel_show_window_menu(window->Wayland.XDG.TopLevel, s_Data.Wayland.Seat,
                                          serial, window->Wayland.CursorPosX, window->Wayland.CursorPosY);
            return;
        }
    }

    //Don't pass the button to the user if it was related to a decoration
    if(window->Wayland.Decorations.Focus != TRAPDecorationSideWayland::MainWindow)
        return;

    s_Data.Wayland.Serial = serial;

    //Makes left, right and middle 0, 1 and 2. Overall order follows evdev codes
    TRAP::Input::MouseButton btn = static_cast<Input::MouseButton>(button - BTN_LEFT);

    InputMouseClick(window, btn, state == WL_POINTER_BUTTON_STATE_PRESSED ? Input::KeyState::Pressed :
                                                                                          Input::KeyState::Released);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleAxis(void* /*userData*/, wl_pointer* /*pointer*/, uint32_t /*time*/,
                                                     uint32_t axis, wl_fixed_t value)
{
    InternalWindow* window = s_Data.Wayland.PointerFocus;
    double x = 0.0, y = 0.0;

    //Wayland scroll events are in pointer motion coordinate space
    //(think two finger scroll).
    //The factor 10 is commonly used to convert to "scroll step" means 1.0
    constexpr double scrollFactor = 1.0 / 10.0;

    if(!window)
        return;

    TRAP_ASSERT(axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL || axis == WL_POINTER_AXIS_VERTICAL_SCROLL);

    if(axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL)
        x = -wl_fixed_to_double(value) * scrollFactor;
    else if(axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
        y = -wl_fixed_to_double(value) * scrollFactor;

    InputScroll(window, x, y);
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

        //Note this pointer will be acquired by a TRAP::Scope in OutputHandleDone()
        InternalMonitor* monitor = new InternalMonitor;
        std::memset(monitor, 0, sizeof(InternalMonitor));
        monitor->Wayland.Scale = 1;
        monitor->Wayland.Output = output;
        monitor->Wayland.Name = name;

        //The actual name of this output will be set in the geometry handler
        wl_output_add_listener(output, &OutputListener, monitor);
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

void TRAP::INTERNAL::WindowingAPI::RegistryHandleGlobalRemove(void* /*userData*/, wl_registry* /*registry*/, uint32_t name)
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

std::string TRAP::INTERNAL::WindowingAPI::ReadDataOfferAsString(wl_data_offer* offer, const char* mimeType)
{
    std::array<int32_t, 2> fds{};

    if(pipe2(fds.data(), O_CLOEXEC) == -1)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create pipe for data offer: " + Utils::String::GetStrError());
        return {};
    }

    wl_data_offer_receive(offer, mimeType, fds[1]);
    FlushDisplay();
    close(fds[1]);

    std::string str = nullptr;
    std::size_t size = 0;
    std::size_t length = 0;

    while(true)
    {
        const std::size_t readSize = 4096;
        const std::size_t requiredSize = length + readSize;
        if(requiredSize > size)
        {
            str.resize(requiredSize);
            size = requiredSize;
        }

        const ssize_t result = read(fds[0], str.data() + length, readSize);
        if(result == 0)
            break;
        else if(result == -1)
        {
            if(errno == EINTR)
                continue;

            InputError(Error::Platform_Error, "[Wayland] Failed to read data offer pipe: " + Utils::String::GetStrError());
            close(fds[0]);
            return nullptr;
        }

        length += result;
    }

    close(fds[0]);

    str.resize(length);

    return str;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::FlushDisplay()
{
    while(s_Data.Wayland.WaylandClient.DisplayFlush(s_Data.Wayland.DisplayWL) == -1)
    {
        if(errno != EAGAIN)
            return false;

        pollfd fd = {s_Data.Wayland.WaylandClient.DisplayGetFD(s_Data.Wayland.DisplayWL), POLLOUT, 0};

        while(poll(&fd, 1, -1) == -1)
        {
            if(errno != EINTR && errno != EAGAIN)
                return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetCursorWayland(InternalWindow* window, const std::string& name)
{
    wl_surface* surface = s_Data.Wayland.CursorSurface;
    wl_cursor_theme* theme = s_Data.Wayland.CursorTheme;
    int32_t scale = 1;

    if(window->Wayland.Scale > 1 && s_Data.Wayland.CursorThemeHiDPI)
    {
        //We only support up to scale=2 for now, since libwayland-cursor
        //requires us to load a different theme for each size.
        scale = 2;
        theme = s_Data.Wayland.CursorThemeHiDPI;
    }

    wl_cursor* cursor = s_Data.Wayland.WaylandCursor.ThemeGetCursor(theme, name.c_str());
    if(!cursor)
    {
        InputError(Error::Cursor_Unavailable, "[Wayland] Standard cursor shape unavailable");
        return;
    }

    wl_cursor_image* image = cursor->images[0];

    if(!image)
        return;

    wl_buffer* buffer = s_Data.Wayland.WaylandCursor.ImageGetBuffer(image);
    if(!buffer)
        return;

    wl_pointer_set_cursor(s_Data.Wayland.Pointer, s_Data.Wayland.PointerEnterSerial,
                          surface, image->hotspot_x / scale, image->hotspot_y / scale);
    wl_surface_set_buffer_scale(surface, scale);
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, image->width, image->height);
    wl_surface_commit(surface);

    s_Data.Wayland.CursorPreviousName = name;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::InternalWindow* TRAP::INTERNAL::WindowingAPI::FindWindowFromDecorationSurface(wl_surface* surface, TRAPDecorationSideWayland& which)
{
    for(InternalWindow* window : s_Data.WindowList)
    {
        if(surface == window->Wayland.Decorations.Top.surface)
        {
            which = TRAPDecorationSideWayland::TopDecoration;
            return window;
        }
        if(surface == window->Wayland.Decorations.Left.surface)
        {
            which = TRAPDecorationSideWayland::LeftDecoration;
            return window;
        }
        if(surface == window->Wayland.Decorations.Right.surface)
        {
            which = TRAPDecorationSideWayland::RightDecoration;
            return window;
        }
        if(surface == window->Wayland.Decorations.Bottom.surface)
        {
            which = TRAPDecorationSideWayland::BottomDecoration;
            return window;
        }
    }

    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputTextWayland(InternalWindow* window, uint32_t scanCode)
{
    const xkb_keysym_t* keysyms;
    const xkb_keycode_t keycode = scanCode + 8;

    if(s_Data.Wayland.WaylandXKB.StateKeyGetSyms(s_Data.Wayland.WaylandXKB.State, keycode, &keysyms) == 1)
    {
        const xkb_keysym_t keysym = ComposeSymbol(keysyms[0]);
        const uint32_t codePoint = KeySymToUnicode(keysym);
        if(codePoint != 0xFFFFFFFFu)
            InputChar(window, codePoint);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

xkb_keysym_t TRAP::INTERNAL::WindowingAPI::ComposeSymbol(xkb_keysym_t sym)
{
    if(sym == XKB_KEY_NoSymbol || !s_Data.Wayland.WaylandXKB.ComposeState)
        return sym;
    if(s_Data.Wayland.WaylandXKB.ComposeStateFeed(s_Data.Wayland.WaylandXKB.ComposeState, sym) != XKB_COMPOSE_FEED_ACCEPTED)
        return sym;

    switch(s_Data.Wayland.WaylandXKB.ComposeStateGetStatus(s_Data.Wayland.WaylandXKB.ComposeState))
    {
    case XKB_COMPOSE_COMPOSED:
        return s_Data.Wayland.WaylandXKB.ComposeStateGetOneSym(s_Data.Wayland.WaylandXKB.ComposeState);

    case XKB_COMPOSE_COMPOSING:
    case XKB_COMPOSE_CANCELLED:
        return XKB_KEY_NoSymbol;

    case XKB_COMPOSE_NOTHING:
    default:
        return sym;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::UpdateContentScaleWayland(InternalWindow* window)
{
    if(s_Data.Wayland.CompositorVersion < WL_SURFACE_SET_BUFFER_SCALE_SINCE_VERSION)
        return;

    //Get the scale factor from the highest scale monitor.
    int32_t maxScale = 1;
    for(const auto& monitor : window->Wayland.Monitors)
        maxScale = TRAP::Math::Max(monitor->Wayland.Scale, maxScale);

    //Only change the framebuffer size if the scale changed.
    if(window->Wayland.Scale != maxScale)
    {
        window->Wayland.Scale = maxScale;
        wl_surface_set_buffer_scale(window->Wayland.Surface, maxScale);
        InputWindowContentScale(window, maxScale, maxScale);
        ResizeWindowWayland(window);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::ResizeWindowWayland(InternalWindow* window)
{
    int32_t scale = window->Wayland.Scale;
    int32_t scaledWidth = window->Width * scale;
    int32_t scaledHeight = window->Height * scale;

    if(!window->Transparent)
        SetContentAreaOpaqueWayland(window);
    InputFrameBufferSize(window, scaledWidth, scaledHeight);

    if(!window->Wayland.Decorations.Top.surface)
        return;

    wp_viewport_set_destination(window->Wayland.Decorations.Top.viewport, window->Width, TRAP_CAPTION_HEIGHT);
    wl_surface_commit(window->Wayland.Decorations.Top.surface);

    wp_viewport_set_destination(window->Wayland.Decorations.Left.viewport, TRAP_BORDER_SIZE, window->Height + TRAP_CAPTION_HEIGHT);
    wl_surface_commit(window->Wayland.Decorations.Left.surface);

    wl_subsurface_set_position(window->Wayland.Decorations.Right.subsurface, window->Width, -TRAP_CAPTION_HEIGHT);
    wp_viewport_set_destination(window->Wayland.Decorations.Right.viewport, TRAP_BORDER_SIZE, window->Height + TRAP_CAPTION_HEIGHT);
    wl_surface_commit(window->Wayland.Decorations.Right.surface);

    wl_subsurface_set_position(window->Wayland.Decorations.Bottom.subsurface, -TRAP_BORDER_SIZE, window->Height);
    wp_viewport_set_destination(window->Wayland.Decorations.Bottom.viewport, window->Width + TRAP_BORDER_SIZE * 2, TRAP_BORDER_SIZE);
    wl_surface_commit(window->Wayland.Decorations.Bottom.surface);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetContentAreaOpaqueWayland(InternalWindow* window)
{
    wl_region* region = wl_compositor_create_region(s_Data.Wayland.Compositor);
    if(!region)
        return;

    wl_region_add(region, 0, 0, window->Width, window->Height);
    wl_surface_set_opaque_region(window->Wayland.Surface, region);
    wl_region_destroy(region);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::ReleaseMonitorWayland(InternalWindow* window)
{
    if(window->Wayland.XDG.TopLevel)
        xdg_toplevel_unset_fullscreen(window->Wayland.XDG.TopLevel);

    SetIdleInhibitorWayland(window, false);

    if(window->Wayland.XDG.Decorationmode != ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE)
    {
        if(window->Decorated)
            CreateFallbackDecorationsWayland(window);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetIdleInhibitorWayland(InternalWindow* window, const bool enable)
{
    if(enable && !window->Wayland.IdleInhibitor && s_Data.Wayland.IdleInhibitManager)
    {
        window->Wayland.IdleInhibitor = zwp_idle_inhibit_manager_v1_create_inhibitor(s_Data.Wayland.IdleInhibitManager, window->Wayland.Surface);
        if(!window->Wayland.IdleInhibitor)
            InputError(Error::Platform_Error, "[Wayland] Failed to create idle inhibitor");
    }
    else if(!enable && window->Wayland.IdleInhibitor)
    {
        zwp_idle_inhibitor_v1_destroy(window->Wayland.IdleInhibitor);
        window->Wayland.IdleInhibitor = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CreateFallbackDecorationsWayland(InternalWindow* window)
{
    std::vector<uint8_t> data{224, 244, 244, 255};
    Scope<Image> img = Image::LoadFromMemory(1, 1, Image::ColorFormat::RGBA, data);

    if(!s_Data.Wayland.Viewporter)
        return;

    if(!window->Wayland.Decorations.Buffer)
        window->Wayland.Decorations.Buffer = CreateShmBufferWayland(img.get());
    if(!window->Wayland.Decorations.Buffer)
        return;

    CreateFallbackDecorationWayland(window->Wayland.Decorations.Top, window->Wayland.Surface, window->Wayland.Decorations.Buffer, 0, -TRAP_CAPTION_HEIGHT, window->Width, TRAP_CAPTION_HEIGHT);
    CreateFallbackDecorationWayland(window->Wayland.Decorations.Left, window->Wayland.Surface, window->Wayland.Decorations.Buffer, -TRAP_BORDER_SIZE, -TRAP_CAPTION_HEIGHT, TRAP_BORDER_SIZE, window->Height + TRAP_CAPTION_HEIGHT);
    CreateFallbackDecorationWayland(window->Wayland.Decorations.Right, window->Wayland.Surface, window->Wayland.Decorations.Buffer, window->Width, -TRAP_CAPTION_HEIGHT, TRAP_BORDER_SIZE, window->Height + TRAP_CAPTION_HEIGHT);
    CreateFallbackDecorationWayland(window->Wayland.Decorations.Bottom, window->Wayland.Surface, window->Wayland.Decorations.Buffer, -TRAP_BORDER_SIZE, window->Height, window->Width + TRAP_BORDER_SIZE * 2, TRAP_BORDER_SIZE);
}

//-------------------------------------------------------------------------------------------------------------------//

wl_buffer* TRAP::INTERNAL::WindowingAPI::CreateShmBufferWayland(const Image* image)
{
    const int32_t stride = image->GetWidth() * 4;
    const int32_t length = image->GetWidth() * image->GetHeight() * 4;

    const int32_t fd = CreateAnonymousFileWayland(length);
    if(fd < 0)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create buffer file of size " +
                   std::to_string(length) + ": " + Utils::String::GetStrError());
        return nullptr;
    }

    void* data = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to map file: " + Utils::String::GetStrError());
        close(fd);
        return nullptr;
    }

    wl_shm_pool* pool = wl_shm_create_pool(s_Data.Wayland.Shm, fd, length);

    close(fd);

    const uint8_t* source = static_cast<const uint8_t*>(image->GetPixelData());
    uint8_t* target = static_cast<uint8_t*>(data);
    for(uint32_t i = 0; i < image->GetWidth() * image->GetHeight(); ++i, source += 4)
    {
        uint32_t alpha = source[3];

        *target++ = static_cast<uint8_t>((source[2] * alpha) / 255);
        *target++ = static_cast<uint8_t>((source[1] * alpha) / 255);
        *target++ = static_cast<uint8_t>((source[0] * alpha) / 255);
        *target++ = static_cast<uint8_t>(alpha);
    }

    wl_buffer* buffer = wl_shm_pool_create_buffer(pool, 0, image->GetWidth(), image->GetHeight(), stride, WL_SHM_FORMAT_ARGB8888);
    munmap(data, length);
    wl_shm_pool_destroy(pool);

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::INTERNAL::WindowingAPI::CreateAnonymousFileWayland(off_t size)
{
    static const char temp[] = "/trap-shared-XXXXXX";
    int32_t fd;
    int32_t ret;

#ifdef _GNU_SOURCE
    fd = memfd_create("trap-shared", MFD_CLOEXEC | MFD_ALLOW_SEALING);
    if(fd >= 0)
    {
        //We can add this seal before calling posix_fallocate(), as the file
        //is currently zero-sized anyway.

        //There is also no need to check for the return value, we couldn't do
        //anyting with it anyway.
        fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK | F_SEAL_SEAL);
    }
    else
#elif defined(SHM_ANON)
    fd = shm_open(SHM_ANON, O_RDWR | O_CLOEXEC, 0600);
    if(fd < 0)
#endif
    {
        const char* path = getenv("XDG_RUNTIME_DIR");
        if(!path)
        {
            errno = ENOENT;
            return -1;
        }

        std::string name = path;
        name += temp;

        fd = CreateTmpFileCloexec(name.data());
        if(fd < 0)
            return -1;
    }

#ifdef SHM_ANON
    //posix fallocate does not work on SHM descriptors
    ret = ftruncate(fd, size);
#else
    ret = posix_fallocate(fd, 0, size);
#endif
    if(ret != 0)
    {
        close(fd);
        errno = ret;
        return -1;
    }

    return fd;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::INTERNAL::WindowingAPI::CreateTmpFileCloexec(char* tmpName)
{
    int32_t fd = mkostemp(tmpName, O_CLOEXEC);
    if(fd >= 0)
        unlink(tmpName);

    return fd;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CreateFallbackDecorationWayland(TRAPDecorationWayland& decoration,
                                                                   wl_surface* parent, wl_buffer* buffer,
                                                                   int32_t x, int32_t y, int32_t width, int32_t height)
{
    decoration.surface = wl_compositor_create_surface(s_Data.Wayland.Compositor);
    decoration.subsurface = wl_subcompositor_get_subsurface(s_Data.Wayland.SubCompositor, decoration.surface, parent);
    wl_subsurface_set_position(decoration.subsurface, x, y);
    decoration.viewport = wp_viewporter_get_viewport(s_Data.Wayland.Viewporter, decoration.surface);
    wp_viewport_set_destination(decoration.viewport, width, height);
    wl_surface_attach(decoration.surface, buffer, 0, 0);

    wl_region* region = wl_compositor_create_region(s_Data.Wayland.Compositor);
    wl_region_add(region, 0, 0, width, height);
    wl_surface_set_opaque_region(decoration.surface, region);
    wl_surface_commit(decoration.surface);
    wl_region_destroy(region);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::AcquireMonitorWayland(InternalWindow* window)
{
    if(window->Wayland.XDG.TopLevel)
        xdg_toplevel_set_fullscreen(window->Wayland.XDG.TopLevel, window->Monitor->Wayland.Output);

    SetIdleInhibitorWayland(window, true);

    if(window->Wayland.Decorations.Top.surface)
        DestroyFallbackDecorationsWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyFallbackDecorationsWayland(InternalWindow* window)
{
    DestroyFallbackDecorationWayland(window->Wayland.Decorations.Top);
    DestroyFallbackDecorationWayland(window->Wayland.Decorations.Left);
    DestroyFallbackDecorationWayland(window->Wayland.Decorations.Right);
    DestroyFallbackDecorationWayland(window->Wayland.Decorations.Bottom);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyFallbackDecorationWayland(TRAPDecorationWayland& decoration)
{
    if(decoration.subsurface)
        wl_subsurface_destroy(decoration.subsurface);
    if(decoration.surface)
        wl_surface_destroy(decoration.surface);
    if(decoration.viewport)
        wp_viewport_destroy(decoration.viewport);
    decoration.surface = nullptr;
    decoration.subsurface = nullptr;
    decoration.viewport = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//


TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModeWayland(const InternalMonitor* monitor)
{
    return monitor->CurrentMode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSizeWayland(const InternalWindow* window, int32_t& width,
                                                                int32_t& height)
{
    width = window->Width;
    height = window->Height;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPosWayland(const InternalWindow* /*window*/, const int32_t /*xPos*/,
                                                               const int32_t /*yPos*/)
{
    //A Wayland client can't set its position, so just warn

    InputError(Error::Feature_Unavailable, "[Wayland] The platform doesn't support setting the window position");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorWayland(InternalWindow* window, InternalMonitor* monitor,
														           const int32_t /*xPos*/, const int32_t /*yPos*/,
                                                                   const int32_t width, const int32_t height,
                                                                   const int32_t /*refreshRate*/)
{
    if(window->Monitor == monitor)
    {
        if(!monitor)
            PlatformSetWindowSizeWayland(window, width, height);

        return;
    }

    if(window->Monitor)
        ReleaseMonitorWayland(window);

    window->Monitor = monitor;

    if(window->Monitor)
        AcquireMonitorWayland(window);
    else
        PlatformSetWindowSizeWayland(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderlessWayland(InternalWindow* window,
                                                                             InternalMonitor* monitor)
{
    //TODO
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