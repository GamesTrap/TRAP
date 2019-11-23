#include "TRAPPCH.h"
#include "Application.h"

#include "VFS/VFS.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Embed.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Input/Input.h"
#include "Utils/String.h"
#include "Core.h"
#include "Graphics/Renderer.h"
#include "Utils/MsgBox/MsgBox.h"

TRAP::Application* TRAP::Application::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Application()
	: m_timer(std::make_unique<Utils::Timer>()),
	m_FramesPerSecond(0),
	m_FrameTime(0.0f),
	m_drawCalls(0),
	m_fpsLimit(0),
	m_tickRate(100)
{
	TP_DEBUG("[Application] Initializing TRAP Modules...");

	TRAP_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

#ifdef TRAP_PLATFORM_WINDOWS
	CheckIfWindows7OrNewer();
#endif
	
	//Check if machine is using little-endian or big-endian
	int32_t intVal = 1;
	uint8_t* uVal = reinterpret_cast<uint8_t*>(&intVal);
	m_endian = static_cast<Endian>(uVal[0] == 1);

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
	uint32_t monitor = 0;
	Graphics::API::RenderAPI renderAPI = Graphics::API::RenderAPI::NONE;
	Input::ControllerAPI controllerAPI = Input::ControllerAPI::Unknown;
#ifdef TRAP_PLATFORM_WINDOWS
	controllerAPI = Input::ControllerAPI::XInput;
#elif defined(TRAP_PLATFORM_LINUX)
	controllerAPI = Input::ControllerAPI::Linux;
#endif
	bool hotShaderReloading = false;
	bool hotTextureReloading = false;
	m_config.Get("Width", width);
	m_config.Get("Height", height);
	m_config.Get("RefreshRate", refreshRate);
	m_config.Get("VSync", vsync);
	m_config.Get("FPSLimit", fpsLimit);
	m_config.Get("DisplayMode", displayMode);
	m_config.Get("Monitor", monitor);
	m_config.Get("RenderAPI", renderAPI);
	m_config.Get("ControllerAPI", controllerAPI);
	m_config.Get("HotShaderReloading", hotShaderReloading);
	m_config.Get("HotTextureReloading", hotTextureReloading);

	if (fpsLimit > 0)
	{
		if (fpsLimit >= 25 && fpsLimit <= 500)
			m_fpsLimit = fpsLimit;
		else
			m_fpsLimit = 0;
	}

	VFS::Get()->SetHotShaderReloading(hotShaderReloading);
	VFS::Get()->SetHotTextureReloading(hotTextureReloading);

	Graphics::API::Context::SetRenderAPI(renderAPI);
	m_window = std::make_unique<Window>
		(
			WindowProps
			(
				"Sandbox",
				width,
				height,
				refreshRate,
				vsync,
				displayMode,
				monitor
			)
			);
	m_window->SetEventCallback(TRAP_BIND_EVENT_FN(Application::OnEvent));

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
	Input::SetEventCallback(TRAP_BIND_EVENT_FN(Application::OnEvent));
	Input::Init(controllerAPI);

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
	m_config.Set("Monitor", m_window->GetMonitor());
	m_config.Set("RenderAPI", Graphics::API::Context::GetRenderAPI());
	m_config.Set("ControllerAPI", Input::GetControllerAPI());
	m_config.Set("HotShaderReloading", VFS::Get()->GetHotShaderReloading());
	m_config.Set("HotTextureReloading", VFS::Get()->GetHotTextureReloading());
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
		if (VFS::Get()->GetHotShaderReloading() && VFS::Get()->GetShaderFileWatcher())
		{
			//Check monitoring shader folders for changes and
			//in case of changes run ShaderManager::Reload(virtualPath)
			VFS::Get()->GetShaderFileWatcher()->Check([](const std::filesystem::path& physicalPath,
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
		if (VFS::Get()->GetHotTextureReloading() && VFS::Get()->GetTextureFileWatcher())
		{
			//Check monitoring texture folders for changes and
			//in case of changes run TextureManager::Reload(virtualPath)
			VFS::Get()->GetTextureFileWatcher()->Check([](const std::filesystem::path& physicalPath,
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

		m_FrameTime = FrameTimeTimer.ElapsedMilliseconds();
		m_FramesPerSecond = static_cast<uint32_t>(framesPerSecond.size());
		framesPerSecond.emplace_back();
		while (framesPerSecond.front().ElapsedMilliseconds() >= 1000.0f)
			framesPerSecond.pop_front();

		//FPSLimiter
		if (m_fpsLimit)
			std::this_thread::sleep_until(nextFrame);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(TRAP_BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(TRAP_BIND_EVENT_FN(Application::OnWindowResize));

	m_input->OnEvent(e); //Controller Connect/Disconnect Events

	for (auto it = m_layerStack->end(); it != m_layerStack->begin();)
	{
		(*--it)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushLayer(Scope<Layer> layer)
{
	m_layerStack->PushLayer(std::move(layer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushOverlay(Scope<Layer> overlay)
{
	m_layerStack->PushOverlay(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Config* TRAP::Application::GetConfig()
{
	return &m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::LayerStack& TRAP::Application::GetLayerStack()
{
	return *m_layerStack;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetFPS() const
{
	return m_FramesPerSecond;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Application::GetFrameTime() const
{
	return m_FrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetDrawCalls() const
{
	return m_drawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::AddSingleDrawCall()
{
	++m_drawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetTickRate() const
{
	return m_tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTickRate(const uint32_t tickRate)
{
	m_tickRate = tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Shutdown()
{
	Get().m_running = false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application& TRAP::Application::Get()
{
	return *s_Instance;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Window>& TRAP::Application::GetWindow()
{
	return Get().m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep TRAP::Application::GetTime()
{
	return Get().GetTimeInternal();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Endian TRAP::Application::GetEndian()
{
	return Get().m_endian;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ReCreateWindow(const Graphics::API::RenderAPI renderAPI)
{
	for (const auto& layer : *m_layerStack)
		layer->OnDetach();
	Graphics::API::Context::SetRenderAPI(renderAPI);

	WindowProps props{ std::string(m_window->GetTitle()), m_window->GetWidth(), m_window->GetHeight(), m_window->GetRefreshRate(), m_window->GetVSyncInterval(), m_window->GetDisplayMode(), m_window->GetMonitor() };
	m_window.reset();
	m_window = std::make_unique<Window>(props);
	m_window->SetEventCallback(TRAP_BIND_EVENT_FN(Application::OnEvent));
	//Initialize Renderer
	Graphics::Renderer::Init();
	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Fallback", Embed::FallbackVS, Embed::FallbackFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());

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
	//Initialize Renderer
	Graphics::Renderer::Init();
	//Note: Input doesn't need to be Initialized here because GLFW is still initialized
	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Fallback", Embed::FallbackVS, Embed::FallbackFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());

	for (const auto& layer : *m_layerStack)
		layer->OnAttach();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
void TRAP::Application::CheckIfWindows7OrNewer() const
{
	if(!IsWindows7OrGreater())
	{
		//Windows Version is older than XInput 1.4 requires!
		TP_CRITICAL("[Engine] Unsupported Windows Version!");
		Show("Windows Version is unsupported!\nWindows Version is older than Windows 7!\nTRAP Engine requires Windows 7 or newer!",
		     "Unsupported Windows Version",
		     Utils::MsgBox::Style::Error,
		     Utils::MsgBox::Buttons::Quit);
		exit(-1);
	}
}
#endif

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

bool TRAP::Application::OnWindowResize(WindowResizeEvent& e)
{
	if (Window::GetActiveWindows() > 1)
		Window::Use();

	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_minimized = true;

		return false;
	}

	Graphics::RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());

	m_minimized = false;

	return false;
}