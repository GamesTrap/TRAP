#include "TRAPPCH.h"

#include "WindowingAPI.h"
#include "Window.h"
#include "Utils/String.h"

TRAP::INTERNAL::WindowingAPI::Data TRAP::INTERNAL::WindowingAPI::s_Data{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::WindowingError TRAP::INTERNAL::WindowingAPI::s_MainThreadError{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::ErrorFunc TRAP::INTERNAL::WindowingAPI::s_ErrorCallback = nullptr;

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

	if(!PlatformCreateMutex(s_Data.ErrorLock) || !PlatformCreateTLS(s_Data.ErrorSlot) || !PlatformCreateTLS(s_Data.ContextSlot))
	{
		Shutdown();
		return false;
	}

	PlatformSetTLS(s_Data.ErrorSlot, &s_MainThreadError);

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

	for (auto& Monitor : s_Data.Monitors)
		if (Monitor)
			Monitor.reset();

	s_Data.Monitors.clear();

	PlatformShutdown();

	s_Data.Initialized = false;

	PlatformDestroyTLS(s_Data.ContextSlot);
	PlatformDestroyTLS(s_Data.ErrorSlot);
	PlatformDestroyMutex(s_Data.ErrorLock);

	s_Data = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyWindow(Scope<InternalWindow> window)
{
	if (window == nullptr)
		return;

	//Clear all callbacks to avoid exposing a half torn-down window object
	window->Callbacks = {};

	//The window's context must not be current on another thread when the window is destroyed
	if (window.get() == static_cast<InternalWindow*>(PlatformGetTLS(s_Data.ContextSlot)))
		MakeContextCurrent(nullptr);

	PlatformDestroyWindow(window.get());

	//Unlink window from global linked list
	s_Data.WindowList.remove(window.get());

	window.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

//Makes the context of the specified window current for the calling
void TRAP::INTERNAL::WindowingAPI::MakeContextCurrent(InternalWindow* window)
{
	const auto previousPtr = static_cast<InternalWindow*>(PlatformGetTLS(s_Data.ContextSlot));

	if (window && window->context.Client == ContextAPI::None)
	{
		InputError(Error::No_Window_Context, " Cannot make current with a window that has no OpenGL context");
		return;
	}

	if (previousPtr)
	{
		if (!window)
			previousPtr->context.MakeCurrent(nullptr);
	}

	if (window)
		window->context.MakeCurrent(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DefaultWindowHints()
{
	//The default is OpenGL with minimum version 4.6
	s_Data.Hints.Context = {};
	s_Data.Hints.Context.Client = ContextAPI::OpenGL;

	//The default is a focused, visible and resizable window with decorations
	s_Data.Hints.Window = {};
	s_Data.Hints.Window.Resizable = true;
	s_Data.Hints.Window.Visible = true;
	s_Data.Hints.Window.Decorated = true;
	s_Data.Hints.Window.Focused = true;
	s_Data.Hints.Window.FocusOnShow = true;
	s_Data.Hints.Window.MousePassthrough = false;

	//The default is 24 bits of color, 24 bits of depth and 8 bits of stencil, double buffered
	s_Data.Hints.FrameBuffer = {};
	s_Data.Hints.FrameBuffer.RedBits = 8;
	s_Data.Hints.FrameBuffer.GreenBits = 8;
	s_Data.Hints.FrameBuffer.BlueBits = 8;
	s_Data.Hints.FrameBuffer.AlphaBits = 8;
	s_Data.Hints.FrameBuffer.DepthBits = 24;
	s_Data.Hints.FrameBuffer.StencilBits = 8;
	s_Data.Hints.FrameBuffer.DoubleBuffer = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::WindowHint(const Hint hint, const bool value)
{
	switch(hint)
	{
		case Hint::Resizable:
		{
			s_Data.Hints.Window.Resizable = value;
			break;
		}

		case Hint::Maximized:
		{
			s_Data.Hints.Window.Maximized = value;
			break;
		}

		case Hint::Visible:
		{
			s_Data.Hints.Window.Visible = value;
			break;
		}

		case Hint::Focused:
		{
			s_Data.Hints.Window.Focused = value;
			break;
		}

		case Hint::FocusOnShow:
		{
			s_Data.Hints.Window.FocusOnShow = value;
			break;
		}

		case Hint::Decorated:
		{
			s_Data.Hints.Window.Decorated = value;
			break;
		}

		case Hint::Floating:
		{
			s_Data.Hints.Window.Floating = value;
			break;
		}

		case Hint::MousePassthrough:
		{
			s_Data.Hints.Window.MousePassthrough = value;
			break;
		}

		/*case Hint::Stereo:
		{
			s_Data.Hints.FrameBuffer.Stereo = value;
			break;
		}*/

		default:
		{
			InputError(Error::Invalid_Enum, " Invalid Window Hint!");
			break;			
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetSamples(const uint32_t samples)
{
	s_Data.Hints.FrameBuffer.Samples = samples;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetContextAPI(const ContextAPI contextAPI)
{
	s_Data.Hints.Context.Client = contextAPI;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::WindowingAPI::GetMonitorName(const InternalMonitor* monitor)
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

const TRAP::INTERNAL::WindowingAPI::VideoMode& TRAP::INTERNAL::WindowingAPI::GetVideoMode(InternalMonitor* monitor)
{
	monitor->CurrentMode = PlatformGetVideoMode(monitor);
	return monitor->CurrentMode;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::INTERNAL::WindowingAPI::VideoMode> TRAP::INTERNAL::WindowingAPI::GetVideoModes(InternalMonitor* monitor)
{
	if (!monitor)
		return std::vector<VideoMode>{};
	if (!RefreshVideoModes(monitor))
		return std::vector<VideoMode>{};

	return monitor->Modes;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalWindow> TRAP::INTERNAL::WindowingAPI::CreateWindow(const uint32_t width,
	                                                                                                 const uint32_t height,
	                                                                                                 const std::string& title,
	                                                                                                 InternalMonitor* monitor,
	                                                                                                 const InternalWindow* share)
{
	TRAP_WINDOW_ASSERT(!title.empty(), "[Window] Empty Title provided!");
	TRAP_WINDOW_ASSERT(width > 0, "[Window] Invalid width provided!");
	TRAP_WINDOW_ASSERT(height > 0, "[Window] Invalid height provided!");

	if(width <= 0 || height <= 0)
	{
		InputError(Error::Invalid_Value, " Invalid Window size: " + std::to_string(width) + "x" + std::to_string(height));
		return nullptr;
	}

	const FrameBufferConfig FBConfig = s_Data.Hints.FrameBuffer;
	ContextConfig CTXConfig = s_Data.Hints.Context;
	WindowConfig WNDConfig = s_Data.Hints.Window;

	WNDConfig.Width = width;
	WNDConfig.Height = height;
	WNDConfig.Title = title;
	CTXConfig.Share = share;

	if (!IsValidContextConfig(CTXConfig))
		return nullptr;

	Scope<InternalWindow> window = MakeScope<InternalWindow>();

	s_Data.WindowList.emplace_front(window.get());

	window->videoMode.Width = width;
	window->videoMode.Height = height;
	window->videoMode.RedBits = FBConfig.RedBits;
	window->videoMode.GreenBits = FBConfig.GreenBits;
	window->videoMode.BlueBits = FBConfig.BlueBits;
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
	if (!PlatformCreateWindow(window.get(), WNDConfig, CTXConfig, FBConfig))
	{
		DestroyWindow(std::move(window));
		return nullptr;
	}

	if (CTXConfig.Client != ContextAPI::None)
	{
		if (!RefreshContextAttribs(window.get(), CTXConfig))
		{
			DestroyWindow(std::move(window));
			return nullptr;
		}
	}

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

	PlatformSetWindowMousePassthrough(window.get(), WNDConfig.MousePassthrough);

	return window;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowShouldClose(InternalWindow* window, const bool value)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	window->ShouldClose = value;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowTitle(const InternalWindow* window, const std::string& title)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	PlatformSetWindowTitle(window, title);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::GetMonitorContentScale(const InternalMonitor* monitor, float& xScale, float& yScale)
{
	PlatformGetMonitorContentScale(monitor, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

//Checks whether the desired context attributes are valid
bool TRAP::INTERNAL::WindowingAPI::IsValidContextConfig(const ContextConfig& CTXConfig)
{
	if (CTXConfig.Share)
	{
		if (CTXConfig.Client == ContextAPI::None || CTXConfig.Share->context.Client == ContextAPI::None)
		{
			InputError(Error::No_Window_Context, "");
			return false;
		}
	}

	return true;
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
	Scope<WindowingError> error;
	std::string description = "[Window]";
	
	if (!str.empty())
		description += str;
	else
	{
		if (code == Error::No_Current_Context)
			description += " There is no current context";
		else if (code == Error::Invalid_Enum)
			description += " Invalid argument for enum parameter";
		else if (code == Error::Invalid_Value)
			description += " Invalid value for parameter";
		else if (code == Error::Out_Of_Memory)
			description += " Out of memory";
		else if (code == Error::API_Unavailable)
			description += " The requested API is unavailable";
		else if (code == Error::Version_Unavailable)
			description += " The requested API version is unavailable";
		else if (code == Error::Platform_Error)
			description += " A platform-specific error occurred";
		else if (code == Error::Format_Unavailable)
			description += " The requested format is unavailable";
		else if (code == Error::No_Window_Context)
			description += " The specified window has no context";
		else if (code == Error::Cursor_Unavailable)
			description += " The specified cursor shape is unavailable";
		else
			description += " UNKNOWN WINDOWING ERROR";
	}

	const auto errorPtr = static_cast<WindowingError*>(PlatformGetTLS(s_Data.ErrorSlot));
	if (!errorPtr)
	{			
		error = MakeScope<WindowingError>();
		PlatformSetTLS(s_Data.ErrorSlot, error.get());
		PlatformLockMutex(s_Data.ErrorLock);
		error->Next = s_Data.ErrorListHead.get();
		s_Data.ErrorListHead = std::move(error);
		PlatformUnlockMutex(s_Data.ErrorLock);
		s_Data.ErrorListHead->ErrorCode = code;
		s_Data.ErrorListHead->Description = description;
	}

	if (s_ErrorCallback)
		s_ErrorCallback(code, description);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::DestroyCursor(Scope<InternalCursor> cursor)
{
	if (cursor == nullptr)
		return;

	//Make sure the cursor is not being used by any window
	{
		//for (InternalWindow* window = s_Data.WindowListHead; window; window = window->Next)
		for(InternalWindow* window : s_Data.WindowList)
		{
			if (window->Cursor == cursor.get())
				SetCursor(window, nullptr);
		}
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

	TRAP_WINDOW_ASSERT(image.get(), "[Window] image is nullptr!");

	if (image) //If image is not nullptr
	{
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

		if (!((image->GetFormat() == ImageFormat::RGB && image->GetBitsPerPixel() == 24) || (image->GetFormat() == ImageFormat::RGBA && image->GetBitsPerPixel() == 32)))
		{
			InputError(Error::Invalid_Value, "[Cursor] Unsupported BPP or format used!");
			return nullptr;
		}

		//Convert to RGBA 32BPP
		if (image->GetFormat() == ImageFormat::RGB)
		{
			std::vector<uint8_t> pixelData(static_cast<uint8_t*>(image->GetPixelData()), static_cast<uint8_t*>(image->GetPixelData()) + image->GetPixelDataSize());
			const uint32_t newSize = static_cast<uint32_t>(pixelData.size()) + (image->GetWidth() * image->GetHeight());
			std::vector<uint8_t> pixelDataRGBA(newSize, 0);
			uint32_t pixelCount = 0;
			for (uint32_t i = 0; i < pixelData.size(); i++)
			{
				pixelDataRGBA[pixelCount] = pixelData[i];
				i++; pixelCount++;
				pixelDataRGBA[pixelCount] = pixelData[i];
				i++; pixelCount++;
				pixelDataRGBA[pixelCount] = pixelData[i];
				pixelCount++;
				pixelDataRGBA[pixelCount] = 255;
			}			

			const Scope<Image> iconImage = Image::LoadFromMemory(image->GetWidth(), image->GetHeight(), 32, ImageFormat::RGBA, pixelDataRGBA);
			
			cursor = MakeScope<InternalCursor>();
			s_Data.CursorList.emplace_front(cursor.get());

			if(!PlatformCreateCursor(cursor.get(), image, xHotspot, yHotspot))
			{
				DestroyCursor(std::move(cursor));
				return nullptr;
			}

			return cursor;
		}
		if (image->GetFormat() == ImageFormat::RGBA)
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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	window->Cursor = cursor;

	PlatformSetCursor(window, cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowIcon(InternalWindow* window, const Scope<Image>& image)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	if (image) //If image is not nullptr
	{
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

		if(!((image->GetFormat() == ImageFormat::RGB && image->GetBitsPerPixel() == 24) || (image->GetFormat() == ImageFormat::RGBA && image->GetBitsPerPixel() == 32)))
		{
			InputError(Error::Invalid_Value, "[Icon] Unsupported BPP or format used!");
			return;			
		}

		//Convert to RGBA 32BPP
		if (image->GetFormat() == ImageFormat::RGB)
		{
			std::vector<uint8_t> pixelData(static_cast<uint8_t*>(image->GetPixelData()), static_cast<uint8_t*>(image->GetPixelData()) + image->GetPixelDataSize());
			const uint32_t newSize = static_cast<uint32_t>(pixelData.size()) + (image->GetWidth() * image->GetHeight());
			std::vector<uint8_t> pixelDataRGBA(newSize, 0);
			uint32_t pixelCount = 0;
			for (uint32_t i = 0; i < pixelData.size(); i++)
			{
				pixelDataRGBA[pixelCount] = pixelData[i];
				i++; pixelCount++;
				pixelDataRGBA[pixelCount] = pixelData[i];
				i++; pixelCount++;
				pixelDataRGBA[pixelCount] = pixelData[i];
				pixelCount++;
				pixelDataRGBA[pixelCount] = 255;
			}

			const Scope<Image> iconImage = Image::LoadFromMemory(image->GetWidth(), image->GetHeight(), 32, ImageFormat::RGBA, pixelDataRGBA);
			PlatformSetWindowIcon(window, iconImage);
		}
		else if (image->GetFormat() == ImageFormat::RGBA)
			PlatformSetWindowIcon(window, image);
	}
	else
		PlatformSetWindowIcon(window, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowPos(const InternalWindow* window, const int32_t xPos, const int32_t yPos)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	if (window->Monitor)
		return;

	PlatformSetWindowPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the position of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowPos(const InternalWindow* window, int32_t& xPos, int32_t& yPos)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	xPos = 0;
	yPos = 0;
	
	PlatformGetWindowPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the size of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowSize(InternalWindow* window, const int32_t width, const int32_t height)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	TRAP_WINDOW_ASSERT(width > 0, "[Window] width is smaller than or equal to 0!");
	TRAP_WINDOW_ASSERT(height > 0, "[Window] height is smaller than or equal to 0!");

	window->videoMode.Width = width;
	window->videoMode.Height = height;

	PlatformSetWindowSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the size of the content area of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowSize(const InternalWindow* window, int32_t& width, int32_t& height)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	width = 0;
	height = 0;
	
	PlatformGetWindowSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the size of the framebuffer of the specified window.
void TRAP::INTERNAL::WindowingAPI::GetFrameBufferSize(const InternalWindow* window, int32_t& width, int32_t& height)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	width = 0;
	height = 0;
	
	PlatformGetFrameBufferSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the opacity of the whole window.
void TRAP::INTERNAL::WindowingAPI::SetWindowOpacity(const InternalWindow* window, const float opacity)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return PlatformGetWindowOpacity(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the content scale for the specified window.
void TRAP::INTERNAL::WindowingAPI::GetWindowContentScale(const InternalWindow* window, float& xScale, float& yScale)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	xScale = 0.0f;
	yScale = 0.0f;

	PlatformGetWindowContentScale(window, xScale, yScale);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets an attribute for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowAttrib(InternalWindow* window, const Hint hint, const bool value)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	switch(hint)
	{
		case Hint::Resizable:
		{
			if (window->Resizable == value)
				return;

			window->Resizable = value;

			if (!window->Monitor)
				PlatformSetWindowResizable(window, value);
			break;
		}

		case Hint::FocusOnShow:
		{
			window->FocusOnShow = value;
			break;
		}

		case Hint::Decorated:
		{
			if (window->Decorated == value)
				return;

			window->Decorated = value;
			if (!window->Monitor)
				PlatformSetWindowDecorated(window, value);
			break;
		}

		case Hint::Floating:
		{
			if (window->Floating == value)
				return;

			window->Floating = value;
			if (!window->Monitor)
				PlatformSetWindowFloating(window, value);
			break;
		}

		case Hint::MousePassthrough:
		{
			PlatformSetWindowMousePassthrough(window, value);
			break;
		}

		default:
		{
			InputError(Error::Invalid_Enum, " Invalid window attribute provided!");
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns an attribute of the specified window.
bool TRAP::INTERNAL::WindowingAPI::GetWindowAttrib(const InternalWindow* window, const Hint hint)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	TRAP_WINDOW_ASSERT(width > 0, "[Window] width is smaller than or equal to 0!");
	TRAP_WINDOW_ASSERT(height > 0, "[Window] height is smaller than or equal to 0!");

	if (width <= 0 || height <= 0)
	{
		InputError(Error::Invalid_Value, " Invalid window size " + std::to_string(width) + "x" + std::to_string(height));
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

	PlatformSetWindowMonitor(window, monitor,
		xPos, yPos, width, height,
		refreshRate);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the mode, monitor and placement of a window.
void TRAP::INTERNAL::WindowingAPI::SetWindowMonitorBorderless(InternalWindow* window, InternalMonitor* monitor)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	TRAP_WINDOW_ASSERT(monitor, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->UserPointer = pointer;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the user pointer of the specified window.
void* TRAP::INTERNAL::WindowingAPI::GetWindowUserPointer(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	return window->UserPointer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetErrorCallback(const ErrorFunc callback)
{
	s_ErrorCallback = callback;
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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.Pos = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the size callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowSizeCallback(InternalWindow* window, const WindowSizeFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.Size = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the close callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowCloseCallback(InternalWindow* window, const WindowCloseFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.Close = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the focus callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetWindowFocusCallback(InternalWindow* window, const WindowFocusFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.Focus = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the framebuffer resize callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetFrameBufferSizeCallback(InternalWindow* window, const FrameBufferSizeFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.FBSize = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the window content scale callback for the specified window.
void TRAP::INTERNAL::WindowingAPI::SetContentScaleCallback(InternalWindow* window, const WindowContentScaleFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.Scale = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the key callback.
void TRAP::INTERNAL::WindowingAPI::SetKeyCallback(InternalWindow* window, const KeyFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.Key = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the Unicode character callback.
void TRAP::INTERNAL::WindowingAPI::SetCharCallback(InternalWindow* window, const CharFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.Character = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the mouse button callback.
void TRAP::INTERNAL::WindowingAPI::SetMouseButtonCallback(InternalWindow* window, const MouseButtonFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.MouseButton = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor position callback.
void TRAP::INTERNAL::WindowingAPI::SetCursorPosCallback(InternalWindow* window, const CursorPositionFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.CursorPos = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the cursor enter callback.
void TRAP::INTERNAL::WindowingAPI::SetCursorEnterCallback(InternalWindow* window, const CursorEnterFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	window->Callbacks.CursorEnter = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the scroll callback.
void TRAP::INTERNAL::WindowingAPI::SetScrollCallback(InternalWindow* window, const ScrollFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.Scroll = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the path drop callback.
void TRAP::INTERNAL::WindowingAPI::SetDropCallback(InternalWindow* window, const DropFunc callback)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	window->Callbacks.Drop = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::ErrorFunc TRAP::INTERNAL::WindowingAPI::GetErrorCallback()
{
	return s_ErrorCallback;
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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Pos;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the size callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowSizeFunc TRAP::INTERNAL::WindowingAPI::GetWindowSizeCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Size;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the close callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowCloseFunc TRAP::INTERNAL::WindowingAPI::GetWindowCloseCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Close;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the focus callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowFocusFunc TRAP::INTERNAL::WindowingAPI::GetWindowFocusCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Focus;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the framebuffer resize callback for the specified window.
TRAP::INTERNAL::WindowingAPI::FrameBufferSizeFunc TRAP::INTERNAL::WindowingAPI::GetFrameBufferSizeCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.FBSize;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the window content scale callback for the specified window.
TRAP::INTERNAL::WindowingAPI::WindowContentScaleFunc TRAP::INTERNAL::WindowingAPI::GetWindowContentScaleCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Scale;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the key callback.
TRAP::INTERNAL::WindowingAPI::KeyFunc TRAP::INTERNAL::WindowingAPI::GetKeyCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	return window->Callbacks.Key;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the Unicode character callback.
TRAP::INTERNAL::WindowingAPI::CharFunc TRAP::INTERNAL::WindowingAPI::GetCharCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Character;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the mouse button callback.
TRAP::INTERNAL::WindowingAPI::MouseButtonFunc TRAP::INTERNAL::WindowingAPI::GetMouseButtonCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.MouseButton;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the cursor position callback.
TRAP::INTERNAL::WindowingAPI::CursorPositionFunc TRAP::INTERNAL::WindowingAPI::GetCursorPosCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.CursorPos;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the cursor enter callback.
TRAP::INTERNAL::WindowingAPI::CursorEnterFunc TRAP::INTERNAL::WindowingAPI::GetCursorEnterCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.CursorEnter;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the scroll callback.
TRAP::INTERNAL::WindowingAPI::ScrollFunc TRAP::INTERNAL::WindowingAPI::GetScrollCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->Callbacks.Scroll;
}

//-------------------------------------------------------------------------------------------------------------------//

//Gets the path drop callback.
TRAP::INTERNAL::WindowingAPI::DropFunc TRAP::INTERNAL::WindowingAPI::GetDropCallback(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->MouseButtons[static_cast<uint32_t>(button)];
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the position of the cursor, relative to the content area of the window
void TRAP::INTERNAL::WindowingAPI::SetCursorPos(InternalWindow* window, const double xPos, const double yPos)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	if (xPos < -DBL_MAX || xPos > DBL_MAX ||
	    yPos < -DBL_MAX || yPos > DBL_MAX)
	{
		InputError(Error::Invalid_Value, " Invalid cursor position: " + std::to_string(xPos) + "x" + std::to_string(yPos));
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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(monitor, "[Window] monitor is nullptr!");

	xPos = 0;
	yPos = 0;

	PlatformGetMonitorPos(monitor, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::GetMonitorWorkArea(const InternalMonitor* monitor, int32_t& xPos, int32_t& yPos, int32_t& width, int32_t& height)
{
	TRAP_WINDOW_ASSERT(monitor, "[Window] monitor is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

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
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	PlatformFocusWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Maximizes the specified window.
void TRAP::INTERNAL::WindowingAPI::MaximizeWindow(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	if (window->Monitor || !window->Resizable)
		return;

	PlatformMaximizeWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Minimizes the specified window.
void TRAP::INTERNAL::WindowingAPI::MinimizeWindow(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");
	
	PlatformMinimizeWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Requests user attention to the specified window.
void TRAP::INTERNAL::WindowingAPI::RequestWindowAttention(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	PlatformRequestWindowAttention(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Hides the specified window.
void TRAP::INTERNAL::WindowingAPI::HideWindow(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	if (window->Monitor)
		return;

	PlatformHideWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::RestoreWindow(InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	PlatformRestoreWindow(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::SetWindowSizeLimits(InternalWindow* window,
                                                       const int32_t minWidth,
                                                       const int32_t minHeight,
                                                       const int32_t maxWidth,
                                                       const int32_t maxHeight)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	if(minWidth != -1 && minHeight != -1)
	{
		if(minWidth < 0 || minHeight < 0)
		{
			InputError(Error::Invalid_Value, " Invalid Minimum Window Size!");
			return;
		}
	}

	if(maxWidth != -1 || maxHeight != -1)
	{
		if(maxWidth < 0 || maxHeight < 0 || maxWidth < minWidth || maxHeight < minHeight)
		{
			InputError(Error::Invalid_Value, " Invalid Maximum Window Size!");
			return;
		}
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
void TRAP::INTERNAL::WindowingAPI::SetClipboardString(const std::string& string)
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

//Returns the window whose context is current on the calling thread.
TRAP::INTERNAL::WindowingAPI::InternalWindow* TRAP::INTERNAL::WindowingAPI::GetCurrentContext()
{
	if (!PlatformGetTLS(s_Data.ContextSlot))
		return nullptr;

	return static_cast<InternalWindow*>(PlatformGetTLS(s_Data.ContextSlot));
}

//-------------------------------------------------------------------------------------------------------------------//

//Swaps the front and back buffers of the specified window.
void TRAP::INTERNAL::WindowingAPI::SwapBuffers(InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	if (window->context.Client == ContextAPI::None)
	{
		InputError(Error::No_Window_Context, " Cannot swap buffers of a window that has no OpenGL context!");
		return;
	}

	window->context.SwapBuffers(window);
}

//-------------------------------------------------------------------------------------------------------------------//

//Sets the swap interval for the current context.
void TRAP::INTERNAL::WindowingAPI::SwapInterval(const int32_t interval)
{
	const auto windowPtr = static_cast<InternalWindow*>(PlatformGetTLS(s_Data.ContextSlot));
	
	if (!windowPtr)
	{
		InputError(Error::No_Window_Context, " Cannot set swap interval without a current OpenGL context");
		return;
	}

	windowPtr->context.SwapInterval(interval);
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns whether the specified extension is available.
bool TRAP::INTERNAL::WindowingAPI::ExtensionSupported(const std::string& extension)
{
	const auto windowPtr = static_cast<InternalWindow*>(PlatformGetTLS(s_Data.ContextSlot));
	
	if (!windowPtr)
	{
		InputError(Error::No_Current_Context, " Cannot query extension without a current OpenGL context");
		return false;
	}

	if (extension.empty())
	{
		InputError(Error::Invalid_Value, " Extension name cannot be an empty string");
		return false;
	}

	GLint count;

	//Check if extension is in the modern OpenGL extensions string list

	windowPtr->context.GetIntegerv(GL_NUM_EXTENSIONS, &count);

	for (uint32_t i = 0; i < static_cast<uint32_t>(count); i++)
	{
		const char* en = reinterpret_cast<const char*>(windowPtr->context.GetStringi(GL_EXTENSIONS, i));
		if (!en)
		{
			InputError(Error::Platform_Error, " Extension string retrieval is broken");
			return false;
		}
		const std::string_view ext(en);

		if (ext == extension)
			return true;
	}

	//Check if extension is in the platform-specific string
	return windowPtr->context.ExtensionSupported(extension.data());
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns the address of the specified function for the current context.
TRAP::INTERNAL::WindowingAPI::GLProcess TRAP::INTERNAL::WindowingAPI::GetProcAddress(const char* procName)
{
	TRAP_WINDOW_ASSERT(procName,"[Window] Process name is nullptr!");

	const auto windowPtr = static_cast<InternalWindow*>(PlatformGetTLS(s_Data.ContextSlot));
	
	if (!windowPtr)
	{
		InputError(Error::No_Current_Context, " Cannot query entry point without a current OpenGL context");
		return nullptr;
	}

	return windowPtr->context.GetProcAddress(procName);
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
	TRAP_WINDOW_ASSERT(instance, "[Vulkan] Instance is nullptr!");
	TRAP_WINDOW_ASSERT(window, " Window is nullptr!");

	if (!InitVulkan(2))
		return VK_ERROR_INITIALIZATION_FAILED;

	if constexpr (s_Data.VK.Extensions.empty())
	{
		InputError(Error::API_Unavailable, "[Vulkan] Window surface creation extensions not found");
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	if (window->context.Client != ContextAPI::None)
	{
		InputError(Error::Invalid_Value, "[Vulkan] Window surface creation requires the window to have the client API set to None");
		return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
	}

	return PlatformCreateWindowSurface(instance, window, allocator, surface);
}

//-------------------------------------------------------------------------------------------------------------------//

//Searches an extension string for the specified extension
bool TRAP::INTERNAL::WindowingAPI::StringInExtensionString(const char* string, const char* extensions)
{
	const char* start = extensions;

	for (;;)
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

//Chooses the framebuffer config that best matches the desired one
const TRAP::INTERNAL::WindowingAPI::FrameBufferConfig* TRAP::INTERNAL::WindowingAPI::ChooseFBConfig(const FrameBufferConfig& desired,
	                                                                                                const std::vector<FrameBufferConfig>& alternatives)
{
	uint32_t missing, leastMissing = UINT_MAX;
	uint32_t colorDiff, leastColorDiff = UINT_MAX;
	uint32_t extraDiff, leastExtraDiff = UINT_MAX;
	const FrameBufferConfig* closest = nullptr;

	for (uint32_t i = 0; i < alternatives.size(); i++)
	{
		const FrameBufferConfig* current = &alternatives[i];

		if (desired.Stereo != current->Stereo)
			//Stereo is a hard constraint
			continue;

		if (desired.DoubleBuffer != current->DoubleBuffer)
			//Double buffering is a hard constraint
			continue;

			//Count number of missing buffers
		{
			missing = 0;

			if (desired.AlphaBits > 0 && current->AlphaBits == 0)
				missing++;

			if (desired.DepthBits > 0 && current->DepthBits == 0)
				missing++;

			if (desired.StencilBits > 0 && current->StencilBits == 0)
				missing++;

			if (desired.AuxBuffers > 0 && current->AuxBuffers < desired.AuxBuffers)
				missing += desired.AuxBuffers - current->AuxBuffers;
			
			if (desired.Samples > 0 && current->Samples == 0)
			{
				//Technically, several multisampling buffers could be
				//involved, but that's a lower level implementation detail and
				//not important to us here, so we count them as one
				missing++;
			}

			if (desired.Transparent != current->Transparent)
				missing++;
		}

		//These polynomials make many small channel size differences matter
		//less than one large channel size difference

		//Calculate color channel size difference value
		{
			colorDiff = 0;

			if (desired.RedBits != -1)
			{
				colorDiff += (desired.RedBits - current->RedBits) *
					         (desired.RedBits - current->RedBits);
			}

			if (desired.GreenBits != -1)
			{
				colorDiff += (desired.GreenBits - current->GreenBits) *
					         (desired.GreenBits - current->GreenBits);
			}

			if (desired.BlueBits != -1)
			{
				colorDiff += (desired.BlueBits - current->BlueBits) *
					         (desired.BlueBits - current->BlueBits);
			}
		}

		//Calculate non-color channel size difference value
		{
			extraDiff = 0;

			if (desired.AlphaBits != -1)
			{
				extraDiff += (desired.AlphaBits - current->AlphaBits) *
					         (desired.AlphaBits - current->AlphaBits);
			}

			if (desired.DepthBits != -1)
			{
				extraDiff += (desired.DepthBits - current->DepthBits) *
					         (desired.DepthBits - current->DepthBits);
			}

			if (desired.StencilBits != -1)
			{
				extraDiff += (desired.StencilBits - current->StencilBits) *
					         (desired.StencilBits - current->StencilBits);
			}

			if (desired.AccumRedBits != -1)
			{
				extraDiff += (desired.AccumRedBits - current->AccumRedBits) *
					         (desired.AccumRedBits - current->AccumRedBits);
			}

			if (desired.AccumGreenBits != -1)
			{
				extraDiff += (desired.AccumGreenBits - current->AccumGreenBits) *
					         (desired.AccumGreenBits - current->AccumGreenBits);
			}

			if (desired.AccumBlueBits != -1)
			{
				extraDiff += (desired.AccumBlueBits - current->AccumBlueBits) *
					         (desired.AccumBlueBits - current->AccumBlueBits);
			}

			if (desired.AccumAlphaBits != -1)
			{
				extraDiff += (desired.AccumAlphaBits - current->AccumAlphaBits) *
					         (desired.AccumAlphaBits - current->AccumAlphaBits);
			}

			if (desired.Samples != -1)
			{
				extraDiff += (desired.Samples - current->Samples) *
					         (desired.Samples - current->Samples);
			}

			if (desired.SRGB && !current->SRGB)
				extraDiff++;
		}

		//Figure out if the current one is better than the best one found so far
		//Least number of missing buffers is the most important heuristic,
		//then color buffer size match and lastly size match for other buffers

		if (missing < leastMissing)
			closest = current;
		else if (missing == leastMissing)
		{
			if ((colorDiff < leastColorDiff) ||
				(colorDiff == leastColorDiff && extraDiff < leastExtraDiff))
			{
				closest = current;
			}
		}

		if (current == closest)
		{
			leastMissing = missing;
			leastColorDiff = colorDiff;
			leastExtraDiff = extraDiff;
		}
	}

	return closest;
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
		return "UNKNOWN VULKAN ERROR";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a cursor motion event
//The position is specified in content area relative screen coordinates
void TRAP::INTERNAL::WindowingAPI::InputCursorPos(InternalWindow* window, const double xPos, const double yPos)
{
	if (window->VirtualCursorPosX == xPos && window->VirtualCursorPosY == yPos)
		return;

	window->VirtualCursorPosX = xPos;
	window->VirtualCursorPosY = yPos;

	if (window->Callbacks.CursorPos)
		window->Callbacks.CursorPos(window, xPos, yPos);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a physical key event
void TRAP::INTERNAL::WindowingAPI::InputKey(InternalWindow* window, Input::Key key, const int32_t scancode, const bool pressed)
{
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
void TRAP::INTERNAL::WindowingAPI::InputMouseClick(InternalWindow* window, Input::MouseButton button, const bool pressed)
{
	window->MouseButtons[static_cast<uint32_t>(button)] = pressed;

	if (window->Callbacks.MouseButton)
		window->Callbacks.MouseButton(window, button, pressed);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a scroll event
void TRAP::INTERNAL::WindowingAPI::InputScroll(const InternalWindow* window, const double xOffset, const double yOffset)
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
void TRAP::INTERNAL::WindowingAPI::InputFrameBufferSize(const InternalWindow* window, const int32_t width, const int32_t height)
{
	if (window->Callbacks.FBSize)
		window->Callbacks.FBSize(window, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code that a window has been resized
//The size is specified in screen coordinates
void TRAP::INTERNAL::WindowingAPI::InputWindowSize(const InternalWindow* window, const int32_t width, const int32_t height)
{
	if (window->Callbacks.Size)
		window->Callbacks.Size(window, width, height);
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

	if (!focused)
	{
		for (uint32_t key = 0; key <= static_cast<uint32_t>(Input::Key::Menu); key++)
			if (window->Keys[key] == true)
			{
				const int32_t scanCode = PlatformGetKeyScanCode(static_cast<Input::Key>(key));
				InputKey(window, static_cast<Input::Key>(key), scanCode, false);
			}

		for (uint32_t button = 0; button <= static_cast<uint32_t>(Input::MouseButton::Eight); button++)
			if (window->MouseButtons[button])
				InputMouseClick(window, static_cast<Input::MouseButton>(button), false);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the available modes for the specified monitor
bool TRAP::INTERNAL::WindowingAPI::RefreshVideoModes(InternalMonitor* monitor)
{
	if (!monitor->Modes.empty())
		return true;

	std::vector<VideoMode> modes = PlatformGetVideoModes(monitor);
	if (modes.empty())
		return false;

	std::qsort(modes.data(), modes.size(), sizeof(VideoMode), CompareVideoModes);

	monitor->Modes = {};
	monitor->Modes = modes;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrieves the attributes of the current context
bool TRAP::INTERNAL::WindowingAPI::RefreshContextAttribs(InternalWindow* window,
	                                                     const ContextConfig& CTXConfig)
{
	window->context.Client = ContextAPI::OpenGL;

	const auto previousPtr = static_cast<InternalWindow*>(PlatformGetTLS(s_Data.ContextSlot));
	MakeContextCurrent(window);

	window->context.GetIntegerv = reinterpret_cast<PFNGLGETINTEGERVPROC>(window->context.GetProcAddress("glGetIntegerv"));
	window->context.GetString = reinterpret_cast<PFNGLGETSTRINGPROC>(window->context.GetProcAddress("glGetString"));
	if (!window->context.GetIntegerv || !window->context.GetString)
	{
		InputError(Error::Platform_Error, "[OpenGL] Entry point retrieval is broken");
		MakeContextCurrent(previousPtr);
		return false;
	}

	const char* version = reinterpret_cast<const char*>(window->context.GetString(GL_VERSION));
	if (!version)
	{
		if (CTXConfig.Client == ContextAPI::OpenGL)
		{
			InputError(Error::Platform_Error, "[OpenGL] version string retrieval is broken");
		}

		MakeContextCurrent(previousPtr);
		return false;
	}

	std::vector<std::string> splittedVersion = Utils::String::SplitString(version, '.');

	if (splittedVersion.empty() || splittedVersion.size() < 2)
	{
		if (window->context.Client == ContextAPI::OpenGL)
			InputError(Error::Platform_Error, "[OpenGL] No version found in OpenGL version string");

		MakeContextCurrent(previousPtr);
		return false;
	}

	window->context.Major = std::stoi(splittedVersion[0]);
	window->context.Minor = std::stoi(splittedVersion[1]);

	if (window->context.Major < 4 ||
		(window->context.Major == 4 &&
			window->context.Minor < 6))
	{
		//The desired OpenGL version is greater than the actual version
		//This only happens if the machine lacks {GLX|WGL}_ARB_create_context
		//and the user has requested an OpenGL version greater than 1.0

		//For API consistency, we emulate the behavior of the
		//{GLX|WGL}_ARB_create_context extension and fail here

		if (window->context.Client == ContextAPI::OpenGL)
			InputError(Error::Version_Unavailable, "[OpenGl] Requested OpenGL version 4.6, got version " + std::to_string(window->context.Major) + "." + std::to_string(window->context.Minor));

		MakeContextCurrent(previousPtr);
		return false;
	}

	//OpenGL 3.0+ uses a different function for extension string retrieval
	//We cache it here instead of in ExtensionSupported mostly to alert
	//users as early as possible that their build may be broken

	window->context.GetStringi = reinterpret_cast<PFNGLGETSTRINGIPROC>(window->context.GetProcAddress("glGetStringi"));
	if (!window->context.GetStringi)
	{
		InputError(Error::Platform_Error, "[OpenGL] Entry point retrieval is broken");
		MakeContextCurrent(previousPtr);
		return false;
	}

	if (window->context.Client == ContextAPI::OpenGL)
	{
		GLint flags;
		window->context.GetIntegerv(GL_CONTEXT_FLAGS, &flags);

		//Read back OpenGL context profile (OpenGL 3.2 and above)
		GLint mask;
		window->context.GetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
	}

	//Clearing the front buffer to black to avoid garbage pixels left over from
	//previous uses of our bit of VRAM
	{
		const PFNGLCLEARPROC glClearWin = reinterpret_cast<PFNGLCLEARPROC>(window->context.GetProcAddress("glClear"));
		glClearWin(GL_COLOR_BUFFER_BIT);
		window->context.SwapBuffers(window);
	}

	MakeContextCurrent(previousPtr);
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::WindowingAPI::InitVulkan(const uint32_t mode)
{
	uint32_t count;

	if (s_Data.VK.Available)
		return true;

	VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
	if (err)
	{
		//NOTE: This happens on systems with a loader but without any Vulkan ICD
		if (mode == 2)
			InputError(Error::API_Unavailable, "[Vulkan] Failed to query instance extension count: " + GetVulkanResultString(err));

		return false;
	}

	std::vector<VkExtensionProperties> ep(count);

	err = vkEnumerateInstanceExtensionProperties(nullptr, &count, ep.data());
	if (err)
	{
		InputError(Error::API_Unavailable, "[Vulkan] Failed to query instance extensions: " + GetVulkanResultString(err));
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
#elif defined(TRAP_PLATFORM_LINUX)
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
	const VideoMode* fm = static_cast<const VideoMode*>(fp);
	const VideoMode* sm = static_cast<const VideoMode*>(sp);
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
TRAP::INTERNAL::WindowingAPI::VideoMode* TRAP::INTERNAL::WindowingAPI::ChooseVideoMode(InternalMonitor* monitor, const VideoMode& desired)
{
	uint32_t leastSizeDiff = UINT_MAX;
	uint32_t rateDiff, leastRateDiff = UINT_MAX;
	uint32_t leastColorDiff = UINT_MAX;
	VideoMode* closest = nullptr;

	if (!RefreshVideoModes(monitor))
		return nullptr;

	for (uint32_t i = 0; i < monitor->Modes.size(); i++)
	{
		VideoMode* current = &monitor->Modes[i];

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
void TRAP::INTERNAL::WindowingAPI::InputMonitor(Scope<InternalMonitor> monitor, const bool connected, const uint32_t placement)
{
	if (connected)
	{
		if (placement == 0)
			s_Data.Monitors.insert(s_Data.Monitors.begin(), std::move(monitor));
		else
			s_Data.Monitors.push_back(std::move(monitor));
	}
	else
	{
		for(InternalWindow* window : s_Data.WindowList)
		{
			if (window->Monitor == monitor.get())
			{
				int32_t width = 0, height = 0, xOff = 0, yOff = 0, unused = 0;
				PlatformGetWindowSize(window, width, height);
				PlatformSetWindowMonitor(window, nullptr, 0, 0, width, height, 0);
				PlatformGetWindowFrameSize(window, xOff, yOff, unused, unused);
				PlatformSetWindowPos(window, xOff, yOff);
			}
		}

		for (uint32_t i = 0; i < s_Data.Monitors.size(); i++)
		{
			if (s_Data.Monitors[i] == monitor)
			{
				s_Data.Monitors.erase(s_Data.Monitors.begin() + i);
				break;
			}
		}
	}

	if (s_Data.Callbacks.Monitor)
		s_Data.Callbacks.Monitor(monitor.get(), connected);

	if (!connected)
		if (monitor)
			monitor.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of a monitor disconnection
void TRAP::INTERNAL::WindowingAPI::InputMonitorDisconnect(const uint32_t monitorIndex, const uint32_t placement)
{
	Scope<InternalMonitor>& monitor = s_Data.Monitors[monitorIndex];
	
	for(InternalWindow* window : s_Data.WindowList)
	{
		if (window->Monitor == monitor.get())
		{
			int32_t width = 0, height = 0, xOff = 0, yOff = 0, unused = 0;
			PlatformGetWindowSize(window, width, height);
			PlatformSetWindowMonitor(window, nullptr, 0, 0, width, height, 0);
			PlatformGetWindowFrameSize(window, xOff, yOff, unused, unused);
			PlatformSetWindowPos(window, xOff, yOff);
		}
	}

	for (uint32_t i = 0; i < s_Data.Monitors.size(); i++)
	{
		if (s_Data.Monitors[i] == monitor)
		{
			s_Data.Monitors.erase(s_Data.Monitors.begin() + i);
			break;
		}
	}

	if (s_Data.Callbacks.Monitor)
		s_Data.Callbacks.Monitor(monitor.get(), false);

	if (monitor)
		monitor.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::HideWindowFromTaskbar(InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, " Window is nullptr!");
	
	PlatformHideWindowFromTaskbar(window);
}