#include "TRAPPCH.h"
#include "Application.h"

#include "Core.h"
#include "VFS/VFS.h"
#include "Embed.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"
#include "Utils/String.h"
#include "Utils/MsgBox/MsgBox.h"
#include "Event/KeyEvent.h"
#include "Event/WindowEvent.h"

TRAP::Application* TRAP::Application::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Application()
	: m_timer(std::make_unique<Utils::Timer>()),
	  m_FramesPerSecond(0),
	  m_FrameTime(0.0f),
	  m_drawCalls(0),
	  m_fpsLimit(0),
	  m_tickRate(100),
      m_linuxWindowManager(LinuxWindowManager::Unknown)
{	
	TP_DEBUG("[Application] Initializing TRAP Modules...");

	TRAP_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	//Check if machine is using little-endian or big-endian
	int32_t intVal = 1;
	uint8_t* uVal = reinterpret_cast<uint8_t*>(&intVal);
#if __cplusplus > 201703L
	m_endian = static_cast<Endian>(std::endian::native == std::endian::little);
#else
	m_endian = static_cast<Endian>(uVal[0] == 1);
#endif

	UpdateLinuxWindowManager();
	
	//TODO Future Remove
	if(GetLinuxWindowManager() == LinuxWindowManager::Wayland)
	{
		TP_CRITICAL("[Engine][Wayland] Wayland is currently not supported by TRAP! Please us X11 instead");
		exit(-1);
	}

	VFS::Init();
	if (!m_config.LoadFromFile("Engine.cfg"))
		TP_INFO("[Config] Using default values");
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	m_config.Print();
#endif

	uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t refreshRate = 60;
	uint32_t vsync = 0;
	uint32_t fpsLimit = 0;
	Window::DisplayMode displayMode = Window::DisplayMode::Windowed;
	bool maximized = false;
	uint32_t monitor = 0;
	Graphics::API::RenderAPI renderAPI = Graphics::API::RenderAPI::NONE;
	m_config.Get("Width", width);
	m_config.Get("Height", height);
	m_config.Get("RefreshRate", refreshRate);
	m_config.Get("VSync", vsync);
	m_config.Get("FPSLimit", fpsLimit);
	m_config.Get("DisplayMode", displayMode);
	m_config.Get("Maximized", maximized);
	m_config.Get("Monitor", monitor);
	m_config.Get("RenderAPI", renderAPI);

	if (fpsLimit > 0)
	{
		if (fpsLimit >= 25 && fpsLimit <= 500)
			m_fpsLimit = fpsLimit;
		else
			m_fpsLimit = 0;
	}

	Graphics::API::Context::SetRenderAPI(renderAPI);
	m_window = MakeScope<Window>
	(
		WindowProps
		(
			"TRAP Engine",
			width,
			height,
			refreshRate,
			displayMode,
			WindowProps::Advanced{
				vsync,
				true,
				maximized,
				true,
				true,
				true,
				true,
				false,
				Window::CursorMode::Normal
			},
			monitor
		)
	);
	m_window->SetEventCallback([this](Event& e) {OnEvent(e); });

	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Fallback", Embed::FallbackVS, Embed::FallbackFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());
	Graphics::TextureManager::Add(Graphics::TextureCube::Create());

	//Initialize Renderer
	Graphics::Renderer::Init();

	m_layerStack = std::make_unique<LayerStack>();

	//Initialize Input for Joysticks
	m_input = std::make_unique<Input>();
	Input::SetEventCallback([this](Event& e) {OnEvent(e); });
	Input::Init();

	m_ImGuiLayer = std::make_unique<ImGuiLayer>();
	PushOverlay(std::move(m_ImGuiLayer));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::~Application()
{
	TP_DEBUG("[Application] Shutting down TRAP Modules...");
	m_input->Shutdown();
	m_layerStack.reset();
	m_config.Set("Width", m_window->GetWidth());
	m_config.Set("Height", m_window->GetHeight());
	m_config.Set("RefreshRate", m_window->GetRefreshRate());
	m_config.Set("VSync", m_window->GetVSyncInterval());
	m_config.Set("FPSLimit", m_fpsLimit);
	m_config.Set("DisplayMode", m_window->GetDisplayMode());
	m_config.Set("Maximized", m_window->IsMaximized());
	m_config.Set("Monitor", m_window->GetMonitor().GetID());
	m_config.Set("RenderAPI", Graphics::API::Context::GetRenderAPI());
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	m_config.Print();
#endif
	m_config.SaveToFile("Engine.cfg");
	m_window.reset();
	VFS::Shutdown();
};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Run()
{
	float lastFrameTime = 0.0f;
	std::deque<Utils::Timer> framesPerSecond;
	auto nextFrame = std::chrono::steady_clock::now();
	Utils::Timer tickTimer;	
	
	while (m_running)
	{
		if (m_fpsLimit)
			nextFrame += std::chrono::milliseconds(1000 / m_fpsLimit);
		if (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
			nextFrame += std::chrono::milliseconds(1000 / 30); //30 FPS
		
		m_drawCalls = 0;

		Utils::Timer FrameTimeTimer;
		const float time = m_timer->Elapsed();
		const Utils::TimeStep deltaTime{ time - lastFrameTime };
		lastFrameTime = time;

		if (!m_minimized)
		{
			for (const auto& layer : *m_layerStack)
				layer->OnUpdate(deltaTime);

			if (tickTimer.ElapsedMilliseconds() > 1000.0f / static_cast<float>(m_tickRate))
			{
				for (const auto& layer : *m_layerStack)
					layer->OnTick();

				tickTimer.Reset();
			}
		}

		Window::Use(m_window);
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
		{
			ImGuiLayer::Begin();
			for (const auto& layer : *m_layerStack)
				layer->OnImGuiRender();
			ImGuiLayer::End();
		}

		if (!m_minimized)
			Graphics::RenderCommand::Present(m_window);
		m_window->OnUpdate();

		//Update Shaders if needed
		if (VFS::GetHotShaderReloading() && VFS::GetShaderFileWatcher())
		{
			//Check monitoring shader folders for changes and
			//in case of changes run ShaderManager::Reload(virtualPath)
			VFS::GetShaderFileWatcher()->Check([](const std::filesystem::path& physicalPath,
				const std::string& virtualPath,
				const FileStatus status) -> void
				{
					//Process only regular files and FileStatus::Modified
					if (!is_regular_file(physicalPath))
						return;
					if (status == FileStatus::Created || status == FileStatus::Erased)
						return;

					if (Utils::String::ToLower(Utils::String::GetSuffix(virtualPath)) == "shader")
					{
						if (Graphics::ShaderManager::ExistsVirtualPath(virtualPath))
						{
							TP_INFO("[ShaderManager] Shader Modified Reloading...");
							Graphics::ShaderManager::Reload(virtualPath);
						}
					}
				});
		}
		//Update Textures if needed
		if (VFS::GetHotTextureReloading() && VFS::GetTextureFileWatcher())
		{
			//Check monitoring texture folders for changes and
			//in case of changes run TextureManager::Reload(virtualPath)
			VFS::GetTextureFileWatcher()->Check([](const std::filesystem::path& physicalPath,
				const std::string& virtualPath,
				const FileStatus status) -> void
				{
					//Process only regular files and FileStatus::Modified
					if (!is_regular_file(physicalPath))
						return;
					if (status == FileStatus::Created || status == FileStatus::Erased)
						return;

					const std::string suffix = Utils::String::ToLower(Utils::String::GetSuffix(virtualPath));
					if (suffix == "pgm" || suffix == "ppm" || suffix == "pnm" || suffix == "pam" || suffix == "pfm" ||
						suffix == "tga" || suffix == "icb" || suffix == "vda" || suffix == "vst" || suffix == "bmp" ||
						suffix == "dib")
					{
						if (Graphics::TextureManager::ExistsVirtualPath(virtualPath))
						{
							TP_INFO("[TextureManager] Texture Modified Reloading...");
							Graphics::TextureManager::Reload(virtualPath);
						}
					}
				});
		}

		if (Graphics::API::Context::s_newRenderAPI != Graphics::API::RenderAPI::NONE && Graphics::API::Context::s_newRenderAPI != Graphics::API::Context::GetRenderAPI())
		{
			if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL || Graphics::API::Context::s_newRenderAPI == Graphics::API::RenderAPI::OpenGL)
				ReCreateWindow(Graphics::API::Context::s_newRenderAPI);
			else
				ReCreate(Graphics::API::Context::s_newRenderAPI);

			Graphics::API::Context::SetRenderAPI(Graphics::API::Context::s_newRenderAPI);
		}

		if (!m_minimized)
		{
			m_FrameTime = FrameTimeTimer.ElapsedMilliseconds();
			m_FramesPerSecond = static_cast<uint32_t>(framesPerSecond.size());
			framesPerSecond.emplace_back();
			while (framesPerSecond.front().ElapsedMilliseconds() >= 1000.0f)
				framesPerSecond.pop_front();
		}

		//FPSLimiter
		if (m_fpsLimit || (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)))
			std::this_thread::sleep_until(nextFrame);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {return OnWindowClose(e); });
	dispatcher.Dispatch<FrameBufferResizeEvent>([this](FrameBufferResizeEvent& e) {return OnFrameBufferResize(e); });
	dispatcher.Dispatch<KeyPressEvent>([this](KeyPressEvent& e) {return OnKeyPress(e); });
	dispatcher.Dispatch<WindowFocusEvent>([this](WindowFocusEvent& e) {return OnWindowFocus(e); });
	dispatcher.Dispatch<WindowLostFocusEvent>([this](WindowLostFocusEvent& e) {return OnWindowLostFocus(e); });

	if (m_layerStack)
	{
		for (auto it = m_layerStack->end(); it != m_layerStack->begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushLayer(Scope<Layer> layer) const
{
	m_layerStack->PushLayer(std::move(layer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushOverlay(Scope<Layer> overlay) const
{
	m_layerStack->PushOverlay(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Config* TRAP::Application::GetConfig()
{
	return &m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::LayerStack& TRAP::Application::GetLayerStack() const
{
	return *m_layerStack;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetFPS()
{
	return s_Instance->m_FramesPerSecond;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Application::GetFrameTime()
{
	return s_Instance->m_FrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetDrawCalls()
{
	return s_Instance->m_drawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::AddSingleDrawCall()
{
	++(s_Instance->m_drawCalls);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetTickRate()
{
	return s_Instance->m_tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTickRate(const uint32_t tickRate)
{
	s_Instance->m_tickRate = tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetHotShaderReloading(const bool enabled)
{
	VFS::SetHotShaderReloading(enabled);
	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetHotTextureReloading(const bool enabled)
{
	VFS::SetHotTextureReloading(enabled);
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Shutdown()
{
	s_Instance->m_running = false;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Window>& TRAP::Application::GetWindow()
{
	return s_Instance->m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep TRAP::Application::GetTime()
{
	return s_Instance->GetTimeInternal();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Endian TRAP::Application::GetEndian()
{
	return s_Instance->m_endian;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::LinuxWindowManager TRAP::Application::GetLinuxWindowManager()
{
	return s_Instance->m_linuxWindowManager;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetClipboardString(const std::string& string)
{
	INTERNAL::WindowingAPI::SetClipboardString(string);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Application::GetClipboardString()
{
	return INTERNAL::WindowingAPI::GetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ReCreateWindow(const Graphics::API::RenderAPI renderAPI)
{
	for (const auto& layer : *m_layerStack)
		layer->OnDetach();
	Graphics::API::Context::SetRenderAPI(renderAPI);

	WindowProps props
	{
		std::string(m_window->GetTitle()),
		m_window->GetWidth(),
		m_window->GetHeight(),
		m_window->GetRefreshRate(),
		m_window->GetDisplayMode(),
		WindowProps::Advanced
		{
			m_window->GetVSyncInterval(),
			m_window->IsResizable(),
			m_window->IsMaximized(),
			true,
			true,
			true,
			m_window->IsDecorated(),
			m_window->GetRawMouseInput(),
			m_window->GetCursorMode()
		},
		m_window->GetMonitor().GetID()
	};	
	m_window.reset();
	m_window = std::make_unique<Window>(props);
	m_window->SetEventCallback([this](Event& e) {OnEvent(e); });
	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Fallback", Embed::FallbackVS, Embed::FallbackFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());
	Graphics::TextureManager::Add(Graphics::TextureCube::Create());	
	//Initialize Renderer
	Graphics::Renderer::Init();

	for (const auto& layer : *m_layerStack)
		layer->OnAttach();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ReCreate(const Graphics::API::RenderAPI renderAPI) const
{
	for (const auto& layer : *m_layerStack)
		layer->OnDetach();
	Graphics::API::Context::SetRenderAPI(renderAPI);

	Graphics::TextureManager::Shutdown();
	Graphics::ShaderManager::Shutdown();
	Graphics::Renderer::Shutdown();
	Graphics::API::RendererAPI::Shutdown();
	Graphics::API::Context::Shutdown();

	Graphics::API::Context::Create(m_window.get());
	Graphics::API::Context::SetVSyncInterval(m_window->GetVSyncInterval());
	Graphics::API::RendererAPI::Init();
	m_window->SetTitle(std::string(m_window->GetTitle()));
	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Fallback", Embed::FallbackVS, Embed::FallbackFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());
	Graphics::TextureManager::Add(Graphics::TextureCube::Create());	
	//Initialize Renderer
	Graphics::Renderer::Init();

	for (const auto& layer : *m_layerStack)
		layer->OnAttach();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::UpdateLinuxWindowManager()
{	
#ifdef TRAP_PLATFORM_LINUX
	if (std::getenv("WAYLAND_DISPLAY") || std::getenv("XDG_SESSION_TYPE") == "wayland")
		m_linuxWindowManager = LinuxWindowManager::Wayland;
	else if (std::getenv("DISPLAY") || std::getenv("XDG_SESSION_TYPE") == "x11")
		m_linuxWindowManager = LinuxWindowManager::X11;
	else
	{
		TP_CRITICAL("[Engine][Linux] Unsupported Window Manager!");
		Show("Window Manager is unsupported!\nTRAP Engine uses X11 or Wayland\nMake sure the appropriate environment variables are set!", "Unsupported Window Manager", Utils::MsgBox::Style::Error, Utils::MsgBox::Buttons::Quit);
		exit(-1);
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep TRAP::Application::GetTimeInternal() const
{
	const Utils::TimeStep timeStep(m_timer->Elapsed());

	return timeStep;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_running = false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnFrameBufferResize(FrameBufferResizeEvent& e)
{
	if (Window::GetActiveWindows() > 1)
		Window::Use();

	if ((e.GetWidth() == 0 || e.GetHeight() == 0) && Window::GetActiveWindows() == 1)
	{
		m_minimized = true;

		return false;
	}

	Graphics::RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());

	m_minimized = false;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnKeyPress(KeyPressEvent& e) const
{
	if (Window::GetActiveWindows() == 1)
	{
		if ((e.GetKeyCode() == Input::Key::Enter || e.GetKeyCode() == Input::Key::KP_Enter) && Input::IsKeyPressed(Input::Key::Left_ALT) && e.GetRepeatCount() < 1)
		{
			if (m_window->GetDisplayMode() == Window::DisplayMode::Windowed ||
				m_window->GetDisplayMode() == Window::DisplayMode::Borderless)
				m_window->SetDisplayMode(Window::DisplayMode::Fullscreen, 0, 0, 0);
			else if (m_window->GetDisplayMode() == Window::DisplayMode::Fullscreen)
				m_window->SetDisplayMode(Window::DisplayMode::Windowed, 0, 0, 0);
		}
	}
	
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowFocus(WindowFocusEvent& e)
{
	m_focused = true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowLostFocus(WindowLostFocusEvent& e)
{
	if (Window::GetActiveWindows() == 1)
		m_focused = false;
	
	return false;
}