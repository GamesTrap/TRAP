#include "TRAPPCH.h"
#include "Application.h"

#include "Core/Base.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileWatcher.h"
#include "Embed.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Renderer2D.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Textures/SpriteManager.h"
#include "Graphics/Textures/Texture.h"
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
#include "Utils/Steam/SteamworksSDK.h"
#include "Utils/Memory.h"

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

#if defined(ENABLE_SINGLE_PROCESS_ONLY) && defined(TRAP_PLATFORM_LINUX)
static bool CheckSingleProcessLinux()
{
	static int32_t socketFD = -1;
	static int32_t rc = 1;
	static constexpr uint16_t port = 49420; //Just a free (hopefully) random port

	if(socketFD == -1 || rc)
	{
		socketFD = -1;
		rc = 1;

		socketFD = socket(AF_INET, SOCK_DGRAM, 0);
		if(socketFD < 0)
			return false;

		sockaddr_in name;
		name.sin_family = AF_INET;
		name.sin_port = port;
		name.sin_addr.s_addr = INADDR_ANY;

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(name.sin_port);

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(name.sin_addr.s_addr);

		sockaddr convertedSock = TRAP::Utils::BitCast<sockaddr_in, sockaddr>(name); //Prevent usage of reinterpret_cast
		rc = bind(socketFD, &convertedSock, sizeof(name));
	}

	return (socketFD != -1 && rc == 0);
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#if defined(ENABLE_SINGLE_PROCESS_ONLY) && defined(TRAP_PLATFORM_WINDOWS)
static bool CheckSingleProcessWindows()
{
	const HANDLE hMutex = CreateMutex(0, 0, L"TRAP-Engine");
	if(!hMutex) //Error creating mutex
		return false;
	if(hMutex && GetLastError() == ERROR_ALREADY_EXISTS)
		return false;

	return true;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Application(std::string gameName, const uint32_t appID)
	: m_hotReloadingEnabled(false),
	  m_timer(),
	  m_FramesPerSecond(0),
	  m_FrameTime(0.0f),
	  m_fpsLimit(0),
	  m_tickRate(64),
	  m_timeScale(1.0f),
	  m_gameName(std::move(gameName)),
	  m_globalCounter(0),
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
#endif
#endif

	//Single process mode
#ifdef ENABLE_SINGLE_PROCESS_ONLY
	bool singleProcess = true;
	#ifdef TRAP_PLATFORM_LINUX
		singleProcess = CheckSingleProcessLinux();
	#elif defined(TRAP_PLATFORM_WINDOWS)
		singleProcess = CheckSingleProcessWindows();
	#endif

	if(!singleProcess)
	{
		Utils::Dialogs::ShowMsgBox("TRAP™ already running", "A TRAP™ Application is already running!\n"
		                           "Error code: 0x0012", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		std::exit(0x0012);
	}
#endif

	TP_DEBUG(Log::ApplicationPrefix, "Initializing TRAP modules...");

	TRAP_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;
	m_mainThreadID = std::this_thread::get_id();

	TP_INFO(Log::ApplicationPrefix, "CPU: ", Utils::GetCPUInfo().LogicalCores, "x ", Utils::GetCPUInfo().Model);

	FileSystem::Init();

	//Set main log file path
	const auto logFolder = FileSystem::GetGameLogFolderPath();
	if(logFolder)
		TRAP::TRAPLog.SetFilePath(*logFolder / "trap.log");
	else //Fallback to current directory
		TRAP::TRAPLog.SetFilePath("trap.log");

	TRAP::Utils::Steam::Initalize(appID);

	std::filesystem::path cfgPath = "engine.cfg";
#ifndef TRAP_HEADLESS_MODE
	const auto docsFolder = FileSystem::GetGameDocumentsFolderPath();
	if(docsFolder)
		cfgPath = *docsFolder / "engine.cfg";
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
	const Graphics::LatencyMode latencyMode = m_config.Get<Graphics::LatencyMode>("NVIDIAReflex");
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
	{
		//Get Anti aliasing from engine.cfg
		const Graphics::AntiAliasing antiAliasing = m_config.Get<TRAP::Graphics::AntiAliasing>("AntiAliasing");
		const Graphics::SampleCount sampleCount = m_config.Get<TRAP::Graphics::SampleCount>("AntiAliasingQuality");

		Graphics::RendererAPI::Init(m_gameName, renderAPI, antiAliasing, sampleCount);
	}
#else
	//Get Anti aliasing from engine.cfg
	const Graphics::AntiAliasing antiAliasing = m_config.Get<TRAP::Graphics::AntiAliasing>("AntiAliasing");
	const Graphics::SampleCount sampleCount = m_config.Get<TRAP::Graphics::SampleCount>("AntiAliasingQuality");

	Graphics::RendererAPI::Init(m_gameName, renderAPI, antiAliasing, sampleCount);
#endif

	//Window creation stuff
#ifdef TRAP_PLATFORM_LINUX
	if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
	{
#endif
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

		//Update Window Title (Debug/RelWithDebInfo)
		if(renderAPI != Graphics::RenderAPI::NONE)
			m_window->SetTitle(m_window->GetTitle() + Graphics::Renderer::GetTitle());
#ifdef TRAP_PLATFORM_LINUX
	}
#ifdef TRAP_HEADLESS_MODE
	else //Headless without X11 or Wayland
	{
		if(TRAP::Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
			TRAP::Graphics::RendererAPI::GetRenderer()->InitPerWindowData(nullptr);
	}
#endif
#endif

	SetFPSLimit(fpsLimit);

#ifndef TRAP_HEADLESS_MODE
	//NVIDIA Reflex
	Graphics::RenderCommand::SetLatencyMode(latencyMode);

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
		Graphics::ShaderManager::LoadSource("FallbackGraphics", std::string(Embed::FallbackGraphicsShader))->Use();
		Graphics::ShaderManager::LoadSource("FallbackCompute", std::string(Embed::FallbackComputeShader))->Use();

		//Always added as a fallback texture
		Graphics::TextureManager::Add(Graphics::Texture::CreateFallback2D());
		Graphics::TextureManager::Add(Graphics::Texture::CreateFallbackCube());

		//Initialize Renderer
		Graphics::Renderer::Init();
	}

	//Initialize Input for Joysticks
#ifdef TRAP_PLATFORM_LINUX
	if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
	{
#endif
		Input::SetEventCallback([this](Events::Event& e) {OnEvent(e); });
		Input::Init();
#ifdef TRAP_PLATFORM_LINUX
	}
#endif

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

	TRAP::Utils::Steam::Shutdown();
	TRAP::Utils::Discord::Destroy();
	if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
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

	if (Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
		Graphics::AntiAliasing antiAliasing = Graphics::AntiAliasing::Off;
		Graphics::SampleCount sampleCount = Graphics::SampleCount::One;
		Graphics::RenderCommand::GetAntiAliasing(antiAliasing, sampleCount);
		m_config.Set("AntiAliasing", antiAliasing);
		m_config.Set("AntiAliasingQuality", sampleCount);

		//NVIDIA Reflex
		m_config.Set("NVIDIAReflex", Graphics::RenderCommand::GetLatencyMode());
	}

	std::filesystem::path cfgPath = "engine.cfg";
#ifndef TRAP_HEADLESS_MODE
	const auto docsFolder = FileSystem::GetGameDocumentsFolderPath();
	if(docsFolder)
		cfgPath = *docsFolder / "engine.cfg";
#endif
	m_config.SaveToFile(cfgPath);
	m_window.reset();

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
		TP_TRACE(Log::ApplicationPrefix, "Shutting down Renderer");
		Graphics::Renderer::Shutdown();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down SpriteManager");
		Graphics::SpriteManager::Clean();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down TextureManager");
		Graphics::TextureManager::Clean();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down ShaderManager");
		Graphics::ShaderManager::Clean();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down RendererAPI");
		Graphics::RendererAPI::Shutdown();
	}

	FileSystem::Shutdown();

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

		const Utils::Timer FrameTimeTimer;
		const float time = m_timer.Elapsed();
		const Utils::TimeStep deltaTime{ (time - lastFrameTime) * m_timeScale };
		lastFrameTime = time;

		//FPSLimiter
		if(Graphics::RendererAPI::GPUSettings.ReflexSupported)
			Graphics::RendererAPI::GetRenderer()->ReflexSleep();
		else if (m_fpsLimit || (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)))
			std::this_thread::sleep_until(nextFrame);

#ifdef NVIDIA_REFLEX_AVAILABLE
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, VK_SIMULATION_START);
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, VK_INPUT_SAMPLE);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

#ifdef TRAP_PLATFORM_LINUX
		if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
			TRAP::Window::OnUpdate();
#else
		TRAP::Window::OnUpdate();
#endif

#ifdef NVIDIA_REFLEX_AVAILABLE
		if(Input::IsMouseButtonPressed(Input::MouseButton::Left))
			Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, VK_TRIGGER_FLASH);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

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

#ifndef TRAP_HEADLESS_MODE
			ImGuiLayer::Begin();
			{
				TP_PROFILE_SCOPE("LayerStack OnImGuiRender");

				for (const auto& layer : m_layerStack)
					layer->OnImGuiRender();
			}
			ImGuiLayer::End();
#endif
			if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
				Graphics::RenderCommand::Present(m_window.get());
		}

		UpdateHotReloading();

		if (!m_minimized)
		{
			m_FrameTime = FrameTimeTimer.ElapsedMilliseconds();
			m_FramesPerSecond = static_cast<uint32_t>(1000.0f / m_FrameTime);
		}

		//Needed by Discord Game SDK
		TRAP::Utils::Discord::RunCallbacks();
		//Needed by Steamworks SDK
		TRAP::Utils::Steam::RunCallbacks();

#ifdef NVIDIA_REFLEX_AVAILABLE
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, VK_SIMULATION_END);
#endif /*NVIDIA_REFLEX_AVAILABLE*/
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
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>([](Events::FrameBufferResizeEvent& event)
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

TRAP::ImGuiLayer& TRAP::Application::GetImGuiLayer()
{
	return *(s_Instance->m_ImGuiLayer);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetFPS()
{
	return s_Instance->m_FramesPerSecond;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetFPSLimit(const uint32_t fps)
{
	if(fps == 0)
		s_Instance->m_fpsLimit = 0;
	else
		s_Instance->m_fpsLimit = TRAP::Math::Clamp(fps, 25u, 500u);

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::NONE &&
	   TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported)
	{
		Graphics::RendererAPI::GetRenderer()->SetReflexFPSLimit(s_Instance->m_fpsLimit);
	}
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetFPSLimit()
{
	return s_Instance->m_fpsLimit;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Application::GetCPUFrameTime()
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

uint64_t TRAP::Application::GetGlobalCounter()
{
	return s_Instance->m_globalCounter;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileSystem::FileWatcher* TRAP::Application::GetHotReloadingFileWatcher()
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
		s_Instance->m_hotReloadingFileWatcher = std::make_unique<FileSystem::FileWatcher>("", false);
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

	Graphics::RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight(), e.GetWindow());
	Graphics::RendererAPI::GetRenderer()->ResizeSwapChain(e.GetWindow());

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

		return true;
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
			Ref<TRAP::Graphics::Shader> shader = Graphics::ShaderManager::Reload(p.string());

			//By binding the fallback shader, we can make sure that the
			//new shader will trigger a pipeline rebuild.
			if(static_cast<bool>(shader->GetShaderStages() & Graphics::RendererAPI::ShaderStage::Compute))
				TRAP::Graphics::ShaderManager::Get("FallbackCompute")->Use();
			else
				TRAP::Graphics::ShaderManager::Get("FallbackGraphics")->Use();

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
			TRAP::Ref<TRAP::Graphics::Texture> texture = Graphics::TextureManager::Reload(p.string());

			//Send event
			TRAP::Events::TextureReloadEvent e(texture);
			OnEvent(e);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnFileChangeEvent(const Events::FileChangeEvent& event)
{
	if(event.GetStatus() != FileSystem::FileStatus::Modified)
		return false; //Only handle modified files

	const auto fileEnding = FileSystem::GetFileEnding(event.GetPath());
	if(!fileEnding) //Ignore files without an extension
		return false;

	const std::string fEnding = Utils::String::ToLower(*fileEnding);

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
		return false; //Not a texture or shader

	if(texture)
	{
		std::lock_guard<std::mutex> lock(m_hotReloadingMutex); //Hot code

		//Don't add duplicates!
		for(const auto& p : m_hotReloadingTexturePaths)
		{
			if(FileSystem::IsPathEquivalent(p, event.GetPath()))
				return false;
		}

		m_hotReloadingTexturePaths.push_back(event.GetPath());
	}
	else if(shader)
	{
		std::lock_guard<std::mutex> lock(m_hotReloadingMutex); //Hot code

		//Don't add duplicates!
		for(const auto& p : m_hotReloadingShaderPaths)
		{
			if(FileSystem::IsPathEquivalent(p, event.GetPath()))
				return false;
		}

		m_hotReloadingShaderPaths.push_back(event.GetPath());
	}

	return false;
}