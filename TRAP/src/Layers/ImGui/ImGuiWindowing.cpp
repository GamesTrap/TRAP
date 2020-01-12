#include "TRAPPCH.h"
#include "ImGuiWindowing.h"
#include "Application.h"

TRAP::INTERNAL::WindowingAPI::InternalWindow* TRAP::INTERNAL::ImGuiWindowing::s_window = nullptr;
TRAP::INTERNAL::WindowingAPI::ContextAPI TRAP::INTERNAL::ImGuiWindowing::s_clientAPI = WindowingAPI::ContextAPI::OpenGL;
double TRAP::INTERNAL::ImGuiWindowing::s_time = 0.0;
std::array<bool, 5> TRAP::INTERNAL::ImGuiWindowing::s_mouseJustPressed{};
std::array<TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalCursor>, ImGuiMouseCursor_COUNT> TRAP::INTERNAL::ImGuiWindowing::s_mouseCursors{};
bool TRAP::INTERNAL::ImGuiWindowing::s_installedCallbacks = false;
bool TRAP::INTERNAL::ImGuiWindowing::s_wantUpdateMonitors = true;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::MouseButtonFunc TRAP::INTERNAL::ImGuiWindowing::s_prevUserCallbackMouseButton = nullptr;
TRAP::INTERNAL::WindowingAPI::ScrollFunc TRAP::INTERNAL::ImGuiWindowing::s_prevUserCallbackScroll = nullptr;
TRAP::INTERNAL::WindowingAPI::KeyFunc TRAP::INTERNAL::ImGuiWindowing::s_prevUserCallbackKey = nullptr;
TRAP::INTERNAL::WindowingAPI::CharFunc TRAP::INTERNAL::ImGuiWindowing::s_prevUserCallbackChar = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::ImGuiWindowing::InitForOpenGL(WindowingAPI::InternalWindow* window, const bool installCallbacks)
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
		WindowingAPI::DestroyCursor(std::move(s_mouseCursors[cursorN]));
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
	UpdateGamepads();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::ImGuiWindowing::Init(WindowingAPI::InternalWindow* window, const bool installCallbacks, const WindowingAPI::ContextAPI clientAPI)
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
		s_prevUserCallbackMouseButton = WindowingAPI::GetMouseButtonCallback(s_window);
		WindowingAPI::SetMouseButtonCallback(s_window, MouseButtonCallback);
		s_prevUserCallbackScroll = WindowingAPI::GetScrollCallback(s_window);
		WindowingAPI::SetScrollCallback(s_window, ScrollCallback);
		s_prevUserCallbackKey = WindowingAPI::GetKeyCallback(s_window);
		WindowingAPI::SetKeyCallback(s_window, KeyCallback);
		s_prevUserCallbackChar = WindowingAPI::GetCharCallback(s_window);
		WindowingAPI::SetCharCallback(s_window, CharCallback);
	}

	//Our mouse update function expect PlatformHandle to be filled for the main viewport
	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	mainViewport->PlatformHandle = s_window;
#ifdef TRAP_PLATFORM_WINDOWS
	mainViewport->PlatformHandleRaw = WindowingAPI::GetWin32Window(s_window);
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
	platformIO.Platform_RenderWindow = RenderWindow;
	platformIO.Platform_SwapBuffers = SwapBuffers;
	platformIO.Platform_SetWindowAlpha = SetWindowAlpha;
	platformIO.Platform_CreateVkSurface = CreateVkSurface;
#ifdef TRAP_PLATFORM_WINDOWS
	platformIO.Platform_SetImeInputPos = SetIMEInputPos;
#endif

	UpdateMonitors();
	WindowingAPI::SetMonitorCallback(MonitorCallback);

	//Register main window handle (which is owned by the main application, not by us)
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGuiViewportDataTRAP* data = IM_NEW(ImGuiViewportDataTRAP)();
	data->WindowPtr = s_window;
	data->WindowOwned = false;
	main_viewport->PlatformUserData = data;
	main_viewport->PlatformHandle = s_window;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateMousePosAndButtons()
{
	//Update buttons
	ImGuiIO& io = ImGui::GetIO();
	for(uint32_t i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
	{
		//If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		io.MouseDown[i] = s_mouseJustPressed[i] || WindowingAPI::GetMouseButton(s_window, static_cast<Input::MouseButton>(i)) != 0;
		s_mouseJustPressed[i] = false;
	}

	//Update mouse position
	const ImVec2 mousePosBackup = io.MousePos;
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	io.MouseHoveredViewport = 0;
	ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	for(uint32_t n = 0; n < static_cast<uint32_t>(platformIO.Viewports.Size); n++)
	{
		ImGuiViewport* viewport = platformIO.Viewports[n];
		WindowingAPI::InternalWindow* windowPtr = static_cast<WindowingAPI::InternalWindow*>(viewport->PlatformHandle);
		
		if (!windowPtr)
			return;
		
		const bool focused = WindowingAPI::GetWindowAttrib(windowPtr, WindowingAPI::Hint::Focused);
		if(focused)
		{
			if (io.WantSetMousePos)
				WindowingAPI::SetCursorPos(windowPtr, static_cast<double>(mousePosBackup.x - viewport->Pos.x), static_cast<double>(mousePosBackup.y - viewport->Pos.y));
			else
			{
				double mouseX = 0.0, mouseY = 0.0;
				WindowingAPI::GetCursorPos(windowPtr, mouseX, mouseY);
				if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					//Multi-Viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
					int32_t windowX = 0, windowY = 0;
					WindowingAPI::GetWindowPos(windowPtr, windowX, windowY);
					io.MousePos = ImVec2(static_cast<float>(mouseX) + windowX, static_cast<float>(mouseY) + windowY);
				}
				else
				{
					//Single-Viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
					io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
				}
			}
			for (uint32_t i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
				io.MouseDown[i] |= WindowingAPI::GetMouseButton(windowPtr, static_cast<Input::MouseButton>(i)) != 0;
		}

		//(Optional) When using multiple viewports: set io.MouseHoveredViewport to the viewport the OS mouse cursor is hovering.
		//Important: This information is not easy to provide and many high-level windowing libraries won't be able to provide it correctly, because
		//- This is _ignoring_ viewport with the ImGuiViewportFlags_NoInputs flag (pass-through windows).
		//- This is _regardless_ of whether another viewport is focused or being dragged from.
		//If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the back-end, imgui will ignore this field and infer the information by relying on the
		//rectangles and last focused time of every viewports it knows about. It will be unaware of other windows that may be sitting between or over your windows.
		if (WindowingAPI::GetWindowAttrib(windowPtr, WindowingAPI::Hint::Hovered) && !(viewport->Flags & ImGuiViewportFlags_NoInputs))
			io.MouseHoveredViewport = viewport->ID;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateMouseCursor()
{
	ImGuiIO& io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || WindowingAPI::GetCursorMode(s_window) == WindowingAPI::CursorMode::Disabled)
		return;

	const ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	for(uint32_t n = 0; n < static_cast<uint32_t>(platformIO.Viewports.Size); n++)
	{
		WindowingAPI::InternalWindow* windowPtr = static_cast<WindowingAPI::InternalWindow*>(platformIO.Viewports[n]->PlatformHandle);
		
		if (!windowPtr)
			return;

		if(imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			//Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			WindowingAPI::SetCursorMode(windowPtr, WindowingAPI::CursorMode::Hidden);
		}
		else
		{
			//Show OS mouse cursor
			WindowingAPI::SetCursor(windowPtr, s_mouseCursors[imguiCursor] ? s_mouseCursors[imguiCursor].get() : s_mouseCursors[ImGuiMouseCursor_Arrow].get());
			WindowingAPI::SetCursorMode(windowPtr, WindowingAPI::CursorMode::Normal);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateGamepads()
{
	ImGuiIO& io = ImGui::GetIO();
	std::memset(io.NavInputs, 0, sizeof(io.NavInputs));
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
		return;

	if (Input::IsControllerConnected(Input::Controller::One))
	{
		std::vector<float> axes = Input::GetAllControllerAxes(Input::Controller::One);
		std::vector<bool> buttons = Input::GetAllControllerButtons(Input::Controller::One);
		auto MAP_BUTTON = [buttons, &io](const uint32_t NAV_NO, const uint32_t BUTTON_NO) -> void
		{
			if (buttons.size() > BUTTON_NO&& buttons[BUTTON_NO])
				io.NavInputs[NAV_NO] = 1.0f;
		};
		auto MAP_ANALOG = [axes, &io](const uint32_t NAV_NO,
			const uint32_t AXIS_NO,
			const float V0,
			const float V1) -> void
		{
			float v = (axes.size() > AXIS_NO) ? axes[AXIS_NO] : V0;
			v = (v - V0) / (V1 - V0);
			if (v > 1.0f)
				v = 1.0f;
			if (io.NavInputs[NAV_NO] < v)
				io.NavInputs[NAV_NO] = v;
		};
		MAP_BUTTON(ImGuiNavInput_Activate, 0); //Cross|A
		MAP_BUTTON(ImGuiNavInput_Cancel, 1); //Circle|B
		MAP_BUTTON(ImGuiNavInput_Menu, 2); //Square|X
		MAP_BUTTON(ImGuiNavInput_Input, 3); //Triangle|Y
		MAP_BUTTON(ImGuiNavInput_DpadLeft, 13); //DPad Left
		MAP_BUTTON(ImGuiNavInput_DpadRight, 11); //DPad Right
		MAP_BUTTON(ImGuiNavInput_DpadUp, 10); //DPad Up
		MAP_BUTTON(ImGuiNavInput_DpadDown, 12); //DPad Down
		MAP_BUTTON(ImGuiNavInput_FocusPrev, 4); //L1|LB
		MAP_BUTTON(ImGuiNavInput_FocusNext, 5); //R1|RB
		MAP_BUTTON(ImGuiNavInput_TweakSlow, 4); //L1|LB
		MAP_BUTTON(ImGuiNavInput_TweakFast, 5); //R1|RB
		MAP_ANALOG(ImGuiNavInput_LStickLeft, 0, -0.3f, -0.9f);
		MAP_ANALOG(ImGuiNavInput_LStickRight, 0, +0.3f, +0.9f);
		MAP_ANALOG(ImGuiNavInput_LStickUp, 1, +0.3f, +0.9f);
		MAP_ANALOG(ImGuiNavInput_LStickDown, 1, -0.3f, -0.9f);
	
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
	}
	else
		io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::UpdateMonitors()
{
	ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	const auto& monitors = WindowingAPI::GetMonitors();
	platformIO.Monitors.resize(0);
	for (uint32_t n = 0; n < monitors.size(); n++)
	{
		ImGuiPlatformMonitor monitor;
		int32_t x, y;
		WindowingAPI::GetMonitorPos(monitors[n], x, y);
		const WindowingAPI::VideoMode videoMode = WindowingAPI::GetVideoMode(monitors[n]);

		monitor.MainPos = ImVec2(static_cast<float>(x), static_cast<float>(y));
		monitor.MainSize = ImVec2(static_cast<float>(videoMode.Width), static_cast<float>(videoMode.Height));
		int32_t width, height;
		WindowingAPI::GetMonitorWorkArea(monitors[n], x, y, width, height);
		monitor.WorkPos = ImVec2(static_cast<float>(x), static_cast<float>(y));
		monitor.WorkSize = ImVec2(static_cast<float>(width), static_cast<float>(height));

		//Warning: The validity of monitor DPI information on Windows depends on the application DPI awareness settings,
		//which generally needs to be set in the manifest or at runtime.
		float xScale, yScale;
		WindowingAPI::GetMonitorContentScale(monitors[n], xScale, yScale);
		monitor.DpiScale = xScale;
		platformIO.Monitors.push_back(monitor);
	}
	s_wantUpdateMonitors = false;
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::INTERNAL::ImGuiWindowing::GetClipboardText(void* userData)
{
	return WindowingAPI::GetClipboardString().c_str();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetClipboardText(void* userData, const char* text)
{
	WindowingAPI::SetClipboardString(text);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::MouseButtonCallback(const WindowingAPI::InternalWindow* window, Input::MouseButton mouseButton, const bool pressed)
{
	if (s_prevUserCallbackMouseButton != nullptr && window == s_window)
		s_prevUserCallbackMouseButton(window, mouseButton, pressed);

	if (pressed && static_cast<uint32_t>(mouseButton) >= 0 && static_cast<uint32_t>(mouseButton) < s_mouseJustPressed.size())
		s_mouseJustPressed[static_cast<uint32_t>(mouseButton)] = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::ScrollCallback(const WindowingAPI::InternalWindow* window, const double xOffset, const double yOffset)
{
	if (s_prevUserCallbackScroll != nullptr && window == s_window)
		s_prevUserCallbackScroll(window, xOffset, yOffset);

	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += static_cast<float>(xOffset);
	io.MouseWheel += static_cast<float>(yOffset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::KeyCallback(const WindowingAPI::InternalWindow* window, Input::Key key, const bool pressed)
{
	if (s_prevUserCallbackKey != nullptr && window == s_window)
		s_prevUserCallbackKey(window, key, pressed);

	ImGuiIO& io = ImGui::GetIO();
	if (pressed)
		io.KeysDown[static_cast<uint32_t>(key)] = true;
	if (!pressed)
		io.KeysDown[static_cast<uint32_t>(key)] = false;

	//Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[static_cast<uint32_t>(Input::Key::Left_Control)] || io.KeysDown[static_cast<uint32_t>(Input::Key::Right_Control)];
	io.KeyShift = io.KeysDown[static_cast<uint32_t>(Input::Key::Left_Shift)] || io.KeysDown[static_cast<uint32_t>(Input::Key::Right_Shift)];
	io.KeyAlt = io.KeysDown[static_cast<uint32_t>(Input::Key::Left_ALT)] || io.KeysDown[static_cast<uint32_t>(Input::Key::Right_ALT)];
	io.KeySuper = io.KeysDown[static_cast<uint32_t>(Input::Key::Left_Super)] || io.KeysDown[static_cast<uint32_t>(Input::Key::Right_Super)];
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::CharCallback(const WindowingAPI::InternalWindow* window, const uint32_t codePoint)
{
	if (s_prevUserCallbackChar != nullptr && window == s_window)
		s_prevUserCallbackChar(window, codePoint);

	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(codePoint);
}

//-------------------------------------------------------------------------------------------------------------------//
//MULTI-VIEWPORT | PLATFORM INTERFACE SUPPORT
//This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
//If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first...
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::WindowCloseCallback(const WindowingAPI::InternalWindow* window)
{
	if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(const_cast<WindowingAPI::InternalWindow*>(window)))
		viewport->PlatformRequestClose = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::WindowPosCallback(const WindowingAPI::InternalWindow* window, int32_t x, int32_t y)
{
	if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(const_cast<WindowingAPI::InternalWindow*>(window)))
		viewport->PlatformRequestMove = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::WindowSizeCallback(const WindowingAPI::InternalWindow* window, int32_t width, int32_t height)
{
	if(ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(const_cast<WindowingAPI::InternalWindow*>(window)))
	{
		if(ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData))
		{
			//TRAP may dispatch window size event after calling SetWindowSize().
			//However depending on the platform the callback may be invoked at different time:
			//On Windows it appears to be called within the SetWindowSize() call whereas
			//on Linux it is queued and invoked during PollEvents().
			//Because the event doesn't always fire on SetWindowSize() we use a frame counter tag to only ignore recent SetWindowSize() calls.
			const bool ignoreEvent = (ImGui::GetFrameCount() <= data->IgnoreWindowSizeEventFrame + 1);
			data->IgnoreWindowSizeEventFrame = -1;
			if (ignoreEvent)
				return;
		}
		viewport->PlatformRequestResize = true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::CreateWindow(ImGuiViewport* viewport)
{
	ImGuiViewportDataTRAP* data = IM_NEW(ImGuiViewportDataTRAP)();
	viewport->PlatformUserData = data;

	WindowingAPI::WindowHint(WindowingAPI::Hint::Visible, false);
	WindowingAPI::WindowHint(WindowingAPI::Hint::Focused, false);
	WindowingAPI::WindowHint(WindowingAPI::Hint::FocusOnShow, false);
	WindowingAPI::WindowHint(WindowingAPI::Hint::Decorated, (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? false : true);
	WindowingAPI::WindowHint(WindowingAPI::Hint::Floating, (viewport->Flags & ImGuiViewportFlags_TopMost) ? true : false);
	const WindowingAPI::InternalWindow* shareWindow = (s_clientAPI == WindowingAPI::ContextAPI::OpenGL) ? s_window : nullptr;
	data->Window = WindowingAPI::CreateWindow(static_cast<int32_t>(viewport->Size.x), static_cast<int32_t>(viewport->Size.y), "No Title Yet", nullptr, shareWindow);
	data->WindowPtr = data->Window.get();
	data->WindowOwned = true;
	viewport->PlatformHandle = static_cast<void*>(data->WindowPtr);
#ifdef TRAP_PLATFORM_WINDOWS
	viewport->PlatformHandleRaw = WindowingAPI::GetWin32Window(data->WindowPtr);
#endif
	WindowingAPI::SetWindowPos(data->WindowPtr, static_cast<int32_t>(viewport->Pos.x), static_cast<int32_t>(viewport->Pos.y));

	//Install callbacks for secondary viewports
	WindowingAPI::SetMouseButtonCallback(data->WindowPtr, MouseButtonCallback);
	WindowingAPI::SetScrollCallback(data->WindowPtr, ScrollCallback);
	WindowingAPI::SetKeyCallback(data->WindowPtr, KeyCallback);
	WindowingAPI::SetCharCallback(data->WindowPtr, CharCallback);
	WindowingAPI::SetWindowCloseCallback(data->WindowPtr, WindowCloseCallback);
	WindowingAPI::SetWindowPosCallback(data->WindowPtr, WindowPosCallback);
	WindowingAPI::SetWindowSizeCallback(data->WindowPtr, WindowSizeCallback);
	if(s_clientAPI == WindowingAPI::ContextAPI::OpenGL)
	{
		WindowingAPI::MakeContextCurrent(data->WindowPtr);
		WindowingAPI::SwapInterval(0);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::DestroyWindow(ImGuiViewport* viewport)
{
	if(ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData))
	{
		if(data->WindowOwned)
		{
#ifdef TRAP_PLATFORM_WINDOWS
			const HWND hwnd = static_cast<HWND>(viewport->PlatformHandleRaw);
			::RemovePropA(hwnd, "IMGUI_VIEWPORT");
#endif
			WindowingAPI::DestroyWindow(std::move(data->Window));
		}
		data->Window = nullptr;
		data->WindowPtr = nullptr;
		IM_DELETE(data);
	}
	viewport->PlatformUserData = viewport->PlatformHandle = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::ShowWindow(ImGuiViewport* viewport)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

#ifdef TRAP_PLATFORM_WINDOWS
	//TRAP hack: Hide icon from task bar
	const HWND hwnd = static_cast<HWND>(viewport->PlatformHandleRaw);
	if(viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon)
	{
		LONG exStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
		exStyle &= ~WS_EX_APPWINDOW;
		exStyle |= WS_EX_TOOLWINDOW;
		::SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
	}
#endif
	
	WindowingAPI::ShowWindow(data->WindowPtr);
}

//-------------------------------------------------------------------------------------------------------------------//

ImVec2 TRAP::INTERNAL::ImGuiWindowing::GetWindowPos(ImGuiViewport* viewport)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	int32_t x = 0, y = 0;
	WindowingAPI::GetWindowPos(data->WindowPtr, x, y);
	
	return ImVec2(static_cast<float>(x), static_cast<float>(y));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowPos(ImGuiViewport* viewport, const ImVec2 pos)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	return WindowingAPI::SetWindowPos(data->WindowPtr, static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y));
}

//-------------------------------------------------------------------------------------------------------------------//

ImVec2 TRAP::INTERNAL::ImGuiWindowing::GetWindowSize(ImGuiViewport* viewport)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	int32_t width = 0, height = 0;
	WindowingAPI::GetWindowSize(data->WindowPtr, width, height);

	return ImVec2(static_cast<float>(width), static_cast<float>(height));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowSize(ImGuiViewport* viewport, const ImVec2 size)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	data->IgnoreWindowSizeEventFrame = ImGui::GetFrameCount();

	WindowingAPI::SetWindowSize(data->WindowPtr, static_cast<int32_t>(size.x), static_cast<int32_t>(size.y));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::SetWindowTitle(data->WindowPtr, title);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowFocus(ImGuiViewport* viewport)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::FocusWindow(data->WindowPtr);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::ImGuiWindowing::GetWindowFocus(ImGuiViewport* viewport)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	return WindowingAPI::GetWindowAttrib(data->WindowPtr, WindowingAPI::Hint::Focused) != 0;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::ImGuiWindowing::GetWindowMinimized(ImGuiViewport* viewport)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	return WindowingAPI::GetWindowAttrib(data->WindowPtr, WindowingAPI::Hint::Minimized) != 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetWindowAlpha(ImGuiViewport* viewport, const float alpha)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);

	WindowingAPI::SetWindowOpacity(data->WindowPtr, alpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::RenderWindow(ImGuiViewport* viewport, void* unused)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	if (s_clientAPI == WindowingAPI::ContextAPI::OpenGL)
		WindowingAPI::MakeContextCurrent(data->WindowPtr);

	if(viewport->Flags & ImGuiViewportFlags_NoInputs)
		WindowingAPI::SetWindowAttrib(data->WindowPtr, WindowingAPI::Hint::MousePassthrough, true);
	else
		WindowingAPI::SetWindowAttrib(data->WindowPtr, WindowingAPI::Hint::MousePassthrough, false);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SwapBuffers(ImGuiViewport* viewport, void* unused)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	if(s_clientAPI == WindowingAPI::ContextAPI::OpenGL)
	{
		WindowingAPI::MakeContextCurrent(data->WindowPtr);
		WindowingAPI::SwapBuffers(data->WindowPtr);
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//IME (Input Method Editor) basic support for e.g. Asian language users
//-------------------------------------------------------------------------------------------------------------------//

//We provide a Win32 implementation because this is such a common issue for IME users
#ifdef TRAP_PLATFORM_WINDOWS
void TRAP::INTERNAL::ImGuiWindowing::SetIMEInputPos(ImGuiViewport* viewport, const ImVec2 pos)
{
	COMPOSITIONFORM cf = { CFS_FORCE_POSITION, {static_cast<LONG>(pos.x - viewport->Pos.x), static_cast<LONG>(pos.y - viewport->Pos.y)}, {0, 0, 0, 0} };
	if(const HWND hwnd = static_cast<HWND>(viewport->PlatformHandleRaw))
		if(const HIMC himc = ::ImmGetContext(hwnd))
		{
			::ImmSetCompositionWindow(himc, &cf);
			::ImmReleaseContext(hwnd, himc);
		}
}
#endif

//-------------------------------------------------------------------------------------------------------------------//
// Vulkan support (the Vulkan renderer needs to call a platform-side support function to create the surface)
//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::INTERNAL::ImGuiWindowing::CreateVkSurface(ImGuiViewport* viewport, const ImU64 vkInstance, const void* vkAllocator, ImU64* outVkSurface)
{
	ImGuiViewportDataTRAP* data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
	IM_ASSERT(s_clientAPI == WindowingAPI::ContextAPI::None);
	const VkResult err = WindowingAPI::CreateWindowSurface(reinterpret_cast<VkInstance>(vkInstance),
	                                                       data->WindowPtr,
	                                                       static_cast<const VkAllocationCallbacks*>(vkAllocator),
	                                                       reinterpret_cast<VkSurfaceKHR&>(*outVkSurface));

	return static_cast<int32_t>(err);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::MonitorCallback(const WindowingAPI::InternalMonitor* unused1, bool unused2)
{
	s_wantUpdateMonitors = true;
}