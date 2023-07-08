#include "TRAPPCH.h"
#include "Window.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"
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
#include "Application.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

TRAP::Window::Window(const WindowProps &props)
	: m_window(nullptr)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TP_INFO
	(
	    Log::WindowPrefix, "Initializing window: \"", props.Title, "\" ", props.Width, 'x', props.Height, '@',
		props.RefreshRate, "Hz VSync: ", props.VSync ? "True" : "False",
		//Output DisplayMode
		" Display mode: ", props.DisplayMode == DisplayMode::Windowed ? "Windowed" :
		props.DisplayMode == DisplayMode::Borderless ? "Borderless" : "Fullscreen",
		" Monitor: \"", props.Monitor.GetName(), "\"(", props.Monitor.GetID(), ')',
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
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::NONE)
		TRAP::Graphics::RendererAPI::GetRenderer()->RemovePerViewportData(this);

	--s_windows;

	TP_DEBUG(Log::WindowPrefix, "Destroying window: \"", m_data.Title, "\"");
	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::OnUpdate()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::PollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Window::GetActiveWindows() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_windows;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Window::GetWidth() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetSize().x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Window::GetHeight() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetSize().y;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2ui TRAP::Window::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	int32_t width = 0, height = 0;
	INTERNAL::WindowingAPI::GetWindowSize(*m_window, width, height);

	return { width, height };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Window::GetFrameBufferWidth() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return GetFrameBufferSize().x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Window::GetFrameBufferHeight() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return GetFrameBufferSize().y;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2ui TRAP::Window::GetFrameBufferSize() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	int32_t width = 0, height = 0;
	INTERNAL::WindowingAPI::GetFrameBufferSize(*m_window, width, height);

	return { width, height };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2i TRAP::Window::GetPosition() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TRAP::Math::Vec2i pos{};

	INTERNAL::WindowingAPI::GetWindowPos(*m_window, std::get<0>(pos), std::get<1>(pos));

	return pos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] double TRAP::Window::GetRefreshRate() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const auto currVideoMode = m_data.Monitor.GetCurrentVideoMode();

	return currVideoMode ? currVideoMode->RefreshRate : 60.0;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Window::CursorMode TRAP::Window::GetCursorMode() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return INTERNAL::WindowingAPI::GetCursorMode(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Monitor TRAP::Window::GetMonitor() const
{
	return m_data.Monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::GetRawMouseInput() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return INTERNAL::WindowingAPI::GetRawMouseMotionMode(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2 TRAP::Window::GetContentScale() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	Math::Vec2 contentScale{};
	INTERNAL::WindowingAPI::GetWindowContentScale(*m_window, contentScale.x, contentScale.y);

	return contentScale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<float> TRAP::Window::GetOpacity() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return INTERNAL::WindowingAPI::GetWindowOpacity(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::GetVSync() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Graphics::RendererAPI::GetRenderer()->GetVSync(this);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Window::GetAspectRatio() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const auto frameBufferSize = GetFrameBufferSize();

	return NumericCast<float>(frameBufferSize.x) / NumericCast<float>(frameBufferSize.y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetTitle(const std::string& title)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	if (!title.empty())
		m_data.Title = title;
	else
		m_data.Title = "TRAP™";

#ifndef TRAP_RELEASE
	std::string newTitle = fmt::format("{} - TRAP™ V{}.{}.{}[INDEV]{}{}", m_data.Title,
	                                   TRAP_VERSION_MAJOR(TRAP_VERSION), TRAP_VERSION_MINOR(TRAP_VERSION),
									   TRAP_VERSION_PATCH(TRAP_VERSION), Log::WindowVersion,
									   Graphics::RendererAPI::GetRenderer()->GetTitle());
#ifdef TRAP_PLATFORM_LINUX
		newTitle += fmt::format("[{}]", Utils::String::ConvertToString(Utils::GetLinuxWindowManager()));
#elif defined(TRAP_PLATFORM_WINDOWS)
		newTitle += "[Win32]";
#endif

#if defined(__SANITIZE_ADDRESS__) || __has_feature(address_sanitizer) || defined(TRAP_ASAN)
		newTitle += "[ASan]";
#elif defined(__SANITIZE_UNDEFINED__) || __has_feature(undefined_sanitizer) || defined(TRAP_UBSAN)
		newTitle += "[UBSan]";
#elif defined(__SANITIZE_LEAK__) || __has_feature(leak_sanitizer) || defined(TRAP_LSAN)
		newTitle += "[LSan]";
#elif defined(__SANITIZE_MEMORY__) || __has_feature(memory_sanitizer) || defined(TRAP_MSAN)
		newTitle += "[MSan]";
#elif defined(__SANITIZE_THREAD__) || __has_feature(thread_sanitizer) || defined(TRAP_TSAN)
		newTitle += "[TSan]";
#endif

	//Add additional information to the given title for non release builds
	INTERNAL::WindowingAPI::SetWindowTitle(*m_window, newTitle);
#else
	//Only set given title for release builds
	INTERNAL::WindowingAPI::SetWindowTitle(*m_window, m_data.Title);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetFullscreen()
{
	const auto nativeMode = m_data.Monitor.GetNativeVideoMode();
	if(nativeMode)
		SetFullscreen(*nativeMode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetFullscreen(const Monitor::VideoMode& videoMode)
{
	//Do nothing if already fullscreen using the given video mode
	const auto currVideoMode = m_data.Monitor.GetCurrentVideoMode();
	if(m_data.displayMode == DisplayMode::Fullscreen && currVideoMode && videoMode == *currVideoMode)
		return;

	if(m_data.displayMode == DisplayMode::Windowed) //Stash current window position
		INTERNAL::WindowingAPI::GetWindowPos(*m_window, m_data.windowModeParams.XPos, m_data.windowModeParams.YPos);

	//Check if given video mode is supported by monitor
	const auto monitorVideoModes = m_data.Monitor.GetVideoModes();
	if(std::find(monitorVideoModes.begin(), monitorVideoModes.end(), videoMode) == monitorVideoModes.end())
	{
		TP_ERROR(Log::WindowPrefix, "Unsupported video mode provided to set on monitor \"", m_data.Monitor.GetName(), "\"!");
		return;
	}

	TP_INFO(Log::WindowPrefix, "\"", m_data.Title, "\" changing display mode from ",
	        Utils::String::ConvertToString(m_data.displayMode), " to ",
			Utils::String::ConvertToString(DisplayMode::Fullscreen), ": ",
	        videoMode.Width, 'x', videoMode.Height, '@', videoMode.RefreshRate, "Hz");

	INTERNAL::WindowingAPI::SetWindowMonitor(*m_window, static_cast<INTERNAL::WindowingAPI::InternalMonitor*>(m_data.Monitor.GetInternalMonitor()),
	                                         0, 0, videoMode.Width, videoMode.Height, videoMode.RefreshRate);

	m_data.displayMode = DisplayMode::Fullscreen;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetFullscreenBorderless()
{
	const auto nativeMode = m_data.Monitor.GetNativeVideoMode();
	if(!nativeMode)
	{
		TP_ERROR(Log::WindowPrefix, "Failed to retrieve native video mode from monitor \"", m_data.Monitor.GetName(), "\"!");
		return;
	}

	if(m_data.displayMode == DisplayMode::Fullscreen) //Un-fullscreen before applying fullscreen borderless
		SetWindowed();

	if(m_data.displayMode == DisplayMode::Windowed) //Stash current window position
		INTERNAL::WindowingAPI::GetWindowPos(*m_window, m_data.windowModeParams.XPos, m_data.windowModeParams.YPos);

	TP_INFO(Log::WindowPrefix, "\"", m_data.Title, "\" changing display mode from ",
	        Utils::String::ConvertToString(m_data.displayMode), " to ",
			Utils::String::ConvertToString(DisplayMode::Borderless), ": ",
	        nativeMode->Width, 'x', nativeMode->Height, '@', nativeMode->RefreshRate, "Hz");

	INTERNAL::WindowingAPI::SetWindowMonitorBorderless(*m_window, *static_cast<INTERNAL::WindowingAPI::InternalMonitor*>(m_data.Monitor.GetInternalMonitor()));

	m_data.displayMode = DisplayMode::Borderless;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetWindowed()
{
	if(m_data.displayMode == DisplayMode::Windowed)
		return;

	SetWindowed(m_data.windowModeParams.Width, m_data.windowModeParams.Height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetWindowed(const uint32_t width, const uint32_t height)
{
	//Check for invalid parameters
	if(width < MinimumSupportedWindowWidth || height < MinimumSupportedWindowHeight)
	{
		TP_ERROR(Log::WindowPrefix, "\"", m_data.Title, "\" SetWindowed() with invalid resolution ", width,
					'x', height, " called!");
		return;
	}

	if(m_data.displayMode != DisplayMode::Windowed) //Change to windowed and resize
	{
		TP_INFO(Log::WindowPrefix, "\"", m_data.Title, "\" changing display mode from ",
				Utils::String::ConvertToString(m_data.displayMode), " to ",
				Utils::String::ConvertToString(DisplayMode::Windowed), ": ", width, 'x', height);

		INTERNAL::WindowingAPI::SetWindowMonitor(*m_window, nullptr, m_data.windowModeParams.XPos,
												 m_data.windowModeParams.YPos, NumericCast<int32_t>(width),
												 NumericCast<int32_t>(height), -1.0);

		m_data.displayMode = DisplayMode::Windowed;
	}
	else /*if(m_data.displayMode == DisplayMode::Windowed)*/ //Normal resize
		INTERNAL::WindowingAPI::SetWindowSize(*m_window, NumericCast<int32_t>(width), NumericCast<int32_t>(height));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMonitor(const Monitor& monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	m_data.Monitor = monitor;

	TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set monitor: ", monitor.GetName(), " (", monitor.GetID(), ")");

	if (m_data.displayMode == DisplayMode::Windowed)
		return;

	if(m_data.displayMode == DisplayMode::Fullscreen)
	{
		const auto nativeVideoMode = m_data.Monitor.GetNativeVideoMode();
		if(!nativeVideoMode)
			return;

		SetFullscreen(*nativeVideoMode);
	}
	else if(m_data.displayMode == DisplayMode::Borderless)
		SetFullscreenBorderless();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorMode(const CursorMode& mode)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

#ifndef TRAP_RELEASE
	if (mode == CursorMode::Normal)
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set cursor mode: Normal");
	else if (mode == CursorMode::Hidden)
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set cursor mode: Hidden");
	else if (mode == CursorMode::Disabled)
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set cursor mode: Disabled");
	else if(mode == CursorMode::Captured)
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" set cursor mode: Captured");
#endif /*TRAP_RELEASE*/

	INTERNAL::WindowingAPI::SetCursorMode(*m_window, mode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorType(const CursorType& cursor) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::InternalCursor* internalCursor = INTERNAL::WindowingAPI::CreateStandardCursor(cursor);
	INTERNAL::WindowingAPI::SetCursor(*m_window, internalCursor);
	INTERNAL::ImGuiWindowing::SetCustomCursor(internalCursor); //Make ImGui the owner of the cursor
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetCursorIcon(const Image* const image, const int32_t xHotspot, const int32_t yHotspot) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::InternalCursor* cursor = INTERNAL::WindowingAPI::CreateCursor
		(
			*image, xHotspot, yHotspot
		);
	INTERNAL::WindowingAPI::SetCursor(*m_window, cursor);
	INTERNAL::ImGuiWindowing::SetCustomCursor(cursor); //Make ImGui the owner of the cursor
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetRawMouseInput(const bool enabled)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	if(Input::IsRawMouseInputSupported())
	{
		INTERNAL::WindowingAPI::SetRawMouseMotionMode(*m_window, enabled);
		TP_DEBUG(Log::WindowPrefix, "\"", m_data.Title, "\" Raw mouse input: ", enabled ? "Enabled" : "Disabled");
	}
	else
	{
		TP_ERROR(Log::WindowPrefix, "\"", m_data.Title, "\" Raw mouse input is unsupported!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetProgress(const ProgressState state, const double progress) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::SetWindowProgressIndicator(*m_window, state, progress);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetIcon() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	const std::vector<uint8_t> TRAPLogo{ Embed::TRAPLogo.begin(), Embed::TRAPLogo.end() };
	INTERNAL::WindowingAPI::SetWindowIcon(*m_window, Image::LoadFromMemory(32u, 32u, Image::ColorFormat::RGBA,
	                                                                       TRAPLogo).get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetIcon(const Image* const image) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	if (image == nullptr)
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

	INTERNAL::WindowingAPI::SetWindowIcon(*m_window, image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetEventCallback(const EventCallbackFn& callback) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_data.EventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetResizable(const bool enabled) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::SetWindowHint(*m_window, INTERNAL::WindowingAPI::Hint::Resizable, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMinimumSize(const uint32_t minWidth, const uint32_t minHeight)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	m_data.MinWidth = NumericCast<int32_t>(minWidth);
	m_data.MinHeight = NumericCast<int32_t>(minHeight);

	if(m_data.MaxWidth == 0)
		m_data.MaxWidth = -1;
	if(m_data.MaxHeight == 0)
		m_data.MaxHeight = -1;

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

	INTERNAL::WindowingAPI::SetWindowSizeLimits(*m_window, m_data.MinWidth, m_data.MinHeight,
											    m_data.MaxWidth, m_data.MaxHeight);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMaximumSize(const uint32_t maxWidth, const uint32_t maxHeight)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	m_data.MaxWidth = NumericCast<int32_t>(maxWidth);
	m_data.MaxHeight = NumericCast<int32_t>(maxHeight);

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

	INTERNAL::WindowingAPI::SetWindowSizeLimits(*m_window, m_data.MinWidth, m_data.MinHeight,
											    m_data.MaxWidth, m_data.MaxHeight);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetAspectRatio(const uint32_t numerator, const uint32_t denominator)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	if(numerator == 0 && denominator == 0) //Disable aspect ratio
		INTERNAL::WindowingAPI::SetWindowAspectRatio(*m_window, -1, -1);
	else //Enable aspect ratio
		INTERNAL::WindowingAPI::SetWindowAspectRatio(*m_window, NumericCast<int32_t>(numerator),
		                                             NumericCast<int32_t>(denominator));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetPosition(const uint32_t x, const uint32_t y)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::SetWindowPos(*m_window, NumericCast<int32_t>(x), NumericCast<int32_t>(y));

	m_data.windowModeParams.XPos = NumericCast<int32_t>(x);
	m_data.windowModeParams.YPos = NumericCast<int32_t>(y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetOpacity(const float opacity) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	if(opacity >= 0.0f || opacity <= 1.0f)
		INTERNAL::WindowingAPI::SetWindowOpacity(*m_window, opacity);
	else
		TP_ERROR(Log::WindowPrefix, "Invalid window opacity: ", opacity, "! Valid range: 0.0 - 1.0f");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetDragAndDrop(const bool enabled) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::SetDragAndDrop(*m_window, enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetVSync(const bool enabled) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TRAP::Graphics::RendererAPI::GetRenderer()->SetVSync(enabled, this);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::IsMaximized() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return INTERNAL::WindowingAPI::GetWindowHint(*m_window, INTERNAL::WindowingAPI::Hint::Maximized);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::IsMinimized() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return INTERNAL::WindowingAPI::GetWindowHint(*m_window, INTERNAL::WindowingAPI::Hint::Minimized);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::IsResizable() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return INTERNAL::WindowingAPI::GetWindowHint(*m_window, INTERNAL::WindowingAPI::Hint::Resizable);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::IsVisible() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return INTERNAL::WindowingAPI::GetWindowHint(*m_window, INTERNAL::WindowingAPI::Hint::Visible);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::IsFocused() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return INTERNAL::WindowingAPI::GetWindowHint(*m_window, INTERNAL::WindowingAPI::Hint::Focused);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::IsDecorated() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return INTERNAL::WindowingAPI::GetWindowHint(*m_window, INTERNAL::WindowingAPI::Hint::Decorated);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Window::IsHovered() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	return INTERNAL::WindowingAPI::GetWindowHint(*m_window, INTERNAL::WindowingAPI::Hint::Hovered);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Maximize() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	if(m_data.displayMode == DisplayMode::Windowed)
		INTERNAL::WindowingAPI::MaximizeWindow(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Minimize() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::MinimizeWindow(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::RequestAttention() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::RequestWindowAttention(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Focus() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::FocusWindow(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Hide() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::HideWindow(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Show() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::ShowWindow(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Restore() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::RestoreWindow(*m_window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Init(const WindowProps& props)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	uint32_t width = props.Width;
	uint32_t height = props.Height;
	m_data.Title = props.Title;
	m_data.Monitor = props.Monitor;

	if (!INTERNAL::WindowingAPI::Init())
		Utils::DisplayError(Utils::ErrorCode::WindowingAPIFailedInitialization);

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
	const std::string newTitle = fmt::format("{} - TRAP™ V{}.{}.{}[INDEV]{}{}[{}]", m_data.Title,
	                                         TRAP_VERSION_MAJOR(TRAP_VERSION),
											 TRAP_VERSION_MINOR(TRAP_VERSION),
											 TRAP_VERSION_PATCH(TRAP_VERSION), Log::WindowVersion,
											 Graphics::RendererAPI::GetRenderer()->GetTitle(),

#ifdef TRAP_PLATFORM_LINUX
						         Utils::String::ConvertToString(Utils::GetLinuxWindowManager())
#elif defined(TRAP_PLATFORM_LINUX)
						         "[Win32]"
#else
						         "[Unknown]"
#endif
	);
#else
	const std::string newTitle = m_data.Title;
#endif

	if(width < MinimumSupportedWindowWidth)
	{
		TP_WARN(Log::WindowPrefix, "Specified window width is smaller than the minimum supported!");
		width = MinimumSupportedWindowWidth;
	}
	if(height < MinimumSupportedWindowHeight)
	{
		TP_WARN(Log::WindowPrefix, "Specified window height is smaller than the minimum supported!");
		height = MinimumSupportedWindowHeight;
	}

	m_window = INTERNAL::WindowingAPI::CreateWindow(width, height, newTitle, nullptr);

	if (m_window == nullptr)
		Utils::DisplayError(Utils::ErrorCode::WindowingAPIWindowCreationFailed);

	s_windows++;

	INTERNAL::WindowingAPI::GetWindowSize(*m_window, m_data.windowModeParams.Width, m_data.windowModeParams.Height);

	INTERNAL::WindowingAPI::SetWindowSizeLimits(*m_window,
												MinimumSupportedWindowWidth, MinimumSupportedWindowHeight,
												m_data.MaxWidth, m_data.MaxHeight);

	if(props.DisplayMode == DisplayMode::Fullscreen)
		SetFullscreen(Monitor::VideoMode(NumericCast<int32_t>(width), NumericCast<int32_t>(height), props.RefreshRate));
	else if(props.DisplayMode == DisplayMode::Borderless)
		SetFullscreenBorderless();

	m_data.Win = this;

	INTERNAL::WindowingAPI::SetWindowUserPointer(*m_window, &m_data);

	SetIcon();

	INTERNAL::WindowingAPI::SetCursorMode(*m_window, props.Advanced.CursorMode);

	if (Input::IsRawMouseInputSupported())
		INTERNAL::WindowingAPI::SetRawMouseMotionMode(*m_window, props.Advanced.RawMouseInput);
	else
	{
		TP_ERROR(Log::WindowPrefix, "\"", m_data.Title, "\" Raw mouse input is unsupported!");
	}

	SetupEventCallbacks();

	TRAP::Graphics::RendererAPI::GetRenderer()->InitPerViewportData(this, props.VSync);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	INTERNAL::WindowingAPI::DestroyWindow(m_window);
	m_window = nullptr;
	if (s_windows == 0u)
		INTERNAL::WindowingAPI::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetupEventCallbacks()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	//Set WindowingAPI callbacks
	INTERNAL::WindowingAPI::SetWindowSizeCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, const int32_t w, const int32_t h)
		{
			WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));
			if(data == nullptr)
				return;

			if (!data->EventCallback)
				return;

			Events::WindowResizeEvent event(NumericCast<uint32_t>(w), NumericCast<uint32_t>(h), data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetWindowMinimizeCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, bool restored)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			if (restored)
			{
				Events::WindowRestoreEvent event(data->Win);
				data->EventCallback(event);
			}
			else
			{
				Events::WindowMinimizeEvent event(data->Win);
				data->EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetWindowMaximizeCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, bool restored)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			if (restored)
			{
				Events::WindowRestoreEvent event(data->Win);
				data->EventCallback(event);
			}
			else
			{
				Events::WindowMaximizeEvent event(data->Win);
				data->EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetWindowPosCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, const int32_t x, const int32_t y)
		{
			WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));
			if(data == nullptr)
				return;

			if(data->displayMode == DisplayMode::Windowed && x > 0 && y > 0)
			{
				data->windowModeParams.XPos = x;
				data->windowModeParams.YPos = y;
			}

			if (!data->EventCallback)
				return;

			Events::WindowMoveEvent event(x, y, data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetWindowFocusCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, const bool focused)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			if (focused)
			{
				Events::WindowFocusEvent event(data->Win);
				data->EventCallback(event);
			}
			else
			{
				Events::WindowLostFocusEvent event(data->Win);
				data->EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetWindowCloseCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			Events::WindowCloseEvent event(data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetKeyCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, const Input::Key key, const Input::KeyState state)
		{
			WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if(data == nullptr)
				return;

			if(state == Input::KeyState::Pressed)
			{
				if (!data->EventCallback)
					return;

				Events::KeyPressEvent event(key, data->Win);
				data->EventCallback(event);
			}
			else if(state == Input::KeyState::Repeat)
			{
				if (!data->EventCallback)
					return;

				Events::KeyRepeatEvent event(key, data->Win);
				data->EventCallback(event);
			}
			else /*if(state == Input::KeyState::Released)*/
			{
				if (!data->EventCallback)
					return;

				Events::KeyReleaseEvent event(key, data->Win);
				data->EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetCharCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, const uint32_t codePoint)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			Events::KeyTypeEvent event(codePoint, data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetMouseButtonCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window,
		   const Input::MouseButton button,
		   const Input::KeyState state)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			if (state == Input::KeyState::Pressed)
			{
				Events::MouseButtonPressEvent event(button, data->Win);
				data->EventCallback(event);
			}
			else
			{
				Events::MouseButtonReleaseEvent event(button, data->Win);
				data->EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetScrollCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window,
		   const double xOffset, const double yOffset)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			Events::MouseScrollEvent event(NumericCast<float>(xOffset), NumericCast<float>(yOffset), data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetCursorPosCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, double xPos, double yPos)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if (!data->EventCallback)
				return;

#ifdef TRAP_PLATFORM_WINDOWS
			int32_t winPosX = 0, winPosY = 0;
			INTERNAL::WindowingAPI::GetWindowPos(window, winPosX, winPosY);
			xPos += winPosX;
			yPos += winPosY;
#endif /*TRAP_PLATFORM_WINDOWS*/

			Events::MouseMoveEvent event(NumericCast<float>(xPos), NumericCast<float>(yPos), data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetFrameBufferSizeCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, const int32_t w, const int32_t h)
		{
			WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));
			if(data == nullptr)
				return;

			if (!data->EventCallback || w == 0 || h == 0)
				return;

			Events::FrameBufferResizeEvent event(NumericCast<uint32_t>(w), NumericCast<uint32_t>(h), data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetCursorEnterCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, const bool entered)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			if (entered)
			{
				Events::MouseEnterEvent event(data->Win);
				data->EventCallback(event);
			}
			else
			{
				Events::MouseLeaveEvent event(data->Win);
				data->EventCallback(event);
			}
		}
	);

	INTERNAL::WindowingAPI::SetDropCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, std::vector<std::string> paths)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			Events::WindowDropEvent event(std::move(paths), data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetContentScaleCallback(*m_window,
		[](const INTERNAL::WindowingAPI::InternalWindow& window, const float xScale, const float yScale)
		{
			const WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(window));

			if ((data == nullptr) || !data->EventCallback)
				return;

			Events::WindowContentScaleEvent event(xScale, yScale, data->Win);
			data->EventCallback(event);
		}
	);

	INTERNAL::WindowingAPI::SetMonitorCallback([](const INTERNAL::WindowingAPI::InternalMonitor& mon,
	                                              const bool connected)
	{
		if(!connected && Monitor::GetAllMonitors().size() == 1)
			Utils::DisplayError(Utils::ErrorCode::MonitorNoneFound);

		if(mon.Window == nullptr)
			return;

		WindowData* const data = static_cast<WindowData*>(INTERNAL::WindowingAPI::GetWindowUserPointer(*mon.Window));
		if(data == nullptr)
			return;

		//Change window to windowed mode if monitor got disconnected
		if(!connected && mon.Window == data->Win->m_window)
		{
			data->Monitor = Monitor::GetPrimaryMonitor();
			data->Win->Restore();
			data->Win->SetWindowed(data->Win->GetWidth(), data->Win->GetHeight());
		}

		if (!data->EventCallback)
			return;

		for (const auto& m : Monitor::GetAllMonitors())
		{
			if(&mon != m.GetInternalMonitor())
				continue;

			if (connected)
			{
				Events::MonitorConnectEvent event(m);
				data->EventCallback(event);
			}
			else
			{
				Events::MonitorDisconnectEvent event(m);
				data->EventCallback(event);
			}

			return;
		}
	});
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::WindowProps::WindowProps(std::string title,
                               const uint32_t width,
							   const uint32_t height,
							   const double refreshRate,
							   const bool vsync,
							   const Window::DisplayMode displayMode,
							   AdvancedProps advanced,
							   const TRAP::Monitor& monitor) noexcept
	: Title(std::move(title)),
	  Width(width),
	  Height(height),
	  RefreshRate(refreshRate),
	  VSync(vsync),
	  DisplayMode(displayMode),
	  Monitor(monitor),
	  Advanced{advanced}
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);
}

#endif /*TRAP_HEADLESS_MODE*/