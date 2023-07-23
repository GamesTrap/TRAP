/*The MIT License(MIT)

Copyright(c) 2014 - 2023 Omar Cornut

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

#ifndef TRAP_HEADLESS_MODE

#include "Application.h"
#include "Maths/Math.h"
#include "Utils/Time/TimeStep.h"

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
	std::get<ImGuiMouseCursor_Arrow>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::Arrow);
	std::get<ImGuiMouseCursor_TextInput>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::Input);
	std::get<ImGuiMouseCursor_ResizeNS>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeVertical
	);
	std::get<ImGuiMouseCursor_ResizeEW>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeHorizontal
	);
	std::get<ImGuiMouseCursor_Hand>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::PointingHand
	);
	std::get<ImGuiMouseCursor_ResizeAll>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeAll
	);
	std::get<ImGuiMouseCursor_ResizeNESW>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeDiagonalTopLeftBottomRight
	);
	std::get<ImGuiMouseCursor_ResizeNWSE>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeDiagonalTopRightBottomLeft
	);
	std::get<ImGuiMouseCursor_NotAllowed>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::NotAllowed
	);

	//Chain WindowingAPI callback: our callback will call the user's previously installed callbacks, if any.
	if(installCallbacks)
		InstallCallbacks(window);

	UpdateMonitors();
	WindowingAPI::SetMonitorCallback(MonitorCallback);

	//Set platform dependent data in viewport
	ImGuiViewport* const mainViewport = ImGui::GetMainViewport();
	mainViewport->PlatformHandle = bd->Window;
#ifdef TRAP_PLATFORM_WINDOWS
	mainViewport->PlatformHandleRaw = WindowingAPI::GetWin32Window(*bd->Window);
#endif /*TRAP_PLATFORM_WINDOWS*/
	if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
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
		WindowingAPI::DestroyCursor(bd->MouseCursors[NumericCast<std::size_t>(cursorN)]);
		bd->MouseCursors[NumericCast<std::size_t>(cursorN)] = nullptr;
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
	IM_ASSERT(bd != nullptr && "Did you call ImGuiWindowing::Init()?");

	//Setup display size (every frame to accommodate for window resizing)
	int32_t width = 0, height = 0;
	int32_t displayWidth = 0, displayHeight = 0;
	WindowingAPI::GetWindowSize(*bd->Window, width, height);
	WindowingAPI::GetFrameBufferSize(*bd->Window, displayWidth, displayHeight);
	io.DisplaySize = ImVec2(NumericCast<float>(width), NumericCast<float>(height));
	if (width > 0 && height > 0)
		io.DisplayFramebufferScale = ImVec2(NumericCast<float>(displayWidth) / io.DisplaySize.x,
		                                    NumericCast<float>(displayHeight) / io.DisplaySize.y);
	if (bd->WantUpdateMonitors)
		UpdateMonitors();

	//Setup time step
	double currentTime = static_cast<double>(Application::GetTime());
	if(currentTime <= bd->Time)
		currentTime = bd->Time + 0.00001f;
	io.DeltaTime = bd->Time > 0.0 ? NumericCast<float>(currentTime - bd->Time) : (1.0f / 60.0f);
	bd->Time = currentTime;

	UpdateMouseData();
	UpdateMouseCursor();

	//Update game controller (if enabled and available)
	UpdateGamepads();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetCustomCursor(WindowingAPI::InternalCursor* cursor)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	bd->CustomCursor = cursor;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetCallbacksChainForAllWindows(const bool chainForAllWindows)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	bd->CallbacksChainForAllWindows = chainForAllWindows;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::InstallCallbacks(WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	IM_ASSERT(bd->InstalledCallbacks == false && "Callbacks are already installed!");
	IM_ASSERT(bd->Window == window);

	bd->PrevUserCallbackWindowFocus = WindowingAPI::GetWindowFocusCallback(*window);
	WindowingAPI::SetWindowFocusCallback(*window, WindowFocusCallback);

	bd->PrevUserCallbackCursorEnter = WindowingAPI::GetCursorEnterCallback(*window);
	WindowingAPI::SetCursorEnterCallback(*window, CursorEnterCallback);

	bd->PrevUserCallbackCursorPos = WindowingAPI::GetCursorPosCallback(*window);
	WindowingAPI::SetCursorPosCallback(*window, CursorPosCallback);

	bd->PrevUserCallbackMouseButton = WindowingAPI::GetMouseButtonCallback(*window);
	WindowingAPI::SetMouseButtonCallback(*window, MouseButtonCallback);

	bd->PrevUserCallbackScroll = WindowingAPI::GetScrollCallback(*window);
	WindowingAPI::SetScrollCallback(*window, ScrollCallback);

	bd->PrevUserCallbackKey = WindowingAPI::GetKeyCallback(*window);
	WindowingAPI::SetKeyCallback(*window, KeyCallback);

	bd->PrevUserCallbackChar = WindowingAPI::GetCharCallback(*window);
	WindowingAPI::SetCharCallback(*window, CharCallback);

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

	WindowingAPI::SetWindowFocusCallback(*window, bd->PrevUserCallbackWindowFocus);
	WindowingAPI::SetCursorEnterCallback(*window, bd->PrevUserCallbackCursorEnter);
	WindowingAPI::SetCursorPosCallback(*window, bd->PrevUserCallbackCursorPos);
	WindowingAPI::SetMouseButtonCallback(*window, bd->PrevUserCallbackMouseButton);
	WindowingAPI::SetScrollCallback(*window, bd->PrevUserCallbackScroll);
	WindowingAPI::SetKeyCallback(*window, bd->PrevUserCallbackKey);
	WindowingAPI::SetCharCallback(*window, bd->PrevUserCallbackChar);
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

bool TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(const WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	return bd->CallbacksChainForAllWindows ? true : (window == bd->Window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::WindowFocusCallback(const WindowingAPI::InternalWindow& window,
													     const bool focused)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackWindowFocus != nullptr && ShouldChainCallback(&window))
		bd->PrevUserCallbackWindowFocus(window, focused);

	ImGuiIO& io = ImGui::GetIO();
	io.AddFocusEvent(focused);
}

//-------------------------------------------------------------------------------------------------------------------//

//Workaround: X11 seems to send spurious Leave/Enter events which would make us lose our position,
//so we back it up and restore on Leave/Enter (see https://github.com/ocornut/imgui/issues/4984)
void TRAP::INTERNAL::ImGuiWindowing::CursorEnterCallback(const WindowingAPI::InternalWindow& window,
                                                         const bool entered)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackCursorEnter != nullptr && ShouldChainCallback(&window))
		bd->PrevUserCallbackCursorEnter(window, entered);

	ImGuiIO& io = ImGui::GetIO();
	if(entered)
	{
		bd->MouseWindow = &window;
		io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
	}
	else if(!entered && bd->MouseWindow == &window)
	{
		bd->LastValidMousePos = io.MousePos;
		bd->MouseWindow = nullptr;
		io.AddMousePosEvent(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::CursorPosCallback(const WindowingAPI::InternalWindow& window,
                                                       double xPos, double yPos)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackCursorPos != nullptr && ShouldChainCallback(&window))
		bd->PrevUserCallbackCursorPos(window, xPos, yPos);

	ImGuiIO& io = ImGui::GetIO();
	if((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
	{
		int32_t windowX = 0, windowY = 0;
		WindowingAPI::GetWindowPos(window, windowX, windowY);
		xPos += windowX;
		yPos += windowY;
	}
	io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
	bd->LastValidMousePos = ImVec2(NumericCast<float>(xPos), NumericCast<float>(yPos));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::MouseButtonCallback(const WindowingAPI::InternalWindow& window,
                                                         Input::MouseButton mouseButton, const Input::KeyState state)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackMouseButton != nullptr && ShouldChainCallback(&window))
		bd->PrevUserCallbackMouseButton(window, mouseButton, state);

	UpdateKeyModifiers(bd->Window);

	ImGuiIO& io = ImGui::GetIO();
	if(std::to_underlying(mouseButton) < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(NumericCast<int32_t>(std::to_underlying(mouseButton)), (state == Input::KeyState::Pressed));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::ScrollCallback(const WindowingAPI::InternalWindow& window,
                                                    const double xOffset, const double yOffset)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackScroll != nullptr && ShouldChainCallback(&window))
		bd->PrevUserCallbackScroll(window, xOffset, yOffset);

	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent(NumericCast<float>(xOffset), NumericCast<float>(yOffset));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::KeyCallback(const WindowingAPI::InternalWindow& window, Input::Key key,
                                                 const Input::KeyState state)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackKey != nullptr && ShouldChainCallback(&window))
		bd->PrevUserCallbackKey(window, key, state);

	if(state != Input::KeyState::Pressed && state != Input::KeyState::Released)
		return;

	UpdateKeyModifiers(bd->Window);

	if(std::to_underlying(key) >= 0 && std::to_underlying(key) < NumericCast<int32_t>(bd->KeyOwnerWindows.size()))
		bd->KeyOwnerWindows[NumericCast<std::size_t>(std::to_underlying(key))] = (state == Input::KeyState::Pressed) ? &window : nullptr;

	key = TranslateUntranslateKey(key);

	ImGuiIO& io = ImGui::GetIO();
	const ImGuiKey imguiKey = KeyToImGuiKey(key);
	io.AddKeyEvent(imguiKey, (state == Input::KeyState::Pressed));
	//io.SetKeyEventNativeData(); //We don't support the old API (< 1.87 user code)
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::CharCallback(const WindowingAPI::InternalWindow& window,
                                                  const uint32_t codePoint)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiTRAPData* const bd = GetBackendData();
	if (bd->PrevUserCallbackChar != nullptr && ShouldChainCallback(&window))
		bd->PrevUserCallbackChar(window, codePoint);

	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(codePoint);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::MonitorCallback([[maybe_unused]] const WindowingAPI::InternalMonitor& monitor,
                                                     [[maybe_unused]] const bool connected)
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

	return ImGui::GetCurrentContext() != nullptr ? static_cast<ImGuiTRAPData*>(ImGui::GetIO().BackendPlatformUserData) : nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const char* TRAP::INTERNAL::ImGuiWindowing::GetClipboardText([[maybe_unused]] void* const userData)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	s_clipboardText = WindowingAPI::GetClipboardString();
	return s_clipboardText.c_str();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetClipboardText([[maybe_unused]] void* const userData, const char* const text)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	WindowingAPI::SetClipboardString(text);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateKeyModifiers(const WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(ImGuiMod_Ctrl, WindowingAPI::GetKey(*window, Input::Key::Left_Control) != Input::KeyState::Released ||
		                             WindowingAPI::GetKey(*window, Input::Key::Right_Control) != Input::KeyState::Released);
	io.AddKeyEvent(ImGuiMod_Shift, WindowingAPI::GetKey(*window, Input::Key::Left_Shift) != Input::KeyState::Released ||
									  WindowingAPI::GetKey(*window, Input::Key::Right_Shift) != Input::KeyState::Released);
	io.AddKeyEvent(ImGuiMod_Alt, WindowingAPI::GetKey(*window, Input::Key::Left_ALT) != Input::KeyState::Released ||
									WindowingAPI::GetKey(*window, Input::Key::Right_ALT) != Input::KeyState::Released);
	io.AddKeyEvent(ImGuiMod_Super, WindowingAPI::GetKey(*window, Input::Key::Left_Super) != Input::KeyState::Released ||
									  WindowingAPI::GetKey(*window, Input::Key::Right_Super) != Input::KeyState::Released);
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
		const std::array<char, 11>::const_iterator it = std::ranges::find(charNames, keyName[0]);
		if(keyName[0] >= '0' && keyName[0] <= '9')
			key = static_cast<TRAP::Input::Key>(std::to_underlying(TRAP::Input::Key::Zero) + (keyName[0] - '0'));
		else if(keyName[0] >= 'A' && keyName[0] <= 'Z')
			key = static_cast<TRAP::Input::Key>(std::to_underlying(TRAP::Input::Key::A) + (keyName[0] - 'A'));
		else if(keyName[0] >= 'a' && keyName[0] <= 'z')
			key = static_cast<TRAP::Input::Key>(std::to_underlying(TRAP::Input::Key::A) + (keyName[0] - 'a'));
		else if(it != charNames.cend())
			key = charKeys[NumericCast<std::size_t>(it - charNames.cbegin())];
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

	ImGuiID mouseViewportID = 0;
	const ImVec2 mousePosPrev = io.MousePos;
	for(const ImGuiViewport* const viewport : platformIO.Viewports)
	{
		WindowingAPI::InternalWindow* const window = static_cast<WindowingAPI::InternalWindow*>(viewport->PlatformHandle);

		const bool isWindowFocused = WindowingAPI::GetWindowHint(*window, WindowingAPI::Hint::Focused);

		if(isWindowFocused)
		{
			//(Optional) Set OS mouse position from Dear ImGui if requested
			//(rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by used)
			//When multi-viewports are enabled, all Dear ImGui positions are same as OS positions.
			if(io.WantSetMousePos)
				WindowingAPI::SetCursorPos(*window, NumericCast<double>(mousePosPrev.x - viewport->Pos.x),
				                                    NumericCast<double>(mousePosPrev.y - viewport->Pos.y));

			//(Optional) Fallback to provide mouse position when focused (CursorPosCallback already provides this when hovered or captured)
			if(bd->MouseWindow == nullptr)
			{
				double mouseX = 0.0, mouseY = 0.0;
				WindowingAPI::GetCursorPos(*window, mouseX, mouseY);
				if((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
				{
					//Single-viewport mode: mouse position in client window coordinates
					//(io.MousePos is (0, 0) when the mouse is on the upper-left corner of the app window)
					//Multi-viewport mode: mouse position in OS absolute coordinates
					//(io.MousePos is (0, 0) when the mouse is on the upper-left of the primary monitor)
					int32_t windowX = 0, windowY = 0;
					WindowingAPI::GetWindowPos(*window, windowX, windowY);
					mouseX += windowX;
					mouseY += windowY;
				}

				bd->LastValidMousePos = ImVec2(NumericCast<float>(mouseX), NumericCast<float>(mouseY));
				io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
			}
		}

		//(Optional) When using multiple viewports: call io.AddMouseViewportEvent() with the viewport
		//the OS mouse cursor is hovering.
		//If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, Dear ImGui will ignore this field
		//and infer the information using its flawed heuristic.
		const bool windowNoInput = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;
		WindowingAPI::SetWindowHint(*window, WindowingAPI::Hint::MousePassthrough, windowNoInput);
		if(WindowingAPI::GetWindowHint(*window, WindowingAPI::Hint::Hovered) && !windowNoInput)
			mouseViewportID = viewport->ID;
	}

	if((io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport) != 0)
		io.AddMouseViewportEvent(mouseViewportID);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateMouseCursor()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiIO& io = ImGui::GetIO();
	const ImGuiTRAPData* const bd = GetBackendData();
	if (((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) != 0) ||
	    WindowingAPI::GetCursorMode(*bd->Window) == WindowingAPI::CursorMode::Disabled)
		return;

	const ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	const ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	for(const ImGuiViewport* viewport : platformIO.Viewports)
	{
		WindowingAPI::InternalWindow* const windowPtr = static_cast<WindowingAPI::InternalWindow*>
		(
			viewport->PlatformHandle
		);

		if(imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			//Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			WindowingAPI::SetCursorMode(*windowPtr, WindowingAPI::CursorMode::Hidden);
		}
		else
		{
			if(imguiCursor != ImGuiMouseCursor_Arrow)
			{
				//Show OS mouse cursor
				WindowingAPI::SetCursor(*windowPtr, bd->MouseCursors[NumericCast<std::size_t>(imguiCursor)] != nullptr ?
												    bd->MouseCursors[NumericCast<std::size_t>(imguiCursor)] :
												    std::get<ImGuiMouseCursor_Arrow>(bd->MouseCursors));
				WindowingAPI::SetCursorMode(*windowPtr, WindowingAPI::CursorMode::Normal);
			}
			else
			{
				if(bd->CustomCursor != nullptr)
					WindowingAPI::SetCursor(*windowPtr, bd->CustomCursor);
				else
					WindowingAPI::SetCursor(*windowPtr, std::get<ImGuiMouseCursor_Arrow>(bd->MouseCursors));
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
	if((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
		return;

	io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
	if(!TRAP::Input::IsControllerConnected(TRAP::Input::Controller::One))
		return;

	#define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED) { io.AddKeyEvent(KEY_NO, TRAP::Input::IsControllerButtonPressed(TRAP::Input::Controller::One, BUTTON_NO)); }
	#define MAP_DPAD(KEY_NO, DPAD_NO, _UNUSED) { io.AddKeyEvent(KEY_NO, static_cast<bool>(TRAP::Input::GetControllerDPad(TRAP::Input::Controller::One, 0) & (DPAD_NO))); }
	#define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1) { float v = TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, AXIS_NO); v = (v - (V0)) / ((V1) - (V0)); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); }

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
	bd->WantUpdateMonitors = false;
	for (const auto& n : monitors)
	{
		if(n == nullptr)
			continue;

		ImGuiPlatformMonitor monitor;
		int32_t x = 0, y = 0;
		WindowingAPI::GetMonitorPos(*n, x, y);
		const std::optional<WindowingAPI::InternalVideoMode> videoMode = WindowingAPI::GetVideoMode(*n);
		if(!videoMode)
			continue; // Failed to get Video mode
		monitor.MainPos = monitor.WorkPos = ImVec2(NumericCast<float>(x), NumericCast<float>(y));
		monitor.MainSize = monitor.WorkSize = ImVec2(NumericCast<float>(videoMode->Width), NumericCast<float>(videoMode->Height));
		int32_t width = 0, height = 0;
		WindowingAPI::GetMonitorWorkArea(*n, x, y, width, height);
		if(width > 0 && height > 0)
		{
			monitor.WorkPos = ImVec2(NumericCast<float>(x), NumericCast<float>(y));
			monitor.WorkSize = ImVec2(NumericCast<float>(width), NumericCast<float>(height));
		}

		//Warning: The validity of monitor DPI information on Windows depends on the application
		//DPI awareness settings,
		//which generally needs to be set in the manifest or at runtime.
		float xScale = 0.0f, yScale = 0.0f;
		WindowingAPI::GetMonitorContentScale(*n, xScale, yScale);
		monitor.DpiScale = xScale;

		platformIO.Monitors.push_back(monitor);
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//MULTI-VIEWPORT | PLATFORM INTERFACE SUPPORT
//This is an _advanced_ and _optional_ feature,
//allowing the back-end to create and handle multiple viewports simultaneously.
//If you are new to dear imgui or creating a new binding for dear imgui,
//it is recommended that you completely ignore this section first...
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::WindowCloseCallback(const WindowingAPI::InternalWindow& window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
	if (ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
	(
		const_cast<WindowingAPI::InternalWindow*>(&window))
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
void TRAP::INTERNAL::ImGuiWindowing::WindowPosCallback(const WindowingAPI::InternalWindow& window,
                                                       [[maybe_unused]] const int32_t xPos,
													   [[maybe_unused]] const int32_t yPos)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
	if (ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
	(
		const_cast<WindowingAPI::InternalWindow*>(&window))
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

void TRAP::INTERNAL::ImGuiWindowing::WindowSizeCallback(const WindowingAPI::InternalWindow& window,
                                                        [[maybe_unused]] const int32_t width,
														[[maybe_unused]] const int32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
	if(ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
	(
		const_cast<WindowingAPI::InternalWindow*>(&window)
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
	WindowingAPI::WindowHint(WindowingAPI::Hint::Decorated, (viewport->Flags & ImGuiViewportFlags_NoDecoration) == 0);
	WindowingAPI::WindowHint(WindowingAPI::Hint::Floating, (viewport->Flags & ImGuiViewportFlags_TopMost) != 0);
	vd->Window = WindowingAPI::CreateWindow(NumericCast<uint32_t>(viewport->Size.x),
	                                        NumericCast<uint32_t>(viewport->Size.y), "No Title Yet", nullptr);
	vd->WindowOwned = true;
	viewport->PlatformHandle = static_cast<void*>(vd->Window);
#ifdef TRAP_PLATFORM_WINDOWS
	viewport->PlatformHandleRaw = WindowingAPI::GetWin32Window(*vd->Window);
#endif /*TRAP_PLATFORM_WINDOWS*/
	WindowingAPI::SetWindowPos(*vd->Window, NumericCast<int32_t>(viewport->Pos.x),
	                           NumericCast<int32_t>(viewport->Pos.y));

	//Install callbacks for secondary viewports
	WindowingAPI::SetWindowFocusCallback(*vd->Window, WindowFocusCallback);
	WindowingAPI::SetCursorEnterCallback(*vd->Window, CursorEnterCallback);
	WindowingAPI::SetCursorPosCallback(*vd->Window, CursorPosCallback);
	WindowingAPI::SetMouseButtonCallback(*vd->Window, MouseButtonCallback);
	WindowingAPI::SetScrollCallback(*vd->Window, ScrollCallback);
	WindowingAPI::SetKeyCallback(*vd->Window, KeyCallback);
	WindowingAPI::SetCharCallback(*vd->Window, CharCallback);
	WindowingAPI::SetWindowCloseCallback(*vd->Window, WindowCloseCallback);
	WindowingAPI::SetWindowPosCallback(*vd->Window, WindowPosCallback);
	WindowingAPI::SetWindowSizeCallback(*vd->Window, WindowSizeCallback);
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
				if(bd->KeyOwnerWindows[i] == vd->Window)
					KeyCallback(*vd->Window, static_cast<TRAP::Input::Key>(i), Input::KeyState::Released); //Later params are only used for main viewport, on which this function is never called.
			}

			WindowingAPI::DestroyWindow(vd->Window);
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

	if((viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon) != 0)
		WindowingAPI::HideWindowFromTaskbar(*static_cast<WindowingAPI::InternalWindow*>(viewport->PlatformHandle));

	WindowingAPI::ShowWindow(*vd->Window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImVec2 TRAP::INTERNAL::ImGuiWindowing::GetWindowPos(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	int32_t x = 0, y = 0;
	WindowingAPI::GetWindowPos(*vd->Window, x, y);

	return ImVec2(NumericCast<float>(x), NumericCast<float>(y));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowPos(ImGuiViewport* const viewport, const ImVec2 pos)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	vd->IgnoreWindowPosEventFrame = ImGui::GetFrameCount();
	WindowingAPI::SetWindowPos(*vd->Window, NumericCast<int32_t>(pos.x), NumericCast<int32_t>(pos.y));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImVec2 TRAP::INTERNAL::ImGuiWindowing::GetWindowSize(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	int32_t width = 0, height = 0;
	WindowingAPI::GetWindowSize(*vd->Window, width, height);

	return ImVec2(NumericCast<float>(width), NumericCast<float>(height));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowSize(ImGuiViewport* const viewport, const ImVec2 size)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	vd->IgnoreWindowSizeEventFrame = ImGui::GetFrameCount();

	WindowingAPI::SetWindowSize(*vd->Window, NumericCast<int32_t>(size.x), NumericCast<int32_t>(size.y));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowTitle(ImGuiViewport* const viewport, const char* const title)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::SetWindowTitle(*vd->Window, title);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowFocus(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::FocusWindow(*vd->Window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::ImGuiWindowing::GetWindowFocus(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	return WindowingAPI::GetWindowHint(*vd->Window, WindowingAPI::Hint::Focused);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::ImGuiWindowing::GetWindowMinimized(ImGuiViewport* const viewport)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	return WindowingAPI::GetWindowHint(*vd->Window, WindowingAPI::Hint::Minimized);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowAlpha(ImGuiViewport* const viewport, const float alpha)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::SetWindowOpacity(*vd->Window, alpha);
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
	const VkResult err = WindowingAPI::CreateWindowSurface(std::bit_cast<const VkInstance>(vkInstance),
	                                                       *vd->Window,
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
	vd->Window = bd->Window;
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

#endif /*TRAP_HEADLESS_MODE*/