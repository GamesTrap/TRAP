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

#include <limits>
#include <thread>

#include "Core/PlatformDetection.h"
#include "WindowingAPI.h"
#include "Window.h"
#include "Events/KeyEvent.h"
#include "Layers/ImGui/ImGuiWindowing.h"
#include "Application.h"

TRAP::INTERNAL::WindowingAPI::Data TRAP::INTERNAL::WindowingAPI::s_Data{};

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::Init()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::Init(): must only be called from main thread");

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::Shutdown(): must only be called from main thread");

	if (!s_Data.Initialized)
		return;

	s_Data.Callbacks = {};

	while(!s_Data.WindowList.empty())
		DestroyWindow(s_Data.WindowList.front().get());

	while(!s_Data.CursorList.empty())
		DestroyCursor(s_Data.CursorList.front().get());

	s_Data.Monitors.clear();

	PlatformShutdown();

	s_Data.Initialized = false;

	s_Data = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyWindow(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::DestroyWindow(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if (window == nullptr)
		return;

	//Clear all callbacks to avoid exposing a half torn-down window object
	window->Callbacks = {};

	PlatformDestroyWindow(window);

	//Unlink window from global linked list
	s_Data.WindowList.remove_if([window](const Scope<InternalWindow>& winOwner)
	{
		return winOwner.get() == window;
	});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DefaultWindowHints()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::DefaultWindowHints(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::WindowHint(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	switch(hint)
	{
	case Hint::Resizable:
		s_Data.Hints.Window.Resizable = value;
		break;

	case Hint::Maximized:
		s_Data.Hints.Window.Maximized = value;
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
		[[fallthrough]];
	default:
		InputError(Error::Invalid_Enum, " Invalid window hint!");
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::GetMonitorName(const InternalMonitor* const monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetMonitorName(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return {};
	}

	return monitor->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::INTERNAL::WindowingAPI::InternalMonitor* TRAP::INTERNAL::WindowingAPI::GetPrimaryMonitor()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetPrimaryMonitor(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return nullptr;
	}

	return s_Data.Monitors[0].get();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::INTERNAL::WindowingAPI::InternalMonitor*> TRAP::INTERNAL::WindowingAPI::GetMonitors()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetMonitors(): must only be called from main thread");
	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return {};
	}

	std::vector<InternalMonitor*> monitors{};

	for (const Scope<InternalMonitor>& monitor : s_Data.Monitors)
		monitors.push_back(monitor.get());

	return monitors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::INTERNAL::WindowingAPI::InternalVideoMode TRAP::INTERNAL::WindowingAPI::GetVideoMode(InternalMonitor* const monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetVideoMode(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return {};
	}

	monitor->CurrentMode = PlatformGetVideoMode(monitor);
	return monitor->CurrentMode;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::INTERNAL::WindowingAPI::GetVideoModes(InternalMonitor* const monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetVideoModes(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return {};
	}

	if (!monitor)
		return std::vector<InternalVideoMode>{};
	if (!RefreshVideoModes(monitor))
		return std::vector<InternalVideoMode>{};

	return monitor->Modes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::INTERNAL::WindowingAPI::InternalWindow* TRAP::INTERNAL::WindowingAPI::CreateWindow(const uint32_t width,
	                                                                                                   const uint32_t height,
	                                                                                                   const std::string title,
	                                                                                                   InternalMonitor* const monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::CreateWindow(): must only be called from main thread");
	TRAP_ASSERT(!title.empty(), "WindowingAPI::CreateWindow(): Empty title provided!");
	TRAP_ASSERT(width > 0, "WindowingAPI::CreateWindow(): Invalid width provided!");
	TRAP_ASSERT(height > 0, "WindowingAPI::CreateWindow(): Invalid height provided!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return nullptr;
	}

	if(width <= 0 || height <= 0)
	{
		InputError(Error::Invalid_Value, " Invalid window size: " + std::to_string(width) + "x" +
		           std::to_string(height));
		return nullptr;
	}

	WindowConfig WNDConfig = s_Data.Hints.Window;

	WNDConfig.Width = width;
	WNDConfig.Height = height;
	WNDConfig.Title = std::move(title);

	s_Data.WindowList.push_front(MakeScope<InternalWindow>());
	InternalWindow* window = s_Data.WindowList.front().get();

	window->videoMode.Width = static_cast<int32_t>(width);
	window->videoMode.Height = static_cast<int32_t>(height);
	window->videoMode.RedBits = 8;
	window->videoMode.GreenBits = 8;
	window->videoMode.BlueBits = 8;
	window->videoMode.RefreshRate = -1.0;

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
	if (!PlatformCreateWindow(window, WNDConfig))
	{
		DestroyWindow(window);
		return nullptr;
	}

	return window;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowShouldClose(InternalWindow* const window, const bool value)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowShouldClose(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowShouldClose(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	window->ShouldClose = value;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowTitle(InternalWindow* const window, const std::string& title)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowTitle(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowTitle(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformSetWindowTitle(window, title);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::GetMonitorContentScale(const InternalMonitor* const monitor, float& xScale,
                                                          float& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetMonitorContentScale(): must only be called from main thread");
	TRAP_ASSERT(monitor, "WindowingAPI::GetMonitorContentScale(): Monitor is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	xScale = 0.0f;
	yScale = 0.0f;

	PlatformGetMonitorContentScale(monitor, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

//Center the cursor in the content area of the specified window
void TRAP::INTERNAL::WindowingAPI::CenterCursorInContentArea(InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	int32_t width = 0, height = 0;

	PlatformGetWindowSize(window, width, height);
	PlatformSetCursorPos(window, static_cast<double>(width) / 2.0, static_cast<double>(height) / 2.0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::InputError(const Error code, const std::string& str)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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

void TRAP::INTERNAL::WindowingAPI::DestroyCursor(InternalCursor* cursor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::DestroyCursor(): must only be called from main thread");
	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	if (cursor == nullptr)
		return;

	//Make sure the cursor is not being used by any window
	for(const Scope<InternalWindow>& window : s_Data.WindowList)
	{
		if (window->Cursor == cursor)
			SetCursor(window.get(), nullptr);
	}

	PlatformDestroyCursor(cursor);

	//Unlink window from global linked list
	s_Data.CursorList.remove_if([cursor](const Scope<InternalCursor>& cursorOwner)
	{
		return cursorOwner.get() == cursor;
	});
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a custom cursor.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::InternalCursor* TRAP::INTERNAL::WindowingAPI::CreateCursor(const Image* const image,
	                                                                                                   const int32_t xHotspot,
	                                                                                                   const int32_t yHotspot)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::CreateCursor(): must only be called from main thread");

	if(!image)
		return nullptr;

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return nullptr;
	}

	if(image->GetWidth() <= 0 || image->GetHeight() <= 0)
	{
		InputError(Error::Invalid_Value, "[Cursor] Invalid image dimensions for cursor!");
		return nullptr;
	}

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

		s_Data.CursorList.push_front(MakeScope<InternalCursor>());
		InternalCursor* cursor = s_Data.CursorList.front().get();

		if(!PlatformCreateCursor(cursor, iconImage.get(), xHotspot, yHotspot))
		{
			DestroyCursor(cursor);
			return nullptr;
		}

		return cursor;
	}
	if (image->GetColorFormat() == Image::ColorFormat::RGBA)
	{
		s_Data.CursorList.push_front(MakeScope<InternalCursor>());
		InternalCursor* cursor = s_Data.CursorList.front().get();

		if (!PlatformCreateCursor(cursor, image, xHotspot, yHotspot))
		{
			DestroyCursor(cursor);
			return nullptr;
		}

		return cursor;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::INTERNAL::WindowingAPI::InternalCursor* TRAP::INTERNAL::WindowingAPI::CreateStandardCursor(const CursorType& type)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::CreateStandardCursor(): must only be called from main thread");
	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return nullptr;
	}

	s_Data.CursorList.push_front(MakeScope<InternalCursor>());
	InternalCursor* cursor = s_Data.CursorList.front().get();

	if (!PlatformCreateStandardCursor(cursor, type))
	{
		DestroyCursor(cursor);
		return nullptr;
	}

	return cursor;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetCursor(InternalWindow* const window, InternalCursor* const cursor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetCursor(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetCursor(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Cursor = cursor;

	PlatformSetCursor(window, cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowIcon(InternalWindow* const window, const Image* const image)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowIcon(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowIcon(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if(!image)
	{
		PlatformSetWindowIcon(window, nullptr);
		return;
	}

	if(image->GetWidth() <= 0 || image->GetHeight() <= 0)
	{
		InputError(Error::Invalid_Value, "[Window] Invalid image dimensions for window icon!");
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
		PlatformSetWindowIcon(window, imageRGBA.get());
	}
	else if (image->GetColorFormat() == Image::ColorFormat::RGBA)
		PlatformSetWindowIcon(window, image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowPos(const InternalWindow* const window, const int32_t xPos, const int32_t yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowPos(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowPos(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if (window->Monitor)
		return;

	PlatformSetWindowPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the position of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowPos(const InternalWindow* const window, int32_t& xPos, int32_t& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetWindowPos(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetWindowPos(): Window is nullptr!");

	xPos = 0;
	yPos = 0;

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformGetWindowPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the size of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowSize(InternalWindow* const window, const int32_t width, const int32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowSize(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowSize(): Window is nullptr!");
	TRAP_ASSERT(width > 0, "WindowingAPI::SetWindowSize(): Width is smaller than or equal to 0!");
	TRAP_ASSERT(height > 0, "WindowingAPI::SetWindowSize(): Height is smaller than or equal to 0!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	window->videoMode.Width = width;
	window->videoMode.Height = height;

	PlatformSetWindowSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the size of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowSize(const InternalWindow* const window, int32_t& width, int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetWindowSize(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetWindowSize(): Window is nullptr!");

	width = 0;
	height = 0;

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformGetWindowSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the size of the framebuffer of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetFrameBufferSize(const InternalWindow* const window, int32_t& width, int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetFrameBufferSize(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetFrameBufferSize(): Window is nullptr!");

	width = 0;
	height = 0;

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformGetFrameBufferSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the opacity of the whole window.
void TRAP::INTERNAL::WindowingAPI::SetWindowOpacity(const InternalWindow* const window, const float opacity)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowOpacity(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowOpacity(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if(opacity < 0.0f || opacity > 1.0f)
	{
		InputError(Error::Invalid_Value, " Invalid window opacity: " + std::to_string(opacity));
		return;
	}

	PlatformSetWindowOpacity(window, opacity);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the opacity of the whole window.
[[nodiscard]] float TRAP::INTERNAL::WindowingAPI::GetWindowOpacity(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetWindowOpacity(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetWindowOpacity(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return 1.0f;
	}

	return PlatformGetWindowOpacity(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the content scale for the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowContentScale(const InternalWindow* window, float& xScale, float& yScale)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetWindowContentScale(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetWindowContentScale(): Window is nullptr!");

	xScale = 0.0f;
	yScale = 0.0f;

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformGetWindowContentScale(window, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets an attribute for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowHint(InternalWindow* const window, const Hint hint, const bool value)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowHint(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowHint(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

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
		[[fallthrough]];
	case Hint::Minimized:
		[[fallthrough]];
	case Hint::Visible:
		[[fallthrough]];
	case Hint::Focused:
		[[fallthrough]];
	case Hint::Hovered:
		[[fallthrough]];
	default:
		InputError(Error::Invalid_Enum, " Invalid window attribute provided!");
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns an attribute of the specified window.
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::GetWindowHint(const InternalWindow* const window, const Hint hint)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetWindowHint(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetWindowHint(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return false;
	}

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
void TRAP::INTERNAL::WindowingAPI::SetWindowMonitor(InternalWindow* const window,
                                                    InternalMonitor* const monitor,
                                                    const int32_t xPos,
                                                    const int32_t yPos,
                                                    const int32_t width,
                                                    const int32_t height,
                                                    const double refreshRate)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowMonitor(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowMonitor(): Window is nullptr!");
	TRAP_ASSERT(width > 0, "WindowingAPI::SetWindowMonitor(): Width is smaller than or equal to 0!");
	TRAP_ASSERT(height > 0, "WindowingAPI::SetWindowMonitor(): Height is smaller than or equal to 0!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if (width <= 0 || height <= 0)
	{
		InputError(Error::Invalid_Value, " Invalid window size " + std::to_string(width) + "x" +
		           std::to_string(height));
		return;
	}

	if (refreshRate < 0.0 && refreshRate != -1.0)
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
void TRAP::INTERNAL::WindowingAPI::SetWindowMonitorBorderless(InternalWindow* const window, InternalMonitor* const monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(window, "WindowingAPI::SetWindowMonitorBorderless(): Window is nullptr!");
	TRAP_ASSERT(monitor, "WindowingAPI::SetWindowMonitorBorderless(): Monitor is nullptr!");

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
void TRAP::INTERNAL::WindowingAPI::SetWindowUserPointer(InternalWindow* const window, void* const ptr)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::SetWindowUserPointer(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	window->UserPointer = ptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the user pointer of the specified window.
[[nodiscard]] void* TRAP::INTERNAL::WindowingAPI::GetWindowUserPointer(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetWindowUserPointer(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return nullptr;
	}

	return window->UserPointer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetMonitorCallback(const MonitorFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetMonitorCallback(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	s_Data.Callbacks.Monitor = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the position callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowPosCallback(InternalWindow* const window, const WindowPositionFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowPosCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowPosCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Pos = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the size callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowSizeCallback(InternalWindow* const window, const WindowSizeFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowSizeCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowSizeCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Size = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the minimize callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowMinimizeCallback(InternalWindow* const window,
                                                             const WindowMinimizeFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowMinimizeCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowMinimizeCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Minimize = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the maximize callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowMaximizeCallback(InternalWindow* const window,
                                                             const WindowMaximizeFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowMaximizeCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowMaximizeCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Maximize = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the close callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowCloseCallback(InternalWindow* const window, const WindowCloseFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowCloseCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowCloseCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Close = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the focus callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowFocusCallback(InternalWindow* const window, const WindowFocusFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowFocusCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowFocusCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Focus = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the framebuffer resize callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetFrameBufferSizeCallback(InternalWindow* const window,
                                                              const FrameBufferSizeFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetFrameBufferSizeCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetFrameBufferSizeCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.FBSize = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the window content scale callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetContentScaleCallback(InternalWindow* window,
                                                           const WindowContentScaleFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::SetContentScaleCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Scale = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the key callback.
void TRAP::INTERNAL::WindowingAPI::SetKeyCallback(InternalWindow* const window, const KeyFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetKeyCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetKeyCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Key = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the Unicode character callback.
void TRAP::INTERNAL::WindowingAPI::SetCharCallback(InternalWindow* const window, const CharFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetCharCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetCharCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Character = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the mouse button callback.
void TRAP::INTERNAL::WindowingAPI::SetMouseButtonCallback(InternalWindow* const window, const MouseButtonFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetMouseButtonCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetMouseButtonCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.MouseButton = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor position callback.
void TRAP::INTERNAL::WindowingAPI::SetCursorPosCallback(InternalWindow* const window, const CursorPositionFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetCursorPosCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetCursorPosCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.CursorPos = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor enter callback.
void TRAP::INTERNAL::WindowingAPI::SetCursorEnterCallback(InternalWindow* const window, const CursorEnterFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetCursorEnterCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetCursorEnterCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.CursorEnter = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the scroll callback.
void TRAP::INTERNAL::WindowingAPI::SetScrollCallback(InternalWindow* const window, const ScrollFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetScrollCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetScrollCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Scroll = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the path drop callback.
void TRAP::INTERNAL::WindowingAPI::SetDropCallback(InternalWindow* const window, const DropFunc callback)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetDropCallback(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetDropCallback(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	window->Callbacks.Drop = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::INTERNAL::WindowingAPI::MonitorFunc TRAP::INTERNAL::WindowingAPI::GetMonitorCallback()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Data.Callbacks.Monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the position callback for the specified window.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::WindowPositionFunc TRAP::INTERNAL::WindowingAPI::GetWindowPosCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetWindowPosCallback(): Window is nullptr!");

	return window->Callbacks.Pos;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the size callback for the specified window.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::WindowSizeFunc TRAP::INTERNAL::WindowingAPI::GetWindowSizeCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetWindowSizeCallback(): Window is nullptr!");

	return window->Callbacks.Size;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the close callback for the specified window.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::WindowCloseFunc TRAP::INTERNAL::WindowingAPI::GetWindowCloseCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetWindowCloseCallback(): Window is nullptr!");

	return window->Callbacks.Close;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the focus callback for the specified window.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::WindowFocusFunc TRAP::INTERNAL::WindowingAPI::GetWindowFocusCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetWindowFocusCallback(): Window is nullptr!");

	return window->Callbacks.Focus;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the framebuffer resize callback for the specified window.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::FrameBufferSizeFunc TRAP::INTERNAL::WindowingAPI::GetFrameBufferSizeCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetFrameBufferSizeCallback(): Window is nullptr!");

	return window->Callbacks.FBSize;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the window content scale callback for the specified window.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::WindowContentScaleFunc TRAP::INTERNAL::WindowingAPI::GetWindowContentScaleCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetWindowContentScaleCallback(): Window is nullptr!");

	return window->Callbacks.Scale;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the key callback.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::KeyFunc TRAP::INTERNAL::WindowingAPI::GetKeyCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetKeyCallback(): Window is nullptr!");

	return window->Callbacks.Key;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the Unicode character callback.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::CharFunc TRAP::INTERNAL::WindowingAPI::GetCharCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetCharCallback(): Window is nullptr!");

	return window->Callbacks.Character;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the mouse button callback.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::MouseButtonFunc TRAP::INTERNAL::WindowingAPI::GetMouseButtonCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetMouseButtonCallback(): Window is nullptr!");

	return window->Callbacks.MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the cursor position callback.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::CursorPositionFunc TRAP::INTERNAL::WindowingAPI::GetCursorPosCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetCursorPosCallback(): Window is nullptr!");

	return window->Callbacks.CursorPos;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the cursor enter callback.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::CursorEnterFunc TRAP::INTERNAL::WindowingAPI::GetCursorEnterCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetCursorEnterCallback(): Window is nullptr!");

	return window->Callbacks.CursorEnter;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the scroll callback.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::ScrollFunc TRAP::INTERNAL::WindowingAPI::GetScrollCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetScrollCallback(): Window is nullptr!");

	return window->Callbacks.Scroll;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the path drop callback.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::DropFunc TRAP::INTERNAL::WindowingAPI::GetDropCallback(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "WindowingAPI::GetDropCallback(): Window is nullptr!");

	return window->Callbacks.Drop;
}

//-------------------------------------------------------------------------------------------------------------------//

//Processes all pending events.
void TRAP::INTERNAL::WindowingAPI::PollEvents()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::PollEvents(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformPollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::WaitEvents(const double timeout)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	TRAP_ASSERT(timeout >= 0.0, "WindowingAPI::WaitEvents(): Timeout must be positive");
	TRAP_ASSERT(timeout <= std::numeric_limits<double>::max(), "WindowingAPI::WaitEvents(): Timeout must be less than or equal to DBL_MAX");

	if(timeout < 0.0 || timeout > std::numeric_limits<double>::max())
	{
		InputError(Error::Invalid_Value, "[Window] Timeout must be positive");
		return;
	}

	PlatformWaitEvents(timeout);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::PostEmptyEvent()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformPostEmptyEvent();
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor mode for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetCursorMode(InternalWindow* const window, const CursorMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetCursorMode(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetCursorMode(): Window is nullptr!");

	if (window->cursorMode == mode)
		return;

	window->cursorMode = mode;

	PlatformGetCursorPos(window, window->VirtualCursorPosX, window->VirtualCursorPosY);
	PlatformSetCursorMode(window, mode);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the cursor mode for the specified window.
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::CursorMode TRAP::INTERNAL::WindowingAPI::GetCursorMode(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetCursorMode(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetCursorMode(): Window is nullptr!");

	return window->cursorMode;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether raw mouse motion is supported.
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::RawMouseMotionSupported()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::RawMouseMotionSupported(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return false;
	}

	return PlatformRawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetRawMouseMotionMode(InternalWindow* const window, const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetRawMouseMotionMode(): must only be called from main thread");

	if (!PlatformRawMouseMotionSupported())
	{
		InputError(Error::Platform_Error, "[Window] Raw mouse motion is not supported on this system");
		return;
	}

	if (window->RawMouseMotion == enabled)
		return;

	window->RawMouseMotion = enabled;
	PlatformSetRawMouseMotion(window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrvieves the raw mouse motion mode for the specified window.
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::GetRawMouseMotionMode(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetRawMouseMotionMode(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetRawMouseMotionMode(): Window is nullptr!");

	return window->RawMouseMotion;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetProgress(const InternalWindow* const window, const ProgressState state,
											   const uint32_t progress)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(window, "WindowingAPI::SetProgress(): Window is nullptr!");
	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetProgress(): must only be called from main thread");
	TRAP_ASSERT(progress <= 100, "WindowingAPI::SetProgress(): Progress must be between 0 and 100");

	PlatformSetProgress(window, state, progress);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the layout-specific name of the specified printable key.
[[nodiscard]] const char* TRAP::INTERNAL::WindowingAPI::GetKeyName(const Input::Key key, int32_t scanCode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetKeyName(): must only be called from main thread");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return nullptr;
	}

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
[[nodiscard]] TRAP::Input::KeyState TRAP::INTERNAL::WindowingAPI::GetKey(const InternalWindow* const window, const Input::Key key)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetKey(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetKey(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return Input::KeyState::Released;
	}

	if (key < Input::Key::Space || key > Input::Key::Menu)
	{
		InputError(Error::Invalid_Enum, " Invalid key: " + std::to_string(static_cast<int32_t>(key)));
		return Input::KeyState::Released;
	}

	return window->Keys[static_cast<uint32_t>(key)];
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the last reported state of a mouse button for the specified window.
[[nodiscard]] TRAP::Input::KeyState TRAP::INTERNAL::WindowingAPI::GetMouseButton(const InternalWindow* const window, const Input::MouseButton button)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetMouseButton(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetMouseButton(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return Input::KeyState::Released;
	}

	return window->MouseButtons[static_cast<uint32_t>(button)];
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the position of the cursor, relative to the content area of the window
void TRAP::INTERNAL::WindowingAPI::SetCursorPos(InternalWindow* const window, const double xPos, const double yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetCursorPos(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetCursorPos(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

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
void TRAP::INTERNAL::WindowingAPI::GetCursorPos(const InternalWindow* const window, double& xPos, double& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetCursorPos(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::GetCursorPos(): Window is nullptr!");

	xPos = 0.0;
	yPos = 0.0;

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

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
void TRAP::INTERNAL::WindowingAPI::GetMonitorPos(const InternalMonitor* const monitor, int32_t& xPos, int32_t& yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetMonitorPos(): must only be called from main thread");
	TRAP_ASSERT(monitor, "WindowingAPI::GetMonitorPos(): Monitor is nullptr!");

	xPos = 0;
	yPos = 0;

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	PlatformGetMonitorPos(monitor, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::GetMonitorWorkArea(const InternalMonitor* const monitor, int32_t& xPos, int32_t& yPos,
                                                      int32_t& width, int32_t& height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetMonitorWorkArea(): must only be called from main thread");
	TRAP_ASSERT(monitor, "WindowingAPI::GetMonitorWorkArea(): Monitor is nullptr!");

	xPos = 0;
	yPos = 0;
	width = 0;
	height = 0;

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	PlatformGetMonitorWorkArea(monitor, xPos, yPos, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Makes the specified window visible.
void TRAP::INTERNAL::WindowingAPI::ShowWindow(InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::ShowWindow(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::ShowWindow(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if (window->Monitor)
		return;

	PlatformShowWindow(window);

	if (window->FocusOnShow)
		PlatformFocusWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Brings the specified window to front and sets input focus.
void TRAP::INTERNAL::WindowingAPI::FocusWindow(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::FocusWindow(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::FocusWindow(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformFocusWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Maximizes the specified window.
void TRAP::INTERNAL::WindowingAPI::MaximizeWindow(InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::MaximizeWindow(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::MaximizeWindow(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if (window->Monitor || !window->Resizable)
		return;

	PlatformMaximizeWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Minimizes the specified window.
void TRAP::INTERNAL::WindowingAPI::MinimizeWindow(const InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::MinimizeWindow(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::MinimizeWindow(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformMinimizeWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Requests user attention to the specified window.
void TRAP::INTERNAL::WindowingAPI::RequestWindowAttention(InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::RequestWindowAttention(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::RequestWindowAttention(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformRequestWindowAttention(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Hides the specified window.
void TRAP::INTERNAL::WindowingAPI::HideWindow(InternalWindow* window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::HideWindow(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::HideWindow(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if (window->Monitor)
		return;

	PlatformHideWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::RestoreWindow(InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::RestoreWindow(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::RestoreWindow(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	PlatformRestoreWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowSizeLimits(InternalWindow* const window,
                                                       const int32_t minWidth,
                                                       const int32_t minHeight,
                                                       const int32_t maxWidth,
                                                       const int32_t maxHeight)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowSizeLimits(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowSizeLimits(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized");
		return;
	}

	if(minWidth != -1 && minHeight != -1 && (minWidth < 0 || minHeight < 0))
	{
		InputError(Error::Invalid_Value, " Invalid minimum window size!");
		return;
	}

	if((maxWidth != -1 || maxHeight != -1) &&
	   (maxWidth < 0 || maxHeight < 0 || maxWidth < minWidth || maxHeight < minHeight))
	{
		InputError(Error::Invalid_Value, " Invalid maximum window size!");
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

void TRAP::INTERNAL::WindowingAPI::SetWindowAspectRatio(InternalWindow* window, const int32_t numerator,
                                                        const int32_t denominator)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetWindowAspectRatio(): must only be called from main thread");
	TRAP_ASSERT(window, "WindowingAPI::SetWindowAspectRatio(): Window is nullptr!");
	TRAP_ASSERT(numerator > 0 && denominator > 0, "WindowingAPI::SetWindowAspectRatio(): Invalid window aspect ratio!");

	if(numerator != -1 && denominator != -1)
	{
		if(numerator < 0 || denominator < 0)
		{
			InputError(Error::Invalid_Value, " Invalid window aspect ratio " +
			           std::to_string(numerator) + ":" + std::to_string(denominator) + "!");
			return;
		}
	}

	window->Numerator = numerator;
	window->Denominator = denominator;

	if(window->Monitor || !window->Resizable)
		return;

	PlatformSetWindowAspectRatio(window, numerator, denominator);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the clipboard to the specified string.
void TRAP::INTERNAL::WindowingAPI::SetClipboardString(const std::string& string)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::SetClipboardString(): must only be called from main thread");
	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return;
	}

	PlatformSetClipboardString(string);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the contents of the clipboard as a string.
[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::GetClipboardString()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(std::this_thread::get_id() == TRAP::Application::GetMainThreadID(),
	            "WindowingAPI::GetClipboardString(): must only be called from main thread");
	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return "";
	}

	return PlatformGetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the Vulkan loader and an ICD have been found.
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::VulkanSupported()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	// if(!s_Data.Initialized)
	// {
	// 	InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
	// 	return false;
	// }

	return InitVulkan(1);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the Vulkan instance extensions required by TRAP.
[[nodiscard]] std::array<std::string, 2> TRAP::INTERNAL::WindowingAPI::GetRequiredInstanceExtensions()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	// if(!s_Data.Initialized)
	// {
	// 	InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
	// 	return {};
	// }

	if (!InitVulkan(2))
		return {};

	if (s_Data.VK.Extensions[0].empty() || s_Data.VK.Extensions[1].empty())
		return {};

	return s_Data.VK.Extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a Vulkan surface for the specified window.
[[nodiscard]] VkResult TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(VkInstance instance,
                                                                         const InternalWindow* const window,
                                                                         const VkAllocationCallbacks* const allocator,
                                                                         VkSurfaceKHR& surface)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(instance, "WindowingAPI::CreateWindowSurface(): Instance is nullptr!");
	TRAP_ASSERT(window, "WindowingAPI::CreateWindowSurface(): Window is nullptr!");

	if(!s_Data.Initialized)
	{
		InputError(Error::Not_Initialized, "[Window] WindowingAPI is not initialized!");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (!InitVulkan(2))
		return VK_ERROR_INITIALIZATION_FAILED;

	if (s_Data.VK.Extensions[0].empty() || s_Data.VK.Extensions[1].empty())
	{
		InputError(Error::API_Unavailable, "[Vulkan] Window surface creation extensions not found");
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	return PlatformCreateWindowSurface(instance, window, allocator, surface);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::INTERNAL::WindowingAPI::GetVulkanResultString(const VkResult result)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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

	case VK_SUBOPTIMAL_KHR:
		return "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully";

	case VK_THREAD_IDLE_KHR:
		return "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call";

	case VK_THREAD_DONE_KHR:
		return "A deferred operation is not complete but there is no work remaining to assign to additional threads";

	case VK_OPERATION_DEFERRED_KHR:
		return "A deferred operation was requested and at least some of the work was deferred";

	case VK_OPERATION_NOT_DEFERRED_KHR:
		return "A deferred operation was requested and no work was deferred";

	case VK_PIPELINE_COMPILE_REQUIRED_EXT:
		return "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed";

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
		return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons";

	case VK_ERROR_TOO_MANY_OBJECTS:
		return "Too many objects of the type have already been created";

	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		return "A requested format is not supported on this device";

	case VK_ERROR_FRAGMENTED_POOL:
		return "A pool allocation has failed due to fragmentation of the pool's memory";

	case VK_ERROR_SURFACE_LOST_KHR:
		return "A surface is no longer available";

	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";

	case VK_ERROR_OUT_OF_DATE_KHR:
		return "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail";

	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		return "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image";

	case VK_ERROR_INVALID_SHADER_NV:
		return "One or more shaders failed to compile or link";

	case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
		return "A pool memory allocation has failed";

	case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
		return "A synchronization primitive returned an invalid external handle";

	case VK_ERROR_FRAGMENTATION_EXT:
		return "A descriptor pool creation has failed due to fragmentation";

	case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
		return "A buffer creation failed because the requested address is not available";

	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
		return "A swapchain created with the full screen exclusive mode cannot be used to create another full screen exclusive swapchain";

	default:
		return "Unknown Vulkan error";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a cursor motion event
//The position is specified in content area relative screen coordinates
void TRAP::INTERNAL::WindowingAPI::InputCursorPos(InternalWindow* const window, const double xPos, const double yPos)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputCursorPos(): Window is nullptr!");
	TRAP_ASSERT(xPos > -std::numeric_limits<float>::max(), "WindowingAPI::InputCursorPos(): xPos is NaN!");
	TRAP_ASSERT(xPos <  std::numeric_limits<float>::max(), "WindowingAPI::InputCursorPos(): xPos is NaN!");
	TRAP_ASSERT(yPos > -std::numeric_limits<float>::max(), "WindowingAPI::InputCursorPos(): yPos is NaN!");
	TRAP_ASSERT(yPos <  std::numeric_limits<float>::max(), "WindowingAPI::InputCursorPos(): yPos is NaN!");

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
void TRAP::INTERNAL::WindowingAPI::InputKey(InternalWindow* const window, Input::Key key, const int32_t,
                                            Input::KeyState state)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputKey(): Window is nullptr!");
	TRAP_ASSERT(key != Input::Key::Unknown, "WindowingAPI::InputKey(): Key is unknown!");
	TRAP_ASSERT(state == Input::KeyState::Pressed || state == Input::KeyState::Released, "WindowingAPI::InputKey(): KeyState is invalid!");

	if(key != Input::Key::Unknown)
	{
		bool repeated = false;

		if(state == Input::KeyState::Released && window->Keys[static_cast<uint32_t>(key)] == Input::KeyState::Released)
			return;

		if(state == Input::KeyState::Pressed && window->Keys[static_cast<uint32_t>(key)] == Input::KeyState::Pressed)
			repeated = true;

		window->Keys[static_cast<uint32_t>(key)] = state;

		if(repeated)
			state = Input::KeyState::Repeat;
	}

	if (window->Callbacks.Key)
		window->Callbacks.Key(window, key, state);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a Unicode codepoint input event
//The 'plain' parameter determines whether to emit a regular character event
void TRAP::INTERNAL::WindowingAPI::InputChar(const InternalWindow* const window, const uint32_t codePoint)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputChar(): Window is nullptr!");

	if (codePoint < 32 || (codePoint > 126 && codePoint < 160))
		return;

	if (window->Callbacks.Character)
		window->Callbacks.Character(window, codePoint);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a mouse button click event
void TRAP::INTERNAL::WindowingAPI::InputMouseClick(InternalWindow* const window, const Input::MouseButton button,
                                                   const Input::KeyState state)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputMouseClick(): Window is nullptr!");
	TRAP_ASSERT(static_cast<int32_t>(button) >= 0, "WindowingAPI::InputMouseClick(): Button is invalid!");
	TRAP_ASSERT(state == Input::KeyState::Pressed || state == Input::KeyState::Released, "WindowingAPI::InputMouseClick(): KeyState is invalid!");

	if(static_cast<int32_t>(button) < 0 || button > Input::MouseButton::Eight)
		return;

	window->MouseButtons[static_cast<uint32_t>(button)] = state;

	if (window->Callbacks.MouseButton)
		window->Callbacks.MouseButton(window, button, state);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a scroll event
void TRAP::INTERNAL::WindowingAPI::InputScroll(const InternalWindow* const window, const double xOffset,
                                               const double yOffset)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputScroll(): Window is nullptr!");
	TRAP_ASSERT(xOffset > -std::numeric_limits<float>::max(), "WindowingAPI::InputScroll(): xOffset is NaN!");
	TRAP_ASSERT(xOffset <  std::numeric_limits<float>::max(), "WindowingAPI::InputScroll(): xOffset is NaN!");
	TRAP_ASSERT(yOffset > -std::numeric_limits<float>::max(), "WindowingAPI::InputScroll(): yOffset is NaN!");
	TRAP_ASSERT(yOffset <  std::numeric_limits<float>::max(), "WindowingAPI::InputScroll(): yOffset is NaN!");

	if (window->Callbacks.Scroll)
		window->Callbacks.Scroll(window, xOffset, yOffset);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a cursor enter/leave event
void TRAP::INTERNAL::WindowingAPI::InputCursorEnter(const InternalWindow* const window, const bool entered)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputCursorEnter(): Window is nullptr!");

	if (window->Callbacks.CursorEnter)
		window->Callbacks.CursorEnter(window, entered);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window framebuffer has been resized
//The size is specified in pixels
void TRAP::INTERNAL::WindowingAPI::InputFrameBufferSize(const InternalWindow* const window, const int32_t width,
                                                        const int32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputFrameBufferSize(): Window is nullptr!");
	TRAP_ASSERT(width >= 0, "WindowingAPI::InputFrameBufferSize(): Width is invalid!");
	TRAP_ASSERT(height >= 0, "WindowingAPI::InputFrameBufferSize(): Height is invalid!");

	if (window->Callbacks.FBSize)
		window->Callbacks.FBSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been resized
//The size is specified in screen coordinates
void TRAP::INTERNAL::WindowingAPI::InputWindowSize(const InternalWindow* const window, const int32_t width,
                                                   const int32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputWindowSize(): Window is nullptr!");
	TRAP_ASSERT(width >= 0, "WindowingAPI::InputWindowSize(): Width is invalid!");
	TRAP_ASSERT(height >= 0, "WindowingAPI::InputWindowSize(): Height is invalid!");

	if (window->Callbacks.Size)
		window->Callbacks.Size(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been minimized
void TRAP::INTERNAL::WindowingAPI::InputWindowMinimize(const InternalWindow* const window, const bool restored)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputWindowMinimize(): Window is nullptr!");

	if (window->Callbacks.Minimize)
		window->Callbacks.Minimize(window, !restored);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been maximized
void TRAP::INTERNAL::WindowingAPI::InputWindowMaximize(const InternalWindow* const window, const bool restored)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputWindowMaximize(): Window is nullptr!");

	if (window->Callbacks.Maximize)
		window->Callbacks.Maximize(window, !restored);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has moved
//The position is specified in content area relative screen coordinates
void TRAP::INTERNAL::WindowingAPI::InputWindowPos(const InternalWindow* const window, const int32_t x, const int32_t y)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputWindowPos(): Window is nullptr!");

	if (window->Callbacks.Pos)
		window->Callbacks.Pos(window, x, y);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that the user wishes to close a window
void TRAP::INTERNAL::WindowingAPI::InputWindowCloseRequest(InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputWindowCloseRequest(): Window is nullptr!");

	window->ShouldClose = true;

	if (window->Callbacks.Close)
		window->Callbacks.Close(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of files or directories dropped on a window
void TRAP::INTERNAL::WindowingAPI::InputDrop(const InternalWindow* const window, const std::vector<std::string>& paths)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputDrop(): Window is nullptr!");

	if (window->Callbacks.Drop)
		window->Callbacks.Drop(window, paths);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has lost or received input focus
void TRAP::INTERNAL::WindowingAPI::InputWindowFocus(InternalWindow* const window, const bool focused)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window != nullptr, "WindowingAPI::InputWindowFocus(): Window is nullptr!");

	if (window->Callbacks.Focus)
		window->Callbacks.Focus(window, focused);

	if(focused)
		return;

	for (uint32_t key = 0; key <= static_cast<uint32_t>(Input::Key::Menu); key++)
	{
		if (window->Keys[key] == Input::KeyState::Pressed)
		{
			const int32_t scanCode = PlatformGetKeyScanCode(static_cast<Input::Key>(key));
			InputKey(window, static_cast<Input::Key>(key), scanCode, Input::KeyState::Released);
		}
	}

	for (uint32_t button = 0; button <= static_cast<uint32_t>(Input::MouseButton::Eight); button++)
	{
		if (window->MouseButtons[button] == Input::KeyState::Pressed)
			InputMouseClick(window, static_cast<Input::MouseButton>(button), Input::KeyState::Pressed);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a the keyboard layout has changed
void TRAP::INTERNAL::WindowingAPI::InputKeyboardLayout()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(!TRAP::Input::GetEventCallback())
		return;

	//This function is not window specific because you can only have 1 keyboard layout selected and not multiple!
	//So apply this globally
	//This event gets redirected to the TRAP::Input callback
	TRAP::Events::KeyLayoutEvent event(TRAP::Input::GetKeyboardLayoutName());
	TRAP::Input::GetEventCallback()(event);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the available modes for the specified monitor
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::RefreshVideoModes(InternalMonitor* const monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	if (!monitor->Modes.empty())
		return true;

	std::vector<InternalVideoMode> modes = PlatformGetVideoModes(monitor);
	if (modes.empty())
		return false;

	std::sort(modes.begin(), modes.end(), CompareVideoModes);

	monitor->Modes = {};
	monitor->Modes = modes;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::InitVulkan(const uint32_t mode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	uint32_t count = 0;

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

//Lexically compare video modes
[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::CompareVideoModes(const InternalVideoMode& fm, const InternalVideoMode& sm)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const int32_t fbpp = fm.RedBits + fm.GreenBits + fm.BlueBits;
	const int32_t sbpp = sm.RedBits + sm.GreenBits + sm.BlueBits;
	const int32_t farea = fm.Width * fm.Height;
	const int32_t sarea = sm.Width * sm.Height;

	//First sort on color bits per pixel
	if (fbpp != sbpp)
		return fbpp < sbpp;

	//Then sort on screen area
	if (farea != sarea)
		return farea < sarea;

	//Then sort on width
	if (fm.Width != sm.Width)
		return fm.Width < sm.Width;

	//Lastly sort on refresh rate
	return fm.RefreshRate < sm.RefreshRate;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::WindowingAPI::IsSameVideoMode(const InternalVideoMode& fm, const InternalVideoMode& sm)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const int32_t fbpp = fm.RedBits + fm.GreenBits + fm.BlueBits;
	const int32_t sbpp = sm.RedBits + sm.GreenBits + sm.BlueBits;
	const int32_t farea = fm.Width * fm.Height;
	const int32_t sarea = sm.Width * sm.Height;

	return fbpp == sbpp && farea == sarea && fm.Width == sm.Width && fm.RefreshRate == sm.RefreshRate;
}

//-------------------------------------------------------------------------------------------------------------------//

//Splits a color depth into red, green and blue bit depths
void TRAP::INTERNAL::WindowingAPI::SplitBPP(int32_t bpp, int32_t& red, int32_t& green, int32_t& blue)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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
[[nodiscard]] TRAP::INTERNAL::WindowingAPI::InternalVideoMode* TRAP::INTERNAL::WindowingAPI::ChooseVideoMode(InternalMonitor* const monitor,
                                                                                                             const InternalVideoMode& desired)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	uint32_t leastSizeDiff = std::numeric_limits<uint32_t>::max();
	double rateDiff = 0.0, leastRateDiff = std::numeric_limits<double>::max();
	uint32_t leastColorDiff = std::numeric_limits<uint32_t>::max();
	InternalVideoMode* closest = nullptr;

	if (!RefreshVideoModes(monitor))
		return nullptr;

	for (auto& mode : monitor->Modes)
	{
		InternalVideoMode* const current = &mode;

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

		if (desired.RefreshRate != -1.0)
			rateDiff = abs(current->RefreshRate - desired.RefreshRate);
		else
			rateDiff = std::numeric_limits<double>::max() - current->RefreshRate;

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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(monitor != nullptr, "WindowingAPI::InputMonitor(): Monitor is nullptr!");

	if (connected)
	{
		const InternalMonitor* mon = nullptr;

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
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a monitor disconnection
void TRAP::INTERNAL::WindowingAPI::InputMonitorDisconnect(const uint32_t monitorIndex,
                                                          [[maybe_unused]] const uint32_t placement)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	const Scope<InternalMonitor>& monitor = s_Data.Monitors[monitorIndex];

	if (s_Data.Callbacks.Monitor)
		s_Data.Callbacks.Monitor(monitor.get(), false);

	//Remove monitor from monitors list
	s_Data.Monitors.erase(std::remove(s_Data.Monitors.begin(), s_Data.Monitors.end(), monitor), s_Data.Monitors.end());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::HideWindowFromTaskbar(InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(window, "WindowingAPI::HideWindowFromTaskbar(): Window is nullptr!");

	PlatformHideWindowFromTaskbar(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetDragAndDrop(InternalWindow* const window, const bool value)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::WindowingAPI);

	TRAP_ASSERT(window, "WindowingAPI::SetDragAndDrop(): Window is nullptr!");

	PlatformSetDragAndDrop(window, value);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window content scale has changed
//The scale is specified as the ratio between the current and default DPI
void TRAP::INTERNAL::WindowingAPI::InputWindowContentScale(const InternalWindow* window, const float xScale,
                                                           const float yScale)
{
	TRAP_ASSERT(window != nullptr);
	TRAP_ASSERT(xScale > 0.0f);
	TRAP_ASSERT(xScale < std::numeric_limits<float>::max());
	TRAP_ASSERT(yScale > 0.0f);
	TRAP_ASSERT(yScale < std::numeric_limits<float>::max());

	if (window->Callbacks.Scale)
		window->Callbacks.Scale(window, xScale, yScale);
}
