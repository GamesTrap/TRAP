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

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::s_windows = 0;

//-------------------------------------------------------------------------------------------------------------------//

static void GLFWErrorCallback(const int32_t error, const char *description)
{
	TP_ERROR("[Window][GLFW] (", error, "): ", description);
}

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
		Graphics::Renderer::Cleanup();
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
	glfwPollEvents();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Use(Window* window)
{
	if (window)
		Graphics::API::Context::Use(window);
	else
		Use();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Use()
{
	Graphics::API::Context::Use(Application::Get().GetWindow());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetActiveWindows()
{
	return s_windows;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetMonitors()
{
	uint32_t monitors = 0;
	glfwGetMonitors(reinterpret_cast<int32_t*>(&monitors));

	return monitors;
}

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<uint32_t, std::string> TRAP::Window::GetMonitorNames()
{
	uint32_t monitorCount = 0;
	const auto monitors = glfwGetMonitors(reinterpret_cast<int32_t*>(&monitorCount));

	std::unordered_map<uint32_t, std::string> result;
	
	for(uint32_t i = 0; i < monitorCount; i++)
		result.try_emplace(i, glfwGetMonitorName(monitors[i]));

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Window::GetTitle() const
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

TRAP::DisplayMode TRAP::Window::GetDisplayMode() const
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
	return glfwGetMonitorName(m_useMonitor);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Window::GetVSyncInterval() const
{
	return m_data.VSync;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::CursorMode TRAP::Window::GetCursorMode() const
{
	return m_data.cursorMode;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Window::GetRawMouseInput() const
{
	return m_data.rawMouseInput;
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::Window::GetNativeWindow() const
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
		"[INDEV][19w36a7]" + std::string(Graphics::Renderer::GetTitle());
	glfwSetWindowTitle(m_window, newTitle.c_str());
#else
	glfwSetWindowTitle(m_window, m_data.Title.c_str());
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
		glfwGetWindowPos(m_window, &(m_oldWindowedParams.XPos), &(m_oldWindowedParams.YPos));
	}

	GLFWmonitor* monitor = nullptr;

	if (mode == DisplayMode::Borderless)
	{
		//For borderless fullscreen, the new width, height and refresh rate will be the video mode width, height and refresh rate
		width = m_baseVideoMode.width;
		height = m_baseVideoMode.height;
		refreshRate = m_baseVideoMode.refreshRate;
		monitor = m_useMonitor;
		TP_DEBUG("[Window] \"", m_data.Title, "\" Using Monitor: ", m_data.Monitor, '(', glfwGetMonitorName(monitor), ')');
	}
	else if (mode == DisplayMode::Windowed && (width == 0 || height == 0))
	{
		//For windowed, use old window height and width if none provided
		width = m_oldWindowedParams.Width;
		height = m_oldWindowedParams.Height;
		refreshRate = m_oldWindowedParams.RefreshRate;
	}
	else if (mode == DisplayMode::Fullscreen)
	{
		monitor = m_useMonitor;
		bool valid = false;
		int32_t monitorVideoModesCount = 0;
		const auto monitorVideoModes = glfwGetVideoModes(m_useMonitor, &monitorVideoModesCount);
		if (width != 0 && height != 0 && refreshRate != 0)
		{
			for (int32_t i = 0; i < monitorVideoModesCount; i++)
			{
				//Check if resolution pair is valid
				if (static_cast<uint32_t>(monitorVideoModes[i].width) == width && static_cast<uint32_t>(monitorVideoModes[i].height) == height && static_cast<uint32_t>(monitorVideoModes[i].refreshRate) == refreshRate)
				{
					valid = true;
					break;
				}
			}
		}

		//Resolution pair is invalid so use native/default resolution
		if (!valid)
		{
			width = monitorVideoModes[monitorVideoModesCount - 1].width;
			height = monitorVideoModes[monitorVideoModesCount - 1].height;
			refreshRate = monitorVideoModes[monitorVideoModesCount - 1].refreshRate;
		}

		TP_DEBUG("[Window] \"", m_data.Title, "\" Using Monitor: ", m_data.Monitor, '(', glfwGetMonitorName(monitor), ')');
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

	constexpr auto GetModeStr = [&](const DisplayMode displayMode) {
		if (displayMode == DisplayMode::Windowed)
			return "Windowed";
		return displayMode == DisplayMode::Borderless ? "Borderless" : "Fullscreen";
	}; //Little hack to convert enum class DisplayMode to string
	TP_INFO("[Window] \"", m_data.Title, "\" Changing window mode from ",
		GetModeStr(m_data.displayMode), " to ", GetModeStr(mode), ": ",
		width, 'x', height, '@', refreshRate, "Hz");

	//Record new window type
	m_data.displayMode = mode;

	glfwSetWindowMonitor(m_window,
		monitor,
		m_oldWindowedParams.XPos,
		m_oldWindowedParams.YPos,
		static_cast<int32_t>(width),
		static_cast<int32_t>(height),
		static_cast<int32_t>(refreshRate));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetMonitor(const uint32_t monitor)
{
	int32_t monitorCount;
	const auto monitors = glfwGetMonitors(&monitorCount);
	if (monitor <= static_cast<uint32_t>(monitorCount))
	{
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set Monitor: ", monitor, " Name: ", glfwGetMonitorName(monitors[monitor]));
		m_data.Monitor = monitor;
		m_useMonitor = monitors[monitor];
	}
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Invalid Monitor!");
		TP_WARN("[Window] \"", m_data.Title, "\" Using Primary Monitor!");
		m_data.Monitor = 0;
		m_useMonitor = glfwGetPrimaryMonitor();
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
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_data.cursorMode = mode;
	}
	else if (mode == CursorMode::Hidden)
	{
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Hidden");
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		m_data.cursorMode = mode;
	}
	else if (mode == CursorMode::Disabled)
	{
		TP_DEBUG("[Window] \"", m_data.Title, "\" Set CursorMode: Disabled");
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_data.cursorMode = mode;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetRawMouseInput(const bool enabled)
{
	if(Input::IsRawMouseInputSupported())
	{
		m_data.rawMouseInput = enabled;
		glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, enabled);
		TP_DEBUG("[Window] \"", m_data.Title, "\" Raw Mouse Input ", enabled ? "Enabled" : "Disabled");
	}
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Raw Mouse Input is unsupported!");
		m_data.rawMouseInput = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetIcon() const
{
	GLFWimage image{ 32, 32, Utils::TRAPLogo.data() };
	glfwSetWindowIcon(m_window, 1, &image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetIcon(const std::unique_ptr<Image>& image) const
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

	GLFWimage glfwImage{ static_cast<int32_t>(image->GetWidth()), static_cast<int32_t>(image->GetHeight()), static_cast<uint8_t*>(image->GetPixelData()) };
	glfwSetWindowIcon(m_window, 1, &glfwImage);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::SetEventCallback(const EventCallbackFn& callback)
{
	m_data.EventCallback = callback;
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
	m_data.rawMouseInput = props.rawMouseInput;

	static bool sGLFWInitialized = false;
	if (!sGLFWInitialized)
	{
		const int32_t success = glfwInit();
		TP_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
		sGLFWInitialized = true;

		Graphics::API::Context::CheckAllRenderAPIs();
	}
	
	int32_t monitorCount;
	const auto monitors = glfwGetMonitors(&monitorCount);
	if (props.Monitor <= static_cast<uint32_t>(monitorCount))
		m_useMonitor = monitors[props.Monitor];
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Invalid Monitor!");
		TP_WARN("[Window] \"", m_data.Title, "\" Using Primary Monitor!");
		m_data.Monitor = 0;
		m_useMonitor = glfwGetPrimaryMonitor();
	}

	m_baseVideoMode = *(glfwGetVideoMode(m_useMonitor));
	TP_DEBUG("[Window] Storing underlying OS video mode: ",
		m_baseVideoMode.width, 'x', m_baseVideoMode.height, '@', m_baseVideoMode.refreshRate, "Hz (R",
		m_baseVideoMode.redBits, 'G', m_baseVideoMode.greenBits, 'B', m_baseVideoMode.blueBits, ')');

	glfwDefaultWindowHints();

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
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}
	}

	//Create Window
#ifndef TRAP_RELEASE
	std::string newTitle = m_data.Title + " - TRAP Engine V" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) + "." +
		std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) + "." + std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) +
		"[INDEV][19w36a7]";
#else
	const std::string newTitle = m_data.Title;
#endif

	m_window = glfwCreateWindow(static_cast<int32_t>(props.Width),
		static_cast<int32_t>(props.Height),
		newTitle.c_str(),
		nullptr,
		nullptr);

	if (m_window == nullptr)
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
		Use(this);
		SetVSyncInterval(props.VSync);
		Use();
	}

	//Update Window Title
#ifndef TRAP_RELEASE
	newTitle += Graphics::Renderer::GetTitle();
#endif
	glfwSetWindowTitle(m_window, newTitle.c_str());

	{
		uint32_t width = 0, height = 0, refreshRate = 0;
		
		//If currently windowed, stash the current size and position of the window
		if (m_data.displayMode == DisplayMode::Windowed)
		{
			m_oldWindowedParams.Width = m_data.Width;
			m_oldWindowedParams.Height = m_data.Height;
			m_oldWindowedParams.RefreshRate = m_data.RefreshRate;
			glfwGetWindowPos(m_window, &(m_oldWindowedParams.XPos), &(m_oldWindowedParams.YPos));
		}

		GLFWmonitor* monitor = nullptr;

		if (props.displayMode == DisplayMode::Borderless)
		{
			//For borderless fullscreen, the new width, height and refresh rate will be the video mode width, height and refresh rate
			width = m_baseVideoMode.width;
			height = m_baseVideoMode.height;
			refreshRate = m_baseVideoMode.refreshRate;
			monitor = m_useMonitor;
		}
		else if (props.displayMode == DisplayMode::Windowed && (width == 0 || height == 0))
		{
			//For windowed, use old window height and width if none provided
			width = m_oldWindowedParams.Width;
			height = m_oldWindowedParams.Height;
			refreshRate = m_oldWindowedParams.RefreshRate;
		}
		else if (props.displayMode == DisplayMode::Fullscreen)
		{
			monitor = m_useMonitor;
			bool valid = false;
			int32_t monitorVideoModesCount = 0;
			const auto monitorVideoModes = glfwGetVideoModes(m_useMonitor, &monitorVideoModesCount);
			if (width != 0 && height != 0 && refreshRate != 0)
			{
				for (int32_t i = 0; i < monitorVideoModesCount; i++)
				{
					//Check if resolution pair is valid
					if (static_cast<uint32_t>(monitorVideoModes[i].width) == width && static_cast<uint32_t>(monitorVideoModes[i].height) == height && static_cast<uint32_t>(monitorVideoModes[i].refreshRate) == refreshRate)
					{
						valid = true;
						break;
					}
				}
			}

			//Resolution pair is invalid so use native/default resolution
			if (!valid)
			{
				width = monitorVideoModes[monitorVideoModesCount - 1].width;
				height = monitorVideoModes[monitorVideoModesCount - 1].height;
				refreshRate = monitorVideoModes[monitorVideoModesCount - 1].refreshRate;
			}
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

		//Record new window type
		m_data.displayMode = props.displayMode;

		glfwSetWindowMonitor(m_window,
			monitor,
			m_oldWindowedParams.XPos,
			m_oldWindowedParams.YPos,
			static_cast<int32_t>(width),
			static_cast<int32_t>(height),
			static_cast<int32_t>(refreshRate));
	}

	glfwSetWindowUserPointer(m_window, &m_data);

	SetIcon();

	if (m_data.cursorMode == CursorMode::Normal)
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else if (m_data.cursorMode == CursorMode::Hidden)
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	else if (m_data.cursorMode == CursorMode::Disabled)
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	if (Input::IsRawMouseInputSupported())
		glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, m_data.rawMouseInput);
	else
	{
		TP_ERROR("[Window] \"", m_data.Title, "\" Raw Mouse Input is unsupported!");
		m_data.rawMouseInput = false;
	}

	//Set GLFW callbacks
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, const int32_t width, const int32_t height) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data.Width = width;
		data.Height = height;

		WindowResizeEvent event(width, height, data.Title);
		data.EventCallback(event);
		});

	glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, const int32_t x, const int32_t y) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		WindowMovedEvent event(x, y, data.Title);
		data.EventCallback(event);
		});

	glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, const int32_t focused) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

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

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		WindowCloseEvent event(data.Title);
		data.EventCallback(event);
		});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, const int32_t key, int32_t scancode, const int32_t action, int32_t mods) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(static_cast<Key>(key), 0, data.Title);
			data.EventCallback(event);
			break;
		}

		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(static_cast<Key>(key), data.Title);
			data.EventCallback(event);
			break;
		}

		case GLFW_REPEAT:
		{
			KeyPressedEvent event(static_cast<Key>(key), 1, data.Title);
			data.EventCallback(event);
			break;
		}

		default:
			break;
		}
		});

	glfwSetCharCallback(m_window, [](GLFWwindow* window, const uint32_t keycode) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		KeyTypedEvent event(static_cast<Key>(keycode), data.Title);
		data.EventCallback(event);
		});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, const int32_t button, const int32_t action, int32_t mods) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event(static_cast<MouseButton>(button), data.Title);
			data.EventCallback(event);
			break;
		}

		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(static_cast<MouseButton>(button), data.Title);
			data.EventCallback(event);
			break;
		}

		default:
			break;
		}
		});

	glfwSetScrollCallback(m_window, [](GLFWwindow* window, const double xOffset, const double yOffset) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset), data.Title);
		data.EventCallback(event);
		});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, const double xPos, const double yPos) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos), data.Title);
		data.EventCallback(event);
		});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Window::Shutdown()
{
	glfwDestroyWindow(m_window);
	m_window = nullptr;
}