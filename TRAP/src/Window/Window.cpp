#include "TRAPPCH.h"
#include "Window.h"

#include "Core/PlatformDetection.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include "Events/MonitorEvent.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Input/Input.h"
#include "Embed.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Layers/ImGui/ImGuiWindowing.h"
#include "Utils/Utils.h"

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::s_windows = 0;

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::s_WindowingAPIInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Window*> TRAP::Window::s_fullscreenWindows{};

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<std::size_t, TRAP::INTERNAL::WindowingAPI::InternalVideoMode> TRAP::Window::s_baseVideoModes{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::Window(const WindowProps &props)
	: m_window(nullptr), m_useMonitor(nullptr)
{
	TP_PROFILE_FUNCTION();

	TP_INFO
	(
	    Log::WindowPrefix, "Initializing window: \"", props.Title, "\" ", props.Width, 'x', props.Height, '@',
		props.RefreshRate, "Hz VSync: ", props.VSync ? "True" : "False",
		//Output DisplayMode
		" Display mode: ", props.DisplayMode == DisplayMode::Windowed ? "Windowed" :
		props.DisplayMode == DisplayMode::Borderless ? "Borderless" : "Fullscreen",
		" Monitor: ", props.Monitor,
		//Output CursorMode
		" Cursor Mode: ", props.Advanced.CursorMode == CursorMode::Normal ? "Normal" :
		props.Advanced.CursorMode == CursorMode::Hidden ? "Hidden" : "Disabled",
		" Raw mouse input: ", props.Advanced.RawMouseInput ? "Enabled" : "Disabled"
	);

	Init(props);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::~Window()
{
	TP_PROFILE_FUNCTION();

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::NONE)
		TRAP::Graphics::RendererAPI::GetRenderer()->RemovePerWindowData(this);

	--s_windows;

	TP_DEBUG(Log::WindowPrefix, "Destroying window: \"", m_data.Title, "\"");
	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::OnUpdate()
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::PollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetActiveWindows() noexcept
{
	return s_windows;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Window::GetTitle() const noexcept
{
	return m_data.Title;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetWidth() const noexcept
{
	return m_data.Width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetHeight() const noexcept
{
	return m_data.Height;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Window::GetSize() const noexcept
{
	return { m_data.Width, m_data.Height };
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetRefreshRate() const noexcept
{
	return m_data.RefreshRate;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::DisplayMode TRAP::Window::GetDisplayMode() const noexcept
{
	return m_data.displayMode;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Monitor TRAP::Window::GetMonitor() const
{
	return Monitor(m_data.Monitor);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window::CursorMode TRAP::Window::GetCursorMode() const noexcept
{
	return m_data.cursorMode;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::GetRawMouseInput() const noexcept
{
	return m_data.RawMouseInput;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Window::GetContentScale() const
{
	TP_PROFILE_FUNCTION();

	Math::Vec2 contentScale{};
	INTERNAL::WindowingAPI::GetWindowContentScale(m_window.get(), contentScale.x, contentScale.y);

	return contentScale;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Window::GetOpacity() const
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetWindowOpacity(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::GetVSync() const noexcept
{
	return m_data.VSync;
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::Window::GetInternalWindow() const
{
	return m_window.get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetTitle(const std::string& title)
{
	TP_PROFILE_FUNCTION();

	if (!title.empty())
		m_data.Title = title;
	else
		m_data.Title = "TRAP™";

#ifndef TRAP_RELEASE
	std::string newTitle = m_data.Title + " - TRAP™ V" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) +
	                       "." + std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) +
						   "." + std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) +
		                   "[INDEV]" + Log::WindowVersion + Graphics::Renderer::GetTitle();
#ifdef TRAP_PLATFORM_LINUX
		newTitle += "[" + Utils::LinuxWindowManagerToString(Utils::GetLinuxWindowManager()) + "]";
#endif
	//Add additional information to the given title for non release builds
	INTERNAL::WindowingAPI::SetWindowTitle(m_window.get(), newTitle);
#else
	//Only set given title for release builds
	INTERNAL::WindowingAPI::SetWindowTitle(m_window.get(), m_data.Title);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetDisplayMode(const DisplayMode& mode, uint32_t width, uint32_t height, uint32_t refreshRate)
{
	TP_PROFILE_FUNCTION();

	constexpr auto GetModeStr = [&](const DisplayMode displayMode)
	{
		if (displayMode == DisplayMode::Windowed)
			return "Windowed";

		return displayMode == DisplayMode::Borderless ? "Borderless" : "Fullscreen";
	}; //Little hack to convert enum class DisplayMode to string

	if(mode == DisplayMode::Windowed) //Check if new mode will also be windowed
	{
		if(m_data.displayMode == DisplayMode::Windowed)
		{
			//Check for invalid parameters
			if(width == 0 || height == 0)
			{
				TP_ERROR(Log::WindowPrefix, "\"", m_data.Title, "\" SetDisplayMode with invalid resolution ", width,
				         'x', height, '@', refreshRate, "Hz called!");
				return;
			}

			//Check if refresh rate differs from current
			if(m_data.RefreshRate != static_cast<int32_t>(refreshRate) && refreshRate > 0)
			{
				//Update refresh rate for Fullscreen or Boderless use
				m_data.windowModeParams.RefreshRate = m_data.RefreshRate;
				m_data.RefreshRate = static_cast<int32_t>(refreshRate);
			}

			//Check if width or height differs from current
			if(m_data.Width != static_cast<int32_t>(width) || m_data.Height != static_cast<int32_t>(height))
			{
				//Set new window resolution
				m_data.windowModeParams.Width = m_data.Width;
				m_data.Width = static_cast<int32_t>(width);
				m_data.windowModeParams.Height = m_data.Height;
				m_data.Height = static_cast<int32_t>(height);
				INTERNAL::WindowingAPI::SetWindowSize(m_window.get(), m_data.Width,
				                                      m_data.Height);
			}

			return;
		}

		//else
		{
			//Remove Window from FullscreenWindows as it gets windowed now
			for(std::size_t i = 0; i < s_fullscreenWindows.size(); ++i)
			{
				if(s_fullscreenWindows[i] == this)
					s_fullscreenWindows[m_data.Monitor] = nullptr;
			}

			//Check if old resolution should be re-used
			if (width == 0 || height == 0 || refreshRate == 0)
			{
				//Use old window width, height and refresh rate if none provided
				m_data.Width = m_data.windowModeParams.Width;
				m_data.Height = m_data.windowModeParams.Height;
				m_data.RefreshRate = m_data.windowModeParams.RefreshRate;
			}
			else
			{
				//Use provided window width, height and refresh rate
				m_data.Width = static_cast<int32_t>(width);
				m_data.Height = static_cast<int32_t>(height);
				m_data.RefreshRate = static_cast<int32_t>(refreshRate);
			}

			INTERNAL::WindowingAPI::SetWindowMonitor(m_window.get(), nullptr, m_data.windowModeParams.XPos,
			                                         m_data.windowModeParams.YPos, m_data.Width,
													 m_data.Height, m_data.RefreshRate);
		}
	}
	else
	{
		//Stash the current resolution & position of the window
		if (m_data.displayMode == DisplayMode::Windowed)
		{
			m_data.windowModeParams.Width = m_data.Width;
			m_data.windowModeParams.Height = m_data.Height;
			m_data.windowModeParams.RefreshRate = m_data.RefreshRate;
			INTERNAL::WindowingAPI::GetWindowPos(m_window.get(), m_data.windowModeParams.XPos,
			                                     m_data.windowModeParams.YPos);
		}

		//Check if Monitor is already used by another window
		if (s_fullscreenWindows[m_data.Monitor])
		{
			//Check if Monitor is used by another window
			if(s_fullscreenWindows[m_data.Monitor] != this)
			{
				TP_ERROR(Log::WindowPrefix, "\"", m_data.Title,
						 "\" couldn't set display mode to ", GetModeStr(mode), " because monitor: ", m_data.Monitor,
						 '(', INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ')',
						 " is already used by window: \"", s_fullscreenWindows[m_data.Monitor]->GetTitle(), "\"!");
				return;
			}
			//Or is the Monitor used by this monitor and the current and new display modes are the same
			if(s_fullscreenWindows[m_data.Monitor] == this && m_data.displayMode == mode &&
			   mode == DisplayMode::Borderless)
			{
				TP_WARN(Log::WindowPrefix, "\"", m_data.Title, "\" already uses display mode ",
						GetModeStr(m_data.displayMode), " on monitor: ", m_data.Monitor,
						"(", INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ")!");
				return;
			}
		}

		s_fullscreenWindows[m_data.Monitor] = this;

		if(mode == DisplayMode::Borderless)
		{
			//For borderless fullscreen, the new width, height and refresh rate will be the
			//default/native video modes width, height and refresh rate
			m_data.Width = s_baseVideoModes[m_data.Monitor].Width;
			m_data.Height = s_baseVideoModes[m_data.Monitor].Height;
			m_data.RefreshRate = s_baseVideoModes[m_data.Monitor].RefreshRate;
		}
		else /*if(mode == DisplayMode::Fullscreen)*/
		{
			//Reset windows monitor usage
			if(m_data.displayMode == DisplayMode::Borderless)
			{
				INTERNAL::WindowingAPI::SetWindowMonitor(m_window.get(),
					                                     nullptr,
					                                     m_data.windowModeParams.XPos,
					                                     m_data.windowModeParams.YPos,
					                                     m_data.windowModeParams.Width,
					                                     m_data.windowModeParams.Height,
					                                     m_data.windowModeParams.RefreshRate);
			}

			bool valid = false;
			if (width != 0 && height != 0 && refreshRate != 0)
			{
				//Check if current running mode is same as target mode
				if (static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].Width) == width &&
					static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].Height) == height &&
					static_cast<uint32_t>(s_baseVideoModes[m_data.Monitor].RefreshRate) == refreshRate)
					valid = true;

				if (!valid) //If not check every video mode of the monitor
				{
					const auto monitorVideoModes = INTERNAL::WindowingAPI::GetVideoModes(m_useMonitor);
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

			m_data.Width = static_cast<int32_t>(width);
			m_data.Height = static_cast<int32_t>(height);
			m_data.RefreshRate = static_cast<int32_t>(refreshRate);
		}

		//Prevent RefreshRate = 0 inside Engine.cfg
		if (m_data.RefreshRate <= 0)
		{
			const INTERNAL::WindowingAPI::InternalVideoMode videoMode = INTERNAL::WindowingAPI::GetVideoMode
				(
					m_useMonitor
				);
			m_data.RefreshRate = videoMode.RefreshRate;
		}

		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" using monitor: ", m_data.Monitor,
				 '(', INTERNAL::WindowingAPI::GetMonitorName(m_useMonitor), ')');

		if(mode == DisplayMode::Borderless)
			INTERNAL::WindowingAPI::SetWindowMonitorBorderless(m_window.get(), m_useMonitor);
		else /*if(mode == DisplayMode::Fullscreen)*/
		{
			INTERNAL::WindowingAPI::SetWindowMonitor(m_window.get(),
				                                     m_useMonitor,
				                                     m_data.windowModeParams.XPos,
				                                     m_data.windowModeParams.YPos,
				                                     m_data.Width,
				                                     m_data.Height,
				                                     m_data.RefreshRate);
		}
	}

	TP_INFO(Log::WindowPrefix, "\"", m_data.Title, "\" changing display mode from ",
	        GetModeStr(m_data.displayMode), " to ", GetModeStr(mode), ": ",
	        m_data.Width, 'x', m_data.Height, '@', m_data.RefreshRate, "Hz");

	//Save new display mode
	m_data.displayMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMonitor(Monitor& monitor)
{
	TP_PROFILE_FUNCTION();

	const uint32_t oldMonitor = m_data.Monitor;

	TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set monitor: ", monitor.GetName(), " (", monitor.GetID(), ")");
	m_data.Monitor = monitor.GetID();
	m_useMonitor = static_cast<INTERNAL::WindowingAPI::InternalMonitor*>(monitor.GetInternalMonitor());

	if(s_fullscreenWindows[oldMonitor] == this)
		s_fullscreenWindows[oldMonitor] = nullptr;

	if (m_data.displayMode == DisplayMode::Windowed)
		return;

	if (s_fullscreenWindows[m_data.Monitor]) //Monitor already has a Fullscreen/Borderless Window
	{
		TP_ERROR(Log::WindowPrefix, "Monitor: ", monitor.GetName(), " (", m_data.Monitor,
		         ") is already used by another window!");
		SetDisplayMode(DisplayMode::Windowed, 0, 0, 0);
	}
	else
		SetDisplayMode(m_data.displayMode, m_data.Width, m_data.Height, m_data.RefreshRate);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorMode(const CursorMode& mode)
{
	TP_PROFILE_FUNCTION();

#ifndef TRAP_RELEASE
	if (mode == CursorMode::Normal)
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set cursor mode: Normal");
	else if (mode == CursorMode::Hidden)
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set cursor mode: Hidden");
	else if (mode == CursorMode::Disabled)
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set cursor mode: Disabled");
	else if(mode == CursorMode::Captured)
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set cursor mode: Captured");
#endif

	INTERNAL::WindowingAPI::SetCursorMode(m_window.get(), mode);
	m_data.cursorMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorType(const CursorType& cursor) const
{
	TP_PROFILE_FUNCTION();

	Scope<INTERNAL::WindowingAPI::InternalCursor> internalCursor = INTERNAL::WindowingAPI::CreateStandardCursor(cursor);
	INTERNAL::WindowingAPI::SetCursor(m_window.get(), internalCursor.get());
	INTERNAL::ImGuiWindowing::SetCustomCursor(internalCursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorIcon(const Image* const image, const int32_t xHotspot, const int32_t yHotspot) const
{
	TP_PROFILE_FUNCTION();

	Scope<INTERNAL::WindowingAPI::InternalCursor> cursor = INTERNAL::WindowingAPI::CreateCursor
		(
			image, xHotspot, yHotspot
		);
	INTERNAL::WindowingAPI::SetCursor(m_window.get(), cursor.get());
	INTERNAL::ImGuiWindowing::SetCustomCursor(cursor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetRawMouseInput(const bool enabled)
{
	TP_PROFILE_FUNCTION();

	if(Input::IsRawMouseInputSupported())
	{
		m_data.RawMouseInput = enabled;
		INTERNAL::WindowingAPI::SetRawMouseMotionMode(m_window.get(), enabled);
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" Raw mouse input: ", enabled ? "Enabled" : "Disabled");
	}
	else
	{
		TP_ERROR(Log::WindowPrefix, "\"", m_data.Title, "\" Raw mouse input is unsupported!");
		m_data.RawMouseInput = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetIcon() const
{
	TP_PROFILE_FUNCTION();

	const std::vector<uint8_t> TRAPLogo{ Embed::TRAPLogo.begin(), Embed::TRAPLogo.end() };
	INTERNAL::WindowingAPI::SetWindowIcon(m_window.get(), Image::LoadFromMemory(32, 32, Image::ColorFormat::RGBA,
	                                                                            TRAPLogo).get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetIcon(const Image* const image) const
{
	TP_PROFILE_FUNCTION();

	if (!image)
	{
		SetIcon();
		return;
	}
	if (image->IsHDR() && image->GetBytesPerChannel() == 4)
	{
		TP_ERROR(Log::WindowIconPrefix, "\"", m_data.Title, "\" HDR is not supported for window icons!");
		TP_WARN(Log::WindowIconPrefix, "\"", m_data.Title, "\" Using default icon!");
		SetIcon();
		return;
	}
	if (image->IsLDR() && image->GetBytesPerChannel() == 2)
	{
		TP_ERROR(Log::WindowIconPrefix, "\"", m_data.Title,
		         "\" Images with short pixel data are not supported for window icons!");
		TP_WARN(Log::WindowIconPrefix, "\"", m_data.Title, "\" Using default icon!");
		SetIcon();
		return;
	}
	if (image->IsImageGrayScale())
	{
		TP_ERROR(Log::WindowIconPrefix, "\"", m_data.Title, "\" Only RGBA images are supported for window icons!");
		TP_WARN(Log::WindowIconPrefix, "\"", m_data.Title, "\" Using default icon!");
		SetIcon();
		return;
	}

	INTERNAL::WindowingAPI::SetWindowIcon(m_window.get(), image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetEventCallback(const EventCallbackFn& callback) noexcept
{
	m_data.EventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetResizable(const bool enabled) const
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::SetWindowHint(m_window.get(), INTERNAL::WindowingAPI::Hint::Resizable, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMinimumSize(const uint32_t minWidth, const uint32_t minHeight)
{
	TP_PROFILE_FUNCTION();

	m_data.MinWidth = static_cast<int32_t>(minWidth);
	m_data.MinHeight = static_cast<int32_t>(minHeight);

	if(m_data.MaxWidth >= MinimumSupportedWindowWidth && m_data.MinWidth > m_data.MaxWidth)
	{
		m_data.MinWidth = m_data.MaxWidth;
		TP_WARN(Log::WindowPrefix,
				"SetMinimumSize: Provided minimum width is bigger than the current maximum width!",
				"Using maximum width as the new minimum width");
	}
	if(m_data.MaxHeight >= MinimumSupportedWindowHeight && m_data.MinHeight > m_data.MaxHeight)
	{
		m_data.MinHeight = m_data.MaxHeight;
		TP_WARN(Log::WindowPrefix,
				"SetMinimumSize: Provided minimum height is smaller than the current maximum height!",
				"Using maximum height as the new minimum height");
	}

	if(m_data.MinWidth < MinimumSupportedWindowWidth)
		m_data.MinWidth = MinimumSupportedWindowWidth;
	if(m_data.MinHeight < MinimumSupportedWindowHeight)
		m_data.MinHeight = MinimumSupportedWindowHeight;

	INTERNAL::WindowingAPI::SetWindowSizeLimits(m_window.get(), m_data.MinWidth, m_data.MinHeight,
											    m_data.MaxWidth, m_data.MaxHeight);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMaximumSize(const uint32_t maxWidth, const uint32_t maxHeight)
{
	TP_PROFILE_FUNCTION();

	m_data.MaxWidth = static_cast<int32_t>(maxWidth);
	m_data.MaxHeight = static_cast<int32_t>(maxHeight);

	if(m_data.MaxWidth == 0)
		m_data.MaxWidth = -1;
	if(m_data.MaxHeight == 0)
		m_data.MaxHeight = -1;

	if(m_data.MaxWidth != -1 && m_data.MaxWidth < m_data.MinWidth)
	{
		m_data.MaxWidth = m_data.MinWidth;
		TP_WARN(Log::WindowPrefix,
				"SetMaximumSize: Provided maximum width is smaller than the current minimum width!",
				"Using minimum width as the new maximum width");
	}
	if(m_data.MaxHeight != -1 && m_data.MaxHeight < m_data.MinHeight)
	{
		m_data.MaxHeight = m_data.MinHeight;
		TP_WARN(Log::WindowPrefix,
				"SetMaximumSize: Provided maximum height is smaller than the current minimum height!",
				"Using minimum height as the new maximum height");
	}

	INTERNAL::WindowingAPI::SetWindowSizeLimits(m_window.get(), m_data.MinWidth, m_data.MinHeight,
											    m_data.MaxWidth, m_data.MaxHeight);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetOpacity(const float opacity) const
{
	TP_PROFILE_FUNCTION();

	if(opacity >= 0.0f || opacity <= 1.0f)
		INTERNAL::WindowingAPI::SetWindowOpacity(m_window.get(), opacity);
	else
		TP_ERROR(Log::WindowPrefix, "Invalid window opacity: ", opacity, "! Valid range: 0.0 - 1.0f");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetDragAndDrop(const bool enabled) const
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::SetDragAndDrop(m_window.get(), enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetVSync(const bool enabled)
{
	m_data.VSync = enabled;
	TRAP::Graphics::RendererAPI::GetRenderer()->SetVSync(enabled, this);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsMaximized() const
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetWindowHint(m_window.get(), INTERNAL::WindowingAPI::Hint::Maximized);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsMinimized() const
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetWindowHint(m_window.get(), INTERNAL::WindowingAPI::Hint::Minimized);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsResizable() const
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetWindowHint(m_window.get(), INTERNAL::WindowingAPI::Hint::Resizable);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsVisible() const
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetWindowHint(m_window.get(), INTERNAL::WindowingAPI::Hint::Visible);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsFocused() const
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetWindowHint(m_window.get(), INTERNAL::WindowingAPI::Hint::Focused);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::IsDecorated() const
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetWindowHint(m_window.get(), INTERNAL::WindowingAPI::Hint::Decorated);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Maximize() const
{
	TP_PROFILE_FUNCTION();

	if(m_data.displayMode == DisplayMode::Windowed)
		INTERNAL::WindowingAPI::MaximizeWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Minimize() const
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::MinimizeWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::RequestAttention() const
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::RequestWindowAttention(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Focus() const
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::FocusWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Hide() const
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::HideWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Show() const
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::ShowWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Restore() const
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::RestoreWindow(m_window.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Init(const WindowProps& props)
{
	m_data.Title = props.Title;
	m_data.Width = static_cast<int32_t>(props.Width);
	m_data.Height = static_cast<int32_t>(props.Height);
	m_data.RefreshRate = static_cast<int32_t>(props.RefreshRate);
	m_data.VSync = props.VSync;
	m_data.Monitor = props.Monitor;
	m_data.cursorMode = props.Advanced.CursorMode;
	m_data.RawMouseInput = props.Advanced.RawMouseInput;

	if (!s_WindowingAPIInitialized)
	{
		const int32_t success = INTERNAL::WindowingAPI::Init();
		TRAP_ASSERT(success, "Couldn't initialize WindowingAPI!");
		if (!success)
			Utils::Dialogs::ShowMsgBox("WindowingAPI Error",
									   "Couldn't initialize WindowingAPI!\nError code: 0x0011",
									   Utils::Dialogs::Style::Error,
									   Utils::Dialogs::Buttons::Quit);
		s_WindowingAPIInitialized = true;
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
		TP_ERROR(Log::WindowPrefix, "\"", m_data.Title, "\" Invalid monitor!");
		TP_WARN(Log::WindowPrefix, "\"", m_data.Title, "\" Using primary monitor!");
		m_data.Monitor = 0;
		m_useMonitor = INTERNAL::WindowingAPI::GetPrimaryMonitor();
	}

	if(s_baseVideoModes.empty())
	{
		//Store the current VideoMode of each monitor
		for(std::size_t i = 0; i < monitors.size(); i++)
		{
			s_baseVideoModes[i] = INTERNAL::WindowingAPI::GetVideoMode(monitors[i]);
			TP_DEBUG(Log::WindowPrefix, "Storing underlying OS video mode");
			TP_DEBUG(Log::WindowPrefix, "Monitor: ", i, " ",
			         s_baseVideoModes[i].Width, 'x', s_baseVideoModes[i].Height, '@',
					 s_baseVideoModes[i].RefreshRate, "Hz (R", s_baseVideoModes[i].RedBits,
					                                      'G', s_baseVideoModes[i].GreenBits,
														  'B', s_baseVideoModes[i].BlueBits, ')');
		}
	}

	INTERNAL::WindowingAPI::DefaultWindowHints();

	if (props.DisplayMode == DisplayMode::Windowed)
	{
		if(props.Advanced.Maximized)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Maximized, true);
		if (!props.Advanced.Resizable)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Resizable, false);
		if (!props.Advanced.Visible)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Visible, false);
		if (!props.Advanced.Decorated)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Decorated, false);
		if (!props.Advanced.Focused && !props.Advanced.Visible)
			INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Focused, false);
	}
	if (!props.Advanced.FocusOnShow)
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::FocusOnShow, false);

	//Create Window
#ifndef TRAP_RELEASE
	std::string newTitle = m_data.Title + " - TRAP™ V" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) +
	                       "." + std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) +
						   "." + std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) +
						   "[INDEV]" + Log::WindowVersion;
#else
	std::string newTitle = m_data.Title;
#endif

	if(m_data.Width < MinimumSupportedWindowWidth)
	{
		TP_WARN(Log::WindowPrefix, "Specified window width is smaller than the minimum supported!");
		m_data.Width = MinimumSupportedWindowWidth;
	}
	if(m_data.Height < MinimumSupportedWindowHeight)
	{
		TP_WARN(Log::WindowPrefix, "Specified window height is smaller than the minimum supported!");
		m_data.Height = MinimumSupportedWindowHeight;
	}

	m_window = INTERNAL::WindowingAPI::CreateWindow(m_data.Width, m_data.Height,
		                                            newTitle, nullptr);

	if (!m_window)
	{
		TRAP::Utils::Dialogs::ShowMsgBox("Failed to create window",
										 "Failed to create window!\nError code: 0x0009",
										 Utils::Dialogs::Style::Error,
										 Utils::Dialogs::Buttons::Quit);
		TRAP::Application::Shutdown();
		TP_CRITICAL(Log::WindowPrefix, "Failed to create window");
		exit(-1);
	}

	INTERNAL::WindowingAPI::GetWindowSize(m_window.get(), m_data.Width, m_data.Height);

	s_windows++;
	if (s_windows > 1)
	{
		//Update Window Title
	#ifndef TRAP_RELEASE
		newTitle += Graphics::Renderer::GetTitle();
	#ifdef TRAP_PLATFORM_LINUX
		if (Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::Wayland)
			newTitle += "[Wayland]";
		else if (Utils::GetLinuxWindowManager() == Utils::LinuxWindowManager::X11)
			newTitle += "[X11]";
		else
			newTitle += "[Unknown]";
	#endif
	#endif
		INTERNAL::WindowingAPI::SetWindowTitle(m_window.get(), newTitle);
	}

	//If currently windowed, stash the current size and position of the window
	if (m_data.displayMode == DisplayMode::Windowed)
	{
		//Prevent RefreshRate = 0 inside Engine.cfg
		if (m_data.RefreshRate <= 0)
		{
			const INTERNAL::WindowingAPI::InternalVideoMode videoMode = INTERNAL::WindowingAPI::GetVideoMode
				(
					m_useMonitor
				);
			m_data.RefreshRate = videoMode.RefreshRate;
		}

		INTERNAL::WindowingAPI::SetWindowSizeLimits(m_window.get(),
													MinimumSupportedWindowWidth, MinimumSupportedWindowHeight,
													m_data.MaxWidth, m_data.MaxHeight);
	}

	SetDisplayMode(props.DisplayMode, m_data.Width, m_data.Height, m_data.RefreshRate);

	m_data.Win = this;

	INTERNAL::WindowingAPI::SetWindowUserPointer(m_window.get(), &m_data);

	SetIcon();

	INTERNAL::WindowingAPI::SetCursorMode(m_window.get(), m_data.cursorMode);

	if (Input::IsRawMouseInputSupported())
		INTERNAL::WindowingAPI::SetRawMouseMotionMode(m_window.get(), m_data.RawMouseInput);
	else
	{
		TP_ERROR(Log::WindowPrefix, "\"", m_data.Title, "\" Raw mouse input is unsupported!");
		m_data.RawMouseInput = false;
	}

	SetupEventCallbacks();

#ifdef TRAP_HEADLESS_MODE
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
		TRAP::Graphics::RendererAPI::GetRenderer()->InitPerWindowData(this);
#else
	TRAP::Graphics::RendererAPI::GetRenderer()->InitPerWindowData(this);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Shutdown()
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::DestroyWindow(std::move(m_window));
	m_window = nullptr;
	if (!s_windows)
	{
		INTERNAL::WindowingAPI::Shutdown();
		s_WindowingAPIInitialized = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetupEventCallbacks()
{
	//Set WindowingAPI callbacks
	INTERNAL::WindowingAPI::SetWindowSizeCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const int32_t w, const int32_t h)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));
			data.Width = w;
			data.Height = h;

			if (!data.EventCallback)
				return;

			Events::WindowResizeEvent event(w, h, data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetWindowMinimizeCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, bool restored)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			if (restored)
			{
				Events::WindowRestoreEvent event(data.Win);
				data.EventCallback(event);
			}
			else
			{
				Events::WindowMinimizeEvent event(data.Win);
				data.EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetWindowMaximizeCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, bool restored)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			if (restored)
			{
				Events::WindowRestoreEvent event(data.Win);
				data.EventCallback(event);
			}
			else
			{
				Events::WindowMaximizeEvent event(data.Win);
				data.EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetWindowPosCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const int32_t x, const int32_t y)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if(data.displayMode == DisplayMode::Windowed && x > 0 && y > 0)
			{
				data.windowModeParams.XPos = x;
				data.windowModeParams.YPos = y;
			}

			if (!data.EventCallback)
				return;

			Events::WindowMoveEvent event(x, y, data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetWindowFocusCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const bool focused)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			if (focused)
			{
				Events::WindowFocusEvent event(data.Win);
				data.EventCallback(event);
			}
			else
			{
				Events::WindowLostFocusEvent event(data.Win);
				data.EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetWindowCloseCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			Events::WindowCloseEvent event(data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetKeyCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const Input::Key key, const bool pressed)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if(pressed)
			{
				if(data.KeyRepeatCounts.find(static_cast<uint16_t>(key)) == data.KeyRepeatCounts.end())
				{
					data.KeyRepeatCounts[static_cast<uint16_t>(key)] = 0;

					if (!data.EventCallback)
						return;

					Events::KeyPressEvent event(static_cast<Input::Key>(key), 0, data.Win);
					data.EventCallback(event);
				}
				else
				{
					data.KeyRepeatCounts[static_cast<uint16_t>(key)]++;

					if (!data.EventCallback)
						return;

					Events::KeyPressEvent event(static_cast<Input::Key>(key),
					                            data.KeyRepeatCounts[static_cast<uint16_t>(key)], data.Win);
					data.EventCallback(event);
				}
			}
			else
			{
				data.KeyRepeatCounts.erase(static_cast<uint16_t>(key));

				if (!data.EventCallback)
					return;

				Events::KeyReleaseEvent event(static_cast<Input::Key>(key), data.Win);
				data.EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetCharCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const uint32_t codePoint)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			Events::KeyTypeEvent event(codePoint, data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetMouseButtonCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const Input::MouseButton button, const bool pressed)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			if (pressed)
			{
				Events::MouseButtonPressEvent event(button, data.Win);
				data.EventCallback(event);
			}
			else
			{
				Events::MouseButtonReleaseEvent event(button, data.Win);
				data.EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetScrollCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const double xOffset, const double yOffset)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			Events::MouseScrollEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset), data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetCursorPosCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const double xPos, const double yPos)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			Events::MouseMoveEvent event(static_cast<float>(xPos), static_cast<float>(yPos), data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetFrameBufferSizeCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const int32_t w, const int32_t h)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));
			data.Width = w;
			data.Height = h;

			if (!data.EventCallback)
				return;

			Events::FrameBufferResizeEvent event(w, h, data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetCursorEnterCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const bool entered)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			if (entered)
			{
				Events::MouseEnterEvent event(data.Win);
				data.EventCallback(event);
			}
			else
			{
				Events::MouseLeaveEvent event(data.Win);
				data.EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetDropCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, std::vector<std::string> paths)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			Events::WindowDropEvent event(std::move(paths), data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetContentScaleCallback(m_window.get(),
		[](const INTERNAL::WindowingAPI::InternalWindow* window, const float xScale, const float yScale)
		{
			WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data.EventCallback)
				return;

			Events::WindowContentScaleEvent event(xScale, yScale, data.Win);
			data.EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetMonitorCallback([](const INTERNAL::WindowingAPI::InternalMonitor* mon,
	                                              const bool connected)
	{
		if(!connected && Monitor::GetAllMonitors().size() == 1)
		{
			TP_ERROR(Log::WindowPrefix, "No monitor found!");
			TP_ERROR(Log::WindowPrefix, "Error code: 0x000C");
			TP_ERROR(Log::WindowPrefix, "Closing TRAP!");
			exit(-1);
		}

		if(!mon->Window)
			return;

		WindowData& data = *static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(mon->Window));

		for (const auto& win : s_fullscreenWindows)
		{
			if(!win || win->m_useMonitor != mon)
				continue;

			const auto removeWindowIterator = s_fullscreenWindows.begin() + win->m_data.Monitor;
			if(removeWindowIterator != std::end(s_fullscreenWindows))
			{
				*removeWindowIterator = s_fullscreenWindows.back();
				s_fullscreenWindows.pop_back();
			}

			win->m_data.Monitor = 0;
			win->m_useMonitor = static_cast<INTERNAL::WindowingAPI::InternalMonitor*>
				(
					Monitor::GetPrimaryMonitor().GetInternalMonitor()
				);
			win->Restore();
			win->SetDisplayMode(DisplayMode::Windowed, win->GetWidth(), win->GetHeight(),
								win->GetRefreshRate());
			break;
		}

		if (!data.EventCallback)
			return;

		for (const auto& m : Monitor::GetAllMonitors())
		{
			if(mon != m.GetInternalMonitor())
				continue;

			if (connected)
			{
				Events::MonitorConnectEvent event(m);
				data.EventCallback(event);
			}
			else
			{
				Events::MonitorDisconnectEvent event(m);
				data.EventCallback(event);
			}

			return;
		}
	});
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowProps::WindowProps(std::string title,
                               const uint32_t width,
							   const uint32_t height,
							   const uint32_t refreshRate,
							   const bool vsync,
							   const Window::DisplayMode displayMode,
							   AdvancedProps advanced,
							   const uint32_t monitor) noexcept
	: Title(std::move(title)),
	  Width(width),
	  Height(height),
	  RefreshRate(refreshRate),
	  VSync(vsync),
	  DisplayMode(displayMode),
	  Monitor(monitor),
	  Advanced{advanced}
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowProps::AdvancedProps::AdvancedProps(const bool resizable,
                                                const bool maximized,
                                                const bool visible,
                                                const bool focused,
                                                const bool focusOnShow,
                                                const bool decorated,
                                                const bool rawMouseInput,
                                                const Window::CursorMode cursorMode) noexcept
	: Resizable(resizable),
	  Maximized(maximized),
	  Visible(visible),
	  Focused(focused),
	  FocusOnShow(focusOnShow),
	  Decorated(decorated),
	  RawMouseInput(rawMouseInput),
	  CursorMode(cursorMode)
{
	TP_PROFILE_FUNCTION();
}