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

#define types _TRAP_xdg_activation_types
#include "wayland-xdg-activation-v1-client-protocol-code.h"
#undef types

#define types _TRAP_content_type_types
#include "wayland-content-type-v1-client-protocol-code.h"
#undef types

using namespace std::string_view_literals;

static constexpr int32_t TRAP_BORDER_SIZE = 4;
static constexpr int32_t TRAP_CAPTION_HEIGHT = 24;

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataSourceHandleTarget([[maybe_unused]] void* const userData,
                                                          wl_data_source* const source,
                                                          [[maybe_unused]] const char* const mimeType)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    if(s_Data.Wayland.SelectionSource != source)
    {
        InputError(Error::Platform_Error, "[Wayland] Unknown clipboard data source");
        return;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataSourceHandleSend([[maybe_unused]] void* const userData,
                                                        wl_data_source* const source,
                                                        const char* const mimeType, const int32_t fd)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    //Ignore it if this is an outdated or invalid request
    if(s_Data.Wayland.SelectionSource != source ||
       mimeType != "text/plain;charset=utf-8"sv)
    {
        close(fd);
        return;
    }

    const char* string = s_Data.ClipboardString.c_str();
    std::size_t length = s_Data.ClipboardString.size();

    while(length > 0)
    {
        const ssize_t result = write(fd, string, length);
        if(result == -1)
        {
            if(errno == EINTR)
                continue;

            InputError(Error::Platform_Error, "[Wayland] Error while writing the clipboard: " +
                                              Utils::String::GetStrError());
            break;
        }

        length -= NumericCast<std::size_t>(result);
        string += result;
    }

    close(fd);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataSourceHandleCancelled([[maybe_unused]] void* const userData,
                                                             wl_data_source* const source)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(source != nullptr)
        wl_data_source_destroy(source);

    if(s_Data.Wayland.SelectionSource != source)
        return;

    s_Data.Wayland.SelectionSource = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::RelativePointerHandleRelativeMotion(void* const userData,
                                                                       [[maybe_unused]] zwp_relative_pointer_v1* const relPointer,
                                                                       [[maybe_unused]] const uint32_t timeHi,
                                                                       [[maybe_unused]] const uint32_t timeLo,
                                                                       const wl_fixed_t dx, const wl_fixed_t dy,
                                                                       const wl_fixed_t dxUnaccel,
                                                                       const wl_fixed_t dyUnaccel)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::RelativePointerHandleRelativeMotion(): User data pointer is nullptr!");

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    double xPos = window->VirtualCursorPosX;
    double yPos = window->VirtualCursorPosY;

    if(window->cursorMode != CursorMode::Disabled)
        return;

    if(window->RawMouseMotion)
    {
        xPos += wl_fixed_to_double(dxUnaccel);
        yPos += wl_fixed_to_double(dyUnaccel);
    }
    else
    {
        xPos += wl_fixed_to_double(dx);
        yPos += wl_fixed_to_double(dy);
    }

    InputCursorPos(*window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SurfaceHandleEnter(void* const userData,
                                                      [[maybe_unused]] wl_surface* const surface,
                                                      wl_output* const output)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(output, "WindowingAPI::SurfaceHandleEnter(): wl_output is nullptr!");

    if(s_Data.Wayland.WaylandClient.ProxyGetTag(reinterpret_cast<wl_proxy*>(output)) != &s_Data.Wayland.TagCStr)
        return;

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    const InternalMonitor* const monitor = static_cast<InternalMonitor*>(wl_output_get_user_data(output));
    if((window == nullptr) || (monitor == nullptr))
        return;

    window->Wayland.Scales.emplace_back(TRAPScaleWayland{output, monitor->Wayland.ContentScale});

    UpdateContentScaleWayland(*window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SurfaceHandleLeave(void* const userData,
                                                      [[maybe_unused]] wl_surface* const surface,
                                                      wl_output* const output)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::SurfaceHandleLeave(): User data pointer is nullptr!");
	TRAP_ASSERT(output, "WindowingAPI::SurfaceHandleLeave(): wl_output is nullptr!");

    if(s_Data.Wayland.WaylandClient.ProxyGetTag(reinterpret_cast<wl_proxy*>(output)) != &s_Data.Wayland.TagCStr)
        return;

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    for(auto& scale : window->Wayland.Scales)
    {
        if(scale.output == output)
        {
            scale = window->Wayland.Scales.back(); //Overwrite scale with last element in vector
            window->Wayland.Scales.pop_back(); //Remove last element from vector
            break;
        }
    }

    UpdateContentScaleWayland(*window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::XDGDecorationHandleConfigure(void* const userData,
                                                                [[maybe_unused]] zxdg_toplevel_decoration_v1* const decoration,
                                                                const uint32_t mode)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::XDGDecorationHandleConfigure(): User data pointer is nullptr!");

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    window->Wayland.XDG.DecorationMode = mode;

    if(mode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE)
    {
        if(window->Decorated && (window->Monitor == nullptr))
            CreateFallbackDecorationsWayland(*window);
    }
    else
        DestroyFallbackDecorationsWayland(*window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::XDGTopLevelHandleConfigure(void* const userData,
                                                              [[maybe_unused]] xdg_toplevel* const topLevel,
                                                              const int32_t width, const int32_t height,
                                                              wl_array* const states)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::XDGTopLevelHandleConfigure(): User data pointer is nullptr!");
	TRAP_ASSERT(states, "WindowingAPI::XDGTopLevelHandleConfigure(): wl_array is nullptr!");

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    window->Wayland.Pending.Activated = false;
    window->Wayland.Pending.Maximized = false;
    window->Wayland.Pending.Fullscreen = false;

    uint32_t* state = nullptr;
    for (state = static_cast<uint32_t*>((states)->data);
         reinterpret_cast<const char*>(state)<(static_cast<const char*>((states)->data) + (states)->size);
         state++)
    {
        switch(*state)
        {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
            window->Wayland.Pending.Maximized = true;
            break;
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            window->Wayland.Pending.Fullscreen = true;
            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            window->Wayland.Pending.Activated = true;
            break;
        }
    }

    if((width != 0) && (height != 0))
    {
        if(window->Wayland.Decorations.Top.surface != nullptr)
        {
            window->Wayland.Pending.Width = TRAP::Math::Max(0, width - TRAP_BORDER_SIZE * 2);
            window->Wayland.Pending.Height = TRAP::Math::Max(0, height - TRAP_BORDER_SIZE - TRAP_CAPTION_HEIGHT);
        }
        else
        {
            window->Wayland.Pending.Width = width;
            window->Wayland.Pending.Height = height;
        }
    }
    else
    {
        window->Wayland.Pending.Width = window->Width;
        window->Wayland.Pending.Height = window->Height;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::XDGTopLevelHandleClose(void* const userData,
                                                          [[maybe_unused]] xdg_toplevel* const topLevel)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::XDGTopLevelHandleClose(): User data pointer is nullptr!");

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    InputWindowCloseRequest(*window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::XDGSurfaceHandleConfigure(void* const userData, xdg_surface* const surface,
                                                             const uint32_t serial)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::XDGSurfaceHandleConfigure(): User data pointer is nullptr!");
	TRAP_ASSERT(surface, "WindowingAPI::XDGSurfaceHandleConfigure(): xdg_surface is nullptr!");

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    xdg_surface_ack_configure(surface, serial);

    if(window->Wayland.Activated != window->Wayland.Pending.Activated)
        window->Wayland.Activated = window->Wayland.Pending.Activated;

    if(window->Maximized != window->Wayland.Pending.Maximized)
    {
        window->Maximized = window->Wayland.Pending.Maximized;
        InputWindowMaximize(*window, window->Maximized);
    }

    window->Wayland.Fullscreen = window->Wayland.Pending.Fullscreen;

    const int32_t width = window->Wayland.Pending.Width;
    const int32_t height = window->Wayland.Pending.Height;

    if(width != window->Width || height != window->Height)
    {
        window->Width = width;
        window->Height = height;
        ResizeWindowWayland(*window);

        InputWindowSize(*window, width, height);
    }

    if(!window->Wayland.Visible)
    {
        //Allow the window to be mapped only if it either has no XDG
        //decorations or they have already received a configure vent
        if((window->Wayland.XDG.Decoration == nullptr) || (window->Wayland.XDG.DecorationMode != 0u))
            window->Wayland.Visible = true;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::XDGActivationHandleDone(void* const userData,
                                                           xdg_activation_token_v1* const activationToken,
                                                           const char* const token)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::XDGActivationHandleDone(): User data pointer is nullptr!");
	TRAP_ASSERT(activationToken, "WindowingAPI::XDGActivationHandleDone(): xdg_activation_token_v1 is nullptr!");

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if((window == nullptr) || activationToken != window->Wayland.ActivationToken)
        return;

    xdg_activation_v1_activate(s_Data.Wayland.ActivationManager, token, window->Wayland.Surface);
    xdg_activation_token_v1_destroy(window->Wayland.ActivationToken);
    window->Wayland.ActivationToken = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataOfferHandleOffer([[maybe_unused]] void* const userData,
                                                        wl_data_offer* const offer, const char* const mimeType)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(offer, "WindowingAPI::DataOfferHandleOffer(): wl_data_offer is nullptr!");

    for(auto& off : s_Data.Wayland.Offers)
    {
        if(off.offer != offer)
            continue;

        if("text/plain;charset=utf-8"sv.compare(mimeType) == 0)
            off.text_plain_utf8 = true;
        else if("text/uri-list"sv.compare(mimeType) == 0)
            off.text_uri_list = true;
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleDataOffer([[maybe_unused]] void* const userData,
                                                             [[maybe_unused]] wl_data_device* const device,
                                                             wl_data_offer* const offer)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(offer, "WindowingAPI::DataDeviceHandleDataOffer(): wl_data_offer is nullptr!");

    s_Data.Wayland.Offers.push_back(TRAPOfferWayland{offer, false, false});
    wl_data_offer_add_listener(offer, &DataOfferListener, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleEnter([[maybe_unused]] void* const userData,
                                                         [[maybe_unused]] wl_data_device* const device,
                                                         const uint32_t serial, wl_surface* const surface,
                                                         [[maybe_unused]] const wl_fixed_t xPos,
                                                         [[maybe_unused]] const wl_fixed_t yPos,
                                                         wl_data_offer* const offer)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(surface, "WindowingAPI::DataDeviceHandleEnter(): wl_surface is nullptr!");
	TRAP_ASSERT(offer, "WindowingAPI::DataDeviceHandleEnter(): wl_data_offer is nullptr!");

    if(s_Data.Wayland.DragOffer != nullptr)
    {
        wl_data_offer_destroy(s_Data.Wayland.DragOffer);
        s_Data.Wayland.DragOffer = nullptr;
        s_Data.Wayland.DragFocus = nullptr;
    }

    for(auto& off : s_Data.Wayland.Offers)
    {
        if(off.offer != offer)
            continue;

        InternalWindow* window = nullptr;

        if(surface != nullptr)
        {
            if(s_Data.Wayland.WaylandClient.ProxyGetTag(reinterpret_cast<wl_proxy*>(surface)) == &s_Data.Wayland.TagCStr)
                window = static_cast<InternalWindow*>(wl_surface_get_user_data(surface));
        }

        if((window != nullptr) && surface == window->Wayland.Surface && off.text_uri_list)
        {
            s_Data.Wayland.DragOffer = offer;
            s_Data.Wayland.DragFocus = window;
            s_Data.Wayland.DragSerial = serial;
        }

        off = s_Data.Wayland.Offers.back();
        s_Data.Wayland.Offers.pop_back();
        break;
    }

    if(s_Data.Wayland.WaylandClient.ProxyGetTag(reinterpret_cast<wl_proxy*>(surface)) != &s_Data.Wayland.TagCStr)
        return;

    if(s_Data.Wayland.DragOffer != nullptr)
        wl_data_offer_accept(offer, serial, "text/uri-list");
    else
    {
        wl_data_offer_accept(offer, serial, nullptr);
        wl_data_offer_destroy(offer);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleLeave([[maybe_unused]] void* const userData,
                                                         [[maybe_unused]] wl_data_device* const device)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.DragOffer != nullptr)
    {
        wl_data_offer_destroy(s_Data.Wayland.DragOffer);
        s_Data.Wayland.DragOffer = nullptr;
        s_Data.Wayland.DragFocus = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleDrop([[maybe_unused]] void* const userData,
                                                        [[maybe_unused]] wl_data_device* const device)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.DragOffer == nullptr)
        return;

    std::optional<std::string> string = ReadDataOfferAsString(*s_Data.Wayland.DragOffer, "text/uri-list");
    if(string && !string->empty())
    {
        const std::vector<std::string> paths = ParseUriList(*string);
        if(!paths.empty())
            InputDrop(*s_Data.Wayland.DragFocus, paths);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DataDeviceHandleSelection([[maybe_unused]] void* const userData,
                                                             [[maybe_unused]] wl_data_device* const device,
                                                             wl_data_offer* const offer)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.SelectionOffer != nullptr)
    {
        wl_data_offer_destroy(s_Data.Wayland.SelectionOffer);
        s_Data.Wayland.SelectionOffer = nullptr;
    }

    for(auto& off : s_Data.Wayland.Offers)
    {
        if(off.offer != offer)
            continue;

        if(off.text_plain_utf8)
            s_Data.Wayland.SelectionOffer = offer;
        else if(offer != nullptr)
            wl_data_offer_destroy(offer);

        off = s_Data.Wayland.Offers.back();
        s_Data.Wayland.Offers.pop_back();
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr std::array<std::pair<int32_t, int32_t>, 31> EmulatedVideoModes
{
    {
        //16:9 (1.77)
        { 7680, 4320 },
        { 6144, 3160 },
        { 5120, 2880 },
        { 4096, 2304 },
        { 3840, 2160 },
        { 3200, 1800 },
        { 2880, 1620 },
        { 2560, 1440 },
        { 2048, 1152 },
        { 1920, 1080 },
        { 1600, 900 },
        { 1368, 768 },
        { 1280, 720 },
        { 864, 486 },

        //16:10 (1.6)
        { 2560, 1600 },
        { 1920, 1200 },
        { 1680, 1050 },
        { 1440, 900 },
        { 1280, 800 },

        //3:2 (1.5)
        { 720, 480 },

        //4:3 (1.33)
        { 2048, 1536 },
        { 1920, 1440 },
        { 1600, 1200 },
        { 1440, 1080 },
        { 1400, 1050 },
        { 1280, 1024 },
        { 1280, 960 },
        { 1152, 864 },
        { 1024, 768 },
        { 800, 600 },
        { 640, 480 }
    }
};

void TRAP::INTERNAL::WindowingAPI::AddEmulatedVideoModes(InternalMonitor& monitor)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    const InternalVideoMode& nativeMode = monitor.NativeMode.value_or(monitor.CurrentMode);
    std::vector<InternalVideoMode>& modes = monitor.Modes;

    const bool rot90 = nativeMode.Width < nativeMode.Height; //Reverse width / height for portrait displays.

    InternalVideoMode mode{};
    mode.RedBits = 8;
    mode.GreenBits = 8;
    mode.BlueBits = 8;
    mode.RefreshRate = nativeMode.RefreshRate;

    for(const auto& [width, height] : EmulatedVideoModes)
    {
        if(rot90)
        {
            mode.Width = height;
            mode.Height = width;
        }
        else
        {
            mode.Width = width;
            mode.Height = height;
        }

        //Only add modes that are smaller than the native mode
        if((mode.Width < nativeMode.Width && mode.Height < nativeMode.Height) ||
           (mode.Width < nativeMode.Width && mode.Height == nativeMode.Height) ||
           (mode.Width == nativeMode.Width && mode.Height < nativeMode.Height))
        {
            modes.push_back(mode);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleGeometry(void* const userData,
                                                        [[maybe_unused]] wl_output* const output,
                                                        const int32_t xPos, const int32_t yPos,
                                                        [[maybe_unused]] const int32_t physicalWidth,
                                                        [[maybe_unused]] const int32_t physicalHeight,
                                                        [[maybe_unused]] const int32_t subpixel,
                                                        const char* const make,
                                                        const char* const model,
                                                        [[maybe_unused]] const int32_t transform)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::OutputHandleGeometry(): User data pointer is nullptr!");

    InternalMonitor* const monitor = static_cast<InternalMonitor*>(userData);
    if(monitor == nullptr)
        return;

    monitor->Wayland.X = xPos;
    monitor->Wayland.Y = yPos;

    if((make != nullptr) && (model != nullptr))
        monitor->Name = fmt::format("{} {}", make, model);
    else if(make != nullptr)
        monitor->Name = make;
    else if(model != nullptr)
        monitor->Name = model;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleMode(void* const userData, [[maybe_unused]] wl_output* const output,
                                                    const uint32_t flags, const int32_t width, const int32_t height,
                                                    const int32_t refresh)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::OutputHandleMode(): User data pointer is nullptr!");

    InternalMonitor* const monitor = static_cast<InternalMonitor*>(userData);
    if(monitor == nullptr)
        return;

    const InternalVideoMode mode{width, height, 8, 8, 8, refresh / 1000.0};

    const auto ele = std::ranges::find_if(monitor->Modes, [&mode](const InternalVideoMode& e)
    {
        return mode.Width == e.Width && mode.Height == e.Height && mode.RedBits == e.RedBits &&
               mode.GreenBits == e.GreenBits && mode.BlueBits == e.BlueBits && mode.RefreshRate == e.RefreshRate;
    });

    if(ele != monitor->Modes.cend())
    {
        if((flags & WL_OUTPUT_MODE_CURRENT) != 0u)
        {
            if(!monitor->NativeMode)
                monitor->NativeMode = mode;
            monitor->CurrentMode = mode;
        }
    }
    else
    {
        monitor->Modes.push_back(mode);

        if((flags & WL_OUTPUT_MODE_CURRENT) != 0u)
        {
            if(!monitor->NativeMode)
                monitor->NativeMode = mode;
            monitor->CurrentMode = monitor->Modes.back();
            AddEmulatedVideoModes(*monitor);
        }
    }

	std::ranges::sort(monitor->Modes, CompareVideoModes);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleDone(void* const userData, [[maybe_unused]] wl_output* const output)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::OutputHandleDone(): User data pointer is nullptr!");

    InternalMonitor* const monitor = static_cast<InternalMonitor*>(userData);
    if(monitor == nullptr)
        return;

    //Note that this is valid here because the monitor itself got allocated via new in RegistryHandleGlobal()
    TRAP::Scope<InternalMonitor> scopedMonitor(monitor);

    //Check if monitor already exists
    if(std::ranges::contains(s_Data.Monitors, scopedMonitor))
        return;

    InputMonitor(std::move(scopedMonitor), true, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::OutputHandleScale(void* const userData, [[maybe_unused]] wl_output* const output,
                                                     const int32_t factor)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    TRAP_ASSERT(userData, "WindowingAPI::OutputHandleScale(): User data pointer is nullptr!");

    InternalMonitor* const monitor = static_cast<InternalMonitor*>(userData);
    if(monitor == nullptr)
        return;

    monitor->Wayland.ContentScale = factor;

    for(const TRAP::Scope<InternalWindow>& window : s_Data.WindowList)
    {
        for(TRAPScaleWayland& scale : window->Wayland.Scales)
        {
            if(scale.output == monitor->Wayland.Output)
            {
                scale.factor = monitor->Wayland.ContentScale;
                UpdateContentScaleWayland(*window);
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
void TRAP::INTERNAL::WindowingAPI::OutputHandleName(void* const userData, [[maybe_unused]] wl_output* const output,
                                                    const char* const name)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    TRAP_ASSERT(userData, "WindowingAPI::OutputHandleName(): User data pointer is nullptr!");

    InternalMonitor* const monitor = static_cast<InternalMonitor*>(userData);
    if(monitor == nullptr)
        return;

    if(name != nullptr)
        monitor->Name = name;
}
#endif /*WL_OUTPUT_NAME_SINCE_VERSION*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleKeymap([[maybe_unused]] void* const userData,
                                                        [[maybe_unused]] wl_keyboard* const keyboard,
                                                        const uint32_t format, const int32_t fd,
                                                        const uint32_t size)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

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

    xkb_keymap* const keymap = s_Data.Wayland.WaylandXKB.KeyMapNewFromString(s_Data.Wayland.WaylandXKB.Context,
                                                                             mapStr,
                                                                             XKB_KEYMAP_FORMAT_TEXT_V1,
                                                                             XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(mapStr, size);
    close(fd);

    if(keymap == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to compile keymap");
        return;
    }

    xkb_state* const state = s_Data.Wayland.WaylandXKB.StateNew(keymap);
    if(state == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create XKB state");
        s_Data.Wayland.WaylandXKB.KeyMapUnref(keymap);
        return;
    }

    //Look up the preferred locale, falling back to "C" as default.
    const char* locale = getenv("LC_ALL");
    if(locale == nullptr)
        locale = getenv("LC_CTYPE");
    if(locale == nullptr)
        locale = getenv("LANG");
    if(locale == nullptr)
        locale = "C";

    xkb_compose_table* const composeTable = s_Data.Wayland.WaylandXKB.ComposeTableNewFromLocale(s_Data.Wayland.WaylandXKB.Context,
                                                                                                locale,
                                                                                                XKB_COMPOSE_COMPILE_NO_FLAGS);

    if(composeTable != nullptr)
    {
        xkb_compose_state* const composeState = s_Data.Wayland.WaylandXKB.ComposeStateNew(composeTable,
                                                                                          XKB_COMPOSE_STATE_NO_FLAGS);
        s_Data.Wayland.WaylandXKB.ComposeTableUnref(composeTable);
        if(composeState != nullptr)
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

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleEnter([[maybe_unused]] void* const userData,
                                                       [[maybe_unused]] wl_keyboard* const keyboard,
                                                       const uint32_t serial, wl_surface* const surface,
                                                       [[maybe_unused]] wl_array* const keys)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    //Happens in the case we just destroyed the surface.
    if(surface == nullptr)
        return;

    if(s_Data.Wayland.WaylandClient.ProxyGetTag(reinterpret_cast<wl_proxy*>(surface)) != &s_Data.Wayland.TagCStr)
        return;

    InternalWindow* const window = static_cast<InternalWindow*>(wl_surface_get_user_data(surface));
    if((window == nullptr) || surface != window->Wayland.Surface)
        return;

    s_Data.Wayland.Serial = serial;
    s_Data.Wayland.KeyboardFocus = window;
    InputWindowFocus(*window, true);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleLeave([[maybe_unused]] void* const userData,
                                                       [[maybe_unused]] wl_keyboard* const keyboard,
                                                       const uint32_t serial,
                                                       [[maybe_unused]] wl_surface* const surface)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InternalWindow* const window = s_Data.Wayland.KeyboardFocus;
    if(window == nullptr)
        return;

    itimerspec timer{};
    timerfd_settime(s_Data.Wayland.KeyRepeatTimerFD, 0, &timer, nullptr);

    s_Data.Wayland.Serial = serial;
    s_Data.Wayland.KeyboardFocus = nullptr;
    InputWindowFocus(*window, false);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleKey([[maybe_unused]] void* const userData,
                                                     [[maybe_unused]] wl_keyboard* const keyboard,
                                                     const uint32_t serial, [[maybe_unused]] const uint32_t time,
                                                     const uint32_t scanCode, const uint32_t state)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InternalWindow* const window = s_Data.Wayland.KeyboardFocus;
    if(window == nullptr)
        return;

    const Input::Key key = TranslateKey(NumericCast<int32_t>(scanCode));
    const Input::KeyState action = state == WL_KEYBOARD_KEY_STATE_PRESSED ? Input::KeyState::Pressed :
                                                                            Input::KeyState::Released;

    s_Data.Wayland.Serial = serial;

    itimerspec timer{};

    if(action == Input::KeyState::Pressed)
    {
        const xkb_keycode_t keycode = scanCode + 8;

        if((s_Data.Wayland.WaylandXKB.KeyMapKeyRepeats(s_Data.Wayland.WaylandXKB.KeyMap, keycode) != 0) &&
           s_Data.Wayland.KeyRepeatRate > 0)
        {
            s_Data.Wayland.KeyRepeatScancode = NumericCast<int32_t>(scanCode);
            if(s_Data.Wayland.KeyRepeatRate > 1)
                timer.it_interval.tv_nsec = 1000000000 / s_Data.Wayland.KeyRepeatRate;
            else
                timer.it_interval.tv_sec = 1;

            timer.it_value.tv_sec = s_Data.Wayland.KeyRepeatDelay / 1000;
            timer.it_value.tv_nsec = (static_cast<long>(s_Data.Wayland.KeyRepeatDelay % 1000) * 1000000);
        }
    }

    timerfd_settime(s_Data.Wayland.KeyRepeatTimerFD, 0, &timer, nullptr);

    InputKey(*window, key, NumericCast<int32_t>(scanCode), action);

    if(action == Input::KeyState::Pressed)
        InputTextWayland(*window, scanCode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::KeyboardHandleModifiers([[maybe_unused]] void* const userData,
                                                           [[maybe_unused]] wl_keyboard* const keyboard,
                                                           [[maybe_unused]] const uint32_t serial,
                                                           [[maybe_unused]] const uint32_t modsDepressed,
                                                           [[maybe_unused]] const uint32_t modsLatched,
                                                           [[maybe_unused]] const uint32_t modsLocked,
                                                           const uint32_t group)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.WaylandXKB.Group != group)
    {
        s_Data.Wayland.WaylandXKB.Group = group;
        InputKeyboardLayout();
    }
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
void TRAP::INTERNAL::WindowingAPI::KeyboardHandleRepeatInfo([[maybe_unused]] void* const userData,
                                                            wl_keyboard* const keyboard, const int32_t rate,
                                                            const int32_t delay)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(keyboard, "WindowingAPI::KeyboardHandleRepeatInfo(): wl_keyboard is nullptr!");

    if(keyboard != s_Data.Wayland.Keyboard)
        return;

    s_Data.Wayland.KeyRepeatRate = rate;
    s_Data.Wayland.KeyRepeatDelay = delay;
}
#endif /*WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleEnter([[maybe_unused]] void* const userData,
                                                      [[maybe_unused]] wl_pointer* const wlPointer,
                                                      const uint32_t serial, wl_surface* const surface,
                                                      [[maybe_unused]] const wl_fixed_t sXPos,
                                                      [[maybe_unused]] const wl_fixed_t sYPos)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    //Happens in the case we just destroyed the surface.
    if(surface == nullptr)
        return;

    if(s_Data.Wayland.WaylandClient.ProxyGetTag(reinterpret_cast<wl_proxy*>(surface)) != &s_Data.Wayland.TagCStr)
        return;

    InternalWindow* const window = static_cast<InternalWindow*>(wl_surface_get_user_data(surface));
    if(window == nullptr)
        return;

    if(surface == window->Wayland.Decorations.Top.surface)
        window->Wayland.Decorations.Focus = TRAPDecorationSideWayland::TopDecoration;
    else if(surface == window->Wayland.Decorations.Left.surface)
        window->Wayland.Decorations.Focus = TRAPDecorationSideWayland::LeftDecoration;
    else if(surface == window->Wayland.Decorations.Right.surface)
        window->Wayland.Decorations.Focus = TRAPDecorationSideWayland::RightDecoration;
    else if(surface == window->Wayland.Decorations.Bottom.surface)
        window->Wayland.Decorations.Focus = TRAPDecorationSideWayland::BottomDecoration;
    else
        window->Wayland.Decorations.Focus = TRAPDecorationSideWayland::MainWindow;

    s_Data.Wayland.Serial = serial;
    s_Data.Wayland.PointerEnterSerial = serial;
    s_Data.Wayland.PointerFocus = window;

    window->Wayland.Hovered = true;

    PlatformSetCursorWayland(*window, window->Cursor);
    InputCursorEnter(*window, true);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleLeave([[maybe_unused]] void* const userData,
                                                      [[maybe_unused]] wl_pointer* const wlPointer, const uint32_t serial,
                                                      wl_surface* const surface)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(surface == nullptr)
        return;

    if(s_Data.Wayland.WaylandClient.ProxyGetTag(reinterpret_cast<wl_proxy*>(surface)) != &s_Data.Wayland.TagCStr)
        return;

    InternalWindow* const window = s_Data.Wayland.PointerFocus;
    if(window == nullptr)
        return;

    window->Wayland.Hovered = false;

    s_Data.Wayland.Serial = serial;
    s_Data.Wayland.PointerFocus = nullptr;
    s_Data.Wayland.CursorPreviousName = "";

    InputCursorEnter(*window, false);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleMotion([[maybe_unused]] void* const userData,
                                                       [[maybe_unused]] wl_pointer* const wlPointer,
                                                       [[maybe_unused]] const uint32_t time, const wl_fixed_t sXPos,
                                                       const wl_fixed_t sYPos)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InternalWindow* const window = s_Data.Wayland.PointerFocus;
    std::string cursorName{};

    if(window == nullptr)
        return;

    if(window->cursorMode == CursorMode::Disabled)
        return;

    const double x = wl_fixed_to_double(sXPos);
    const double y = wl_fixed_to_double(sYPos);
    window->Wayland.CursorPosX = x;
    window->Wayland.CursorPosY = y;

    switch(window->Wayland.Decorations.Focus)
    {
    case TRAPDecorationSideWayland::MainWindow:
        s_Data.Wayland.CursorPreviousName = "";
        InputCursorPos(*window, x, y);
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
        TRAP_ASSERT(false, "WindowingAPI::PointerHandleMotion(): Unknown decoration side");
    }

    if(s_Data.Wayland.CursorPreviousName != cursorName)
        SetCursorWayland(*window, cursorName);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleButton([[maybe_unused]] void* const userData,
                                                       [[maybe_unused]] wl_pointer* const wlPointer,
                                                       const uint32_t serial, [[maybe_unused]] const uint32_t time,
                                                       const uint32_t button, const uint32_t state)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InternalWindow* const window = s_Data.Wayland.PointerFocus;
    uint32_t edges = XDG_TOPLEVEL_RESIZE_EDGE_NONE;

    if(window == nullptr)
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
            TRAP_ASSERT(false, "WindowingAPI::PointerHandleButton(): Unknown decoration side");
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
           (window->Wayland.XDG.TopLevel != nullptr))
        {
            xdg_toplevel_show_window_menu(window->Wayland.XDG.TopLevel, s_Data.Wayland.Seat,
                                          serial, NumericCast<int32_t>(window->Wayland.CursorPosX),
                                          NumericCast<int32_t>(window->Wayland.CursorPosY));
            return;
        }
    }

    //Don't pass the button to the user if it was related to a decoration
    if(window->Wayland.Decorations.Focus != TRAPDecorationSideWayland::MainWindow)
        return;

    s_Data.Wayland.Serial = serial;

    //Makes left, right and middle 0, 1 and 2. Overall order follows evdev codes
    const TRAP::Input::MouseButton btn = static_cast<Input::MouseButton>(button - BTN_LEFT);

    InputMouseClick(*window, btn, state == WL_POINTER_BUTTON_STATE_PRESSED ? Input::KeyState::Pressed :
                                                                             Input::KeyState::Released);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PointerHandleAxis([[maybe_unused]] void* const userData,
                                                     [[maybe_unused]] wl_pointer* const wlPointer,
                                                     [[maybe_unused]] const uint32_t time, const uint32_t axis,
                                                     const wl_fixed_t value)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    TRAP_ASSERT(axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL || axis == WL_POINTER_AXIS_VERTICAL_SCROLL,
                "WindowingAPI::PointerHandleAxis(): Unknown pointer axis");

    const InternalWindow* const window = s_Data.Wayland.PointerFocus;
    if(window == nullptr)
        return;

    double x = 0.0, y = 0.0;

    //Wayland scroll events are in pointer motion coordinate space
    //(think two finger scroll).
    //The factor 10 is commonly used to convert to "scroll step" means 1.0
    static constexpr double scrollFactor = 1.0 / 10.0;

    if(axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL)
        x = -wl_fixed_to_double(value) * scrollFactor;
    else if(axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
        y = -wl_fixed_to_double(value) * scrollFactor;

    InputScroll(*window, x, y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SeatHandleCapabilities([[maybe_unused]] void* const userData,
                                                          wl_seat* const seat, const uint32_t caps)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(seat, "WindowingAPI::SeatHandleCapabilities(): wl_seat is nullptr!");

    if(((caps & WL_SEAT_CAPABILITY_POINTER) != 0u) && (s_Data.Wayland.Pointer == nullptr))
    {
        s_Data.Wayland.Pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(s_Data.Wayland.Pointer, &PointerListener, nullptr);
    }
    else if(((caps & WL_SEAT_CAPABILITY_POINTER) == 0u) && (s_Data.Wayland.Pointer != nullptr))
    {
        wl_pointer_destroy(s_Data.Wayland.Pointer);
        s_Data.Wayland.Pointer = nullptr;
    }

    if(((caps & WL_SEAT_CAPABILITY_KEYBOARD) != 0u) && (s_Data.Wayland.Keyboard == nullptr))
    {
        s_Data.Wayland.Keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(s_Data.Wayland.Keyboard, &KeyboardListener, nullptr);
    }
    else if(((caps & WL_SEAT_CAPABILITY_KEYBOARD) == 0u) && (s_Data.Wayland.Keyboard != nullptr))
    {
        wl_keyboard_destroy(s_Data.Wayland.Keyboard);
        s_Data.Wayland.Keyboard = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::WMBaseHandlePing([[maybe_unused]] void* const userData,
                                                    xdg_wm_base* const wmBase, const uint32_t serial)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(wmBase, "WindowingAPI::WMBaseHandlePing(): xdg_wm_base is nullptr!");

    xdg_wm_base_pong(wmBase, serial);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::RegistryHandleGlobal([[maybe_unused]] void* const userData,
                                                        wl_registry* const registry,
                                                        const uint32_t name, const char* const interface,
                                                        uint32_t version)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(registry, "WindowingAPI::RegistryHandleGlobal(): wl_registry is nullptr!");

    if(interface == "wl_compositor"sv)
    {
        s_Data.Wayland.Compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name,
                                                                                 &wl_compositor_interface,
                                                                                 TRAP::Math::Min(3u, version)));
    }
    else if(interface == "wl_subcompositor"sv)
    {
        s_Data.Wayland.SubCompositor = static_cast<wl_subcompositor*>(wl_registry_bind(registry, name,
                                                                                       &wl_subcompositor_interface,
                                                                                       1));
    }
    else if(interface == "wl_shm"sv)
        s_Data.Wayland.Shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, 1));
    else if(interface == "wl_output"sv)
    {
        if(version < 2u)
        {
            InputError(Error::Platform_Error, "[Wayland] Unsupported output interface version");
            return;
        }

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
        version = TRAP::Math::Min(version, NumericCast<uint32_t>(WL_OUTPUT_NAME_SINCE_VERSION));
#else
        version = 2u;
#endif

        wl_output* const output = static_cast<wl_output*>(wl_registry_bind(s_Data.Wayland.Registry, name,
                                                                           &wl_output_interface, version));
        if(output == nullptr)
            return;

        //The actual name of this output will be set in the geometry handler
        //Note this pointer will be acquired by a TRAP::Scope in OutputHandleDone()
        InternalMonitor* monitor = new InternalMonitor();
        monitor->Wayland.ContentScale = 1;
        monitor->Wayland.Output = output;
        monitor->Wayland.Name = name;

        s_Data.Wayland.WaylandClient.ProxySetTag(reinterpret_cast<wl_proxy*>(output), &s_Data.Wayland.TagCStr);
        wl_output_add_listener(output, &OutputListener, monitor);
    }
    else if(interface == "wl_seat"sv)
    {
        if(s_Data.Wayland.Seat == nullptr)
        {
            s_Data.Wayland.Seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface,
                                                                         TRAP::Math::Min(4u, version)));
            wl_seat_add_listener(s_Data.Wayland.Seat, &SeatListener, nullptr);
        }
    }
    else if(interface == "wl_data_device_manager"sv)
    {
        if(s_Data.Wayland.DataDeviceManager == nullptr)
        {
            s_Data.Wayland.DataDeviceManager = static_cast<wl_data_device_manager*>(wl_registry_bind(registry, name,
                                                                                                     &wl_data_device_manager_interface,
                                                                                                     1));
        }
    }
    else if(interface == "xdg_wm_base"sv)
    {
        s_Data.Wayland.WMBase = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name,
                                                                           &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(s_Data.Wayland.WMBase, &WMBaseListener, nullptr);
    }
    else if(interface == "zxdg_decoration_manager_v1"sv)
    {
        s_Data.Wayland.DecorationManager = static_cast<zxdg_decoration_manager_v1*>(wl_registry_bind(registry, name,
                                                                                                     &zxdg_decoration_manager_v1_interface,
                                                                                                     1));
    }
    else if(interface == "wp_viewporter"sv)
    {
        s_Data.Wayland.Viewporter = static_cast<wp_viewporter*>(wl_registry_bind(registry, name,
                                                                                 &wp_viewporter_interface, 1));
    }
    else if(interface == "zwp_relative_pointer_manager_v1"sv)
    {
        s_Data.Wayland.RelativePointerManager = static_cast<zwp_relative_pointer_manager_v1*>(wl_registry_bind(registry,
                                                                                                               name,
                                                                                                               &zwp_relative_pointer_manager_v1_interface,
                                                                                                               1));
    }
    else if(interface == "zwp_pointer_constraints_v1"sv)
    {
        s_Data.Wayland.PointerConstraints = static_cast<zwp_pointer_constraints_v1*>(wl_registry_bind(registry, name,
                                                                                                      &zwp_pointer_constraints_v1_interface,
                                                                                                      1));
    }
    else if(interface == "zwp_idle_inhibit_manager_v1"sv)
    {
        s_Data.Wayland.IdleInhibitManager = static_cast<zwp_idle_inhibit_manager_v1*>(wl_registry_bind(registry, name,
                                                                                      &zwp_idle_inhibit_manager_v1_interface,
                                                                                      1));
    }
    else if(interface == "xdg_activation_v1"sv)
    {
        s_Data.Wayland.ActivationManager = static_cast<xdg_activation_v1*>(wl_registry_bind(registry, name,
                                                                                            &xdg_activation_v1_interface,
                                                                                            1));
    }
    else if(interface == "wp_content_type_manager_v1"sv)
    {
        s_Data.Wayland.ContentTypeManager = static_cast<wp_content_type_manager_v1*>(wl_registry_bind(registry, name,
                                                                                                      &wp_content_type_manager_v1_interface,
                                                                                                      1));
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::RegistryHandleGlobalRemove([[maybe_unused]] void* const userData,
                                                              [[maybe_unused]] wl_registry* const registry,
                                                              const uint32_t name)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

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

void TRAP::INTERNAL::WindowingAPI::LibDecorHandleError([[maybe_unused]] libdecor* const context,
                                                       const libdecor_error error, const char* const message)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    if(message == nullptr)
        return;

    InputError(Error::Platform_Error, fmt::format("[Wayland] libdecor error: {} ({})", message, std::to_underlying(error)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CreateKeyTablesWayland()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::ranges::fill(s_Data.KeyCodes, Input::Key::Unknown);
	std::ranges::fill(s_Data.ScanCodes, -1);

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

    for (std::size_t scancode = 0; scancode < s_Data.KeyCodes.size();  scancode++)
    {
        if (s_Data.KeyCodes[scancode] != TRAP::Input::Key::Unknown)
            s_Data.ScanCodes[NumericCast<uint32_t>(std::to_underlying(s_Data.KeyCodes[scancode]))] = NumericCast<int16_t>(scancode);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::LoadCursorThemeWayland()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    int32_t cursorSize = 16;

    const char* const sizeString = getenv("XCURSOR_SIZE");
    if(sizeString != nullptr)
    {
        errno = 0;
        const long cursorSizeLong = std::stol(sizeString);
        if(errno == 0 && cursorSizeLong > 0 && cursorSizeLong < std::numeric_limits<int32_t>::max())
            cursorSize = NumericCast<int32_t>(cursorSizeLong);
    }

    const char* const themeName = getenv("XCURSOR_THEME");

    s_Data.Wayland.CursorTheme = s_Data.Wayland.WaylandCursor.ThemeLoad(themeName, cursorSize, s_Data.Wayland.Shm);
    if(s_Data.Wayland.CursorTheme == nullptr)
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

std::optional<std::string> TRAP::INTERNAL::WindowingAPI::ReadDataOfferAsString(wl_data_offer& offer,
                                                                               const std::string_view mimeType)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    std::array<int32_t, 2> fds{};

    if(pipe2(fds.data(), O_CLOEXEC) == -1)
    {
        InputError(Error::Platform_Error, fmt::format("[Wayland] Failed to create pipe for data offer: {}", Utils::String::GetStrError()));
        return std::nullopt;
    }

    wl_data_offer_receive(&offer, mimeType.data(), std::get<1>(fds));
    FlushDisplay();
    close(std::get<1>(fds));

    std::string str;
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

        const ssize_t result = read(std::get<0>(fds), str.data() + length, readSize);
        if(result == 0)
            break;
        if(result == -1)
        {
            if(errno == EINTR)
                continue;

            InputError(Error::Platform_Error, fmt::format("[Wayland] Failed to read data offer pipe: {}", Utils::String::GetStrError()));
            close(std::get<0>(fds));
            return std::nullopt;
        }

        length += NumericCast<std::size_t>(result);
    }

    close(std::get<0>(fds));

    str.resize(length);

    return str;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::FlushDisplay()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    while(s_Data.Wayland.WaylandClient.DisplayFlush(s_Data.Wayland.DisplayWL) == -1)
    {
        if(errno != EAGAIN)
            return false;

        pollfd fd{s_Data.Wayland.WaylandClient.DisplayGetFD(s_Data.Wayland.DisplayWL), POLLOUT, 0};

        while(poll(&fd, 1, -1) == -1)
        {
            if(errno != EINTR && errno != EAGAIN)
                return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetCursorWayland(const InternalWindow& window, const std::string& name)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    wl_surface* const surface = s_Data.Wayland.CursorSurface;
    wl_cursor_theme* theme = s_Data.Wayland.CursorTheme;
    int32_t scale = 1;

    if(window.Wayland.ContentScale > 1 && (s_Data.Wayland.CursorThemeHiDPI != nullptr))
    {
        //We only support up to scale=2 for now, since libwayland-cursor
        //requires us to load a different theme for each size.
        scale = 2;
        theme = s_Data.Wayland.CursorThemeHiDPI;
    }

    const wl_cursor* const cursor = s_Data.Wayland.WaylandCursor.ThemeGetCursor(theme, name.c_str());
    if(cursor == nullptr)
        return;

    wl_cursor_image* const image = cursor->images[0];

    if(image == nullptr)
        return;

    wl_buffer* const buffer = s_Data.Wayland.WaylandCursor.ImageGetBuffer(image);
    if(buffer == nullptr)
        return;

    wl_pointer_set_cursor(s_Data.Wayland.Pointer, s_Data.Wayland.PointerEnterSerial,
                          surface, NumericCast<int32_t>(image->hotspot_x) / scale,
                          NumericCast<int32_t>(image->hotspot_y) / scale);
    wl_surface_set_buffer_scale(surface, scale);
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, NumericCast<int32_t>(image->width), NumericCast<int32_t>(image->height));
    wl_surface_commit(surface);

    s_Data.Wayland.CursorPreviousName = name;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputTextWayland(const InternalWindow& window, const uint32_t scanCode)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    const xkb_keysym_t* keysyms = nullptr;
    const xkb_keycode_t keycode = scanCode + 8;

    if(s_Data.Wayland.WaylandXKB.StateKeyGetSyms(s_Data.Wayland.WaylandXKB.State, keycode, &keysyms) == 1 && keysyms)
    {
        const xkb_keysym_t keysym = ComposeSymbol(keysyms[0]);
        const std::optional<uint32_t> codePoint = KeySymToUnicode(keysym);
        if(codePoint)
            InputChar(window, *codePoint);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

xkb_keysym_t TRAP::INTERNAL::WindowingAPI::ComposeSymbol(const xkb_keysym_t sym)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(sym == XKB_KEY_NoSymbol || (s_Data.Wayland.WaylandXKB.ComposeState == nullptr))
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

void TRAP::INTERNAL::WindowingAPI::UpdateContentScaleWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(wl_compositor_get_version(s_Data.Wayland.Compositor) < WL_SURFACE_SET_BUFFER_SCALE_SINCE_VERSION)
        return;

    //Get the scale factor from the highest scale monitor.
    int32_t maxScale = 1;
    for(const TRAPScaleWayland& scale : window.Wayland.Scales)
        maxScale = TRAP::Math::Max(scale.factor, maxScale);

    //Only change the framebuffer size if the scale changed.
    if(window.Wayland.ContentScale != maxScale)
    {
        window.Wayland.ContentScale = maxScale;
        wl_surface_set_buffer_scale(window.Wayland.Surface, maxScale);
        InputWindowContentScale(window, NumericCast<float>(maxScale), NumericCast<float>(maxScale));
        ResizeWindowWayland(window);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::ResizeWindowWayland(const InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    const int32_t scale = window.Wayland.ContentScale;
    const int32_t scaledWidth = window.Width * scale;
    const int32_t scaledHeight = window.Height * scale;

    if(!window.Transparent)
        SetContentAreaOpaqueWayland(window);
    InputFrameBufferSize(window, scaledWidth, scaledHeight);

    if(window.Wayland.Decorations.Top.surface == nullptr)
        return;

    wp_viewport_set_destination(window.Wayland.Decorations.Top.viewport, window.Width, TRAP_CAPTION_HEIGHT);
    wl_surface_commit(window.Wayland.Decorations.Top.surface);

    wp_viewport_set_destination(window.Wayland.Decorations.Left.viewport, TRAP_BORDER_SIZE, window.Height + TRAP_CAPTION_HEIGHT);
    wl_surface_commit(window.Wayland.Decorations.Left.surface);

    wl_subsurface_set_position(window.Wayland.Decorations.Right.subsurface, window.Width, -TRAP_CAPTION_HEIGHT);
    wp_viewport_set_destination(window.Wayland.Decorations.Right.viewport, TRAP_BORDER_SIZE, window.Height + TRAP_CAPTION_HEIGHT);
    wl_surface_commit(window.Wayland.Decorations.Right.surface);

    wl_subsurface_set_position(window.Wayland.Decorations.Bottom.subsurface, -TRAP_BORDER_SIZE, window.Height);
    wp_viewport_set_destination(window.Wayland.Decorations.Bottom.viewport, window.Width + TRAP_BORDER_SIZE * 2, TRAP_BORDER_SIZE);
    wl_surface_commit(window.Wayland.Decorations.Bottom.surface);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetContentAreaOpaqueWayland(const InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    wl_region* const region = wl_compositor_create_region(s_Data.Wayland.Compositor);
    if(region == nullptr)
        return;

    wl_region_add(region, 0, 0, window.Width, window.Height);
    wl_surface_set_opaque_region(window.Wayland.Surface, region);
    wl_region_destroy(region);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::ReleaseMonitorWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.LibDecor.Frame != nullptr)
        s_Data.Wayland.LibDecor.FrameUnsetFullscreen(window.Wayland.LibDecor.Frame);
    else if(window.Wayland.XDG.TopLevel != nullptr)
        xdg_toplevel_unset_fullscreen(window.Wayland.XDG.TopLevel);

    SetIdleInhibitorWayland(window, false);

    if((window.Wayland.LibDecor.Frame == nullptr) &&
       window.Wayland.XDG.DecorationMode != ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE)
    {
        if(window.Decorated)
            CreateFallbackDecorationsWayland(window);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetIdleInhibitorWayland(InternalWindow& window, const bool enable)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(enable && (window.Wayland.IdleInhibitor == nullptr) && (s_Data.Wayland.IdleInhibitManager != nullptr))
    {
        window.Wayland.IdleInhibitor = zwp_idle_inhibit_manager_v1_create_inhibitor(s_Data.Wayland.IdleInhibitManager, window.Wayland.Surface);
        if(window.Wayland.IdleInhibitor == nullptr)
            InputError(Error::Platform_Error, "[Wayland] Failed to create idle inhibitor");
    }
    else if(!enable && (window.Wayland.IdleInhibitor != nullptr))
    {
        zwp_idle_inhibitor_v1_destroy(window.Wayland.IdleInhibitor);
        window.Wayland.IdleInhibitor = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CreateFallbackDecorationsWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    const std::vector<uint8_t> data{224, 244, 244, 255};
    const Scope<Image> img = Image::LoadFromMemory(1, 1, Image::ColorFormat::RGBA, data);

    if((s_Data.Wayland.Viewporter == nullptr) || !img)
        return;

    if(window.Wayland.Decorations.Buffer == nullptr)
        window.Wayland.Decorations.Buffer = CreateShmBufferWayland(*img);
    if((window.Wayland.Decorations.Buffer == nullptr) || (window.Wayland.Surface == nullptr))
        return;

    CreateFallbackDecorationWayland(window, window.Wayland.Decorations.Top, *window.Wayland.Surface,
                                    *window.Wayland.Decorations.Buffer, 0, -TRAP_CAPTION_HEIGHT, window.Width,
                                    TRAP_CAPTION_HEIGHT);

    CreateFallbackDecorationWayland(window, window.Wayland.Decorations.Left, *window.Wayland.Surface,
                                    *window.Wayland.Decorations.Buffer, -TRAP_BORDER_SIZE, -TRAP_CAPTION_HEIGHT,
                                    TRAP_BORDER_SIZE, window.Height + TRAP_CAPTION_HEIGHT);

    CreateFallbackDecorationWayland(window, window.Wayland.Decorations.Right, *window.Wayland.Surface,
                                    *window.Wayland.Decorations.Buffer, window.Width, -TRAP_CAPTION_HEIGHT,
                                    TRAP_BORDER_SIZE, window.Height + TRAP_CAPTION_HEIGHT);

    CreateFallbackDecorationWayland(window, window.Wayland.Decorations.Bottom, *window.Wayland.Surface,
                                    *window.Wayland.Decorations.Buffer, -TRAP_BORDER_SIZE, window.Height,
                                    window.Width + TRAP_BORDER_SIZE * 2, TRAP_BORDER_SIZE);
}

//-------------------------------------------------------------------------------------------------------------------//

wl_buffer* TRAP::INTERNAL::WindowingAPI::CreateShmBufferWayland(const Image& image)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    const int32_t stride = NumericCast<int32_t>(image.GetWidth()) * 4;
    const int32_t length = NumericCast<int32_t>(image.GetWidth() * image.GetHeight() * 4u);

    const std::optional<int32_t> fd = CreateAnonymousFileWayland(length);
    if(!fd)
    {
        InputError(Error::Platform_Error, fmt::format("[Wayland] Failed to create buffer file of size {}: {}",
                                                      length, Utils::String::GetStrError()));
        return nullptr;
    }

    void* data = mmap(nullptr, NumericCast<std::size_t>(length), PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
    if(data == MAP_FAILED)
    {
        InputError(Error::Platform_Error, fmt::format("[Wayland] Failed to map file: {}", Utils::String::GetStrError()));
        close(*fd);
        return nullptr;
    }

    wl_shm_pool* const pool = wl_shm_create_pool(s_Data.Wayland.Shm, *fd, length);

    close(*fd);

    const uint8_t* source = static_cast<const uint8_t*>(image.GetPixelData());
    uint8_t* target = static_cast<uint8_t*>(data);
    for(uint32_t i = 0; i < image.GetWidth() * image.GetHeight(); ++i, source += 4)
    {
        uint32_t alpha = source[3];

        *target++ = NumericCast<uint8_t>((source[2] * alpha) / 255u);
        *target++ = NumericCast<uint8_t>((source[1] * alpha) / 255u);
        *target++ = NumericCast<uint8_t>((source[0] * alpha) / 255u);
        *target++ = NumericCast<uint8_t>(alpha);
    }

    wl_buffer* const buffer = wl_shm_pool_create_buffer(pool, 0, NumericCast<int32_t>(image.GetWidth()),
                                                        NumericCast<int32_t>(image.GetHeight()), stride,
                                                        WL_SHM_FORMAT_ARGB8888);
    munmap(data, NumericCast<std::size_t>(length));
    wl_shm_pool_destroy(pool);

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<int32_t> TRAP::INTERNAL::WindowingAPI::CreateAnonymousFileWayland(const off_t size)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    constinit static const char temp[] = "/trap-shared-XXXXXX";
    int32_t fd = 0;
    int32_t ret = 0;

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
        const char* const path = getenv("XDG_RUNTIME_DIR");
        if(path == nullptr)
        {
            errno = ENOENT;
            return std::nullopt;
        }

        std::string name = fmt::format("{}{}", path, temp);

        fd = CreateTmpFileCloexec(std::move(name)).value_or(-1);
        if(fd < 0)
            return std::nullopt;
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
        return std::nullopt;
    }

    return fd;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<int32_t> TRAP::INTERNAL::WindowingAPI::CreateTmpFileCloexec(std::string tmpName)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    const int32_t fd = mkostemp(tmpName.data(), O_CLOEXEC);
    if(fd >= 0)
    {
        unlink(tmpName.c_str());
        return fd;
    }

    return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CreateFallbackDecorationWayland(InternalWindow& window,
                                                                   TRAPDecorationWayland& decoration,
                                                                   wl_surface& parent,
                                                                   wl_buffer& buffer,
                                                                   const int32_t xPos, const int32_t yPos,
                                                                   const int32_t width, const int32_t height)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    decoration.surface = wl_compositor_create_surface(s_Data.Wayland.Compositor);
    wl_surface_set_user_data(decoration.surface, &window);
    s_Data.Wayland.WaylandClient.ProxySetTag(reinterpret_cast<wl_proxy*>(decoration.surface), &s_Data.Wayland.TagCStr);
    decoration.subsurface = wl_subcompositor_get_subsurface(s_Data.Wayland.SubCompositor, decoration.surface, &parent);
    wl_subsurface_set_position(decoration.subsurface, xPos, yPos);
    decoration.viewport = wp_viewporter_get_viewport(s_Data.Wayland.Viewporter, decoration.surface);
    wp_viewport_set_destination(decoration.viewport, width, height);
    wl_surface_attach(decoration.surface, &buffer, 0, 0);

    wl_region* const region = wl_compositor_create_region(s_Data.Wayland.Compositor);
    wl_region_add(region, 0, 0, width, height);
    wl_surface_set_opaque_region(decoration.surface, region);
    wl_surface_commit(decoration.surface);
    wl_region_destroy(region);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::AcquireMonitorWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.LibDecor.Frame != nullptr)
        s_Data.Wayland.LibDecor.FrameSetFullscreen(window.Wayland.LibDecor.Frame, window.Monitor->Wayland.Output);
    else if(window.Wayland.XDG.TopLevel != nullptr)
        xdg_toplevel_set_fullscreen(window.Wayland.XDG.TopLevel, window.Monitor->Wayland.Output);

    SetIdleInhibitorWayland(window, true);

    if(window.Wayland.Decorations.Top.surface != nullptr)
        DestroyFallbackDecorationsWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyFallbackDecorationsWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    DestroyFallbackDecorationWayland(window.Wayland.Decorations.Top);
    DestroyFallbackDecorationWayland(window.Wayland.Decorations.Left);
    DestroyFallbackDecorationWayland(window.Wayland.Decorations.Right);
    DestroyFallbackDecorationWayland(window.Wayland.Decorations.Bottom);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyFallbackDecorationWayland(TRAPDecorationWayland& decoration)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(decoration.subsurface != nullptr)
        wl_subsurface_destroy(decoration.subsurface);
    if(decoration.surface != nullptr)
        wl_surface_destroy(decoration.surface);
    if(decoration.viewport != nullptr)
        wp_viewport_destroy(decoration.viewport);

    decoration.surface = nullptr;
    decoration.subsurface = nullptr;
    decoration.viewport = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyShellObjectsWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    DestroyFallbackDecorationsWayland(window);

    if(window.Wayland.LibDecor.Frame != nullptr)
        s_Data.Wayland.LibDecor.FrameUnref(window.Wayland.LibDecor.Frame);

    if(window.Wayland.XDG.Decoration != nullptr)
        zxdg_toplevel_decoration_v1_destroy(window.Wayland.XDG.Decoration);

    if(window.Wayland.XDG.TopLevel != nullptr)
        xdg_toplevel_destroy(window.Wayland.XDG.TopLevel);

    if(window.Wayland.XDG.Surface != nullptr)
        xdg_surface_destroy(window.Wayland.XDG.Surface);

    window.Wayland.LibDecor.Frame = nullptr;
    window.Wayland.XDG.Decoration = nullptr;
    window.Wayland.XDG.DecorationMode = 0;
    window.Wayland.XDG.TopLevel = nullptr;
    window.Wayland.XDG.Surface = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::LibDecorFrameHandleConfigure(libdecor_frame* const frame,
                                                                libdecor_configuration* const config,
                                                                void* const userData)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::LibDecorFrameHandleConfigure(): User data pointer is nullptr!");
	TRAP_ASSERT(frame, "WindowingAPI::LibDecorFrameHandleConfigure(): libdecor_frame is nullptr!");
	TRAP_ASSERT(config, "WindowingAPI::LibDecorFrameHandleConfigure(): libdecor_configuration is nullptr!");

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    libdecor_window_state windowState{};
    bool fullscreen = false, activated = false, maximized = false;

    if(s_Data.Wayland.LibDecor.ConfigurationGetWindowState(config, &windowState))
    {
        fullscreen = (std::to_underlying(windowState) & std::to_underlying(libdecor_window_state::Fullscreen)) != 0u;
        activated = (std::to_underlying(windowState) & std::to_underlying(libdecor_window_state::Active)) != 0u;
        maximized = (std::to_underlying(windowState) & std::to_underlying(libdecor_window_state::Maximized)) != 0u;
    }
    else
    {
        fullscreen = window->Wayland.Fullscreen;
        activated = window->Wayland.Activated;
        maximized = window->Maximized;
    }

    int32_t width = 0, height = 0;
    if(!s_Data.Wayland.LibDecor.ConfigurationGetContentSize(config, frame, &width, &height))
    {
        width = window->Width;
        height = window->Height;
    }

    if(!maximized && !fullscreen)
    {
        if(window->Numerator != -1 && window->Denominator != -1)
        {
            const float aspectRatio = NumericCast<float>(width) / NumericCast<float>(height);
            const float targetRatio = NumericCast<float>(window->Numerator) / NumericCast<float>(window->Denominator);

            if(aspectRatio < targetRatio)
                height = NumericCast<int32_t>(NumericCast<float>(width) / targetRatio);
            else if(aspectRatio > targetRatio)
                width = NumericCast<int32_t>(NumericCast<float>(height) * targetRatio);
        }
    }

    libdecor_state* const frameState = s_Data.Wayland.LibDecor.StateNew(width, height);
    s_Data.Wayland.LibDecor.FrameCommit(frame, frameState, config);
    s_Data.Wayland.LibDecor.StateFree(frameState);

    if(window->Wayland.Activated != activated)
    {
        window->Wayland.Activated = activated;

        if(!window->Wayland.Activated)
        {
            if(window->Monitor != nullptr)
                s_Data.Wayland.LibDecor.FrameSetMinimized(window->Wayland.LibDecor.Frame);
        }
    }

    if(window->Maximized != maximized)
    {
        window->Maximized = maximized;
        InputWindowMaximize(*window, window->Maximized);
    }

    window->Wayland.Fullscreen = fullscreen;

    bool damaged = false;

    if(!window->Wayland.Visible)
    {
        window->Wayland.Visible = true;
        damaged = true;
    }

    if(width != window->Width || height != window->Height)
    {
        window->Width = width;
        window->Height = height;
        ResizeWindowWayland(*window);

        InputWindowSize(*window, width, height);
        damaged = true;
    }

    if(!damaged)
        wl_surface_commit(window->Wayland.Surface);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::LibDecorFrameHandleClose([[maybe_unused]] libdecor_frame* const frame,
                                                            void* const userData)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::LibDecorFrameHandleClose(): User data pointer is nullptr!");

    InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    InputWindowCloseRequest(*window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::LibDecorFrameHandleCommit([[maybe_unused]] libdecor_frame* const frame,
                                                             void* const userData)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(userData, "WindowingAPI::LibDecorFrameHandleCommit(): User data pointer is nullptr!");

    const InternalWindow* const window = static_cast<InternalWindow*>(userData);
    if(window == nullptr)
        return;

    wl_surface_commit(window->Wayland.Surface);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::CreateLibDecorFrame(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    window.Wayland.LibDecor.Frame = s_Data.Wayland.LibDecor.Decorate(s_Data.Wayland.LibDecor.Context,
                                                                     window.Wayland.Surface,
                                                                     &LibDecorFrameInterface, &window);

    if(window.Wayland.LibDecor.Frame == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create libdecor frame");
        return false;
    }

    if(!window.Wayland.AppID.empty())
        s_Data.Wayland.LibDecor.FrameSetAppID(window.Wayland.LibDecor.Frame, window.Wayland.AppID.c_str());

    if(!window.Wayland.Title.empty())
        s_Data.Wayland.LibDecor.FrameSetTitle(window.Wayland.LibDecor.Frame, window.Wayland.Title.c_str());

    if(window.MinWidth != -1 && window.MinHeight != -1)
        s_Data.Wayland.LibDecor.FrameSetMinContentSize(window.Wayland.LibDecor.Frame, window.MinWidth, window.MinHeight);

    if(window.MaxWidth != -1 && window.MaxHeight != -1)
        s_Data.Wayland.LibDecor.FrameSetMaxContentSize(window.Wayland.LibDecor.Frame, window.MaxWidth, window.MaxHeight);

    if(!window.Resizable)
        s_Data.Wayland.LibDecor.FrameUnsetCapabilities(window.Wayland.LibDecor.Frame, libdecor_capabilities::ActionResize);

    if(window.Monitor != nullptr)
    {
        //HACK: Allow libdecor to finish initialization of itself and its plugin so it will create the
        //      xdg_toplevel for the frame.
        //      This needs to exist when setting the frame to fullscreen.
        while(s_Data.Wayland.LibDecor.FrameGetXDGTopLevel(window.Wayland.LibDecor.Frame) == nullptr)
            PlatformWaitEventsWayland(0.0f);

        s_Data.Wayland.LibDecor.FrameSetFullscreen(window.Wayland.LibDecor.Frame, window.Monitor->Wayland.Output);
        SetIdleInhibitorWayland(window, true);
    }
    else
    {
        if(window.Maximized)
            s_Data.Wayland.LibDecor.FrameSetMaximized(window.Wayland.LibDecor.Frame);

        if(!window.Decorated)
            s_Data.Wayland.LibDecor.FrameSetVisibility(window.Wayland.LibDecor.Frame, false);

        SetIdleInhibitorWayland(window, false);
    }

    s_Data.Wayland.LibDecor.FrameMap(window.Wayland.LibDecor.Frame);
    s_Data.Wayland.WaylandClient.DisplayRoundtrip(s_Data.Wayland.DisplayWL);

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::CreateXDGShellObjectsWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    window.Wayland.XDG.Surface = xdg_wm_base_get_xdg_surface(s_Data.Wayland.WMBase, window.Wayland.Surface);
    if(window.Wayland.XDG.Surface == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create xdg-surface for window");
        return false;
    }

    xdg_surface_add_listener(window.Wayland.XDG.Surface, &XDGSurfaceListener, &window);

    window.Wayland.XDG.TopLevel = xdg_surface_get_toplevel(window.Wayland.XDG.Surface);
    if(window.Wayland.XDG.TopLevel == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create xdg-toplevel for window");
        return false;
    }

    xdg_toplevel_add_listener(window.Wayland.XDG.TopLevel, &XDGTopLevelListener, &window);

    if(!window.Wayland.AppID.empty())
        xdg_toplevel_set_app_id(window.Wayland.XDG.TopLevel, window.Wayland.AppID.c_str());

    if(!window.Wayland.Title.empty())
        xdg_toplevel_set_title(window.Wayland.XDG.TopLevel, window.Wayland.Title.c_str());

    if(window.Monitor != nullptr)
    {
        xdg_toplevel_set_fullscreen(window.Wayland.XDG.TopLevel, window.Monitor->Wayland.Output);
        SetIdleInhibitorWayland(window, true);
    }
    else
    {
        if(window.Maximized)
            xdg_toplevel_set_maximized(window.Wayland.XDG.TopLevel);

        SetIdleInhibitorWayland(window, false);
    }

    if(s_Data.Wayland.DecorationManager != nullptr)
    {
        window.Wayland.XDG.Decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(s_Data.Wayland.DecorationManager, window.Wayland.XDG.TopLevel);
        zxdg_toplevel_decoration_v1_add_listener(window.Wayland.XDG.Decoration, &XDGDecorationListener, &window);

        uint32_t mode = 0;

        if(window.Decorated)
            mode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE;
        else
            mode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;

        zxdg_toplevel_decoration_v1_set_mode(window.Wayland.XDG.Decoration, mode);
    }
    else
    {
        if(window.Decorated && (window.Monitor == nullptr))
            CreateFallbackDecorationsWayland(window);
    }

    if(window.MinWidth != -1 && window.MinHeight != -1)
    {
        int32_t minWidth = window.MinWidth;
        int32_t minHeight = window.MinHeight;

        if(window.Wayland.Decorations.Top.surface != nullptr)
        {
            minWidth += TRAP_BORDER_SIZE * 2;
            minHeight += TRAP_CAPTION_HEIGHT + TRAP_BORDER_SIZE;
        }

        xdg_toplevel_set_min_size(window.Wayland.XDG.TopLevel, minWidth, minHeight);
    }

    if(window.MaxWidth != -1 && window.MaxHeight != -1)
    {
        int32_t maxWidth = window.MaxWidth;
        int32_t maxHeight = window.MaxHeight;

        if(window.Wayland.Decorations.Top.surface != nullptr)
        {
            maxWidth += TRAP_BORDER_SIZE * 2;
            maxHeight += TRAP_CAPTION_HEIGHT + TRAP_BORDER_SIZE;
        }

        xdg_toplevel_set_max_size(window.Wayland.XDG.TopLevel, maxWidth, maxHeight);
    }

    wl_surface_commit(window.Wayland.Surface);
    s_Data.Wayland.WaylandClient.DisplayRoundtrip(s_Data.Wayland.DisplayWL);

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::CreateShellObjectsWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.LibDecor.Context != nullptr)
    {
        if(CreateLibDecorFrame(window))
            return true;
    }

    return CreateXDGShellObjectsWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::CreateNativeSurfaceWayland(InternalWindow& window, WindowConfig& WNDConfig)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    window.Wayland.Surface = wl_compositor_create_surface(s_Data.Wayland.Compositor);
    if(window.Wayland.Surface == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create window surface");
        return false;
    }

    s_Data.Wayland.WaylandClient.ProxySetTag(reinterpret_cast<wl_proxy*>(window.Wayland.Surface), &s_Data.Wayland.TagCStr);
    wl_surface_add_listener(window.Wayland.Surface, &SurfaceListener, &window);

    window.Width = NumericCast<int32_t>(WNDConfig.Width);
    window.Height = NumericCast<int32_t>(WNDConfig.Height);
    window.Wayland.ContentScale = 1;
    window.Wayland.Title = WNDConfig.Title;
    window.Wayland.AppID = WNDConfig.Wayland.AppID;

    window.Maximized = WNDConfig.Maximized;

    window.Transparent = false;
    if(!window.Transparent)
        SetContentAreaOpaqueWayland(window);

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::ConfinePointerWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    window.Wayland.ConfinedPointer = zwp_pointer_constraints_v1_confine_pointer(s_Data.Wayland.PointerConstraints,
                                                                                window.Wayland.Surface,
                                                                                s_Data.Wayland.Pointer,
                                                                                nullptr,
                                                                                ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT);

    zwp_confined_pointer_v1_add_listener(window.Wayland.ConfinedPointer, &ConfinedPointerListener, &window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::UnconfinePointerWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    zwp_confined_pointer_v1_destroy(window.Wayland.ConfinedPointer);
    window.Wayland.ConfinedPointer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::LockPointerWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.RelativePointerManager == nullptr)
    {
        InputError(Error::Feature_Unavailable, "[Wayland] The compositor does not support pointer locking");
        return;
    }

    window.Wayland.RelativePointer = zwp_relative_pointer_manager_v1_get_relative_pointer(s_Data.Wayland.RelativePointerManager,
                                                                                          s_Data.Wayland.Pointer);
    zwp_relative_pointer_v1_add_listener(window.Wayland.RelativePointer,
                                         &RelativePointerListener, &window);

    window.Wayland.LockedPointer = zwp_pointer_constraints_v1_lock_pointer(s_Data.Wayland.PointerConstraints,
                                                                           window.Wayland.Surface,
                                                                           s_Data.Wayland.Pointer,
                                                                           nullptr,
                                                                           ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT);
    zwp_locked_pointer_v1_add_listener(window.Wayland.LockedPointer, &LockedPointerListener, &window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::UnlockPointerWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    zwp_relative_pointer_v1_destroy(window.Wayland.RelativePointer);
    window.Wayland.RelativePointer = nullptr;

    zwp_locked_pointer_v1_destroy(window.Wayland.LockedPointer);
    window.Wayland.LockedPointer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetCursorImageWayland(const InternalWindow& window,
                                                         InternalCursor::wayland& cursorWayland)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    wl_cursor* wlCursor = cursorWayland.CursorWL;
    wl_buffer* buffer = nullptr;
    int32_t scale = 1;

    if(wlCursor == nullptr)
        buffer = cursorWayland.Buffer;
    else
    {
        if(window.Wayland.ContentScale > 1 && (cursorWayland.CursorHiDPI != nullptr))
        {
            wlCursor = cursorWayland.CursorHiDPI;
            scale = 2;
        }

        wl_cursor_image* const image = wlCursor->images[cursorWayland.CurrentImage];
        if(image == nullptr)
            return;
        buffer = s_Data.Wayland.WaylandCursor.ImageGetBuffer(image);
        if(buffer == nullptr)
            return;

        itimerspec timer{};
        timer.it_value.tv_sec = image->delay / 1000;
        timer.it_value.tv_nsec = (static_cast<long>(image->delay % 1000) * 1000000);
        timerfd_settime(s_Data.Wayland.CursorTimerFD, 0, &timer, nullptr);

        cursorWayland.Width = NumericCast<int32_t>(image->width);
        cursorWayland.Height = NumericCast<int32_t>(image->height);
        cursorWayland.XHotspot = NumericCast<int32_t>(image->hotspot_x);
        cursorWayland.YHotspot = NumericCast<int32_t>(image->hotspot_y);
    }

    wl_surface* const surface = s_Data.Wayland.CursorSurface;
    wl_pointer_set_cursor(s_Data.Wayland.Pointer, s_Data.Wayland.PointerEnterSerial,
                          surface, cursorWayland.XHotspot / scale,
                          cursorWayland.YHotspot / scale);
    wl_surface_set_buffer_scale(surface, scale);
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, cursorWayland.Width, cursorWayland.Height);
    wl_surface_commit(surface);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::HandleEventsWayland(double* const timeout)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    bool event = false;

    std::array<pollfd, 4> fds
    {
        {
            {s_Data.Wayland.WaylandClient.DisplayGetFD(s_Data.Wayland.DisplayWL), POLLIN, 0},
            {s_Data.Wayland.KeyRepeatTimerFD, POLLIN, 0},
            {s_Data.Wayland.CursorTimerFD, POLLIN, 0},
            {-1, POLLIN, 0}
        }
    };

    if(s_Data.Wayland.LibDecor.Context != nullptr)
        std::get<3>(fds).fd = s_Data.Wayland.LibDecor.GetFD(s_Data.Wayland.LibDecor.Context);

    while(!event)
    {
        while(s_Data.Wayland.WaylandClient.DisplayPrepareRead(s_Data.Wayland.DisplayWL) != 0)
            s_Data.Wayland.WaylandClient.DisplayDispatchPending(s_Data.Wayland.DisplayWL);

        //If an error other than EAGAIN happens, we have likely been disconnected
        //from the Wayland session; try to handle that the best we can.
        if(!FlushDisplay())
        {
            s_Data.Wayland.WaylandClient.DisplayCancelRead(s_Data.Wayland.DisplayWL);

            for(const TRAP::Scope<InternalWindow>& window : s_Data.WindowList)
            {
                if(window)
                    InputWindowCloseRequest(*window);
            }

            return;
        }

        if(!PollPOSIX(fds.data(), fds.size(), timeout))
        {
            s_Data.Wayland.WaylandClient.DisplayCancelRead(s_Data.Wayland.DisplayWL);
            return;
        }

        if((std::get<0>(fds).revents & POLLIN) != 0)
        {
            s_Data.Wayland.WaylandClient.DisplayReadEvents(s_Data.Wayland.DisplayWL);
            if(s_Data.Wayland.WaylandClient.DisplayDispatchPending(s_Data.Wayland.DisplayWL) > 0)
                event = true;
        }
        else
            s_Data.Wayland.WaylandClient.DisplayCancelRead(s_Data.Wayland.DisplayWL);

        if((std::get<1>(fds).revents & POLLIN) != 0)
        {
            uint64_t repeats = 0;

            if(read(s_Data.Wayland.KeyRepeatTimerFD, &repeats, sizeof(repeats)) == sizeof(repeats))
            {
                for(uint64_t i = 0; i < repeats; ++i)
                {
                    InputKey(*s_Data.Wayland.KeyboardFocus,
                             TranslateKey(s_Data.Wayland.KeyRepeatScancode),
                             s_Data.Wayland.KeyRepeatScancode, Input::KeyState::Pressed);

                    InputTextWayland(*s_Data.Wayland.KeyboardFocus,
                                     NumericCast<uint32_t>(s_Data.Wayland.KeyRepeatScancode));
                }

                event = true;
            }
        }

        if((std::get<2>(fds).revents & POLLIN) != 0)
        {
            uint64_t repeats = 0;

            if(read(s_Data.Wayland.CursorTimerFD, &repeats, sizeof(repeats)) == sizeof(repeats))
            {
                if(s_Data.Wayland.PointerFocus != nullptr)
                {
                    IncrementCursorImageWayland(*s_Data.Wayland.PointerFocus);
                    event = true;
                }
            }
        }

        if((std::get<3>(fds).revents & POLLIN) != 0)
            s_Data.Wayland.LibDecor.Dispatch(s_Data.Wayland.LibDecor.Context, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::IncrementCursorImageWayland(const InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.Decorations.Focus != TRAPDecorationSideWayland::MainWindow)
        return;

    InternalCursor* const cursor = window.Cursor;
    if((cursor != nullptr) && (cursor->Wayland.CursorWL != nullptr))
    {
        cursor->Wayland.CurrentImage++;
        cursor->Wayland.CurrentImage %= cursor->Wayland.CursorWL->image_count;
        SetCursorImageWayland(window, cursor->Wayland);
    }
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPosWayland([[maybe_unused]] const InternalWindow& window,
                                                               [[maybe_unused]] const int32_t xPos,
                                                               [[maybe_unused]] const int32_t yPos)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    //A Wayland client can't set its position, so just warn

    InputError(Error::Feature_Unavailable, "[Wayland] The platform doesn't support setting the window position");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorWayland(InternalWindow& window,
                                                                   InternalMonitor* const monitor,
														           [[maybe_unused]] const int32_t xPos,
                                                                   [[maybe_unused]] const int32_t yPos,
                                                                   const int32_t width, const int32_t height,
                                                                   [[maybe_unused]] const double refreshRate)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Monitor == monitor)
    {
        if(monitor == nullptr)
            PlatformSetWindowSizeWayland(window, width, height);

        return;
    }

    if(window.Monitor != nullptr)
        ReleaseMonitorWayland(window);

    window.Monitor = monitor;

    if(window.Monitor != nullptr)
        AcquireMonitorWayland(window);
    else
        PlatformSetWindowSizeWayland(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderlessWayland(InternalWindow& window,
                                                                             InternalMonitor& monitor)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    window.BorderlessFullscreen = true;

    if(window.Monitor != nullptr)
        ReleaseMonitorWayland(window);

    window.Monitor = &monitor;

    if(window.Monitor != nullptr)
        AcquireMonitorWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModesWayland(const InternalMonitor& monitor)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    return monitor.Modes;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformInitWayland()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    //These must be set before any failure checks
    s_Data.Wayland.KeyRepeatTimerFD = -1;
    s_Data.Wayland.CursorTimerFD = -1;

    s_Data.Wayland.Tag = fmt::format("{}.{}.{} Wayland",
                                     TRAP_VERSION_MAJOR(TRAP_VERSION),
                                     TRAP_VERSION_MINOR(TRAP_VERSION),
                                     TRAP_VERSION_PATCH(TRAP_VERSION));
    s_Data.Wayland.TagCStr = s_Data.Wayland.Tag.c_str();

    {
        s_Data.Wayland.WaylandClient.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libwayland-client.so.0");
        if(s_Data.Wayland.WaylandClient.Handle == nullptr)
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
        s_Data.Wayland.WaylandClient.ProxyGetTag = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_get_tag>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_get_tag");
        s_Data.Wayland.WaylandClient.ProxySetTag = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_set_tag>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_set_tag");
        s_Data.Wayland.WaylandClient.ProxyGetVersion = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_get_version>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_get_version");
        s_Data.Wayland.WaylandClient.ProxyMarshalFlags = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_wl_proxy_marshal_flags>(s_Data.Wayland.WaylandClient.Handle, "wl_proxy_marshal_flags");

        if((s_Data.Wayland.WaylandClient.DisplayFlush == nullptr) ||
           (s_Data.Wayland.WaylandClient.DisplayCancelRead == nullptr) ||
           (s_Data.Wayland.WaylandClient.DisplayDispatchPending == nullptr) ||
           (s_Data.Wayland.WaylandClient.DisplayReadEvents == nullptr) ||
           (s_Data.Wayland.WaylandClient.DisplayConnect == nullptr) ||
           (s_Data.Wayland.WaylandClient.DisplayDisconnect == nullptr) ||
           (s_Data.Wayland.WaylandClient.DisplayRoundtrip == nullptr) ||
           (s_Data.Wayland.WaylandClient.DisplayGetFD == nullptr) ||
           (s_Data.Wayland.WaylandClient.DisplayPrepareRead == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxyMarshal == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxyAddListener == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxyDestroy == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxyMarshalConstructor == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxyMarshalConstructorVersioned == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxyGetUserData == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxySetUserData == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxyGetTag == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxySetTag == nullptr) /*||
           (s_Data.Wayland.WaylandClient.ProxyGetVersion == nullptr) ||
           (s_Data.Wayland.WaylandClient.ProxyMarshalFlags == nullptr) */)
        {
            TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.Wayland.WaylandClient.Handle);
            InputError(Error::Platform_Error, "[Wayland] Failed to load libwayland-client entry point");
            return false;
        }

        s_Data.Wayland.DisplayWL = s_Data.Wayland.WaylandClient.DisplayConnect(nullptr);
        if(s_Data.Wayland.DisplayWL == nullptr)
        {
            TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.Wayland.WaylandClient.Handle);
            InputError(Error::Platform_Error, "[Wayland] Failed to connect to display");
            return false;
        }
    }

    {
        s_Data.Wayland.WaylandCursor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libwayland-cursor.so.0");
        if(s_Data.Wayland.WaylandCursor.Handle == nullptr)
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
        if(s_Data.Wayland.WaylandXKB.Handle == nullptr)
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
        s_Data.Wayland.WaylandXKB.KeyMapLayoutGetName = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_keymap_layout_get_name>(s_Data.Wayland.WaylandXKB.Handle, "xkb_keymap_layout_get_name");
        s_Data.Wayland.WaylandXKB.StateKeyGetLayout = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_state_key_get_layout>(s_Data.Wayland.WaylandXKB.Handle, "xkb_state_key_get_layout");
        s_Data.Wayland.WaylandXKB.StateModIndexIsActive = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_state_mod_index_is_active>(s_Data.Wayland.WaylandXKB.Handle, "xkb_state_mod_index_is_active");
        s_Data.Wayland.WaylandXKB.ComposeTableNewFromLocale = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_table_new_from_locale>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_table_new_from_locale");
        s_Data.Wayland.WaylandXKB.ComposeTableUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_table_unref>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_table_unref");
        s_Data.Wayland.WaylandXKB.ComposeStateNew = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_new>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_new");
        s_Data.Wayland.WaylandXKB.ComposeStateUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_unref>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_unref");
        s_Data.Wayland.WaylandXKB.ComposeStateFeed = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_feed>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_feed");
        s_Data.Wayland.WaylandXKB.ComposeStateGetStatus = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_get_status>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_get_status");
        s_Data.Wayland.WaylandXKB.ComposeStateGetOneSym = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_xkb_compose_state_get_one_sym>(s_Data.Wayland.WaylandXKB.Handle, "xkb_compose_state_get_one_sym");

        s_Data.Wayland.LibDecor.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libdecor-0.so.0");
        if(s_Data.Wayland.LibDecor.Handle != nullptr)
        {
            s_Data.Wayland.LibDecor.New = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_new>(s_Data.Wayland.LibDecor.Handle, "libdecor_new");
            s_Data.Wayland.LibDecor.Unref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_unref>(s_Data.Wayland.LibDecor.Handle, "libdecor_unref");
            s_Data.Wayland.LibDecor.GetFD = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_get_fd>(s_Data.Wayland.LibDecor.Handle, "libdecor_get_fd");
            s_Data.Wayland.LibDecor.Dispatch = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_dispatch>(s_Data.Wayland.LibDecor.Handle, "libdecor_dispatch");
            s_Data.Wayland.LibDecor.Decorate = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_decorate>(s_Data.Wayland.LibDecor.Handle, "libdecor_decorate");
            s_Data.Wayland.LibDecor.FrameUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_unref>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_unref");
            s_Data.Wayland.LibDecor.FrameSetAppID = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_app_id>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_app_id");
            s_Data.Wayland.LibDecor.FrameSetTitle = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_title>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_title");
            s_Data.Wayland.LibDecor.FrameSetMinimized = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_minimized>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_minimized");
            s_Data.Wayland.LibDecor.FrameSetFullscreen = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_fullscreen>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_fullscreen");
            s_Data.Wayland.LibDecor.FrameUnsetFullscreen = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_unset_fullscreen>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_unset_fullscreen");
            s_Data.Wayland.LibDecor.FrameMap = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_map>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_map");
            s_Data.Wayland.LibDecor.FrameCommit = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_commit>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_commit");
            s_Data.Wayland.LibDecor.FrameSetMinContentSize = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_min_content_size>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_min_content_size");
            s_Data.Wayland.LibDecor.FrameSetMaxContentSize = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_max_content_size>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_max_content_size");
            s_Data.Wayland.LibDecor.FrameSetMaximized = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_maximized>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_maximized");
            s_Data.Wayland.LibDecor.FrameUnsetMaximized = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_unset_maximized>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_unset_maximized");
            s_Data.Wayland.LibDecor.FrameSetCapabilities = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_capabilities>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_capabilities");
            s_Data.Wayland.LibDecor.FrameUnsetCapabilities = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_unset_capabilities>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_unset_capabilities");
            s_Data.Wayland.LibDecor.FrameSetVisibility = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_set_visibility>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_set_visibility");
            s_Data.Wayland.LibDecor.FrameGetXDGTopLevel = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_frame_get_xdg_toplevel>(s_Data.Wayland.LibDecor.Handle, "libdecor_frame_get_xdg_toplevel");
            s_Data.Wayland.LibDecor.ConfigurationGetContentSize = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_configuration_get_content_size>(s_Data.Wayland.LibDecor.Handle, "libdecor_configuration_get_content_size");
            s_Data.Wayland.LibDecor.ConfigurationGetWindowState = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_configuration_get_window_state>(s_Data.Wayland.LibDecor.Handle, "libdecor_configuration_get_window_state");
            s_Data.Wayland.LibDecor.StateNew = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_state_new>(s_Data.Wayland.LibDecor.Handle, "libdecor_state_new");
            s_Data.Wayland.LibDecor.StateFree = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_libdecor_state_free>(s_Data.Wayland.LibDecor.Handle, "libdecor_state_free");

            if((s_Data.Wayland.LibDecor.New == nullptr) ||
               (s_Data.Wayland.LibDecor.Unref == nullptr) ||
               (s_Data.Wayland.LibDecor.GetFD == nullptr) ||
               (s_Data.Wayland.LibDecor.Dispatch == nullptr) ||
               (s_Data.Wayland.LibDecor.Decorate == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameUnref == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetAppID == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetTitle == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetMinimized == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetFullscreen == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameUnsetFullscreen == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameMap == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameCommit == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetMinContentSize == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetMaxContentSize == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetMaximized == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameUnsetMaximized == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetCapabilities == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameUnsetCapabilities == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameSetVisibility == nullptr) ||
               (s_Data.Wayland.LibDecor.FrameGetXDGTopLevel == nullptr) ||
               (s_Data.Wayland.LibDecor.ConfigurationGetContentSize == nullptr) ||
               (s_Data.Wayland.LibDecor.ConfigurationGetWindowState == nullptr) ||
               (s_Data.Wayland.LibDecor.StateNew == nullptr) ||
               (s_Data.Wayland.LibDecor.StateFree == nullptr))
            {
                TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.Wayland.LibDecor.Handle);
                s_Data.Wayland.LibDecor.Handle = nullptr;
                s_Data.Wayland.LibDecor = {};
            }
        }

        s_Data.Wayland.Registry = wl_display_get_registry(s_Data.Wayland.DisplayWL);
        wl_registry_add_listener(s_Data.Wayland.Registry, &RegistryListener, nullptr);

        CreateKeyTablesWayland();

        s_Data.Wayland.WaylandXKB.Context = s_Data.Wayland.WaylandXKB.ContextNew(XKB_CONTEXT_NO_FLAGS);
        if(s_Data.Wayland.WaylandXKB.Context == nullptr)
        {
            InputError(Error::Platform_Error, "[Wayland] Failed to initialize xkb context");
            return false;
        }
    }

    //Sync so we got all registry objects
    s_Data.Wayland.WaylandClient.DisplayRoundtrip(s_Data.Wayland.DisplayWL);

    //Sync so we got all initial output events
    s_Data.Wayland.WaylandClient.DisplayRoundtrip(s_Data.Wayland.DisplayWL);

    if(s_Data.Wayland.LibDecor.Handle != nullptr)
    {
        s_Data.Wayland.LibDecor.Context = s_Data.Wayland.LibDecor.New(s_Data.Wayland.DisplayWL, &LibDecorInterface);

        //Allow libdecor to receive its globals before proceeding
        if(s_Data.Wayland.LibDecor.Context != nullptr)
            s_Data.Wayland.LibDecor.Dispatch(s_Data.Wayland.LibDecor.Context, 1);
    }

#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
    if(wl_seat_get_version(s_Data.Wayland.Seat) >= WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION)
    {
        s_Data.Wayland.KeyRepeatTimerFD = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    }
#endif /*WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION*/

    if(s_Data.Wayland.WMBase == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to find xdg-shell in your compositor");
        return false;
    }

    if(s_Data.Wayland.Shm == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to find wl_shm in your compositor");
        return false;
    }

    if(!LoadCursorThemeWayland())
        return false;

    if((s_Data.Wayland.Seat != nullptr) && (s_Data.Wayland.DataDeviceManager != nullptr))
    {
        s_Data.Wayland.DataDevice = wl_data_device_manager_get_data_device(s_Data.Wayland.DataDeviceManager, s_Data.Wayland.Seat);
        wl_data_device_add_listener(s_Data.Wayland.DataDevice, &DataDeviceListener, nullptr);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyWindowWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(&window == s_Data.Wayland.PointerFocus)
        s_Data.Wayland.PointerFocus = nullptr;

    if(&window == s_Data.Wayland.KeyboardFocus)
        s_Data.Wayland.KeyboardFocus = nullptr;

    if(window.Wayland.ContentType != nullptr)
        wp_content_type_v1_destroy(window.Wayland.ContentType);

    if(window.Wayland.ActivationToken != nullptr)
        xdg_activation_token_v1_destroy(window.Wayland.ActivationToken);

    if(window.Wayland.IdleInhibitor != nullptr)
        zwp_idle_inhibitor_v1_destroy(window.Wayland.IdleInhibitor);

    if(window.Wayland.RelativePointer != nullptr)
        zwp_relative_pointer_v1_destroy(window.Wayland.RelativePointer);

    if(window.Wayland.LockedPointer != nullptr)
        zwp_locked_pointer_v1_destroy(window.Wayland.LockedPointer);

    if(window.Wayland.ConfinedPointer != nullptr)
        zwp_confined_pointer_v1_destroy(window.Wayland.ConfinedPointer);

    DestroyShellObjectsWayland(window);

    if(window.Wayland.Decorations.Buffer != nullptr)
        wl_buffer_destroy(window.Wayland.Decorations.Buffer);

    if(window.Wayland.Surface != nullptr)
        wl_surface_destroy(window.Wayland.Surface);

    window.Wayland.Title = "";
    window.Wayland.AppID = "";
    window.Wayland.Scales.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShutdownWayland()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.LibDecor.Context != nullptr)
        s_Data.Wayland.LibDecor.Unref(s_Data.Wayland.LibDecor.Context);
    if(s_Data.Wayland.LibDecor.Handle != nullptr)
    {
        Utils::DynamicLoading::FreeLibrary(s_Data.Wayland.LibDecor.Handle);
        s_Data.Wayland.LibDecor.Handle = nullptr;
    }

    if(s_Data.Wayland.WaylandXKB.ComposeState != nullptr)
        s_Data.Wayland.WaylandXKB.ComposeStateUnref(s_Data.Wayland.WaylandXKB.ComposeState);
    if(s_Data.Wayland.WaylandXKB.KeyMap != nullptr)
        s_Data.Wayland.WaylandXKB.KeyMapUnref(s_Data.Wayland.WaylandXKB.KeyMap);
    if(s_Data.Wayland.WaylandXKB.State != nullptr)
        s_Data.Wayland.WaylandXKB.StateUnref(s_Data.Wayland.WaylandXKB.State);
    if(s_Data.Wayland.WaylandXKB.Context != nullptr)
        s_Data.Wayland.WaylandXKB.ContextUnref(s_Data.Wayland.WaylandXKB.Context);
    if(s_Data.Wayland.WaylandXKB.Handle != nullptr)
    {
        Utils::DynamicLoading::FreeLibrary(s_Data.Wayland.WaylandXKB.Handle);
        s_Data.Wayland.WaylandXKB.Handle = nullptr;
    }

    if(s_Data.Wayland.CursorTheme != nullptr)
        s_Data.Wayland.WaylandCursor.ThemeDestroy(s_Data.Wayland.CursorTheme);
    if(s_Data.Wayland.CursorThemeHiDPI != nullptr)
        s_Data.Wayland.WaylandCursor.ThemeDestroy(s_Data.Wayland.CursorThemeHiDPI);
    if(s_Data.Wayland.WaylandCursor.Handle != nullptr)
    {
        Utils::DynamicLoading::FreeLibrary(s_Data.Wayland.WaylandCursor.Handle);
        s_Data.Wayland.WaylandCursor.Handle = nullptr;
    }

    for(TRAPOfferWayland& offer : s_Data.Wayland.Offers)
        wl_data_offer_destroy(offer.offer);

    s_Data.Wayland.Offers.clear();

    if(s_Data.Wayland.CursorSurface != nullptr)
        wl_surface_destroy(s_Data.Wayland.CursorSurface);
    if(s_Data.Wayland.SubCompositor != nullptr)
        wl_subcompositor_destroy(s_Data.Wayland.SubCompositor);
    if(s_Data.Wayland.Compositor != nullptr)
        wl_compositor_destroy(s_Data.Wayland.Compositor);
    if(s_Data.Wayland.Shm != nullptr)
        wl_shm_destroy(s_Data.Wayland.Shm);
    if(s_Data.Wayland.Viewporter != nullptr)
        wp_viewporter_destroy(s_Data.Wayland.Viewporter);
    if(s_Data.Wayland.DecorationManager != nullptr)
        zxdg_decoration_manager_v1_destroy(s_Data.Wayland.DecorationManager);
    if(s_Data.Wayland.WMBase != nullptr)
        xdg_wm_base_destroy(s_Data.Wayland.WMBase);
    if(s_Data.Wayland.SelectionOffer != nullptr)
        wl_data_offer_destroy(s_Data.Wayland.SelectionOffer);
    if(s_Data.Wayland.DragOffer != nullptr)
        wl_data_offer_destroy(s_Data.Wayland.DragOffer);
    if(s_Data.Wayland.SelectionSource != nullptr)
        wl_data_source_destroy(s_Data.Wayland.SelectionSource);
    if(s_Data.Wayland.DataDevice != nullptr)
        wl_data_device_destroy(s_Data.Wayland.DataDevice);
    if(s_Data.Wayland.DataDeviceManager != nullptr)
        wl_data_device_manager_destroy(s_Data.Wayland.DataDeviceManager);
    if(s_Data.Wayland.Pointer != nullptr)
        wl_pointer_destroy(s_Data.Wayland.Pointer);
    if(s_Data.Wayland.Keyboard != nullptr)
        wl_keyboard_destroy(s_Data.Wayland.Keyboard);
    if(s_Data.Wayland.Seat != nullptr)
        wl_seat_destroy(s_Data.Wayland.Seat);
    if(s_Data.Wayland.RelativePointerManager != nullptr)
        zwp_relative_pointer_manager_v1_destroy(s_Data.Wayland.RelativePointerManager);
    if(s_Data.Wayland.PointerConstraints != nullptr)
        zwp_pointer_constraints_v1_destroy(s_Data.Wayland.PointerConstraints);
    if(s_Data.Wayland.IdleInhibitManager != nullptr)
        zwp_idle_inhibit_manager_v1_destroy(s_Data.Wayland.IdleInhibitManager);
    if(s_Data.Wayland.ActivationManager != nullptr)
        xdg_activation_v1_destroy(s_Data.Wayland.ActivationManager);
    if(s_Data.Wayland.ContentTypeManager != nullptr)
        wp_content_type_manager_v1_destroy(s_Data.Wayland.ContentTypeManager);
    if(s_Data.Wayland.Registry != nullptr)
        wl_registry_destroy(s_Data.Wayland.Registry);
    if(s_Data.Wayland.DisplayWL != nullptr)
    {
        s_Data.Wayland.WaylandClient.DisplayFlush(s_Data.Wayland.DisplayWL);
        s_Data.Wayland.WaylandClient.DisplayDisconnect(s_Data.Wayland.DisplayWL);
    }

    if(s_Data.Wayland.KeyRepeatTimerFD >= 0)
        close(s_Data.Wayland.KeyRepeatTimerFD);
    if(s_Data.Wayland.CursorTimerFD >= 0)
        close(s_Data.Wayland.CursorTimerFD);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindowWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if((window.Wayland.LibDecor.Frame == nullptr) && (window.Wayland.XDG.TopLevel == nullptr))
    {
        //Note: The XDG surface and role are created here so command-line applications
        //      with off-screen windows do not appear in for example the Unity dock
        CreateShellObjectsWayland(window);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindowWayland([[maybe_unused]] const InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InputError(Error::Feature_Unavailable, "[Wayland] The platform does not support setting the input focus");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowWayland(InternalWindow& window,
			                                                   WindowConfig& WNDConfig)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(!CreateNativeSurfaceWayland(window, WNDConfig))
        return false;

    if(WNDConfig.MousePassthrough)
        PlatformSetWindowMousePassthroughWayland(window, true);

    if((window.Monitor != nullptr) || WNDConfig.Visible)
    {
        if(!CreateShellObjectsWayland(window))
            return false;
    }

    if(s_Data.Wayland.ContentTypeManager != nullptr)
    {
        window.Wayland.ContentType = wp_content_type_manager_v1_get_surface_content_type(s_Data.Wayland.ContentTypeManager,
                                                                                         window.Wayland.Surface);
        if(window.Wayland.ContentType != nullptr)
            wp_content_type_v1_set_content_type(window.Wayland.ContentType, WP_CONTENT_TYPE_V1_TYPE_GAME);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowTitleWayland(InternalWindow& window, const std::string& title)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    window.Wayland.Title = title;

    if(window.Wayland.LibDecor.Frame != nullptr)
        s_Data.Wayland.LibDecor.FrameSetTitle(window.Wayland.LibDecor.Frame, title.c_str());
    else if(window.Wayland.XDG.TopLevel != nullptr)
        xdg_toplevel_set_title(window.Wayland.XDG.TopLevel, title.c_str());
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursorWayland(InternalCursor& cursor,
                                                               const Image& image, const uint32_t xHotspot,
                                                               const uint32_t yHotspot)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    cursor.Wayland.Buffer = CreateShmBufferWayland(image);
    if(cursor.Wayland.Buffer == nullptr)
        return false;

    cursor.Wayland.Width = NumericCast<int32_t>(image.GetWidth());
    cursor.Wayland.Height = NumericCast<int32_t>(image.GetHeight());
    cursor.Wayland.XHotspot = NumericCast<int32_t>(xHotspot);
    cursor.Wayland.YHotspot = NumericCast<int32_t>(yHotspot);

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursorWayland(InternalCursor& cursor,
                                                                       const CursorType& type)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    std::string name{};

    switch(type)
    {
    case CursorType::Arrow:
        name = "default";
        break;
    case CursorType::Input:
        name = "text";
        break;
    case CursorType::Crosshair:
        name = "crosshair";
        break;
    case CursorType::PointingHand:
        name = "pointer";
        break;
    case CursorType::ResizeHorizontal:
        name = "ew-resize";
        break;
    case CursorType::ResizeVertical:
        name = "ns-resize";
        break;
    case CursorType::ResizeDiagonalTopLeftBottomRight:
        name = "nwse-resize";
        break;
    case CursorType::ResizeDiagonalTopRightBottomLeft:
        name = "nesw-resize";
        break;
    case CursorType::ResizeAll:
        name = "all-scroll";
        break;
    case CursorType::NotAllowed:
        name = "not-allowed";
        break;
    }

    cursor.Wayland.CursorWL = s_Data.Wayland.WaylandCursor.ThemeGetCursor(s_Data.Wayland.CursorTheme, name.c_str());

    if(s_Data.Wayland.CursorThemeHiDPI != nullptr)
        cursor.Wayland.CursorHiDPI = s_Data.Wayland.WaylandCursor.ThemeGetCursor(s_Data.Wayland.CursorThemeHiDPI, name.c_str());

    if(cursor.Wayland.CursorWL == nullptr)
    {
        //Fall back to the core X11 names
        switch(type)
        {
        case CursorType::Arrow:
            name = "left-ptr";
            break;
        case CursorType::Input:
            name = "xterm";
            break;
        case CursorType::Crosshair:
            name = "crosshair";
            break;
        case CursorType::PointingHand:
            name = "hand2";
            break;
        case CursorType::ResizeHorizontal:
            name = "sb_h_double_arrow";
            break;
        case CursorType::ResizeVertical:
            name = "sb_v_double_arrow";
            break;
        case CursorType::ResizeAll:
            name = "fleur";
            break;

        default:
            InputError(Error::Cursor_Unavailable, "[Wayland] Standard cursor shape unavailable");
            return false;
        }

        cursor.Wayland.CursorWL = s_Data.Wayland.WaylandCursor.ThemeGetCursor(s_Data.Wayland.CursorTheme, name.c_str());
        if(cursor.Wayland.CursorWL == nullptr)
        {
            InputError(Error::Cursor_Unavailable, fmt::format("[Wayland] Failed to create standard cursor \"{}\"", name));
            return false;
        }

        if(s_Data.Wayland.CursorThemeHiDPI != nullptr)
        {
            if(cursor.Wayland.CursorHiDPI == nullptr)
            {
                cursor.Wayland.CursorHiDPI = s_Data.Wayland.WaylandCursor.ThemeGetCursor(s_Data.Wayland.CursorThemeHiDPI, name.c_str());
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursorWayland(InternalCursor& cursor)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    //If it's a standard cursor we don't need to do anything here
    if(cursor.Wayland.CursorWL != nullptr)
        return;

    if(cursor.Wayland.Buffer != nullptr)
        wl_buffer_destroy(cursor.Wayland.Buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorWayland(InternalWindow& window,
                                                            InternalCursor* const cursor)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.Pointer == nullptr)
        return;

    window.Cursor = cursor;

    //If we're not in the correct window just save the cursor
    //the next time the pointer enters the window the cursor will change
    if(&window != s_Data.Wayland.PointerFocus || window.Wayland.Decorations.Focus != TRAPDecorationSideWayland::MainWindow)
        return;

    //Update pointer lock to match cursor mode
    if(window.cursorMode == CursorMode::Disabled)
    {
        if(window.Wayland.ConfinedPointer != nullptr)
            UnconfinePointerWayland(window);
        if(window.Wayland.LockedPointer == nullptr)
            LockPointerWayland(window);
    }
    else if(window.cursorMode == CursorMode::Captured)
    {
        if(window.Wayland.LockedPointer != nullptr)
            UnlockPointerWayland(window);
        if(window.Wayland.ConfinedPointer == nullptr)
            ConfinePointerWayland(window);
    }
    else if(window.cursorMode == CursorMode::Normal ||
            window.cursorMode == CursorMode::Hidden)
    {
        if(window.Wayland.LockedPointer != nullptr)
            UnlockPointerWayland(window);
        else if(window.Wayland.ConfinedPointer != nullptr)
            UnconfinePointerWayland(window);
    }

    if(window.cursorMode == CursorMode::Normal ||
       window.cursorMode == CursorMode::Captured)
    {
        if(cursor != nullptr)
            SetCursorImageWayland(window, cursor->Wayland);
        else
        {
            wl_cursor* const defaultCursor = s_Data.Wayland.WaylandCursor.ThemeGetCursor(s_Data.Wayland.CursorTheme, "left_ptr");
            if(defaultCursor == nullptr)
            {
                InputError(Error::Platform_Error, "[Wayland] Standard cursor not found");
                return;
            }

            wl_cursor* defaultCursorHiDPI = nullptr;
            if(s_Data.Wayland.CursorThemeHiDPI != nullptr)
            {
                defaultCursorHiDPI = s_Data.Wayland.WaylandCursor.ThemeGetCursor(s_Data.Wayland.CursorThemeHiDPI, "left_ptr");
            }

            InternalCursor::wayland cursorWayland =
            {
                defaultCursor,
                defaultCursorHiDPI,
                nullptr,
                0, 0,
                0, 0,
                0
            };

            SetCursorImageWayland(window, cursorWayland);
        }
    }
    else if(window.cursorMode == CursorMode::Hidden ||
            window.cursorMode == CursorMode::Disabled)
    {
        wl_pointer_set_cursor(s_Data.Wayland.Pointer, s_Data.Wayland.PointerEnterSerial, nullptr, 0, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorModeWayland(InternalWindow& window,
                                                                [[maybe_unused]] const CursorMode mode)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(mode == CursorMode::Disabled || mode == CursorMode::Hidden)
        PlatformSetCursorWayland(window, nullptr);
    else
        PlatformSetCursorWayland(window, window.Cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPosWayland([[maybe_unused]] InternalWindow& window,
                                                               [[maybe_unused]] const double xPos,
                                                               [[maybe_unused]] const double yPos)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InputError(Error::Feature_Unavailable, "[Wayland] The platform does not support setting the cursor position");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIconWayland([[maybe_unused]] InternalWindow& window,
                                                                [[maybe_unused]] const Image* const image)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InputError(Error::Feature_Unavailable, "[Wayland] The platform does not support setting the window icon, use a .desktop file instead");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPosWayland([[maybe_unused]] const InternalWindow& window,
                                                               [[maybe_unused]] int32_t& xPos,
                                                               [[maybe_unused]] int32_t& yPos)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    //A Wayland client is not aware of its position, so just warn and leave it as (0, 0)

    InputError(Error::Feature_Unavailable, "[Wayland] The platform does not provide the window position");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeWayland(InternalWindow& window, const int32_t width,
                                                                const int32_t height)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Monitor != nullptr)
    {
        //Video mode settings is not available on Wayland
    }
    else
    {
        window.Width = width;
        window.Height = height;
        ResizeWindowWayland(window);

        if(window.Wayland.LibDecor.Frame != nullptr)
        {
            libdecor_state* const frameState = s_Data.Wayland.LibDecor.StateNew(width, height);
            s_Data.Wayland.LibDecor.FrameCommit(window.Wayland.LibDecor.Frame, frameState, nullptr);
            s_Data.Wayland.LibDecor.StateFree(frameState);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizableWayland(InternalWindow& window,
                                                                     const bool enabled)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.LibDecor.Frame != nullptr)
    {
        if(enabled)
            s_Data.Wayland.LibDecor.FrameSetCapabilities(window.Wayland.LibDecor.Frame, libdecor_capabilities::ActionResize);
        else
            s_Data.Wayland.LibDecor.FrameUnsetCapabilities(window.Wayland.LibDecor.Frame, libdecor_capabilities::ActionResize);
    }
    else
    {
        if(enabled)
            PlatformSetWindowSizeLimitsWayland(window, window.MinWidth, window.MinHeight, window.MaxWidth, window.MaxHeight);
        else
            PlatformSetWindowSizeLimitsWayland(window, window.Width, window.Height, window.Width, window.Height);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecoratedWayland(InternalWindow& window,
                                                                     const bool enabled)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.LibDecor.Frame != nullptr)
        s_Data.Wayland.LibDecor.FrameSetVisibility(window.Wayland.LibDecor.Frame, enabled);
    else if(window.Wayland.XDG.Decoration != nullptr)
    {
        uint32_t mode = 0;

        if(enabled)
            mode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE;
        else
            mode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;

        zxdg_toplevel_decoration_v1_set_mode(window.Wayland.XDG.Decoration, mode);
    }
    else if(window.Wayland.XDG.TopLevel != nullptr)
    {
        if(enabled)
            CreateFallbackDecorationsWayland(window);
        else
            DestroyFallbackDecorationsWayland(window);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloatingWayland([[maybe_unused]] const InternalWindow& window,
                                                                    [[maybe_unused]] const bool enabled)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InputError(Error::Feature_Unavailable, "[Wayland] Platform does not support making a window floating");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacityWayland([[maybe_unused]] const InternalWindow& window,
                                                                   [[maybe_unused]] const float opacity)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InputError(Error::Feature_Unavailable, "[Wayland] Platform does not support setting the window opacity");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthroughWayland(InternalWindow& window,
                                                                            const bool enabled)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(enabled)
    {
        wl_region* const region = wl_compositor_create_region(s_Data.Wayland.Compositor);
        wl_surface_set_input_region(window.Wayland.Surface, region);
        wl_region_destroy(region);
    }
    else
        wl_surface_set_input_region(window.Wayland.Surface, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbarWayland([[maybe_unused]] InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InputError(Error::Feature_Unavailable, "[Wayland] Platform does not support hiding windows from the taskbar");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPollEventsWayland()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    double timeout = 0.0;
    HandleEventsWayland(&timeout);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformWaitEventsWayland(double timeout)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(timeout == 0.0)
        HandleEventsWayland(nullptr);
    else
        HandleEventsWayland(&timeout);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPostEmptyEventWayland()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    wl_display_sync(s_Data.Wayland.DisplayWL);
    FlushDisplay();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocusedWayland(const InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    return s_Data.Wayland.KeyboardFocus == &window;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::INTERNAL::WindowingAPI::PlatformGetKeyScanCodeWayland(const Input::Key key)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    return s_Data.ScanCodes[NumericCast<std::size_t>(std::to_underlying(key))];
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::string> TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeNameWayland(const int32_t scanCode)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(scanCode < 0 || scanCode > 255 || s_Data.KeyCodes[NumericCast<uint32_t>(scanCode)] == Input::Key::Unknown)
    {
        InputError(Error::Invalid_Value, fmt::format("[Wayland] Invalid scancode {}", scanCode));
        return std::nullopt;
    }

    const Input::Key key = s_Data.KeyCodes[NumericCast<uint32_t>(scanCode)];
    const xkb_keycode_t keycode = NumericCast<uint32_t>(scanCode) + 8u;
    const xkb_layout_index_t layout = s_Data.Wayland.WaylandXKB.StateKeyGetLayout(s_Data.Wayland.WaylandXKB.State, keycode);
    if(layout == XKB_LAYOUT_INVALID)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to retrieve layout for the key name");
        return std::nullopt;
    }

    const xkb_keysym_t* keysyms = nullptr;
    s_Data.Wayland.WaylandXKB.KeyMapKeyGetSymsByLevel(s_Data.Wayland.WaylandXKB.KeyMap, keycode, layout, 0, &keysyms);
    if(keysyms == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to retrieve keysym for key name");
        return std::nullopt;
    }

    const std::optional<uint32_t> codePoint = KeySymToUnicode(keysyms[0]);
    if(!codePoint)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to retrieve codepoint for key name");
        return std::nullopt;
    }

    const std::string utf8Str = Utils::String::EncodeUTF8(*codePoint);
	if(utf8Str.empty())
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to encode codepoint for key name");
		return std::nullopt;
    }

    std::string& keyStr = s_Data.KeyNames[NumericCast<std::size_t>(std::to_underlying(key))];
    keyStr = utf8Str;

    return keyStr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetClipboardStringWayland(const std::string& string)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.SelectionSource != nullptr)
    {
        wl_data_source_destroy(s_Data.Wayland.SelectionSource);
        s_Data.Wayland.SelectionSource = nullptr;
    }

    s_Data.ClipboardString = string;

    s_Data.Wayland.SelectionSource = wl_data_device_manager_create_data_source(s_Data.Wayland.DataDeviceManager);
    if(s_Data.Wayland.SelectionSource == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to create clipboard data source");
        return;
    }
    wl_data_source_add_listener(s_Data.Wayland.SelectionSource, &DataSourceListener, nullptr);
    wl_data_source_offer(s_Data.Wayland.SelectionSource, "text/plain;charset=utf-8");
    wl_data_device_set_selection(s_Data.Wayland.DataDevice, s_Data.Wayland.SelectionSource, s_Data.Wayland.Serial);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::PlatformGetClipboardStringWayland()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.SelectionOffer == nullptr)
    {
        InputError(Error::Format_Unavailable, "[Wayland] No clipboard data available");
        return "";
    }

    if(s_Data.Wayland.SelectionSource != nullptr)
        return s_Data.ClipboardString;

    const auto clipboard = ReadDataOfferAsString(*s_Data.Wayland.SelectionOffer, "text/plain;charset=utf-8");
    s_Data.ClipboardString = clipboard.value_or("");

    return s_Data.ClipboardString;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetRequiredInstanceExtensionsWayland(std::array<std::string, 2>& extensions)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(!s_Data.VK.KHR_Surface || !s_Data.VK.KHR_Wayland_Surface)
        return;

    std::get<0>(extensions) = "VK_KHR_surface";
    std::get<1>(extensions) = "VK_KHR_wayland_surface";
}

//-------------------------------------------------------------------------------------------------------------------//

VkResult TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowSurfaceWayland(VkInstance instance,
                                                                          const InternalWindow& window,
																          const VkAllocationCallbacks* const allocator,
																          VkSurfaceKHR& surface)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(instance != VK_NULL_HANDLE, "WindowingAPI::PlatformCreateWindowSurfaceWayland(): VkInstance is nullptr!");

    const PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWaylandSurfaceKHR"));
    if(vkCreateWaylandSurfaceKHR == nullptr)
    {
        InputError(Error::API_Unavailable, "[Wayland] Vulkan instance missing VK_KHR_wayland_surface extension");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    VkWaylandSurfaceCreateInfoKHR sci{};
    sci.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    sci.display = s_Data.Wayland.DisplayWL;
    sci.surface = window.Wayland.Surface;

    VkResult err = vkCreateWaylandSurfaceKHR(instance, &sci, allocator, &surface);
    if(err != 0)
        InputError(Error::Platform_Error, fmt::format("[Wayland] Failed to create Vulkan surface: {}", GetVulkanResultString(err)));

    return err;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindowWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.LibDecor.Frame != nullptr)
        s_Data.Wayland.LibDecor.FrameSetMaximized(window.Wayland.LibDecor.Frame);
    else if(window.Wayland.XDG.TopLevel != nullptr)
        xdg_toplevel_set_maximized(window.Wayland.XDG.TopLevel);
    else
        window.Maximized = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindowWayland(const InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.LibDecor.Frame != nullptr)
        s_Data.Wayland.LibDecor.FrameSetMinimized(window.Wayland.LibDecor.Frame);
    else if(window.Wayland.XDG.TopLevel != nullptr)
        xdg_toplevel_set_minimized(window.Wayland.XDG.TopLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttentionWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.ActivationManager == nullptr)
        return;

    if(window.Wayland.ActivationToken != nullptr) //We're about to overwrite this with a new request
        xdg_activation_token_v1_destroy(window.Wayland.ActivationToken);

    window.Wayland.ActivationToken = xdg_activation_v1_get_activation_token(s_Data.Wayland.ActivationManager);
    xdg_activation_token_v1_add_listener(window.Wayland.ActivationToken, &XDGActivationListener, &window);

    xdg_activation_token_v1_commit(window.Wayland.ActivationToken);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.Visible)
    {
        window.Wayland.Visible = false;
        DestroyShellObjectsWayland(window);

        wl_surface_attach(window.Wayland.Surface, nullptr, 0, 0);
        wl_surface_commit(window.Wayland.Surface);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindowWayland(InternalWindow& window)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Monitor != nullptr)
    {
        //There is no way to unset minimized, or even to know if we are minimized,
        //so there is nothng to do in this case
    }
    else
    {
        //We assume we are not minimized and act only on maximization

        if(window.Maximized)
        {
            if(window.Wayland.LibDecor.Frame != nullptr)
                s_Data.Wayland.LibDecor.FrameUnsetMaximized(window.Wayland.LibDecor.Frame);
            else if(window.Wayland.XDG.TopLevel != nullptr)
                xdg_toplevel_unset_maximized(window.Wayland.XDG.TopLevel);
            else
                window.Maximized = false;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimitsWayland(InternalWindow& window,
                                                                      int32_t minWidth, int32_t minHeight,
                                                                      int32_t maxWidth, int32_t maxHeight)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(window.Wayland.LibDecor.Frame != nullptr)
    {
        if(minWidth == -1 || minHeight == -1)
            minWidth = minHeight = 0;

        if(maxWidth == -1 || maxHeight == -1)
            maxWidth = maxHeight = 0;

        s_Data.Wayland.LibDecor.FrameSetMinContentSize(window.Wayland.LibDecor.Frame, minWidth, minHeight);
        s_Data.Wayland.LibDecor.FrameSetMaxContentSize(window.Wayland.LibDecor.Frame, maxWidth, maxHeight);
    }
    else if(window.Wayland.XDG.TopLevel != nullptr)
    {
        if(minWidth == -1 || minHeight == -1)
            minWidth = minHeight = 0;
        else
        {
            if(window.Wayland.Decorations.Top.surface != nullptr)
            {
                minWidth += TRAP_BORDER_SIZE * 2;
                minHeight += TRAP_CAPTION_HEIGHT + TRAP_BORDER_SIZE;
            }
        }

        if(maxWidth == -1 || maxHeight == -1)
            maxWidth = maxHeight = 0;
        else
        {
            if(window.Wayland.Decorations.Top.surface != nullptr)
            {
                maxWidth += TRAP_BORDER_SIZE * 2;
                maxHeight += TRAP_CAPTION_HEIGHT + TRAP_BORDER_SIZE;
            }
        }

        xdg_toplevel_set_min_size(window.Wayland.XDG.TopLevel, minWidth, minHeight);
        xdg_toplevel_set_max_size(window.Wayland.XDG.TopLevel, maxWidth, maxHeight);
        wl_surface_commit(window.Wayland.Surface);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowAspectRatioWayland(InternalWindow& window,
                                                                       const int32_t numerator,
                                                                       const int32_t denominator)
{
    if(window.Maximized || window.Wayland.Fullscreen)
        return;

    int32_t width = window.Width, height = window.Height;

    if(numerator == -1 || denominator == -1)
    {
        const float aspectRatio = NumericCast<float>(width) / NumericCast<float>(height);
        const float targetRatio = NumericCast<float>(numerator) / NumericCast<float>(denominator);
        if(aspectRatio < targetRatio)
            height = NumericCast<int32_t>(NumericCast<float>(height) / targetRatio);
        else if(aspectRatio > targetRatio)
            width = NumericCast<int32_t>(NumericCast<float>(width) * targetRatio);
    }

    if(width != window.Width || height != window.Height)
    {
        window.Width = width;
        window.Height = height;
        ResizeWindowWayland(window);

        InputWindowSize(window, width, height);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::string> TRAP::INTERNAL::WindowingAPI::GetLinuxKeyboardLayoutNameWayland()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(s_Data.Wayland.WaylandXKB.KeyMap == nullptr)
    {
        InputError(Error::Platform_Error, "[Wayland] Key map missing");
        return std::nullopt;
    }

    const std::string name = s_Data.Wayland.WaylandXKB.KeyMapLayoutGetName(s_Data.Wayland.WaylandXKB.KeyMap,
                                                                           s_Data.Wayland.WaylandXKB.Group);

    if(name.empty())
    {
        InputError(Error::Platform_Error, "[Wayland] Failed to query keyboard layout name");
        return std::nullopt;
    }

    return s_Data.Wayland.WaylandXKB.KeyboardLayoutName;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDropWayland([[maybe_unused]] InternalWindow& window,
                                                                 [[maybe_unused]] const bool value)
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    InputError(Error::Feature_Unavailable, "[Wayland] Platform does not support toggling drag and drop. Drag and drop is enabled by default.");
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_PLATFORM_LINUX*/