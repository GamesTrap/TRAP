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
#include "Utils/Time/TimeStep.h"
#include "Utils/DynamicLoading/DynamicLoading.h"

std::optional<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoMode(const InternalMonitor& monitor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformGetVideoModeX11(monitor);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformGetVideoModeWayland(monitor);

    return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSize(const InternalWindow& window, int32_t& width,
                                                         int32_t& height)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetWindowSizeX11(window, width, height);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetWindowSizeWayland(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPos(const InternalWindow& window, const int32_t xPos,
                                                        const int32_t yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowPosX11(window, xPos, yPos);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowPosWayland(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitor(InternalWindow& window,
                                                            InternalMonitor* const monitor,
														    const int32_t xPos, const int32_t yPos,
                                                            const int32_t width, const int32_t height,
                                                            const double refreshRate)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowMonitorX11(window, monitor, xPos, yPos, width, height, refreshRate);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowMonitorWayland(window, monitor, xPos, yPos, width, height, refreshRate);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderless(InternalWindow& window,
                                                                      InternalMonitor& monitor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowMonitorBorderlessX11(window, monitor);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowMonitorBorderlessWayland(window, monitor);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModes(const InternalMonitor& monitor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformGetVideoModesX11(monitor);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformGetVideoModesWayland(monitor);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformInit()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    InitDBusPOSIX();

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformInitX11();
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformInitWayland();

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyWindow(InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformDestroyWindowX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformDestroyWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShutdown()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    TerminateDBusPOSIX();

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformShutdownX11();
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformShutdownWayland();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScale(const InternalMonitor& monitor,
                                                                  float& xScale, float& yScale)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetMonitorContentScaleX11(monitor, xScale, yScale);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetMonitorContentScaleWayland(monitor, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPos(const InternalMonitor& monitor, int32_t& xPos,
                                                         int32_t& yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetMonitorPosX11(monitor, xPos, yPos);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetMonitorPosWayland(monitor, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindow(InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformShowWindowX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformShowWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindow(const InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformFocusWindowX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformFocusWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateWindow(InternalWindow& window,
			                                            WindowConfig& WNDConfig)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformCreateWindowX11(window, WNDConfig);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformCreateWindowWayland(window, WNDConfig);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowTitle(InternalWindow& window, const std::string& title)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowTitleX11(window, title);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowTitleWayland(window, title);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursor(InternalCursor& cursor, const Image& image,
                                                        const uint32_t xHotspot, const uint32_t yHotspot)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformCreateCursorX11(cursor, image, xHotspot, yHotspot);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformCreateCursorWayland(cursor, image, xHotspot, yHotspot);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursor(InternalCursor& cursor,
                                                                const CursorType& type)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformCreateStandardCursorX11(cursor, type);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformCreateStandardCursorWayland(cursor, type);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursor(InternalCursor& cursor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformDestroyCursorX11(cursor);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformDestroyCursorWayland(cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursor(InternalWindow& window, InternalCursor* const cursor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetCursorX11(window, cursor);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetCursorWayland(window, cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorMode(InternalWindow& window, const CursorMode mode)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetCursorModeX11(window, mode);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetCursorModeWayland(window, mode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPos(InternalWindow& window,
                                                        const double xPos, const double yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetCursorPosX11(window, xPos, yPos);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetCursorPosWayland(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPos(const InternalWindow& window,
                                                        double& xPos, double& yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetCursorPosX11(window, xPos, yPos);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetCursorPosWayland(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIcon(InternalWindow& window, const Image* const image)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowIconX11(window, image);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowIconWayland(window, image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPos(const InternalWindow& window,
                                                        int32_t& xPos, int32_t& yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetWindowPosX11(window, xPos, yPos);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetWindowPosWayland(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSize(InternalWindow& window,
                                                         const int32_t width, const int32_t height)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowSizeX11(window, width, height);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowSizeWayland(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizable(InternalWindow& window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowResizableX11(window, enabled);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowResizableWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecorated(InternalWindow& window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowDecoratedX11(window, enabled);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowDecoratedWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloating(const InternalWindow& window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowFloatingX11(window, enabled);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowFloatingWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacity(const InternalWindow& window, const float opacity)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowOpacityX11(window, opacity);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowOpacityWayland(window, opacity);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthrough(InternalWindow& window,
                                                                     const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowMousePassthroughX11(window, enabled);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowMousePassthroughWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbar(InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformHideWindowFromTaskbarX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformHideWindowFromTaskbarWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<float> TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacity(const InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformGetWindowOpacityX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformGetWindowOpacityWayland(window);

    return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSize(const InternalWindow& window,
                                                              int32_t& width, int32_t& height)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetFrameBufferSizeX11(window, width, height);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetFrameBufferSizeWayland(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScale(const InternalWindow& window,
                                                                 float& xScale, float& yScale)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetWindowContentScaleX11(window, xScale, yScale);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetWindowContentScaleWayland(window, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkArea(const InternalMonitor& monitor, int32_t& xPos,
                                                              int32_t& yPos, int32_t& width, int32_t& height)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetMonitorWorkAreaX11(monitor, xPos, yPos, width, height);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetMonitorWorkAreaWayland(monitor, xPos, yPos, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowVisible(const InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformWindowVisibleX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformWindowVisibleWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximized(const InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformWindowMaximizedX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformWindowMaximizedWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMinimized(const InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformWindowMinimizedX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformWindowMinimizedWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPollEvents()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformPollEventsX11();
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformPollEventsWayland();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformWaitEvents(const double timeout)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformWaitEventsX11(timeout);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformWaitEventsWayland(timeout);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPostEmptyEvent()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformPostEmptyEventX11();
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformPostEmptyEventWayland();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocused(const InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformWindowFocusedX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformWindowFocusedWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHovered(const InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformWindowHoveredX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformWindowHoveredWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformRawMouseMotionSupported()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformRawMouseMotionSupportedX11();
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformRawMouseMotionSupportedWayland();

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotion(const InternalWindow& window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetRawMouseMotionX11(window, enabled);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetRawMouseMotionWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetProgressIndicator(const ProgressState state, const double progress)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if((s_Data.DBUS.Handle == nullptr) || (s_Data.DBUS.Connection == nullptr))
		return;

	//Setup parameters
	const dbus_bool_t progressVisible = NumericCast<dbus_bool_t>(state != ProgressState::Disabled);

	DBusMessageIter args{};

	DBusMessage* const msg = s_Data.DBUS.MessageNewSignal("/com/trap", "com.canonical.Unity.LauncherEntry", "Update");
	if(msg == nullptr)
	{
		InputError(Error::Platform_Error, "Failed to allocate new D-Bus message");
		return;
	}

	s_Data.DBUS.MessageIterInitAppend(msg, &args);

	//Set app_uri paramter
	const std::string desktopName = fmt::format("application://{}.desktop", TRAP::Application::GetGameName());
	const char* const desktopNameCStr = desktopName.c_str();
	s_Data.DBUS.MessageIterAppendBasic(&args, DBUS_TYPE_STRING, &desktopNameCStr); //Desktop file name

	//Set properties parameter
	DBusMessageIter sub1{}, sub2{}, sub3{};
	s_Data.DBUS.MessageIterOpenContainer(&args, DBUS_TYPE_ARRAY, "{sv}", &sub1);

	//Set progress visible
	s_Data.DBUS.MessageIterOpenContainer(&sub1, DBUS_TYPE_DICT_ENTRY, nullptr, &sub2);
	const std::string progressVisibleStr = "progress-visible";
	const char* const progressVisibleCStr = progressVisibleStr.c_str();
	s_Data.DBUS.MessageIterAppendBasic(&sub2, DBUS_TYPE_STRING, &progressVisibleCStr);
	s_Data.DBUS.MessageIterOpenContainer(&sub2, DBUS_TYPE_VARIANT, "b", &sub3);
	s_Data.DBUS.MessageIterAppendBasic(&sub3, DBUS_TYPE_BOOLEAN, &progressVisible);
	s_Data.DBUS.MessageIterCloseContainer(&sub2, &sub3);
	s_Data.DBUS.MessageIterCloseContainer(&sub1, &sub2);

	//Set progress value
	s_Data.DBUS.MessageIterOpenContainer(&sub1, DBUS_TYPE_DICT_ENTRY, nullptr, &sub2);
	const std::string progressValueStr = "progress";
	const char* const progressValueCStr = progressValueStr.c_str();
	s_Data.DBUS.MessageIterAppendBasic(&sub2, DBUS_TYPE_STRING, &progressValueCStr);
	s_Data.DBUS.MessageIterOpenContainer(&sub2, DBUS_TYPE_VARIANT, "d", &sub3);
	s_Data.DBUS.MessageIterAppendBasic(&sub3, DBUS_TYPE_DOUBLE, &progress);
	s_Data.DBUS.MessageIterCloseContainer(&sub2, &sub3);
	s_Data.DBUS.MessageIterCloseContainer(&sub1, &sub2);
	s_Data.DBUS.MessageIterCloseContainer(&args, &sub1);

	//Finally send the signal
	uint32_t serial = 0;
	if(s_Data.DBUS.ConnectionSend(s_Data.DBUS.Connection, msg, &serial) == 0u)
		InputError(Error::Platform_Error, "Failed to send D-Bus signal");
	else
		s_Data.DBUS.ConnectionFlush(s_Data.DBUS.Connection);

	s_Data.DBUS.MessageUnref(msg);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowProgressIndicator([[maybe_unused]] const InternalWindow& window,
                                                                      const ProgressState state, const double progress)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    SetProgressIndicator(state, progress);
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::INTERNAL::WindowingAPI::PlatformGetKeyScanCode(const Input::Key key)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformGetKeyScanCodeX11(key);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformGetKeyScanCodeWayland(key);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::string> TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeName(const int32_t scanCode)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformGetScanCodeNameX11(scanCode);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformGetScanCodeNameWayland(scanCode);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetClipboardString(const std::string& string)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetClipboardStringX11(string);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetClipboardStringWayland(string);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::PlatformGetClipboardString()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformGetClipboardStringX11();
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformGetClipboardStringWayland();

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetRequiredInstanceExtensions(std::array<std::string, 2>& extensions)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetRequiredInstanceExtensionsX11(extensions);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetRequiredInstanceExtensionsWayland(extensions);
}

//-------------------------------------------------------------------------------------------------------------------//

VkResult TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowSurface(VkInstance instance,
                                                                   const InternalWindow& window,
																   const VkAllocationCallbacks* const allocator,
																   VkSurfaceKHR& surface)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformCreateWindowSurfaceX11(instance, window, allocator, surface);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformCreateWindowSurfaceWayland(instance, window, allocator, surface);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindow(InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformMaximizeWindowX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformMaximizeWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindow(const InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformMinimizeWindowX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformMinimizeWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttention(InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformRequestWindowAttentionX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformRequestWindowAttentionWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindow(InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformHideWindowX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformHideWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindow(InternalWindow& window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformRestoreWindowX11(window);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformRestoreWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimits(InternalWindow& window,
                                                               int32_t minWidth, int32_t minHeight,
                                                               int32_t maxWidth, int32_t maxHeight)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowSizeLimitsX11(window, minWidth, minHeight, maxWidth, maxHeight);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowSizeLimitsWayland(window, minWidth, minHeight, maxWidth, maxHeight);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowAspectRatio(InternalWindow& window,
                                                                const int32_t numerator, const int32_t denominator)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowAspectRatioX11(window, numerator, denominator);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowAspectRatioWayland(window, numerator, denominator);
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::string> TRAP::INTERNAL::WindowingAPI::GetLinuxKeyboardLayoutName()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return GetLinuxKeyboardLayoutNameX11();
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return GetLinuxKeyboardLayoutNameWayland();

    return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDrop(InternalWindow& window, const bool value)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetDragAndDropX11(window, value);
    else if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetDragAndDropWayland(window, value);
}

//-------------------------------------------------------------------------------------------------------------------//
//Shared Code--------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Splits and translates a text/uri-list into separate file paths
[[nodiscard]] std::vector<std::string> TRAP::INTERNAL::WindowingAPI::ParseUriList(const std::string_view text)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

    if(text.empty())
        return {};

    static constexpr std::string_view prefix = "file://";
	std::vector<std::string> paths{};

    for(std::string_view line : TRAP::Utils::String::SplitStringView(text, "\r\n"))
    {
        if(line.empty() || line[0] == '#') //Ignore empty lines and comments
            continue;

        std::size_t prefixOffset = 0;
        if(line.starts_with(prefix))
            prefixOffset = line.find_first_of('/', prefix.size());

        std::string path;
        path.reserve(line.size() - prefixOffset);

        //Copy path without prefix and the "%<hex><hex>" stuff
        for(std::size_t i = prefixOffset; i < line.size(); ++i)
        {
            if(line[i] == '%' && Utils::String::IsHexDigit(line[i + 1]) && Utils::String::IsHexDigit(line[i + 2]))
            {
                path.push_back(NumericCast<char>(std::stoi(std::string{line[i + 1], line[i + 2]}, nullptr, 16)));
                i += 2;
            }
            else
                path.push_back(line[i]);
        }

        paths.push_back(path);
    }

	return paths;
}

//-------------------------------------------------------------------------------------------------------------------//

//Translates an X11 or Wayland key code to a TRAP key token
TRAP::Input::Key TRAP::INTERNAL::WindowingAPI::TranslateKey(const int32_t scanCode)
{
	//Use the pre-filled LUT (see CreateKeyTables())
	if(scanCode < 0 || scanCode >= NumericCast<int32_t>(s_Data.KeyCodes.size()))
		return Input::Key::Unknown;

	return s_Data.KeyCodes[NumericCast<uint32_t>(scanCode)];
}

//-------------------------------------------------------------------------------------------------------------------//

//Wait for data to arrive on any of the specified file descriptors
bool TRAP::INTERNAL::WindowingAPI::PollPOSIX(pollfd* const fds, const nfds_t count, double* const timeout)
{
	while(true)
	{
		if(timeout != nullptr)
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

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_PLATFORM_LINUX*/