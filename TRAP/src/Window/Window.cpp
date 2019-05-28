#include "TRAPPCH.h"
#include "Window.h"

#include <glad/glad.h>

namespace TRAP
{
static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(const int error, const char *description)
{
	TP_ERROR("[Window][GLFW] (", error, "): ", description);
}

Window::Window(const WindowProps &props)
{
	std::string init = "[Window] Initializing Window: \"" + props.Title + "\" " + std::to_string(props.Width) + 'x' + std::to_string(props.Height) + "@" + std::to_string(props.RefreshRate) + "Hz VSync: ";
	if (props.VSync > 0)
		init += "On(" + std::to_string(props.VSync) + ") Mode: ";
	else
		init += "Off Mode: ";

	if (props.Mode == DisplayMode::WINDOWED)
		init += "Windowed";
	else if (props.Mode == DisplayMode::BORDERLESS)
		init += "Borderless";
	else if (props.Mode == DisplayMode::FULLSCREEN)
		init += "Fullscreen";

	init += " Monitor: " + std::to_string(props.Monitor);

	TP_INFO(init);
	Init(props);
}

Window::~Window()
{
	Graphics::ShaderManager::Shutdown();
	TP_DEBUG("[Window] Destroying Window: \"", m_data.Title, "\"");
	Shutdown();
}

void Window::Init(const WindowProps &props)
{
	m_data.Title = props.Title;
	m_data.Width = props.Width;
	m_data.Height = props.Height;
	m_data.RefreshRate = props.RefreshRate;

	if (!s_GLFWInitialized)
	{
		const int success = glfwInit();
		TP_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
		s_GLFWInitialized = true;
	}

	SetMonitor(props.Monitor);

	m_baseVideoMode = *(glfwGetVideoMode(m_useMonitor));
	TP_DEBUG("[Window] Storing underlying OS video mode: ", m_baseVideoMode.width, 'x', m_baseVideoMode.height, '@', m_baseVideoMode.refreshRate, "Hz (R", m_baseVideoMode.redBits, 'G', m_baseVideoMode.greenBits, 'B', m_baseVideoMode.blueBits, ')');

	if (props.RenderAPI != Graphics::API::RenderAPI::OPENGL)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	if (props.RenderAPI == Graphics::API::RenderAPI::OPENGL)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}
	Graphics::API::Context::SetRenderAPI(props.RenderAPI);

	//Create Window
	std::string newTitle = m_data.Title + " - TRAP Engine V" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) + "." + std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) + "." + std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) + "[INDEV][19w22a1]";
	m_window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), newTitle.c_str(), nullptr, nullptr);

	//Create Context & Initialize Renderer
	Graphics::API::Context::Create(this);
	Graphics::API::Context::SetVSyncInterval(props.VSync);
	Graphics::Renderer::Init();

	//Update Window Title
	newTitle += Graphics::Renderer::GetTitle();
	glfwSetWindowTitle(m_window, newTitle.c_str());

	//Change to windowed, fullscreen, or fullscreen borderless
	//Store current information about the window location and size(starting location/size)
	m_oldWindowedParams.Width = props.Width;
	m_oldWindowedParams.Height = props.Height;
	m_oldWindowedParams.RefreshRate = props.RefreshRate;
	glfwGetWindowPos(m_window, &(m_oldWindowedParams.XPos), &(m_oldWindowedParams.YPos));
	SetWindowMode(props.Mode, 0, 0);

	glfwSetWindowUserPointer(m_window, &m_data);

	//Set GLFW callbacks
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow *window, const int width, const int height) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
		data.Width = width;
		data.Height = height;

		WindowResizeEvent event(width, height);
		data.EventCallback(event);
	});

	glfwSetWindowPosCallback(m_window, [](GLFWwindow *window, const int x, const int y) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
		WindowMovedEvent event(x, y);
		data.EventCallback(event);
	});

	glfwSetWindowFocusCallback(m_window, [](GLFWwindow *window, const int focused) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		if (focused)
		{
			WindowFocusEvent event;
			data.EventCallback(event);
		}
		else
		{
			WindowLostFocusEvent event;
			data.EventCallback(event);
		}
	});

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow *window) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
		WindowCloseEvent event;
		data.EventCallback(event);
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow *window, const int key, int scancode, const int action, int mods) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		switch (action)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(key, 0);
			data.EventCallback(event);
			break;
		}

		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(key);
			data.EventCallback(event);
			break;
		}

		case GLFW_REPEAT:
		{
			KeyPressedEvent event(key, 1);
			data.EventCallback(event);
			break;
		}

		default:
			break;
		}
	});

	glfwSetCharCallback(m_window, [](GLFWwindow *window, const unsigned int keycode) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		KeyTypedEvent event(static_cast<int>(keycode));
		data.EventCallback(event);
	});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow *window, const int button, const int action, int mods) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		switch (action)
		{
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event(button);
			data.EventCallback(event);
			break;
		}

		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(button);
			data.EventCallback(event);
			break;
		}

		default:
			break;
		}
	});

	glfwSetScrollCallback(m_window, [](GLFWwindow *window, const double xOffset, const double yOffset) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
		data.EventCallback(event);
	});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, const double xPos, const double yPos) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
		data.EventCallback(event);
	});
}

void Window::Shutdown() const
{
	glfwDestroyWindow(m_window);
}

void Window::OnUpdate()
{
	glfwPollEvents();
}

void Window::SetWindowMode(const DisplayMode &mode, unsigned int width, unsigned int height, unsigned int refreshRate)
{
	if (!m_window) //Ensure there is a window to work on
		return;
	if (mode == m_data.Mode) //Nothing to do as this is not a change
		return;

	//If currently windowed, stash the current size and position of the window
	if (m_data.Mode == DisplayMode::WINDOWED)
	{
		m_oldWindowedParams.Width = m_data.Width;
		m_oldWindowedParams.Height = m_data.Height;
		m_oldWindowedParams.RefreshRate = m_data.RefreshRate;
		glfwGetWindowPos(m_window, &(m_oldWindowedParams.XPos), &(m_oldWindowedParams.YPos));
	}

	GLFWmonitor *monitor = nullptr;

	if (mode == DisplayMode::BORDERLESS)
	{
		//For borderless fullscreen, the new width and height will be the video mode width and height
		width = m_baseVideoMode.width;
		height = m_baseVideoMode.height;
		refreshRate = m_baseVideoMode.refreshRate;
		monitor = m_useMonitor;
		TP_DEBUG("[Window] Using Monitor: ", m_data.Monitor, '(', glfwGetMonitorName(monitor), ')');
	}
	else if (mode == DisplayMode::WINDOWED && (width == 0 || height == 0))
	{
		//For windowed, use old window height and width if none provided
		width = m_oldWindowedParams.Width;
		height = m_oldWindowedParams.Height;
		refreshRate = m_oldWindowedParams.RefreshRate;
	}
	else if (mode == DisplayMode::FULLSCREEN)
	{
		monitor = m_useMonitor;
		if (width == 0 || height == 0)
		{
			//Use the old window size
			int monitorVideoModesCount = 0;
			const auto monitorVideoModes = glfwGetVideoModes(m_useMonitor, &monitorVideoModesCount);
			for (int i = 0; i < monitorVideoModesCount; i++)
			{
				//Check if resolution pair is valid
				if (static_cast<unsigned int>(monitorVideoModes[i].width) == m_data.Width && static_cast<unsigned int>(monitorVideoModes[i].height) == m_data.Height && static_cast<unsigned int>(monitorVideoModes[i].refreshRate))
				{
					width = m_data.Width;
					height = m_data.Height;
					refreshRate = m_data.RefreshRate;
				}
			}

			//Resolution pair is invalid so use default
			if (width == 0 || height == 0)
			{
				width = monitorVideoModes[monitorVideoModesCount - 1].width;
				height = monitorVideoModes[monitorVideoModesCount - 1].height;
				refreshRate = monitorVideoModes[monitorVideoModesCount - 1].refreshRate;
			}
		}

		TP_DEBUG("[Window] Using Monitor: ", m_data.Monitor, '(', glfwGetMonitorName(monitor), ')');
	}

	//Update stored width and height
	m_data.Width = width;
	m_data.Height = height;
	m_data.RefreshRate = refreshRate;

	//Trigger resize event
	if (m_data.EventCallback)
	{
		WindowResizeEvent event(width, height);
		m_data.EventCallback(event);
	}

	constexpr auto GetModeStr = [&](const DisplayMode displayMode) {if (displayMode == DisplayMode::WINDOWED) return "Windowed"; return displayMode == DisplayMode::BORDERLESS ? "Borderless" : "Fullscreen"; }; //Little hack to convert enum class DisplayMode to string
	TP_INFO("[Window] Changing window mode from ", GetModeStr(m_data.Mode), " to ", GetModeStr(mode), ": ", width, 'x', height, '@', refreshRate, "Hz");

	//Record new window type
	m_data.Mode = mode;

	glfwSetWindowMonitor(m_window, monitor, m_oldWindowedParams.XPos, m_oldWindowedParams.YPos, static_cast<int>(width), static_cast<int>(height), static_cast<int>(refreshRate));
}

void Window::SetMonitor(const unsigned int monitor)
{
	int monitorCount;
	const auto monitors = glfwGetMonitors(&monitorCount);
	if (monitor <= static_cast<unsigned int>(monitorCount))
	{
		m_data.Monitor = monitor;
		m_useMonitor = monitors[monitor];
	}
	else
	{
		m_data.Monitor = 0;
		m_useMonitor = glfwGetPrimaryMonitor();
	}
}

void Window::Clear()
{
	Graphics::Renderer::Clear(static_cast<int>(Graphics::RendererBufferType::RENDERER_BUFFER_COLOR) | static_cast<int>(Graphics::RendererBufferType::RENDERER_BUFFER_DEPTH));
}
} // namespace TRAP
