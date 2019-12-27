#include "TRAPPCH.h"
#include "ImGuiWindowing.h"
#include "Application.h"

/*
std::shared_ptr<TRAP::INTERNAL::WindowingAPI::InternalWindow> TRAP::INTERNAL::ImGuiWindowing::s_window = nullptr;
TRAP::INTERNAL::WindowingAPI::ContextAPI TRAP::INTERNAL::ImGuiWindowing::s_clientAPI = WindowingAPI::ContextAPI::OpenGL;
double TRAP::INTERNAL::ImGuiWindowing::s_time = 0.0;
std::array<bool, 5> TRAP::INTERNAL::ImGuiWindowing::s_mouseJustPressed{};
std::array<std::shared_ptr<TRAP::INTERNAL::WindowingAPI::InternalCursor>, ImGuiMouseCursor_COUNT> TRAP::INTERNAL::ImGuiWindowing::s_mouseCursors{};
bool TRAP::INTERNAL::ImGuiWindowing::s_installedCallbacks = false;
bool TRAP::INTERNAL::ImGuiWindowing::s_wantUpdateMonitors = true;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::MouseButtonFunc TRAP::INTERNAL::ImGuiWindowing::s_prevUserCallbackMouseButton = nullptr;
TRAP::INTERNAL::WindowingAPI::ScrollFunc TRAP::INTERNAL::ImGuiWindowing::s_prevUserCallbackScroll = nullptr;
TRAP::INTERNAL::WindowingAPI::KeyFunc TRAP::INTERNAL::ImGuiWindowing::s_prevUserCallbackKey = nullptr;
TRAP::INTERNAL::WindowingAPI::CharFunc TRAP::INTERNAL::ImGuiWindowing::s_prevUserCallbackChar = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::ImGuiWindowing::InitForOpenGL(const std::shared_ptr<WindowingAPI::InternalWindow>& window, const bool installCallbacks)
{
	return Init(window, installCallbacks, WindowingAPI::ContextAPI::OpenGL);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::Shutdown()
{
	if (s_installedCallbacks)
	{
		WindowingAPI::SetMouseButtonCallback(s_window, s_prevUserCallbackMouseButton);
		WindowingAPI::SetScrollCallback(s_window, s_prevUserCallbackScroll);
		WindowingAPI::SetKeyCallback(s_window, s_prevUserCallbackKey);
		WindowingAPI::SetCharCallback(s_window, s_prevUserCallbackChar);
		s_installedCallbacks = false;
	}

	for (ImGuiMouseCursor cursorN = 0; cursorN < ImGuiMouseCursor_COUNT; cursorN++)
	{
		WindowingAPI::DestroyCursor(s_mouseCursors[cursorN]);
		s_mouseCursors[cursorN] = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::NewFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

	//Setup display size (every frame to accommodate for window resizing)
	int32_t width, height;
	int32_t displayWidth, displayHeight;
	WindowingAPI::GetWindowSize(s_window, width, height);
	WindowingAPI::GetFrameBufferSize(s_window, displayWidth, displayHeight);
	io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
	if (width > 0 && height > 0)
		io.DisplayFramebufferScale = ImVec2(static_cast<float>(displayWidth) / width, static_cast<float>(displayHeight) / height);
	if (s_wantUpdateMonitors)
		UpdateMonitors();

	//Setup time step
	const double currentTime = Application::GetTime();
	io.DeltaTime = s_time > 0.0 ? static_cast<float>(currentTime - s_time) : static_cast<float>(1.0f / 60.0f);
	s_time = currentTime;

	UpdateMousePosAndButtons();
	UpdateMouseCursor();

	//Update game controller (if enabled and available)
	UpdateGamespads();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::ImGuiWindowing::Init(const std::shared_ptr<WindowingAPI::InternalWindow>& window, const bool installCallbacks, const WindowingAPI::ContextAPI clientAPI)
{
	s_window = window;
	s_time = 0.0;

	//Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; //We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; //We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports; //We can create multi-viewports on the Platform side (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; //We can set io.MouseHoveredViewport correctly (optional, not easy)
	io.BackendPlatformName = "TRAP";

	//Keyboard mapping.
	//ImGui will use those indices to peak into the io.KeysDown[] array
	io.KeyMap[ImGuiKey_Tab] = static_cast<int32_t>(Input::Key::Tab);
	io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int32_t>(Input::Key::Left);
	io.KeyMap[ImGuiKey_RightArrow] = static_cast<int32_t>(Input::Key::Right);
	io.KeyMap[ImGuiKey_UpArrow] = static_cast<int32_t>(Input::Key::Up);
	io.KeyMap[ImGuiKey_DownArrow] = static_cast<int32_t>(Input::Key::Down);
	io.KeyMap[ImGuiKey_PageUp] = static_cast<int32_t>(Input::Key::Page_Up);
	io.KeyMap[ImGuiKey_PageDown] = static_cast<int32_t>(Input::Key::Page_Down);
	io.KeyMap[ImGuiKey_Home] = static_cast<int32_t>(Input::Key::Home);
	io.KeyMap[ImGuiKey_End] = static_cast<int32_t>(Input::Key::End);
	io.KeyMap[ImGuiKey_Insert] = static_cast<int32_t>(Input::Key::Insert);
	io.KeyMap[ImGuiKey_Delete] = static_cast<int32_t>(Input::Key::Delete);
	io.KeyMap[ImGuiKey_Backspace] = static_cast<int32_t>(Input::Key::Backspace);
	io.KeyMap[ImGuiKey_Space] = static_cast<int32_t>(Input::Key::Space);
	io.KeyMap[ImGuiKey_Enter] = static_cast<int32_t>(Input::Key::Enter);
	io.KeyMap[ImGuiKey_Escape] = static_cast<int32_t>(Input::Key::Escape);
	io.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<int32_t>(Input::Key::KP_Enter);
	io.KeyMap[ImGuiKey_A] = static_cast<int32_t>(Input::Key::A);
	io.KeyMap[ImGuiKey_C] = static_cast<int32_t>(Input::Key::C);
	io.KeyMap[ImGuiKey_V] = static_cast<int32_t>(Input::Key::V);
	io.KeyMap[ImGuiKey_X] = static_cast<int32_t>(Input::Key::X);
	io.KeyMap[ImGuiKey_Y] = static_cast<int32_t>(Input::Key::Y);
	io.KeyMap[ImGuiKey_Z] = static_cast<int32_t>(Input::Key::Z);

	io.SetClipboardTextFn = SetClipboardText;
	io.GetClipboardTextFn = GetClipboardText;
	io.ClipboardUserData = static_cast<void*>(&s_window);

	s_mouseCursors[ImGuiMouseCursor_Arrow] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::Arrow);
	s_mouseCursors[ImGuiMouseCursor_TextInput] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::Input);
	s_mouseCursors[ImGuiMouseCursor_ResizeNS] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::ResizeVertical);
	s_mouseCursors[ImGuiMouseCursor_ResizeEW] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::ResizeHorizontal);
	s_mouseCursors[ImGuiMouseCursor_Hand] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::PointingHand);
	s_mouseCursors[ImGuiMouseCursor_ResizeAll] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::ResizeAll);
	s_mouseCursors[ImGuiMouseCursor_ResizeNESW] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::ResizeDiagonalTopLeftBottomRight);
	s_mouseCursors[ImGuiMouseCursor_ResizeNWSE] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::ResizeDiagonalTopRightBottomLeft);
	s_mouseCursors[ImGuiMouseCursor_NotAllowed] = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::NotAllowed);

	//Chain WindowingAPI callback: our callback will call the user's previously installed callbacks, if any.
	s_prevUserCallbackMouseButton = nullptr;
	s_prevUserCallbackScroll = nullptr;
	s_prevUserCallbackKey = nullptr;
	s_prevUserCallbackChar = nullptr;
	if(installCallbacks)
	{
		s_installedCallbacks = true;
		s_prevUserCallbackMouseButton = WindowingAPI::GetMouseButtonCallback(window);
		WindowingAPI::SetMouseButtonCallback(window, MouseButtonCallback);
		s_prevUserCallbackScroll = WindowingAPI::GetScrollCallback(window);
		WindowingAPI::SetScrollCallback(window, ScrollCallback);
		s_prevUserCallbackKey = WindowingAPI::GetKeyCallback(window);
		WindowingAPI::SetKeyCallback(window, KeyCallback);
		s_prevUserCallbackChar = WindowingAPI::GetCharCallback(window);
		WindowingAPI::SetCharCallback(window, CharCallback);
	}

	//Our mouse update function expect PlatformHandle to be filled for the main viewport
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	main_viewport->PlatformHandle = static_cast<void*>(&s_window);
#ifdef TRAP_PLATFORM_WINDOWS
	main_viewport->PlatformHandleRaw = WindowingAPI::GetWin32Window(s_window);
#endif
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		InitPlatformInterface();

	s_clientAPI = clientAPI;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::InitPlatformInterface()
{
	//Register platform interface (will be coupled with a renderer interface)
	ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	platformIO.Platform_CreateWindow = CreateWindow;
	platformIO.Platform_DestroyWindow = ImGuiWindowing::DestroyWindow;
	platformIO.Platform_ShowWindow = ImGuiWindowing::ShowWindow;
	platformIO.Platform_SetWindowPos = ImGuiWindowing::SetWindowPos;
	platformIO.Platform_GetWindowPos = GetWindowPos;
	platformIO.Platform_SetWindowSize = SetWindowSize;
	platformIO.Platform_GetWindowSize = GetWindowSize;
	platformIO.Platform_SetWindowFocus = SetWindowFocus;
	platformIO.Platform_GetWindowFocus = GetWindowFocus;
	platformIO.Platform_GetWindowMinimized = GetWindowMinimized;
	platformIO.Platform_SetWindowTitle = SetWindowTitle;
	platformIO.Platform_RenderWindow = RenderWindow;
	platformIO.Platform_SwapBuffers = ImGuiWindowing::SwapBuffers;
	platformIO.Platform_SetWindowAlpha = SetWindowAlpha;
	platformIO.Platform_CreateVkSurface = CreateVkSurface;
#ifdef TRAP_PLATFORM_WINDOWS
	platformIO.Platform_SetImeInputPos = IMEInputPos;
#endif

	UpdateMonitors();
	WindowingAPI::SetMonitorCallback(MonitorCallback);

	//Register main window handle (which is owned by the main application, not by us)
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGuiViewportDataTRAP* data = IM_NEW(ImGuiViewportDataTRAP)();
	data->Window = s_window;
	data->WindowOwned = false;
	main_viewport->PlatformUserData = data;
	main_viewport->PlatformHandle = static_cast<void*>(&s_window);
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::INTERNAL::ImGuiWindowing::GetClipboardText(void* userData)
{
	return WindowingAPI::GetClipboardString(*static_cast<std::shared_ptr<WindowingAPI::InternalWindow>*>(userData)).c_str();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetClipboardText(void* userData, const char* text)
{
	WindowingAPI::SetClipboardString(*static_cast<std::shared_ptr<WindowingAPI::InternalWindow>*>(userData), text);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::MouseButtonCallback(const std::shared_ptr<WindowingAPI::InternalWindow>& window, Input::MouseButton mouseButton, const bool pressed)
{
	if (s_prevUserCallbackMouseButton != nullptr && window == s_window)
		s_prevUserCallbackMouseButton(window, mouseButton, pressed);

	if (pressed && static_cast<uint32_t>(mouseButton) >= 0 && static_cast<uint32_t>(mouseButton) < s_mouseJustPressed.size())
		s_mouseJustPressed[static_cast<uint32_t>(mouseButton)] = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::ScrollCallback(const std::shared_ptr<WindowingAPI::InternalWindow>& window, const double xOffset, const double yOffset)
{
	if (s_prevUserCallbackScroll != nullptr && window == s_window)
		s_prevUserCallbackScroll(window, xOffset, yOffset);

	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += static_cast<float>(xOffset);
	io.MouseWheel += static_cast<float>(yOffset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::KeyCallback(const std::shared_ptr<WindowingAPI::InternalWindow>& window, Input::Key key, const int32_t scanCode, const bool pressed)
{
	if (s_prevUserCallbackKey != nullptr && window == s_window)
		s_prevUserCallbackKey(window, key, scanCode, pressed);

	ImGuiIO& io = ImGui::GetIO();
	if (pressed)
		io.KeysDown[static_cast<uint32_t>(key)] = true;
	if (!pressed)
		io.KeysDown[static_cast<uint32_t>(key)] = false;

	//Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown(static_cast<uint32_t>(Input::Key::Left_Control)) || io.KeysDown[static_cast<uint32_t>(Input::Key::Right_Control)];
	io.KeyShift = io.KeysDown(static_cast<uint32_t>(Input::Key::Left_Shift)) || io.KeysDown[static_cast<uint32_t>(Input::Key::Right_Shift)];
	io.KeyAlt = io.KeysDown(static_cast<uint32_t>(Input::Key::Left_ALT)) || io.KeysDown[static_cast<uint32_t>(Input::Key::Right_ALT)];
	io.KeySuper = io.KeysDown(static_cast<uint32_t>(Input::Key::Left_Super)) || io.KeysDown[static_cast<uint32_t>(Input::Key::Right_Super)];
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::CharCallback(const std::shared_ptr<WindowingAPI::InternalWindow>& window, const uint32_t codePoint)
{
	if (s_prevUserCallbackChar != nullptr && window == s_window)
		s_prevUserCallbackChar(window, codePoint);

	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(codePoint);
}*/