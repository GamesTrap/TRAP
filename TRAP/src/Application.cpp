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

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
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

	TP_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

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
	DisplayMode displayMode = DisplayMode::Windowed;
	uint32_t monitor;
	Graphics::API::RenderAPI renderAPI = Graphics::API::RenderAPI::NONE;
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
	m_config.Get("HotShaderReloading", hotShaderReloading);
	m_config.Get("HotTextureReloading", hotTextureReloading);

	if(fpsLimit > 0)
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
	m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Passthrough", Embed::PassthroughVS, Embed::PassthroughFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());
	Graphics::TextureManager::Add(Graphics::TextureCube::Create());

	m_ImGuiLayer = std::make_unique<ImGuiLayer>();
	PushOverlay(std::move(m_ImGuiLayer));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::~Application()
{
	TP_DEBUG("[Application] Shutting down TRAP Modules...");
	Graphics::TextureManager::Shutdown();
	Graphics::ShaderManager::Shutdown();
	m_config.Set("Width", m_window->GetWidth());
	m_config.Set("Height", m_window->GetHeight());
	m_config.Set("RefreshRate", m_window->GetRefreshRate());
	m_config.Set("VSync", m_window->GetVSyncInterval());
	m_config.Set("FPSLimit", m_fpsLimit);
	m_config.Set("DisplayMode", m_window->GetDisplayMode());
	m_config.Set("Monitor", m_window->GetMonitor());
	m_config.Set("RenderAPI", Graphics::API::Context::GetRenderAPI());
	m_config.Set("HotShaderReloading", VFS::Get()->GetHotShaderReloading());
	m_config.Set("HotTextureReloading", VFS::Get()->GetHotTextureReloading());
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	m_config.Print();
#endif
	m_config.SaveToFile();
	VFS::Shutdown();
};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushLayer(std::unique_ptr<Layer> layer)
{
	//layer->OnAttach();
	m_layerStack.PushLayer(std::move(layer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushOverlay(std::unique_ptr<Layer> overlay)
{
	//overlay->OnAttach();
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
	Utils::TimeStep deltaTime(0.0f);
	std::deque<Utils::Timer> framesPerSecond;
	auto nextFrame = std::chrono::steady_clock::now();
	Utils::Timer tickTimer;

	while (m_running)
	{
		if (m_fpsLimit)
			nextFrame += std::chrono::milliseconds(1000 / m_fpsLimit);
			
		m_drawCalls = 0;

		Utils::Timer FrameTimeTimer;
		deltaTime.Update(m_timer->Elapsed());

		for (const auto& layer : m_layerStack)
			layer->OnUpdate(deltaTime);

		if(tickTimer.ElapsedMilliseconds() > 1000.0f / static_cast<float>(m_tickRate))
		{
			for (const auto& layer : m_layerStack)			
				layer->OnTick();

			tickTimer.Reset();
		}

		Window::Use(m_window.get());
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
		{
			ImGuiLayer::Begin();
			for (const auto& layer : m_layerStack)
				layer->OnImGuiRender();
			ImGuiLayer::End();
		}

		Graphics::RenderCommand::Present(m_window.get());
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
					if (!is_regular_file(physicalPath) || status == FileStatus::Created || status == FileStatus::Erased)
						return;

					TP_INFO("[ShaderManager] Shader Modified Reloading...");
					Graphics::ShaderManager::Reload(virtualPath);
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
					if (!is_regular_file(physicalPath) || status == FileStatus::Created || status == FileStatus::Erased)
						return;

					TP_INFO("[TextureManager] Texture Modified Reloading...");
					Graphics::TextureManager::Reload(virtualPath);
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

bool TRAP::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_running = false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ReCreateWindow(const Graphics::API::RenderAPI renderAPI)
{
	for (const auto& layer : m_layerStack)
		layer->OnDetach();
	Graphics::API::Context::SetRenderAPI(renderAPI);

	WindowProps props{ std::string(m_window->GetTitle()), m_window->GetWidth(), m_window->GetHeight(), m_window->GetRefreshRate(), m_window->GetVSyncInterval(), m_window->GetDisplayMode(), m_window->GetMonitor() };
	m_window.reset();
	m_window = std::make_unique<Window>(props);
	m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Passthrough", Embed::PassthroughVS, Embed::PassthroughFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());

	for (const auto& layer : m_layerStack)
		layer->OnAttach();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ReCreate(const Graphics::API::RenderAPI renderAPI)
{
	for (const auto& layer : m_layerStack)
		layer->OnDetach();
	Graphics::API::Context::SetRenderAPI(renderAPI);

	Graphics::TextureManager::Shutdown();
	Graphics::ShaderManager::Shutdown();
	Graphics::API::RendererAPI::Shutdown();
	Graphics::API::Context::Shutdown();

	Graphics::API::Context::Create(m_window.get());
	Graphics::API::Context::SetVSyncInterval(m_window->GetVSyncInterval());
	Graphics::API::RendererAPI::Init();
	m_window->SetTitle(std::string(m_window->GetTitle()));

	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Passthrough", Embed::PassthroughVS, Embed::PassthroughFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());

	for (const auto& layer : m_layerStack)
		layer->OnAttach();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep TRAP::Application::GetTime() const
{
	Utils::TimeStep timeStep(0.0f);
	timeStep.Update(m_timer->Elapsed());

	return timeStep;
}