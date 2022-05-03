#include "TRAPPCH.h"
#include "Application.h"

#include "Core/Base.h"
#include "FS/FS.h"
#include "FS/FileWatcher.h"
#include "Embed.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Renderer2D.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/Renderer.h"
#include "Utils/String/String.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
#include "Events/HotReloadEvent.h"
#include "Events/FileEvent.h"
#include "Input/Input.h"
#include "Utils/Utils.h"
#include "Window/Monitor.h"
#include "Utils/Discord/DiscordGameSDK.h"

TRAP::Application* TRAP::Application::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_PLATFORM_WINDOWS) && defined(TRAP_HEADLESS_MODE)
static BOOL WINAPI SIGINTHandlerRoutine(_In_ DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_C_EVENT)
	{
		TRAP::Application::Shutdown();
		return TRUE;
	}
	else if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		TRAP::Application::Shutdown();
		Sleep(10000);
		return TRUE;
	}

	return FALSE;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Application(std::string gameName)
	: m_hotReloadingEnabled(false),
	  m_timer(),
	  m_FramesPerSecond(0),
	  m_FrameTime(0.0f),
	  m_fpsLimit(0),
	  m_tickRate(64),
	  m_timeScale(1.0f),
	  m_gameName(std::move(gameName)),
	  m_threadPool(Utils::GetCPUInfo().LogicalCores > 1 ? (Utils::GetCPUInfo().LogicalCores - 1) :
	               std::thread::hardware_concurrency()),
	  m_newRenderAPI(Graphics::RenderAPI::NONE)
{
	TP_PROFILE_FUNCTION();

	//Register SIGINT callback to capture CTRL+C
#ifdef TRAP_HEADLESS_MODE
#ifdef TRAP_PLATFORM_LINUX
	signal(SIGINT, [](int) {TRAP::Application::Shutdown(); });
#elif defined(TRAP_PLATFORM_WINDOWS)
	SetConsoleCtrlHandler(SIGINTHandlerRoutine, TRUE);
	//SetConsoleCtrlHandler(NULL, TRUE);
#endif
#endif

	TP_DEBUG(Log::ApplicationPrefix, "Initializing TRAP modules...");

	TRAP_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;
	m_mainThreadID = std::this_thread::get_id();

	//Set main log file path
	std::filesystem::path logFilePath = "logs/trap.log";
#ifndef TRAP_HEADLESS_MODE
	logFilePath = FS::GetGameDocumentsFolderPath() / "logs/trap.log";
#endif
	TRAP::FS::CreateFolder(TRAP::FS::GetFolderPath(logFilePath));
	TRAP::TRAPLog.SetFilePath(logFilePath);

	TP_INFO(Log::ApplicationPrefix, "CPU: ", Utils::GetCPUInfo().LogicalCores, "x ", Utils::GetCPUInfo().Model);

	//TODO Future remove when Wayland Windows are implemented
	if (TRAP::Utils::GetLinuxWindowManager() == TRAP::Utils::LinuxWindowManager::Wayland)
	{
        TRAP::Utils::Dialogs::ShowMsgBox("Wayland unsupported!",
		                                 "Wayland is currently not supported by TRAP™! Please use X11 instead\n"
										 "Error code: 0x0001",
                                         TRAP::Utils::Dialogs::Style::Error, TRAP::Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::EngineLinuxWaylandPrefix, "Wayland is currently not supported by TRAP!\n"
					"Please use X11 instead");
		exit(-1);
	}

	FS::Init();

	std::filesystem::path cfgPath = "engine.cfg";
#ifndef TRAP_HEADLESS_MODE
	cfgPath = FS::GetGameDocumentsFolderPath() / "engine.cfg";
#endif
	if (!m_config.LoadFromFile(cfgPath))
		TP_INFO(Log::ConfigPrefix, "Using default values");

#ifndef TRAP_HEADLESS_MODE
	uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t refreshRate = 60;
	m_config.Get("Width", width);
	m_config.Get("Height", height);
	m_config.Get("RefreshRate", refreshRate);
	const bool vsync = m_config.Get<bool>("VSync");
	const bool visible = true;
#else
	const uint32_t width = 2;
	const uint32_t height = 2;
	const uint32_t refreshRate = 60;
	const bool vsync = true;
	const Window::DisplayMode displayMode = Window::DisplayMode::Windowed;
	const uint32_t monitor = 0;
	const bool maximized = false;
	const bool rawInput = false;
	const bool visible = false;
#endif
	const uint32_t fpsLimit = m_config.Get<uint32_t>("FPSLimit");
#ifndef TRAP_HEADLESS_MODE
	const Window::DisplayMode displayMode = m_config.Get<Window::DisplayMode>("DisplayMode");
	const bool maximized = m_config.Get<bool>("Maximized");
	const uint32_t monitor = m_config.Get<uint32_t>("Monitor");
	const bool rawInput = m_config.Get<bool>("RawMouseInput");
#else
	bool enableGPU = true;
	m_config.Get("EnableGPU", enableGPU);
#endif
	Graphics::RenderAPI renderAPI = m_config.Get<Graphics::RenderAPI>("RenderAPI");

	if (fpsLimit > 0)
	{
		if (fpsLimit >= 25 && fpsLimit <= 500)
			m_fpsLimit = fpsLimit;
		else
			m_fpsLimit = 0;
	}

#ifdef TRAP_HEADLESS_MODE
	if(enableGPU)
	{
#endif
	if (renderAPI == Graphics::RenderAPI::NONE || !Graphics::RendererAPI::IsSupported(renderAPI))
		renderAPI = Graphics::RendererAPI::AutoSelectRenderAPI();
#ifdef TRAP_HEADLESS_MODE
	}
	else
		renderAPI = Graphics::RenderAPI::NONE;
#endif

	//Initialize Renderer
#ifdef TRAP_HEADLESS_MODE
	if(renderAPI != Graphics::RenderAPI::NONE)
		Graphics::RendererAPI::Init(m_gameName, renderAPI);
#else
	Graphics::RendererAPI::Init(m_gameName, renderAPI);
#endif

	//Window creation stuff
	{
		WindowProps::AdvancedProps advWinProps{};
		advWinProps.Maximized = maximized;
		advWinProps.Visible = visible;
		advWinProps.RawMouseInput = rawInput;

		WindowProps winProps{};
		winProps.Title = "TRAP™";
		winProps.Width = width;
		winProps.Height = height;
		winProps.RefreshRate = refreshRate;
		winProps.VSync = vsync;
		winProps.DisplayMode = displayMode;
		winProps.Monitor = monitor;
		winProps.Advanced = advWinProps;

		m_window = std::make_unique<Window>(winProps);
		m_window->SetEventCallback([this](Events::Event& e) { OnEvent(e); });

		//Update Window Title (Debug/DebWithRelInfo)
#ifdef TRAP_HEADLESS_MODE
		if(renderAPI != Graphics::RenderAPI::NONE)
		{
#endif
		m_window->SetTitle(m_window->GetTitle() + Graphics::Renderer::GetTitle());
#ifdef TRAP_HEADLESS_MODE
		}
#endif
	}

#ifndef TRAP_HEADLESS_MODE
	//Update Viewport
	int32_t w = 0, h = 0;
	INTERNAL::WindowingAPI::GetFrameBufferSize(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
		(
			m_window->GetInternalWindow()
		), w, h);
	Graphics::RenderCommand::SetViewport(0, 0, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
#endif

	if(renderAPI != Graphics::RenderAPI::NONE)
	{
		//Always added as a fallback shader
		Graphics::ShaderManager::LoadSource("Fallback", Embed::FallbackShader)->Use();

		//Always added as a fallback texture
		Graphics::TextureManager::Add(Graphics::Texture2D::Create());
		Graphics::TextureManager::Add(Graphics::TextureCube::Create());

		//Initialize Renderer
		Graphics::Renderer::Init();
	}

	//Initialize Input for Joysticks
	Input::SetEventCallback([this](Events::Event& e) {OnEvent(e); });
	Input::Init();

#ifndef TRAP_HEADLESS_MODE
	if(Graphics::RendererAPI::GPUSettings.SurfaceSupported &&
	   Graphics::RendererAPI::GPUSettings.PresentSupported)
	{
		Scope<ImGuiLayer> imguiLayer = TRAP::MakeScope<ImGuiLayer>();
		m_ImGuiLayer = imguiLayer.get();
		m_layerStack.PushOverlay(std::move(imguiLayer));
	}
#endif

	TRAP::Utils::Discord::Create();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::~Application()
{
	TP_PROFILE_BEGIN_SESSION("Shutdown", "TRAPProfile-Shutdown.json");
	TP_PROFILE_FUNCTION();
	TP_DEBUG(Log::ApplicationPrefix, "Shutting down TRAP modules...");

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
		TRAP::Graphics::RendererAPI::GetRenderer()->WaitIdle();

	m_layerStack.Shutdown();

	TRAP::Utils::Discord::Destroy();
	Input::Shutdown();

#ifndef TRAP_HEADLESS_MODE
	if(m_window->GetWidth() > 0)
		m_config.Set("Width", m_window->GetWidth());
	if(m_window->GetHeight() > 0)
		m_config.Set("Height", m_window->GetHeight());
	m_config.Set("RefreshRate", m_window->GetRefreshRate());
	m_config.Set("VSync", m_window->GetVSync());
	m_config.Set("DisplayMode", m_window->GetDisplayMode());
	m_config.Set("Maximized", m_window->IsMaximized());
	m_config.Set("Monitor", m_window->GetMonitor().GetID());
	m_config.Set("RawMouseInput", m_window->GetRawMouseInput());
#else
	m_config.Set("EnableGPU", Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE);
#endif

	m_config.Set("FPSLimit", m_fpsLimit);
	m_config.Set("RenderAPI", (m_newRenderAPI != Graphics::RenderAPI::NONE) ? m_newRenderAPI :
	                                                                          Graphics::RendererAPI::GetRenderAPI());
	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		const std::array<uint8_t, 16> VulkanGPUUUID = Graphics::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
		m_config.Set("VulkanGPU", Utils::UUIDToString(VulkanGPUUUID));
	}

	std::filesystem::path cfgPath = "engine.cfg";
#ifndef TRAP_HEADLESS_MODE
	cfgPath = FS::GetGameDocumentsFolderPath() / "engine.cfg";
#endif
	m_config.SaveToFile(cfgPath);
	m_window.reset();

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
		TP_TRACE(Log::ApplicationPrefix, "Shutting down Renderer");
		Graphics::Renderer::Shutdown();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down TextureManager");
		Graphics::TextureManager::Shutdown();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down ShaderManager");
		Graphics::ShaderManager::Shutdown();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down RendererAPI");
		Graphics::RendererAPI::Shutdown();
	}

	FS::Shutdown();

	s_Instance = nullptr;

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
		const float time = m_timer.Elapsed();
		const Utils::TimeStep deltaTime{ (time - lastFrameTime) * m_timeScale };
		lastFrameTime = time;

		if (!m_minimized)
		{
			{
				TP_PROFILE_SCOPE("LayerStack OnUpdate");

				for (const auto& layer : m_layerStack)
					layer->OnUpdate(deltaTime);
			}

			if (tickTimer.ElapsedMilliseconds() > 1000.0f / static_cast<float>(m_tickRate))
			{
				{
					TP_PROFILE_SCOPE("LayerStack OnTick");

					for (const auto& layer : m_layerStack)
						layer->OnTick();
				}

				tickTimer.Reset();
			}
		}

#ifndef TRAP_HEADLESS_MODE
		ImGuiLayer::Begin();
		{
			TP_PROFILE_SCOPE("LayerStack OnImGuiRender");

			for (const auto& layer : m_layerStack)
				layer->OnImGuiRender();
		}
		ImGuiLayer::End();
#endif

		if (!m_minimized)
			Graphics::RenderCommand::Present(m_window.get());

		TRAP::Window::OnUpdate();

		UpdateHotReloading();

		//FPSLimiter
		if (m_fpsLimit || (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)))
			std::this_thread::sleep_until(nextFrame);

		if (!m_minimized)
		{
			m_FrameTime = FrameTimeTimer.ElapsedMilliseconds();
			m_FramesPerSecond = static_cast<uint32_t>(1000.0f / m_FrameTime);
		}

		//Needed by Discord Game SDK
		TRAP::Utils::Discord::RunCallbacks();
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
	dispatcher.Dispatch<Events::FileChangeEvent>([this](Events::FileChangeEvent& event)
		{
			return OnFileChangeEvent(event);
		});

	for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
	{
		if (e.Handled)
			break;
		(*it)->OnEvent(e);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushLayer(std::unique_ptr<Layer> layer)
{
	m_layerStack.PushLayer(std::move(layer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushOverlay(std::unique_ptr<Layer> overlay)
{
	m_layerStack.PushOverlay(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Utils::Config& TRAP::Application::GetConfig()
{
	return s_Instance->m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::LayerStack& TRAP::Application::GetLayerStack()
{
	return s_Instance->m_layerStack;
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

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetNewRenderAPI(const Graphics::RenderAPI renderAPI)
{
	s_Instance->m_newRenderAPI = renderAPI;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Shutdown()
{
	s_Instance->m_running = false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Window* TRAP::Application::GetWindow()
{
	return s_Instance->m_window.get();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep TRAP::Application::GetTime()
{
	const Utils::TimeStep timeStep(s_Instance->m_timer.Elapsed());

	return timeStep;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ThreadPool& TRAP::Application::GetThreadPool()
{
	return s_Instance->m_threadPool;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetClipboardString(const std::string& string)
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

std::string TRAP::Application::GetGameName()
{
	return s_Instance->m_gameName;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FS::FileWatcher* TRAP::Application::GetHotReloadingFileWatcher()
{
	if(s_Instance->m_hotReloadingFileWatcher)
		return s_Instance->m_hotReloadingFileWatcher.get();

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::IsHotReloadingEnabled()
{
	return s_Instance->m_hotReloadingEnabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetHotReloading(const bool enable)
{
	s_Instance->m_hotReloadingEnabled = enable;

	if(enable && !s_Instance->m_hotReloadingFileWatcher)
	{
		s_Instance->m_hotReloadingFileWatcher = std::make_unique<FS::FileWatcher>("", false);
		s_Instance->m_hotReloadingFileWatcher->SetEventCallback([](Events::Event& e) {s_Instance->OnEvent(e); });
	}
	else if(s_Instance->m_hotReloadingFileWatcher)
		s_Instance->m_hotReloadingFileWatcher.reset();
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

#ifndef TRAP_HEADLESS_MODE
	if ((e.GetKey() == Input::Key::Enter || e.GetKey() == Input::Key::KP_Enter) &&
	    Input::IsKeyPressed(Input::Key::Left_ALT) && e.GetRepeatCount() < 1)
	{
		if (m_window->GetDisplayMode() == Window::DisplayMode::Windowed ||
			m_window->GetDisplayMode() == Window::DisplayMode::Borderless)
			m_window->SetDisplayMode(Window::DisplayMode::Fullscreen, 0, 0, 0);
		else if (m_window->GetDisplayMode() == Window::DisplayMode::Fullscreen)
			m_window->SetDisplayMode(Window::DisplayMode::Windowed, 0, 0, 0);
	}
#endif
	(void)e; //Silence unused parameter warning

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

void TRAP::Application::UpdateHotReloading()
{
	std::vector<std::filesystem::path> shaderPaths;
	std::vector<std::filesystem::path> texturePaths;

	//Hot code
	{
		std::lock_guard<std::mutex> lock(m_hotReloadingMutex);

		//Shader
		shaderPaths = m_hotReloadingShaderPaths;
		m_hotReloadingShaderPaths.clear();

		//Textures
		texturePaths = m_hotReloadingTexturePaths;
		m_hotReloadingTexturePaths.clear();
	}

	//Shader reloading
	for(const auto& p : shaderPaths)
	{
		if(Graphics::ShaderManager::ExistsPath(p))
		{
			TP_INFO(Log::HotReloadingPrefix, "Shader modified reloading...");
			Graphics::RendererAPI::GetRenderer()->WaitIdle();
			TRAP::Graphics::Shader* shader = Graphics::ShaderManager::Reload(p.string());

			//By binding the fallback shader, we can make sure that the
			//new shader will trigger a pipeline rebuild.
			TRAP::Graphics::ShaderManager::Get("Fallback")->Use();

			//Send event
			TRAP::Events::ShaderReloadEvent e(shader);
			OnEvent(e);
		}
	}

	//Texture reloading
	for(const auto& p : texturePaths)
	{
		if(Graphics::TextureManager::ExistsPath(p))
		{
			TP_INFO(Log::HotReloadingPrefix, "Texture modified reloading...");
			Graphics::RendererAPI::GetRenderer()->WaitIdle();
			Graphics::Renderer2D::ClearTextures();
			TRAP::Graphics::Texture* texture = Graphics::TextureManager::Reload(p.string());

			//Send event
			TRAP::Events::TextureReloadEvent e(texture);
			OnEvent(e);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnFileChangeEvent(const Events::FileChangeEvent& event)
{
	if(event.GetStatus() != FS::FileStatus::Modified)
		return true; //Only handle modified files

	std::string fEnding = Utils::String::ToLower(FS::GetFileEnding(event.GetPath()));

	//Is it a texture?
	bool texture = false;
	for(const auto& fmt : Image::SupportedImageFormatSuffixes)
	{
		if(fEnding == fmt)
		{
			texture = true;
			break;
		}
	}

	bool shader = false;
	if(!texture) //Or is it a shader?
	{
		for(const auto& fmt : Graphics::Shader::SupportedShaderFormatSuffixes)
		{
			if(fEnding == fmt)
			{
				shader = true;
				break;
			}
		}
	}

	if(!texture && !shader)
		return true; //Not a texture or shader

	if(texture)
	{
		std::lock_guard<std::mutex> lock(m_hotReloadingMutex); //Hot code

		//Don't add duplicates!
		for(const auto& p : m_hotReloadingTexturePaths)
		{
			if(FS::IsPathEquivalent(p, event.GetPath()))
				return true;
		}

		m_hotReloadingTexturePaths.push_back(event.GetPath());
	}
	else if(shader)
	{
		std::lock_guard<std::mutex> lock(m_hotReloadingMutex); //Hot code

		//Don't add duplicates!
		for(const auto& p : m_hotReloadingShaderPaths)
		{
			if(FS::IsPathEquivalent(p, event.GetPath()))
				return true;
		}

		m_hotReloadingShaderPaths.push_back(event.GetPath());
	}

	return true; //Don't send this event to other listeners
}