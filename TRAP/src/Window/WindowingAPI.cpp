#include "TRAPPCH.h"

#include "WindowingAPI.h"
#include "Window.h"

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

	//Should always be empty
	/*while (s_Data.WindowListHead)
		DestroyWindow(s_Data.WindowListHead);

	//Should always be empty
	while (s_Data.CursorListHead)
		DestroyCursor(s_Data.CursorListHead);*/

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
	{
		InternalWindow** prev = &s_Data.WindowListHead;

		while (*prev != window.get())
			prev = &((*prev)->Next);

		*prev = window->Next;
	}

	window.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

//Makes the context of the specified window current for the calling
void TRAP::INTERNAL::WindowingAPI::MakeContextCurrent(InternalWindow* window)
{
	const auto previsouPtr = static_cast<InternalWindow*>(PlatformGetTLS(s_Data.ContextSlot));

	if (window && window->Context.Client == ContextAPI::None)
	{
		InputError(Error::No_Window_Context, " Cannot make current with a window that has no OpenGL context");
		return;
	}

	if (previsouPtr)
	{
		if (!window)
			previsouPtr->Context.MakeCurrent(nullptr);
	}

	if (window)
		window->Context.MakeCurrent(window);
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
	window->Next = s_Data.WindowListHead;
	s_Data.WindowListHead = window.get();

	window->VideoMode.Width = width;
	window->VideoMode.Height = height;
	window->VideoMode.RedBits = FBConfig.RedBits;
	window->VideoMode.GreenBits = FBConfig.GreenBits;
	window->VideoMode.BlueBits = FBConfig.BlueBits;
	window->VideoMode.RefreshRate = -1;

	window->Monitor = monitor;
	window->Resizable = WNDConfig.Resizable;
	window->Decorated = WNDConfig.Decorated;
	window->Floating = WNDConfig.Floating;
	window->FocusOnShow = WNDConfig.FocusOnShow;
	window->CursorMode = CursorMode::Normal;
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
		if (CTXConfig.Client == ContextAPI::None || CTXConfig.Share->Context.Client == ContextAPI::None)
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
		for (InternalWindow* window = s_Data.WindowListHead; window; window = window->Next)
		{
			if (window->Cursor == cursor.get())
				SetCursor(window, nullptr);
		}
	}

	PlatformDestroyCursor(cursor.get());

	//Unlink window from global linked list
	{
		InternalCursor** prev = &s_Data.CursorListHead;

		while (*prev != cursor.get())
			prev = &((*prev)->Next);

		*prev = cursor->Next;
	}

	cursor.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

//Creates a custom cursor.
TRAP::Scope<TRAP::INTERNAL::WindowingAPI::InternalCursor> TRAP::INTERNAL::WindowingAPI::CreateCursor(const Scope<Image>& image,
	                                                                                                 const int32_t xHotspot,
	                                                                                                 const int32_t yHotspot)
{
	Scope<InternalCursor> cursor;

	TRAP_WINDOW_ASSERT(image, "[Window] image is nullptr!");

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
			cursor->Next = s_Data.CursorListHead;
			s_Data.CursorListHead = cursor.get();

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
			cursor->Next = s_Data.CursorListHead;
			s_Data.CursorListHead = cursor.get();

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
	
	cursor->Next = s_Data.CursorListHead;
	s_Data.CursorListHead = cursor.get();

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

	window->VideoMode.Width = width;
	window->VideoMode.Height = height;

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
		}

		case Hint::Floating:
		{
			if (window->Floating == value)
				return;

			window->Floating = value;
			if (!window->Monitor)
				PlatformSetWindowFloating(window, value);
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

	window->VideoMode.Width = width;
	window->VideoMode.Height = height;
	window->VideoMode.RefreshRate = refreshRate;

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

	window->VideoMode.Width = monitor->CurrentMode.Width;
	window->VideoMode.Height = monitor->CurrentMode.Height;
	window->VideoMode.RefreshRate = monitor->CurrentMode.RefreshRate;
	window->VideoMode.RedBits = monitor->CurrentMode.RedBits;
	window->VideoMode.GreenBits = monitor->CurrentMode.GreenBits;
	window->VideoMode.BlueBits = monitor->CurrentMode.BlueBits;

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
void TRAP::INTERNAL::WindowingAPI::SetWindowContentScaleCallback(InternalWindow* window, const WindowContentScaleFunc callback)
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
	
	if (window->CursorMode == mode)
		return;

	window->CursorMode = mode;

	PlatformGetCursorPos(window, window->VirtualCursorPosX, window->VirtualCursorPosY);
	PlatformSetCursorMode(window, mode);
}

//-------------------------------------------------------------------------------------------------------------------//

//Retrrieves the cursor mode for the specified window.
TRAP::INTERNAL::WindowingAPI::CursorMode TRAP::INTERNAL::WindowingAPI::GetCursorMode(const InternalWindow* window)
{
	TRAP_WINDOW_ASSERT(window, "[Window] window is nullptr!");

	return window->CursorMode;
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
	if (key != Input::Key::Unknown && key != Input::Key::Invalid)
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

	if (window->CursorMode == CursorMode::Disabled)
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

	if (window->CursorMode == CursorMode::Disabled)
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
void TRAP::INTERNAL::WindowingAPI::ShowWindow(const InternalWindow* window)
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

void TRAP::INTERNAL::WindowingAPI::RestoreWindow(const InternalWindow* window)
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

	if (window->Context.Client == ContextAPI::None)
	{
		InputError(Error::No_Window_Context, " Cannot swap buffers of a window that has no OpenGL context!");
		return;
	}

	window->Context.SwapBuffers(window);
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

	windowPtr->Context.SwapInterval(interval);
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

	windowPtr->Context.GetIntegerv(GL_NUM_EXTENSIONS, &count);

	for (uint32_t i = 0; i < static_cast<uint32_t>(count); i++)
	{
		const char* en = reinterpret_cast<const char*>(windowPtr->Context.GetStringi(GL_EXTENSIONS, i));
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
	return windowPtr->Context.ExtensionSupported(extension.data());
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

	return windowPtr->Context.GetProcAddress(procName);
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

	if (window->Context.Client != ContextAPI::None)
	{
		InputError(Error::Invalid_Value, "[Vulkan] Window surface creation requires the window to have the client API set to None");
		return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
	}

	return PlatformCreateWindowSurface(instance, window, allocator, surface);
}