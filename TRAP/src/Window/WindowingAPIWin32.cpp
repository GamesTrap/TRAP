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
#include "WindowingAPI.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Application.h"
#include "Utils/Time/TimeStep.h"

#ifdef TRAP_PLATFORM_WINDOWS

//Load necessary libraries (DLLs)
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::LoadLibraries()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
	                       reinterpret_cast<const WCHAR*>(&s_Data), reinterpret_cast<HMODULE*>(&s_Data.Instance)))
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to retrieve own module handle");
		return false;
	}

	s_Data.User32.Instance = static_cast<HINSTANCE>(TRAP::Utils::DynamicLoading::LoadLibrary("user32.dll"));
	if (!s_Data.User32.Instance)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to load user32.dll");
		return false;
	}

	s_Data.User32.SetProcessDPIAware = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_SetProcessDPIAware>(s_Data.User32.Instance,
																											 "SetProcessDPIAware");
	s_Data.User32.ChangeWindowMessageFilterEx = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_ChangeWindowMessageFilterEx>(s_Data.User32.Instance,
	 																														   "ChangeWindowMessageFilterEx");
	s_Data.User32.EnableNonClientDPIScaling = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_EnableNonClientDPIScaling>(s_Data.User32.Instance,
																														   "EnableNonClientDpiScaling");
	s_Data.User32.SetProcessDPIAwarenessContext = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_SetProcessDPIAwarenessContext>(s_Data.User32.Instance,
																																   "SetProcessDpiAwarenessContext");
	s_Data.User32.GetDPIForWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GetDPIForWindow>(s_Data.User32.Instance, "GetDpiForWindow");
	s_Data.User32.AdjustWindowRectExForDPI = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_AdjustWindowRectExForDPI>(s_Data.User32.Instance,
																														 "AdjustWindowRectExForDpi");
	s_Data.User32.GetSystemMetricsForDPI = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GetSystemMetricsForDPI>(s_Data.User32.Instance,
																													 "GetSystemMetricsForDpi");

	s_Data.DWMAPI_.Instance = static_cast<HINSTANCE>(TRAP::Utils::DynamicLoading::LoadLibrary("dwmapi.dll"));
	if (s_Data.DWMAPI_.Instance)
	{
		s_Data.DWMAPI_.IsCompositionEnabled = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DwmIsCompositionEnabled>(s_Data.DWMAPI_.Instance,
																														 "DwmIsCompositionEnabled");
		s_Data.DWMAPI_.Flush = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DwmFlush>(s_Data.DWMAPI_.Instance, "DwmFlush");
		s_Data.DWMAPI_.EnableBlurBehindWindow = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DwmEnableBlurBehindWindow>(s_Data.DWMAPI_.Instance,
																															 "DwmEnableBlurBehindWindow");
		s_Data.DWMAPI_.SetWindowAttribute = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DwmSetWindowAttribute>(s_Data.DWMAPI_.Instance,
																													 "DwmSetWindowAttribute");
	}

	s_Data.SHCore.Instance = static_cast<HINSTANCE>(TRAP::Utils::DynamicLoading::LoadLibrary("shcore.dll"));
	if (s_Data.SHCore.Instance)
	{
		s_Data.SHCore.SetProcessDPIAwareness = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_SetProcessDPIAwareness>(s_Data.SHCore.Instance,
																														 "SetProcessDpiAwareness");
		s_Data.SHCore.GetDPIForMonitor = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_GetDPIForMonitor>(s_Data.SHCore.Instance,
																											 "GetDpiForMonitor");
	}

	if (Utils::IsWindows7OrGreaterWin32())
		return true;

	Utils::Dialogs::ShowMsgBox("Unsupported Windows version", "Unsupported Windows version!\n"
	                           "TRAP™ needs Windows 7 or newer\nError code: 0x000A",
							   Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);
	TP_CRITICAL(Log::EngineWindowsPrefix, "Unsupported Windows version!");
	TRAP::Application::Shutdown();
	exit(-1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Unload used libraries (DLLs)
void TRAP::INTERNAL::WindowingAPI::FreeLibraries()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (s_Data.User32.Instance)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.User32.Instance);

	if (s_Data.DWMAPI_.Instance)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.DWMAPI_.Instance);

	if (s_Data.SHCore.Instance)
		TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.SHCore.Instance);
}

//-------------------------------------------------------------------------------------------------------------------//

//Reports the specified error, appending information about the last Win32 error
void TRAP::INTERNAL::WindowingAPI::InputErrorWin32(const Error error, const std::string& description)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::wstring buffer{};
	buffer.resize(1024);
	std::string message;
	message.resize(10249);

	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |	FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		           nullptr, GetLastError() & 0xFFFF, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	buffer.data(),
		           static_cast<DWORD>(buffer.size()), nullptr);
	WideCharToMultiByte(CP_UTF8, 0, buffer.data(), -1, message.data(), static_cast<int32_t>(message.size()),
	                    nullptr, nullptr);

	InputError(error, description + ": " + message);
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates key names according to the current keyboard layout
void TRAP::INTERNAL::WindowingAPI::UpdateKeyNamesWin32()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::array<BYTE, 256> state{};

	s_Data.KeyNames = {};

	for (uint32_t key = static_cast<uint32_t>(Input::Key::Space); key <= static_cast<uint32_t>(Input::Key::Menu);
	     key++)
	{
		uint32_t virtualKey;
		std::wstring chars{};
		chars.resize(16);

		const int32_t scanCode = s_Data.ScanCodes[key];
		if (scanCode == -1)
			continue;

		if (key >= static_cast<uint32_t>(Input::Key::KP_0) && key <= static_cast<uint32_t>(Input::Key::KP_Add))
		{
			static constexpr std::array<uint32_t, 15> virtualKeys =
			{
				VK_NUMPAD0,  VK_NUMPAD1,  VK_NUMPAD2, VK_NUMPAD3,
				VK_NUMPAD4,  VK_NUMPAD5,  VK_NUMPAD6, VK_NUMPAD7,
				VK_NUMPAD8,  VK_NUMPAD9,  VK_DECIMAL, VK_DIVIDE,
				VK_MULTIPLY, VK_SUBTRACT, VK_ADD
			};

			virtualKey = virtualKeys[key - static_cast<uint32_t>(Input::Key::KP_0)];
		}
		else
			virtualKey = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK);

		int32_t length = ToUnicode(virtualKey, scanCode, state.data(), chars.data(),
		                           static_cast<int32_t>(chars.size()), 0);

		if (length == -1)
		{
			//This is a dead key, so we need a second simulated key press to make it output its own
			//character (usually a diacritic)
			length = ToUnicode(virtualKey, scanCode, state.data(), chars.data(), static_cast<int32_t>(chars.size()),
				               0);
		}

		if (length < 1)
			continue;

		WideCharToMultiByte(CP_UTF8, 0, chars.data(), 1, s_Data.KeyNames[key].data(),
		                    static_cast<int32_t>(s_Data.KeyNames[key].size()), nullptr, nullptr);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window content scale has changed
//The scale is specified as the ratio between the current and default DPI
void TRAP::INTERNAL::WindowingAPI::InputWindowContentScale(const InternalWindow* window, const float xScale,
                                                           const float yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputWindowContentScale(): Window is nullptr!");
	TRAP_ASSERT(xScale > 0.0f, "WindowingAPI::InputWindowContentScale(): XScale is 0.0f!");
	TRAP_ASSERT(xScale < std::numeric_limits<float>::max(), "WindowingAPI::InputWindowContentScale(): XScale is too big!");
	TRAP_ASSERT(yScale > 0.0f, "WindowingAPI::InputWindowContentScale(): YScale is 0.0f!");
	TRAP_ASSERT(yScale < std::numeric_limits<float>::max(), "WindowingAPI::InputWindowContentScale(): YScale is too big!");

	if (window->Callbacks.Scale)
		window->Callbacks.Scale(window, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

//Window procedure for user-created windows
LRESULT CALLBACK TRAP::INTERNAL::WindowingAPI::WindowProc(HWND hWnd, const UINT uMsg, const WPARAM wParam,
                                                          const LPARAM lParam)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(NVSTATS_IS_PING_MSG_ID(uMsg))
		TRAP::Graphics::RendererAPI::GetRenderer()->ReflexMarker(TRAP::Application::GetGlobalCounter(), NVSTATS_PC_LATENCY_PING);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	InternalWindow* windowPtr = static_cast<InternalWindow*>(GetPropW(hWnd, L"TRAP"));

	if (!windowPtr)
	{
		if(uMsg == WM_NCCREATE)
		{
			if(Utils::IsWindows10Version1607OrGreaterWin32())
				s_Data.User32.EnableNonClientDPIScaling(hWnd);
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	switch (uMsg)
	{
	case WM_MOUSEACTIVATE:
		//HACK: Postpone cursor disabling when the window was activated by
		//      clicking a caption button
		if (HIWORD(lParam) == WM_LBUTTONDOWN && LOWORD(lParam) != HTCLIENT)
			windowPtr->FrameAction = true;
		break;

	case WM_CAPTURECHANGED:
	{
		//HACK: Disable the cursor once the caption button action has been
		//      completed or cancelled
		if (lParam == 0 && windowPtr->FrameAction)
		{
			if (windowPtr->cursorMode == CursorMode::Disabled)
				DisableCursor(windowPtr);
			else if (windowPtr->cursorMode == CursorMode::Captured)
				CaptureCursor(windowPtr);

			windowPtr->FrameAction = false;
		}
		break;
	}

	case WM_SETFOCUS:
	{
		InputWindowFocus(windowPtr, true);

		//HACK: Do not disable cursor while the user is interacting with
		//      a caption button
		if (windowPtr->FrameAction)
			break;

		if (windowPtr->cursorMode == CursorMode::Disabled)
			DisableCursor(windowPtr);
		else if (windowPtr->cursorMode == CursorMode::Captured)
			CaptureCursor(windowPtr);

		return 0;
	}

	case WM_KILLFOCUS:
	{
		if (windowPtr->cursorMode == CursorMode::Disabled)
			EnableCursor(windowPtr);
		else if (windowPtr->cursorMode == CursorMode::Captured)
			ReleaseCursor();

		if (windowPtr->Monitor && !windowPtr->BorderlessFullscreen)
			PlatformMinimizeWindow(windowPtr);

		InputWindowFocus(windowPtr, false);

		return 0;
	}

	case WM_SYSCOMMAND:
	{
		switch (wParam & 0xFFF0)
		{
		case SC_SCREENSAVE:
			[[fallthrough]];
		case SC_MONITORPOWER:
		{
			if (windowPtr->Monitor)
				//We are running in full screen mode, so disallow
				//screen saver and screen blanking
				return 0;

			break;
		}

		//User trying to access application menu using ALT?
		case SC_KEYMENU:
		{
			return 0;
		}

		default:
			break;
		}

		break;
	}

	case WM_CLOSE:
	{
		InputWindowCloseRequest(windowPtr);
		return 0;
	}

	case WM_INPUTLANGCHANGE:
	{
		UpdateKeyNamesWin32();
		InputKeyboardLayout();
		break;
	}

	case WM_CHAR:
		[[fallthrough]];
	case WM_SYSCHAR:
	{
		if (wParam >= 0xD800 && wParam <= 0xDBFF)
			windowPtr->HighSurrogate = static_cast<WCHAR>(wParam);
		else
		{
			uint32_t codePoint = 0;

			if (wParam >= 0xDC00 && wParam <= 0xDFFF)
			{
				if (windowPtr->HighSurrogate)
				{
					codePoint += (windowPtr->HighSurrogate - 0xD800) << 10;
					codePoint += static_cast<WCHAR>(wParam) - 0xDC00;
					codePoint += 0x10000;
				}
			}
			else
				codePoint = static_cast<WCHAR>(wParam);

			windowPtr->HighSurrogate = 0;
			InputChar(windowPtr, codePoint);
		}

		return 0;
	}

	case WM_UNICHAR:
	{
		if(wParam == UNICODE_NOCHAR)
		{
			//WM_UNICHAR is not sent by Windows, but is sent by some third-party input method engine
			//Returning TRUE(1) here announces support for this message
			return 1;
		}

		InputChar(windowPtr, static_cast<uint32_t>(wParam));
		return 0;
	}

	case WM_KEYDOWN:
		[[fallthrough]];
	case WM_SYSKEYDOWN:
		[[fallthrough]];
	case WM_KEYUP:
		[[fallthrough]];
	case WM_SYSKEYUP:
	{
		Input::Key key = Input::Key::Unknown;
		int32_t scanCode = 0;
		const Input::KeyState state = (HIWORD(lParam) & KF_UP) ? Input::KeyState::Released : Input::KeyState::Pressed;

		scanCode = (HIWORD(lParam) & (KF_EXTENDED | 0xFF));
		if(!scanCode)
		{
			//NOTE: Some synthetic key messages have a scanCode of 0
			//HACK: Map the virtual key back to a usable scanCode
			scanCode = MapVirtualKeyW(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC);
		}

		//HACK: ALT+PrtSc has a different scanCode than just PrtSc
		if(scanCode == 0x54)
			scanCode = 0x137;

		//HACK: CTRL+Pause has a different scanCode than just Pause
		if(scanCode == 0x16)
			scanCode = 0x45;

		//HACK: CJK IME sets the extended bit for right shift
		if(scanCode == 0x136)
			scanCode = 0x36;

		key = s_Data.KeyCodes[scanCode];

		//The CTRL keys require special handling
		if(wParam == VK_CONTROL)
		{
			if(HIWORD(lParam) & KF_EXTENDED)
			{
				//Right side keys have the extended key bit set
				key = Input::Key::Right_Control;
			}
			else
			{
				//NOTE: ALT GR sends Left CTRL followed by Right ALT
				//HACK: We only want one vent for ALT GR, so if we detect this sequence we discard this
				//      Left CTRL message now and later report Right ALT normally
				MSG next;
				const DWORD time = GetMessageTime();

				if(PeekMessageW(&next, nullptr, 0, 0, PM_NOREMOVE))
				{
					if(next.message == WM_KEYDOWN || next.message == WM_SYSKEYDOWN ||
						next.message == WM_KEYUP || next.message == WM_SYSKEYUP)
					{
						if(next.wParam == VK_MENU && (HIWORD(next.lParam) & KF_EXTENDED) && next.time == time)
						{
							//Next message is Right ALT down so discard this
							break;
						}
					}
				}

				//This is a regular Left CTRL message
				key = Input::Key::Left_Control;
			}
		}
		else if(wParam == VK_PROCESSKEY)
		{
			//IME notifies that keys have been filtered by setting the virtual key-code to VK_PROCESSKEY
			break;
		}

		if (state == Input::KeyState::Released && wParam == VK_SHIFT)
		{
			//HACK: Release both Shift keys on Shift up event, as when both
			//      are pressed the first release does not emit any event
			//NOTE: The other half of this is in PlatformPollEvents
			InputKey(windowPtr, Input::Key::Left_Shift, scanCode, state);
			InputKey(windowPtr, Input::Key::Right_Shift, scanCode, state);
		}
		else if (wParam == VK_SNAPSHOT)
		{
			//HACK: Key down is not reported for the Print Screen Key
			InputKey(windowPtr, key, scanCode, Input::KeyState::Pressed);
			InputKey(windowPtr, key, scanCode, Input::KeyState::Released);
		}
		else if(wParam != 0x07) //Fixes Xbox controller Xbox button presses (Windows 11)
			InputKey(windowPtr, key, scanCode, state);

		break;
	}

	case WM_LBUTTONDOWN:
		[[fallthrough]];
	case WM_RBUTTONDOWN:
		[[fallthrough]];
	case WM_MBUTTONDOWN:
		[[fallthrough]];
	case WM_XBUTTONDOWN:
		[[fallthrough]];
	case WM_LBUTTONUP:
		[[fallthrough]];
	case WM_RBUTTONUP:
		[[fallthrough]];
	case WM_MBUTTONUP:
		[[fallthrough]];
	case WM_XBUTTONUP:
	{
		uint32_t i;
		Input::MouseButton button;
		Input::KeyState state;

		if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
			button = Input::MouseButton::Left;
		else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
			button = Input::MouseButton::Right;
		else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
			button = Input::MouseButton::Middle;
		else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			button = Input::MouseButton::Four;
		else
			button = Input::MouseButton::Five;

		if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN ||
			uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN)
			state = Input::KeyState::Pressed;
		else
			state = Input::KeyState::Released;

		for (i = 0; i <= static_cast<uint32_t>(Input::MouseButton::Eight); i++)
			if (windowPtr->MouseButtons[i] == Input::KeyState::Pressed)
				break;

		if (i > static_cast<uint32_t>(Input::MouseButton::Eight))
			SetCapture(hWnd);

		InputMouseClick(windowPtr, button, state);

		for (i = 0; i <= static_cast<uint32_t>(Input::MouseButton::Eight); i++)
			if (windowPtr->MouseButtons[i] == Input::KeyState::Pressed)
				break;

		if (i > static_cast<uint32_t>(TRAP::Input::MouseButton::Eight))
			ReleaseCapture();

		if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
			return TRUE;

		return 0;
	}

	case WM_MOUSEMOVE:
	{
		const int32_t x = GET_X_LPARAM(lParam);
		const int32_t y = GET_Y_LPARAM(lParam);

		if (!windowPtr->CursorTracked)
		{
			TRACKMOUSEEVENT tme;
			ZeroMemory(&tme, sizeof(tme));
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = windowPtr->Handle;
			TrackMouseEvent(&tme);

			windowPtr->CursorTracked = true;
			InputCursorEnter(windowPtr, true);
		}

		if (windowPtr->cursorMode == CursorMode::Disabled)
		{
			const int32_t dx = x - windowPtr->LastCursorPosX;
			const int32_t dy = y - windowPtr->LastCursorPosY;

			if (s_Data.DisabledCursorWindow != windowPtr)
				break;
			if (windowPtr->RawMouseMotion)
				break;

			InputCursorPos(windowPtr, windowPtr->VirtualCursorPosX + dx, windowPtr->VirtualCursorPosY + dy);
		}
		else
			InputCursorPos(windowPtr, x, y);

		windowPtr->LastCursorPosX = x;
		windowPtr->LastCursorPosY = y;

		return 0;
	}

	case WM_INPUT:
	{
		UINT size = 0;
		const HRAWINPUT ri = reinterpret_cast<HRAWINPUT>(lParam);
		int32_t dx, dy;

		if (s_Data.DisabledCursorWindow != windowPtr)
			break;
		if (!windowPtr->RawMouseMotion)
			break;

		GetRawInputData(ri, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
		if (size > static_cast<UINT>(s_Data.RawInputSize))
		{
			s_Data.RawInput.clear();
			s_Data.RawInput.resize(size);
			s_Data.RawInputSize = size;
		}

		size = s_Data.RawInputSize;
		if (GetRawInputData(ri, RID_INPUT, s_Data.RawInput.data(), &size, sizeof(RAWINPUTHEADER)) ==
		    static_cast<UINT>(-1))
		{
			InputError(Error::Platform_Error, "[WinAPI] Failed to retrieve raw input data!");
			break;
		}

		const std::vector<RAWINPUT> data = s_Data.RawInput;
		if (data[0].data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
		{
			dx = data[0].data.mouse.lLastX - windowPtr->LastCursorPosX;
			dy = data[0].data.mouse.lLastY - windowPtr->LastCursorPosY;
		}
		else
		{
			dx = data[0].data.mouse.lLastX;
			dy = data[0].data.mouse.lLastY;
		}

		InputCursorPos(windowPtr, windowPtr->VirtualCursorPosX + dx, windowPtr->VirtualCursorPosY + dy);

		windowPtr->LastCursorPosX += dx;
		windowPtr->LastCursorPosY += dy;

		break;
	}

	case WM_MOUSELEAVE:
	{
		windowPtr->CursorTracked = false;
		InputCursorEnter(windowPtr, false);
		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		InputScroll(windowPtr, 0.0, static_cast<SHORT>(HIWORD(wParam)) / static_cast<double>(WHEEL_DELTA));
		return 0;
	}

	case WM_MOUSEHWHEEL:
	{
		//This message is only sent on Windows Vista and later
		//NOTE: The X-axis is inverted for consistency with X11
		InputScroll(windowPtr, -(static_cast<SHORT>(HIWORD(wParam)) / static_cast<double>(WHEEL_DELTA)), 0.0);
		return 0;
	}

	case WM_ENTERSIZEMOVE:
		[[fallthrough]];
	case WM_ENTERMENULOOP:
	{
		if (windowPtr->FrameAction)
			break;

		//HACK: Enable the cursor while the user is moving or
		//      resizing the window or using the window menu
		if (windowPtr->cursorMode == CursorMode::Disabled)
			EnableCursor(windowPtr);
		else if (windowPtr->cursorMode == CursorMode::Captured)
			ReleaseCursor();

		break;
	}

	case WM_EXITSIZEMOVE:
		[[fallthrough]];
	case WM_EXITMENULOOP:
	{
		if (windowPtr->FrameAction)
			break;

		//HACK: Disable the cursor once the user is done moving or
		//      resizing the window or using the menu
		if (windowPtr->cursorMode == CursorMode::Disabled)
			DisableCursor(windowPtr);
		else if (windowPtr->cursorMode == CursorMode::Captured)
			CaptureCursor(windowPtr);

		break;
	}

	case WM_SIZE:
	{
		const bool minimized = wParam == SIZE_MINIMIZED;
		const bool maximized = wParam == SIZE_MAXIMIZED || (windowPtr->Maximized && wParam != SIZE_RESTORED);
		const int32_t width = LOWORD(lParam);
		const int32_t height = HIWORD(lParam);

		if (s_Data.CapturedCursorWindow == windowPtr)
			CaptureCursor(windowPtr);

		if (windowPtr->Minimized != minimized)
			InputWindowMinimize(windowPtr, minimized);

		if (windowPtr->Maximized != maximized)
			InputWindowMaximize(windowPtr, maximized);

		if (windowPtr->Width != width || windowPtr->Height != height)
		{
			InputFrameBufferSize(windowPtr, width, height);
			InputWindowSize(windowPtr, width, height);
		}

		if (windowPtr->Monitor && windowPtr->Minimized != minimized)
			if (!minimized)
				FitToMonitor(windowPtr);

		windowPtr->Minimized = minimized;
		windowPtr->Maximized = maximized;
		windowPtr->Width = width;
		windowPtr->Height = height;

		return 0;
	}

	case WM_MOVE:
	{
		if (s_Data.CapturedCursorWindow == windowPtr)
			CaptureCursor(windowPtr);

		//NOTE: This cannot use LOWORD/HIWORD recommended by MSDN, as
		//      those macros do not handle negative window positions correctly
		InputWindowPos(windowPtr, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	case WM_SIZING:
	{
		if(windowPtr->Numerator == -1 || windowPtr->Denominator == -1)
			break;

		ApplyAspectRatio(windowPtr, static_cast<int32_t>(wParam), reinterpret_cast<RECT*>(lParam));
		return TRUE;
	}

	case WM_GETMINMAXINFO:
	{
		RECT frame{};
		MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
		const DWORD style = GetWindowStyle(windowPtr);
		const DWORD exStyle = GetWindowExStyle(windowPtr);


		if (windowPtr->Monitor)
			break;

		if (Utils::IsWindows10Version1607OrGreaterWin32())
			s_Data.User32.AdjustWindowRectExForDPI(&frame, style, FALSE, exStyle, s_Data.User32.GetDPIForWindow(windowPtr->Handle));
		else
			AdjustWindowRectEx(&frame, style, FALSE, exStyle);

		if(windowPtr->MinWidth != -1 && windowPtr->MinHeight != -1)
		{
			mmi->ptMinTrackSize.x = windowPtr->MinWidth + frame.right - frame.left;
			mmi->ptMinTrackSize.y = windowPtr->MinHeight + frame.bottom - frame.top;
		}

		if(windowPtr->MaxWidth != -1 && windowPtr->MaxHeight != -1)
		{
			mmi->ptMaxTrackSize.x = windowPtr->MaxWidth + frame.right - frame.left;
			mmi->ptMaxTrackSize.y = windowPtr->MaxHeight + frame.bottom - frame.top;
		}

		if(!windowPtr->Decorated)
		{
			MONITORINFO mi;
			const HMONITOR mh = MonitorFromWindow(windowPtr->Handle, MONITOR_DEFAULTTONEAREST);

			ZeroMemory(&mi, sizeof(mi));
			mi.cbSize = sizeof(mi);
			GetMonitorInfoW(mh, &mi);

			mmi->ptMaxPosition.x = mi.rcWork.left - mi.rcMonitor.left;
			mmi->ptMaxPosition.y = mi.rcWork.top - mi.rcMonitor.top;
			mmi->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
			mmi->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
		}

		return 0;
	}

	case WM_ERASEBKGND:
	{
		return TRUE;
	}

	case WM_NCACTIVATE:
		[[fallthrough]];
	case WM_NCPAINT:
	{
		//Prevent title bar from being drawn after restoring a minimized undecorated window
		if (!windowPtr->Decorated)
			return TRUE;

		break;
	}

	case WM_DWMCOMPOSITIONCHANGED:
	{
		return 0;
	}

	case WM_GETDPISCALEDSIZE:
	{
		//Adjust the window size to keep the content area size constant
		if (Utils::IsWindows10Version1703OrGreaterWin32())
		{
			RECT source = { 0 }, target = { 0 };
			SIZE* size = reinterpret_cast<SIZE*>(lParam);

			s_Data.User32.AdjustWindowRectExForDPI(&source, GetWindowStyle(windowPtr), FALSE,
			                                       GetWindowExStyle(windowPtr),
												   s_Data.User32.GetDPIForWindow(windowPtr->Handle));
			s_Data.User32.AdjustWindowRectExForDPI(&target, GetWindowStyle(windowPtr), FALSE,
			                                       GetWindowExStyle(windowPtr), LOWORD(wParam));

			size->cx += (target.right - target.left) - (source.right - source.left);
			size->cy += (target.bottom - target.top) - (source.bottom - source.top);

			return TRUE;
		}

		break;
	}

	case WM_DPICHANGED:
	{
		const float xScale = HIWORD(wParam) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
		const float yScale = LOWORD(wParam) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);

		//Resize windowed mode windows that either permit rescaling or that
		//need it to compensate for non-client area scaling
		if (!windowPtr->Monitor && Utils::IsWindows10Version1703OrGreaterWin32())
		{
			const RECT* suggested = reinterpret_cast<RECT*>(lParam);
			::SetWindowPos(windowPtr->Handle, HWND_TOP, suggested->left, suggested->top,
				           suggested->right - suggested->left, suggested->bottom - suggested->top,
					       SWP_NOACTIVATE | SWP_NOZORDER);
		}

		InputWindowContentScale(windowPtr, xScale, yScale);
		break;
	}

	case WM_SETCURSOR:
	{
		if (LOWORD(lParam) == HTCLIENT)
		{
			UpdateCursorImage(windowPtr);
			return TRUE;
		}

		break;
	}

	case WM_DROPFILES:
	{
		const HDROP drop = reinterpret_cast<HDROP>(wParam);
		POINT pt;

		const uint32_t count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
		std::vector<std::string> paths(count);

		//Move the mouse to the position of the drop
		DragQueryPoint(drop, &pt);
		InputCursorPos(windowPtr, pt.x, pt.y);

		for (uint32_t i = 0; i < paths.size(); i++)
		{
			const UINT length = DragQueryFileW(drop, i, nullptr, 0);
			std::wstring buffer{};
			buffer.resize(length + 1);

			DragQueryFileW(drop, i, buffer.data(), static_cast<UINT>(buffer.size()));
			paths[i] = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(buffer);
		}

		InputDrop(windowPtr, paths);

		DragFinish(drop);
		return 0;
	}

	default:
		break;
	}

	if(uMsg == windowPtr->TaskbarListMsgID)
	{
		TRAP::Utils::Windows::COMInitializer comInitializer{};
		if(comInitializer.IsInitialized())
		{
			const HRESULT res = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskbarList3,
								                 reinterpret_cast<LPVOID*>(&windowPtr->TaskbarList));
		}
		else
			InputError(Error::Platform_Error, "[Window] Failed to initialize COM library");
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------------------------//

//Callback for EnumDisplayMonitors in CreateMonitor
BOOL CALLBACK TRAP::INTERNAL::WindowingAPI::MonitorCallback(HMONITOR handle, HDC, RECT*, const LPARAM data)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	MONITORINFOEXW mi;
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);

	if (GetMonitorInfoW(handle, static_cast<MONITORINFO*>(&mi)))
	{
		InternalMonitor* monitor = reinterpret_cast<InternalMonitor*>(data);
		if (wcscmp(mi.szDevice, monitor->AdapterName.data()) == 0)
			monitor->Handle = handle;
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------//

//Create monitor from an adapter and (optionally) a display
[[nodiscard]] TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalMonitor> TRAP::INTERNAL::WindowingAPI::CreateMonitor(DISPLAY_DEVICEW* adapter,
                                                                                                                     DISPLAY_DEVICEW* display)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::string name;
	DEVMODEW dm{};
	RECT rect;

	if (display)
		name = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(display->DeviceString);
	else
		name = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(adapter->DeviceString);
	if (name.empty())
		return nullptr;

	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);
	EnumDisplaySettingsW(adapter->DeviceName, ENUM_CURRENT_SETTINGS, &dm);

	Scope<InternalMonitor> monitor = MakeScope<InternalMonitor>();
	monitor->Name = name;

	if (adapter->StateFlags & DISPLAY_DEVICE_MODESPRUNED)
		monitor->ModesPruned = true;

	monitor->AdapterName = adapter->DeviceName;
	monitor->PublicAdapterName = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(adapter->DeviceName);

	if (display)
	{
		monitor->DisplayName = display->DeviceName;
		monitor->PublicDisplayName = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(display->DeviceName);
	}

	rect.left = dm.dmPosition.x;
	rect.top = dm.dmPosition.y;
	rect.right = dm.dmPosition.x + dm.dmPelsWidth;
	rect.bottom = dm.dmPosition.y + dm.dmPelsHeight;

	EnumDisplayMonitors(nullptr, &rect, MonitorCallback, reinterpret_cast<LPARAM>(monitor.get()));
	return monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Poll for changes in the set of connected monitors
void TRAP::INTERNAL::WindowingAPI::PollMonitorsWin32()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	uint32_t i;
	std::vector<bool> disconnected(s_Data.Monitors.size(), true);
	DWORD displayIndex;
	DISPLAY_DEVICEW adapter, display;
	Scope<InternalMonitor> monitor;

	for (DWORD adapterIndex = 0; ; adapterIndex++)
	{
		uint32_t type = 1;

		ZeroMemory(&adapter, sizeof(adapter));
		adapter.cb = sizeof(adapter);

		if (!EnumDisplayDevicesW(nullptr, adapterIndex, &adapter, 0))
			break;

		if (!(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE))
			continue;

		if (adapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			type = 0;

		for (displayIndex = 0; ; displayIndex++)
		{
			ZeroMemory(&display, sizeof(display));
			display.cb = sizeof(display);

			if (!EnumDisplayDevicesW(adapter.DeviceName, displayIndex, &display, 0))
				break;

			if (!(display.StateFlags & DISPLAY_DEVICE_ACTIVE))
				continue;

			for (i = 0; i < disconnected.size(); i++)
			{
				if (s_Data.Monitors[i] && wcscmp(s_Data.Monitors[i]->DisplayName.data(), display.DeviceName) == 0)
				{
					disconnected[i] = false;
					//Handle may have changed, update
					EnumDisplayMonitors(nullptr, nullptr, MonitorCallback,
					                    reinterpret_cast<LPARAM>(s_Data.Monitors[i].get()));
					break;
				}
			}

			if (i < disconnected.size())
				continue;

			monitor = CreateMonitor(&adapter, &display);
			if (!monitor)
				return;

			InputMonitor(std::move(monitor), true, type);

			type = 1;
		}

		//HACK: If an active adapter does not have any display devices
		//      (as sometimes happens), add it directly as a monitor
		if (displayIndex == 0)
		{
			for (i = 0; i < disconnected.size(); i++)
			{
				if (s_Data.Monitors[i] && wcscmp(s_Data.Monitors[i]->AdapterName.data(), adapter.DeviceName) == 0)
				{
					disconnected[i] = false;
					break;
				}
			}

			if (i < disconnected.size())
				continue;

			monitor = CreateMonitor(&adapter, nullptr);
			if (!monitor)
				return;

			InputMonitor(std::move(monitor), true, type);
		}
	}

	for (i = 0; i < disconnected.size(); i++)
	{
		if (disconnected[i])
			InputMonitorDisconnect(i, 0);
	}

	//Update HMonitor Handles
	for (i = 0; i < s_Data.Monitors.size(); i++)
	{
		DEVMODEW dm{};
		RECT rect;

		ZeroMemory(&dm, sizeof(dm));
		dm.dmSize = sizeof(dm);
		EnumDisplaySettingsW(s_Data.Monitors[i]->AdapterName.data(), ENUM_CURRENT_SETTINGS, &dm);

		rect.left = dm.dmPosition.x;
		rect.top = dm.dmPosition.y;
		rect.right = dm.dmPosition.x + dm.dmPelsWidth;
		rect.bottom = dm.dmPosition.y + dm.dmPelsHeight;

		EnumDisplayMonitors(nullptr, &rect, MonitorCallback, reinterpret_cast<LPARAM>(s_Data.Monitors[i].get()));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Make the specified window and its video mode active on its monitor
void TRAP::INTERNAL::WindowingAPI::AcquireMonitor(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (!s_Data.AcquiredMonitorCount)
	{
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);

		//HACK: When mouse trails are enabled the cursor becomes invisible when
		//      the OpenGL ICD switches to page flipping
		SystemParametersInfoW(SPI_GETMOUSETRAILS, 0, &s_Data.MouseTrailSize, 0);
		SystemParametersInfoW(SPI_SETMOUSETRAILS, 0, nullptr, 0);
	}

	if (!window->Monitor->Window)
		s_Data.AcquiredMonitorCount++;

	SetVideoModeWin32(window->Monitor, window->videoMode);
	window->Monitor->Window = window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Make the specified window active on its monitor
void TRAP::INTERNAL::WindowingAPI::AcquireMonitorBorderless(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.AcquiredMonitorCount)
	{
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);

		//HACK: When mouse trails are enabled the cursor becomes invisible when
		//      the OpenGL ICD switches to page flipping
		SystemParametersInfoW(SPI_GETMOUSETRAILS, 0, &s_Data.MouseTrailSize, 0);
		SystemParametersInfoW(SPI_SETMOUSETRAILS, 0, nullptr, 0);
	}

	if (!window->Monitor->Window)
		s_Data.AcquiredMonitorCount++;

	window->Monitor->Window = window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Remove the window and restore the original video mode
void TRAP::INTERNAL::WindowingAPI::ReleaseMonitor(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (window->Monitor->Window != window)
		return;

	--s_Data.AcquiredMonitorCount;
	if (!s_Data.AcquiredMonitorCount)
	{
		SetThreadExecutionState(ES_CONTINUOUS);

		//HACK: Restore mouse trail length saved in acquireMonitor
		SystemParametersInfoW(SPI_SETMOUSETRAILS, s_Data.MouseTrailSize, nullptr, 0);
	}

	window->Monitor->Window = nullptr;

	if (window->Monitor->ModeChanged)
	{
		ChangeDisplaySettingsExW(window->Monitor->AdapterName.data(), nullptr, nullptr, CDS_FULLSCREEN, nullptr);
		window->Monitor->ModeChanged = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::FitToMonitor(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	MONITORINFO mi = {};
	mi.cbSize = sizeof(mi);
	GetMonitorInfoW(window->Monitor->Handle, &mi);
	::SetWindowPos(window->Handle, HWND_TOPMOST, mi.rcMonitor.left, mi.rcMonitor.top,
	               mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
		           SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
}

//-------------------------------------------------------------------------------------------------------------------//

//Change the current video mode
void TRAP::INTERNAL::WindowingAPI::SetVideoModeWin32(InternalMonitor* monitor, const InternalVideoMode& desired)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	DEVMODEW dm;

	const InternalVideoMode* best = ChooseVideoMode(monitor, desired);

	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
	dm.dmPelsWidth = best->Width;
	dm.dmPelsHeight = best->Height;
	dm.dmBitsPerPel = best->RedBits + best->GreenBits + best->BlueBits;
	dm.dmDisplayFrequency = static_cast<DWORD>(best->RefreshRate);

	if (dm.dmBitsPerPel < 15 || dm.dmBitsPerPel >= 24)
		dm.dmBitsPerPel = 32;

	const LONG result = ChangeDisplaySettingsExW(monitor->AdapterName.data(), &dm, nullptr, CDS_FULLSCREEN, nullptr);
	if (result == DISP_CHANGE_SUCCESSFUL)
		monitor->ModeChanged = true;
	else
	{
		std::string description = "Unknown error";

		if (result == DISP_CHANGE_BADDUALVIEW)
			description = "The system uses DualView";
		else if (result == DISP_CHANGE_BADFLAGS)
			description = "Invalid flags";
		else if (result == DISP_CHANGE_BADMODE)
			description = "Graphics mode not supported";
		else if (result == DISP_CHANGE_BADPARAM)
			description = "Invalid parameter";
		else if (result == DISP_CHANGE_FAILED)
			description = "Graphics mode failed";
		else if (result == DISP_CHANGE_NOTUPDATED)
			description = "Failed to write to registry";
		else if (result == DISP_CHANGE_RESTART)
			description = "Computer restart required";

		InputError(Error::Platform_Error, "[WinAPI] Failed to set video mode: " + description);
	}
}


//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::GetMonitorContentScaleWin32(HMONITOR handle, float& xScale, float& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	UINT xDPI = 0, yDPI = 0;

	if (Utils::IsWindows8Point1OrGreaterWin32())
	{
		if(s_Data.SHCore.GetDPIForMonitor(handle, Monitor_DPI_Type::MDT_Effective_DPI, &xDPI, &yDPI) != S_OK)
		{
			InputError(Error::Platform_Error, "[WinAPI] Failed to query monitor DPI");
			return;
		}
	}
	else
	{
		const HDC dc = GetDC(nullptr);
		xDPI = GetDeviceCaps(dc, LOGPIXELSX);
		yDPI = GetDeviceCaps(dc, LOGPIXELSY);
		ReleaseDC(nullptr, dc);
	}

	xScale = static_cast<float>(xDPI) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
	yScale = static_cast<float>(yDPI) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the window style for the specified window
[[nodiscard]] DWORD TRAP::INTERNAL::WindowingAPI::GetWindowStyle(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (window->Monitor)
		style |= WS_POPUP;
	else
	{
		style |= WS_SYSMENU | WS_MINIMIZEBOX;

		if (window->Decorated)
		{
			style |= WS_CAPTION;

			if (window->Resizable)
				style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
		}
		else
			style |= WS_POPUP;
	}

	return style;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the extended window style for the specified window
[[nodiscard]] DWORD TRAP::INTERNAL::WindowingAPI::GetWindowExStyle(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	DWORD style = WS_EX_APPWINDOW;

	if (window->Monitor || window->Floating)
		style |= WS_EX_TOPMOST;

	return style;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates the TRAP window
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::CreateNativeWindow(InternalWindow* window, const WindowConfig& WNDConfig)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t frameX, frameY, frameWidth, frameHeight;
	DWORD style = GetWindowStyle(window);
	const DWORD exStyle = GetWindowExStyle(window);

	if(!s_Data.MainWindowClass)
	{
		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = s_Data.Instance;
		wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
		wc.lpszClassName = L"TRAP";

		//Load user-provided icon if available
		wc.hIcon = static_cast<HICON>(::LoadImageW(::GetModuleHandleW(nullptr), L"TRAP_ICON", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
		if(!wc.hIcon)
		{
			//No user-provided icon found, load default icon
			wc.hIcon = static_cast<HICON>(::LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
		}

		s_Data.MainWindowClass = ::RegisterClassExW(&wc);
		if(!s_Data.MainWindowClass)
		{
			InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to register window class");
			return false;
		}
	}

	if (window->Monitor)
	{
		MONITORINFO mi{};
		mi.cbSize = sizeof(mi);
		GetMonitorInfoW(window->Monitor->Handle, &mi);
		//NOTE: This window placement is temporary and approximate, as the
		//      correct position and size cannot be known until the monitor
		//      video mode has been picked in SetVideoModeWin32
		frameX = mi.rcMonitor.left;
		frameY = mi.rcMonitor.top;
		frameWidth = mi.rcMonitor.right - mi.rcMonitor.left;
		frameHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}
	else
	{
		RECT rect = { 0, 0, WNDConfig.Width, WNDConfig.Height };

		window->Maximized = WNDConfig.Maximized;
		if (WNDConfig.Maximized)
			style |= WS_MAXIMIZE;

		AdjustWindowRectEx(&rect, style, FALSE, exStyle);

		frameX = CW_USEDEFAULT;
		frameY = CW_USEDEFAULT;

		frameWidth = rect.right - rect.left;
		frameHeight = rect.bottom - rect.top;
	}

	const std::wstring wideTitle = TRAP::Utils::String::CreateWideStringFromUTF8StringWin32(WNDConfig.Title);
	if (wideTitle.empty())
		return false;

	window->Handle = CreateWindowExW(exStyle, MAKEINTATOM(s_Data.MainWindowClass), wideTitle.data(), style, frameX, frameY, frameWidth, frameHeight,
		                             nullptr /*No parent window*/, nullptr /*No window menu*/,
									 s_Data.Instance, nullptr);

	if (!window->Handle)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to create window");
		return false;
	}

	SetPropW(window->Handle, L"TRAP", static_cast<void*>(window));

	if (Utils::IsWindows7OrGreaterWin32())
	{
		s_Data.User32.ChangeWindowMessageFilterEx(window->Handle,
			WM_DROPFILES, MSGFLT_ALLOW, nullptr);
		s_Data.User32.ChangeWindowMessageFilterEx(window->Handle,
			WM_COPYDATA, MSGFLT_ALLOW, nullptr);
		s_Data.User32.ChangeWindowMessageFilterEx(window->Handle,
			WM_COPYGLOBALDATA, MSGFLT_ALLOW, nullptr);

		//TaskbarList stuff
		window->TaskbarListMsgID = RegisterWindowMessageW(L"TaskbarButtonCreated");
		if(window->TaskbarListMsgID)
			s_Data.User32.ChangeWindowMessageFilterEx(window->Handle, window->TaskbarListMsgID, MSGFLT_ALLOW, nullptr);
	}

	if (Utils::IsWindows10BuildOrGreaterWin32(10240)) //First Windows 10 version
	{
		//Enable Immersive dark mode (using pre-official method)
		static constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_UNOFFICIAL = 19;
		static constexpr BOOL useDarkMode = true;
		s_Data.DWMAPI_.SetWindowAttribute(window->Handle, DWMWA_USE_IMMERSIVE_DARK_MODE_UNOFFICIAL, &useDarkMode, sizeof(useDarkMode));
	}
	else if (Utils::IsWindows11BuildOrGreaterWin32(22000)) //First Windows 11 version
	{
		//Enable Immersive dark mode (using official method)
		static constexpr BOOL useDarkMode = true;
		s_Data.DWMAPI_.SetWindowAttribute(window->Handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
	}

	if (!window->Monitor)
	{
		RECT rect = { 0, 0, static_cast<LONG>(WNDConfig.Width), static_cast<LONG>(WNDConfig.Height) };
		WINDOWPLACEMENT wp = { sizeof(wp) };
		const HMONITOR mh = MonitorFromWindow(window->Handle, MONITOR_DEFAULTTONEAREST);

		//Adjust window rect to account for DPI scaling of the window frame and
		//(if enabled) DPI scaling of the content area.
		//This cannot be done until we know what monitor the window was placed on.
		//Only update the restored window rect as the window may be maximized

		if (Utils::IsWindows10Version1607OrGreaterWin32())
		{
			s_Data.User32.AdjustWindowRectExForDPI(&rect, style, FALSE, exStyle,
				s_Data.User32.GetDPIForWindow(window->Handle));
		}
		else
			AdjustWindowRectEx(&rect, style, FALSE, exStyle);

		GetWindowPlacement(window->Handle, &wp);
		OffsetRect(&rect,
				   wp.rcNormalPosition.left - rect.left,
				   wp.rcNormalPosition.top - rect.top);
		wp.rcNormalPosition = rect;
		wp.showCmd = SW_HIDE;
		SetWindowPlacement(window->Handle, &wp);

		//Adjust rect of maximized undecorated window, beacuse by default
		//Windows will make such window cover the whole monitor instead of its workarea
		if(WNDConfig.Maximized && !WNDConfig.Decorated)
		{
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfoW(mh, &mi);

			::SetWindowPos(window->Handle, HWND_TOP,
						   mi.rcWork.left, mi.rcWork.top,
						   mi.rcWork.right - mi.rcWork.left,
						   mi.rcWork.bottom - mi.rcWork.top,
						   SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}

	PlatformGetWindowSize(window, window->Width, window->Height);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::MaximizeWindowManually(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	RECT rect;
	DWORD style;
	MONITORINFO mi = {sizeof(mi)};

	GetMonitorInfoW(MonitorFromWindow(window->Handle, MONITOR_DEFAULTTONEAREST), &mi);

	rect = mi.rcWork;

	if(window->MaxWidth != -1 && window->MaxHeight != -1)
	{
		if(rect.right - rect.left > window->MaxWidth)
			rect.right = rect.left + window->MaxWidth;
		if(rect.bottom - rect.top > window->MaxHeight)
			rect.bottom = rect.top + window->MaxHeight;
	}

	style = GetWindowLongW(window->Handle, GWL_STYLE);
	style |= WS_MAXIMIZE;
	SetWindowLongW(window->Handle, GWL_STYLE, style);

	if(window->Decorated)
	{
		const DWORD exStyle = GetWindowLongW(window->Handle, GWL_EXSTYLE);

		if(Utils::IsWindows10Version1607OrGreaterWin32())
		{
			const UINT dpi = s_Data.User32.GetDPIForWindow(window->Handle);
			s_Data.User32.AdjustWindowRectExForDPI(&rect, style, FALSE, exStyle, dpi);
			OffsetRect(&rect, 0, s_Data.User32.GetSystemMetricsForDPI(SM_CYCAPTION, dpi));
		}
		else
		{
			AdjustWindowRectEx(&rect, style, FALSE, exStyle);
			OffsetRect(&rect, 0, GetSystemMetrics(SM_CYCAPTION));
		}

		if(rect.bottom > mi.rcWork.bottom)
			rect.bottom = mi.rcWork.bottom;
	}

	::SetWindowPos(window->Handle, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
				   SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] HINSTANCE TRAP::INTERNAL::WindowingAPI::GetWin32HInstance()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Data.Instance;
}

//-------------------------------------------------------------------------------------------------------------------//

LRESULT CALLBACK TRAP::INTERNAL::WindowingAPI::HelperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	switch(uMsg)
	{
	case WM_DISPLAYCHANGE:
		PollMonitorsWin32();
		break;

	case WM_DEVICECHANGE:
	{
		if (wParam == DBT_DEVICEARRIVAL)
		{
			const DEV_BROADCAST_HDR* dbh = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);
			if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
				Input::DetectControllerConnectionWin32();
		}
		else if (wParam == DBT_DEVICEREMOVECOMPLETE)
		{
			const DEV_BROADCAST_HDR* dbh = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);
			if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
				Input::DetectControllerDisconnectionWin32();
		}
		break;
	}

	default:
		break;
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::CreateHelperWindow()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	MSG msg;
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = static_cast<WNDPROC>(HelperWindowProc);
	wc.hInstance = s_Data.Instance;
	wc.lpszClassName = L"TRAP Helper";

	s_Data.HelperWindowClass = RegisterClassExW(&wc);
	if(!s_Data.HelperWindowClass)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to register helper window class");
		return false;
	}

	s_Data.HelperWindowHandle = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, MAKEINTATOM(s_Data.HelperWindowClass),
	 											L"TRAP Message Window", WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
												0, 0, 1, 1, nullptr, nullptr, s_Data.Instance, nullptr);

	if (!s_Data.HelperWindowHandle)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to create helper window");
		return false;
	}

	//HACK: The command to the first ShowWindow call is ignored if the parent
	//      process passed along a STARTUPINFO, so clear that with a no-op call
	::ShowWindow(s_Data.HelperWindowHandle, SW_HIDE);

	//Register for HID device notifications
	{
		DEV_BROADCAST_DEVICEINTERFACE_W dbi;
		ZeroMemory(&dbi, sizeof(dbi));
		dbi.dbcc_size = sizeof(dbi);
		dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		dbi.dbcc_classguid = { 0x4d1e55b2,0xf16f,0x11cf,{0x88,0xcb,0x00,0x11,0x11,0x00,0x00,0x30} };

		s_Data.DeviceNotificationHandle = RegisterDeviceNotificationW(s_Data.HelperWindowHandle,
				                                                      reinterpret_cast<DEV_BROADCAST_HDR*>(&dbi),
				                                                      DEVICE_NOTIFY_WINDOW_HANDLE);
	}

	while (PeekMessageW(&msg, s_Data.HelperWindowHandle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the cursor is in the content area of the specified window
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::CursorInContentArea(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	RECT area{};
	POINT pos{};

	if (!::GetCursorPos(&pos))
		return false;

	if (WindowFromPoint(pos) != window->Handle)
		return false;

	GetClientRect(window->Handle, &area);
	ClientToScreen(window->Handle, reinterpret_cast<POINT*>(&area.left));
	ClientToScreen(window->Handle, reinterpret_cast<POINT*>(&area.right));

	return PtInRect(&area, pos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates an RGBA icon or cursor
[[nodiscard]] HICON TRAP::INTERNAL::WindowingAPI::CreateIcon(const Image* const image, const int32_t xHot, const int32_t yHot,
                                                             const bool icon)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	BITMAPV5HEADER bi;
	ICONINFO ii;
	uint8_t* target = nullptr;
	const uint8_t* source = static_cast<const uint8_t*>(image->GetPixelData());

	ZeroMemory(&bi, sizeof(bi));
	bi.bV5Size = sizeof(bi);
	bi.bV5Width = image->GetWidth();
	bi.bV5Height = -static_cast<int32_t>(image->GetHeight());
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	bi.bV5RedMask = 0x00ff0000;
	bi.bV5GreenMask = 0x0000ff00;
	bi.bV5BlueMask = 0x000000ff;
	bi.bV5AlphaMask = 0xff000000;

	const HDC dc = GetDC(nullptr);
	const HBITMAP color = CreateDIBSection(dc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS,
		                                   reinterpret_cast<void**>(&target), nullptr, static_cast<DWORD>(0));
	ReleaseDC(nullptr, dc);

	if (!color)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to create RGBA bitmap");
		return nullptr;
	}

	const HBITMAP mask = CreateBitmap(image->GetWidth(), image->GetHeight(), 1, 1, nullptr);
	if (!mask)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to create mask bitmap");
		DeleteObject(color);
		return nullptr;
	}

	for (uint32_t i = 0; i < image->GetWidth() * image->GetHeight(); i++)
	{
		target[0] = source[2];
		target[1] = source[1];
		target[2] = source[0];
		target[3] = source[3];
		target += 4;
		source += 4;
	}

	ZeroMemory(&ii, sizeof(ii));
	ii.fIcon = icon;
	ii.xHotspot = xHot;
	ii.yHotspot = yHot;
	ii.hbmMask = mask;
	ii.hbmColor = color;

	const HICON handle = CreateIconIndirect(&ii);

	DeleteObject(color);
	DeleteObject(mask);

	if (!handle)
	{
		if (icon)
			InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to create icon");
		else
			InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to create cursor");
	}

	return handle;
}

//-------------------------------------------------------------------------------------------------------------------//

//Enforce the content area aspect ratio based on which edge is being dragged
void TRAP::INTERNAL::WindowingAPI::ApplyAspectRatio(InternalWindow* window, int32_t edge, RECT* area)
{
	RECT frame{};
	const float ratio = static_cast<float>(window->Numerator) / static_cast<float>(window->Denominator);
	const DWORD style = GetWindowStyle(window);
	const DWORD exStyle = GetWindowExStyle(window);

	if(Utils::IsWindows10Version1607OrGreaterWin32())
	{
		s_Data.User32.AdjustWindowRectExForDPI(&frame, style, FALSE, exStyle,
		                                       s_Data.User32.GetDPIForWindow(window->Handle));
	}
	else
		AdjustWindowRectEx(&frame, style, FALSE, exStyle);

	if(edge == WMSZ_LEFT || edge == WMSZ_BOTTOMLEFT || edge == WMSZ_RIGHT || edge == WMSZ_BOTTOMRIGHT)
		area->bottom = area->top + (frame.bottom - frame.top) + static_cast<int32_t>(((area->right - area->left) - (frame.right - frame.left)) / ratio);
	else if(edge == WMSZ_TOPLEFT || edge == WMSZ_TOPRIGHT)
		area->top = area->bottom - (frame.bottom - frame.top) - static_cast<int32_t>(((area->right - area->left) - (frame.right - frame.left)) / ratio);
	else if(edge == WMSZ_TOP || edge == WMSZ_BOTTOM)
		area->right = area->left + (frame.right - frame.left) + static_cast<int32_t>(((area->bottom - area->top) - (frame.bottom - frame.top)) * ratio);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor clip rect to the window content area
void TRAP::INTERNAL::WindowingAPI::CaptureCursor(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	RECT clipRect;
	GetClientRect(window->Handle, &clipRect);
	ClientToScreen(window->Handle, reinterpret_cast<POINT*>(&clipRect.left));
	ClientToScreen(window->Handle, reinterpret_cast<POINT*>(&clipRect.right));
	ClipCursor(&clipRect);
	s_Data.CapturedCursorWindow = window;
}

//-------------------------------------------------------------------------------------------------------------------//

//Disabled clip cursor
void TRAP::INTERNAL::WindowingAPI::ReleaseCursor()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	ClipCursor(nullptr);
	s_Data.CapturedCursorWindow = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Enables WM_INPUT messages for the mouse for the specified window
void TRAP::INTERNAL::WindowingAPI::EnableRawMouseMotion(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const RAWINPUTDEVICE rid = { 0x01, 0x02, 0, window->Handle };

	if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to register raw input device");
}

//-------------------------------------------------------------------------------------------------------------------//

//Disables WM_INPUT messages for the mouse
void TRAP::INTERNAL::WindowingAPI::DisableRawMouseMotion(const InternalWindow*)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const RAWINPUTDEVICE rid = { 0x01, 0x02, RIDEV_REMOVE, nullptr };

	if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to remove raw input device");
}

//-------------------------------------------------------------------------------------------------------------------//

//Update native window styles to match attributes
void TRAP::INTERNAL::WindowingAPI::UpdateWindowStyles(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	RECT rect;
	DWORD style = static_cast<DWORD>(GetWindowLongPtrW(window->Handle, GWL_STYLE));
	style &= ~(WS_OVERLAPPEDWINDOW | WS_POPUP);
	style |= GetWindowStyle(window);

	GetClientRect(window->Handle, &rect);

	if (Utils::IsWindows10Version1607OrGreaterWin32())
		s_Data.User32.AdjustWindowRectExForDPI(&rect, style, FALSE, GetWindowExStyle(window), s_Data.User32.GetDPIForWindow(window->Handle));
	else
		AdjustWindowRectEx(&rect, style, FALSE, GetWindowExStyle(window));

	ClientToScreen(window->Handle, reinterpret_cast<POINT*>(&rect.left));
	ClientToScreen(window->Handle, reinterpret_cast<POINT*>(&rect.right));
	SetWindowLongPtrW(window->Handle, GWL_STYLE, style);
	::SetWindowPos(window->Handle, HWND_TOP, rect.left, rect.top,
		           rect.right - rect.left, rect.bottom - rect.top,
		           SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] HWND TRAP::INTERNAL::WindowingAPI::GetWin32Window(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return nullptr;
	}

	return window->Handle;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::PlatformGetVideoMode(const InternalMonitor* monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	DEVMODEW dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	EnumDisplaySettingsW(monitor->AdapterName.data(), ENUM_CURRENT_SETTINGS, &dm);

	//dm.dmDisplayFrequency is an integer which is rounded down, so it's
	//highly likely that 23 represents 24/1.001 Hz, 59 represents 60/1.001 Hz, etc.
	//A caller can always reproduce the original value by using floor.
	double refreshRate = static_cast<double>(dm.dmDisplayFrequency);
	switch(dm.dmDisplayFrequency)
	{
	case 23:
		[[fallthrough]];
	case 29:
		[[fallthrough]];
	case 47:
		[[fallthrough]];
	case 59:
		[[fallthrough]];
	case 71:
		[[fallthrough]];
	case 89:
		[[fallthrough]];
	case 95:
		[[fallthrough]];
	case 119:
		[[fallthrough]];
	case 143:
		refreshRate = static_cast<double>(refreshRate + 1) / 1.001;
		break;

	default:
		break;
	}

	InternalVideoMode mode{};
	mode.Width = dm.dmPelsWidth;
	mode.Height = dm.dmPelsHeight;
	mode.RefreshRate = refreshRate;
	SplitBPP(dm.dmBitsPerPel, mode.RedBits, mode.GreenBits, mode.BlueBits);

	return mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowSize(const InternalWindow* window, int32_t& width,
                                                         int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	RECT area{};
	GetClientRect(window->Handle, &area);

	width = area.right;
	height = area.bottom;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowPos(const InternalWindow* window, const int32_t xPos,
                                                        const int32_t yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	RECT rect = { xPos, yPos, xPos, yPos };

	if (Utils::IsWindows10Version1607OrGreaterWin32())
	{
		s_Data.User32.AdjustWindowRectExForDPI(&rect, GetWindowStyle(window), FALSE, GetWindowExStyle(window),
			                                   s_Data.User32.GetDPIForWindow(window->Handle));
	}
	else
		AdjustWindowRectEx(&rect, GetWindowStyle(window), FALSE, GetWindowExStyle(window));

	::SetWindowPos(window->Handle, nullptr, rect.left, rect.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitor(InternalWindow* window, InternalMonitor* monitor,
	                                                        const int32_t xPos, const int32_t yPos,
	                                                        const int32_t width, const int32_t height, const double)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (window->Monitor == monitor)
	{
		if (monitor)
		{
			if (monitor->Window == window)
			{
				AcquireMonitor(window);
				FitToMonitor(window);
			}
		}
		else
		{
			RECT rect = { xPos, yPos, xPos + width, yPos + height };

			if (Utils::IsWindows10Version1607OrGreaterWin32())
			{
				s_Data.User32.AdjustWindowRectExForDPI(&rect, GetWindowStyle(window), FALSE,
				                                       GetWindowExStyle(window),
													   s_Data.User32.GetDPIForWindow(window->Handle));
			}
			else
				AdjustWindowRectEx(&rect, GetWindowStyle(window), FALSE, GetWindowExStyle(window));

			::SetWindowPos(window->Handle, HWND_TOP, rect.left, rect.top, rect.right - rect.left,
			               rect.bottom - rect.top, SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER);
		}

		return;
	}

	if (window->Monitor)
		ReleaseMonitor(window);
	if (window->BorderlessFullscreen)
		window->BorderlessFullscreen = false;

	window->Monitor = monitor;

	if (window->Monitor)
	{
		MONITORINFO mi = { sizeof(mi) };
		UINT flags = SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOCOPYBITS;

		if (window->Decorated)
		{
			DWORD style = static_cast<DWORD>(GetWindowLongPtrW(window->Handle, GWL_STYLE));
			style &= ~WS_OVERLAPPEDWINDOW;
			style |= GetWindowStyle(window);
			SetWindowLongPtrW(window->Handle, GWL_STYLE, style);
			flags |= SWP_FRAMECHANGED;
		}

		AcquireMonitor(window);

		GetMonitorInfoW(window->Monitor->Handle, &mi);
		::SetWindowPos(window->Handle, HWND_TOPMOST, mi.rcMonitor.left, mi.rcMonitor.top,
		               mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, flags);
	}
	else
	{
		RECT rect = { xPos, yPos, xPos + width, yPos + height };
		DWORD style = static_cast<DWORD>(GetWindowLongPtrW(window->Handle, GWL_STYLE));
		UINT flags = SWP_NOACTIVATE | SWP_NOCOPYBITS;

		if (window->Decorated)
		{
			style &= ~WS_POPUP;
			style |= GetWindowStyle(window);
			SetWindowLongPtrW(window->Handle, GWL_STYLE, style);

			flags |= SWP_FRAMECHANGED;
		}

		HWND after;
		if (window->Floating)
			after = HWND_TOPMOST;
		else
			after = HWND_NOTOPMOST;

		if (Utils::IsWindows10Version1607OrGreaterWin32())
		{
			s_Data.User32.AdjustWindowRectExForDPI(&rect, GetWindowStyle(window), FALSE, GetWindowExStyle(window),
				                                   s_Data.User32.GetDPIForWindow(window->Handle));
		}
		else
			AdjustWindowRectEx(&rect, GetWindowStyle(window), FALSE, GetWindowExStyle(window));

		::SetWindowPos(window->Handle, after, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			           flags);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	window->BorderlessFullscreen = true;
	window->Monitor = monitor;

	if(!window->Monitor)
		return;

	MONITORINFO mi = { sizeof(mi) };
	UINT flags = SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOCOPYBITS;

	if (window->Decorated)
	{
		DWORD style = static_cast<DWORD>(GetWindowLongPtrW(window->Handle, GWL_STYLE));
		style &= ~WS_OVERLAPPEDWINDOW;
		style |= GetWindowStyle(window);
		SetWindowLongPtrW(window->Handle, GWL_STYLE, style);
		flags |= SWP_FRAMECHANGED;
	}

	AcquireMonitorBorderless(window);

	GetMonitorInfoW(window->Monitor->Handle, &mi);
	::SetWindowPos(window->Handle, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
	               mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, flags);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::PlatformGetVideoModes(const InternalMonitor* monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	uint32_t modeIndex = 0, size = 0, count = 0;
	std::vector<InternalVideoMode> result{};

	while(true)
	{
		uint32_t i;
		InternalVideoMode mode;
		DEVMODEW dm;

		ZeroMemory(&dm, sizeof(dm));
		dm.dmSize = sizeof(dm);

		if (!EnumDisplaySettingsW(monitor->AdapterName.data(), modeIndex, &dm))
			break;

		modeIndex++;

		//Skip modes with less than 15 BPP
		if (dm.dmBitsPerPel < 15)
			continue;

		//dm.dmDisplayFrequency is an integer which is rounded down, so it's
		//highly likely that 23 represents 24/1.001 Hz, 59 represents 60/1.001 Hz, etc.
		//A caller can always reproduce the original value by using floor.
		double refreshRate = static_cast<double>(dm.dmDisplayFrequency);
		switch(dm.dmDisplayFrequency)
		{
		case 23:
			[[fallthrough]];
		case 29:
			[[fallthrough]];
		case 47:
			[[fallthrough]];
		case 59:
			[[fallthrough]];
		case 71:
			[[fallthrough]];
		case 89:
			[[fallthrough]];
		case 95:
			[[fallthrough]];
		case 119:
			[[fallthrough]];
		case 143:
			refreshRate = static_cast<double>(refreshRate + 1) / 1.001;

		default:
			break;
		}

		mode.Width = dm.dmPelsWidth;
		mode.Height = dm.dmPelsHeight;
		mode.RefreshRate = refreshRate;
		SplitBPP(dm.dmBitsPerPel, mode.RedBits, mode.GreenBits, mode.BlueBits);

		for (i = 0; i < count; i++)
		{
			if (IsSameVideoMode(result[i], mode))
				break;
		}

		//Skip duplicate modes
		if (i < count)
			continue;

		//I do not see any difference with this enabled other than WinAPI lagging while doing this call
		/*if (monitor->ModesPruned)
		{
			//Skip modes not supported by the connected displays
			if (ChangeDisplaySettingsExW(monitor->AdapterName.data(),
				&dm,
				nullptr,
				CDS_TEST,
				nullptr) != DISP_CHANGE_SUCCESSFUL) //Performance Slow!
			{
				continue;
			}
		}*/

		if (count == size)
		{
			size += 128;
			result.reserve(result.size() + 128);
		}

		count++;
		result.push_back(mode);
	}

	result.shrink_to_fit();

	if (!count)
	{
		//HACK: Report the current mode if no valid modes were found
		result.resize(1);
		result[0] = PlatformGetVideoMode(monitor);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformInit()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (!LoadLibraries())
		return false;

	CreateKeyTables();
	UpdateKeyNamesWin32();

	if (Utils::IsWindows10Version1703OrGreaterWin32())
		s_Data.User32.SetProcessDPIAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	else if (Utils::IsWindows8Point1OrGreaterWin32())
		s_Data.SHCore.SetProcessDPIAwareness(Process_DPI_Awareness::Process_Per_Monitor_DPI_Aware); //Process per monitor DPI aware

	s_Data.User32.SetProcessDPIAware();

	if (!CreateHelperWindow())
		return false;

	PollMonitorsWin32();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyWindow(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (window->Monitor)
		ReleaseMonitor(window);

	if (s_Data.DisabledCursorWindow == window)
		EnableCursor(window);

	if (s_Data.CapturedCursorWindow == window)
		ReleaseCursor();

	if (window->TaskbarList)
		window->TaskbarList->Release();

	if (window->Handle)
	{
		RemovePropW(window->Handle, L"TRAP");
		::DestroyWindow(window->Handle);
		window->Handle = nullptr;
	}

	if (window->BigIcon)
		DestroyIcon(window->BigIcon);

	if (window->SmallIcon)
		DestroyIcon(window->SmallIcon);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShutdown()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (s_Data.DeviceNotificationHandle)
		UnregisterDeviceNotification(s_Data.DeviceNotificationHandle);

	if (s_Data.HelperWindowHandle)
		::DestroyWindow(s_Data.HelperWindowHandle);
	if (s_Data.HelperWindowClass)
		::UnregisterClassW(MAKEINTATOM(s_Data.HelperWindowClass), s_Data.Instance);
	if (s_Data.MainWindowClass)
		::UnregisterClassW(MAKEINTATOM(s_Data.MainWindowClass), s_Data.Instance);

	s_Data.ClipboardString = {};
	s_Data.RawInput = {};

	FreeLibraries();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorContentScale(const InternalMonitor* monitor, float& xScale,
                                                                  float& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	GetMonitorContentScaleWin32(monitor->Handle, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorPos(const InternalMonitor* monitor, int32_t& xPos,
                                                         int32_t& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	DEVMODEW dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	EnumDisplaySettingsExW(monitor->AdapterName.data(), ENUM_CURRENT_SETTINGS, &dm, EDS_ROTATEDMODE);

	xPos = dm.dmPosition.x;
	yPos = dm.dmPosition.y;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformShowWindow(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	::ShowWindow(window->Handle, SW_SHOWNA);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformFocusWindow(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	BringWindowToTop(window->Handle);
	SetForegroundWindow(window->Handle);
	SetFocus(window->Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformCreateWindow(InternalWindow* window, WindowConfig& WNDConfig)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (!CreateNativeWindow(window, WNDConfig))
		return false;

	if(WNDConfig.MousePassthrough)
		PlatformSetWindowMousePassthrough(window, true);

	if (window->Monitor)
	{
		PlatformShowWindow(window);
		PlatformFocusWindow(window);
		AcquireMonitor(window);
		FitToMonitor(window);
	}
	else
	{
		if(WNDConfig.Visible)
		{
			PlatformShowWindow(window);
			if(WNDConfig.Focused)
				PlatformFocusWindow(window);
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowTitle(const InternalWindow* window, const std::string& title)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::wstring wideTitle = TRAP::Utils::String::CreateWideStringFromUTF8StringWin32(title);
	if (wideTitle.empty())
		return;

	SetWindowTextW(window->Handle, wideTitle.data());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformCreateCursor(InternalCursor* cursor, const Image* const image,
                                                                      const int32_t xHotspot, const int32_t yHotspot)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	cursor->Handle = CreateIcon(image, xHotspot, yHotspot, false);
	if (!cursor->Handle)
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformCreateStandardCursor(InternalCursor* cursor, const CursorType& type)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	uint32_t id = OCR_NORMAL;

	switch(type)
	{
	case CursorType::Arrow:
		id = OCR_NORMAL;
		break;

	case CursorType::Input:
		id = OCR_IBEAM;
		break;

	case CursorType::Crosshair:
		id = OCR_CROSS;
		break;

	case CursorType::PointingHand:
		id = OCR_HAND;
		break;

	case CursorType::ResizeVertical:
		id = OCR_SIZENS;
		break;

	case CursorType::ResizeHorizontal:
		id = OCR_SIZEWE;
		break;

	case CursorType::ResizeDiagonalTopLeftBottomRight:
		id = OCR_SIZENWSE;
		break;

	case CursorType::ResizeDiagonalTopRightBottomLeft:
		id = OCR_SIZENESW;
		break;

	case CursorType::ResizeAll:
		id = OCR_SIZEALL;
		break;

	case CursorType::NotAllowed:
		id = OCR_NO;
		break;

	default:
		break;
	}

	cursor->Handle = static_cast<HCURSOR>(LoadImageW(nullptr, MAKEINTRESOURCEW(id), IMAGE_CURSOR, 0, 0,
		                                             LR_DEFAULTSIZE | LR_SHARED));
	if (!cursor->Handle)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to create standard cursor");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformDestroyCursor(InternalCursor* cursor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (cursor->Handle)
		DestroyIcon(cursor->Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursor(const InternalWindow* window, const InternalCursor*)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (CursorInContentArea(window))
		UpdateCursorImage(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorMode(InternalWindow* window, const CursorMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

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

	if (CursorInContentArea(window))
		UpdateCursorImage(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetCursorPos(InternalWindow* window, const double xPos, const double yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	POINT pos = { static_cast<int32_t>(xPos), static_cast<int32_t>(yPos) };

	//Store the new position so it can be recognized later
	window->LastCursorPosX = pos.x;
	window->LastCursorPosY = pos.y;

	ClientToScreen(window->Handle, &pos);
	::SetCursorPos(pos.x, pos.y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetCursorPos(const InternalWindow* window, double& xPos, double& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	POINT pos;

	if (::GetCursorPos(&pos))
	{
		ScreenToClient(window->Handle, &pos);

		xPos = pos.x;
		yPos = pos.y;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowIcon(InternalWindow* window, const Image* const image)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	HICON bigIcon, smallIcon;

	if (image)
	{
		bigIcon = CreateIcon(image, 0, 0, true);
		smallIcon = bigIcon;
	}
	else
	{
		bigIcon = reinterpret_cast<HICON>(GetClassLongPtrW(window->Handle, GCLP_HICON));
		smallIcon = reinterpret_cast<HICON>(GetClassLongPtrW(window->Handle, GCLP_HICONSM));
	}

	SendMessageW(window->Handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(bigIcon));
	SendMessageW(window->Handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(smallIcon));

	if (window->BigIcon)
		DestroyIcon(window->BigIcon);

	if (window->SmallIcon)
		DestroyIcon(window->SmallIcon);

	if (image)
	{
		window->BigIcon = bigIcon;
		window->SmallIcon = smallIcon;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	POINT pos = { 0, 0 };
	ClientToScreen(window->Handle, &pos);

	xPos = pos.x;
	yPos = pos.y;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSize(InternalWindow* window, const int32_t width,
                                                         const int32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (window->Monitor)
	{
		if (window->Monitor->Window == window)
		{
			AcquireMonitor(window);
			FitToMonitor(window);
		}
	}
	else
	{
		RECT rect = { 0, 0, width, height };

		if (Utils::IsWindows10Version1607OrGreaterWin32())
		{
			s_Data.User32.AdjustWindowRectExForDPI(&rect, GetWindowStyle(window), FALSE, GetWindowExStyle(window),
				                                   s_Data.User32.GetDPIForWindow(window->Handle));
		}
		else
			AdjustWindowRectEx(&rect, GetWindowStyle(window), FALSE, GetWindowExStyle(window));

		::SetWindowPos(window->Handle, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
			           SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowResizable(InternalWindow* window, const bool)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	UpdateWindowStyles(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowDecorated(const InternalWindow* window, const bool)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	UpdateWindowStyles(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowFloating(const InternalWindow* window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const HWND after = enabled ? HWND_TOPMOST : HWND_NOTOPMOST;
	::SetWindowPos(window->Handle, after, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowOpacity(const InternalWindow* window, const float opacity)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	LONG exStyle = GetWindowLongW(window->Handle, GWL_EXSTYLE);
	if (opacity < 1.0f || (exStyle & WS_EX_TRANSPARENT))
	{
		const BYTE alpha = static_cast<BYTE>(255 * opacity);
		exStyle |= WS_EX_LAYERED;
		SetWindowLongPtrW(window->Handle, GWL_EXSTYLE, exStyle);
		SetLayeredWindowAttributes(window->Handle, 0, alpha, LWA_ALPHA);
	}
	else if (exStyle & WS_EX_TRANSPARENT)
		SetLayeredWindowAttributes(window->Handle, 0, 0, 0);
	else
	{
		exStyle &= ~WS_EX_LAYERED;
		SetWindowLongPtrW(window->Handle, GWL_EXSTYLE, exStyle);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowMousePassthrough(InternalWindow* window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	COLORREF key = 0;
	BYTE alpha = 0;
	DWORD flags = 0;
	DWORD exStyle = static_cast<DWORD>(GetWindowLongPtrW(window->Handle, GWL_EXSTYLE));

	if (exStyle & WS_EX_LAYERED)
		GetLayeredWindowAttributes(window->Handle, &key, &alpha, &flags);

	if (enabled)
		exStyle |= (WS_EX_TRANSPARENT | WS_EX_LAYERED);
	else
	{
		exStyle &= ~WS_EX_TRANSPARENT;
		//Note: Window opacity and framebuffer transparency also need to
		//      control the layered style so avoid stepping on their feet
		if(exStyle & WS_EX_LAYERED)
		{
			if (!(flags & (LWA_ALPHA | LWA_COLORKEY)))
				exStyle &= ~WS_EX_LAYERED;
		}
	}

	SetWindowLongPtrW(window->Handle, GWL_EXSTYLE, exStyle);

	if (enabled)
		SetLayeredWindowAttributes(window->Handle, key, alpha, flags);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::INTERNAL::WindowingAPI::PlatformGetWindowOpacity(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	BYTE alpha;
	DWORD flags;

	if((GetWindowLongPtrW(window->Handle, GWL_EXSTYLE) & WS_EX_LAYERED) &&
	   GetLayeredWindowAttributes(window->Handle, nullptr, &alpha, &flags))
	{
		if (flags & LWA_ALPHA)
			return static_cast<float>(alpha) / 255.0f;
	}

	return 1.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetFrameBufferSize(const InternalWindow* window, int32_t& width,
                                                              int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	PlatformGetWindowSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetWindowContentScale(const InternalWindow* window, float& xScale,
                                                                 float& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const HANDLE handle = MonitorFromWindow(window->Handle, MONITOR_DEFAULTTONEAREST);
	GetMonitorContentScaleWin32(static_cast<HMONITOR>(handle), xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos,
                                                              int32_t& yPos, int32_t& width, int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	MONITORINFO mi = {};
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfoW(monitor->Handle, &mi);

	xPos = mi.rcWork.left;
	yPos = mi.rcWork.top;
	width = mi.rcWork.right - mi.rcWork.left;
	height = mi.rcWork.bottom - mi.rcWork.top;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowVisible(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	return IsWindowVisible(window->Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMaximized(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	return IsZoomed(window->Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowMinimized(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	return IsIconic(window->Handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPollEvents()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	MSG msg;

	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			//NOTE: While TRAP does not itself post WM_QUIT, other processes
			//      may post it to this one, for example Task Manager
			//HACK: Treat WM_QUIT as a close on all windows

			for(const Scope<InternalWindow>& win : s_Data.WindowList)
				InputWindowCloseRequest(win.get());
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	//HACK: Release modifier keys that the system did not emit KEYUP for
	//NOTE: Shift keys on Windows tend to "stick" when both are pressed as no key up message is generated
	//      by the first key release
	//NOTE: Windows key is not reported as released by the Win+V hot-key
	//      Other Win hot-keys are handled implicitly by InputWindowFocus because they change the input focus
	const HWND handle = GetActiveWindow();
	if (handle)
	{
		InternalWindow* windowPtr = static_cast<InternalWindow*>(GetPropW(handle, L"TRAP"));

		if (windowPtr)
		{
			const std::array<std::array<int32_t, 2>, 4> keys =
			{
				{
					{ VK_LSHIFT, static_cast<int32_t>(Input::Key::Left_Shift)},
					{ VK_RSHIFT, static_cast<int32_t>(Input::Key::Right_Super)},
					{ VK_LWIN, static_cast<int32_t>(Input::Key::Left_Super)},
					{ VK_RWIN, static_cast<int32_t>(Input::Key::Right_Super)}
				}
			};

			for(int32_t i = 0; i < 4; i++)
			{
				const int32_t vk = keys[i][0];
				const Input::Key key = static_cast<Input::Key>(keys[i][1]);
				const int32_t scanCode = s_Data.ScanCodes[static_cast<int32_t>(key)];

				if ((GetKeyState(vk) & 0x8000))
					continue;
				if (windowPtr->Keys[static_cast<int32_t>(key)] != Input::KeyState::Pressed)
					continue;

				InputKey(windowPtr, key, scanCode, Input::KeyState::Released);
			}
		}
	}

	InternalWindow* window = s_Data.DisabledCursorWindow;
	if (window)
	{
		int32_t width, height;
		PlatformGetWindowSize(window, width, height);

		if (window->LastCursorPosX != width / 2 ||
			window->LastCursorPosY != height / 2)
			PlatformSetCursorPos(window, static_cast<double>(width) / 2.0, static_cast<double>(height) / 2.0);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformWaitEvents(const double timeout)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(timeout == 0.0)
		WaitMessage();
	else
		MsgWaitForMultipleObjects(0, nullptr, FALSE, static_cast<DWORD>(timeout * 1000.0), QS_ALLEVENTS);

	PlatformPollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformPostEmptyEvent()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	PostMessageW(s_Data.HelperWindowHandle, WM_NULL, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowFocused(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	return window->Handle == GetForegroundWindow();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformWindowHovered(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	return CursorInContentArea(window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::PlatformRawMouseMotionSupported()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetRawMouseMotion(const InternalWindow* window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (s_Data.DisabledCursorWindow != window)
		return;

	if (enabled)
		EnableRawMouseMotion(window);
	else
		DisableRawMouseMotion(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetProgress(const InternalWindow* window, const ProgressState state,
													   const uint32_t completed)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!window->TaskbarList)
		return;

	const uint32_t progress = TRAP::Math::Clamp(completed, 0u, 100u);

	HRESULT res = window->TaskbarList->SetProgressValue(window->Handle, progress, 100u);
	if(res != S_OK)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to set progress value");
		return;
	}

	TBPFLAG flag{};
	switch(state)
	{
	case ProgressState::Indeterminate:
		flag = TBPF_INDETERMINATE;
		break;
	case ProgressState::Normal:
		flag = TBPF_NORMAL;
		break;
	case ProgressState::Error:
		flag = TBPF_ERROR;
		break;
	case ProgressState::Paused:
		flag = TBPF_PAUSED;
		break;
	case ProgressState::NoProgress:
		[[fallthrough]];
	default:
		flag = TBPF_NOPROGRESS;
		break;
	}

	res = window->TaskbarList->SetProgressState(window->Handle, flag);
	if(res != S_OK)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to set progress state");
		return;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] int32_t TRAP::INTERNAL::WindowingAPI::PlatformGetKeyScanCode(const Input::Key key)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Data.ScanCodes[static_cast<uint32_t>(key)];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const char* TRAP::INTERNAL::WindowingAPI::PlatformGetScanCodeName(const int32_t scanCode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Data.KeyNames[static_cast<uint32_t>(s_Data.KeyCodes[scanCode])].data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetClipboardString(const std::string& string)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const int32_t characterCount = MultiByteToWideChar(CP_UTF8, 0, string.data(), -1, nullptr, 0);
	if (!characterCount)
		return;

	const HANDLE object = GlobalAlloc(GMEM_MOVEABLE, characterCount * sizeof(WCHAR));
	if (!object)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to allocate global handle for clipboard");
		return;
	}

	WCHAR* buffer = static_cast<WCHAR*>(GlobalLock(object));
	if (!buffer)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to lock global handle");
		GlobalFree(object);
		return;
	}

	MultiByteToWideChar(CP_UTF8, 0, string.data(), -1, buffer, characterCount);
	GlobalUnlock(object);

	if (!OpenClipboard(s_Data.HelperWindowHandle))
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to open clipboard");
		GlobalFree(object);
		return;
	}

	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, object);
	CloseClipboard();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::PlatformGetClipboardString()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (!OpenClipboard(s_Data.HelperWindowHandle))
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to open clipboard");
		return {};
	}

	const HANDLE object = GetClipboardData(CF_UNICODETEXT);
	if (!object)
	{
		InputErrorWin32(Error::Format_Unavailable, "[WinAPI] Failed to convert clipboard to string");
		CloseClipboard();
		return {};
	}

	WCHAR* buffer = static_cast<WCHAR*>(GlobalLock(object));
	if (!buffer)
	{
		InputErrorWin32(Error::Platform_Error, "[WinAPI] Failed to lock global handle");
		CloseClipboard();
		return {};
	}

	s_Data.ClipboardString = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(buffer);

	GlobalUnlock(object);
	CloseClipboard();

	return s_Data.ClipboardString;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformGetRequiredInstanceExtensions(std::array<std::string, 2>& extensions)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (!s_Data.VK.KHR_Surface || !s_Data.VK.KHR_Win32_Surface)
		return;

	extensions[0] = "VK_KHR_surface";
	extensions[1] = "VK_KHR_win32_surface";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkResult TRAP::INTERNAL::WindowingAPI::PlatformCreateWindowSurface(VkInstance instance, const InternalWindow* window,
                                                                                 const VkAllocationCallbacks* allocator,
																                 VkSurfaceKHR& surface)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const PFN_vkCreateWin32SurfaceKHR _vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>
		(
			vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"
		));

	if (!_vkCreateWin32SurfaceKHR)
	{
		InputError(Error::API_Unavailable, "[WinAPI] Vulkan instance missing VK_KHR_win32_surface extension");
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	const VkWin32SurfaceCreateInfoKHR sci
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0,
		s_Data.Instance,
		window->Handle
	};

	const VkResult err = _vkCreateWin32SurfaceKHR(instance, &sci, allocator, &surface);
	if (err)
		InputError(Error::Platform_Error, "[WinAPI] Failed to create Vulkan surface: " + GetVulkanResultString(err));

	return err;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMaximizeWindow(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(IsWindowVisible(window->Handle))
		::ShowWindow(window->Handle, SW_MAXIMIZE);
	else
		MaximizeWindowManually(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformMinimizeWindow(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	::ShowWindow(window->Handle, SW_MINIMIZE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRequestWindowAttention(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	FlashWindow(window->Handle, TRUE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindow(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	::ShowWindow(window->Handle, SW_HIDE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformRestoreWindow(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	::ShowWindow(window->Handle, SW_RESTORE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowSizeLimits(InternalWindow* window, const int32_t minWidth,
                                                               const int32_t minHeight, const int32_t maxWidth,
                                                               const int32_t maxHeight)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	RECT area{};

	if ((minWidth == -1 || minHeight == -1) &&
		(maxWidth == -1 || maxHeight == -1))
		return;

	GetWindowRect(window->Handle, &area);
	MoveWindow(window->Handle, area.left, area.top, area.right - area.left, area.bottom - area.top, TRUE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetWindowAspectRatio(InternalWindow* window, const int32_t numerator,
                                                                const int32_t denominator)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	RECT area{};

	if(numerator == -1 || denominator == -1)
		return;

	GetWindowRect(window->Handle, &area);
	ApplyAspectRatio(window, WMSZ_BOTTOMRIGHT, &area);
	MoveWindow(window->Handle, area.left, area.top, area.right - area.left, area.bottom - area.top, TRUE);
}

//-------------------------------------------------------------------------------------------------------------------//

//Updates the cursor image according to its cursor mode
void TRAP::INTERNAL::WindowingAPI::UpdateCursorImage(const InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (window->cursorMode == CursorMode::Normal || window->cursorMode == CursorMode::Captured)
	{
		if (window->Cursor)
			::SetCursor(window->Cursor->Handle);
		else
			::SetCursor(LoadCursorW(nullptr, IDC_ARROW));
	}
	else
		::SetCursor(nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

//Exit disabled cursor mode for the specified window
void TRAP::INTERNAL::WindowingAPI::EnableCursor(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (window->RawMouseMotion)
		DisableRawMouseMotion(window);

	s_Data.DisabledCursorWindow = nullptr;
	ReleaseCursor();
	PlatformSetCursorPos(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
	UpdateCursorImage(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Apply disabled cursor mode to a focused window
void TRAP::INTERNAL::WindowingAPI::DisableCursor(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	s_Data.DisabledCursorWindow = window;
	PlatformGetCursorPos(window, s_Data.RestoreCursorPosX, s_Data.RestoreCursorPosY);
	UpdateCursorImage(window);
	CenterCursorInContentArea(window);
	CaptureCursor(window);

	if (window->RawMouseMotion)
		EnableRawMouseMotion(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Create key code translation tables
void TRAP::INTERNAL::WindowingAPI::CreateKeyTables()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	std::fill(s_Data.KeyCodes.begin(), s_Data.KeyCodes.end(), Input::Key::Unknown);
	std::fill(s_Data.ScanCodes.begin(), s_Data.ScanCodes.end(), static_cast<int16_t>(-1));

	s_Data.KeyCodes[0x00B] = Input::Key::Zero;
	s_Data.KeyCodes[0x002] = Input::Key::One;
	s_Data.KeyCodes[0x003] = Input::Key::Two;
	s_Data.KeyCodes[0x004] = Input::Key::Three;
	s_Data.KeyCodes[0x005] = Input::Key::Four;
	s_Data.KeyCodes[0x006] = Input::Key::Five;
	s_Data.KeyCodes[0x007] = Input::Key::Six;
	s_Data.KeyCodes[0x008] = Input::Key::Seven;
	s_Data.KeyCodes[0x009] = Input::Key::Eight;
	s_Data.KeyCodes[0x00A] = Input::Key::Nine;
	s_Data.KeyCodes[0x01E] = Input::Key::A;
	s_Data.KeyCodes[0x030] = Input::Key::B;
	s_Data.KeyCodes[0x02E] = Input::Key::C;
	s_Data.KeyCodes[0x020] = Input::Key::D;
	s_Data.KeyCodes[0x012] = Input::Key::E;
	s_Data.KeyCodes[0x021] = Input::Key::F;
	s_Data.KeyCodes[0x022] = Input::Key::G;
	s_Data.KeyCodes[0x023] = Input::Key::H;
	s_Data.KeyCodes[0x017] = Input::Key::I;
	s_Data.KeyCodes[0x024] = Input::Key::J;
	s_Data.KeyCodes[0x025] = Input::Key::K;
	s_Data.KeyCodes[0x026] = Input::Key::L;
	s_Data.KeyCodes[0x032] = Input::Key::M;
	s_Data.KeyCodes[0x031] = Input::Key::N;
	s_Data.KeyCodes[0x018] = Input::Key::O;
	s_Data.KeyCodes[0x019] = Input::Key::P;
	s_Data.KeyCodes[0x010] = Input::Key::Q;
	s_Data.KeyCodes[0x013] = Input::Key::R;
	s_Data.KeyCodes[0x01F] = Input::Key::S;
	s_Data.KeyCodes[0x014] = Input::Key::T;
	s_Data.KeyCodes[0x016] = Input::Key::U;
	s_Data.KeyCodes[0x02F] = Input::Key::V;
	s_Data.KeyCodes[0x011] = Input::Key::W;
	s_Data.KeyCodes[0x02D] = Input::Key::X;
	s_Data.KeyCodes[0x015] = Input::Key::Y;
	s_Data.KeyCodes[0x02C] = Input::Key::Z;

	s_Data.KeyCodes[0x028] = Input::Key::Apostrophe;
	s_Data.KeyCodes[0x02B] = Input::Key::Backslash;
	s_Data.KeyCodes[0x033] = Input::Key::Comma;
	s_Data.KeyCodes[0x00D] = Input::Key::Equal;
	s_Data.KeyCodes[0x029] = Input::Key::Grave_Accent;
	s_Data.KeyCodes[0x01A] = Input::Key::Left_Bracket;
	s_Data.KeyCodes[0x00C] = Input::Key::Minus;
	s_Data.KeyCodes[0x034] = Input::Key::Period;
	s_Data.KeyCodes[0x01B] = Input::Key::Right_Bracket;
	s_Data.KeyCodes[0x027] = Input::Key::Semicolon;
	s_Data.KeyCodes[0x035] = Input::Key::Slash;
	s_Data.KeyCodes[0x056] = Input::Key::World_2;

	s_Data.KeyCodes[0x00E] = Input::Key::Backspace;
	s_Data.KeyCodes[0x153] = Input::Key::Delete;
	s_Data.KeyCodes[0x14F] = Input::Key::End;
	s_Data.KeyCodes[0x01C] = Input::Key::Enter;
	s_Data.KeyCodes[0x001] = Input::Key::Escape;
	s_Data.KeyCodes[0x147] = Input::Key::Home;
	s_Data.KeyCodes[0x152] = Input::Key::Insert;
	s_Data.KeyCodes[0x15D] = Input::Key::Menu;
	s_Data.KeyCodes[0x151] = Input::Key::Page_Down;
	s_Data.KeyCodes[0x149] = Input::Key::Page_Up;
	s_Data.KeyCodes[0x045] = Input::Key::Pause;
	s_Data.KeyCodes[0x039] = Input::Key::Space;
	s_Data.KeyCodes[0x00F] = Input::Key::Tab;
	s_Data.KeyCodes[0x03A] = Input::Key::Caps_Lock;
	s_Data.KeyCodes[0x145] = Input::Key::Num_Lock;
	s_Data.KeyCodes[0x046] = Input::Key::Scroll_Lock;
	s_Data.KeyCodes[0x03B] = Input::Key::F1;
	s_Data.KeyCodes[0x03C] = Input::Key::F2;
	s_Data.KeyCodes[0x03D] = Input::Key::F3;
	s_Data.KeyCodes[0x03E] = Input::Key::F4;
	s_Data.KeyCodes[0x03F] = Input::Key::F5;
	s_Data.KeyCodes[0x040] = Input::Key::F6;
	s_Data.KeyCodes[0x041] = Input::Key::F7;
	s_Data.KeyCodes[0x042] = Input::Key::F8;
	s_Data.KeyCodes[0x043] = Input::Key::F9;
	s_Data.KeyCodes[0x044] = Input::Key::F10;
	s_Data.KeyCodes[0x057] = Input::Key::F11;
	s_Data.KeyCodes[0x058] = Input::Key::F12;
	s_Data.KeyCodes[0x064] = Input::Key::F13;
	s_Data.KeyCodes[0x065] = Input::Key::F14;
	s_Data.KeyCodes[0x066] = Input::Key::F15;
	s_Data.KeyCodes[0x067] = Input::Key::F16;
	s_Data.KeyCodes[0x068] = Input::Key::F17;
	s_Data.KeyCodes[0x069] = Input::Key::F18;
	s_Data.KeyCodes[0x06A] = Input::Key::F19;
	s_Data.KeyCodes[0x06B] = Input::Key::F20;
	s_Data.KeyCodes[0x06C] = Input::Key::F21;
	s_Data.KeyCodes[0x06D] = Input::Key::F22;
	s_Data.KeyCodes[0x06E] = Input::Key::F23;
	s_Data.KeyCodes[0x076] = Input::Key::F24;
	s_Data.KeyCodes[0x038] = Input::Key::Left_ALT;
	s_Data.KeyCodes[0x01D] = Input::Key::Left_Control;
	s_Data.KeyCodes[0x02A] = Input::Key::Left_Shift;
	s_Data.KeyCodes[0x15B] = Input::Key::Left_Super;
	s_Data.KeyCodes[0x137] = Input::Key::Print_Screen;
	s_Data.KeyCodes[0x138] = Input::Key::Right_ALT;
	s_Data.KeyCodes[0x11D] = Input::Key::Right_Control;
	s_Data.KeyCodes[0x036] = Input::Key::Right_Shift;
	s_Data.KeyCodes[0x15C] = Input::Key::Right_Super;
	s_Data.KeyCodes[0x150] = Input::Key::Down;
	s_Data.KeyCodes[0x14B] = Input::Key::Left;
	s_Data.KeyCodes[0x14D] = Input::Key::Right;
	s_Data.KeyCodes[0x148] = Input::Key::Up;

	s_Data.KeyCodes[0x052] = Input::Key::KP_0;
	s_Data.KeyCodes[0x04F] = Input::Key::KP_1;
	s_Data.KeyCodes[0x050] = Input::Key::KP_2;
	s_Data.KeyCodes[0x051] = Input::Key::KP_3;
	s_Data.KeyCodes[0x04B] = Input::Key::KP_4;
	s_Data.KeyCodes[0x04C] = Input::Key::KP_5;
	s_Data.KeyCodes[0x04D] = Input::Key::KP_6;
	s_Data.KeyCodes[0x047] = Input::Key::KP_7;
	s_Data.KeyCodes[0x048] = Input::Key::KP_8;
	s_Data.KeyCodes[0x049] = Input::Key::KP_9;
	s_Data.KeyCodes[0x04E] = Input::Key::KP_Add;
	s_Data.KeyCodes[0x053] = Input::Key::KP_Decimal;
	s_Data.KeyCodes[0x135] = Input::Key::KP_Divide;
	s_Data.KeyCodes[0x11C] = Input::Key::KP_Enter;
	s_Data.KeyCodes[0x059] = Input::Key::KP_Equal;
	s_Data.KeyCodes[0x037] = Input::Key::KP_Multiply;
	s_Data.KeyCodes[0x04A] = Input::Key::KP_Subtract;

	for (uint32_t scanCode = 0; scanCode < 512; scanCode++)
	{
		if (static_cast<int32_t>(s_Data.KeyCodes[scanCode]) > 0)
			s_Data.ScanCodes[static_cast<int32_t>(s_Data.KeyCodes[scanCode])] = static_cast<int16_t>(scanCode);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformHideWindowFromTaskbar(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	LONG exStyle = static_cast<LONG>(::GetWindowLongPtrW(window->Handle, GWL_EXSTYLE));
	exStyle &= ~WS_EX_APPWINDOW;
	exStyle |= WS_EX_TOOLWINDOW;
	::SetWindowLongPtrW(window->Handle, GWL_EXSTYLE, exStyle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PlatformSetDragAndDrop(InternalWindow* window, const bool value)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	DragAcceptFiles(window->Handle, value);
}

#endif