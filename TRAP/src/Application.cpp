#include "TRAPPCH.h"
#include "Application.h"

#include "Core/Base.h"
#include "VFS/VFS.h"
#include "Embed.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/Renderer.h"
#include "Utils/String/String.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
#include "Input/Input.h"
#include "Utils/Utils.h"
#include "Window/Monitor.h"

TRAP::Application* TRAP::Application::s_Instance = nullptr;
std::mutex TRAP::Application::s_hotReloadingMutex;


TRAP::Application::Application(const std::string& gameName)
	: m_timer(std::make_unique<Utils::Timer>()),
	  m_FramesPerSecond(0),
	  m_FrameTime(0.0f),
	  m_fpsLimit(0),
	  m_tickRate(100),
	  m_timeScale(1.0f),
	  m_gameName(gameName),
	  m_threadPool(Utils::GetCPUInfo().LogicalCores > 1 ? (Utils::GetCPUInfo().LogicalCores - 1) :
	               std::thread::hardware_concurrency()),
	  m_newRenderAPI(Graphics::RenderAPI::NONE)
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::ApplicationPrefix, "Initializing TRAP Modules...");

	TRAP_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;
	m_mainThreadID = std::this_thread::get_id();

	TP_INFO(Log::ApplicationPrefix, "CPU: ", Utils::GetCPUInfo().LogicalCores, "x ", Utils::GetCPUInfo().Model);

	//TODO Future remove when Wayland Windows are implemented
	if (TRAP::Utils::GetLinuxWindowManager() == TRAP::Utils::LinuxWindowManager::Wayland)
	{
        TRAP::Utils::Dialogs::ShowMsgBox("Wayland unsupported!",
		                                 "Wayland is currently not supported by TRAP! Please use X11 instead",
                                         TRAP::Utils::Dialogs::Style::Error, TRAP::Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::EngineLinuxWaylandPrefix,
		            "Wayland is currently not supported by TRAP! Please use X11 instead");
		exit(-1);
	}

	VFS::Init();
	if (!m_config.LoadFromFile("Engine.cfg"))
		TP_INFO(Log::ConfigPrefix, "Using default values");

	uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t refreshRate = 60;
	m_config.Get("Width", width);
	m_config.Get("Height", height);
	m_config.Get("RefreshRate", refreshRate);
	const bool vsync = m_config.Get<bool>("VSync");
	const uint32_t fpsLimit = m_config.Get<uint32_t>("FPSLimit");
	const Window::DisplayMode displayMode = m_config.Get<Window::DisplayMode>("DisplayMode");
	const bool maximized = m_config.Get<bool>("Maximized");
	const uint32_t monitor = m_config.Get<uint32_t>("Monitor");
	const bool rawInput = m_config.Get<bool>("RawMouseInput");
	const Graphics::RenderAPI renderAPI = m_config.Get<Graphics::RenderAPI>("RenderAPI");

	if (fpsLimit > 0)
	{
		if (fpsLimit >= 25 && fpsLimit <= 500)
			m_fpsLimit = fpsLimit;
		else
			m_fpsLimit = 0;
	}

	if (renderAPI == Graphics::RenderAPI::NONE || !Graphics::RendererAPI::IsSupported(renderAPI))
		Graphics::RendererAPI::AutoSelectRenderAPI();
	else
	{
		if (renderAPI == Graphics::RenderAPI::Vulkan)
			Graphics::RendererAPI::SwitchRenderAPI(Graphics::RenderAPI::Vulkan);
		else
		{
			//All RenderAPIs are unsupported
			TRAP::Utils::Dialogs::ShowMsgBox("Incompatible Device",
				"Every RenderAPI that TRAP Engine uses is unsupported on your device!\n"
				"Does your system meet the minimum system requirements for running TRAP Engine?",
				Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);
			exit(-1);
		}
	}

	//Initialize Renderer
	Graphics::RendererAPI::Init(gameName);

	m_window = MakeScope<Window>
	(
		WindowProps
		(
			"TRAP Engine",
			width,
			height,
			refreshRate,
			vsync,
			displayMode,
			WindowProps::AdvancedProps
			{
				true,
				maximized,
				true,
				true,
				true,
				true,
				rawInput,
				Window::CursorMode::Normal
			},
			monitor
		)
	);
	m_window->SetEventCallback([this](Events::Event& e) { OnEvent(e); });

	//Update Window Title (Debug/DebWithRelInfo)
	m_window->SetTitle(m_window->GetTitle() + Graphics::Renderer::GetTitle());

	//Update Viewport
	int32_t w, h;
	INTERNAL::WindowingAPI::GetFrameBufferSize(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
		(
			m_window->GetInternalWindow()
		), w, h);
	Graphics::RenderCommand::SetViewport(0, 0, static_cast<uint32_t>(w), static_cast<uint32_t>(h));

	//Always added as a fallback shader
	Graphics::ShaderManager::LoadSource("Fallback", Embed::FallbackShader)->Use();
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create(Graphics::TextureUsage::Static));
	Graphics::TextureManager::Add(Graphics::TextureCube::Create(Graphics::TextureUsage::Static));

	//Initialize Renderer
	Graphics::Renderer::Init();

	m_layerStack = std::make_unique<LayerStack>();

	//Initialize Input for Joysticks
	Input::SetEventCallback([this](Events::Event& e) {OnEvent(e); });
	Input::Init();

	Scope<ImGuiLayer> imguiLayer = TRAP::MakeScope<ImGuiLayer>();
	m_ImGuiLayer = imguiLayer.get();
	m_layerStack->PushOverlay(std::move(imguiLayer));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::~Application()
{
	TP_PROFILE_BEGIN_SESSION("Shutdown", "TRAPProfile-Shutdown.json");

	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::ApplicationPrefix, "Shutting down TRAP Modules...");
	if(m_hotReloadingThread)
	{
		m_hotReloadingThread->join();
		m_hotReloadingThread.reset();
	}
	Input::Shutdown();
	TRAP::Graphics::RendererAPI::GetRenderer()->WaitIdle();
	m_layerStack.reset();
	if(m_window->GetWidth() > 0)
		m_config.Set("Width", m_window->GetWidth());
	if(m_window->GetHeight() > 0)
		m_config.Set("Height", m_window->GetHeight());
	m_config.Set("RefreshRate", m_window->GetRefreshRate());
	m_config.Set("VSync", m_window->GetVSync());
	m_config.Set("FPSLimit", m_fpsLimit);
	m_config.Set("DisplayMode", m_window->GetDisplayMode());
	m_config.Set("Maximized", m_window->IsMaximized());
	m_config.Set("Monitor", m_window->GetMonitor().GetID());
	m_config.Set("RawMouseInput", m_window->GetRawMouseInput());
	m_config.Set("RenderAPI", Graphics::RendererAPI::GetRenderAPI());
	const std::array<uint8_t, 16> VulkanGPUUUID = Graphics::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
		m_config.Set("VulkanGPU", Utils::UUIDToString(VulkanGPUUUID));
	else
	{
		if (m_config.Get<std::string_view>("VulkanGPU").empty())
			m_config.Set("VulkanGPU", "");
	}
	m_config.SaveToFile("Engine.cfg");
	m_window.reset();
	VFS::Shutdown();

	TP_PROFILE_END_SESSION();
};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Run()
{
	TP_PROFILE_FUNCTION();

	float lastFrameTime = 0.0f;
	auto nextFrame = std::chrono::steady_clock::now();
	Utils::Timer tickTimer;

	while (m_running)
	{
		TP_PROFILE_SCOPE("RunLoop");

		if (m_fpsLimit)
			nextFrame += std::chrono::milliseconds(1000 / m_fpsLimit);
		if (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
			nextFrame += std::chrono::milliseconds(1000 / 30); //30 FPS

		Utils::Timer FrameTimeTimer;
		const float time = m_timer->Elapsed();
		const Utils::TimeStep deltaTime{ (time - lastFrameTime) * m_timeScale };
		lastFrameTime = time;

		if (!m_minimized)
		{
			{
				TP_PROFILE_SCOPE("LayerStack OnUpdate");

				for (const auto& layer : *m_layerStack)
					layer->OnUpdate(deltaTime);
			}

			if (tickTimer.ElapsedMilliseconds() > 1000.0f / static_cast<float>(m_tickRate))
			{
				{
					TP_PROFILE_SCOPE("LayerStack OnTick");

					for (const auto& layer : *m_layerStack)
						layer->OnTick();
				}

				tickTimer.Reset();
			}
		}

		ImGuiLayer::Begin();
		{
			TP_PROFILE_SCOPE("LayerStack OnImGuiRender");

			for (const auto& layer : *m_layerStack)
				layer->OnImGuiRender();
		}
		ImGuiLayer::End();

		if (!m_minimized)
			Graphics::RenderCommand::Present(m_window);
		TRAP::Window::OnUpdate();

		Graphics::Texture2D::UpdateLoadingTextures();

		if (!m_hotReloadingThread && (VFS::GetHotShaderReloading() || VFS::GetHotTextureReloading()))
		{
			m_hotReloadingThread = TRAP::MakeScope<std::thread>(ProcessHotReloading,
			                                                    std::ref(m_hotReloadingShaderPaths),
																std::ref(m_hotReloadingTexturePaths),
																std::ref(m_running));
		}
		UpdateHotReloading();

		if (m_newRenderAPI != Graphics::RenderAPI::NONE && m_newRenderAPI != Graphics::RendererAPI::GetRenderAPI())
			ReCreate(m_newRenderAPI);

		//FPSLimiter
		if (m_fpsLimit || (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)))
			std::this_thread::sleep_until(nextFrame);

		if (!m_minimized)
		{
			m_FrameTime = FrameTimeTimer.ElapsedMilliseconds();
			m_FramesPerSecond = static_cast<uint32_t>(1000.0f / m_FrameTime);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::OnEvent(Events::Event& e)
{
	TP_PROFILE_FUNCTION();

	Events::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Events::WindowCloseEvent>([this](Events::WindowCloseEvent& event)
		{
			return OnWindowClose(event);
		});
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>([this](Events::FrameBufferResizeEvent& event)
		{
			return OnFrameBufferResize(event);
		});
	dispatcher.Dispatch<Events::KeyPressEvent>([this](Events::KeyPressEvent& event) {return OnKeyPress(event); });
	dispatcher.Dispatch<Events::WindowFocusEvent>([this](Events::WindowFocusEvent& event)
		{
			return OnWindowFocus(event);
		});
	dispatcher.Dispatch<Events::WindowLostFocusEvent>([this](Events::WindowLostFocusEvent& event)
		{
			return OnWindowLostFocus(event);
		});
	dispatcher.Dispatch<Events::WindowMinimizeEvent>([this](Events::WindowMinimizeEvent& event)
		{
			return OnWindowMinimize(event);
		});
	dispatcher.Dispatch<Events::WindowRestoreEvent>([this](Events::WindowRestoreEvent& event)
		{
			return OnWindowRestore(event);
		});

	if(!m_layerStack)
		return;

	for (auto it = m_layerStack->rbegin(); it != m_layerStack->rend(); ++it)
	{
		if (e.Handled)
			break;
		(*it)->OnEvent(e);
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

const TRAP::Utils::Config& TRAP::Application::GetConfig()
{
	return s_Instance->m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::LayerStack& TRAP::Application::GetLayerStack()
{
	return *s_Instance->m_layerStack;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImGuiLayer TRAP::Application::GetImGuiLayer()
{
	return *s_Instance->m_ImGuiLayer;
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

float TRAP::Application::GetTimeScale()
{
	return s_Instance->m_timeScale;
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

void TRAP::Application::SetTimeScale(const float timeScale)
{
	s_Instance->m_timeScale = timeScale;
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
	const Utils::TimeStep timeStep(s_Instance->m_timer->Elapsed());

	return timeStep;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ThreadPool& TRAP::Application::GetThreadPool()
{
	return s_Instance->m_threadPool;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetClipboardString(const std::string_view string)
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::SetClipboardString(string);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Application::GetClipboardString()
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

std::thread::id TRAP::Application::GetMainThreadID()
{
	return s_Instance->m_mainThreadID;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ReCreate(const Graphics::RenderAPI renderAPI) const
{
	TP_PROFILE_FUNCTION();

	for (const auto& layer : *m_layerStack)
		layer->OnDetach();

	Graphics::RendererAPI::SwitchRenderAPI(renderAPI);

	Graphics::TextureManager::Shutdown();
	Graphics::ShaderManager::Shutdown();
	Graphics::Renderer::Shutdown();
	Graphics::RendererAPI::Shutdown();

	Graphics::RendererAPI::Init(m_gameName);
	m_window->SetTitle(m_window->GetTitle());
	//Always added as a fallback shader
	Graphics::ShaderManager::LoadSource("Fallback", Embed::FallbackShader);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create(Graphics::TextureUsage::Static));
	Graphics::TextureManager::Add(Graphics::TextureCube::Create(Graphics::TextureUsage::Static));
	//Initialize Renderer
	Graphics::Renderer::Init();

	for (const auto& layer : *m_layerStack)
		layer->OnAttach();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowClose(Events::WindowCloseEvent& e)
{
	if(e.GetWindow() == m_window.get())
		m_running = false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnFrameBufferResize(Events::FrameBufferResizeEvent& e)
{
	TP_PROFILE_FUNCTION();

	Graphics::RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnKeyPress(Events::KeyPressEvent& e) const
{
	if(Window::GetActiveWindows() != 1)
		return false;

	if ((e.GetKey() == Input::Key::Enter || e.GetKey() == Input::Key::KP_Enter) &&
	    Input::IsKeyPressed(Input::Key::Left_ALT) && e.GetRepeatCount() < 1)
	{
		if (m_window->GetDisplayMode() == Window::DisplayMode::Windowed ||
			m_window->GetDisplayMode() == Window::DisplayMode::Borderless)
			m_window->SetDisplayMode(Window::DisplayMode::Fullscreen, 0, 0, 0);
		else if (m_window->GetDisplayMode() == Window::DisplayMode::Fullscreen)
			m_window->SetDisplayMode(Window::DisplayMode::Windowed, 0, 0, 0);
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowFocus(Events::WindowFocusEvent&)
{
	m_focused = true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowLostFocus(Events::WindowLostFocusEvent&)
{
	if (Window::GetActiveWindows() == 1)
		m_focused = false;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowMinimize(Events::WindowMinimizeEvent&)
{
	if (Window::GetActiveWindows() == 1)
		m_minimized = true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowRestore(Events::WindowRestoreEvent&)
{
	m_minimized = false;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ProcessHotReloading(std::vector<std::string>& shaders, std::vector<std::string>& textures,
                                            const bool& run)
{
	while (run)
	{
		//Update Shaders if needed
		if (VFS::GetHotShaderReloading() && VFS::GetShaderFileWatcher())
		{
			//Check monitoring shader folders for changes and
			//in case of changes run ShaderManager::Reload(virtualPath) (deferred into main thread)
			VFS::GetShaderFileWatcher()->Check([&](const std::filesystem::path& physicalPath,
				const std::string& virtualPath,
				const FileWatcher::FileStatus status) -> void
				{
					//Process only regular files and FileStatus::Modified
					if (!std::filesystem::is_regular_file(physicalPath))
						return;
					if (status == FileWatcher::FileStatus::Erased)
						return;

					const std::string_view suffix = Utils::String::GetSuffixStringView(virtualPath);
					if (suffix == "shader" || suffix == "spirv")
					{
						if (std::find(shaders.begin(), shaders.end(), virtualPath) == shaders.end())
						{
							{
								std::lock_guard<std::mutex> lock(s_hotReloadingMutex);
								shaders.emplace_back(virtualPath);
							}
						}
					}
				});
		}
		//Update Textures if needed
		if (VFS::GetHotTextureReloading() && VFS::GetTextureFileWatcher())
		{
			//Check monitoring texture folders for changes and
			//in case of changes run TextureManager::Reload(virtualPath) (deferred into main thread)
			VFS::GetTextureFileWatcher()->Check([&](const std::filesystem::path& physicalPath,
				const std::string& virtualPath,
				const FileWatcher::FileStatus status) -> void
			{
				//Process only regular files and FileStatus::Modified
				if (!std::filesystem::is_regular_file(physicalPath))
					return;
				if (status == FileWatcher::FileStatus::Erased)
					return;

				const std::string_view suffix = Utils::String::GetSuffixStringView(virtualPath);
				if(std::any_of(Image::SupportedImageFormatSuffixes.begin(),
				               Image::SupportedImageFormatSuffixes.end(), [suffix](const std::string& sfx)
				{
					return suffix == sfx;
				}))
				{
					if (std::find(textures.begin(), textures.end(), virtualPath) == textures.end())
					{
						{
							std::lock_guard<std::mutex> lock(s_hotReloadingMutex);
							textures.emplace_back(virtualPath);
						}
					}
				}
			});
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::UpdateHotReloading()
{
	//Shader Reloading
	for (const std::string& virtualPath : m_hotReloadingShaderPaths)
	{
		if (Graphics::ShaderManager::ExistsVirtualPath(virtualPath))
		{
			TP_INFO(Log::ShaderManagerPrefix, "Shader Modified Reloading...");
			Graphics::ShaderManager::Reload(virtualPath);
		}
	}
	m_hotReloadingShaderPaths.clear();

	//Texture Reloading
	for (const std::string& virtualPath : m_hotReloadingTexturePaths)
	{
		if (Graphics::TextureManager::ExistsVirtualPath(virtualPath))
		{
			TP_INFO(Log::ShaderManagerPrefix, "Texture Modified Reloading...");
			Graphics::TextureManager::Reload(virtualPath);
		}
	}
	m_hotReloadingTexturePaths.clear();
}