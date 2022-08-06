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

TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::PlatformGetVideoMode(const InternalMonitor* monitor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformGetVideoModeX11(monitor);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformGetVideoModeWayland(monitor);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSize(const InternalWindow* window, int32_t& width,
                                                         int32_t& height)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetWindowSizeX11(window, width, height);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetWindowSizeWayland(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPos(const InternalWindow* window, const int32_t xPos, const int32_t yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowPosX11(window, xPos, yPos);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowPosWayland(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitor(InternalWindow* window, InternalMonitor* monitor,
														    const int32_t xPos, const int32_t yPos, const int32_t width,
															const int32_t height, const int32_t refreshRate)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowMonitorX11(window, monitor, xPos, yPos, width, height, refreshRate);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowMonitorWayland(window, monitor, xPos, yPos, width, height, refreshRate);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderless(InternalWindow* window,
                                                                      InternalMonitor* monitor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowMonitorBorderlessX11(window, monitor);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowMonitorBorderlessWayland(window, monitor);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModes(const InternalMonitor* monitor)
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

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformInitX11();
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformInitWayland();

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyWindow(InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformDestroyWindowX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformDestroyWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShutdown()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformShutdownX11();
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformShutdownWayland();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScale(const InternalMonitor* monitor, float& xScale,
                                                                  float& yScale)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetMonitorContentScaleX11(monitor, xScale, yScale);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetMonitorContentScaleWayland(monitor, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPos(const InternalMonitor* monitor, int32_t& xPos,
                                                         int32_t& yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetMonitorPosX11(monitor, xPos, yPos);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetMonitorPosWayland(monitor, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindow(InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformShowWindowX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformShowWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindow(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformFocusWindowX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformFocusWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateWindow(InternalWindow* window,
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowTitle(const InternalWindow* window, const std::string& title)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowTitleX11(window, title);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowTitleWayland(window, title);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursor(InternalCursor* cursor, const Image* const image,
                                                        const int32_t xHotspot, const int32_t yHotspot)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformCreateCursorX11(cursor, image, xHotspot, yHotspot);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformCreateCursorWayland(cursor, image, xHotspot, yHotspot);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursor(InternalCursor* cursor, const CursorType& type)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformCreateStandardCursorX11(cursor, type);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformCreateStandardCursorWayland(cursor, type);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursor(InternalCursor* cursor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformDestroyCursorX11(cursor);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformDestroyCursorWayland(cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursor(const InternalWindow* window, const InternalCursor* cursor)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetCursorX11(window, cursor);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetCursorWayland(window, cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorMode(InternalWindow* window, const CursorMode mode)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetCursorModeX11(window, mode);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetCursorModeWayland(window, mode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPos(InternalWindow* window, const double xPos, const double yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetCursorPosX11(window, xPos, yPos);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetCursorPosWayland(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPos(const InternalWindow* window, double& xPos, double& yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetCursorPosX11(window, xPos, yPos);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetCursorPosWayland(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIcon(InternalWindow* window, const Image* const image)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowIconX11(window, image);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowIconWayland(window, image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetWindowPosX11(window, xPos, yPos);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetWindowPosWayland(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSize(InternalWindow* window, const int32_t width, const int32_t height)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowSizeX11(window, width, height);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowSizeWayland(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizable(InternalWindow* window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowResizableX11(window, enabled);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowResizableWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecorated(const InternalWindow* window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowDecoratedX11(window, enabled);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowDecoratedWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloating(const InternalWindow* window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowFloatingX11(window, enabled);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowFloatingWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacity(const InternalWindow* window, const float opacity)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowOpacityX11(window, opacity);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowOpacityWayland(window, opacity);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthrough(InternalWindow* window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowMousePassthroughX11(window, enabled);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowMousePassthroughWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbar(InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformHideWindowFromTaskbarX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformHideWindowFromTaskbarWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacity(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformGetWindowOpacityX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformGetWindowOpacityWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSize(const InternalWindow* window, int32_t& width,
                                                              int32_t& height)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetFrameBufferSizeX11(window, width, height);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetFrameBufferSizeWayland(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScale(const InternalWindow* window, float& xScale,
                                                                 float& yScale)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetWindowContentScaleX11(window, xScale, yScale);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetWindowContentScaleWayland(window, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos,
                                                              int32_t& yPos, int32_t& width, int32_t& height)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformGetMonitorWorkAreaX11(monitor, xPos, yPos, width, height);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetMonitorWorkAreaWayland(monitor, xPos, yPos, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowVisible(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformWindowVisibleX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformWindowVisibleWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximized(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformWindowMaximizedX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformWindowMaximizedWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMinimized(const InternalWindow* window)
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
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformPollEventsWayland();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocused(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return PlatformWindowFocusedX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return PlatformWindowFocusedWayland(window);

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHovered(const InternalWindow* window)
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

void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotion(const InternalWindow* window, const bool enabled)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetRawMouseMotionX11(window, enabled);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetRawMouseMotionWayland(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetProgress(const InternalWindow* window, const ProgressState state,
													   const uint32_t completed)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetProgressX11(window, state, completed);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetProgressWayland(window, state, completed);
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

const char* TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeName(const int32_t scanCode)
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
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
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
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformGetRequiredInstanceExtensionsWayland(extensions);
}

//-------------------------------------------------------------------------------------------------------------------//

VkResult TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowSurface(VkInstance instance, const InternalWindow* window,
																   const VkAllocationCallbacks* allocator,
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

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindow(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformMaximizeWindowX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformMaximizeWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindow(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformMinimizeWindowX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformMinimizeWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttention(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformRequestWindowAttentionX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformRequestWindowAttentionWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindow(const InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformHideWindowX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformHideWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindow(InternalWindow* window)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformRestoreWindowX11(window);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformRestoreWindowWayland(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimits(InternalWindow* window, int32_t minWidth,
                                                               int32_t minHeight, int32_t maxWidth,
                                                               int32_t maxHeight)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetWindowSizeLimitsX11(window, minWidth, minHeight, maxWidth, maxHeight);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetWindowSizeLimitsWayland(window, minWidth, minHeight, maxWidth, maxHeight);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::GetLinuxKeyboardLayoutName()
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        return GetLinuxKeyboardLayoutNameX11();
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        return GetLinuxKeyboardLayoutNameWayland();

    return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDrop(InternalWindow* window, const bool value)
{
    TRAP_ASSERT(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Unknown, "Unsupported window manager");

    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
        PlatformSetDragAndDropX11(window, value);
    if(Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
        PlatformSetDragAndDropWayland(window, value);
}

#endif