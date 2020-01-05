#include "TRAPPCH.h"
#include "Window.h"

#include <glad/glad.h>

#include "Utils/MsgBox/MsgBox.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Renderer.h"
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Input/Input.h"
#include "Embed.h"

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::s_windows = 0;

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::s_WindowingAPIInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Window*> TRAP::Window::s_fullscreenWindows{};

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<uint32_t, TRAP::INTERNAL::WindowingAPI::VideoMode> TRAP::Window::s_baseVideoModes{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::Window(const WindowProps &props)
	: m_window(nullptr), m_useMonitor(nullptr)
{
	std::string init = "[Window] Initializing Window: \"" + props.Title + "\" " + std::to_string(props.Width) + 'x' +
					   std::to_string(props.Height) + "@" + std::to_string(props.RefreshRate) + "Hz VSync: ";
	if (props.VSync > 0)
		init += "On(" + std::to_string(props.VSync) + ") DisplayMode: ";
	else
		init += "Off DisplayMode: ";

	if (props.displayMode == DisplayMode::Windowed)
		init += "Windowed";
	else if (props.displayMode == DisplayMode::Borderless)
		init += "Borderless";
	else if (props.displayMode == DisplayMode::Fullscreen)
		init += "Fullscreen";

	init += " Monitor: " + std::to_string(props.Monitor) + " CursorMode: ";

	if (props.cursorMode == CursorMode::Normal)
		init += "Normal";
	else if (props.cursorMode == CursorMode::Hidden)
		init += "Hidden";
	else if (props.cursorMode == CursorMode::Disabled)
		init += "Disabled";

	init += " RawMouseInput: ";
	if(props.rawMouseInput)
		init += "Enabled";
	else
		init += "Disabled";
	
	TP_INFO(init);
	Init(props);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::~Window()
{
	if (s_windows > 1)
		Use();
	s_windows--;

	if(!s_windows)
	{
		Graphics::Renderer::Shutdown();
		Graphics::TextureManager::Shutdown();
		Graphics::ShaderManager::Shutdown();
		Graphics::API::RendererAPI::Shutdown();
		Graphics::API::Context::Shutdown();
	}	
	TP_DEBUG("[Window] Destroying Window: \"", m_data.Title, "\"");
	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::OnUpdate()
{
	INTERNAL::WindowingAPI::PollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Use(const Scope<Window>& window)
{
	if (window)
		Graphics::API::Context::Use(window);
	else
		Use();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Use()
{
	Graphics::API::Context::Use(Application::GetWindow());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetActiveWindows()
{
	return s_windows;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetMonitors()
{
	const auto monitors = INTERNAL::WindowingAPI::GetMonitors();

	return static_cast<uint32_t>(monitors.size());
}

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<uint32_t, std::string> TRAP::Window::GetMonitorNames()
{
	const auto monitors = INTERNAL::WindowingAPI::GetMonitors();

	std::unordered_map<uint32_t, std::string> result;
	
	for (uint32_t i = 0; i < monitors.size(); i++)
		result.emplace(i, INTERNAL::WindowingAPI::GetMonitorName(monitors[i]));

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Window::GetTitle() const
{
	return m_data.Title;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetWidth() const
{
	return m_data.Width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetHeight() const
{
	return m_data.Height;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetRefreshRate() const
{
	return m_data.RefreshRate;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::DisplayMode TRAP::Window::GetDisplayMode() const
{
	return m_data.displayMode;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetMonitor() const
{
	return m_data.Monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Window::GetMonitorName() const
{
	return INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetVSyncInterval() const
{
	return m_data.VSync;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::CursorMode TRAP::Window::GetCursorMode() const
{
	return m_data.cursorMode;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::GetRawMouseInput() const
{
	return m_data.RawMouseInput;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Ref<TRAP::INTERNAL::WindowingAPI::InternalWindow>& TRAP::Window::GetInternalWindow() const
{
	return m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetTitle(const std::string& title)
{
	const std::string oldTitle = m_data.Title;
	
	if (!title.empty())
		m_data.Title = title;
	else
		m_data.Title = "TRAP Engine";

#ifndef TRAP_RELEASE
	const std::string newTitle = m_data.Title + " - TRAP Engine V" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) + "." +
		std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) + "." + std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) +
		"[INDEV][20w01a2]" + std::string(Graphics::Renderer::GetTitle());
	INTERNAL::WindowingAPI::SetWindowTitle(m_window, newTitle);
#else
	INTERNAL::WindowingAPI::SetWindowTitle(m_window, m_data.Title);
#endif
	TP_DEBUG("[Window] \"", oldTitle, "\" Set Title: \"", m_data.Title, "\"");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetDisplayMode(const DisplayMode& mode,
	uint32_t width,
	uint32_t height,
	uint32_t refreshRate)
{
	//If currently windowed, stash the current size and position of the window
	if (m_data.displayMode == DisplayMode::Windowed)
	{
		m_oldWindowedParams.Width = m_data.Width;
		m_oldWindowedParams.Height = m_data.Height;
		m_oldWindowedParams.RefreshRate = m_data.RefreshRate;
		INTERNAL::WindowingAPI::GetWindowPos(m_window, m_oldWindowedParams.XPos, m_oldWindowedParams.YPos);
	}

	if((m_data.displayMode == DisplayMode::Fullscreen || m_data.displayMode == DisplayMode::Borderless) && mode == DisplayMode::Windowed)
		s_fullscreenWindows[m_data.Monitor] = nullptr;

	Ref<INTERNAL::WindowingAPI::InternalMonitor> monitor = nullptr;

	if (mode == DisplayMode::Borderless)
	{
		if(s_fullscreenWindows[m_data.Monitor])
		{
			if(s_fullscreenWindows[m_data.Monitor] == this)
				TP_WARN("[Window] \"", m_data.Title, "\" already uses DisplayMode: Borderless or Fullscreen!");
			else
				TP_ERROR("[Window] \"", m_data.Title, "\" couldn't set DisplayMode to Borderless because Monitor: ", m_data.Monitor,
					'(', INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ')', " is already used by Window: \"", s_fullscreenWindows[m_data.Monitor]->GetTitle(), "\"!");
			
			return;
		}

		s_fullscreenWindows[m_data.Monitor] = this;
		//For borderless fullscreen, the new width, height and refresh rate will be the video mode width, height and refresh rate
		width = s_baseVideoModes[m_data.Monitor].Width;
		height = s_baseVideoModes[m_data.Monitor].Height;
		refreshRate = s_baseVideoModes[m_data.Monitor].RefreshRate;
		monitor = m_useMonitor;
		TP_DEBUG("[Window] \"", m_data.Title, "\" Using Monitor: ", m_data.Monitor, '(', INTERNAL::WindowingAPI::GetMonitorName(monitor), ')');
	}
	else if (mode == DisplayMode::Windowed && (width == 0 || height == 0 || refreshRate == 0))
	{
		//For windowed, use old window height and width if none provided
		width = m_oldWindowedParams.Width;
		height = m_oldWindowedParams.Height;
		refreshRate = m_oldWindowedParams.RefreshRate;
	}
	else if (mode == DisplayMode::Fullscreen)
	{
		if (s_fullscreenWindows[m_data.Monitor] != this && s_fullscreenWindows[m_data.Monitor])
		{
			TP_ERROR("[Window] \"", m_data.Title, "\" couldn't set DisplayMode to Fullscreen because Monitor: ", m_data.Monitor,
				'(', INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ')', " is already used by Window: \"", s_fullscreenWindows[m_data.Monitor]->GetTitle(), "\"!");
			return;
		}

		s_fullscreenWindows[m_data.Monitor] = this;		
		monitor = m_useMonitor;
		bool valid = false;
		const auto monitorVideoModes = INTERNAL::WindowingAPI::GetVideoModes(m_useMonitor);
		if (width != 0 && height != 0 && refreshRate != 0)
		{
			for (const auto& monitorVideoMode : monitorVideoModes)
			{
				//Check if resolution pair is valid
				if (static_cast<uint32_t>(monitorVideoMode.Width) == width &&
					static_cast<uint32_t>(monitorVideoMode.Height) == height &&
					static_cast<uint32_t>(monitorVideoMode.RefreshRate) == refreshRate)
				{
					valid = true;
					break;
				}
				if (static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].Width) == width &&
				    static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].Height) == height &&
					static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].RefreshRate) == refreshRate)
				{
					valid = true;
					break;
				}
			}
		}

		//Resolution pair is invalid so use native/default resolution
		if (!valid)
		{
			width = s_baseVideoModes[m_data.Monitor].Width;
			height = s_baseVideoModes[m_data.Monitor].Height;
			refreshRate = s_baseVideoModes[m_data.Monitor].RefreshRate;
		}

		TP_DEBUG("[Window] \"", m_data.Title, "\" Using Monitor: ", m_data.Monitor, '(', INTERNAL::WindowingAPI::GetMonitorName(monitor), ')');
	}

	//Update stored width and height
	m_data.Width = width;
	m_data.Height = height;
	m_data.RefreshRate = refreshRate;

	//Trigger resize event
	if (m_data.EventCallback)
	{
		WindowResizeEvent event(width, height, m_data.Title);
		m_data.EventCallback(event);
	}

	constexpr auto GetModeStr = [&](const DisplayMode displayMode)
	{
		if (displayMode == DisplayMode::Windowed)
			return "Windowed";
		return displayMode == DisplayMode::Borderless ? "Borderless" : "Fullscreen";
	}; //Little hack to convert enum class DisplayMode to string
	TP_INFO("[Window] \"", m_data.Title, "\" Changing window mode from ",
		GetModeStr(m_data.displayMode), " to ", GetModeStr(mode), ": ",
		width, 'x', height, '@', refreshRate, "Hz");

	//Record new window type
	m_data.displayMode = mode;

	if(mode == DisplayMode::Borderless)
		INTERNAL::WindowingAPI::SetWindowMonitorBorderless(m_window, monitor);
	else
	{
		INTERNAL::WindowingAPI::SetWindowMonitor(m_window,
			monitor,
			m_oldWindowedParams.XPos,
			m_oldWindowedParams.YPos,
			static_cast<int32_t>(width),
			static_cast<int32_t>(height),
			static_cast<int32_t>(refreshRate));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMonitor(const uint32_t monitor)
{
	const auto monitors = INTERNAL::WindowingAPI::GetMonitors();
	if (monitor <= static_cast<uint32_t>(monitors.size()))
	{
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set Monitor: ", monitor, " Name: ", INTERNAL::WindowingAPI::GetMonitorName(monitors[monitor]));
		m_data.Monitor = monitor;
		m_useMonitor = monitors[monitor];
	}
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Invalid Monitor!");
		TP_WARN("[Window] \"", m_data.Title, "\" Using Primary Monitor!");
		m_data.Monitor = 0;
		m_useMonitor = INTERNAL::WindowingAPI::GetPrimaryMonitor();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetVSyncInterval(const uint32_t interval)
{
	Graphics::API::Context::SetVSyncInterval(interval);
	m_data.VSync = interval;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorMode(const CursorMode& mode)
{
	if (mode == CursorMode::Normal)
	{
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Normal");
		INTERNAL::WindowingAPI::SetCursorMode(m_window, INTERNAL::WindowingAPI::CursorMode::Normal);
		m_data.cursorMode = mode;
	}
	else if (mode == CursorMode::Hidden)
	{
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Hidden");
		INTERNAL::WindowingAPI::SetCursorMode(m_window, INTERNAL::WindowingAPI::CursorMode::Hidden);
		m_data.cursorMode = mode;
	}
	else if (mode == CursorMode::Disabled)
	{
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Disabled");
		INTERNAL::WindowingAPI::SetCursorMode(m_window, INTERNAL::WindowingAPI::CursorMode::Disabled);
		m_data.cursorMode = mode;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorType(const CursorType& cursor) const
{
	switch(cursor)
	{
	case CursorType::Arrow:		
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::Arrow));
		break;

	case CursorType::Input:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::Input));
		break;
		
	case CursorType::Crosshair:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::Crosshair));
		break;
		
	case CursorType::PointingHand:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::PointingHand));
		break;

	case CursorType::ResizeHorizontal:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::ResizeHorizontal));
		break;

	case CursorType::ResizeVertical:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::ResizeVertical));
		break;
		
	case CursorType::ResizeDiagonalTopLeftBottomRight:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::ResizeDiagonalTopLeftBottomRight));
		break;

	case CursorType::ResizeDiagonalTopRightBottomLeft:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::ResizeDiagonalTopRightBottomLeft));
		break;
		
	case CursorType::ResizeAll:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::ResizeAll));
		break;
		
	case CursorType::NotAllowed:
		INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateStandardCursor(INTERNAL::WindowingAPI::CursorType::NotAllowed));
		break;
		
	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorIcon(const Scope<Image>& image) const
{
	INTERNAL::WindowingAPI::SetCursor(m_window, INTERNAL::WindowingAPI::CreateCursor(image, 0, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetRawMouseInput(const bool enabled)
{
	if(Input::IsRawMouseInputSupported())
	{
		m_data.RawMouseInput = enabled;
		INTERNAL::WindowingAPI::SetRawMouseMotionMode(m_window, enabled);
		TP_DEBUG("[Window] \"", m_data.Title, "\" Raw Mouse Input ", enabled ? "Enabled" : "Disabled");
	}
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Raw Mouse Input is unsupported!");
		m_data.RawMouseInput = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetIcon() const
{
	const std::vector<uint8_t> TRAPLogo{ Embed::TRAPLogo.begin(), Embed::TRAPLogo.end() };
	INTERNAL::WindowingAPI::SetWindowIcon(m_window, Image::LoadFromMemory(32, 32, 32, ImageFormat::RGBA, TRAPLogo));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetIcon(const Scope<Image>& image) const
{
	if (!image)
	{
		SetIcon();
		return;
	}
	if (image->IsHDR())
	{
		TP_ERROR("[Window][Icon] \"", m_data.Title, "\" HDR is not supported for window icons!");
		TP_WARN("[Window][Icon] \"", m_data.Title, "\" Using Default Icon!");
		SetIcon();
		return;
	}
	if ((image->IsImageGrayScale() && image->GetBitsPerPixel() == 16 && !image->HasAlphaChannel()) ||
		(image->IsImageGrayScale() && image->GetBitsPerPixel() == 24 && image->HasAlphaChannel()) ||
		(image->IsImageColored() && image->GetBitsPerPixel() == 48 && !image->HasAlphaChannel()) ||
		(image->IsImageColored() && image->GetBitsPerPixel() == 64 && image->HasAlphaChannel()))
	{
		TP_ERROR("[Window][Icon] \"", m_data.Title, "\" Images with short pixel data are not supported for window icons!");
		TP_WARN("[Window][Icon] \"", m_data.Title, "\" Using Default Icon!");
		SetIcon();
		return;
	}
	if (image->GetFormat() != ImageFormat::RGBA)
	{
		TP_ERROR("[Window][Icon] \"", m_data.Title, "\" Only RGBA Images are supported for window icons!");
		TP_WARN("[Window][Icon] \"", m_data.Title, "\" Using Default Icon!");
		SetIcon();
		return;
	}

	INTERNAL::WindowingAPI::SetWindowIcon(m_window, image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetEventCallback(const EventCallbackFn& callback)
{
	m_data.EventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsMaximized() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window, INTERNAL::WindowingAPI::Hint::Maximized);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsMinimized() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window, INTERNAL::WindowingAPI::Hint::Minimized);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsResizable() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window, INTERNAL::WindowingAPI::Hint::Resizable);
}

//-------------------------------------------------------------------------------------------------------------------//

static void WindowingAPIErrorCallback(const TRAP::INTERNAL::WindowingAPI::Error error, const std::string& description)
{
	if(error != TRAP::INTERNAL::WindowingAPI::Error::No_Error)
		TP_ERROR("[Window][Internal] ", description);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Init(const WindowProps& props)
{
	m_data.Title = props.Title;
	m_data.Width = props.Width;
	m_data.Height = props.Height;
	m_data.RefreshRate = props.RefreshRate;
	m_data.VSync = props.VSync;
	m_data.Monitor = props.Monitor;
	m_data.cursorMode = props.cursorMode;
	m_data.RawMouseInput = props.rawMouseInput;
	m_data.windowModeParams = &m_oldWindowedParams;

	if (!s_WindowingAPIInitialized)
	{
		const int32_t success = INTERNAL::WindowingAPI::Init();
		TRAP_CORE_ASSERT(success, "Could not initialize GLFW!");
		INTERNAL::WindowingAPI::SetErrorCallback(WindowingAPIErrorCallback);
		s_WindowingAPIInitialized = true;

		Graphics::API::Context::CheckAllRenderAPIs();
	}

	if(s_fullscreenWindows.empty())
	{
		const auto monitors = INTERNAL::WindowingAPI::GetMonitors();
		s_fullscreenWindows.resize(monitors.size());
	}
	
	const auto monitors = INTERNAL::WindowingAPI::GetMonitors();
	if (props.Monitor < monitors.size())
		m_useMonitor = monitors[props.Monitor];
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Invalid Monitor!");
		TP_WARN("[Window] \"", m_data.Title, "\" Using Primary Monitor!");
		m_data.Monitor = 0;
		m_useMonitor = INTERNAL::WindowingAPI::GetPrimaryMonitor();
	}

	if(s_baseVideoModes.empty())
		for(uint32_t i = 0; i < monitors.size(); i++)
		{
			s_baseVideoModes[i] = INTERNAL::WindowingAPI::GetVideoMode(monitors[i]);
			TP_DEBUG("[Window] Storing underlying OS video mode: Monitor: ", i, " ",
			s_baseVideoModes[i].Width, 'x', s_baseVideoModes[i].Height, '@', s_baseVideoModes[i].RefreshRate, "Hz (R",
			s_baseVideoModes[i].RedBits, 'G', s_baseVideoModes[i].GreenBits, 'B', s_baseVideoModes[i].BlueBits, ')');			
		}

	INTERNAL::WindowingAPI::DefaultWindowHints();

	if (!s_windows)
	{
		if (props.RenderAPI == Graphics::API::RenderAPI::NONE)
			Graphics::API::Context::AutoSelectRenderAPI();
		else
		{
			if (Graphics::API::Context::IsSupported(props.RenderAPI))
				Graphics::API::Context::SetRenderAPI(props.RenderAPI);
			else
			{
				if (Graphics::API::Context::IsD3D12Capable())
					Graphics::API::Context::SetRenderAPI(Graphics::API::RenderAPI::D3D12);
				else if (Graphics::API::Context::IsVulkanCapable())
					Graphics::API::Context::SetRenderAPI(Graphics::API::RenderAPI::Vulkan);
				else if (Graphics::API::Context::IsOpenGLCapable())
					Graphics::API::Context::SetRenderAPI(Graphics::API::RenderAPI::OpenGL);
				else
				{
					//All RenderAPIs are unsupported
					Show("Every RenderAPI that TRAP Engine uses is unsupported on your device!\nDoes your system meet the minimum system requirements for running TRAP Engine?",
						"Incompatible Device",
						Utils::MsgBox::Style::Error,
						Utils::MsgBox::Buttons::Quit);
					exit(-1);
				}
			}
		}
		if (Graphics::API::Context::GetRenderAPI() != Graphics::API::RenderAPI::OpenGL)
			INTERNAL::WindowingAPI::SetContextAPI(INTERNAL::WindowingAPI::ContextAPI::None);
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
			INTERNAL::WindowingAPI::SetContextAPI(INTERNAL::WindowingAPI::ContextAPI::OpenGL);
	}

	if (props.displayMode == DisplayMode::Windowed)
	{
		if(props.Maximized)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Maximized, true);
		if (!props.Resizable)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Resizable, false);
	}
	
	//Create Window
#ifndef TRAP_RELEASE
	std::string newTitle = m_data.Title + " - TRAP Engine V" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) + "." +
		std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) + "." + std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) +
		"[INDEV][20w01a2]";
#else
	const std::string newTitle = m_data.Title;
#endif

	m_window = INTERNAL::WindowingAPI::CreateWindow(static_cast<int32_t>(props.Width),
		static_cast<int32_t>(props.Height),
		newTitle,
		nullptr,
		nullptr);

	INTERNAL::WindowingAPI::GetWindowSize(m_window, m_data.Width, m_data.Height);

	if (!m_window)
	{
		TP_CRITICAL("[Window] Failed to create window");
		Show("Failed to create Window!", "Failed to Create Window", Utils::MsgBox::Style::Error, Utils::MsgBox::Buttons::Quit);
		exit(-1);
	}

	if (!s_windows)
	{
		//Create Context & Initialize Renderer
		Graphics::API::Context::Create(this);
		Graphics::API::Context::SetVSyncInterval(props.VSync);
		Graphics::API::RendererAPI::Init();
	}

	s_windows++;
	if (s_windows > 1)
	{
		Scope<Window> window(this); //Little Hack so this pointer doesnt get invalidated :/
		Use(window);
		auto unused = window.release();
		SetVSyncInterval(props.VSync);
	}

	//Update Window Title
#ifndef TRAP_RELEASE
	newTitle += Graphics::Renderer::GetTitle();
#endif
	INTERNAL::WindowingAPI::SetWindowTitle(m_window, newTitle);

	int32_t width = 0, height = 0, refreshRate = 0;
	
	//If currently windowed, stash the current size and position of the window
	if (m_data.displayMode == DisplayMode::Windowed)
	{
		m_oldWindowedParams.Width = m_data.Width;
		m_oldWindowedParams.Height = m_data.Height;
		m_oldWindowedParams.RefreshRate = m_data.RefreshRate;
		INTERNAL::WindowingAPI::GetWindowPos(m_window, m_oldWindowedParams.XPos, m_oldWindowedParams.YPos);
	}

	Ref<INTERNAL::WindowingAPI::InternalMonitor> monitor = nullptr;

	if (props.displayMode == DisplayMode::Borderless)
	{
		if (s_fullscreenWindows[m_data.Monitor])
		{
			TP_ERROR("[Window] \"", m_data.Title, "\" couldn't set DisplayMode to Borderless because Monitor: ", m_data.Monitor,
				'(', INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ')', " is already used by Window: \"", s_fullscreenWindows[m_data.Monitor]->GetTitle(), "\"!");
			TP_WARN("[Window] \"", m_data.Title, "\" is now using DisplayMode Windowed!");
			m_data.displayMode = DisplayMode::Windowed;
			width = 800;
			height = 600;
			refreshRate = s_baseVideoModes[m_data.Monitor].RefreshRate;
		}
		else
		{
			s_fullscreenWindows[m_data.Monitor] = this;

			//For borderless fullscreen, the new width, height and refresh rate will be the video mode width, height and refresh rate
			width = s_baseVideoModes[m_data.Monitor].Width;
			height = s_baseVideoModes[m_data.Monitor].Height;
			refreshRate = s_baseVideoModes[m_data.Monitor].RefreshRate;
			monitor = m_useMonitor;
		}
	}
	else if (props.displayMode == DisplayMode::Windowed)
	{		
		width = m_oldWindowedParams.Width;
		height = m_oldWindowedParams.Height;
		refreshRate = m_oldWindowedParams.RefreshRate;				
	}
	else if (props.displayMode == DisplayMode::Fullscreen)
	{
		if (s_fullscreenWindows[m_data.Monitor])
		{
			TP_ERROR("[Window] \"", m_data.Title, "\" couldn't set DisplayMode to Fullscreen because Monitor: ", m_data.Monitor,
				'(', INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ')', " is already used by Window: \"", s_fullscreenWindows[m_data.Monitor]->GetTitle(), "\"!");
			TP_WARN("[Window] \"", m_data.Title, "\" is now using DisplayMode Windowed!");
			m_data.displayMode = DisplayMode::Windowed;
			width = 800;
			height = 600;
			refreshRate = s_baseVideoModes[m_data.Monitor].RefreshRate;
		}
		else
		{
			s_fullscreenWindows[m_data.Monitor] = this;

			monitor = m_useMonitor;
			bool valid = false;
			const auto monitorVideoModes = INTERNAL::WindowingAPI::GetVideoModes(m_useMonitor);
			if (width != 0 && height != 0 && refreshRate != 0)
			{
				for (const auto& monitorVideoMode : monitorVideoModes)
				{
					//Check if resolution pair is valid
					if ((monitorVideoMode.Width == width &&
							monitorVideoMode.Height == height) &&
						monitorVideoMode.RefreshRate == refreshRate)
					{
						valid = true;
						break;
					}
					if (s_baseVideoModes[m_data.Monitor].Width == width &&
						s_baseVideoModes[m_data.Monitor].Height == height &&
						s_baseVideoModes[m_data.Monitor].RefreshRate == refreshRate)
					{
						valid = true;
						break;
					}
				}
			}

			//Resolution pair is invalid so use native/default resolution
			if (!valid)
			{
				width = s_baseVideoModes[m_data.Monitor].Width;
				height = s_baseVideoModes[m_data.Monitor].Height;
				refreshRate = s_baseVideoModes[m_data.Monitor].RefreshRate;
			}
		}
	}

	//Update stored width and height
	m_data.Width = width;
	m_data.Height = height;
	m_data.RefreshRate = refreshRate;

	//Record new window type
	m_data.displayMode = props.displayMode;
	

	if (m_data.displayMode == DisplayMode::Borderless)
		INTERNAL::WindowingAPI::SetWindowMonitorBorderless(m_window, monitor);
	else
	{
		INTERNAL::WindowingAPI::SetWindowMonitor(m_window,
			monitor,
			m_oldWindowedParams.XPos,
			m_oldWindowedParams.YPos,
			width,
			height,
			refreshRate);
	}

	INTERNAL::WindowingAPI::GetFrameBufferSize(m_window, width, height);
	Graphics::RenderCommand::SetViewport(0, 0, width, height);
	
	INTERNAL::WindowingAPI::SetWindowUserPointer(m_window, &m_data);

	SetIcon();

	if (m_data.cursorMode == CursorMode::Normal)
		INTERNAL::WindowingAPI::SetCursorMode(m_window, INTERNAL::WindowingAPI::CursorMode::Normal);
	else if (m_data.cursorMode == CursorMode::Hidden)
		INTERNAL::WindowingAPI::SetCursorMode(m_window, INTERNAL::WindowingAPI::CursorMode::Hidden);
	else if (m_data.cursorMode == CursorMode::Disabled)
		INTERNAL::WindowingAPI::SetCursorMode(m_window, INTERNAL::WindowingAPI::CursorMode::Disabled);
	
	if (Input::IsRawMouseInputSupported())
		INTERNAL::WindowingAPI::SetRawMouseMotionMode(m_window, m_data.RawMouseInput);
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Raw Mouse Input is unsupported!");
		m_data.RawMouseInput = false;
	}

	//Set WindowingAPI callbacks
	INTERNAL::WindowingAPI::SetWindowSizeCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window, const int32_t width, const int32_t height)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));
		data.Width = width;
		data.Height = height;

		if (!data.EventCallback)
			return;

		WindowResizeEvent event(width, height, data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetWindowPosCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window, const int32_t x, const int32_t y)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if(data.displayMode == DisplayMode::Windowed)
		{
			data.windowModeParams->XPos = x;
			data.windowModeParams->YPos = y;			
		}

		if (!data.EventCallback)
			return;
		
		WindowMovedEvent event(x, y, data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetWindowFocusCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window, const bool focused)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		if (focused)
		{
			WindowFocusEvent event(data.Title);
			data.EventCallback(event);
		}
		else
		{
			WindowLostFocusEvent event(data.Title);
			data.EventCallback(event);
		}
	});

	INTERNAL::WindowingAPI::SetWindowCloseCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		WindowCloseEvent event(data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetKeyCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window, const Input::Key key, int32_t scancode, const bool pressed)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if(pressed)
		{
			if(data.KeyRepeatCounts.find(static_cast<uint16_t>(key)) == data.KeyRepeatCounts.end())
			{
				data.KeyRepeatCounts[static_cast<uint16_t>(key)] = 0;

				if (!data.EventCallback)
					return;
				
				KeyPressedEvent event(static_cast<Input::Key>(key), 0, data.Title);
				data.EventCallback(event);
			}
			else
			{
				data.KeyRepeatCounts[static_cast<uint16_t>(key)]++;

				if (!data.EventCallback)
					return;
				
				KeyPressedEvent event(static_cast<Input::Key>(key), data.KeyRepeatCounts[static_cast<uint16_t>(key)], data.Title);
				data.EventCallback(event);
			}
		}
		else
		{
			data.KeyRepeatCounts.erase(static_cast<uint16_t>(key));

			if (!data.EventCallback)
				return;
			
			KeyReleasedEvent event(static_cast<Input::Key>(key), data.Title);
			data.EventCallback(event);
		}
	});

	INTERNAL::WindowingAPI::SetCharCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window, const uint32_t keycode)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		KeyTypedEvent event(static_cast<Input::Key>(keycode), data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetMouseButtonCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window, const Input::MouseButton button, const bool pressed)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		if (pressed)
		{
			MouseButtonPressedEvent event(static_cast<Input::MouseButton>(button), data.Title);
			data.EventCallback(event);
		}
		else
		{
			MouseButtonReleasedEvent event(static_cast<Input::MouseButton>(button), data.Title);
			data.EventCallback(event);
		}
	});

	INTERNAL::WindowingAPI::SetScrollCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window, const double xOffset, const double yOffset)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset), data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetCursorPosCallback(m_window, [](Ref<INTERNAL::WindowingAPI::InternalWindow> window, const double xPos, const double yPos)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos), data.Title);
		data.EventCallback(event);
	});

	if (s_windows > 1)
		Use();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Shutdown()
{
	INTERNAL::WindowingAPI::DestroyWindow(m_window);
	m_window = nullptr;
	if (!s_windows)
		INTERNAL::WindowingAPI::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowProps::WindowProps(std::string title,
	                           const uint32_t width,
	                           const uint32_t height,
	                           const uint32_t refreshRate,
	                           const uint32_t vsync,
	                           const Window::DisplayMode displayMode,
							   const bool maximized,
							   const bool resizable,
	                           const uint32_t monitor,
	                           const Window::CursorMode cursorMode,
	                           const bool rawMouseInput)
	: Title(std::move(title)),
	  Width(width),
	  Height(height),
	  RefreshRate(refreshRate),
	  VSync(vsync),
	  RenderAPI(Graphics::API::Context::GetRenderAPI()),
	  displayMode(displayMode),
	  Maximized(maximized),
	  Resizable(resizable),
	  Monitor(monitor),
	  cursorMode(cursorMode),
	  rawMouseInput(rawMouseInput)
{
}