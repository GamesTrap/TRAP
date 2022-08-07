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
#include <wayland-xdg-shell-client-protocol.h>

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
    return {};
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