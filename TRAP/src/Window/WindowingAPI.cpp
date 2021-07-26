/*
Copyright (c) 2002-2006 Marcus Geelnard

Copyright (c) 2006-2019 Camilla Loewy

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
#include "Window.h"
#include "Events/KeyEvent.h"
#include "Layers/ImGui/ImGuiWindowing.h"

TRAP::INTERNAL::WindowingAPI::Data TRAP::INTERNAL::WindowingAPI::s_Data{};

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::Init()
{
	if (s_Data.Initialized)
		return true;

	s_Data = {}; //Reset
	if(!PlatformInit())
	{
		Shutdown();
		return false;
	}

	s_Data.Initialized = true;

	DefaultWindowHints();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::Shutdown()
{
	if (!s_Data.Initialized)
		return;

	s_Data.Callbacks.Monitor = nullptr;

	for (auto& monitor : s_Data.Monitors)
	{
		if (monitor)
			monitor.reset();
	}

	s_Data.Monitors.clear();

	PlatformShutdown();

	s_Data.Initialized = false;

	s_Data = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyWindow(Scope<InternalWindow> window)
{
	if (window == nullptr)
		return;

	//Clear all callbacks to avoid exposing a half torn-down window object
	window->Callbacks = {};

	PlatformDestroyWindow(window.get());

	//Unlink window from global linked list
	s_Data.WindowList.remove(window.get());

	window.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DefaultWindowHints()
{
	//The default is a focused, visible and resizable window with decorations
	s_Data.Hints.Window = {};
	s_Data.Hints.Window.Resizable = true;
	s_Data.Hints.Window.Visible = true;
	s_Data.Hints.Window.Decorated = true;
	s_Data.Hints.Window.Focused = true;
	s_Data.Hints.Window.FocusOnShow = true;
	s_Data.Hints.Window.MousePassthrough = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::WindowHint(const Hint hint, const bool value)
{
	switch(hint)
	{
	case Hint::Resizable:
		s_Data.Hints.Window.Resizable = value;
		break;

	case Hint::Maximized:
		s_Data.Hints.Window.Maximized = value;
		break;

	case Hint::Minimized:
		s_Data.Hints.Window.Maximized = !value;
		break;

	case Hint::Visible:
		s_Data.Hints.Window.Visible = value;
		break;

	case Hint::Focused:
		s_Data.Hints.Window.Focused = value;
		break;

	case Hint::FocusOnShow:
		s_Data.Hints.Window.FocusOnShow = value;
		break;

	case Hint::Decorated:
		s_Data.Hints.Window.Decorated = value;
		break;

	case Hint::Floating:
		s_Data.Hints.Window.Floating = value;
		break;

	case Hint::MousePassthrough:
		s_Data.Hints.Window.MousePassthrough = value;
		break;

	case Hint::Hovered:
	default:
		InputError(Error::Invalid_Enum, " Invalid Window Hint!");
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::INTERNAL::WindowingAPI::GetMonitorName(const InternalMonitor* monitor)
{
	return monitor->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalMonitor>& TRAP::INTERNAL::WindowingAPI::GetPrimaryMonitor()
{
	return s_Data.Monitors[0];
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::InternalMonitor*> TRAP::INTERNAL::WindowingAPI::GetMonitors()
{
	std::vector<InternalMonitor*> monitors{};

	for (const Scope<InternalMonitor>& monitor : s_Data.Monitors)
		monitors.push_back(monitor.get());

	return monitors;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::INTERNAL::WindowingAPI::InternalVideoMode& TRAP::INTERNAL::WindowingAPI::GetVideoMode(InternalMonitor* monitor)
{
	monitor->CurrentMode = PlatformGetVideoMode(monitor);
	return monitor->CurrentMode;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::GetVideoModes(InternalMonitor* monitor)
{
	if (!monitor)
		return std::vector<InternalVideoMode>{};
	if (!RefreshVideoModes(monitor))
		return std::vector<InternalVideoMode>{};

	return monitor->Modes;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalWindow> TRAP::INTERNAL::WindowingAPI::CreateWindow(const uint32_t width,
	                                                                                                 const uint32_t height,
	                                                                                                 const std::string& title,
	                                                                                                 InternalMonitor* monitor)
{
	TRAP_ASSERT(!title.empty(), "[Window] Empty Title provided!");
	TRAP_ASSERT(width > 0, "[Window] Invalid width provided!");
	TRAP_ASSERT(height > 0, "[Window] Invalid height provided!");

	if(width <= 0 || height <= 0)
	{
		InputError(Error::Invalid_Value, " Invalid Window size: " + std::to_string(width) + "x" +
		           std::to_string(height));
		return nullptr;
	}

	WindowConfig WNDConfig = s_Data.Hints.Window;

	WNDConfig.Width = width;
	WNDConfig.Height = height;
	WNDConfig.Title = title;

	Scope<InternalWindow> window = MakeScope<InternalWindow>();

	s_Data.WindowList.emplace_front(window.get());

	window->videoMode.Width = width;
	window->videoMode.Height = height;
	window->videoMode.RedBits = 8;
	window->videoMode.GreenBits = 8;
	window->videoMode.BlueBits = 8;
	window->videoMode.RefreshRate = -1;

	window->Monitor = monitor;
	window->Resizable = WNDConfig.Resizable;
	window->Decorated = WNDConfig.Decorated;
	window->Floating = WNDConfig.Floating;
	window->FocusOnShow = WNDConfig.FocusOnShow;
	window->MousePassthrough = WNDConfig.MousePassthrough;
	window->cursorMode = CursorMode::Normal;
	window->BorderlessFullscreen = false;

	window->MinWidth = -1;
	window->MinHeight = -1;
	window->MaxWidth = -1;
	window->MaxHeight = -1;

	//Open the actual window and create its context
	if (!PlatformCreateWindow(window.get(), WNDConfig))
	{
		DestroyWindow(std::move(window));
		return nullptr;
	}

	if (WNDConfig.MousePassthrough)
		PlatformSetWindowMousePassthrough(window.get(), WNDConfig.MousePassthrough);

	if (window->Monitor)
		CenterCursorInContentArea(window.get());
	else
	{
		if (WNDConfig.Visible)
		{
			PlatformShowWindow(window.get());
			if(WNDConfig.Focused)
				PlatformFocusWindow(window.get());
		}
	}

	return window;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowShouldClose(InternalWindow* window, const bool value)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->ShouldClose = value;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowTitle(const InternalWindow* window, const std::string& title)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	PlatformSetWindowTitle(window, title);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::GetMonitorContentScale(const InternalMonitor* monitor, float& xScale,
                                                          float& yScale)
{
	PlatformGetMonitorContentScale(monitor, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

//Center the cursor in the content area of the specified window
void TRAP::INTERNAL::WindowingAPI::CenterCursorInContentArea(InternalWindow* window)
{
	int32_t width = 0, height = 0;

	PlatformGetWindowSize(window, width, height);
	PlatformSetCursorPos(window, static_cast<double>(width) / 2.0, static_cast<double>(height) / 2.0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputError(const Error code, const std::string& str)
{
	std::string description = "[WindowingAPI]";

	if (!str.empty())
		description += str;
	else
	{
		if (code == Error::Invalid_Enum)
			description += " Invalid argument for enum parameter";
		else if (code == Error::Invalid_Value)
			description += " Invalid value for parameter";
		else if (code == Error::Out_Of_Memory)
			description += " Out of memory";
		else if (code == Error::API_Unavailable)
			description += " The requested API is unavailable";
		else if (code == Error::Platform_Error)
			description += " A platform-specific error occurred";
		else if (code == Error::Format_Unavailable)
			description += " The requested format is unavailable";
		else if (code == Error::Cursor_Unavailable)
			description += " The specified cursor shape is unavailable";
		else
			description += " UNKNOWN WINDOWING ERROR";
	}

	TP_ERROR(description, " Code(", static_cast<uint32_t>(code), ")");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyCursor(Scope<InternalCursor> cursor)
{
	if (cursor == nullptr)
		return;

	//Make sure the cursor is not being used by any window
	for(InternalWindow* window : s_Data.WindowList)
	{
		if (window->Cursor == cursor.get())
			SetCursor(window, nullptr);
	}

	PlatformDestroyCursor(cursor.get());

	//Unlink window from global linked list
	s_Data.CursorList.remove(cursor.get());

	cursor.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a custom cursor.
TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalCursor> TRAP::INTERNAL::WindowingAPI::CreateCursor(const Scope<Image>& image,
	                                                                                                 const int32_t xHotspot,
	                                                                                                 const int32_t yHotspot)
{
	Scope<InternalCursor> cursor;

	TRAP_ASSERT(image.get(), "[Window] image is nullptr!");

	if(!image)
		return nullptr;

	if (image->IsHDR())
	{
		InputError(Error::Invalid_Value, "[Cursor] HDR is unsupported as icon!");
		return nullptr;
	}

	if (image->GetBitsPerPixel() > 32)
	{
		InputError(Error::Invalid_Value, "[Cursor] BPP > 32 is unsupported as icon!");
		return nullptr;
	}

	if (!((image->GetColorFormat() == Image::ColorFormat::RGB && image->GetBitsPerPixel() == 24) ||
	      (image->GetColorFormat() == Image::ColorFormat::RGBA && image->GetBitsPerPixel() == 32)))
	{
		InputError(Error::Invalid_Value, "[Cursor] Unsupported BPP or format used!");
		return nullptr;
	}

	//Convert to RGBA 32BPP
	if (image->GetColorFormat() == Image::ColorFormat::RGB)
	{
		const Scope<Image> iconImage = Image::ConvertRGBToRGBA(image);

		cursor = MakeScope<InternalCursor>();
		s_Data.CursorList.emplace_front(cursor.get());

		if(!PlatformCreateCursor(cursor.get(), iconImage, xHotspot, yHotspot))
		{
			DestroyCursor(std::move(cursor));
			return nullptr;
		}

		return cursor;
	}
	if (image->GetColorFormat() == Image::ColorFormat::RGBA)
	{
		cursor = MakeScope<InternalCursor>();
		s_Data.CursorList.emplace_front(cursor.get());

		if (!PlatformCreateCursor(cursor.get(), image, xHotspot, yHotspot))
		{
			DestroyCursor(std::move(cursor));
			return nullptr;
		}

		return cursor;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalCursor> TRAP::INTERNAL::WindowingAPI::CreateStandardCursor(const CursorType& type)
{
	Scope<InternalCursor> cursor = MakeScope<InternalCursor>();

	s_Data.CursorList.emplace_front(cursor.get());

	if (!PlatformCreateStandardCursor(cursor.get(), type))
	{
		DestroyCursor(std::move(cursor));
		return nullptr;
	}

	return cursor;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetCursor(InternalWindow* window, InternalCursor* cursor)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Cursor = cursor;

	PlatformSetCursor(window, cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowIcon(InternalWindow* window, const Scope<Image>& image)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if(!image)
	{
		PlatformSetWindowIcon(window, nullptr);
		return;
	}

	if (image->IsHDR())
	{
		InputError(Error::Invalid_Value, "[Icon] HDR is unsupported as icon!");
		return;
	}

	if(image->GetBitsPerPixel() > 32)
	{
		InputError(Error::Invalid_Value, "[Icon] BPP > 32 is unsupported as icon!");
		return;
	}

	if(!((image->GetColorFormat() == Image::ColorFormat::RGB && image->GetBitsPerPixel() == 24) ||
			(image->GetColorFormat() == Image::ColorFormat::RGBA && image->GetBitsPerPixel() == 32)))
	{
		InputError(Error::Invalid_Value, "[Icon] Unsupported BPP or format used!");
		return;
	}

	//Convert to RGBA 32BPP
	if (image->GetColorFormat() == Image::ColorFormat::RGB)
	{
		const Scope<Image> imageRGBA = Image::ConvertRGBToRGBA(image);
		PlatformSetWindowIcon(window, imageRGBA);
	}
	else if (image->GetColorFormat() == Image::ColorFormat::RGBA)
		PlatformSetWindowIcon(window, image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowPos(const InternalWindow* window, const int32_t xPos, const int32_t yPos)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if (window->Monitor)
		return;

	PlatformSetWindowPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the position of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	xPos = 0;
	yPos = 0;

	PlatformGetWindowPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the size of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowSize(InternalWindow* window, const int32_t width, const int32_t height)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");
	TRAP_ASSERT(width > 0, "[Window] width is smaller than or equal to 0!");
	TRAP_ASSERT(height > 0, "[Window] height is smaller than or equal to 0!");

	window->videoMode.Width = width;
	window->videoMode.Height = height;

	PlatformSetWindowSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the size of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowSize(const InternalWindow* window, int32_t& width, int32_t& height)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	width = 0;
	height = 0;

	PlatformGetWindowSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the size of the framebuffer of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetFrameBufferSize(const InternalWindow* window, int32_t& width, int32_t& height)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	width = 0;
	height = 0;

	PlatformGetFrameBufferSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the opacity of the whole window.
void TRAP::INTERNAL::WindowingAPI::SetWindowOpacity(const InternalWindow* window, const float opacity)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if(opacity < 0.0f || opacity > 1.0f)
	{
		InputError(Error::Invalid_Value, " Invalid Window Opacity: " + std::to_string(opacity));
		return;
	}

	PlatformSetWindowOpacity(window, opacity);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the opacity of the whole window.
float TRAP::INTERNAL::WindowingAPI::GetWindowOpacity(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return PlatformGetWindowOpacity(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the content scale for the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowContentScale(const InternalWindow* window, float& xScale, float& yScale)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	xScale = 0.0f;
	yScale = 0.0f;

	PlatformGetWindowContentScale(window, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets an attribute for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowHint(InternalWindow* window, const Hint hint, const bool value)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	switch(hint)
	{
	case Hint::Resizable:
		window->Resizable = value;

		if (!window->Monitor)
			PlatformSetWindowResizable(window, value);
		break;

	case Hint::FocusOnShow:
		window->FocusOnShow = value;
		break;

	case Hint::Decorated:
		window->Decorated = value;
		if (!window->Monitor)
			PlatformSetWindowDecorated(window, value);
		break;

	case Hint::Floating:
		window->Floating = value;
		if (!window->Monitor)
			PlatformSetWindowFloating(window, value);
		break;

	case Hint::MousePassthrough:
		window->MousePassthrough = value;
		PlatformSetWindowMousePassthrough(window, value);
		break;

	case Hint::Maximized:
	case Hint::Minimized:
	case Hint::Visible:
	case Hint::Focused:
	case Hint::Hovered:
	default:
		InputError(Error::Invalid_Enum, " Invalid window attribute provided!");
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns an attribute of the specified window.
bool TRAP::INTERNAL::WindowingAPI::GetWindowHint(const InternalWindow* window, const Hint hint)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	switch(hint)
	{
	case Hint::Focused:
		return PlatformWindowFocused(window);

	case Hint::Visible:
		return PlatformWindowVisible(window);

	case Hint::Maximized:
		return PlatformWindowMaximized(window);

	case Hint::Minimized:
		return PlatformWindowMinimized(window);

	case Hint::Resizable:
		return window->Resizable;

	case Hint::FocusOnShow:
		return window->FocusOnShow;

	case Hint::Hovered:
		return PlatformWindowHovered(window);

	case Hint::Decorated:
		return window->Decorated;

	case Hint::Floating:
		return window->Floating;

	case Hint::MousePassthrough:
		return window->MousePassthrough;

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the mode, monitor, video mode and placement of a window.
void TRAP::INTERNAL::WindowingAPI::SetWindowMonitor(InternalWindow* window,
                                                    InternalMonitor* monitor,
                                                    const int32_t xPos,
                                                    const int32_t yPos,
                                                    const int32_t width,
                                                    const int32_t height,
                                                    const int32_t refreshRate)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");
	TRAP_ASSERT(width > 0, "[Window] width is smaller than or equal to 0!");
	TRAP_ASSERT(height > 0, "[Window] height is smaller than or equal to 0!");

	if (width <= 0 || height <= 0)
	{
		InputError(Error::Invalid_Value, " Invalid window size " + std::to_string(width) + "x" +
		           std::to_string(height));
		return;
	}

	if (refreshRate < 0 && refreshRate != -1)
	{
		InputError(Error::Invalid_Value, " Invalid refresh rate " + std::to_string(refreshRate));
		return;
	}

	window->videoMode.Width = width;
	window->videoMode.Height = height;
	window->videoMode.RefreshRate = refreshRate;

	PlatformSetWindowMonitor(window, monitor, xPos, yPos, width, height, refreshRate);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the mode, monitor and placement of a window.
void TRAP::INTERNAL::WindowingAPI::SetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");
	TRAP_ASSERT(monitor, "[Window] window is nullptr!");

	window->videoMode.Width = monitor->CurrentMode.Width;
	window->videoMode.Height = monitor->CurrentMode.Height;
	window->videoMode.RefreshRate = monitor->CurrentMode.RefreshRate;
	window->videoMode.RedBits = monitor->CurrentMode.RedBits;
	window->videoMode.GreenBits = monitor->CurrentMode.GreenBits;
	window->videoMode.BlueBits = monitor->CurrentMode.BlueBits;

	PlatformSetWindowMonitorBorderless(window, monitor);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the user pointer of the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowUserPointer(InternalWindow* window, void* pointer)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->UserPointer = pointer;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the user pointer of the specified window.
void* TRAP::INTERNAL::WindowingAPI::GetWindowUserPointer(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->UserPointer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetMonitorCallback(const MonitorFunc callback)
{
	s_Data.Callbacks.Monitor = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the position callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowPosCallback(InternalWindow* window, const WindowPositionFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Pos = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the size callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowSizeCallback(InternalWindow* window, const WindowSizeFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Size = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the minimize callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowMinimizeCallback(InternalWindow* window,
                                                             const WindowMinimizeFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Minimize = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the maximize callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowMaximizeCallback(InternalWindow* window,
                                                             const WindowMaximizeFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Maximize = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the close callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowCloseCallback(InternalWindow* window, const WindowCloseFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Close = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the focus callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowFocusCallback(InternalWindow* window, const WindowFocusFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Focus = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the framebuffer resize callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetFrameBufferSizeCallback(InternalWindow* window,
                                                              const FrameBufferSizeFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.FBSize = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the window content scale callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetContentScaleCallback(InternalWindow* window,
                                                           const WindowContentScaleFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Scale = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the key callback.
void TRAP::INTERNAL::WindowingAPI::SetKeyCallback(InternalWindow* window, const KeyFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Key = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the Unicode character callback.
void TRAP::INTERNAL::WindowingAPI::SetCharCallback(InternalWindow* window, const CharFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Character = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the mouse button callback.
void TRAP::INTERNAL::WindowingAPI::SetMouseButtonCallback(InternalWindow* window, const MouseButtonFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.MouseButton = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor position callback.
void TRAP::INTERNAL::WindowingAPI::SetCursorPosCallback(InternalWindow* window, const CursorPositionFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.CursorPos = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor enter callback.
void TRAP::INTERNAL::WindowingAPI::SetCursorEnterCallback(InternalWindow* window, const CursorEnterFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.CursorEnter = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the scroll callback.
void TRAP::INTERNAL::WindowingAPI::SetScrollCallback(InternalWindow* window, const ScrollFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Scroll = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the path drop callback.
void TRAP::INTERNAL::WindowingAPI::SetDropCallback(InternalWindow* window, const DropFunc callback)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Drop = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::MonitorFunc TRAP::INTERNAL::WindowingAPI::GetMonitorCallback()
{
	return s_Data.Callbacks.Monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the position callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowPositionFunc TRAP::INTERNAL::WindowingAPI::GetWindowPosCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Pos;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the size callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowSizeFunc TRAP::INTERNAL::WindowingAPI::GetWindowSizeCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Size;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the close callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowCloseFunc TRAP::INTERNAL::WindowingAPI::GetWindowCloseCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Close;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the focus callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowFocusFunc TRAP::INTERNAL::WindowingAPI::GetWindowFocusCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Focus;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the framebuffer resize callback for the specified window.
TRAP::INTERNAL::WindowingAPI::FrameBufferSizeFunc TRAP::INTERNAL::WindowingAPI::GetFrameBufferSizeCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.FBSize;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the window content scale callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowContentScaleFunc TRAP::INTERNAL::WindowingAPI::GetWindowContentScaleCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Scale;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the key callback.
TRAP::INTERNAL::WindowingAPI::KeyFunc TRAP::INTERNAL::WindowingAPI::GetKeyCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Key;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the Unicode character callback.
TRAP::INTERNAL::WindowingAPI::CharFunc TRAP::INTERNAL::WindowingAPI::GetCharCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Character;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the mouse button callback.
TRAP::INTERNAL::WindowingAPI::MouseButtonFunc TRAP::INTERNAL::WindowingAPI::GetMouseButtonCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the cursor position callback.
TRAP::INTERNAL::WindowingAPI::CursorPositionFunc TRAP::INTERNAL::WindowingAPI::GetCursorPosCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.CursorPos;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the cursor enter callback.
TRAP::INTERNAL::WindowingAPI::CursorEnterFunc TRAP::INTERNAL::WindowingAPI::GetCursorEnterCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.CursorEnter;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the scroll callback.
TRAP::INTERNAL::WindowingAPI::ScrollFunc TRAP::INTERNAL::WindowingAPI::GetScrollCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Scroll;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the path drop callback.
TRAP::INTERNAL::WindowingAPI::DropFunc TRAP::INTERNAL::WindowingAPI::GetDropCallback(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Drop;
}

//-------------------------------------------------------------------------------------------------------------------//

//Processes all pending events.
void TRAP::INTERNAL::WindowingAPI::PollEvents()
{
	PlatformPollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor mode for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetCursorMode(InternalWindow* window, const CursorMode mode)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if (window->cursorMode == mode)
		return;

	window->cursorMode = mode;

	PlatformGetCursorPos(window, window->VirtualCursorPosX, window->VirtualCursorPosY);
	PlatformSetCursorMode(window, mode);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrrieves the cursor mode for the specified window.
TRAP::INTERNAL::WindowingAPI::CursorMode TRAP::INTERNAL::WindowingAPI::GetCursorMode(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->cursorMode;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether raw mouse motion is supported.
bool TRAP::INTERNAL::WindowingAPI::RawMouseMotionSupported()
{
	return PlatformRawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetRawMouseMotionMode(InternalWindow* window, const bool enabled)
{
	if (!PlatformRawMouseMotionSupported())
	{
		InputError(Error::Platform_Error, " Raw mouse motion is not supported on this system");
		return;
	}

	if (window->RawMouseMotion == enabled)
		return;

	window->RawMouseMotion = enabled;
	PlatformSetRawMouseMotion(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrvieves the raw mouse motion mode for the specified window.
bool TRAP::INTERNAL::WindowingAPI::GetRawMouseMotionMode(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->RawMouseMotion;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the layout-specific name of the specified printable key.
const char* TRAP::INTERNAL::WindowingAPI::GetKeyName(const Input::Key key, int32_t scanCode)
{
	if (key != Input::Key::Unknown)
	{
		if (key != Input::Key::KP_Equal &&
			(key < Input::Key::KP_0 || key > Input::Key::KP_Add) &&
			(key < Input::Key::Apostrophe || key > Input::Key::World_2))
			return nullptr;

		scanCode = PlatformGetKeyScanCode(key);
	}

	return PlatformGetScanCodeName(scanCode);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the last reported state of a keyboard key for the specified window.
bool TRAP::INTERNAL::WindowingAPI::GetKey(const InternalWindow* window, Input::Key key)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if (key < Input::Key::Space || key > Input::Key::Menu)
	{
		InputError(Error::Invalid_Enum, " Invalid key: " + std::to_string(static_cast<int32_t>(key)));
		return false;
	}

	return window->Keys[static_cast<uint32_t>(key)];
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the last reported state of a mouse button for the specified window.
bool TRAP::INTERNAL::WindowingAPI::GetMouseButton(const InternalWindow* window, Input::MouseButton button)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	return window->MouseButtons[static_cast<uint32_t>(button)];
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the position of the cursor, relative to the content area of the window
void TRAP::INTERNAL::WindowingAPI::SetCursorPos(InternalWindow* window, const double xPos, const double yPos)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if (xPos < -DBL_MAX || xPos > DBL_MAX ||
	    yPos < -DBL_MAX || yPos > DBL_MAX)
	{
		InputError(Error::Invalid_Value, " Invalid cursor position: " + std::to_string(xPos) + "x" +
		           std::to_string(yPos));
		return;
	}

	if (!PlatformWindowFocused(window))
		return;

	if (window->cursorMode == CursorMode::Disabled)
	{
		//Only update the accumulated position if the cursor is disabled
		window->VirtualCursorPosX = xPos;
		window->VirtualCursorPosY = yPos;
	}
	else
		//Update system cursor position
		PlatformSetCursorPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the position of the cursor relative to the content area of the window.
void TRAP::INTERNAL::WindowingAPI::GetCursorPos(const InternalWindow* window, double& xPos, double& yPos)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	xPos = 0.0;
	yPos = 0.0;

	if (window->cursorMode == CursorMode::Disabled)
	{
		xPos = window->VirtualCursorPosX;
		yPos = window->VirtualCursorPosY;
	}
	else
		PlatformGetCursorPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the position of the monitor's viewport on the virtual screen.
void TRAP::INTERNAL::WindowingAPI::GetMonitorPos(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos)
{
	TRAP_ASSERT(monitor, "[Window] monitor is nullptr!");

	xPos = 0;
	yPos = 0;

	PlatformGetMonitorPos(monitor, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::GetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos,
                                                      int32_t& width, int32_t& height)
{
	TRAP_ASSERT(monitor, "[Window] monitor is nullptr!");

	xPos = 0;
	yPos = 0;
	width = 0;
	height = 0;

	PlatformGetMonitorWorkArea(monitor, xPos, yPos, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Makes the specified window visible.
void TRAP::INTERNAL::WindowingAPI::ShowWindow(InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if (window->Monitor)
		return;

	PlatformShowWindow(window);

	if (window->FocusOnShow)
		PlatformFocusWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Brings the specified window to front and sets input focus.
void TRAP::INTERNAL::WindowingAPI::FocusWindow(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	PlatformFocusWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Maximizes the specified window.
void TRAP::INTERNAL::WindowingAPI::MaximizeWindow(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if (window->Monitor || !window->Resizable)
		return;

	PlatformMaximizeWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Minimizes the specified window.
void TRAP::INTERNAL::WindowingAPI::MinimizeWindow(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	PlatformMinimizeWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Requests user attention to the specified window.
void TRAP::INTERNAL::WindowingAPI::RequestWindowAttention(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	PlatformRequestWindowAttention(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Hides the specified window.
void TRAP::INTERNAL::WindowingAPI::HideWindow(const InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if (window->Monitor)
		return;

	PlatformHideWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::RestoreWindow(InternalWindow* window)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	PlatformRestoreWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowSizeLimits(InternalWindow* window,
                                                       const int32_t minWidth,
                                                       const int32_t minHeight,
                                                       const int32_t maxWidth,
                                                       const int32_t maxHeight)
{
	TRAP_ASSERT(window, "[Window] window is nullptr!");

	if(minWidth != -1 && minHeight != -1 && (minWidth < 0 || minHeight < 0))
	{
		InputError(Error::Invalid_Value, " Invalid Minimum Window Size!");
		return;
	}

	if((maxWidth != -1 || maxHeight != -1) &&
	   (maxWidth < 0 || maxHeight < 0 || maxWidth < minWidth || maxHeight < minHeight))
	{
		InputError(Error::Invalid_Value, " Invalid Maximum Window Size!");
		return;
	}

	window->MinWidth = minWidth;
	window->MinHeight = minHeight;
	window->MaxWidth = maxWidth;
	window->MaxHeight = maxHeight;

	if (window->Monitor || !window->Resizable)
		return;

	PlatformSetWindowSizeLimits(window, minWidth, minHeight, maxWidth, maxHeight);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the clipboard to the specified string.
void TRAP::INTERNAL::WindowingAPI::SetClipboardString(const std::string_view string)
{
	PlatformSetClipboardString(string);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the contents of the clipboard as a string.
std::string TRAP::INTERNAL::WindowingAPI::GetClipboardString()
{
	return PlatformGetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the Vulkan loader and an ICD have been found.
bool TRAP::INTERNAL::WindowingAPI::VulkanSupported()
{
	return InitVulkan(1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the Vulkan instance extensions required by TRAP.
std::array<std::string, 2> TRAP::INTERNAL::WindowingAPI::GetRequiredInstanceExtensions()
{
	if (!InitVulkan(2))
		return {};

	if (s_Data.VK.Extensions[0].empty() || s_Data.VK.Extensions[1].empty())
		return {};

	return s_Data.VK.Extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a Vulkan surface for the specified window.
VkResult TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(VkInstance instance,
                                                           const InternalWindow* window,
                                                           const VkAllocationCallbacks* allocator,
                                                           VkSurfaceKHR& surface)
{
	TRAP_ASSERT(instance, "[Vulkan] Instance is nullptr!");
	TRAP_ASSERT(window, " Window is nullptr!");

	if (!InitVulkan(2))
		return VK_ERROR_INITIALIZATION_FAILED;

	if constexpr (s_Data.VK.Extensions.empty())
	{
		InputError(Error::API_Unavailable, "[Vulkan] Window surface creation extensions not found");
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	return PlatformCreateWindowSurface(instance, window, allocator, surface);
}

//-------------------------------------------------------------------------------------------------------------------//

//Searches an extension string for the specified extension
bool TRAP::INTERNAL::WindowingAPI::StringInExtensionString(const char* string, const char* extensions)
{
	const char* start = extensions;

	while(true)
	{
		const char* where = strstr(start, string);
		if (!where)
			return false;

		const char* terminator = where + strlen(string);
		if (where == start || *(where - 1) == ' ')
		{
			if (*terminator == ' ' || *terminator == '\0')
				break;
		}

		start = terminator;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::GetVulkanResultString(const VkResult result)
{
	switch (result)
	{
	case VK_SUCCESS:
		return "Success";

	case VK_NOT_READY:
		return "A fence or query has not yet completed";

	case VK_TIMEOUT:
		return "A wait operation has not completed in the specified time";

	case VK_EVENT_SET:
		return "An event is signaled";

	case VK_EVENT_RESET:
		return "An event is unsignaled";

	case VK_INCOMPLETE:
		return "A return array was too small for the result";

	case VK_ERROR_OUT_OF_HOST_MEMORY:
		return "A host memory allocation has failed";

	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		return "A device memory allocation has failed";

	case VK_ERROR_INITIALIZATION_FAILED:
		return "Initialization of an object could not be completed for implementation-specific reasons";

	case VK_ERROR_DEVICE_LOST:
		return "The logical or physical device has been lost";

	case VK_ERROR_MEMORY_MAP_FAILED:
		return "Mapping of a memory object has failed";

	case VK_ERROR_LAYER_NOT_PRESENT:
		return "A requested layer is not present or could not be loaded";

	case VK_ERROR_EXTENSION_NOT_PRESENT:
		return "A requested extension is not supported";

	case VK_ERROR_FEATURE_NOT_PRESENT:
		return "A requested feature is not supported";

	case VK_ERROR_INCOMPATIBLE_DRIVER:
		return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";

	case VK_ERROR_TOO_MANY_OBJECTS:
		return "Too many objects of the type have already been created";

	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		return "A requested format is not supported on this device";

	case VK_ERROR_SURFACE_LOST_KHR:
		return "A surface is no longer available";

	case VK_SUBOPTIMAL_KHR:
		return "A swapchain no longer matches the surface properties exactly, but can still be used";

	case VK_ERROR_OUT_OF_DATE_KHR:
		return "A surface has changed in such a way that it is no longer compatible with the swapchain";

	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		return "The display used by a swapchain does not use the same presentable image layout";

	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";

	case VK_ERROR_VALIDATION_FAILED_EXT:
		return "A validation layer found an error";

	default:
		return "Unknown Vulkan error";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a cursor motion event
//The position is specified in content area relative screen coordinates
void TRAP::INTERNAL::WindowingAPI::InputCursorPos(InternalWindow* window, const double xPos, const double yPos)
{
	if (Math::Abs(window->VirtualCursorPosX - xPos) < Math::Epsilon<double>() &&
		Math::Abs(window->VirtualCursorPosY - yPos) < Math::Epsilon<double>())
		return;

	window->VirtualCursorPosX = xPos;
	window->VirtualCursorPosY = yPos;

	if (window->Callbacks.CursorPos)
		window->Callbacks.CursorPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a physical key event
void TRAP::INTERNAL::WindowingAPI::InputKey(InternalWindow* window, Input::Key key, const int32_t,
                                            const bool pressed)
{
	if(key == Input::Key::Unknown)
		return;

	if (!pressed && window->Keys[static_cast<uint32_t>(key)] == false)
		return;

	window->Keys[static_cast<uint32_t>(key)] = pressed;

	if (window->Callbacks.Key)
		window->Callbacks.Key(window, key, pressed);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a Unicode codepoint input event
//The 'plain' parameter determines whether to emit a regular character event
void TRAP::INTERNAL::WindowingAPI::InputChar(const InternalWindow* window, const uint32_t codePoint)
{
	if (codePoint < 32 || (codePoint > 126 && codePoint < 160))
		return;

	if (window->Callbacks.Character)
		window->Callbacks.Character(window, codePoint);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a mouse button click event
void TRAP::INTERNAL::WindowingAPI::InputMouseClick(InternalWindow* window, Input::MouseButton button,
                                                   const bool pressed)
{
	window->MouseButtons[static_cast<uint32_t>(button)] = pressed;

	if (window->Callbacks.MouseButton)
		window->Callbacks.MouseButton(window, button, pressed);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a scroll event
void TRAP::INTERNAL::WindowingAPI::InputScroll(const InternalWindow* window, const double xOffset,
                                               const double yOffset)
{
	if (window->Callbacks.Scroll)
		window->Callbacks.Scroll(window, xOffset, yOffset);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a cursor enter/leave event
void TRAP::INTERNAL::WindowingAPI::InputCursorEnter(InternalWindow* window, const bool entered)
{
	if (window->Callbacks.CursorEnter)
		window->Callbacks.CursorEnter(window, entered);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window framebuffer has been resized
//The size is specified in pixels
void TRAP::INTERNAL::WindowingAPI::InputFrameBufferSize(const InternalWindow* window, const int32_t width,
                                                        const int32_t height)
{
	if (window->Callbacks.FBSize)
		window->Callbacks.FBSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been resized
//The size is specified in screen coordinates
void TRAP::INTERNAL::WindowingAPI::InputWindowSize(const InternalWindow* window, const int32_t width,
                                                   const int32_t height)
{
	if (window->Callbacks.Size)
		window->Callbacks.Size(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been minimized
void TRAP::INTERNAL::WindowingAPI::InputWindowMinimize(const InternalWindow* window, const bool restored)
{
	if (window->Callbacks.Minimize)
		window->Callbacks.Minimize(window, !restored);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been maximized
void TRAP::INTERNAL::WindowingAPI::InputWindowMaximize(const InternalWindow* window, const bool restored)
{
	if (window->Callbacks.Maximize)
		window->Callbacks.Maximize(window, !restored);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has moved
//The position is specified in content area relative screen coordinates
void TRAP::INTERNAL::WindowingAPI::InputWindowPos(const InternalWindow* window, const int32_t x, const int32_t y)
{
	if (window->Callbacks.Pos)
		window->Callbacks.Pos(window, x, y);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that the user wishes to close a window
void TRAP::INTERNAL::WindowingAPI::InputWindowCloseRequest(InternalWindow* window)
{
	window->ShouldClose = true;

	if (window->Callbacks.Close)
		window->Callbacks.Close(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of files or directories dropped on a window
void TRAP::INTERNAL::WindowingAPI::InputDrop(const InternalWindow* window, const std::vector<std::string>& paths)
{
	if (window->Callbacks.Drop)
		window->Callbacks.Drop(window, paths);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has lost or received input focus
void TRAP::INTERNAL::WindowingAPI::InputWindowFocus(InternalWindow* window, const bool focused)
{
	if (window->Callbacks.Focus)
		window->Callbacks.Focus(window, focused);

	if(focused)
		return;

	for (uint32_t key = 0; key <= static_cast<uint32_t>(Input::Key::Menu); key++)
	{
		if (window->Keys[key] == true)
		{
			const int32_t scanCode = PlatformGetKeyScanCode(static_cast<Input::Key>(key));
			InputKey(window, static_cast<Input::Key>(key), scanCode, false);
		}
	}

	for (uint32_t button = 0; button <= static_cast<uint32_t>(Input::MouseButton::Eight); button++)
	{
		if (window->MouseButtons[button])
			InputMouseClick(window, static_cast<Input::MouseButton>(button), false);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a the keyboard layout has changed
void TRAP::INTERNAL::WindowingAPI::InputKeyboardLayout()
{
	if(!TRAP::Input::s_eventCallback)
		return;

	//This function is not window specific because you can only have 1 keyboard layout selected and not multiple!
	//So apply this globally
	//This event gets redirected to the TRAP::Input callback
	TRAP::Events::KeyLayoutEvent event(TRAP::Input::GetKeyboardLayoutName());
	TRAP::Input::s_eventCallback(event);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the available modes for the specified monitor
bool TRAP::INTERNAL::WindowingAPI::RefreshVideoModes(InternalMonitor* monitor)
{
	if (!monitor->Modes.empty())
		return true;

	std::vector<InternalVideoMode> modes = PlatformGetVideoModes(monitor);
	if (modes.empty())
		return false;

	std::qsort(modes.data(), modes.size(), sizeof(InternalVideoMode), CompareVideoModes);

	monitor->Modes = {};
	monitor->Modes = modes;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::InitVulkan(const uint32_t mode)
{
	uint32_t count;

	if (s_Data.VK.Available)
		return true;

	//Initialize Vulkan Meta Loader
	VkResult err = VkInitialize();
	if(err != VK_SUCCESS)
	{
		InputError(Error::API_Unavailable, "[Vulkan][Loader] Failed to load function pointers!");
		return false;
	}

	err = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
	if (err)
	{
		//NOTE: This happens on systems with a loader but without any Vulkan ICD
		if (mode == 2)
			InputError(Error::API_Unavailable, "[Vulkan] Failed to query instance extension count: " +
			           GetVulkanResultString(err));

		return false;
	}

	std::vector<VkExtensionProperties> ep(count);

	err = vkEnumerateInstanceExtensionProperties(nullptr, &count, ep.data());
	if (err)
	{
		InputError(Error::API_Unavailable, "[Vulkan] Failed to query instance extensions: " +
		           GetVulkanResultString(err));
		return false;
	}

	for (uint32_t i = 0; i < count; i++)
	{
		const std::string_view ext(ep[i].extensionName);

		if (ext == "VK_KHR_surface")
			s_Data.VK.KHR_Surface = true;
#if defined(TRAP_PLATFORM_WINDOWS)
		else if (ext == "VK_KHR_win32_surface")
			s_Data.VK.KHR_Win32_Surface = true;
#elif defined(TRAP_PLATFORM_LINUX)
		else if (ext == "VK_KHR_xlib_surface")
			s_Data.VK.KHR_XLib_Surface = true;
		else if (ext == "VK_KHR_xcb_surface")
			s_Data.VK.KHR_XCB_Surface = true;
		else if (ext == "VK_KHR_wayland_surface")
			s_Data.VK.KHR_Wayland_Surface = true;
#endif
	}

	s_Data.VK.Available = true;

	PlatformGetRequiredInstanceExtensions(s_Data.VK.Extensions);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Lexically compare video modes, used by qsort
int32_t TRAP::INTERNAL::WindowingAPI::CompareVideoModes(const void* fp, const void* sp)
{
	const InternalVideoMode* fm = static_cast<const InternalVideoMode*>(fp);
	const InternalVideoMode* sm = static_cast<const InternalVideoMode*>(sp);
	const int32_t fbpp = fm->RedBits + fm->GreenBits + fm->BlueBits;
	const int32_t sbpp = sm->RedBits + sm->GreenBits + sm->BlueBits;
	const int32_t farea = fm->Width * fm->Height;
	const int32_t sarea = sm->Width * sm->Height;

	//First sort on color bits per pixel
	if (fbpp != sbpp)
		return fbpp - sbpp;

	//Then sort on screen area
	if (farea != sarea)
		return farea - sarea;

	//Then sort on width
	if (fm->Width != sm->Width)
		return fm->Width - sm->Width;

	//Lastly sort on refresh rate
	return fm->RefreshRate - sm->RefreshRate;
}

//-------------------------------------------------------------------------------------------------------------------//

//Splits a color depth into red, green and blue bit depths
void TRAP::INTERNAL::WindowingAPI::SplitBPP(int32_t bpp, int32_t& red, int32_t& green, int32_t& blue)
{
	//We assume that by 32 the user really meant 24
	if (bpp == 32)
		bpp = 24;

	//Convert "bits per pixel" to red, green & blue sizes

	red = green = blue = bpp / 3;
	const int32_t delta = bpp - (red * 3);
	if (delta >= 1)
		green = green + 1;

	if (delta == 2)
		red = red + 1;
}

//-------------------------------------------------------------------------------------------------------------------//

//Chooses the video mode most closely matching the desired one
TRAP::INTERNAL::WindowingAPI::InternalVideoMode* TRAP::INTERNAL::WindowingAPI::ChooseVideoMode(InternalMonitor* monitor,
                                                                                               const InternalVideoMode& desired)
{
	uint32_t leastSizeDiff = UINT_MAX;
	uint32_t rateDiff, leastRateDiff = UINT_MAX;
	uint32_t leastColorDiff = UINT_MAX;
	InternalVideoMode* closest = nullptr;

	if (!RefreshVideoModes(monitor))
		return nullptr;

	for (auto& mode : monitor->Modes)
	{
		InternalVideoMode* current = &mode;

		uint32_t colorDiff = 0;

		if (desired.RedBits != -1)
			colorDiff += abs(current->RedBits - desired.RedBits);
		if (desired.GreenBits != -1)
			colorDiff += abs(current->GreenBits - desired.GreenBits);
		if (desired.BlueBits != -1)
			colorDiff += abs(current->BlueBits - desired.BlueBits);

		const uint32_t sizeDiff = abs((current->Width - desired.Width) *
			(current->Width - desired.Width) +
			(current->Height - desired.Height) *
			(current->Height - desired.Height));

		if (desired.RefreshRate != -1)
			rateDiff = abs(current->RefreshRate - desired.RefreshRate);
		else
			rateDiff = UINT_MAX - current->RefreshRate;

		if ((colorDiff < leastColorDiff) ||
			(colorDiff == leastColorDiff && sizeDiff < leastSizeDiff) ||
			(colorDiff == leastColorDiff && sizeDiff == leastSizeDiff && rateDiff < leastRateDiff))
		{
			closest = current;
			leastSizeDiff = sizeDiff;
			leastRateDiff = rateDiff;
			leastColorDiff = colorDiff;
		}
	}

	return closest;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a monitor connection or disconnection
void TRAP::INTERNAL::WindowingAPI::InputMonitor(Scope<InternalMonitor> monitor, const bool connected,
                                                const uint32_t placement)
{
	if (connected)
	{
		InternalMonitor* mon = nullptr;

		if (placement == 0)
		{
			s_Data.Monitors.insert(s_Data.Monitors.begin(), std::move(monitor));
			mon = s_Data.Monitors.front().get();
		}
		else
		{
			s_Data.Monitors.emplace_back(std::move(monitor));
			mon = s_Data.Monitors.back().get();
		}

		if(s_Data.Callbacks.Monitor)
			s_Data.Callbacks.Monitor(mon, connected);

		ImGuiWindowing::MonitorCallback(mon, true);
	}
	else
	{
		if (s_Data.Callbacks.Monitor)
			s_Data.Callbacks.Monitor(monitor.get(), connected);

		for (uint32_t i = 0; i < s_Data.Monitors.size(); i++)
		{
			if (s_Data.Monitors[i] == monitor)
			{
				s_Data.Monitors.erase(s_Data.Monitors.begin() + i);
				break;
			}
		}

		ImGuiWindowing::MonitorCallback(monitor.get(), false);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a monitor disconnection
void TRAP::INTERNAL::WindowingAPI::InputMonitorDisconnect(const uint32_t monitorIndex, const uint32_t)
{
	Scope<InternalMonitor>& monitor = s_Data.Monitors[monitorIndex];

	if (s_Data.Callbacks.Monitor)
		s_Data.Callbacks.Monitor(monitor.get(), false);

	for (uint32_t i = 0; i < s_Data.Monitors.size(); i++)
	{
		if (s_Data.Monitors[i] == monitor)
		{
			s_Data.Monitors.erase(s_Data.Monitors.begin() + i);
			break;
		}
	}

	ImGuiWindowing::MonitorCallback(monitor.get(), false);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::HideWindowFromTaskbar(InternalWindow* window)
{
	TRAP_ASSERT(window, " Window is nullptr!");

	PlatformHideWindowFromTaskbar(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetDragAndDrop(InternalWindow* window, const bool value)
{
	TRAP_ASSERT(window, " Window is nullptr!");

	PlatformSetDragAndDrop(window, value);
}