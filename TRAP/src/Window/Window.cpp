#include "TRAPPCH.h"
#include "Window.h"

#include "Utils/MsgBox/MsgBox.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/WindowEvent.h"
#include "Event/MonitorEvent.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Input/Input.h"
#include "Embed.h"
#include "Layers/ImGui/ImGuiWindowing.h"

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::s_windows = 0;

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::s_WindowingAPIInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Window*> TRAP::Window::s_fullscreenWindows{};

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<uint32_t, TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::Window::s_baseVideoModes{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::Window(const WindowProps &props)
	: m_window(nullptr), m_useMonitor(nullptr)
{
	TP_INFO
	(
		"[Window] Initializing Window: \"", props.Title, "\" ", props.Width, "x", props.Height, "@", props.RefreshRate, "Hz VSync: ",
		props.advanced.VSync > 0 ? "On" : "Off", "(", props.advanced.VSync, //Output VSync status
		") DisplayMode: ", props.displayMode == DisplayMode::Windowed ? "Windowed" : props.displayMode == DisplayMode::Borderless ? "Borderless" : "Fullscreen", //Output DisplayMode
		" Monitor: ", props.Monitor,
		" CursorMode: ", props.advanced.CursorMode == CursorMode::Normal ? "Normal" : props.advanced.CursorMode == CursorMode::Hidden ? "Hidden" : "Disabled", //Output CursorMode
		" RawMouseInput: ", props.advanced.RawMouseInput ? "Enabled" : "Disabled"
	);
	
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
		Graphics::API::Context::Use(window.get());
	else
		Use();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Use()
{
	Graphics::API::Context::Use(Application::GetWindow().get());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetActiveWindows()
{
	return s_windows;
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

TRAP::Math::Vec2ui TRAP::Window::GetSize() const
{
	return { m_data.Width, m_data.Height };
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

TRAP::Monitor TRAP::Window::GetMonitor() const
{
	return Monitor(m_data.Monitor);
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

TRAP::Math::Vec2 TRAP::Window::GetContentScale() const
{
	Math::Vec2 contentScale{};
	INTERNAL::WindowingAPI::GetWindowContentScale(m_window.get(), contentScale.x, contentScale.y);
	
	return contentScale;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Window::GetOpacity() const
{
	return INTERNAL::WindowingAPI::GetWindowOpacity(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::Window::GetInternalWindow() const
{
	return m_window.get();
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
	std::string newTitle = m_data.Title + " - TRAP Engine V" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) + "." +
		std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) + "." + std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) +
		"[INDEV][20w08a5]" + std::string(Graphics::Renderer::GetTitle());
#ifdef TRAP_PLATFORM_LINUX
	if (Application::GetLinuxWindowManager() == Application::LinuxWindowManager::Wayland)
		newTitle += "[Wayland]";
	else if (Application::GetLinuxWindowManager() == Application::LinuxWindowManager::X11)
		newTitle += "[X11]";
	else
		newTitle += "[Unknown]";
#endif
	INTERNAL::WindowingAPI::SetWindowTitle(m_window.get(), newTitle);
#else
	INTERNAL::WindowingAPI::SetWindowTitle(m_window.get(), m_data.Title);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetDisplayMode(const DisplayMode& mode,
	uint32_t width,
	uint32_t height,
	uint32_t refreshRate)
{
	//Only change windowed mode if resolution and refresh rate changed
	if(m_data.displayMode == DisplayMode::Windowed && mode == DisplayMode::Windowed)
	{
		if (m_data.Width == static_cast<int32_t>(width) && m_data.Height == static_cast<int32_t>(height))
		{
			//Only update refresh rate used for Borderless or Fullscreen
			if (m_data.RefreshRate != static_cast<int32_t>(refreshRate))
			{
				m_oldWindowedParams.RefreshRate = m_data.RefreshRate;
				m_data.RefreshRate = refreshRate;
			}

			TP_WARN("[Window] \"", m_data.Title, "\" already uses DisplayMode Windowed!");
			return;
		}
	}
	
	//If currently windowed, stash the current size and position of the window
	if (m_data.displayMode == DisplayMode::Windowed)
	{
		m_oldWindowedParams.Width = m_data.Width;
		m_oldWindowedParams.Height = m_data.Height;
		m_oldWindowedParams.RefreshRate = m_data.RefreshRate;
		INTERNAL::WindowingAPI::GetWindowPos(m_window.get(), m_oldWindowedParams.XPos, m_oldWindowedParams.YPos);
	}

	if(mode == DisplayMode::Windowed)
		s_fullscreenWindows[m_data.Monitor] = nullptr;

	INTERNAL::WindowingAPI::InternalMonitor* monitor = nullptr;

	if (mode == DisplayMode::Borderless)
	{
		if(s_fullscreenWindows[m_data.Monitor])
		{
			//Check if Monitor is already used by another window
			if (s_fullscreenWindows[m_data.Monitor] != this) 
			{
				TP_ERROR("[Window] \"", m_data.Title, "\" couldn't set DisplayMode to Borderless because Monitor: ", m_data.Monitor,
					'(', INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ')', " is already used by Window: \"", s_fullscreenWindows[m_data.Monitor]->GetTitle(), "\"!");
				return;
			}
			//Check if Monitor is used by this window and display mode should be set to borderless
			if(s_fullscreenWindows[m_data.Monitor] == this && m_data.displayMode == DisplayMode::Borderless) 
			{
				TP_WARN("[Window] \"", m_data.Title, "\" already uses DisplayMode Borderless on Monitor: ", m_data.Monitor, "(", INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ")!");
				return;
			}			
		}

		s_fullscreenWindows[m_data.Monitor] = this;
		//For borderless fullscreen, the new width, height and refresh rate will be the default/native video modes width, height and refresh rate
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
		if (s_fullscreenWindows[m_data.Monitor] && s_fullscreenWindows[m_data.Monitor] != this)
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
			//Check if current running mode is same as target mode
			if (static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].Width) == width &&
				static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].Height) == height &&
				static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].RefreshRate) == refreshRate)
				valid = true;

			if (!valid) //If not check every video mode of the monitor
			{
				for (const auto& monitorVideoMode : monitorVideoModes)
				{
					//Check if resolution pair is valid and break if found
					if (static_cast<uint32_t>(monitorVideoMode.Width) == width &&
						static_cast<uint32_t>(monitorVideoMode.Height) == height &&
						static_cast<uint32_t>(monitorVideoMode.RefreshRate) == refreshRate)
					{
						valid = true;
						break;
					}
				}
			}
		}

		//Resolution pair is still invalid so use native/default resolution
		if (!valid)
		{
			width = s_baseVideoModes[m_data.Monitor].Width;
			height = s_baseVideoModes[m_data.Monitor].Height;
			refreshRate = s_baseVideoModes[m_data.Monitor].RefreshRate;
		}

		//Needed to switch from borderless to fullscreen directly
		if (m_data.displayMode == DisplayMode::Borderless)
			INTERNAL::WindowingAPI::SetWindowMonitor(m_window.get(),
				nullptr,
				m_oldWindowedParams.XPos,
				m_oldWindowedParams.YPos,
				static_cast<int32_t>(width),
				static_cast<int32_t>(height),
				static_cast<int32_t>(refreshRate));

		TP_DEBUG("[Window] \"", m_data.Title, "\" Using Monitor: ", m_data.Monitor, '(', INTERNAL::WindowingAPI::GetMonitorName(monitor), ')');
	}

	//Update stored width and height
	m_data.Width = width;
	m_data.Height = height;
	m_data.RefreshRate = refreshRate;

	//Set Refresh Rate to the current used VideoMode to prevent RefreshRate = 0 inside Engine.cfg
	if (m_data.displayMode == DisplayMode::Windowed)
	{
		const INTERNAL::WindowingAPI::InternalVideoMode videoMode = INTERNAL::WindowingAPI::GetVideoMode(INTERNAL::WindowingAPI::GetMonitors()[m_data.Monitor]);
		m_data.RefreshRate = videoMode.RefreshRate;
	}

	//Trigger resize event
	if (m_data.EventCallback)
	{
		WindowResizeEvent event(width, height, m_data.Title);
		FrameBufferResizeEvent event1(width, height, m_data.Title);
		m_data.EventCallback(event);
		m_data.EventCallback(event1);
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
		INTERNAL::WindowingAPI::SetWindowMonitorBorderless(m_window.get(), monitor);
	else
	{
		INTERNAL::WindowingAPI::SetWindowMonitor(m_window.get(),
			monitor,
			m_oldWindowedParams.XPos,
			m_oldWindowedParams.YPos,
			static_cast<int32_t>(width),
			static_cast<int32_t>(height),
			static_cast<int32_t>(refreshRate));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMonitor(Monitor& monitor)
{
	const uint32_t oldMonitor = m_data.Monitor;
	
	TP_DEBUG("[Window] \"", m_data.Title, "\" Set Monitor: ", monitor.GetID(), " Name: ", monitor.GetName());
	m_data.Monitor = monitor.GetID();
	m_useMonitor = static_cast<INTERNAL::WindowingAPI::InternalMonitor*>(monitor.GetInternalMonitor());

	s_fullscreenWindows[oldMonitor] = nullptr;
	if (m_data.displayMode != DisplayMode::Windowed)
	{
		if (s_fullscreenWindows[m_data.Monitor]) //Monitor already has a Fullscreen/Borderless Window
		{
			TP_ERROR("[Window] Monitor: ", m_data.Monitor, "(", monitor.GetName(), ") is already used by another Window!");
			SetDisplayMode(DisplayMode::Windowed, 800, 600, 0);
		}
		else
			SetDisplayMode(m_data.displayMode, m_data.Width, m_data.Height, m_data.RefreshRate);
	}
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetVSyncInterval(const uint32_t interval)
{	
	Graphics::API::Context::SetVSyncInterval(interval);
	m_data.VSync = interval;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorMode(const CursorMode& mode)
{
	if (mode == CursorMode::Normal)
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Normal");
	else if (mode == CursorMode::Hidden)
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Hidden");
	else if (mode == CursorMode::Disabled)
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Disabled");
	else if(mode == CursorMode::Captured)
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Captured");

	INTERNAL::WindowingAPI::SetCursorMode(m_window.get(), mode);
	m_data.cursorMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorType(const CursorType& cursor) const
{
	Scope<INTERNAL::WindowingAPI::InternalCursor> internalCursor = INTERNAL::WindowingAPI::CreateStandardCursor(cursor);
	INTERNAL::WindowingAPI::SetCursor(m_window.get(), internalCursor.get());
	INTERNAL::ImGuiWindowing::SetCustomCursor(internalCursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorIcon(const Scope<Image>& image, const int32_t xHotspot, const int32_t yHotspot) const
{
	Scope<INTERNAL::WindowingAPI::InternalCursor> cursor = INTERNAL::WindowingAPI::CreateCursor(image, xHotspot, yHotspot);
	INTERNAL::WindowingAPI::SetCursor(m_window.get(), cursor.get());
	INTERNAL::ImGuiWindowing::SetCustomCursor(cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetRawMouseInput(const bool enabled)
{
	if(Input::IsRawMouseInputSupported())
	{
		m_data.RawMouseInput = enabled;
		INTERNAL::WindowingAPI::SetRawMouseMotionMode(m_window.get(), enabled);
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
	INTERNAL::WindowingAPI::SetWindowIcon(m_window.get(), Image::LoadFromMemory(32, 32, 32, ImageFormat::RGBA, TRAPLogo));
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

	INTERNAL::WindowingAPI::SetWindowIcon(m_window.get(), image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetEventCallback(const EventCallbackFn& callback)
{
	m_data.EventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetResizable(const bool enabled) const
{
	INTERNAL::WindowingAPI::SetWindowAttrib(m_window.get(), INTERNAL::WindowingAPI::Hint::Resizable, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMinimumSize(const uint32_t minWidth, const uint32_t minHeight) const
{
	if (minWidth == 0 && minHeight == 0)
		INTERNAL::WindowingAPI::SetWindowSizeLimits(m_window.get(), -1, -1, -1, -1);
	else
		INTERNAL::WindowingAPI::SetWindowSizeLimits(m_window.get(), minWidth, minHeight, -1, -1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMaximumSize(const uint32_t maxWidth, const uint32_t maxHeight) const
{
	if (maxWidth == 0 && maxHeight == 0)
		INTERNAL::WindowingAPI::SetWindowSizeLimits(m_window.get(), -1, -1, -1, -1);
	else
		INTERNAL::WindowingAPI::SetWindowSizeLimits(m_window.get(), -1, -1, maxWidth, maxHeight);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetOpacity(const float opacity) const
{
	if(opacity >= 0.0f || opacity <= 1.0f)
	{
		INTERNAL::WindowingAPI::SetWindowOpacity(m_window.get(), opacity);		
	}
	else
	{
		TP_ERROR("[Window] Invalid Window Opacity: ", opacity, "! Valid Range: 0.0 - 1.0f");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsMaximized() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window.get(), INTERNAL::WindowingAPI::Hint::Maximized);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsMinimized() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window.get(), INTERNAL::WindowingAPI::Hint::Minimized);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsResizable() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window.get(), INTERNAL::WindowingAPI::Hint::Resizable);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsVisible() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window.get(), INTERNAL::WindowingAPI::Hint::Visible);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsFocused() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window.get(), INTERNAL::WindowingAPI::Hint::Focused);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsDecorated() const
{
	return INTERNAL::WindowingAPI::GetWindowAttrib(m_window.get(), INTERNAL::WindowingAPI::Hint::Decorated);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Maximize() const
{
	if(m_data.displayMode == DisplayMode::Windowed)
		INTERNAL::WindowingAPI::MaximizeWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Minimize() const
{
	INTERNAL::WindowingAPI::MinimizeWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::RequestAttention() const
{	
	INTERNAL::WindowingAPI::RequestWindowAttention(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Focus() const
{
	INTERNAL::WindowingAPI::FocusWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Hide() const
{
	INTERNAL::WindowingAPI::HideWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Show() const
{
	INTERNAL::WindowingAPI::ShowWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Restore() const
{
	INTERNAL::WindowingAPI::RestoreWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

static void WindowingAPIErrorCallback(const TRAP::INTERNAL::WindowingAPI::Error error, const std::string& description)
{
	if(error != TRAP::INTERNAL::WindowingAPI::Error::No_Error)
		TP_ERROR("[Window][Internal]", description);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Init(const WindowProps& props)
{
	m_data.Title = props.Title;
	m_data.Width = props.Width;
	m_data.Height = props.Height;
	m_data.RefreshRate = props.RefreshRate;
	m_data.VSync = props.advanced.VSync;
	m_data.Monitor = props.Monitor;
	m_data.cursorMode = props.advanced.CursorMode;
	m_data.RawMouseInput = props.advanced.RawMouseInput;
	m_data.windowModeParams = &m_oldWindowedParams;

	if (!s_WindowingAPIInitialized)
	{
		const int32_t success = INTERNAL::WindowingAPI::Init();
		TRAP_CORE_ASSERT(success, "Could not initialize WindowingAPI!");
		if (!success)
			Utils::MsgBox::Show("Could not initialize WindowingAPI!", "Error WindowingAPI", Utils::MsgBox::Style::Error, Utils::MsgBox::Buttons::Quit);
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
	if (props.Monitor < static_cast<uint32_t>(monitors.size()))
		m_useMonitor = monitors[props.Monitor];
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Invalid Monitor!");
		TP_WARN("[Window] \"", m_data.Title, "\" Using Primary Monitor!");
		m_data.Monitor = 0;
		m_useMonitor = INTERNAL::WindowingAPI::GetPrimaryMonitor().get();
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
					TRAP::Utils::MsgBox::Show("Every RenderAPI that TRAP Engine uses is unsupported on your device!\nDoes your system meet the minimum system requirements for running TRAP Engine?",
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
		if(props.advanced.Maximized)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Maximized, true);
		if (!props.advanced.Resizable)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Resizable, false);
		if (!props.advanced.Visible)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Visible, false);
		if (!props.advanced.Decorated)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Decorated, false);
		if (!props.advanced.Focused && !props.advanced.Visible)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Focused, false);
	}
	if (!props.advanced.FocusOnShow)
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::FocusOnShow, false);
	
	//Create Window
#ifndef TRAP_RELEASE
	std::string newTitle = m_data.Title + " - TRAP Engine V" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) + "." +
		std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) + "." + std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) +
		"[INDEV][20w08a5]";
#else
	const std::string newTitle = m_data.Title;
#endif

	m_window = INTERNAL::WindowingAPI::CreateWindow(static_cast<int32_t>(props.Width),
		static_cast<int32_t>(props.Height),
		newTitle,
		nullptr,
		nullptr);	

	if (!m_window)
	{
		TP_CRITICAL("[Window] Failed to create window");
		TRAP::Utils::MsgBox::Show("Failed to create Window!", "Failed to Create Window", Utils::MsgBox::Style::Error, Utils::MsgBox::Buttons::Quit);
		exit(-1);
	}
	
	INTERNAL::WindowingAPI::GetWindowSize(m_window.get(), m_data.Width, m_data.Height);

	if (!s_windows)
	{
		//Create Context & Initialize Renderer
		Graphics::API::Context::Create(this);
		Graphics::API::Context::SetVSyncInterval(props.advanced.VSync);
		Graphics::API::RendererAPI::Init();
	}

	s_windows++;
	if (s_windows > 1)
	{
		Graphics::API::Context::Use(this);
		SetVSyncInterval(props.advanced.VSync);
	}

	//Update Window Title
#ifndef TRAP_RELEASE
	newTitle += Graphics::Renderer::GetTitle();
#endif
#ifdef TRAP_PLATFORM_LINUX
	if (Application::GetLinuxWindowManager() == Application::LinuxWindowManager::Wayland)
		newTitle += "[Wayland]";
	else if (Application::GetLinuxWindowManager() == Application::LinuxWindowManager::X11)
		newTitle += "[X11]";
	else
		newTitle += "[Unknown]";
#endif
	INTERNAL::WindowingAPI::SetWindowTitle(m_window.get(), newTitle);

	int32_t width = 0, height = 0, refreshRate = 0;
	
	//If currently windowed, stash the current size and position of the window
	if (m_data.displayMode == DisplayMode::Windowed)
	{
		m_oldWindowedParams.Width = m_data.Width;
		m_oldWindowedParams.Height = m_data.Height;
		m_oldWindowedParams.RefreshRate = m_data.RefreshRate;
		INTERNAL::WindowingAPI::GetWindowPos(m_window.get(), m_oldWindowedParams.XPos, m_oldWindowedParams.YPos);
	}

	INTERNAL::WindowingAPI::InternalMonitor* monitor = nullptr;

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
		INTERNAL::WindowingAPI::SetWindowMonitorBorderless(m_window.get(), monitor);
	else
	{
		INTERNAL::WindowingAPI::SetWindowMonitor(m_window.get(),
			monitor,
			m_oldWindowedParams.XPos,
			m_oldWindowedParams.YPos,
			width,
			height,
			refreshRate);
	}

	INTERNAL::WindowingAPI::GetFrameBufferSize(m_window.get(), width, height);

	//Set Refresh Rate to the current used VideoMode to prevent RefreshRate = 0 inside Engine.cfg
	if(m_data.displayMode == DisplayMode::Windowed)
	{
		const INTERNAL::WindowingAPI::InternalVideoMode videoMode = INTERNAL::WindowingAPI::GetVideoMode(INTERNAL::WindowingAPI::GetMonitors()[m_data.Monitor]);
		m_data.RefreshRate = videoMode.RefreshRate;
	}
	
	Graphics::RenderCommand::SetViewport(0, 0, width, height);
	
	INTERNAL::WindowingAPI::SetWindowUserPointer(m_window.get(), &m_data);

	SetIcon();

	if (m_data.cursorMode == CursorMode::Normal)
		INTERNAL::WindowingAPI::SetCursorMode(m_window.get(), INTERNAL::WindowingAPI::CursorMode::Normal);
	else if (m_data.cursorMode == CursorMode::Hidden)
		INTERNAL::WindowingAPI::SetCursorMode(m_window.get(), INTERNAL::WindowingAPI::CursorMode::Hidden);
	else if (m_data.cursorMode == CursorMode::Disabled)
		INTERNAL::WindowingAPI::SetCursorMode(m_window.get(), INTERNAL::WindowingAPI::CursorMode::Disabled);
	
	if (Input::IsRawMouseInputSupported())
		INTERNAL::WindowingAPI::SetRawMouseMotionMode(m_window.get(), m_data.RawMouseInput);
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Raw Mouse Input is unsupported!");
		m_data.RawMouseInput = false;
	}

	//Set WindowingAPI callbacks
	INTERNAL::WindowingAPI::SetWindowSizeCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const int32_t width, const int32_t height)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));
		data.Width = width;
		data.Height = height;

		if (!data.EventCallback)
			return;

		WindowResizeEvent event(width, height, data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetWindowMinimizeCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, bool restored)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;

		if (restored)
		{
			WindowRestoreEvent event(data.Title);
			data.EventCallback(event);
		}
		else
		{
			WindowMinimizeEvent event(data.Title);
			data.EventCallback(event);
		}
	});

	INTERNAL::WindowingAPI::SetWindowMaximizeCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, bool restored)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;

		if (restored)
		{
			WindowRestoreEvent event(data.Title);
			data.EventCallback(event);
		}
		else
		{
			WindowMaximizeEvent event(data.Title);
			data.EventCallback(event);
		}
	});

	INTERNAL::WindowingAPI::SetWindowPosCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const int32_t x, const int32_t y)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if(data.displayMode == DisplayMode::Windowed)
		{
			data.windowModeParams->XPos = x;
			data.windowModeParams->YPos = y;			
		}

		if (!data.EventCallback)
			return;
		
		WindowMoveEvent event(x, y, data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetWindowFocusCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const bool focused)
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

	INTERNAL::WindowingAPI::SetWindowCloseCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		WindowCloseEvent event(data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetKeyCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const Input::Key key, const bool pressed)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if(pressed)
		{
			if(data.KeyRepeatCounts.find(static_cast<uint16_t>(key)) == data.KeyRepeatCounts.end())
			{
				data.KeyRepeatCounts[static_cast<uint16_t>(key)] = 0;

				if (!data.EventCallback)
					return;
				
				KeyPressEvent event(static_cast<Input::Key>(key), 0, data.Title);
				data.EventCallback(event);
			}
			else
			{
				data.KeyRepeatCounts[static_cast<uint16_t>(key)]++;

				if (!data.EventCallback)
					return;
				
				KeyPressEvent event(static_cast<Input::Key>(key), data.KeyRepeatCounts[static_cast<uint16_t>(key)], data.Title);
				data.EventCallback(event);
			}
		}
		else
		{
			data.KeyRepeatCounts.erase(static_cast<uint16_t>(key));

			if (!data.EventCallback)
				return;
			
			KeyReleaseEvent event(static_cast<Input::Key>(key), data.Title);
			data.EventCallback(event);
		}
	});

	INTERNAL::WindowingAPI::SetCharCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const uint32_t codePoint)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		KeyTypeEvent event(codePoint, data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetMouseButtonCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const Input::MouseButton button, const bool pressed)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		if (pressed)
		{
			MouseButtonPressEvent event(static_cast<Input::MouseButton>(button), data.Title);
			data.EventCallback(event);
		}
		else
		{
			MouseButtonReleaseEvent event(static_cast<Input::MouseButton>(button), data.Title);
			data.EventCallback(event);
		}
	});

	INTERNAL::WindowingAPI::SetScrollCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const double xOffset, const double yOffset)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		MouseScrollEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset), data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetCursorPosCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const double xPos, const double yPos)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;
		
		MouseMoveEvent event(static_cast<float>(xPos), static_cast<float>(yPos), data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetFrameBufferSizeCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const int32_t width, const int32_t height)
	{		
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));
		data.Width = width;
		data.Height = height;

		if (!data.EventCallback)
			return;

		FrameBufferResizeEvent event(width, height, data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetCursorEnterCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const bool entered)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;

		if (entered)
		{
			MouseEnterEvent event(data.Title);
			data.EventCallback(event);
		}
		else
		{
			MouseLeaveEvent event(data.Title);
			data.EventCallback(event);
		}
	});

	INTERNAL::WindowingAPI::SetDropCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, std::vector<std::string> paths)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;

		WindowDropEvent event(std::move(paths), data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetContentScaleCallback(m_window.get(), [](const INTERNAL::WindowingAPI::InternalWindow* window, const float xScale, const float yScale)
	{
		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

		if (!data.EventCallback)
			return;

		WindowContentScaleEvent event(xScale, yScale, data.Title);
		data.EventCallback(event);
	});

	INTERNAL::WindowingAPI::SetMonitorCallback([](const INTERNAL::WindowingAPI::InternalMonitor* monitor, const bool connected)
	{
		if(!connected && Monitor::GetAllMonitors().size() == 1)
		{
			TP_ERROR("No Monitor Found!");
			std::exit(0);
		}
		
		if (monitor->Window)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(monitor->Window));

			for (const auto& win : s_fullscreenWindows)
			{
				if (win)
				{
					Window& window = *win;
					if (window.m_useMonitor == monitor)
					{
						s_fullscreenWindows.erase(s_fullscreenWindows.begin() + window.m_data.Monitor);
						window.m_data.Monitor = 0;
						window.m_useMonitor = static_cast<INTERNAL::WindowingAPI::InternalMonitor*>(Monitor::GetPrimaryMonitor().GetInternalMonitor());
						window.Restore();
						window.SetDisplayMode(DisplayMode::Windowed, window.GetWidth(), window.GetHeight(), window.GetRefreshRate());
						break;
					}
				}
			}

			if (!data.EventCallback)
				return;

			for (const auto& mon : Monitor::GetAllMonitors())
			{
				if (monitor == mon.GetInternalMonitor())
				{
					if (connected)
					{
						MonitorConnectEvent event(mon);
						data.EventCallback(event);
					}
					else
					{
						MonitorDisconnectEvent event(mon);
						data.EventCallback(event);
					}

					return;
				}
			}
		}
	});

	if (s_windows > 1)
		Use();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Shutdown()
{
	INTERNAL::WindowingAPI::DestroyWindow(std::move(m_window));
	m_window = nullptr;
	if (!s_windows)
	{
		INTERNAL::WindowingAPI::Shutdown();
		s_WindowingAPIInitialized = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowProps::WindowProps(std::string title,
                               const uint32_t width,
							   const uint32_t height,
							   const uint32_t refreshRate,
							   const Window::DisplayMode displayMode,
							   Advanced advanced,
							   const uint32_t monitor)
	: Title(std::move(title)),
	  Width(width),
	  Height(height),
	  RefreshRate(refreshRate),
	  RenderAPI(Graphics::API::Context::GetRenderAPI()),
	  displayMode(displayMode),
	  Monitor(monitor),
	  advanced{advanced}
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowProps::Advanced::Advanced(const uint32_t VSync,
                                      const bool resizable,
                                      const bool maximized,
                                      const bool visible,
                                      const bool focused,
                                      const bool focusOnShow,
                                      const bool decorated,
                                      const bool rawMouseInput,
                                      const Window::CursorMode cursorMode)
	: VSync(VSync),
	  Resizable(resizable),
	  Maximized(maximized),
	  Visible(visible),
	  Focused(focused),
	  FocusOnShow(focusOnShow),
	  Decorated(decorated),
	  RawMouseInput(rawMouseInput),
	  CursorMode(cursorMode)
{	
}