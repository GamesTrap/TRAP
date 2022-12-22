/*The MIT License(MIT)

Copyright(c) 2014 - 2020 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/

#include "TRAPPCH.h"
#include "Core/PlatformDetection.h"
#include "ImGuiWindowing.h"
#include "Application.h"
#include "Maths/Math.h"
#include "Utils/Time/TimeStep.h"

std::string TRAP::INTERNAL::ImGuiWindowing::s_clipboardText{};

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::ImGuiWindowing::Init(WindowingAPI::InternalWindow* const window, const bool installCallbacks,
                                                        const TRAP::Graphics::RenderAPI renderAPI)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	if(renderAPI == Graphics::RenderAPI::NONE)
		return true; //Do nothing when not using any RenderAPI

	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

	//Setup back-end capabilities flags
	ImGuiTRAPData* const bd = IM_NEW(ImGuiTRAPData)();
	io.BackendPlatformUserData = bd;
	io.BackendPlatformName = "TRAP";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; //We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; //We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports; //We can create multi-viewports on the Platform side (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; //We can set io.MouseHoveredViewport correctly (optional, not easy)

	bd->Window = window;
	bd->Time = 0.0;
	bd->WantUpdateMonitors = true;

	io.SetClipboardTextFn = SetClipboardText;
	io.GetClipboardTextFn = GetClipboardText;
	io.ClipboardUserData = bd->Window;

	//Create mouse cursors
	bd->MouseCursors[ImGuiMouseCursor_Arrow] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::Arrow);
	bd->MouseCursors[ImGuiMouseCursor_TextInput] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::Input);
	bd->MouseCursors[ImGuiMouseCursor_ResizeNS] = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeVertical
	);
	bd->MouseCursors[ImGuiMouseCursor_ResizeEW] = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeHorizontal
	);
	bd->MouseCursors[ImGuiMouseCursor_Hand] = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::PointingHand
	);
	bd->MouseCursors[ImGuiMouseCursor_ResizeAll] = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeAll
	);
	bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeDiagonalTopLeftBottomRight
	);
	bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeDiagonalTopRightBottomLeft
	);
	bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::NotAllowed
	);

	//Chain WindowingAPI callback: our callback will call the user's previously installed callbacks, if any.
	if(installCallbacks)
		InstallCallbacks(window);

	UpdateMonitors();
	WindowingAPI::SetMonitorCallback(MonitorCallback);

	//Our mouse update function expect PlatformHandle to be filled for the main viewport
	ImGuiViewport* const mainViewport = ImGui::GetMainViewport();
	mainViewport->PlatformHandle = bd->Window;
#ifdef TRAP_PLATFORM_WINDOWS
	mainViewport->PlatformHandleRaw = WindowingAPI::GetWin32Window(bd->Window);
#endif
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		InitPlatformInterface();

	bd->ClientAPI = renderAPI;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
	ImGuiIO& io = ImGui::GetIO();

	ShutdownPlatformInterface();

	if (bd->InstalledCallbacks)
		RestoreCallbacks(bd->Window);

	for (ImGuiMouseCursor cursorN = 0; cursorN < ImGuiMouseCursor_COUNT; cursorN++)
	{
		WindowingAPI::DestroyCursor(std::move(bd->MouseCursors[cursorN]));
		bd->MouseCursors[cursorN] = nullptr;
	}

	io.BackendPlatformName = nullptr;
	io.BackendPlatformUserData = nullptr;
	IM_DELETE(bd);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::NewFrame()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiTRAPData* const bd = GetBackendData();
	IM_ASSERT(bd != NULL && "Did you call ImGuiWindowing::Init()?");

	//Setup display size (every frame to accommodate for window resizing)
	int32_t width = 0, height = 0;
	int32_t displayWidth = 0, displayHeight = 0;
	WindowingAPI::GetWindowSize(bd->Window, width, height);
	WindowingAPI::GetFrameBufferSize(bd->Window, displayWidth, displayHeight);
	io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
	if (width > 0 && height > 0)
		io.DisplayFramebufferScale = ImVec2(static_cast<float>(displayWidth) / static_cast<float>(width),
		                                    static_cast<float>(displayHeight) / static_cast<float>(height));
	if (bd->WantUpdateMonitors)
		UpdateMonitors();

	//Setup time step
	const double currentTime = static_cast<double>(Application::GetTime());
	io.DeltaTime = TRAP::Math::Max(static_cast<float>(currentTime - bd->Time), 1e-4f);
	bd->Time = currentTime;

	UpdateMouseData();
	UpdateMouseCursor();

	//Update game controller (if enabled and available)
	UpdateGamepads();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetCustomCursor(Scope<WindowingAPI::InternalCursor>& cursor)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	bd->CustomCursor = std::move(cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::InstallCallbacks(WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	IM_ASSERT(bd->InstalledCallbacks == false && "Callbacks are already installed!");
	IM_ASSERT(bd->Window == window);

	bd->PrevUserCallbackWindowFocus = WindowingAPI::GetWindowFocusCallback(window);
	WindowingAPI::SetWindowFocusCallback(window, WindowFocusCallback);

	bd->PrevUserCallbackCursorEnter = WindowingAPI::GetCursorEnterCallback(window);
	WindowingAPI::SetCursorEnterCallback(window, CursorEnterCallback);

	bd->PrevUserCallbackCursorPos = WindowingAPI::GetCursorPosCallback(window);
	WindowingAPI::SetCursorPosCallback(window, CursorPosCallback);

	bd->PrevUserCallbackMouseButton = WindowingAPI::GetMouseButtonCallback(window);
	WindowingAPI::SetMouseButtonCallback(window, MouseButtonCallback);

	bd->PrevUserCallbackScroll = WindowingAPI::GetScrollCallback(window);
	WindowingAPI::SetScrollCallback(window, ScrollCallback);

	bd->PrevUserCallbackKey = WindowingAPI::GetKeyCallback(window);
	WindowingAPI::SetKeyCallback(window, KeyCallback);

	bd->PrevUserCallbackChar = WindowingAPI::GetCharCallback(window);
	WindowingAPI::SetCharCallback(window, CharCallback);

	bd->PrevUserCallbackMonitor = WindowingAPI::GetMonitorCallback();
	WindowingAPI::SetMonitorCallback(MonitorCallback);

	bd->InstalledCallbacks = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::RestoreCallbacks(WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	IM_ASSERT(bd->InstalledCallbacks == true && "Callbacks not installed!");
	IM_ASSERT(bd->Window == window);

	WindowingAPI::SetWindowFocusCallback(window, bd->PrevUserCallbackWindowFocus);
	WindowingAPI::SetCursorEnterCallback(window, bd->PrevUserCallbackCursorEnter);
	WindowingAPI::SetCursorPosCallback(window, bd->PrevUserCallbackCursorPos);
	WindowingAPI::SetMouseButtonCallback(window, bd->PrevUserCallbackMouseButton);
	WindowingAPI::SetScrollCallback(window, bd->PrevUserCallbackScroll);
	WindowingAPI::SetKeyCallback(window, bd->PrevUserCallbackKey);
	WindowingAPI::SetCharCallback(window, bd->PrevUserCallbackChar);
	WindowingAPI::SetMonitorCallback(bd->PrevUserCallbackMonitor);
	bd->InstalledCallbacks = false;
	bd->PrevUserCallbackWindowFocus = nullptr;
	bd->PrevUserCallbackCursorEnter = nullptr;
	bd->PrevUserCallbackCursorPos = nullptr;
	bd->PrevUserCallbackMouseButton = nullptr;
	bd->PrevUserCallbackScroll = nullptr;
	bd->PrevUserCallbackKey = nullptr;
	bd->PrevUserCallbackChar = nullptr;
	bd->PrevUserCallbackMonitor = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::WindowFocusCallback(const WindowingAPI::InternalWindow* const window,
													     const bool focused)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackWindowFocus != nullptr && window == bd->Window)
		bd->PrevUserCallbackWindowFocus(window, focused);

	ImGuiIO& io = ImGui::GetIO();
	io.AddFocusEvent(focused);
}

//-------------------------------------------------------------------------------------------------------------------//

//Workaround: X11 seems to send spurious Leave/Enter events which would make us lose our position,
//so we back it up and restore on Leave/Enter (see https://github.com/ocornut/imgui/issues/4984)
void TRAP::INTERNAL::ImGuiWindowing::CursorEnterCallback(const WindowingAPI::InternalWindow* const window,
                                                         const bool entered)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackCursorEnter != nullptr && window == bd->Window)
		bd->PrevUserCallbackCursorEnter(window, entered);

	ImGuiIO& io = ImGui::GetIO();

	if(io.ConfigFlags & ImGuiConfigFlags_NoMouse)
		return;

	if(entered)
	{
		bd->MouseWindow = window;
		io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
	}
	else if(!entered && bd->MouseWindow == window)
	{
		bd->LastValidMousePos = io.MousePos;
		bd->MouseWindow = nullptr;
		io.AddMousePosEvent(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::CursorPosCallback(const WindowingAPI::InternalWindow* const window,
                                                       double xPos, double yPos)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackCursorPos != nullptr && window == bd->Window)
		bd->PrevUserCallbackCursorPos(window, xPos, yPos);

	ImGuiIO& io = ImGui::GetIO();

	if(io.ConfigFlags & ImGuiConfigFlags_NoMouse)
		return;

	if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		int32_t windowX = 0, windowY = 0;
		WindowingAPI::GetWindowPos(window, windowX, windowY);
		xPos += windowX;
		yPos += windowY;
	}
	bd->LastValidMousePos = ImVec2(static_cast<float>(xPos), static_cast<float>(yPos));
	io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::MouseButtonCallback(const WindowingAPI::InternalWindow* const window,
                                                         Input::MouseButton mouseButton, const Input::KeyState state)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackMouseButton != nullptr && window == bd->Window)
		bd->PrevUserCallbackMouseButton(window, mouseButton, state);

	ImGuiIO& io = ImGui::GetIO();
	if(io.ConfigFlags & ImGuiConfigFlags_NoMouse)
		return;

	UpdateKeyModifiers(bd->Window);

	if(static_cast<int32_t>(mouseButton) >= 0 && static_cast<int32_t>(mouseButton) < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(static_cast<int32_t>(mouseButton), (state == Input::KeyState::Pressed));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::ScrollCallback(const WindowingAPI::InternalWindow* const window,
                                                    const double xOffset, const double yOffset)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackScroll != nullptr && window == bd->Window)
		bd->PrevUserCallbackScroll(window, xOffset, yOffset);

	ImGuiIO& io = ImGui::GetIO();
	if(io.ConfigFlags & ImGuiConfigFlags_NoMouse)
		return;

	io.AddMouseWheelEvent(static_cast<float>(xOffset), static_cast<float>(yOffset));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::KeyCallback(const WindowingAPI::InternalWindow* const window, Input::Key key,
                                                 const Input::KeyState state)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackKey != nullptr && window == bd->Window)
		bd->PrevUserCallbackKey(window, key, state);

	if(state != Input::KeyState::Pressed && state != Input::KeyState::Released)
		return;

	UpdateKeyModifiers(bd->Window);

	if(static_cast<int32_t>(key) >= 0 && static_cast<int32_t>(key) < static_cast<int32_t>(bd->KeyOwnerWindows.size()))
		bd->KeyOwnerWindows[static_cast<int32_t>(key)] = (state == Input::KeyState::Pressed) ? window : nullptr;

	key = TranslateUntranslateKey(key);

	ImGuiIO& io = ImGui::GetIO();
	const ImGuiKey imguiKey = KeyToImGuiKey(key);
	io.AddKeyEvent(imguiKey, (state == Input::KeyState::Pressed));

	//We don't support the old API
	//io.SetKeyEventNativeData();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::CharCallback(const WindowingAPI::InternalWindow* const window,
                                                  const uint32_t codePoint)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackChar != nullptr && window == bd->Window)
		bd->PrevUserCallbackChar(window, codePoint);

	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(codePoint);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::MonitorCallback(const WindowingAPI::InternalMonitor* const, const bool)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	ImGuiTRAPData* const bd = GetBackendData();
	bd->WantUpdateMonitors = true;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui context
//It is STRONGLY preferred that you use docking branch with multi-viewports
//(== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
[[nodiscard]] TRAP::INTERNAL::ImGuiWindowing::ImGuiTRAPData* TRAP::INTERNAL::ImGuiWindowing::GetBackendData()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return ImGui::GetCurrentContext() ? static_cast<ImGuiTRAPData*>(ImGui::GetIO().BackendPlatformUserData) : nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const char* TRAP::INTERNAL::ImGuiWindowing::GetClipboardText(void* const /*userData*/)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	s_clipboardText = WindowingAPI::GetClipboardString();
	return s_clipboardText.c_str();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetClipboardText(void* const /*userData*/, const char* const text)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	WindowingAPI::SetClipboardString(text);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImGuiKey TRAP::INTERNAL::ImGuiWindowing::KeyToImGuiKey(const TRAP::Input::Key key) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(key)
	{
	case TRAP::Input::Key::Tab: return ImGuiKey_Tab;
	case TRAP::Input::Key::Left: return ImGuiKey_LeftArrow;
	case TRAP::Input::Key::Right: return ImGuiKey_RightArrow;
	case TRAP::Input::Key::Up: return ImGuiKey_UpArrow;
	case TRAP::Input::Key::Down: return ImGuiKey_DownArrow;
	case TRAP::Input::Key::Page_Up: return ImGuiKey_PageUp;
	case TRAP::Input::Key::Page_Down: return ImGuiKey_PageDown;
	case TRAP::Input::Key::Home: return ImGuiKey_Home;
	case TRAP::Input::Key::End: return ImGuiKey_End;
	case TRAP::Input::Key::Insert: return ImGuiKey_Insert;
	case TRAP::Input::Key::Delete: return ImGuiKey_Delete;
	case TRAP::Input::Key::Backspace: return ImGuiKey_Backspace;
	case TRAP::Input::Key::Space: return ImGuiKey_Space;
	case TRAP::Input::Key::Enter: return ImGuiKey_Enter;
	case TRAP::Input::Key::Escape: return ImGuiKey_Escape;
	case TRAP::Input::Key::Apostrophe: return ImGuiKey_Apostrophe;
	case TRAP::Input::Key::Comma: return ImGuiKey_Comma;
	case TRAP::Input::Key::Minus: return ImGuiKey_Minus;
	case TRAP::Input::Key::Period: return ImGuiKey_Period;
	case TRAP::Input::Key::Slash: return ImGuiKey_Slash;
	case TRAP::Input::Key::Semicolon: return ImGuiKey_Semicolon;
	case TRAP::Input::Key::Equal: return ImGuiKey_Equal;
	case TRAP::Input::Key::Left_Bracket: return ImGuiKey_LeftBracket;
	case TRAP::Input::Key::Backslash: return ImGuiKey_Backslash;
	case TRAP::Input::Key::Right_Bracket: return ImGuiKey_RightBracket;
	case TRAP::Input::Key::Grave_Accent: return ImGuiKey_GraveAccent;
	case TRAP::Input::Key::Caps_Lock: return ImGuiKey_CapsLock;
	case TRAP::Input::Key::Scroll_Lock: return ImGuiKey_ScrollLock;
	case TRAP::Input::Key::Num_Lock: return ImGuiKey_NumLock;
	case TRAP::Input::Key::Print_Screen: return ImGuiKey_PrintScreen;
	case TRAP::Input::Key::Pause: return ImGuiKey_Pause;
	case TRAP::Input::Key::KP_0: return ImGuiKey_Keypad0;
	case TRAP::Input::Key::KP_1: return ImGuiKey_Keypad1;
	case TRAP::Input::Key::KP_2: return ImGuiKey_Keypad2;
	case TRAP::Input::Key::KP_3: return ImGuiKey_Keypad3;
	case TRAP::Input::Key::KP_4: return ImGuiKey_Keypad4;
	case TRAP::Input::Key::KP_5: return ImGuiKey_Keypad5;
	case TRAP::Input::Key::KP_6: return ImGuiKey_Keypad6;
	case TRAP::Input::Key::KP_7: return ImGuiKey_Keypad7;
	case TRAP::Input::Key::KP_8: return ImGuiKey_Keypad8;
	case TRAP::Input::Key::KP_9: return ImGuiKey_Keypad9;
	case TRAP::Input::Key::KP_Decimal: return ImGuiKey_KeypadDecimal;
	case TRAP::Input::Key::KP_Divide: return ImGuiKey_KeypadDivide;
	case TRAP::Input::Key::KP_Multiply: return ImGuiKey_KeypadMultiply;
	case TRAP::Input::Key::KP_Subtract: return ImGuiKey_KeypadSubtract;
	case TRAP::Input::Key::KP_Add: return ImGuiKey_KeypadAdd;
	case TRAP::Input::Key::KP_Enter: return ImGuiKey_KeypadEnter;
	case TRAP::Input::Key::KP_Equal: return ImGuiKey_KeypadEqual;
	case TRAP::Input::Key::Left_Shift: return ImGuiKey_LeftShift;
	case TRAP::Input::Key::Left_Control: return ImGuiKey_LeftCtrl;
	case TRAP::Input::Key::Left_ALT: return ImGuiKey_LeftAlt;
	case TRAP::Input::Key::Left_Super: return ImGuiKey_LeftSuper;
	case TRAP::Input::Key::Right_Shift: return ImGuiKey_RightShift;
	case TRAP::Input::Key::Right_Control: return ImGuiKey_RightCtrl;
	case TRAP::Input::Key::Right_ALT: return ImGuiKey_RightAlt;
	case TRAP::Input::Key::Right_Super: return ImGuiKey_RightSuper;
	case TRAP::Input::Key::Menu: return ImGuiKey_Menu;
	case TRAP::Input::Key::Zero: return ImGuiKey_0;
	case TRAP::Input::Key::One: return ImGuiKey_1;
	case TRAP::Input::Key::Two: return ImGuiKey_2;
	case TRAP::Input::Key::Three: return ImGuiKey_3;
	case TRAP::Input::Key::Four: return ImGuiKey_4;
	case TRAP::Input::Key::Five: return ImGuiKey_5;
	case TRAP::Input::Key::Six: return ImGuiKey_6;
	case TRAP::Input::Key::Seven: return ImGuiKey_7;
	case TRAP::Input::Key::Eight: return ImGuiKey_8;
	case TRAP::Input::Key::Nine: return ImGuiKey_9;
	case TRAP::Input::Key::A: return ImGuiKey_A;
	case TRAP::Input::Key::B: return ImGuiKey_B;
	case TRAP::Input::Key::C: return ImGuiKey_C;
	case TRAP::Input::Key::D: return ImGuiKey_D;
	case TRAP::Input::Key::E: return ImGuiKey_E;
	case TRAP::Input::Key::F: return ImGuiKey_F;
	case TRAP::Input::Key::G: return ImGuiKey_G;
	case TRAP::Input::Key::H: return ImGuiKey_H;
	case TRAP::Input::Key::I: return ImGuiKey_I;
	case TRAP::Input::Key::J: return ImGuiKey_J;
	case TRAP::Input::Key::K: return ImGuiKey_K;
	case TRAP::Input::Key::L: return ImGuiKey_L;
	case TRAP::Input::Key::M: return ImGuiKey_M;
	case TRAP::Input::Key::N: return ImGuiKey_N;
	case TRAP::Input::Key::O: return ImGuiKey_O;
	case TRAP::Input::Key::P: return ImGuiKey_P;
	case TRAP::Input::Key::Q: return ImGuiKey_Q;
	case TRAP::Input::Key::R: return ImGuiKey_R;
	case TRAP::Input::Key::S: return ImGuiKey_S;
	case TRAP::Input::Key::T: return ImGuiKey_T;
	case TRAP::Input::Key::U: return ImGuiKey_U;
	case TRAP::Input::Key::V: return ImGuiKey_V;
	case TRAP::Input::Key::W: return ImGuiKey_W;
	case TRAP::Input::Key::X: return ImGuiKey_X;
	case TRAP::Input::Key::Y: return ImGuiKey_Y;
	case TRAP::Input::Key::Z: return ImGuiKey_Z;
	case TRAP::Input::Key::F1: return ImGuiKey_F1;
	case TRAP::Input::Key::F2: return ImGuiKey_F2;
	case TRAP::Input::Key::F3: return ImGuiKey_F3;
	case TRAP::Input::Key::F4: return ImGuiKey_F4;
	case TRAP::Input::Key::F5: return ImGuiKey_F5;
	case TRAP::Input::Key::F6: return ImGuiKey_F6;
	case TRAP::Input::Key::F7: return ImGuiKey_F7;
	case TRAP::Input::Key::F8: return ImGuiKey_F8;
	case TRAP::Input::Key::F9: return ImGuiKey_F9;
	case TRAP::Input::Key::F10: return ImGuiKey_F10;
	case TRAP::Input::Key::F11: return ImGuiKey_F11;
	case TRAP::Input::Key::F12: return ImGuiKey_F12;

	default:
		return ImGuiKey_None;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateKeyModifiers(const WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(ImGuiKey_ModCtrl, static_cast<bool>(WindowingAPI::GetKey(window, Input::Key::Left_Control)) ||
		                             static_cast<bool>(WindowingAPI::GetKey(window, Input::Key::Right_Control)));
	io.AddKeyEvent(ImGuiKey_ModShift, static_cast<bool>(WindowingAPI::GetKey(window, Input::Key::Left_Shift)) ||
									  static_cast<bool>(WindowingAPI::GetKey(window, Input::Key::Right_Shift)));
	io.AddKeyEvent(ImGuiKey_ModAlt, static_cast<bool>(WindowingAPI::GetKey(window, Input::Key::Left_ALT)) ||
									static_cast<bool>(WindowingAPI::GetKey(window, Input::Key::Right_ALT)));
	io.AddKeyEvent(ImGuiKey_ModSuper, static_cast<bool>(WindowingAPI::GetKey(window, Input::Key::Left_Super)) ||
									  static_cast<bool>(WindowingAPI::GetKey(window, Input::Key::Right_Super)));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::Key TRAP::INTERNAL::ImGuiWindowing::TranslateUntranslateKey(TRAP::Input::Key key)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	if(key >= TRAP::Input::Key::KP_0 && key <= TRAP::Input::Key::KP_Equal)
		return key;
	const std::string keyName = TRAP::Input::GetKeyName(key);
	if(!keyName.empty() && keyName[0] != 0 && keyName[1] == 0)
	{
		static constexpr std::array<char, 11> charNames{'`', '-', '=', '[', ']', '\\', ',', ';', '\'', '.', '/'};
		static constexpr std::array<TRAP::Input::Key, 11> charKeys{TRAP::Input::Key::Grave_Accent, TRAP::Input::Key::Minus,
		                                                           TRAP::Input::Key::Equal, TRAP::Input::Key::Left_Bracket,
																   TRAP::Input::Key::Right_Bracket, TRAP::Input::Key::Backslash,
																   TRAP::Input::Key::Comma, TRAP::Input::Key::Semicolon,
																   TRAP::Input::Key::Apostrophe, TRAP::Input::Key::Period,
																   TRAP::Input::Key::Slash};
		const auto it = std::find(charNames.cbegin(), charNames.cend(), keyName[0]);
		if(keyName[0] >= '0' && keyName[0] <= '9')
			key = static_cast<TRAP::Input::Key>(static_cast<int32_t>(TRAP::Input::Key::Zero) + (keyName[0] - '0'));
		else if(keyName[0] >= 'A' && keyName[0] <= 'Z')
			key = static_cast<TRAP::Input::Key>(static_cast<int32_t>(TRAP::Input::Key::A) + (keyName[0] - 'A'));
		else if(keyName[0] >= 'a' && keyName[0] <= 'z')
			key = static_cast<TRAP::Input::Key>(static_cast<int32_t>(TRAP::Input::Key::A) + (keyName[0] - 'a'));
		else if(it != charNames.cend())
			key = static_cast<TRAP::Input::Key>(charKeys[it - charNames.cbegin()]);
	}

	return key;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateMouseData()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	ImGuiIO& io = ImGui::GetIO();
	const ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

	if(io.ConfigFlags & ImGuiConfigFlags_NoMouse)
		return;

	ImGuiID mouseViewportID = 0;
	const ImVec2 mousePosPrev = io.MousePos;
	for(const ImGuiViewport* const viewport : platformIO.Viewports)
	{
		WindowingAPI::InternalWindow* const window = static_cast<WindowingAPI::InternalWindow*>(viewport->PlatformHandle);

		const bool isWindowFocused = WindowingAPI::GetWindowHint(window, WindowingAPI::Hint::Focused) != false;

		if(isWindowFocused)
		{
			//(Optional) Set OS mouse position from Dear ImGui if requested
			//(rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by used)
			//When multi-viewports are enabled, all Dear ImGui positions are same as OS positions.
			if(io.WantSetMousePos)
				WindowingAPI::SetCursorPos(window, static_cast<double>(mousePosPrev.x - viewport->Pos.x),
				                                   static_cast<double>(mousePosPrev.y - viewport->Pos.y));

			//(Optional) Fallback to provide mouse position when focused (CursorPosCallback already provides this when hovered or captured)
			if(bd->MouseWindow == nullptr)
			{
				double mouseX = 0.0, mouseY = 0.0;
				WindowingAPI::GetCursorPos(window, mouseX, mouseY);
				if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					//Single-viewport mode: mouse position in client window coordinates
					//(io.MousePos is (0, 0) when the mouse is on the upper-left corner of the app window)
					//Multi-viewport mode: mouse position in OS absolute coordinates
					//(io.MousePos is (0, 0) when the mouse is on the upper-left of the primary monitor)
					int32_t windowX = 0, windowY = 0;
					WindowingAPI::GetWindowPos(window, windowX, windowY);
					mouseX += windowX;
					mouseY += windowY;
				}

				bd->LastValidMousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
				io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
			}
		}

		//(Optional) When using multiple viewports: call io.AddMouseViewportEvent() with the viewport
		//the OS mouse cursor is hovering.
		//If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, Dear ImGui will ignore this field
		//and infer the information using its flawed heuristic.
		const bool windowNoInput = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;
		WindowingAPI::SetWindowHint(window, WindowingAPI::Hint::MousePassthrough, windowNoInput);
		if(WindowingAPI::GetWindowHint(window, WindowingAPI::Hint::Hovered) && !windowNoInput)
			mouseViewportID = viewport->ID;
	}

	if(io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)
		io.AddMouseViewportEvent(mouseViewportID);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateMouseCursor()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiIO& io = ImGui::GetIO();
	const ImGuiTRAPData* const bd = GetBackendData();
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) ||
	    (WindowingAPI::GetCursorMode(bd->Window) == WindowingAPI::CursorMode::Disabled ||
		 WindowingAPI::GetCursorMode(bd->Window) == WindowingAPI::CursorMode::Hidden))
		return;

	const ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	const ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	for(int32_t n = 0; n < platformIO.Viewports.Size; n++)
	{
		WindowingAPI::InternalWindow* const windowPtr = static_cast<WindowingAPI::InternalWindow*>
		(
			platformIO.Viewports[n]->PlatformHandle
		);

		if(imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			//Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			WindowingAPI::SetCursorMode(windowPtr, WindowingAPI::CursorMode::Hidden);
		}
		else
		{
			if(imguiCursor != ImGuiMouseCursor_Arrow)
			{
				//Show OS mouse cursor
				WindowingAPI::SetCursor(windowPtr, bd->MouseCursors[imguiCursor] ?
												           bd->MouseCursors[imguiCursor].get() :
												           bd->MouseCursors[ImGuiMouseCursor_Arrow].get());
				WindowingAPI::SetCursorMode(windowPtr, WindowingAPI::CursorMode::Normal);
			}
			else
			{
				if(bd->CustomCursor)
					WindowingAPI::SetCursor(windowPtr, bd->CustomCursor.get());
				else
					WindowingAPI::SetCursor(windowPtr, bd->MouseCursors[ImGuiMouseCursor_Arrow].get());
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline constexpr float Saturate(const float v) noexcept
{
	return v < 0.0f ? 0.0f : v > 1.0f ? 1.0f : v;
}

void TRAP::INTERNAL::ImGuiWindowing::UpdateGamepads()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiIO& io = ImGui::GetIO();
	if((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
		return;

	io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
	if(!TRAP::Input::IsControllerConnected(TRAP::Input::Controller::One))
		return;

	#define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED) { io.AddKeyEvent(KEY_NO, TRAP::Input::IsControllerButtonPressed(TRAP::Input::Controller::One, BUTTON_NO)); }
	#define MAP_DPAD(KEY_NO, DPAD_NO, _UNUSED) { io.AddKeyEvent(KEY_NO, static_cast<bool>(TRAP::Input::GetControllerDPad(TRAP::Input::Controller::One, 0) & DPAD_NO)); }
	#define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1) { float v = TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, AXIS_NO); v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); }

	io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
	MAP_BUTTON(ImGuiKey_GamepadStart, TRAP::Input::ControllerButton::Start, 7);
	MAP_BUTTON(ImGuiKey_GamepadBack, TRAP::Input::ControllerButton::Back, 6);
	MAP_BUTTON(ImGuiKey_GamepadFaceDown, TRAP::Input::ControllerButton::A, 0); //XBox A, PS Cross
	MAP_BUTTON(ImGuiKey_GamepadFaceRight, TRAP::Input::ControllerButton::B, 1); //XBox B, PS Circle
	MAP_BUTTON(ImGuiKey_GamepadFaceLeft, TRAP::Input::ControllerButton::X, 2); //XBox X, PS Square
	MAP_BUTTON(ImGuiKey_GamepadFaceUp, TRAP::Input::ControllerButton::Y, 3); //XBox Y, PS Triangle
	MAP_DPAD(ImGuiKey_GamepadDpadLeft, TRAP::Input::ControllerDPad::Left, 13);
	MAP_DPAD(ImGuiKey_GamepadDpadRight, TRAP::Input::ControllerDPad::Right, 11);
	MAP_DPAD(ImGuiKey_GamepadDpadUp, TRAP::Input::ControllerDPad::Up, 10);
	MAP_DPAD(ImGuiKey_GamepadDpadDown, TRAP::Input::ControllerDPad::Down, 12);
	MAP_BUTTON(ImGuiKey_GamepadL1, TRAP::Input::ControllerButton::Left_Bumper, 4);
	MAP_BUTTON(ImGuiKey_GamepadR1, TRAP::Input::ControllerButton::Right_Bumper, 5);
	MAP_ANALOG(ImGuiKey_GamepadL2, TRAP::Input::ControllerAxis::Left_Trigger, 4, -0.75f, +1.0f);
	MAP_ANALOG(ImGuiKey_GamepadR2, TRAP::Input::ControllerAxis::Right_Trigger, 5, -0.75f, +1.0f);
	MAP_BUTTON(ImGuiKey_GamepadL3, TRAP::Input::ControllerButton::Left_Thumb, 8);
	MAP_BUTTON(ImGuiKey_GamepadR3, TRAP::Input::ControllerButton::Right_Thumb, 9);
	MAP_ANALOG(ImGuiKey_GamepadLStickLeft, TRAP::Input::ControllerAxis::Left_X, 0, -0.25f, -1.0f);
	MAP_ANALOG(ImGuiKey_GamepadLStickRight, TRAP::Input::ControllerAxis::Left_X, 0, +0.25f, +1.0f);
	MAP_ANALOG(ImGuiKey_GamepadLStickUp, TRAP::Input::ControllerAxis::Left_Y, 1, -0.25f, -1.0f);
	MAP_ANALOG(ImGuiKey_GamepadLStickDown, TRAP::Input::ControllerAxis::Left_Y, 1, +0.25f, +1.0f);
	MAP_ANALOG(ImGuiKey_GamepadRStickLeft, TRAP::Input::ControllerAxis::Right_X, 2, -0.25f, -1.0f);
	MAP_ANALOG(ImGuiKey_GamepadRStickRight, TRAP::Input::ControllerAxis::Right_X, 2, +0.25f, +1.0f);
	MAP_ANALOG(ImGuiKey_GamepadRStickUp, TRAP::Input::ControllerAxis::Right_Y, 3, -0.25f, -1.0f);
	MAP_ANALOG(ImGuiKey_GamepadRStickDown, TRAP::Input::ControllerAxis::Right_Y, 3, +0.25f, +1.0f);

	#undef MAP_BUTTON
	#undef MAP_DPAD
	#undef MAP_ANALOG
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateMonitors()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	const auto& monitors = WindowingAPI::GetMonitors();
	platformIO.Monitors.resize(0);
	for (const auto& n : monitors)
	{
		ImGuiPlatformMonitor monitor;
		int32_t x = 0, y = 0;
		WindowingAPI::GetMonitorPos(n, x, y);
		const WindowingAPI::InternalVideoMode videoMode = WindowingAPI::GetVideoMode(n);

		monitor.MainPos = ImVec2(static_cast<float>(x), static_cast<float>(y));
		monitor.MainSize = ImVec2(static_cast<float>(videoMode.Width), static_cast<float>(videoMode.Height));
		int32_t width = 0, height = 0;
		WindowingAPI::GetMonitorWorkArea(n, x, y, width, height);
		monitor.WorkPos = ImVec2(static_cast<float>(x), static_cast<float>(y));
		monitor.WorkSize = ImVec2(static_cast<float>(width), static_cast<float>(height));

		//Warning: The validity of monitor DPI information on Windows depends on the application
		//DPI awareness settings,
		//which generally needs to be set in the manifest or at runtime.
		float xScale = 0.0f, yScale = 0.0f;
		WindowingAPI::GetMonitorContentScale(n, xScale, yScale);
		monitor.DpiScale = xScale;
		platformIO.Monitors.push_back(monitor);
	}
	bd->WantUpdateMonitors = false;
}

//-------------------------------------------------------------------------------------------------------------------//
//MULTI-VIEWPORT | PLATFORM INTERFACE SUPPORT
//This is an _advanced_ and _optional_ feature,
//allowing the back-end to create and handle multiple viewports simultaneously.
//If you are new to dear imgui or creating a new binding for dear imgui,
//it is recommended that you completely ignore this section first...
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::WindowCloseCallback(const WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
	if (ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
	(
		const_cast<WindowingAPI::InternalWindow*>(window))
	)
		viewport->PlatformRequestClose = true;
}

//-------------------------------------------------------------------------------------------------------------------//

//WindowingAPI may dispatch window pos/size events after calling SetWindowPos()/SetWindowSize().
//However: depending on the platform the callback may be invoked at different time:
// - on Windows it appears to be called within the SetWindowPos()/SetWindowSize() call
// - on Linux it is queued and invoked during PollEvents()
//Because the event doesn't always fire on SetWindowXXX() we use a frame counter tag to only
//ignore recent SetWindowXXX() calls.
void TRAP::INTERNAL::ImGuiWindowing::WindowPosCallback(const WindowingAPI::InternalWindow* const window,
                                                       const int32_t, const int32_t)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
	if (ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
	(
		const_cast<WindowingAPI::InternalWindow*>(window))
	)
	{
		if(const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData))
		{
			const bool ignoreEvent = (ImGui::GetFrameCount() <= vd->IgnoreWindowPosEventFrame + 1);
			// vd->IgnoreWindowPosEventFrame = -1;
			if(ignoreEvent)
				return;
		}

		viewport->PlatformRequestMove = true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::WindowSizeCallback(const WindowingAPI::InternalWindow* const window,
                                                        const int32_t, const int32_t)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
	if(ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
	(
		const_cast<WindowingAPI::InternalWindow*>(window)
	))
	{
		if(const ImGuiViewportDataTRAP* const data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData))
		{
			const bool ignoreEvent = (ImGui::GetFrameCount() <= data->IgnoreWindowSizeEventFrame + 1);
			// data->IgnoreWindowSizeEventFrame = -1;
			if (ignoreEvent)
				return;
		}
		viewport->PlatformRequestResize = true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::CreateWindow(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiViewportDataTRAP* const vd = IM_NEW(ImGuiViewportDataTRAP)();
	viewport->PlatformUserData = vd;

	WindowingAPI::WindowHint(WindowingAPI::Hint::Visible, false);
	WindowingAPI::WindowHint(WindowingAPI::Hint::Focused, false);
	WindowingAPI::WindowHint(WindowingAPI::Hint::FocusOnShow, false);
	WindowingAPI::WindowHint(WindowingAPI::Hint::Decorated, (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? false : true);
	WindowingAPI::WindowHint(WindowingAPI::Hint::Floating, (viewport->Flags & ImGuiViewportFlags_TopMost) ? true : false);
	vd->Window = WindowingAPI::CreateWindow(static_cast<int32_t>(viewport->Size.x),
	                                        static_cast<int32_t>(viewport->Size.y), "No Title Yet", nullptr);
	vd->WindowPtr = vd->Window.get();
	vd->WindowOwned = true;
	viewport->PlatformHandle = static_cast<void*>(vd->WindowPtr);
#ifdef TRAP_PLATFORM_WINDOWS
	viewport->PlatformHandleRaw = WindowingAPI::GetWin32Window(vd->WindowPtr);
#endif
	WindowingAPI::SetWindowPos(vd->WindowPtr, static_cast<int32_t>(viewport->Pos.x),
	                           static_cast<int32_t>(viewport->Pos.y));

	//Install callbacks for secondary viewports
	WindowingAPI::SetWindowFocusCallback(vd->WindowPtr, WindowFocusCallback);
	WindowingAPI::SetCursorEnterCallback(vd->WindowPtr, CursorEnterCallback);
	WindowingAPI::SetCursorPosCallback(vd->WindowPtr, CursorPosCallback);
	WindowingAPI::SetMouseButtonCallback(vd->WindowPtr, MouseButtonCallback);
	WindowingAPI::SetScrollCallback(vd->WindowPtr, ScrollCallback);
	WindowingAPI::SetKeyCallback(vd->WindowPtr, KeyCallback);
	WindowingAPI::SetCharCallback(vd->WindowPtr, CharCallback);
	WindowingAPI::SetWindowCloseCallback(vd->WindowPtr, WindowCloseCallback);
	WindowingAPI::SetWindowPosCallback(vd->WindowPtr, WindowPosCallback);
	WindowingAPI::SetWindowSizeCallback(vd->WindowPtr, WindowSizeCallback);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::DestroyWindow(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if(ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData))
	{
		if(vd->WindowOwned)
		{
			//Release any keys that were pressed in the window being destroyed and are still held down,
			//because we will not receive any release events after window is destroyed.
			for(std::size_t i = 0; i < bd->KeyOwnerWindows.size(); ++i)
			{
				if(bd->KeyOwnerWindows[i] == vd->WindowPtr)
					KeyCallback(vd->WindowPtr, static_cast<TRAP::Input::Key>(i), Input::KeyState::Released); //Later params are only used for main viewport, on which this function is never called.
			}

			WindowingAPI::DestroyWindow(std::move(vd->Window));
		}
		vd->Window = nullptr;
		IM_DELETE(vd);
	}
	viewport->PlatformUserData = nullptr;
	viewport->PlatformHandle = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::ShowWindow(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	if(viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon)
		WindowingAPI::HideWindowFromTaskbar(static_cast<WindowingAPI::InternalWindow*>(viewport->PlatformHandle));

	WindowingAPI::ShowWindow(vd->WindowPtr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImVec2 TRAP::INTERNAL::ImGuiWindowing::GetWindowPos(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	int32_t x = 0, y = 0;
	WindowingAPI::GetWindowPos(vd->WindowPtr, x, y);

	return ImVec2(static_cast<float>(x), static_cast<float>(y));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowPos(ImGuiViewport* const viewport, const ImVec2 pos)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	vd->IgnoreWindowPosEventFrame = ImGui::GetFrameCount();
	WindowingAPI::SetWindowPos(vd->WindowPtr, static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImVec2 TRAP::INTERNAL::ImGuiWindowing::GetWindowSize(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	int32_t width = 0, height = 0;
	WindowingAPI::GetWindowSize(vd->WindowPtr, width, height);

	return ImVec2(static_cast<float>(width), static_cast<float>(height));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowSize(ImGuiViewport* const viewport, const ImVec2 size)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	vd->IgnoreWindowSizeEventFrame = ImGui::GetFrameCount();

	WindowingAPI::SetWindowSize(vd->WindowPtr, static_cast<int32_t>(size.x), static_cast<int32_t>(size.y));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowTitle(ImGuiViewport* const viewport, const char* const title)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::SetWindowTitle(vd->WindowPtr, title);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowFocus(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::FocusWindow(vd->WindowPtr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::ImGuiWindowing::GetWindowFocus(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	return WindowingAPI::GetWindowHint(vd->WindowPtr, WindowingAPI::Hint::Focused);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::ImGuiWindowing::GetWindowMinimized(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	return WindowingAPI::GetWindowHint(vd->WindowPtr, WindowingAPI::Hint::Minimized);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowAlpha(ImGuiViewport* const viewport, const float alpha)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::SetWindowOpacity(vd->WindowPtr, alpha);
}

//--------------------------------------------------------------------------------------------------------
// Vulkan support (the Vulkan renderer needs to call a platform-side support function to create the surface)
//--------------------------------------------------------------------------------------------------------

[[nodiscard]] int32_t TRAP::INTERNAL::ImGuiWindowing::CreateVkSurface(ImGuiViewport* const viewport, const ImU64 vkInstance,
                                                                      const void* vkAllocator, ImU64* const outVkSurface)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	IM_UNUSED(bd);
	IM_ASSERT(bd->ClientAPI == TRAP::Graphics::RenderAPI::Vulkan);
	const VkResult err = WindowingAPI::CreateWindowSurface(Utils::BitCast<const ImU64, const VkInstance>(vkInstance),
	                                                       vd->WindowPtr,
	                                                       static_cast<const VkAllocationCallbacks*>(vkAllocator),
														   *reinterpret_cast<VkSurfaceKHR*>(outVkSurface));

	return static_cast<int32_t>(err);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::InitPlatformInterface()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//Register platform interface (will be coupled with a renderer interface)
	const ImGuiTRAPData* const bd = GetBackendData();
	ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	platformIO.Platform_CreateWindow = CreateWindow;
	platformIO.Platform_DestroyWindow = DestroyWindow;
	platformIO.Platform_ShowWindow = ShowWindow;
	platformIO.Platform_SetWindowPos = SetWindowPos;
	platformIO.Platform_GetWindowPos = GetWindowPos;
	platformIO.Platform_SetWindowSize = SetWindowSize;
	platformIO.Platform_GetWindowSize = GetWindowSize;
	platformIO.Platform_SetWindowFocus = SetWindowFocus;
	platformIO.Platform_GetWindowFocus = GetWindowFocus;
	platformIO.Platform_GetWindowMinimized = GetWindowMinimized;
	platformIO.Platform_SetWindowTitle = SetWindowTitle;
	platformIO.Platform_RenderWindow = nullptr; //Unused because we don't use OpenGL.
	platformIO.Platform_SwapBuffers = nullptr; //Unused because we don't use OpenGL.
	platformIO.Platform_SetWindowAlpha = SetWindowAlpha;
	platformIO.Platform_CreateVkSurface = CreateVkSurface;

	//Register main window handle (which is owned by the main application, not by us)
	//This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use
	//same logic for main and secondary viewports.
	ImGuiViewport* const main_viewport = ImGui::GetMainViewport();
	ImGuiViewportDataTRAP* const vd = IM_NEW(ImGuiViewportDataTRAP)();
	vd->Window = nullptr;
	vd->WindowPtr = bd->Window;
	vd->WindowOwned = false;
	main_viewport->PlatformUserData = vd;
	main_viewport->PlatformHandle = bd->Window;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::ShutdownPlatformInterface()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGui::DestroyPlatformWindows();
}
