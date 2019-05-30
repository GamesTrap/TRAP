#include "TRAPPCH.h"
#include "Application.h"

/*void SwitchRenderAPI(Graphics::API::RenderAPI currentRenderAPI) TBC
{
	static bool d3d12Failed = false;

	switch(currentRenderAPI)
	{
	case Graphics::API::RenderAPI::D3D12:
		d3d12Failed = true;
		return; //Switch to Vulkan

	case Graphics::API::RenderAPI::VULKAN:
#ifdef TRAP_PLATFORM_WINDOWS
		if(!d3d12Failed) //Switch to D3D12
		{
			return;
		}
#endif
		return; //Switch to OpenGL

	case Graphics::API::RenderAPI::OPENGL: //Needs Window Recreation(WindowHints???)
		return; //Exit Program(No Renderer Supported!)

	default:
		return;
	}
}*/

//-------------------------------------------------------------------------------------------------------------------//

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
TRAP::Application* TRAP::Application::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Application()
	: m_timer(std::make_unique<Utils::Timer>()), m_FramesPerSecond(0), m_Frametime(0.0f)
{
	TP_DEBUG("[Application] Initializing TRAP Modules...");

	TP_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	VFS::Init();
	if (!m_config.LoadFromFile("Engine.cfg"))
		TP_INFO("[Config] Using default values");
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	m_config.Print();
#endif

	unsigned int width = 1280;
	unsigned int height = 720;
	unsigned int refreshRate = 60;
	unsigned int vsync = 0;
	DisplayMode displayMode = DisplayMode::WINDOWED;
	unsigned int monitor;
	Graphics::API::RenderAPI renderAPI = Graphics::API::RenderAPI::VULKAN;
	bool hotShaderReloading = false;
	m_config.Get("Width", width);
	m_config.Get("Height", height);
	m_config.Get("RefreshRate", refreshRate);
	m_config.Get("VSync", vsync);
	m_config.Get("DisplayMode", displayMode);
	m_config.Get("Monitor", monitor);
	m_config.Get("RenderAPI", renderAPI);
	m_config.Get("HotShaderReloading", hotShaderReloading);

	VFS::Get()->SetHotShaderReloading(hotShaderReloading);

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
	                monitor,
	              renderAPI
		)
	);
	m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

	//Always added as a fallback shader
	Graphics::ShaderManager::Add(Graphics::ShaderFactory::PassthroughShader());

	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OPENGL)
	{
		m_ImGuiLayer = std::make_unique<ImGuiLayer>();
		PushOverlay(std::move(m_ImGuiLayer));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::~Application()
{
	TP_DEBUG("[Application] Shutting down TRAP Modules...");
	m_config.Set("Width", m_window->GetWidth());
	m_config.Set("Height", m_window->GetHeight());
	m_config.Set("RefreshRate", m_window->GetRefreshRate());
	m_config.Set("VSync", Graphics::API::Context::GetVSyncInterval());
	m_config.Set("DisplayMode", m_window->GetDisplayMode());
	m_config.Set("Monitor", m_window->GetMonitor());
	m_config.Set("RenderAPI", Graphics::API::Context::GetRenderAPI());
	m_config.Set("HotShaderReloading", VFS::Get()->GetHotShaderReloading());
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	m_config.Print();
#endif
	m_config.SaveToFile();
	VFS::Shutdown();
};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushLayer(std::unique_ptr<Layer> layer)
{
	layer->OnAttach();
	m_layerStack.PushLayer(std::move(layer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushOverlay(std::unique_ptr<Layer> overlay)
{
	overlay->OnAttach();
	m_layerStack.PushOverlay(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

	//TP_TRACE("[Window] ", e.ToString());

	for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
	{
		(*--it)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Run()
{
	Utils::Timer FPSTimer;
	Utils::TimeStep deltaTime(0.0f);
	unsigned int frames = 0;

	while (m_running)
	{
		m_window->Clear();

		Utils::TimeStep currentFrame(m_timer->Elapsed());
		deltaTime.Update(currentFrame.GetSeconds());

		Utils::Timer FrameTimeTimer;
		for (const auto& layer : m_layerStack)
			layer->OnUpdate(deltaTime.GetSeconds());

		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OPENGL)
		{
			ImGuiLayer::Begin();
			for (const auto& layer : m_layerStack)
				layer->OnImGuiRender();
			ImGuiLayer::End();
		}

		m_Frametime = FrameTimeTimer.ElapsedMilliseconds();
		++frames;

		Graphics::Renderer::Present(m_window.get());
		m_window->OnUpdate();

		if (FPSTimer.Elapsed() >= 1.0f) //Update FPS Counter every Second
		{
			m_FramesPerSecond = frames;
			frames = 0;
			FPSTimer.Reset();
		}

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
					if (!is_regular_file(physicalPath) || status == FileStatus::Created || status == FileStatus::Erased)
						return;

					TP_INFO("[Shader] Shader Modified Reloading...");
					Graphics::ShaderManager::Reload(virtualPath);
				});
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_running = false;

	return true;
}